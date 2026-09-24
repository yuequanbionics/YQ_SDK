/**
 * @file rs485_crc16.hpp
 * @brief RS485 通信 CRC16 校验模块头文件
 *
 * 提供两种 CRC16 算法:
 *   - Modbus 标准: 多项式 0x8005, 初始值 0xFFFF
 *   - CRC-CCITT  : 多项式 0x1021, 初始值 0x1D0F
 */

#ifndef RS485_CRC16_HPP
#define RS485_CRC16_HPP

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CRC16 算法类型枚举
 */
typedef enum {
    CRC16_MODBUS = 0, /**< Modbus 标准 CRC16 (多项式 0x8005, 初始值 0xFFFF) */
    CRC16_CCITT = 1   /**< CRC-CCITT 标准       (多项式 0x1021, 初始值 0x1D0F) */
} rs485_crc16_type_t;

/**
 * @brief 计算 CRC16 校验码(查表法)
 * @param data   输入数据缓冲区
 * @param length 数据长度
 * @param type   CRC 算法类型
 * @return uint16_t CRC16 校验值
 */
uint16_t rs485_crc16_calculate(const uint8_t* data, std::size_t length, rs485_crc16_type_t type);

/**
 * @brief 验证 CRC16 校验码
 * @param data   包含 CRC 的数据缓冲区
 * @param length 数据长度(包含 2 字节 CRC)
 * @param type   CRC 算法类型
 * @return int 校验通过返回 1, 失败返回 0
 */
int rs485_crc16_verify(const uint8_t* data, std::size_t length, rs485_crc16_type_t type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RS485_CRC16_HPP */
