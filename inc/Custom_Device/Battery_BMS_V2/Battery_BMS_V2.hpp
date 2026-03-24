#ifndef BATTERY_BMS_V2_H_
#define BATTERY_BMS_V2_H_
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"
#include "syst.hpp"
// 设备类型定义
#define Battery_BMS_V2_Custom_Type "Battery_BMS_V2_Custom"

// 协议规定CAN波特率：1000K
#define BMS_CAN_BAUDRATE 1000000
// 地址修改广播CAN ID
#define BMS_CMD_CHANGE_ADDRESS_BROADCAST_ID 0x7FF
// 电池包默认CAN ID，可设置范围0x102-0x7FE
#define BMS_DEFAULT_CAN_ID 0x102
#define BMS_CAN_ID_MIN 0x102
#define BMS_CAN_ID_MAX 0x7FE
// 低功耗超时时间：10分钟无通信
#define BMS_LOW_POWER_TIMEOUT_MS (10 * 60 * 1000)

// ==================== 协议功能Cmd枚举（匹配V2协议数据域首字节定义） ====================
enum BMS_Cmd_Type : uint8_t {
    BMS_CMD_BASIC_INFO = 0x01,          // 电池基本信息
    BMS_CMD_VOLTAGE_CURRENT = 0x02,     // 电池能源信息（电压/电流/SOC/SOH）
    BMS_CMD_CAPACITY_TEMP = 0x03,       // 电池容量温度信息
    BMS_CMD_STATUS = 0x04,              // 电池状态信息
    BMS_CMD_CELL_VOLTAGE_LIMIT = 0x05,  // 电池最高最低电压信息
    BMS_CMD_ALARM_PROTECTION = 0x06,    // 电池告警&保护信息
    BMS_CMD_CELL_VOLTAGE_1_3 = 0x0A,    // 1-3节单体电压
    BMS_CMD_CELL_VOLTAGE_4_6 = 0x0B,    // 4-6节单体电压
    BMS_CMD_CELL_VOLTAGE_7_9 = 0x0C,    // 7-9节单体电压
    BMS_CMD_CELL_VOLTAGE_10_12 = 0x0D,  // 10-12节单体电压
    BMS_CMD_CELL_VOLTAGE_13_15 = 0x0E,  // 13-15节单体电压
    BMS_CMD_UID = 0x0F,                 // 电池唯一UID信息
};

#pragma pack(push, 1)

typedef struct {
    u16 Can_Id;  // CAN标识符（电池包地址，匹配协议0x102-0x7FE）
    u16 Len;     // 数据长度
    u16 Flag;    // 标志位
    u8 Data[8];  // 8字节数据域：Data[0]=Cmd指令，Data[1-7]=功能数据
} BMS_Data_Frame;

// ==================== 数据结构体定义 ====================
// 0x01 电池基本信息
typedef struct {
    u8 cell_count;         // 电池单体数（1字节）
    u8 ntc_count;          // NTC数量（1字节）
    u8 nominal_capacity;   // 电池标称容量（1字节，单位：0.1AH）
    u16 software_version;  // BMS软件版本号（2字节，低位在前，单位：0.01）
    u16 hardware_version;  // BMS硬件版本号（2字节，低位在前，单位：0.01）
} BMS_Basic_Info;

// 0x02 电池能源信息（电压/电流/SOC/SOH）
typedef struct {
    u16 total_voltage;  // 总电压（2字节，低位在前，单位：0.01V）
    s16 current;        // 电流（2字节，低位在前，有符号，单位：0.01A）
    u8 soc;             // SOC（1字节，单位：1%）
    u16 soh;            // SOH（2字节，低位在前，单位：0.1%）
} BMS_Voltage_Current_Info;

// 0x03 电池容量温度信息
typedef struct {
    u16 actual_capacity;   // 电池实际容量（2字节，低位在前，单位：0.01AH）
    u16 cycle_count;       // 循环次数（2字节，低位在前）
    s16 cell_temperature;  // 电芯温度（2字节，低位在前，有符号，单位：0.01℃）
    char mos_temperature;  // MOS温度（1字节，有符号，单位：1℃）
} BMS_Capacity_Temp_Info;

// 0x04 电池状态信息
typedef struct {
    u8 discharge_mos_status;      // 放电MOS状态（0断开 1闭合）
    u8 charge_mos_status;         // 充电MOS状态（0断开 1闭合）
    u8 pre_discharge_mos_status;  // 预放电MOS状态（0断开 1闭合，预留）
    u8 charger_connect_status;    // 充电器接入状态（0断开 1接入）
    u8 load_connect_status;       // 负载连接状态（0断开 1接入）
    u8 system_status;             // 系统状态（0空闲 1充电 2放电）
} BMS_Status_Info;

// 0x05 电池最高最低电压信息
typedef struct {
    u16 max_cell_voltage;  // 单体最高电压（2字节，低位在前，单位：0.001V）
    u8 max_cell_index;     // 单体最高电压序号（1字节）
    u16 min_cell_voltage;  // 单体最低电压（2字节，低位在前，单位：0.001V）
    u8 min_cell_index;     // 单体最低电压序号（1字节）
} BMS_Cell_Voltage_Limit_Info;

// 0x06 告警&保护信息位域定义（匹配协议附注1）
// 告警信息1（1字节，8bit）
typedef struct {
    u8 cell_over_voltage_alarm : 1;       // Bit0 单体过压告警
    u8 cell_under_voltage_alarm : 1;      // Bit1 单体欠压告警
    u8 total_over_voltage_alarm : 1;      // Bit2 总压过压告警
    u8 total_under_voltage_alarm : 1;     // Bit3 总压欠压告警
    u8 discharge_over_current_alarm : 1;  // Bit4 放电过流告警
    u8 charge_over_current_alarm : 1;     // Bit5 充电过流告警
    u8 discharge_high_temp_alarm : 1;     // Bit6 放电高温告警
    u8 charge_high_temp_alarm : 1;        // Bit7 充电高温告警
} BMS_Alarm_Info_1;

// 告警信息2（1字节，8bit）
typedef struct {
    u8 discharge_low_temp_alarm : 1;  // Bit0 放电低温告警
    u8 charge_low_temp_alarm : 1;     // Bit1 充电低温告警
    u8 soc_too_low_alarm : 1;         // Bit2 SOC过低告警
    u8 mos_high_temp_alarm : 1;       // Bit3 MOS温度过高告警
    u8 env_high_temp_alarm : 1;       // Bit4 环境温度过高告警
    u8 reserved1 : 3;                 // Bit5-7 预留
} BMS_Alarm_Info_2;

// 保护信息1（1字节，8bit）
typedef struct {
    u8 cell_over_voltage_protect : 1;       // Bit0 单体过压保护
    u8 cell_under_voltage_protect : 1;      // Bit1 单体欠压保护
    u8 total_over_voltage_protect : 1;      // Bit2 总压过压保护
    u8 total_under_voltage_protect : 1;     // Bit3 总压欠压保护
    u8 discharge_over_current_protect : 1;  // Bit4 放电过流保护
    u8 charge_over_current_protect : 1;     // Bit5 充电过流保护
    u8 discharge_high_temp_protect : 1;     // Bit6 放电高温保护
    u8 charge_high_temp_protect : 1;        // Bit7 充电高温保护
} BMS_Protection_Info_1;

// 保护信息2（1字节，8bit）
typedef struct {
    u8 discharge_low_temp_protect : 1;  // Bit0 放电低温保护
    u8 charge_low_temp_protect : 1;     // Bit1 充电低温保护
    u8 mos_high_temp_protect : 1;       // Bit2 MOS温度过高保护
    u8 discharge_mos_fault : 1;         // Bit3 放电MOS故障
    u8 charge_mos_fault : 1;            // Bit4 充电MOS故障
    u8 hardware_ic_fault : 1;           // Bit5 硬件IC故障
    u8 low_voltage_charge_ban : 1;      // Bit6 低压禁止充电
    u8 short_circuit_protect : 1;       // Bit7 短路保护
} BMS_Protection_Info_2;

// 保护信息3（1字节，8bit，预留）
typedef struct {
    u8 reserved[8];  // 全预留
} BMS_Protection_Info_3;

// 0x06 完整告警&保护信息结构体
typedef struct {
    BMS_Alarm_Info_1 alarm1;
    BMS_Alarm_Info_2 alarm2;
    BMS_Protection_Info_1 protect1;
    BMS_Protection_Info_2 protect2;
    BMS_Protection_Info_3 protect3;
} BMS_Alarm_Protection_Info;

// 单体电压详细信息（3节/组，匹配协议0x0A~0x0E）
typedef struct {
    u16 cell_voltage[3];  // 3节单体电压（每节2字节，低位在前，单位：0.001V）
} BMS_Cell_Voltage_Detail;

// 0x0F 电池UID信息
typedef struct {
    u32 full_uid;  // 32位电池唯一UID（低位在前）
} BMS_UID_Info;

// ==================== BMS数据整合结构体 (用于ROS2等数据采集) ====================
typedef struct {
    // 时间戳（毫秒级）
    uint64_t timestamp_ms;

    // 0x01 电池基本信息
    uint8_t cell_count;         // 电池单体数
    uint8_t ntc_count;          // NTC数量
    float nominal_capacity;   // 电池标称容量（单位：0.1AH）
    float software_version;  // BMS软件版本号（单位：0.01）
    float hardware_version;  // BMS硬件版本号（单位：0.01）

    // 0x02 电池能源信息
    float total_voltage;  // 总电压（单位：0.01V）
    float current;         // 电流（有符号，单位：0.01A）
    uint8_t soc;             // SOC（单位：1%）
    float soh;            // SOH（单位：0.1%）

    // 0x03 电池容量温度信息
    float actual_capacity;  // 电池实际容量（单位：0.01AH）
    uint16_t cycle_count;      // 循环次数
    float cell_temperature;  // 电芯温度（有符号，单位：0.01℃）
    int8_t mos_temperature;    // MOS温度（有符号，单位：1℃）

    // 0x04 电池状态信息
    uint8_t discharge_mos_status;      // 放电MOS状态（0断开 1闭合）
    uint8_t charge_mos_status;         // 充电MOS状态（0断开 1闭合）
    uint8_t pre_discharge_mos_status;  // 预放电MOS状态（0断开 1闭合，预留）
    uint8_t charger_connect_status;    // 充电器接入状态（0断开 1接入）
    uint8_t load_connect_status;       // 负载连接状态（0断开 1接入）
    uint8_t system_status;             // 系统状态（0空闲 1充电 2放电）

    // 0x05 电池最高最低电压信息
    float max_cell_voltage;  // 单体最高电压（单位：0.001V）
    uint8_t max_cell_index;     // 单体最高电压序号
    float min_cell_voltage;  // 单体最低电压（单位：0.001V）
    uint8_t min_cell_index;     // 单体最低电压序号

    // 0x06 告警&保护信息
    // 告警信息1
    uint8_t cell_over_voltage_alarm : 1;       // Bit0 单体过压告警
    uint8_t cell_under_voltage_alarm : 1;      // Bit1 单体欠压告警
    uint8_t total_over_voltage_alarm : 1;      // Bit2 总压过压告警
    uint8_t total_under_voltage_alarm : 1;     // Bit3 总压欠压告警
    uint8_t discharge_over_current_alarm : 1;  // Bit4 放电过流告警
    uint8_t charge_over_current_alarm : 1;     // Bit5 充电过流告警
    uint8_t discharge_high_temp_alarm : 1;     // Bit6 放电高温告警
    uint8_t charge_high_temp_alarm : 1;        // Bit7 充电高温告警

    // 告警信息2
    uint8_t discharge_low_temp_alarm : 1;  // Bit0 放电低温告警
    uint8_t charge_low_temp_alarm : 1;     // Bit1 充电低温告警
    uint8_t soc_too_low_alarm : 1;         // Bit2 SOC过低告警
    uint8_t mos_high_temp_alarm : 1;       // Bit3 MOS温度过高告警
    uint8_t env_high_temp_alarm : 1;       // Bit4 环境温度过高告警
    uint8_t alarm_reserved : 3;            // Bit5-7 预留

    // 保护信息1
    uint8_t cell_over_voltage_protect : 1;       // Bit0 单体过压保护
    uint8_t cell_under_voltage_protect : 1;      // Bit1 单体欠压保护
    uint8_t total_over_voltage_protect : 1;      // Bit2 总压过压保护
    uint8_t total_under_voltage_protect : 1;     // Bit3 总压欠压保护
    uint8_t discharge_over_current_protect : 1;  // Bit4 放电过流保护
    uint8_t charge_over_current_protect : 1;     // Bit5 充电过流保护
    uint8_t discharge_high_temp_protect : 1;     // Bit6 放电高温保护
    uint8_t charge_high_temp_protect : 1;        // Bit7 充电高温保护

    // 保护信息2
    uint8_t discharge_low_temp_protect : 1;  // Bit0 放电低温保护
    uint8_t charge_low_temp_protect : 1;     // Bit1 充电低温保护
    uint8_t mos_high_temp_protect : 1;       // Bit2 MOS温度过高保护
    uint8_t discharge_mos_fault : 1;         // Bit3 放电MOS故障
    uint8_t charge_mos_fault : 1;            // Bit4 充电MOS故障
    uint8_t hardware_ic_fault : 1;           // Bit5 硬件IC故障
    uint8_t low_voltage_charge_ban : 1;      // Bit6 低压禁止充电
    uint8_t short_circuit_protect : 1;       // Bit7 短路保护

    // 保护信息3（预留）
    uint8_t protect_reserved;

    // 0x0A~0x0E 单体电压详细信息（最多15节）
    float cell_voltages[15];  // 单体电压数组（单位：0.001V）

    // 0x0F 电池UID信息
    uint32_t uid;  // 32位电池唯一UID

    // 设备状态标志
    bool is_initialized;      // 设备是否初始化
    bool is_low_power_mode;   // 是否低功耗模式
    uint32_t last_comm_time;  // 最后通信时间戳
    uint16_t bms_can_id;      // BMS设备CAN ID

    // 预留字段，用于未来扩展
    uint8_t reserved[16];
} BMS_Consolidated_Data;
#pragma pack(pop)

// ==================== 设备初始化/回调/删除宏定义 ====================
// 设备初始化函数
#define Battery_BMS_V2_Device_Init                                                       \
    [](shared_ptr<Device_class> Device, YAML::Node* Node) -> int {                       \
        BMS_V2_Protocol* bms_v2_device = new BMS_V2_Protocol();                          \
        Device->Device_Private_Class = (void*)bms_v2_device;                             \
        if (Node != nullptr)                                                             \
            return bms_v2_device->Get_BMS_Device_Data_From_Yaml_And_Init(Device, *Node); \
        else                                                                             \
            return 0;                                                                    \
    }

// 设备接收回调函数
#define Battery_BMS_V2_Device_CallBack                                                   \
    [](shared_ptr<Device_class> Device, u8* Msg) -> int {                                \
        return ((BMS_V2_Protocol*)Device->Device_Private_Class)->BMS_Frame_Analyze(Msg); \
    }

// 设备删除函数
#define Battery_BMS_V2_Device_Delete                     \
    [](void* Device_Private_Class) {                     \
        delete ((BMS_V2_Protocol*)Device_Private_Class); \
        Device_Private_Class = nullptr;                  \
    }

// ==================== BMS协议核心类 ====================
class BMS_V2_Protocol : private Robot_Hardware {
   private:
    // BMS设备CAN ID（匹配协议0x102-0x7FE）
    u16 bms_can_id;
    // BMS数据缓存（线程安全）
    BMS_Basic_Info basic_info;
    BMS_Voltage_Current_Info voltage_current_info;
    BMS_Capacity_Temp_Info capacity_temp_info;
    BMS_Status_Info status_info;
    BMS_Cell_Voltage_Limit_Info cell_voltage_limit_info;
    BMS_Alarm_Protection_Info alarm_protection_info;
    BMS_Cell_Voltage_Detail cell_voltage_detail[5];  // 0:1-3, 1:4-6, 2:7-9, 3:10-12, 4:13-15
    BMS_UID_Info uid_info;

    // 设备状态
    bool is_initialized;
    bool is_low_power_mode;
    u32 last_communication_time;

    // 线程锁（保护数据读写）
    mutable std::mutex data_mutex;

    // =================数据采集线程控制=============
    std::atomic<bool> data_collection_running_;  // 原子标志，控制采集线程启停
    std::thread data_collection_thread_;         // 数据采集线程
    uint32_t collection_interval_ms_;            // 数据采集周期（毫秒）

    // =================数据采集=============
    BMS_Consolidated_Data g_comsolidated_data;  // 全面整合数据结构体
    // 数据保护互斥锁
    mutable std::mutex g_bms_data_mutex;

   public:
    // 设备句柄
    std::shared_ptr<Device_class>
        s_device;

    /**
     * @brief 构造函数
     */
    BMS_V2_Protocol(void);
    /**
     * @brief 析构函数
     */
    ~BMS_V2_Protocol();

    // ==================== 核心控制函数 ====================
    /**
     * @brief 唤醒BMS设备（匹配协议：连续3帧数据唤醒低功耗设备）
     * @param Device_P 设备结构体指针
     * @return 执行结果 0成功 -1失败
     */
    int Wake_Up_BMS(std::shared_ptr<Device_class> Device_P);
    /**
     * @brief 修改BMS设备CAN ID（匹配协议0x7FF广播地址修改）
     * @param Device_P 设备结构体指针
     * @param new_can_id 新CAN ID（范围0x102-0x7FE）
     * @return 执行结果 0成功 -1失败
     */
    int Change_BMS_CAN_ID(std::shared_ptr<Device_class> Device_P, u16 new_can_id);

    // ==================== 数据请求函数 ====================
    int Request_Basic_Info(std::shared_ptr<Device_class> Device_P);
    int Request_Voltage_Current_Info(std::shared_ptr<Device_class> Device_P);
    int Request_Capacity_Temp_Info(std::shared_ptr<Device_class> Device_P);
    int Request_Status_Info(std::shared_ptr<Device_class> Device_P);
    int Request_Cell_Voltage_Limit_Info(std::shared_ptr<Device_class> Device_P);
    int Request_Alarm_Protection_Info(std::shared_ptr<Device_class> Device_P);
    int Request_Cell_Voltage_Detail(std::shared_ptr<Device_class> Device_P, u8 group);
    int Request_UID_Info(std::shared_ptr<Device_class> Device_P);

    // ==================== 初始化函数 ====================
    /**
     * @brief 从YAML配置读取参数并初始化设备
     * @param Device 设备结构体
     * @param One_Node YAML配置节点
     * @return 执行结果 0成功 -1失败
     */
    int Get_BMS_Device_Data_From_Yaml_And_Init(std::shared_ptr<Device_class> Device, YAML::Node One_Node);

    // ==================== 接收解析函数 ====================
    /**
     * @brief BMS CAN帧解析入口函数
     * @param Can_Frame 接收到的CAN帧数据
     * @return 解析结果 0成功 -1非本设备数据/解析失败
     */
    int BMS_Frame_Analyze(volatile u8* Can_Frame);

    // ==================== 数据获取函数（const只读，线程安全） ====================
    BMS_Basic_Info Get_Basic_Info() const { return basic_info; }
    BMS_Voltage_Current_Info Get_Voltage_Current_Info() const { return voltage_current_info; }
    BMS_Capacity_Temp_Info Get_Capacity_Temp_Info() const { return capacity_temp_info; }
    BMS_Status_Info Get_Status_Info() const { return status_info; }
    BMS_Cell_Voltage_Limit_Info Get_Cell_Voltage_Limit_Info() const { return cell_voltage_limit_info; }
    BMS_Alarm_Protection_Info Get_Alarm_Protection_Info() const { return alarm_protection_info; }
    BMS_Cell_Voltage_Detail Get_Cell_Voltage_Detail(u8 group) const {
        return (group < 5) ? cell_voltage_detail[group] : BMS_Cell_Voltage_Detail();
    }
    BMS_UID_Info Get_UID_Info() const { return uid_info; }

    // ==================== 状态获取函数 ====================
    u16 Get_BMS_CAN_ID() const { return bms_can_id; }
    bool Is_Initialized() const { return is_initialized; }
    bool Is_Low_Power_Mode() const { return is_low_power_mode; }
    u32 Get_Last_Communication_Time() const { return last_communication_time; }

    //=================== 数据采集函数 ====================
    /**
     * @brief 启动数据采集线程
     * @param collection_interval_ms 采集周期（毫秒），默认1000ms
     * @return 启动结果 0成功 -1失败
     */
    int Start_Data_Collection(std::shared_ptr<Device_class> Device_P, uint32_t collection_interval_ms = 1000);

    /**
     * @brief 停止数据采集线程
     */
    void Stop_Data_Collection();

    /**
     * @brief 获取当前采集的电池数据（线程安全拷贝）
     * @return 电池数据汇总结构体
     */
    BMS_Consolidated_Data Get_Consolidated_Data() const;
    //=================== 数据采集函数 ====================

   private:
    /**
     * @brief 发送BMS指令（匹配协议请求-响应机制）
     * @param Device_P 设备结构体指针
     * @param cmd 功能指令（BMS_Cmd_Type）
     * @param data 额外数据（无额外数据填nullptr）
     * @param data_length 额外数据长度（无额外数据填0）
     * @return 执行结果 0成功 -1失败
     */
    int Send_BMS_Command(std::shared_ptr<Device_class> Device_P, BMS_Cmd_Type cmd, const u8* data, u8 data_length);
    /**
     * @brief 更新最后通信时间
     */
    void Update_Communication_Time();
    /**
     * @brief 检查是否需要唤醒设备
     * @return true需要唤醒 false不需要
     */
    bool Check_Need_Wake_Up() const;

    // ==================== 帧解析函数（100%匹配V2.2协议） ====================
    // 解析基本信息帧
    int Parse_Basic_Info_Frame(const u8* frame_data);
    // 解析电压电流信息帧
    int Parse_Voltage_Current_Info_Frame(const u8* frame_data);
    // 解析容量温度信息帧
    int Parse_Capacity_Temp_Info_Frame(const u8* frame_data);
    // 解析状态信息帧
    int Parse_Status_Info_Frame(const u8* frame_data);
    // 解析单体电压信息帧
    int Parse_Cell_Voltage_Limit_Frame(const u8* frame_data);
    // 解析告警信息帧
    int Parse_Alarm_Protection_Frame(const u8* frame_data);
    // 解析单体电压详细信息帧
    int Parse_Cell_Voltage_Detail_Frame(const u8* frame_data, BMS_Cmd_Type cmd);
    // 解析UID信息帧
    int Parse_UID_Info_Frame(const u8* frame_data);

    // ==================== 数据采集线程函数 ====================
    /**
     * @brief 数据采集线程主函数
     */
    void Data_Collection_Loop(std::shared_ptr<Device_class> Device_P);

    /**
     * @brief 更新整合数据结构体 (在解析函数和本函数内被调用)
     */
    void Update_Consolidated_Data();
    //=========================================================
};

#endif  // BATTERY_BMS_H_
