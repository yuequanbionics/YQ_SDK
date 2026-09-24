import subprocess
import platform
import os
import fcntl

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import Shutdown

class CharPosInfo:
    def __init__(self):
        self.start: int = 0
        self.len: int = 0
        self.is_letter: bool = False
        self.original_char: str = ''

def decodeByTemplate(template_str: str, num: int) -> str:
    pos_vec = []
    start_buff = 1
    total_len = 0
    for c in template_str:
        info = CharPosInfo()
        info.original_char = c
        if c.isalpha():
            info.len = 2
            info.is_letter = True
            total_len += 2
        elif c.isdigit():
            info.len = 1
            info.is_letter = False
            total_len += 1
        else:
            raise ValueError(f"模板包含非法字符：{c}")
        info.start = start_buff
        start_buff += info.len
        pos_vec.append(info)
    num_abs = abs(num)
    num_str = str(num_abs)
    # 补前导0 / 截断
    if len(num_str) < total_len:
        num_str = "0" * (total_len - len(num_str)) + num_str
    if len(num_str) > total_len:
        num_str = num_str[:total_len]
    result = ""
    num_idx = 0
    for info in pos_vec:
        if info.is_letter:
            if num_idx + 2 > len(num_str):
                raise RuntimeError("数字字符串长度不足，无法还原字母位")
            letter_num_str = num_str[num_idx:num_idx+2]
            letter_num = int(letter_num_str)
            num_idx += 2
            if 1 <= letter_num <= 26:
                result += chr(ord('a') + letter_num -1)
            elif 29 <= letter_num <=54:
                result += chr(ord('A') + letter_num -29)
            else:
                print(f"无效的字母编码数字：{letter_num_str}（数值：{letter_num}）")
        else:
            if num_idx >= len(num_str):
                raise RuntimeError("数字字符串长度不足，无法还原数字位")
            result += num_str[num_idx]
            num_idx +=1
    return result

def run_get_sn(sdk_dir, timeout_arg="100"):
    # 判断架构
    arch = platform.machine()
    if arch in ("x86_64", "amd64"):
        sub_dir = "x86"
    elif arch == "aarch64":
        sub_dir = "arm64"
    else:
        raise RuntimeError(f"不支持的架构: {arch}")

    # 候选路径列表，优先顺序
    candidates = [
        sdk_dir + f"/bin/Tool/{sub_dir}/Get_SN",
        sdk_dir + f"/bin/Tool/ubuntu20.04/{sub_dir}/Get_SN"
    ]

    ret = None
    for exe_path in candidates:
        if not os.path.isfile(exe_path):
            continue
        cmd = [exe_path, timeout_arg]
        ret = subprocess.run(cmd, capture_output=True, text=True)
        if ret.returncode == 0:
            break
    else:
        # 全部候选都执行失败
        raise RuntimeError("所有Get_SN可执行文件执行失败")
    
    output = ret.stdout
    print("==== Get_SN原始输出 ====")
    print(output)
    sn_list = []
    for line in output.splitlines():
        line = line.strip()
        if line.isdigit():
            sn_val = int(line)
            sn_list.append(sn_val)
    return sn_list


def DEC(sdk_dir):
    SN_MOD = ["1X1000000000"]
    SN_MSK = ["111000000000"]

    lock_file_path = "/tmp/dec_sn.lock"
    lock_fd = open(lock_file_path, 'w')
    try:
        # LOCK_EX 排他锁，去掉LOCK_NB → 阻塞等待，直到拿到锁
        fcntl.flock(lock_fd, fcntl.LOCK_EX)
    except BlockingIOError:
        print("DEC函数已经在运行，等待中...")
        lock_fd.close()
        return []
    
    sn_collection = run_get_sn(sdk_dir)
    decoded_str_s = []

    for sn in sn_collection:
        # 遍历每组模板+掩码
        for mod, msk in zip(SN_MOD, SN_MSK):
            # 用当前模板解码
            decoded_str = decodeByTemplate(mod, sn)
            # 掩码校验：只检查mask='1'的位置字符是否一致
            match_ok = True
            for d_char, m_char in zip(decoded_str, msk):
                if m_char == '1':
                    # mask为1的位置，要求decoded_str和模板mod对应位置字符相等
                    if d_char != mod[decoded_str.index(d_char)]:
                        match_ok = False
                        break
            # 校验通过才加入结果
            if match_ok:
                decoded_str_s.append(decoded_str)
                print(f"SN原始数值: {sn}")
                print(f"模板:{mod} 掩码:{msk} 校验通过")
                print(f"Main_SN_DATA_STR# {decoded_str}\n")
    
    print("\n==== 解析结果 ====")
    for sn in decoded_str_s:
        print(f"Main_SN_DATA_STR# {sn}\n")

    return decoded_str_s


if __name__ == "__main__":
    DEC("/home/toko/SP/sdk_2")