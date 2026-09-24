/**
 * @file rs485_protocol.cpp
 * @brief RS485 通信协议层实现 (C++ 重命名版)
 *
 * 协议格式:
 * +--------+----------+------+-------------+-------+-----+------+-----+
 * | Header | DeviceID | CMD  | DataLength  | Data  | CRC | CRC  | Tail|
 * +--------+----------+------+-------------+-------+-----+------+-----+
 * | 0xAA   | 8字节    | 1字节 | 1字节       | N字节 | 高8 | 低8  | 0x55|
 * +--------+----------+------+-------------+-------+-----+------+-----+
 *
 * 设计要点:
 *   1. 设备 ID 采用小端存储
 *   2. CRC 只对 data 字段计算, 不含其他字段
 *   3. 响应 cmd = 请求 cmd | 0x80
 *   4. data 字段为 union 视图, 根据 cmd 访问对应数据结构
 *   5. CMD_RESERVED (0x05) 为预留命令, 暂不对外开放
 *
 * 本文件替代了旧的 protocol.c, 统一使用 rs485_ 前缀, 并提供 C 接口供 C 端使用.
 */

#include "rs485_protocol.hpp"

#include <cctype>
#include <cstdio>
#include <cstdint>

/* ===================== 设备 ID 编解码 ===================== */

/**
 * @brief 设备 ID 字符串编码
 *
 * 编码规则:
 *   - 数字字符 '0'-'9' : 直接保留(1 位)
 *   - 大写字母 'A'-'Z' : 映射为 29-54, 固定 2 位
 *   - 小写字母 'a'-'z' : 映射为 1-26,  固定 2 位
 *
 * 拼接后的十进制数字串转换为 uint64, 按小端存储到 8 字节数组
 */
bool rs485_protocol_encode_device_id(const char* input, uint8_t* output)
{
    char buffer[64] = {0};
    uint32_t buf_index = 0;
    uint64_t value = 0;

    if (input == nullptr || output == nullptr) {
        return false;
    }

    std::memset(output, 0, RS485_PROTOCOL_DEVICE_ID_LEN);

    /* 步骤 1: 按规则拼接数字字符串 */
    for (int i = 0; input[i] != '\0'; ++i) {
        const unsigned char c = static_cast<unsigned char>(input[i]);
        int written;

        if (std::islower(c)) {
            /* 小写字母: a-z → 1-26, 固定 2 位 */
            const int num = (c - 'a') + 1;
            written = std::snprintf(buffer + buf_index, sizeof(buffer) - buf_index, "%02d", num);
            if (written < 0 || static_cast<uint32_t>(written) >= sizeof(buffer) - buf_index) {
                return false;
            }
            buf_index += static_cast<uint32_t>(written);
        } else if (std::isupper(c)) {
            /* 大写字母: A-Z → 29-54, 固定 2 位 */
            const int num = (c - 'A') + 29;
            written = std::snprintf(buffer + buf_index, sizeof(buffer) - buf_index, "%02d", num);
            if (written < 0 || static_cast<uint32_t>(written) >= sizeof(buffer) - buf_index) {
                return false;
            }
            buf_index += static_cast<uint32_t>(written);
        } else if (std::isdigit(c)) {
            /* 数字字符: 直接保留(1 位) */
            if (buf_index + 1 >= sizeof(buffer)) {
                return false;
            }
            buffer[buf_index++] = static_cast<char>(c);
        } else {
            /* 非法字符 */
            return false;
        }
    }
    buffer[buf_index] = '\0';

    /* 步骤 2: 转换为 uint64 */
    {
        uint64_t multiplier = 1;
        for (uint32_t j = buf_index; j > 0; --j) {
            const char ch = buffer[j - 1];
            if (ch < '0' || ch > '9') {
                return false;
            }
            value += static_cast<uint64_t>(ch - '0') * multiplier;
            multiplier *= 10;
        }
    }

    /* 步骤 3: 按小端存储到 8 字节 */
    for (int i = 0; i < RS485_PROTOCOL_DEVICE_ID_LEN; ++i) {
        output[i] = static_cast<uint8_t>(value & 0xFF);
        value >>= 8;
    }

    return true;
}

/**
 * @brief 设备 ID 字符串解码
 */
bool rs485_protocol_decode_device_id(const uint8_t* input, char* output, uint32_t output_len)
{
    if (input == nullptr || output == nullptr || output_len < 2) {
        return false;
    }
    output[0] = '\0';

    /* 8 字节小端转 uint64 (byte[0] 为低字节) */
    uint64_t value = 0;
    for (int i = RS485_PROTOCOL_DEVICE_ID_LEN - 1; i >= 0; --i) {
        value = (value << 8) | input[i];
    }

    /* uint64 转为十进制字符串(倒序) */
    if (value == 0) {
        if (output_len < 2) {
            return false;
        }
        output[0] = '0';
        output[1] = '\0';
        return true;
    }

    char buffer[64] = {0};
    uint32_t buf_index = 0;
    while (value > 0 && buf_index < sizeof(buffer) - 1) {
        buffer[buf_index++] = static_cast<char>('0' + (value % 10));
        value /= 10;
    }
    buffer[buf_index] = '\0';

    /* 反转得到正向字符串 */
    if (buf_index + 1 > output_len) {
        return false;
    }
    uint32_t out_idx = 0;
    for (uint32_t j = 0; j < buf_index; ++j) {
        output[out_idx++] = buffer[buf_index - 1 - j];
    }
    output[out_idx] = '\0';

    return true;
}

/* ===================== 帧打包 / 解析 ===================== */

/**
 * @brief 打包协议帧(串行化)
 *
 * 输出格式: [Header][DeviceID][Cmd][DataLength][Data...][CRC_H][CRC_L][Tail]
 * CRC 只对 Data 字段计算
 */
static uint16_t rs485_protocol_pack(rs485_protocol_frame_t* frame,
                                    uint8_t* out_buf,
                                    uint16_t buf_len,
                                    rs485_protocol_crc_type_t crc_type)
{
    if (frame == nullptr || out_buf == nullptr) {
        return 0;
    }

    /* 所需总长度: 1+8+1+1+data_length+2+1 = data_length+14 */
    const uint16_t total_len = static_cast<uint16_t>(14 + frame->data_length);
    if (buf_len < total_len) {
        return 0;
    }

    /* 选择 CRC 算法 */
    const rs485_crc16_type_t crc_algo = (crc_type == RS485_CRC_TYPE_MODBUS)
                                          ? CRC16_MODBUS : CRC16_CCITT;

    uint16_t index = 0;

    /* 帧头 */
    out_buf[index++] = RS485_PROTOCOL_HEADER;

    /* 设备 ID */
    std::memcpy(&out_buf[index], frame->device_id, RS485_PROTOCOL_DEVICE_ID_LEN);
    index += RS485_PROTOCOL_DEVICE_ID_LEN;

    /* 命令码 */
    out_buf[index++] = frame->cmd;

    /* 数据长度 */
    out_buf[index++] = frame->data_length;

    /* 数据区 */
    if (frame->data_length > 0) {
        std::memcpy(&out_buf[index], frame->data.bytes, frame->data_length);
        index += frame->data_length;
    }

    /* 计算 CRC(只对 data 字段) */
    const uint16_t crc = rs485_crc16_calculate(frame->data.bytes, frame->data_length, crc_algo);
    out_buf[index++] = static_cast<uint8_t>(crc >> 8);
    out_buf[index++] = static_cast<uint8_t>(crc & 0xFF);
    frame->crc_high = static_cast<uint8_t>(crc >> 8);
    frame->crc_low  = static_cast<uint8_t>(crc & 0xFF);

    /* 帧尾 */
    out_buf[index++] = RS485_PROTOCOL_TAIL;

    return index;
}

/**
 * @brief 查找帧头位置
 */
static int find_header(const uint8_t* data, uint16_t length)
{
    for (int i = 0; i < static_cast<int>(length); ++i) {
        if (data[i] == RS485_PROTOCOL_HEADER) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 解析协议帧
 *
 * 输入: 完整的字节流
 * 输出: 填充 frame 结构体
 */
static bool rs485_protocol_unpack(const uint8_t* data, uint16_t length,
                                  rs485_protocol_frame_t* frame,
                                  rs485_protocol_crc_type_t crc_type)
{
    if (data == nullptr || frame == nullptr || length < RS485_PROTOCOL_FRAME_MIN_LEN) {
        return false;
    }

    const rs485_crc16_type_t crc_algo = (crc_type == RS485_CRC_TYPE_MODBUS)
                                          ? CRC16_MODBUS : CRC16_CCITT;

    /* 查找帧头 */
    const int header_pos = find_header(data, length);
    if (header_pos < 0 || static_cast<uint16_t>(header_pos) >= length) {
        return false;
    }
    uint16_t index = static_cast<uint16_t>(header_pos + 1);

    if (index + RS485_PROTOCOL_DEVICE_ID_LEN >= length) {
        return false;
    }

    /* 读取设备 ID */
    std::memcpy(frame->device_id, &data[index], RS485_PROTOCOL_DEVICE_ID_LEN);
    index += RS485_PROTOCOL_DEVICE_ID_LEN;

    if (index >= length) return false;
    frame->cmd = data[index++];

    if (index >= length) return false;
    frame->data_length = data[index++];

    if (frame->data_length > 0) {
        if (static_cast<uint32_t>(index) + frame->data_length + 3 > length) {
            return false;
        }
        std::memcpy(frame->data.bytes, &data[index], frame->data_length);
        index += frame->data_length;
    }

    if (static_cast<uint32_t>(index) + 2 + 1 > length) {
        return false;
    }

    /* 验证 CRC(只对 data 字段) */
    const uint8_t recv_crc_h = data[index];
    const uint8_t recv_crc_l = data[index + 1];
    const uint16_t calc_crc = rs485_crc16_calculate(frame->data.bytes, frame->data_length, crc_algo);
    if (recv_crc_h != static_cast<uint8_t>(calc_crc >> 8) ||
        recv_crc_l != static_cast<uint8_t>(calc_crc & 0xFF)) {
        return false;
    }
    frame->crc_high = recv_crc_h;
    frame->crc_low  = recv_crc_l;
    index += 2;

    if (index >= length || data[index] != RS485_PROTOCOL_TAIL) {
        return false;
    }

    return true;
}

/* ===================== 命令构建函数 ===================== */

static void init_frame(rs485_protocol_frame_t* frame, const uint8_t* device_id, uint8_t cmd)
{
    std::memset(frame, 0, sizeof(rs485_protocol_frame_t));
    frame->header = RS485_PROTOCOL_HEADER;
    if (device_id != nullptr) {
        std::memcpy(frame->device_id, device_id, RS485_PROTOCOL_DEVICE_ID_LEN);
    }
    frame->cmd = cmd;
    frame->tail = RS485_PROTOCOL_TAIL;
}

/**
 * @brief 构建读取状态命令
 */
static void build_read_status_cmd(const uint8_t* device_id, rs485_protocol_frame_t* frame)
{
    if (frame == nullptr) return;
    init_frame(frame, device_id, RS485_CMD_READ_STATUS);
    frame->data_length = 0;
}

/**
 * @brief 构建读取版本命令
 */
static void build_read_version_cmd(const uint8_t* device_id, rs485_protocol_frame_t* frame)
{
    if (frame == nullptr) return;
    init_frame(frame, device_id, RS485_CMD_READ_VERSION);
    frame->data_length = 0;
}

/**
 * @brief 构建读取电机数据命令
 */
static void build_read_data_cmd(const uint8_t* device_id, rs485_protocol_frame_t* frame)
{
    if (frame == nullptr) return;
    init_frame(frame, device_id, RS485_CMD_READ_DATA);
    frame->data_length = 0;
}

/**
 * @brief 构建写入电机数据命令
 */
static void build_write_data_cmd(const uint8_t* device_id,
                                 const rs485_motor_control_t* motors,
                                 rs485_protocol_frame_t* frame)
{
    if (frame == nullptr || motors == nullptr) return;

    init_frame(frame, device_id, RS485_CMD_WRITE_DATA);

    for (int i = 0; i < RS485_MOTOR_COUNT; ++i) {
        frame->data.writeMotor[i].id    = static_cast<uint8_t>(i + 1);
        frame->data.writeMotor[i].motor = motors[i];
    }
    frame->data_length = static_cast<uint8_t>(RS485_DATA_LEN_WRITE_MOTOR);
}

/* ===================== 数据解析函数 ===================== */

/**
 * @brief 解析电机状态数据
 * 使用 union 视图: frame->data.readMotor
 */
static bool parse_motor_status(const rs485_protocol_frame_t* frame, rs485_motor_status_t* motors)
{
    if (frame == nullptr || motors == nullptr) return false;

    if (frame->data_length != RS485_DATA_LEN_READ_MOTOR) {
        return false;
    }

    for (int i = 0; i < RS485_MOTOR_COUNT; ++i) {
        motors[i] = frame->data.readMotor[i].motor;
    }
    return true;
}

/**
 * @brief 解析设备状态数据
 *
 * 数据格式: status(1) + error_code(1) = 2 字节
 */
static bool parse_device_status(const rs485_protocol_frame_t* frame, rs485_device_status_t* status)
{
    if (frame == nullptr || status == nullptr) return false;
    if (frame->data_length < RS485_DATA_LEN_READ_STATUS) return false;

    status->status     = frame->data.readStatus.status[0];
    status->error_code = frame->data.readStatus.status[1];
    return true;
}

/**
 * @brief 解析设备版本数据
 *
 * 数据格式: hardware_ver(1) + firmware_ver(1) + build_date(2) = 4 字节
 */
static bool parse_device_version(const rs485_protocol_frame_t* frame, rs485_device_version_t* version)
{
    if (frame == nullptr || version == nullptr) return false;
    if (frame->data_length < RS485_DATA_LEN_READ_VERSION) return false;

    /* version 字符串解析: 简化实现, 只取前 4 字节 */
    version->hardware_ver = static_cast<uint8_t>(frame->data.readVersion.version[0]);
    version->firmware_ver = static_cast<uint8_t>(frame->data.readVersion.version[1]);
    version->build_date   = static_cast<uint16_t>(
        (static_cast<uint16_t>(frame->data.readVersion.version[2]) << 8) |
         static_cast<uint16_t>(frame->data.readVersion.version[3]));
    return true;
}

/* ===================== 操作接口实例 ===================== */

static const rs485_protocol_ops_t rs485_protocol_ops_instance = {
    .pack                  = rs485_protocol_pack,
    .unpack                = rs485_protocol_unpack,
    .encode_device_id      = rs485_protocol_encode_device_id,
    .decode_device_id      = rs485_protocol_decode_device_id,
    .build_read_status_cmd = build_read_status_cmd,
    .build_read_version_cmd= build_read_version_cmd,
    .build_read_data_cmd   = build_read_data_cmd,
    .build_write_data_cmd  = build_write_data_cmd,
    .parse_motor_status    = parse_motor_status,
    .parse_device_status   = parse_device_status,
    .parse_device_version  = parse_device_version,
};

/**
 * @brief 获取协议层操作接口实例
 */
const rs485_protocol_ops_t* rs485_protocol_get_ops(void)
{
    return &rs485_protocol_ops_instance;
}
