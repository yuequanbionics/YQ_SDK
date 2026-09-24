/**
 * @file rs485_driver.hpp
 * @brief RS485 串口驱动模块头文件
 */

#ifndef RS485_DRIVER_HPP
#define RS485_DRIVER_HPP

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

/* 接收缓冲区大小 */
#define RS485_RX_BUFFER_SIZE 512

/**
 * @brief 串口配置参数结构体
 */
typedef struct {
    const char* device;  /**< 串口设备路径 */
    uint32_t baudrate;   /**< 波特率 */
    uint8_t databits;    /**< 数据位,7或8 */
    uint8_t stopbits;    /**< 停止位,1或2 */
    uint8_t parity;      /**< 校验位: 0-无, 1-奇, 2-偶 */
    uint8_t flowctrl;    /**< 流控制: 0-无, 1-硬件, 2-软件 */
    uint32_t timeout_ms; /**< 读取超时时间(毫秒) */
} serial_config_t;

/**
 * @brief RS485设备句柄
 */
typedef struct rs485_device rs485_device_t;

/**
 * @brief RS485驱动操作接口
 */
typedef struct rs485_driver_ops {
    /**
     * @brief 打开串口
     * @param config 串口配置
     * @return rs485_device_t* 成功返回设备句柄,失败返回NULL
     */
    rs485_device_t* (*open)(const serial_config_t* config);

    /**
     * @brief 关闭串口
     * @param dev 设备句柄
     * @return int 成功返回0,失败返回-1
     */
    int (*close)(rs485_device_t* dev);

    /**
     * @brief 发送数据
     * @param dev 设备句柄
     * @param data 数据缓冲区
     * @param len 数据长度
     * @return int 成功返回发送的字节数,失败返回-1
     */
    int (*send)(rs485_device_t* dev, const uint8_t* data, uint32_t len);

    /**
     * @brief 接收数据
     * @param dev 设备句柄
     * @param data 数据缓冲区
     * @param len 最大接收长度
     * @param timeout_ms 超时时间(毫秒),0表示阻塞
     * @return int 成功返回接收的字节数,失败返回-1,超时返回0
     */
    int (*recv)(rs485_device_t* dev, uint8_t* data, uint32_t len, uint32_t timeout_ms);

    /**
     * @brief 刷新串口缓冲区
     * @param dev 设备句柄
     * @return int 成功返回0,失败返回-1
     */
    int (*flush)(rs485_device_t* dev);
} rs485_driver_ops_t;

/**
 * @brief 获取RS485驱动操作接口实例
 * @return const rs485_driver_ops_t* 驱动操作接口指针
 */
const rs485_driver_ops_t* rs485_driver_get_ops(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RS485_DRIVER_HPP */
