//
// Created by lwz on 2025/11/13.
//

#ifndef RHS_MOTOR_DM_J4310_2EC_HPP
#define RHS_MOTOR_DM_J4310_2EC_HPP
#include "HARDWARE_TOP.hpp"

/**
 *  DM-J4310-2EC 电机基本信息
 *
 *  1. 工作电压: 24V
 *  2.
 *
 */

#define DaMiao_Custom_Motor_Device_Init  [](const std::shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int \
                                        { \
                                            Motor_DaMiao *One_DaMiao_Motor = new Motor_DaMiao(); \
                                            Device->Device_Private_Class = (void *)One_DaMiao_Motor; \
                                            if (Node != nullptr) \
                                                return One_DaMiao_Motor->Get_DaMiao_Motor_Data_From_Yaml_And_Init(Device, *Node); \
                                            else \
                                                return 0; \
                                        }

#define DaMiao_Custom_Motor_Device_CallBack_F [](void *Device_Private_Class, volatile u8 *Msg) -> int \
                                             { \
                                                 return ((Motor_DaMiao *)Device_Private_Class)->DaMiao_Custom_Motor_Top_Frame_Analyze(Msg); \
                                             }

#define DaMiao_Custom_Motor_Device_Delete_F [](void *Device_Private_Class) \
                                           { \
                                               delete ((Motor_DaMiao *)Device_Private_Class); \
                                               Device_Private_Class = nullptr; \
                                           }

#define MIT_MODE            0x000
#define POS_MODE            0x100
#define SPD_MODE            0x200
#define PSI_MODE            0x300

#define KP_MIN              0.0f
#define KP_MAX              500.0f
#define KD_MIN              0.0f
#define KD_MAX              5.0f

#define DM_FB_ID_OFFSET     16          // 反馈报文 can id = 电机 can id + 这个偏移
#define DM_FB_DATA_OFFSET   6           // 解析反馈包时, Can_Frame 中包含了 Can_Id, Len, Flag 共 6B

#define DAMIAO_CMD_DATA_LEN 14 // Head_Len + 8

/* motor mode */
typedef enum {
    mit_mode = 1,
    pos_mode = 2,
    spd_mode = 3,
    psi_mode = 4
} mode_e;

inline std::map<std::string, uint8_t> DaMiao_Config_Map = {
    {"mit_mode", mit_mode},
    {"pos_mode", pos_mode},
    {"spd_mode", spd_mode},
    {"psi_mode", psi_mode},
};

/* 达妙电机 UDP 数据包 */
typedef struct
{
    /* UDP message head */
    u16 Can_Id;
    u16 Len;
    u16 Flag;

    /* Custom data */
    u8  Data0;
    u8  Data1;
    u8  Data2;
    u8  Data3;
    u8  Data4;
    u8  Data5;
    u8  Data6;
    u8  Data7;
} DaMiao_CMD_Data;

/* 电机参数 */
typedef struct
{
    uint8_t read_flag;
    uint8_t write_flag;
    uint8_t save_flag;

    float    UV_Value;     // 低压保护值
    float    KT_Value;     // 扭矩系数
    float    OT_Value;     // 过温保护值
    float    OC_Value;     // 过流保护值
    float    ACC;          // 加速度
    float    DEC;          // 减速度
    float    MAX_SPD;      // 最大速度
    uint32_t MST_ID;       // 反馈 ID
    uint32_t ESC_ID;       // 接收 ID
    uint32_t TIMEOUT;      // 超时警报时间
    uint32_t cmode;        // 控制模式
    float    Damp;         // 电机粘滞系数
    float    Inertia;      // 电机转动惯量
    uint32_t hw_ver;       // 保留
    uint32_t sw_ver;       // 软件版本号
    uint32_t SN;           // 保留
    uint32_t NPP;          // 电机极对数
    float    Rs;           // 电阻
    float    Ls;           // 电感
    float    Flux;         // 磁链
    float    Gr;           // 齿轮减速比
    float    PMAX;         // 位置映射范围
    float    VMAX;         // 速度映射范围
    float    TMAX;         // 扭矩映射范围
    float    I_BW;         // 电流环控制带宽
    float    KP_ASR;       // 速度环 Kp
    float    KI_ASR;       // 速度环 Ki
    float    KP_APR;       // 位置环 Kp
    float    KI_APR;       // 位置环 Ki
    float    OV_Value;     // 过压保护值
    float    GREF;         // 齿轮力矩效率
    float    Deta;         // 速度环阻尼系数
    float    V_BW;         // 速度环滤波带宽
    float    IQ_cl;        // 电流环增强系数
    float    VL_cl;        // 速度环增强系数
    uint32_t can_br;       // CAN 波特率代码
    uint32_t sub_ver;      // 子版本号
    float    u_off;        // u 相偏置
    float    v_off;        // v 相偏置
    float    k1;           // 补偿因子 1
    float    k2;           // 补偿因子 2
    float    m_off;        // 角度偏移
    float    dir;          // 方向
    float    p_m;          // 电机位置
    float    x_out;        // 输出轴位置
} esc_inf_t;


/* 电机回传信息结构体 */
typedef struct
{
    int id;
    int state;      // ERR 字段, 即电机状态信息
    int p_int;
    int v_int;
    int t_int;      // 电机扭矩
    int kp_int;
    int kd_int;
    float pos;      // 电机位置
    float vel;      // 电机速度
    float tor;
    float Kp;
    float Kd;
    float Tmos;     // 驱动上 MOS 的平均温度
    float Tcoil;    // 电机内部线圈的平均温度
} motor_fbpara_t;

/* 电机参数设置结构体 */
typedef struct
{
    uint8_t mode;
    float pos_set;
    float vel_set;
    float tor_set;
    float cur_set;
    float kp_set;
    float kd_set;
} motor_ctrl_t;

/* 电机对象 */
typedef struct
{
    uint16_t id;
    uint16_t mst_id;
    motor_fbpara_t para;
    motor_ctrl_t ctrl;
    esc_inf_t tmp;
} motor_t;

/* 电机反馈数据 */
typedef union
{
    float f_val;
    uint32_t u_val;
    uint8_t b_val[4];
}float_type_u;


class Motor_DaMiao : Robot_Hardware
{
public:
    int Get_DaMiao_Motor_Data_From_Yaml_And_Init(std::shared_ptr<Device_Struct> Device, const YAML::Node& One_Node);
    int DaMiao_Custom_Motor_Top_Frame_Analyze(volatile u8* Can_Frame);

    void Send_MIT_PD_Control_Data(const std::shared_ptr<Device_Struct>& Device,
                                  float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    int Get_Motor_FB_Data(const shared_ptr<Device_Struct> &Device, float *P, float *V, float *F, float temp[2], u16 *error);


    void DaMiao_Motor_Enable(const std::shared_ptr<Device_Struct>& Device, const motor_t* motor);
    void DaMiao_Motor_Disable(const std::shared_ptr<Device_Struct>& Device, motor_t* motor);
    void DaMiao_Motor_Clear_Err(const std::shared_ptr<Device_Struct>& Device, const motor_t* motor);
    void DaMiao_Motor_Ctrl_Send(const std::shared_ptr<Device_Struct>& Device, const motor_t *motor);
    static void DaMiao_Motor_Clear_para(motor_t* motor);

private:
    void damiao_motor_fbdata(motor_t* motor, const volatile uint8_t* rx_data);
    void enable_motor_mode(const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, uint16_t mode_id);
    void disable_motor_mode(const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, uint16_t mode_id);
    void save_pos_zero( const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, uint16_t mode_id);
    void clear_err(const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, uint16_t mode_id);

    void mit_ctrl(std::shared_ptr<Device_Struct> Device, const motor_t *motor,                        \
                                                                uint16_t motor_id, float pos, float vel,    \
                                                                float    kp,       float kd,  float tor);
    void pos_ctrl(const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, float pos, float vel);
    void spd_ctrl(const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, float vel);
    void psi_ctrl(const std::shared_ptr<Device_Struct>& Device, uint16_t motor_id, float pos, float vel, float cur);

    void read_motor_data(const std::shared_ptr<Device_Struct>& Device, uint16_t id, uint8_t rid);
    void read_motor_ctrl_fbdata(const std::shared_ptr<Device_Struct>& Device);

    void save_motor_data(const std::shared_ptr<Device_Struct>& Device, uint16_t id, uint8_t rid);
    void write_motor_data( const std::shared_ptr<Device_Struct>& Device,
                           uint16_t id, uint8_t rid, uint8_t d0,
                           uint8_t d1,  uint8_t d2, uint8_t d3);

    static DaMiao_CMD_Data* cmd_data_f(u16 ID, u16 Flag,                       \
                                       u8 data0, u8 data1, u8 data2, u8 data3, \
                                       u8 data4, u8 data5, u8 data6, u8 data7, \
                                       DaMiao_CMD_Data* DaMiao_CMD_Data);

    static void receive_motor_data(motor_t* motor, const uint8_t* data);

    static int float_to_uint(float x_float, float x_min, float x_max, int bits);
    float uint_to_float(int x_int, float x_min, float x_max, int bits);
    float degrees_to_radians(float degrees);
};

#endif //RHS_MOTOR_DM_J4310_2EC_HPP