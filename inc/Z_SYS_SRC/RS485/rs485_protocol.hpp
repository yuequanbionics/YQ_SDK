/**
 * @file rs485_protocol.hpp
 * @brief RS485 通信协议层头文件
 *
 * 协议层: 只负责数据打包与解析, 不涉及硬件操作
 *
 * 协议格式:
 * +--------+----------+------+-------------+-------+-----+------+-----+
 * | Header | DeviceID | CMD  | DataLength  | Data  | CRC | CRC  | Tail|
 * +--------+----------+------+-------------+-------+-----+------+-----+
 * | 0xAA   | 8字节    | 1字节 | 1字节       | N字节 | 高8 | 低8  | 0x55|
 * +--------+----------+------+-------------+-------+-----+------+-----+
 *
 * 设备 ID 编码规则:
 *   - 数字字符 '0'-'9' : 直接保留
 *   - 大写字母 'A'-'Z' : 映射为 29-54, 固定 2 位
 *   - 小写字母 'a'-'z' : 映射为 1-26,  固定 2 位
 *   拼接后的十进制数字串转换为 uint64, 按小端存储到 8 字节数组
 *
 * CMD 命令码定义:
 *   - 0x01 : 读取设备状态
 *   - 0x02 : 读取设备版本
 *   - 0x03 : 读取电机数据 (6 个电机, 数据格式: id + motor_status_t)
 *   - 0x04 : 写入电机数据 (6 个电机, 数据格式: id + motor_control_t)
 *   - 0x05 : 预留命令(暂不对外开放, 保留扩展)
 *
 * CRC 校验: 支持两种模式(实际应用中只能选择一种)
 *   - Modbus 标准 : 多项式 0x8005, 初始值 0xFFFF
 *   - CRC-CCITT   : 多项式 0x1021, 初始值 0x1D0F
 */

#ifndef RS485_PROTOCOL_HPP
#define RS485_PROTOCOL_HPP

#include <cstdbool>
#include <cstdint>
#include <cstring>

#include "rs485_crc16.hpp"

#ifdef __cplusplus
extern "C" {
#endif

/* ===================== 协议常量定义 ===================== */
#define RS485_PROTOCOL_HEADER 0xAA      /**< 帧头 */
#define RS485_PROTOCOL_TAIL 0x55        /**< 帧尾 */
#define RS485_PROTOCOL_DEVICE_ID_LEN 8  /**< 设备 ID 长度 */
#define RS485_PROTOCOL_FRAME_MIN_LEN 14 /**< 最小帧长度 */
#define RS485_PROTOCOL_MIN_DATA_LEN 1   /**< 最小数据长度 */
#define RS485_PROTOCOL_DATA_MAX_LEN 256 /**< 数据区最大长度 */

#ifndef PROTOCOL_HEADER
#define PROTOCOL_HEADER RS485_PROTOCOL_HEADER
#endif
#ifndef PROTOCOL_TAIL
#define PROTOCOL_TAIL RS485_PROTOCOL_TAIL
#endif
#ifndef PROTOCOL_DEVICE_ID_LEN
#define PROTOCOL_DEVICE_ID_LEN RS485_PROTOCOL_DEVICE_ID_LEN
#endif
#ifndef PROTOCOL_FRAME_MIN_LEN
#define PROTOCOL_FRAME_MIN_LEN RS485_PROTOCOL_FRAME_MIN_LEN
#endif
#ifndef PROTOCOL_DATA_MAX_LEN
#define PROTOCOL_DATA_MAX_LEN RS485_PROTOCOL_DATA_MAX_LEN
#endif

/* 响应命令码位标识 */
#define RS485_PROTOCOL_RESP_FLAG 0x80 /**< 响应 cmd = 请求 cmd | 0x80 */
#ifndef PROTOCOL_RESP_FLAG
#define PROTOCOL_RESP_FLAG RS485_PROTOCOL_RESP_FLAG
#endif

/* 电机数据个数定义 */
#define RS485_MOTOR_COUNT 6
#ifndef MOTOR_COUNT
#define MOTOR_COUNT RS485_MOTOR_COUNT
#endif

#define RS485_DATA_LEN_READ_STATUS 2                                                  /* status(1) + error_code(1) */
#define RS485_DATA_LEN_READ_VERSION 8                                                 /* version 字符串 */
#define RS485_DATA_LEN_MOTOR_STATUS 23                                                /* id(1) + P+V+F+temp[2]+error = 23 */
#define RS485_DATA_LEN_MOTOR_CONTROL 21                                               /* id(1) + P+V+F+KP+KD = 21 */
#define RS485_DATA_LEN_WRITE_MOTOR (RS485_DATA_LEN_MOTOR_CONTROL * RS485_MOTOR_COUNT) /* 126 */
#define RS485_DATA_LEN_READ_MOTOR (RS485_DATA_LEN_MOTOR_STATUS * RS485_MOTOR_COUNT)   /* 138 */

#ifndef DATA_LEN_READ_STATUS
#define DATA_LEN_READ_STATUS RS485_DATA_LEN_READ_STATUS
#endif
#ifndef DATA_LEN_READ_VERSION
#define DATA_LEN_READ_VERSION RS485_DATA_LEN_READ_VERSION
#endif
#ifndef DATA_LEN_MOTOR_STATUS
#define DATA_LEN_MOTOR_STATUS RS485_DATA_LEN_MOTOR_STATUS
#endif
#ifndef DATA_LEN_MOTOR_CONTROL
#define DATA_LEN_MOTOR_CONTROL RS485_DATA_LEN_MOTOR_CONTROL
#endif
#ifndef DATA_LEN_WRITE_MOTOR
#define DATA_LEN_WRITE_MOTOR RS485_DATA_LEN_WRITE_MOTOR
#endif
#ifndef DATA_LEN_READ_MOTOR
#define DATA_LEN_READ_MOTOR RS485_DATA_LEN_READ_MOTOR
#endif

/* ===================== 协议命令码定义 ===================== */
typedef enum {
    RS485_CMD_READ_STATUS = 0x01,  /**< 读取设备状态 */
    RS485_CMD_READ_VERSION = 0x02, /**< 读取设备版本 */
    RS485_CMD_READ_DATA = 0x03,    /**< 读取电机数据 */
    RS485_CMD_WRITE_DATA = 0x04,   /**< 写入电机数据 */
    RS485_CMD_RESERVED = 0x05      /**< 预留命令(暂不对外开放, 保留扩展) */
} rs485_protocol_cmd_t;

/* ===================== CRC 类型定义 ===================== */
typedef enum {
    RS485_CRC_TYPE_MODBUS = 0,
    RS485_CRC_TYPE_CCITT = 1
} rs485_protocol_crc_type_t;

/* ===================== 数据结构定义 ===================== */
#pragma pack(push, 1)

/**
 * @brief 电机状态数据结构(读取)
 */
typedef struct {
    float P;        /**< 位置 */
    float V;        /**< 速度 */
    float F;        /**< 力 */
    float temp[2];  /**< 温度数组 */
    uint16_t error; /**< 错误码 */
} rs485_motor_status_t;

typedef rs485_motor_status_t motor_status_t;

/**
 * @brief 电机控制数据结构(写入)
 */
typedef struct {
    float P;  /**< 目标位置 */
    float V;  /**< 目标速度 */
    float F;  /**< 目标力 */
    float KP; /**< 比例系数 */
    float KD; /**< 微分系数 */
} rs485_motor_control_t;

typedef rs485_motor_control_t motor_control_t;

/**
 * @brief 设备状态信息
 */
typedef struct {
    uint8_t status;     /**< 设备状态 */
    uint8_t error_code; /**< 错误码 */
} rs485_device_status_t;

typedef rs485_device_status_t device_status_t;

/**
 * @brief 设备版本信息
 */
typedef struct {
    uint8_t hardware_ver; /**< 硬件版本 */
    uint8_t firmware_ver; /**< 固件版本 */
    uint16_t build_date;  /**< 构建日期 */
} rs485_device_version_t;

typedef rs485_device_version_t device_version_t;

/**
 * @brief 协议帧结构体
 *
 * data 字段为 union 类型, 根据 cmd 选择对应数据结构访问:
 *   CMD_READ_STATUS  -> data.readStatus  (status[2])
 *   CMD_READ_VERSION -> data.readVersion (version[8])
 *   CMD_READ_DATA    -> data.readMotor   (6 个电机状态)
 *   CMD_WRITE_DATA   -> data.writeMotor  (6 个电机控制)
 *   CMD_RESERVED     -> data.bytes[0..data_length-1]  (预留, 暂不对外)
 *
 * CRC 只对 data 字段计算, 不含帧头/设备 ID/cmd/长度/CRC/帧尾
 * 设备 ID 采用小端存储
 */
typedef struct {
    uint8_t header;                                  /**< 帧头 0xAA */
    uint8_t device_id[RS485_PROTOCOL_DEVICE_ID_LEN]; /**< 设备 ID(小端) */
    uint8_t cmd;                                     /**< 命令码 */
    uint8_t data_length;                             /**< 数据长度 */
    union {
        uint8_t bytes[RS485_PROTOCOL_DATA_MAX_LEN]; /**< 原始字节视图 */
        struct {
            uint8_t status[RS485_DATA_LEN_READ_STATUS];
        } readStatus;
        struct {
            char version[RS485_DATA_LEN_READ_VERSION];
        } readVersion;
        struct {
            uint8_t id;                 /**< 电机索引 0~5 */
            rs485_motor_status_t motor; /**< 状态: P/V/F/temp[2]/error */
        } readMotor[RS485_MOTOR_COUNT];
        struct {
            uint8_t id;                  /**< 电机索引 0~5 */
            rs485_motor_control_t motor; /**< 控制: P/V/F/KP/KD */
        } writeMotor[RS485_MOTOR_COUNT];
    } data;
    /* 以下三个字段的偏移固定(占位/兼容), 不要直接使用 */
    uint8_t crc_high; /**< CRC 高 8 位 */
    uint8_t crc_low;  /**< CRC 低 8 位 */
    uint8_t tail;     /**< 帧尾 0x55 */
} rs485_protocol_frame_t;

typedef rs485_protocol_frame_t protocol_frame_t;

#pragma pack(pop)

/**
 * @brief 协议层操作接口
 * @note 只负责协议数据的打包与解析, 不涉及硬件操作
 */
typedef struct {
    /**
     * @brief 打包发送帧
     */
    uint16_t (*pack)(rs485_protocol_frame_t* frame, uint8_t* out_buf,
                     uint16_t buf_len, rs485_protocol_crc_type_t crc_type);

    /**
     * @brief 解析接收帧
     */
    bool (*unpack)(const uint8_t* data, uint16_t length,
                   rs485_protocol_frame_t* frame, rs485_protocol_crc_type_t crc_type);

    /**
     * @brief 编码设备 ID
     */
    bool (*encode_device_id)(const char* input, uint8_t* output);

    /**
     * @brief 解码设备 ID
     */
    bool (*decode_device_id)(const uint8_t* input, char* output, uint32_t output_len);

    /**
     * @brief 构建读取状态命令
     */
    void (*build_read_status_cmd)(const uint8_t* device_id, rs485_protocol_frame_t* frame);

    /**
     * @brief 构建读取版本命令
     */
    void (*build_read_version_cmd)(const uint8_t* device_id, rs485_protocol_frame_t* frame);

    /**
     * @brief 构建读取电机数据命令
     */
    void (*build_read_data_cmd)(const uint8_t* device_id, rs485_protocol_frame_t* frame);

    /**
     * @brief 构建写入电机数据命令
     */
    void (*build_write_data_cmd)(const uint8_t* device_id,
                                 const rs485_motor_control_t* motors,
                                 rs485_protocol_frame_t* frame);

    /**
     * @brief 解析电机状态数据
     */
    bool (*parse_motor_status)(const rs485_protocol_frame_t* frame, rs485_motor_status_t* motors);

    /**
     * @brief 解析设备状态数据
     */
    bool (*parse_device_status)(const rs485_protocol_frame_t* frame, rs485_device_status_t* status);

    /**
     * @brief 解析设备版本数据
     */
    bool (*parse_device_version)(const rs485_protocol_frame_t* frame, rs485_device_version_t* version);
} rs485_protocol_ops_t;

/* 兼容旧名称 */
typedef rs485_protocol_ops_t protocol_ops_t;

/**
 * @brief 设备 ID 字符串编码
 *
 * @param input  输入字符串(只含数字、大写字母、小写字母)
 * @param output 输出 8 字节缓冲区
 * @return true 编码成功, false 编码失败
 */
bool rs485_protocol_encode_device_id(const char* input, uint8_t* output);

/**
 * @brief 设备 ID 字符串解码
 */
bool rs485_protocol_decode_device_id(const uint8_t* input, char* output, uint32_t output_len);

/**
 * @brief 获取协议层操作接口实例
 */
const rs485_protocol_ops_t* rs485_protocol_get_ops(void);

/* 兼容旧函数名(直接转发到新函数) */
static inline bool protocol_encode_device_id(const char* input, uint8_t* output) {
    return rs485_protocol_encode_device_id(input, output);
}

static inline bool protocol_decode_device_id(const uint8_t* input, char* output, uint32_t output_len) {
    return rs485_protocol_decode_device_id(input, output, output_len);
}

static inline const rs485_protocol_ops_t* protocol_get_ops(void) {
    return rs485_protocol_get_ops();
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RS485_PROTOCOL_HPP */
