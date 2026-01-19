#ifndef BATTERY_BMS_H_
#define BATTERY_BMS_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"
#include "syst.hpp"

#pragma pack(push, 1)

// BMS基础数据结构
typedef struct {
    u16 Can_Id;  // CAN标识符
    u16 Len;     // 数据长度
    u16 Flag;    // 标志位
    u8 Data[8];  // 8字节数据域
} BMS_Data_Frame;

// BMS设备信息结构体
typedef struct {
    u8 cell_count;         // 电池单体数
    u8 ntc_count;          // NTC数量
    u16 nominal_capacity;  // 电池标称容量 (0.01AH)
    u16 software_version;  // BMS软件版本号 (0.01)
    u16 hardware_version;  // BMS硬件版本号 (0.01)
} BMS_Basic_Info;

// BMS电压电流信息结构体
typedef struct {
    u16 total_voltage;  // 总电压 (0.01V)
    s16 current;        // 电流 (0.01A 有符号数)
    u16 soc;            // SOC (0.1%)
    u16 soh;            // SOH (0.1%)
} BMS_Voltage_Current_Info;

// BMS容量温度信息结构体
typedef struct {
    u16 actual_capacity;   // 电池实际容量 (0.01AH)
    u16 cycle_count;       // 循环次数
    s16 cell_temperature;  // 电芯温度 (0.1℃ 有符号数)
    s16 mos_temperature;   // MOS温度 (0.1℃ 有符号数)
} BMS_Capacity_Temp_Info;

// BMS状态信息结构体
typedef struct {
    u8 discharge_mos_status;      // 放电MOS状态 (0断开1闭合)
    u8 charge_mos_status;         // 充电MOS状态 (0断开1闭合)
    u8 pre_discharge_mos_status;  // 预放电MOS状态 (0断开1闭合)
    u8 charger_connect_status;    // 充电器接入状态 (0断开1接入)
    u8 load_connect_status;       // 负载连接状态 (0断开1接入)
    u8 system_status;             // 系统状态 (0空闲1充电2放电)
} BMS_Status_Info;

// BMS单体电压信息结构体
typedef struct {
    u16 max_cell_voltage;  // 单体最高电压 (0.001V)
    u16 max_cell_index;    // 单体最高电压序号
    u16 min_cell_voltage;  // 单体最低电压 (0.001V)
    u16 min_cell_index;    // 单体最低电压序号
} BMS_Cell_Voltage_Info;

// BMS告警信息结构体
typedef struct {
    u8 cell_over_voltage_alarm;       // 单体过压告警 (0正常1告警)
    u8 cell_under_voltage_alarm;      // 单体欠压告警 (0正常1告警)
    u8 total_over_voltage_alarm;      // 总压过压告警 (0正常1告警)
    u8 total_under_voltage_alarm;     // 总压欠压告警 (0正常1告警)
    u8 discharge_over_current_alarm;  // 放电过流告警 (0正常1告警)
    u8 charge_over_current_alarm;     // 充电过流告警 (0正常1告警)
    u8 discharge_high_temp_alarm;     // 放电高温告警 (0正常1告警)
    u8 charge_high_temp_alarm;        // 充电高温告警 (0正常1告警)
} BMS_Alarm_Info;

// BMS保护信息结构体
typedef struct {
    u8 cell_over_voltage_protect;       // 单体过压保护 (0正常1保护)
    u8 cell_under_voltage_protect;      // 单体欠压保护 (0正常1保护)
    u8 total_over_voltage_protect;      // 总压过压保护 (0正常1保护)
    u8 total_under_voltage_protect;     // 总压欠压保护 (0正常1保护)
    u8 discharge_over_current_protect;  // 放电过流保护 (0正常1保护)
    u8 charge_over_current_protect;     // 充电过流保护 (0正常1保护)
    u8 discharge_high_temp_protect;     // 放电高温保护 (0正常1保护)
} BMS_Protection_Info;

// BMS故障信息结构体
typedef struct {
    u8 discharge_mos_fault;     // 放电MOS故障 (0正常1故障)
    u8 charge_mos_fault;        // 充电MOS故障 (0正常1故障)
    u8 eeprom_fault;            // EEPROM故障 (0正常1故障)
    u8 low_voltage_charge_ban;  // 低压禁止充电 (0正常1故障)
    u8 short_circuit_protect;   // 短路保护 (0正常1保护)
} BMS_Fault_Info;

// BMS单体电压详细结构体
typedef struct {
    u16 cell_voltage[4];  // 4节单体电压 (0.001V)
} BMS_Cell_Voltage_Detail;

// BMS UID信息结构体
typedef struct {
    u16 uid_low;   // UID低16位
    u16 uid_high;  // UID高16位
} BMS_UID_Info;

#pragma pack(pop)

// BMS命令枚举
enum BMS_Command_ID : uint16_t {
    BMS_CMD_CHANGE_ADDRESS = 0x7FF,      // 电池地址修改命令
    BMS_CMD_BASIC_INFO = 0x700,          // 基本信息 (0x70X, X为地址)
    BMS_CMD_VOLTAGE_CURRENT = 0x710,     // 电压电流信息 (0x71X)
    BMS_CMD_CAPACITY_TEMP = 0x720,       // 容量温度信息 (0x72X)
    BMS_CMD_STATUS = 0x730,              // 状态信息 (0x73X)
    BMS_CMD_CELL_VOLTAGE = 0x740,        // 单体电压信息 (0x74X)
    BMS_CMD_ALARM = 0x750,               // 告警信息 (0x75X)
    BMS_CMD_ALARM2 = 0x760,              // 告警信息2 (0x76X)
    BMS_CMD_PROTECTION = 0x770,          // 保护信息 (0x77X)
    BMS_CMD_PROTECTION2 = 0x780,         // 保护信息2 (0x78X)
    BMS_CMD_FAULT = 0x790,               // 故障信息 (0x79X)
    BMS_CMD_CELL_VOLTAGE_1_4 = 0x7A0,    // 1-4节单体电压 (0x7AX)
    BMS_CMD_CELL_VOLTAGE_5_8 = 0x7B0,    // 5-8节单体电压 (0x7BX)
    BMS_CMD_CELL_VOLTAGE_9_12 = 0x7C0,   // 9-12节单体电压 (0x7CX)
    BMS_CMD_CELL_VOLTAGE_13_16 = 0x7D0,  // 13-16节单体电压 (0x7DX)
    BMS_CMD_UID = 0x7E0                  // UID信息 (0x7EX)
};

// 设备初始化宏定义
#define Battery_BMS_Device_Init                                                       \
    [](shared_ptr<Device_Struct> Device, YAML::Node* Node) -> int {                   \
        BMS_Protocol* bms_device = new BMS_Protocol();                                \
        Device->Device_Private_Class = (void*)bms_device;                             \
        if (Node != nullptr)                                                          \
            return bms_device->Get_BMS_Device_Data_From_Yaml_And_Init(Device, *Node); \
        else                                                                          \
            return 0;                                                                 \
    }

// 设备回调函数宏定义
#define Battery_BMS_Device_CallBack                                           \
    [](void* Device_Private_Class, volatile u8* Msg) -> int {                 \
        return ((BMS_Protocol*)Device_Private_Class)->BMS_Frame_Analyze(Msg); \
    }

// 设备删除函数宏定义
#define Battery_BMS_Device_Delete                     \
    [](void* Device_Private_Class) {                  \
        delete ((BMS_Protocol*)Device_Private_Class); \
        Device_Private_Class = nullptr;               \
    }

class BMS_Protocol : private Robot_Hardware {
   private:
    // BMS设备地址 (0-14, 0为默认地址)
    u8 bms_address;

    // BMS数据缓存
    BMS_Basic_Info basic_info;
    BMS_Voltage_Current_Info voltage_current_info;
    BMS_Capacity_Temp_Info capacity_temp_info;
    BMS_Status_Info status_info;
    BMS_Cell_Voltage_Info cell_voltage_info;
    BMS_Alarm_Info alarm_info;
    BMS_Alarm_Info alarm_info2;
    BMS_Protection_Info protection_info;
    BMS_Protection_Info protection_info2;
    BMS_Fault_Info fault_info;
    BMS_Cell_Voltage_Detail cell_voltage_detail[4];  // 0:1-4, 1:5-8, 2:9-12, 3:13-16
    BMS_UID_Info uid_info;

    // 设备状态
    bool is_initialized;
    bool is_low_power_mode;
    u32 last_communication_time;
    std::mutex data_mutex;

   public:
    std::shared_ptr<Device_Struct> s_device;

    /**
     * @brief 构造函数
     */
    BMS_Protocol(void);

    /**
     * @brief 析构函数
     */
    ~BMS_Protocol();

    /**
     * @brief 唤醒BMS设备
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Wake_Up_BMS(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 修改BMS设备地址
     * @param Device_P 设备结构体指针
     * @param new_address 新地址 (1-14)
     * @return 执行结果
     */
    int Change_BMS_Address(std::shared_ptr<Device_Struct> Device_P, u8 new_address);

    /**
     * @brief 请求BMS基本信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Basic_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求电压电流信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Voltage_Current_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求容量温度信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Capacity_Temp_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求状态信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Status_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求单体电压信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Cell_Voltage_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求告警信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Alarm_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求保护信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Protection_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求故障信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_Fault_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 请求单体电压详细信息
     * @param Device_P 设备结构体指针
     * @param group 组号 (0:1-4, 1:5-8, 2:9-12, 3:13-16)
     * @return 执行结果
     */
    int Request_Cell_Voltage_Detail(std::shared_ptr<Device_Struct> Device_P, u8 group);

    /**
     * @brief 请求UID信息
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Request_UID_Info(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 从YAML配置文件读取BMS参数并初始化
     * @param Device 设备结构体
     * @param One_Node YAML节点
     * @return 执行结果
     */
    int Get_BMS_Device_Data_From_Yaml_And_Init(std::shared_ptr<Device_Struct> Device, YAML::Node One_Node);

    /**
     * @brief BMS帧数据解析回调函数
     * @param Can_Frame CAN帧数据
     * @return 解析结果
     */
    int BMS_Frame_Analyze(volatile u8* Can_Frame);

    // 数据获取函数
    BMS_Basic_Info Get_Basic_Info() const { return basic_info; }
    BMS_Voltage_Current_Info Get_Voltage_Current_Info() const { return voltage_current_info; }
    BMS_Capacity_Temp_Info Get_Capacity_Temp_Info() const { return capacity_temp_info; }
    BMS_Status_Info Get_Status_Info() const { return status_info; }
    BMS_Cell_Voltage_Info Get_Cell_Voltage_Info() const { return cell_voltage_info; }
    BMS_Alarm_Info Get_Alarm_Info() const { return alarm_info; }
    BMS_Alarm_Info Get_Alarm_Info2() const { return alarm_info2; }
    BMS_Protection_Info Get_Protection_Info() const { return protection_info; }
    BMS_Protection_Info Get_Protection_Info2() const { return protection_info2; }
    BMS_Fault_Info Get_Fault_Info() const { return fault_info; }
    BMS_Cell_Voltage_Detail Get_Cell_Voltage_Detail(u8 group) const {
        return (group < 4) ? cell_voltage_detail[group] : BMS_Cell_Voltage_Detail();
    }
    BMS_UID_Info Get_UID_Info() const { return uid_info; }

    /**
     * @brief 获取BMS设备地址
     * @return 设备地址
     */
    u8 Get_BMS_Address() const { return bms_address; }

    /**
     * @brief 获取设备初始化状态
     * @return 初始化状态
     */
    bool Is_Initialized() const { return is_initialized; }

    /**
     * @brief 获取低功耗模式状态
     * @return 低功耗模式状态
     */
    bool Is_Low_Power_Mode() const { return is_low_power_mode; }

    /**
     * @brief 获取最后通信时间
     * @return 最后通信时间
     */
    u32 Get_Last_Communication_Time() const { return last_communication_time; }

   private:
    /**
     * @brief 发送BMS命令
     * @param Device_P 设备结构体指针
     * @param command_id 命令ID
     * @param data 数据内容
     * @param data_length 数据长度
     * @return 执行结果
     */
    int Send_BMS_Command(std::shared_ptr<Device_Struct> Device_P, u16 command_id, const u8* data, u8 data_length);

    /**
     * @brief 解析基本信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Basic_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析电压电流信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Voltage_Current_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析容量温度信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Capacity_Temp_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析状态信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Status_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析单体电压信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Cell_Voltage_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析告警信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Alarm_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析保护信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Protection_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析故障信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_Fault_Info_Frame(const u8* frame_data);

    /**
     * @brief 解析单体电压详细信息帧
     * @param frame_data 帧数据
     * @param command_id 命令ID
     * @return 解析结果
     */
    int Parse_Cell_Voltage_Detail_Frame(const u8* frame_data, u16 command_id);

    /**
     * @brief 解析UID信息帧
     * @param frame_data 帧数据
     * @return 解析结果
     */
    int Parse_UID_Info_Frame(const u8* frame_data);

    /**
     * @brief 更新通信时间
     */
    void Update_Communication_Time();

    /**
     * @brief 检查是否需要唤醒
     * @return 是否需要唤醒
     */
    bool Check_Need_Wake_Up() const;

    /**
     * @brief 计算命令ID (根据设备地址)
     * @param base_command_id 基础命令ID
     * @return 完整的命令ID
     */
    u16 Calculate_Command_ID(u16 base_command_id) const;
};

#endif  // BATTERY_BMS_H_