#ifndef HW_PRESSURE_SENSOR_H_
#define HW_PRESSURE_SENSOR_H_

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "HARDWARE_TOP.hpp"

#define Pressure_Sensor_Custom_Type "Pressure_Sensor_Custom"

// 触觉传感器主从模式通信协议实现
// 版本: V1.6.250303

// 设备ID定义 (对应手指部位)
#define HW_THUMB_ID 0x01          // 大拇指
#define HW_INDEX_FINGER_ID 0x02   // 食指
#define HW_MIDDLE_FINGER_ID 0x03  // 中指
#define HW_RING_FINGER_ID 0x04    // 无名指
#define HW_LITTLE_FINGER_ID 0x05  // 小拇指
#define HW_PALM_CENTER_ID 0x06    // 掌心
#define HW_PALM_BACK_ID 0x07      // 掌背

// 信道定义
#define HW_CHANNEL_DEVICE_INFO 0x01  // 设备信息
#define HW_CHANNEL_SENSOR_DATA 0x02  // 请求上报传感器数据
#define HW_CHANNEL_ZERO_CALIB 0x07   // 归零校准
#define HW_CHANNEL_SET_ADDRESS 0x09  // 设置设备地址

// FLAGS字段定义
#define HW_FLAGS_PUT 0x00  // PUT操作
#define HW_FLAGS_GET 0x01  // GET操作
#define HW_FLAGS_ACK 0x03  // ACK响应

// 设备信息子命令
#define HW_CMD_VERSION 0x01       // 版本号
#define HW_CMD_PROTOCOL_VER 0x02  // 协议版本号
#define HW_CMD_DEVICE_INFO 0x03   // 设备信息
#define HW_CMD_DATA_RANGE 0x04    // 数值范围
#define HW_CMD_SERIAL_NUM 0x05    // 序列号
#define HW_CMD_DEVICE_ADDR 0x06   // 设备地址

// 校准命令
#define HW_CALIB_OFF 0x00    // 关闭校准
#define HW_CALIB_ON 0x01     // 开启校准
#define HW_CALIB_RESET 0x02  // 重新校准

// 协议常量
#define HW_FRAME_HEAD 0x3C3C      // 帧头: <<
#define HW_FRAME_TAIL 0x3E3E      // 帧尾: >>
#define HW_MAX_PAYLOAD_SIZE 1024  // 最大负载长度
#define HW_DATA_RANGE_MAX 4096    // 数据范围最大值

// 帧结构定义 (字节对齐)
#pragma pack(push, 1)
struct Hw_Pressure_Sensor_Frame {
    u16 head;                         // 帧头: 0x3C3C ("<<")
    u8 id_channel;                    // 高4位: ID, 低4位: 信道
    u8 flags;                         // 标志位
    u16 length;                       // 负载长度(小端字节序)
    u8 total_packets;                 // 总包数
    u8 current_packet;                // 当前包序号
    u8 cols;                          // 列数
    u8 rows;                          // 行数
    u8 payload[HW_MAX_PAYLOAD_SIZE];  // 负载数据
    u16 checksum;                     // 校验和(仅对负载计算)
    u16 tail;                         // 帧尾: 0x3E3E (">>")
};
#pragma pack(pop)

// 传感器设备状态（用于监控）
struct Sensor_Device_Status {
    bool online;                   // 设备是否在线
    uint64_t last_response_time;   // 最后响应时间
    uint64_t last_request_time;    // 最后请求时间
    uint32_t timeout_count;        // 超时计数
    uint32_t success_count;        // 成功计数
    uint32_t total_request_count;  // 总请求次数
    float success_rate;            // 成功率
};

// 数据质量统计
struct Data_Quality_Stats {
    u16 valid_count;         // 有效数据点数
    u16 out_of_range_count;  // 超范围数据点数
    u16 min_value;           // 最小值
    u16 max_value;           // 最大值
    u32 sum_value;           // 总和
    u16 average_value;       // 平均值
    float quality_score;     // 数据质量评分
};

// 传感器数据查询结果
struct Sensor_Data_Result {
    bool success;               // 查询是否成功
    bool device_online;         // 设备是否在线
    u8 sensor_id;               // 传感器ID
    uint64_t timestamp;         // 数据时间戳
    std::vector<u16> data;      // 传感器数据
    std::string error_message;  // 错误信息
};

typedef struct Serial_Data {
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

    // DATA
    u8 data[1024];
} Serial_Data;

// 初始化宏定义
#define Hw_Pressure_Sensor_Init                                               \
    [](std::shared_ptr<Device_class> Device, YAML::Node* Node) -> int {       \
        Hw_Pressure_Sensor* sensor = new Hw_Pressure_Sensor();                \
        Device->Device_Private_Class = (void*)sensor;                         \
        if (Node != nullptr)                                                  \
            return sensor->Hw_Pressure_Sensor_Data_From_Yaml_And_Init(Device, \
                                                                      *Node); \
        else                                                                  \
            return 0;                                                         \
    }

#define Hw_Pressure_Sensor_CallBack_F                              \
    [](shared_ptr<Device_class> Device, u8* Msg) -> int {          \
        return ((Hw_Pressure_Sensor*)Device->Device_Private_Class) \
            ->Hw_Pressure_Sensor_Frame_Analyze(Msg);               \
    }

#define Hw_Pressure_Sensor_Delete_F                         \
    [](void* Device_Private_Class) {                        \
        delete ((Hw_Pressure_Sensor*)Device_Private_Class); \
        Device_Private_Class = nullptr;                     \
    }

// 触觉传感器控制类
class Hw_Pressure_Sensor : private Robot_Hardware {
   private:
    bool async_mode = false;  // 是否启用异步采集模式，默认为false，同步模式

   public:
    /* 构造函数 */
    Hw_Pressure_Sensor(void);

    /* 析构函数 */
    ~Hw_Pressure_Sensor();

    /**
     * @brief 从YAML配置初始化设备
     * @param Device 设备结构体
     * @param One_Node YAML配置节点
     * @return 成功返回0，失败返回错误码
     */
    int Hw_Pressure_Sensor_Data_From_Yaml_And_Init(std::shared_ptr<Device_class> Device, YAML::Node One_Node);

    /**
     * @brief 帧解析函数
     * @param Can_Frame 接收到的CAN帧数据
     * @return 成功返回0，失败返回错误码
     */
    int Hw_Pressure_Sensor_Frame_Analyze(volatile u8* Can_Frame);

    // ========== 设备控制接口 ==========
    /* 请求传感器数据 (GET操作) */
    int Request_Sensor_Data(std::shared_ptr<Device_class> Device_P, u8 sensor_id);

    /* 查询设备信息 */
    int Query_Device_Info(std::shared_ptr<Device_class> Device_P, u8 sensor_id, u8 cmd);

    /* 归零校准控制 */
    int Zero_Calibration(std::shared_ptr<Device_class> Device_P, u8 sensor_id, u8 cmd);

    /* 设置设备地址 */
    int Set_Device_Address(std::shared_ptr<Device_class> Device_P, u8 sensor_id, u8 new_addr);

    // ========== 数据采集控制 ==========
    /**
     * @brief 开始周期性数据采集
     * @param Device_P 设备结构体
     * @param interval_ms 采集间隔(毫秒)
     * @return 成功返回0，失败返回错误码
     */
    int Start_Periodic_Data_Collection(std::shared_ptr<Device_class> Device_P, u32 interval_ms);

    /* 停止周期性数据采集 */
    int Stop_Periodic_Data_Collection();

    // ========== 数据存储和查询接口 ==========
    /**
     * @brief 获取指定传感器的存储数据
     * @param sensor_id 传感器ID
     * @return 数据向量
     */
    std::vector<u16> Get_Stored_Sensor_Data(u8 sensor_id);

    /**
     * @brief 获取数据存储统计信息
     * @return std::string 统计信息字符串
     */
    std::string Get_Data_Storage_Statistics();

    // ========== 回调函数设置 ==========
    /**
     * @brief 设置数据接收回调函数
     * @param callback 回调函数
     */
    void Set_Data_Callback(std::function<void(u8 sensor_id, const std::vector<u16>& data)> callback);

    /**
     * @brief 设置设备信息回调函数
     * @param callback 回调函数
     */
    void Set_DeviceInfo_Callback(std::function<void(u8 sensor_id, u8 cmd, const std::vector<u8>& data)> callback);

    // ========== 状态监控接口 ==========
    /* 获取设备状态 */
    Sensor_Device_Status Get_Device_Status(u8 sensor_id);

    /* 设置响应超时时间 */
    void Set_Response_Timeout(u32 timeout_ms);

    /**
     * @brief 获取所有设备状态统计信息
     * @return 状态统计字符串
     */
    std::string Get_Status_Statistics();

   public:
    // 回调函数指针
    int (*Custom_Msg_CallBack)(volatile u8*);

   private:
    // ========== 协议处理私有方法 ==========
    /* CRC16-CCITT校验和计算 */
    u16 Calculate_Checksum(const u8* data, u16 length);

    int Serial_Send(std::shared_ptr<Device_class> Device_P, u8* Data, u32 len);

    /**
     * @brief 构建并发送帧
     * @param Device_P 设备结构体
     * @param sensor_id 传感器ID
     * @param channel 信道
     * @param flags 标志位
     * @param payload 负载数据
     * @param payload_len 负载长度
     * @return 成功返回0，失败返回错误码
     */
    int SendFrame(std::shared_ptr<Device_class> Device_P, u8 sensor_id, u8 channel, u8 flags, const u8* payload, u16 payload_len);

    /* 解析接收到的协议帧 */
    bool Parse_Received_Frame(const u8* data, u16 length, Hw_Pressure_Sensor_Frame& frame);

    // ========== 数据解析方法 ==========
    /**
     * @brief 解析传感器数据
     * @param frame 接收到的帧
     */
    void Parse_Sensor_Data(const Hw_Pressure_Sensor_Frame& frame);

    /* 解析设备信息 */
    void Parse_Device_Info(const Hw_Pressure_Sensor_Frame& frame);

    // ========== 设备状态管理 ==========
    /* 记录设备请求发送 */
    void Record_Device_Request(u8 sensor_id);

    /* 处理设备响应成功 */
    void Handle_Device_Response(u8 sensor_id);

    /* 获取当前时间戳(毫秒) */
    uint64_t Get_Current_Time_Ms();

    /**
     * @brief 如果传感器离线，清空其存储的数据
     * @param sensor_id 传感器ID
     */
    void Clear_Sensor_Data_If_Offline(u8 sensor_id);

    void Check_And_Recover_Offline_Sensor(u8 sensor_id);

    /**
     * @brief 打印十六进制数据
     * @param data 数据指针
     * @param length 数据长度
     * @param prefix 前缀说明
     */
    void Print_Hex_Data(const u8* data, u16 length, const std::string& prefix);

    /**
     * @brief 快速提取传感器ID（不解析完整帧）
     */
    u8 Quick_Extract_Sensor_ID(const u8* data, u16 length);

    /**
     * @brief 存储传感器数据到字典
     */
    void Store_Sensor_Data(u8 sensor_id, const std::vector<u16>& data);

    // 成员变量
    std::atomic<bool> periodic_collection_active_{false};
    std::atomic<u32> collection_interval_{20};
    std::atomic<u32> response_timeout_ms_{5};

    // 状态管理（需要锁保护）
    std::mutex status_mutex_;
    std::map<u8, Sensor_Device_Status> device_status_;  // 设备状态映射表

    // 数据存储
    std::mutex data_mutex_;
    std::map<u8, std::vector<u16>> sensor_data_map_;  // 传感器数据字典
    std::map<u8, uint64_t> data_timestamp_map_;       // 数据时间戳字典

    // --- 异步采集 ---
    std::mutex collection_mutex_;                    // 采集状态变量
    std::atomic<int> current_sensor_index_{-1};      // 当前应发送请求的传感器索引
    std::atomic<bool> waiting_for_response_{false};  // 是否正在等待上一个传感器的响应
    std::vector<u8> sensor_collection_order_;        // 传感器轮询顺序
    std::map<u8, bool> sensor_response_received_;    // 传感器响应到达标志

    // 存储每个传感器最后请求时间
    std::map<u8, uint64_t> last_request_time_;
    // --- 异步采集 ---

    // --- 同步模式改进 ---
    std::mutex sync_response_mutex_;                    // 同步模式响应保护锁
    std::map<u8, bool> sync_sensor_response_received_;  // 同步模式下的传感器响应标志
    // --- 同步模式改进 ---

    // 回调函数
    std::function<void(u8 sensor_id, const std::vector<u16>& data)> data_callback_;
    std::function<void(u8 sensor_id, u8 cmd, const std::vector<u8>& data)> device_info_callback_;
};

#endif  // HW_PRESSURE_SENSOR_H_
