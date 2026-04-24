import subprocess
import threading
import os
import json

# -----------------------------------------------------------------------------
# 加载 SN → 编号 映射
# -----------------------------------------------------------------------------
def load_sn_map():
    try:
        with open("../../../config/OTA_BAG/mcu_bl_HEX/sn_map.json", "r", encoding="utf-8") as f:
            return json.load(f)
    except:
        return {}

sn_map = load_sn_map()

# -----------------------------------------------------------------------------
# 1. 运行 shell 命令，获取输出
# -----------------------------------------------------------------------------
def run_cmd(cmd,timeout=10):
    try:
        result = subprocess.run(
            cmd, shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            encoding="utf-8",
            timeout=timeout
        )
        return result.stdout + result.stderr
    except:
        return ""

# -----------------------------------------------------------------------------
# 2. 扫描所有 ST-LINK：返回 [ {"serial": "xxx", "chip": "xxx"}, ... ]
# -----------------------------------------------------------------------------
def scan_all_stlinks():
    output = run_cmd("st-info --probe 2>/dev/null")
    devices = []
    current_serial = None
    current_chip = None

    for line in output.splitlines():
        line = line.strip()

        if line.startswith("serial:"):
            current_serial = line.split(None, 1)[1].strip()

        if line.startswith("dev-type:"):
            current_chip = line.split(None, 1)[1].strip()

            if current_serial and current_chip:
                devices.append({
                    "serial": current_serial,
                    "chip": current_chip
                })
                current_serial = None
                current_chip = None

    return devices

# -----------------------------------------------------------------------------
# 3. 烧录单个设备
# -----------------------------------------------------------------------------
def flash_one_device(serial, hex_path, result_list, index):
    cmd = f'st-flash --serial {serial} --format=ihex write "{hex_path}"'
    output = run_cmd(cmd , timeout=10)
    success = "Flash written and verified" in output

    result_list[index] = {
        "serial": serial,
        "success": success,
        "output": output
    }

# -----------------------------------------------------------------------------
# ✨ 核心接口：批量烧录（给外部调用）
# -----------------------------------------------------------------------------
def batch_flash(chip_model: str, hex_file: str):
    """
    批量烧录指定芯片型号的所有 ST-LINK 设备
    :param chip_model: 要烧录的芯片型号，例如 STM32G43x_G44x
    :param hex_file: hex 文件路径
    :return: 所有设备烧录结果
    """
    if not os.path.exists(hex_file):
        return {"error": "HEX 文件不存在"}

    # 扫描所有设备
    all_devices = scan_all_stlinks()
    target_devices = [d for d in all_devices if d["chip"] == chip_model]

    if not target_devices:
        found_chips = {d["chip"] for d in all_devices}
        return {"error": f"未找到芯片 {chip_model}，当前设备芯片：{', '.join(found_chips)}"}

    # 并行烧录
    result_list = [None] * len(target_devices)
    threads = []

    for i, dev in enumerate(target_devices):
        t = threading.Thread(
            target=flash_one_device,
            args=(dev["serial"], hex_file, result_list, i)
        )
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    final_results = []
    for r in result_list:
        sn = r["serial"]
        final_results.append({
            "编号": sn_map.get(sn, "未知"),
            "success": r["success"],
            "serial" : r["serial"]
        })

    output = {
        "芯片型号": chip_model,
        "HEX文件": hex_file,
        "总设备数": len(target_devices),
        "烧录结果": final_results
    }

    # 美化打印
    print("=" * 50)
    print("✅ 批量烧录完成")
    print(f"📌 芯片型号: {chip_model}")
    print(f"📄 HEX文件: {hex_file}")
    print(f"🔢 总设备数: {len(target_devices)}")
    print("=" * 50)
    print("烧录结果：")
    for res in final_results:
        status = "✅ 成功" if res["success"] else "❌ 失败"
        # print(f"  编号[{res['serial']}] ")
        print(f"  编号[{res['编号']}] | {status}")
    print("=" * 50)
    return output

# # -----------------------------------------------------------------------------
# # 测试示例（直接运行此文件可测试）
# # -----------------------------------------------------------------------------
# if __name__ == "__main__":
#     result = batch_flash(
#         chip_model="STM32G43x_G44x",
#         hex_file="/home/peng/sdk_2/config/OTA_BAG/X_Hand_OTA/motor-drive-proj.hex"
#     )
#     print(result)