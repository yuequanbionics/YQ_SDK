//
// Created by lwz on 2025/12/4.
//

#ifndef RHS_FEETECH_SCS0009_HPP
#define RHS_FEETECH_SCS0009_HPP

#include "HARDWARE_TOP.hpp"

#define Feetech_Custom_Servo_Device_Init [](const std::shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int \
                                         { \
                                             Servo_SCS0009 *One_Feetech_Servo = new Servo_SCS0009(); \
                                             Device->Device_Private_Class = (void *)One_Feetech_Servo; \
                                             if (Node != nullptr) \
                                                 return One_Feetech_Servo->Get_Feetech_Servo_Data_From_Yaml_And_Init(Device, *Node); \
                                             else \
                                                 return 0; \
                                         }

#define Feetech_Custom_Servo_Device_CallBack_F [](void *Device_Private_Class, volatile u8 *Msg) -> int \
                                               { \
                                               return ((Servo_SCS0009 *)Device_Private_Class)->Feetech_Custom_Motor_Top_Frame_Analyze(Msg); \
                                               }

#define Feetech_Custom_Servo_Device_Delete_F [](void *Device_Private_Class) \
                                             { \
                                                delete ((Servo_SCS0009 *)Device_Private_Class); \
                                                Device_Private_Class = nullptr; \
                                             }

#define BROADCAST_ID 0xFE

/* 指令号 */
#define INST_PING 0x01
#define INST_READ 0x02
#define INST_WRITE 0x03
#define INST_REG_WRITE 0x04
#define INST_REG_ACTION 0x05
#define INST_SYNC_READ 0x82
#define INST_SYNC_WRITE 0x83
#define INST_RESET 0x0A
// #define INST_OFSCAL 0x0B


//内存表定义
//-------EPROM(只读)--------
#define SCSCL_MODEL_L               0x03
#define SCSCL_MODEL_H               0x04

//-------EPROM(读写)--------
#define SCSCL_ID                    0x05
#define SCSCL_BAUD_RATE             0x06
#define SCSCL_MIN_ANGLE_LIMIT_L     0x09
#define SCSCL_MIN_ANGLE_LIMIT_H     0x0A
#define SCSCL_MAX_ANGLE_LIMIT_L     0x0B
#define SCSCL_MAX_ANGLE_LIMIT_H     0x0C
#define SCSCL_CW_DEAD               0x1A    // 正向不灵敏区, 步
#define SCSCL_CCW_DEAD              0x1B    // 负向不灵敏区, 步

//-------SRAM(读写)--------
#define SCSCL_TORQUE_ENABLE         0x28    // 扭矩开关, 写 0: 关闭扭力输出/自由状态; 1: 打开扭力输出; 2: 阻尼状态
#define SCSCL_GOAL_POSITION_L       0x2A    // 目标位置, 步, [1～1023]
#define SCSCL_GOAL_POSITION_H       0x2B
#define SCSCL_GOAL_TIME_L           0x2C    // 运行时间, 1ms, 从当前位置到目标位置的运动时间, 当运行速度为 0 时此参数生效;
#define SCSCL_GOAL_TIME_H           0x2D
#define SCSCL_GOAL_SPEED_L          0x2E    // 运行速度, 步/s, [0~3400]
#define SCSCL_GOAL_SPEED_H          0x2F
#define SCSCL_LOCK                  0x30    // 锁标志, 写 0 关闭写入锁, 写入 EPROM 地址的值掉电保存; 写 1 打开写入锁, 写入 EPROM 地址的值掉电不保存

//-------SRAM(只读)--------
#define SCSCL_PRESENT_POSITION_L    0x38
#define SCSCL_PRESENT_POSITION_H    0x39
#define SCSCL_PRESENT_SPEED_L       0x3A
#define SCSCL_PRESENT_SPEED_H       0x3B
#define SCSCL_PRESENT_LOAD_L        0x3C    // 当前负载, 当前控制输出驱动电机的电压占空比, BIT10 为方向位
#define SCSCL_PRESENT_LOAD_H        0x3D
#define SCSCL_PRESENT_VOLTAGE       0x3E    // 当前电压, 0.1V
#define SCSCL_PRESENT_TEMPERATURE   0x3F    // 当前温度, °C
#define SCSCL_MOVING                0x42    // 移动标志, 舵机在运动时标志为 1; 舵机到达目标并且停止时为 0; 舵机无更新目标位置保持为 0
// #define SCSCL_PRESENT_CURRENT_L 69
// #define SCSCL_PRESENT_CURRENT_H 70

/* 命令/地址 map, 用于 log 打印 */
inline std::map<uint16_t, std::string> Servo_Cmd_Map = {
    {INST_PING, "Ping"},
    {INST_READ, "Read"},
    {INST_WRITE, "Write"},
    {INST_REG_WRITE, "Reg_Write"},
    {INST_REG_ACTION, "Reg_Action"},
    {INST_SYNC_READ, "Sync_Read"},
    {INST_SYNC_WRITE, "Sync_Write"},
    {INST_RESET, "Reset"},

    {SCSCL_PRESENT_POSITION_L, "Position"},
    {SCSCL_PRESENT_SPEED_L, "Speed"},
    {SCSCL_PRESENT_VOLTAGE, "Voltage"},
    {SCSCL_PRESENT_TEMPERATURE, "Temperature"},
    {SCSCL_MOVING, "Moving_Flag"}
};

/* UDP 数据结构 */
typedef struct
{
    /* UDP message head */
    u16 Can_Id;
    u16 Len;
    u16 Flag;

    u8 Data[1024];
} Servo_CMD_Data;

/* 舵机数据 */
typedef struct
{
    uint8_t  Id;
    uint8_t  State;
    uint8_t  Temperature;
    uint8_t  Moving;
    uint16_t Pos;
    uint16_t Pos_Init;      // 舵机上电初始位置
    uint16_t Vel;
}Servo_Info;

class Servo_SCS0009 : Robot_Hardware
{
public:
    static int Get_Feetech_Servo_Data_From_Yaml_And_Init(std::shared_ptr<Device_Struct> Device, const YAML::Node& One_Node);
    static int Feetech_Custom_Motor_Top_Frame_Analyze(const volatile u8* Can_Frame);
    void WritePos(const std::shared_ptr<Device_Struct>& Device, uint8_t servo_id, uint8_t write_type, uint16_t pos, uint16_t vel);
    void Reset(const std::shared_ptr<Device_Struct>& Device, uint8_t servo_id);
    void Ping(const std::shared_ptr<Device_Struct>& Device, uint8_t servo_id);
    void Write(const std::shared_ptr<Device_Struct>& Device, uint8_t servo_id, uint8_t write_type, uint8_t param_num, const uint8_t* write_data);
    void RegAction(const std::shared_ptr<Device_Struct>& Device);
    void SyncWrite(const std::shared_ptr<Device_Struct>& Device, uint8_t servo_num, uint8_t param_num, uint8_t* write_data);
    void Read(const shared_ptr<Device_Struct>& Device, uint8_t servo_id, uint8_t read_addr, uint8_t read_len);

    void Get_Finger_Start_Pos(const shared_ptr<Device_Struct>& Device);
    void Restore(const shared_ptr<Device_Struct>& Device);
    void Movement_1_Yeah(const shared_ptr<Device_Struct>& Device);
    void Movement_2_Point(const shared_ptr<Device_Struct>& Device);
    void Movement_3_Fist(const shared_ptr<Device_Struct>& Device);
    void Movement_4_Pinch(const shared_ptr<Device_Struct>& Device);

private:
    static void Host2SCS(uint8_t* DataL, uint8_t* DataH, int Data);
    static int SCS2Host(uint8_t DataL, uint8_t DataH);
};

#endif //RHS_FEETECH_SCS0009_HPP