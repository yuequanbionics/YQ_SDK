/**
 * @file X_Hand_Protocol.hpp
 * @brief X-Hand M1 系列 CAN 控制协议（信手用户手册）
 *        协议：CAN 2.0A，1Mbps，标准帧，11 位 ID；字节序 Motorola（大端）
 *        控制：4 字节载荷 travel(16bit) + torque(16bit)
 *        每个实例对应一个电机
 */

#ifndef X_HAND_PROTOCOL_H_
#define X_HAND_PROTOCOL_H_

#include "HARDWARE_TOP.hpp"
#include "CAN.hpp"
#include <yaml-cpp/yaml.h>

/** 单帧控制数据：协议规定 4 字节，travel(0~65535)+torque(0~65535)，大端 */
constexpr int X_HAND_CTRL_PAYLOAD_LEN = 4;

#pragma pack(push, 1)
typedef struct {
    u16 Can_Id;
    u16 Len;
    u16 Flag;
    u8  Data[X_HAND_CTRL_PAYLOAD_LEN];  /** Byte0~1: travel MSB/LSB, Byte2~3: torque MSB/LSB */
} X_Hand_Ctrl_Frame;
#pragma pack(pop)

/** 行程：0=伸直，65535=弯曲；力矩协议暂未开放，可填 0 */
constexpr u16 TRAVEL_STRAIGHT = 0;
constexpr u16 TRAVEL_BENT     = 65535;

/**
 * X-Hand M1 协议驱动：每个实例对应一个电机
 * 发送命令时使用本实例的 can_id_
 */
class X_Hand_Protocol : private Robot_Hardware {
public:
    /** 本实例对应的 CAN ID */
    u16 Get_CanId() const { return can_id_; }

    /** 应用层显式设置 CAN ID */
    void Set_CanId(u16 id) { can_id_ = id; }

    /**
     * 设置本电机：travel(0~65535)，torque(0~65535)，使用实例的 CanId 发送
     */
    int Set_Motor(u16 travel, u16 torque = 0);

    /**
     * 按归一化角度设置本电机：0.0=伸直，1.0=弯曲
     */
    int Set_Motor_Angle(float angle_01, float torque_01 = 0.0f);

    // void Flush() const;

    /** 从 YAML 初始化：绑定设备，供设备类型 Init 回调使用 */
    int Get_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, const YAML::Node &Node);

    int X_Hand_Protocol_Frame_Analyze(volatile u8* Can_Frame);

private:
    int Send_One_Frame(u16 can_id, u16 travel, u16 torque);
    static void Pack_Travel_Torque_BigEndian(u8 data[4], u16 travel, u16 torque);

    /** 本实例设备，用于取 Head_Flag_CHX 决定发到主板哪路 CAN） */
    std::shared_ptr<Device_class> device_;
    u16 can_id_ = 0;
};

/** 设备类型名与 Init/CallBack/Delete 宏，供 Add_Device_Type 与 YAML DeviceType 使用 */
#define X_Hand_Protocol_Device_Type "X_Hand_Protocol_Device_Custom"

#define X_Hand_Protocol_Device_Init  ([](std::shared_ptr<Device_class> Device, YAML::Node *Node) -> int {\
        X_Hand_Protocol *proto = new X_Hand_Protocol();\
        Device->Device_Private_Class = (void *)proto;\
        return proto->Get_Device_Data_From_Yaml_And_Init(Device, Node ? *Node : YAML::Node());\
    })

#define X_Hand_Protocol_Device_CallBack_F  ([](std::shared_ptr<Device_class> Device, u8 *Msg) -> int {\
        return ((X_Hand_Protocol *)Device->Device_Private_Class)\
            ->X_Hand_Protocol_Frame_Analyze(Msg); \
    })

#define X_Hand_Protocol_Device_Delete_F  ([](void *Device_Private_Class) {\
        delete ((X_Hand_Protocol *)Device_Private_Class);\
        Device_Private_Class = nullptr;\
    })

#endif  // X_HAND_PROTOCOL_H_
