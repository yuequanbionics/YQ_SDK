from stlink_flasher import batch_flash
# STM32H74x_H75x
# STM32G43x_G44x

batch_flash("STM32G43x_G44x", "../../../config/OTA_BAG/mcu_bl_HEX/motor-drive-proj.hex")

# result = batch_flash(
#     chip_model="STM32G43x_G44x",   # 芯片型号
#     hex_file="../../../config/OTA_BAG/mcu_bl_HEX/motor-drive-proj.hex"          # HEX 路径
# )

# print(result)