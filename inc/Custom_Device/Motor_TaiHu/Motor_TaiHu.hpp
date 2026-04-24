#ifndef MOTOR_TAIHU_H_
#define MOTOR_TAIHU_H_
#include "syst.hpp"
#include "Motor.hpp"
#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"

#pragma pack(push, 1) 

typedef struct
{
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 Cmd;
	int32_t Data;
} CMD_Data_TaiHu;

typedef struct
{
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	int64_t Data;
} CMD_callback_TaiHu;

typedef struct
{ 
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 Cmd;
} CMD_Data_TaiHu_Read;

#pragma pack(pop) // 恢复默认的对齐设置

enum Command : uint8_t
{
    CMD_STOP = 0x02,              // 失能           
    CMD_GET_POS_VALUE = 0x08,     // 位置读取
    CMD_FORWARD_VEL = 0X24,       // 正转速度控制
    CMD_BACKWARD_VEL = 0X25,      // 反转速度控制
    CMD_SET_OFFSET = 0x53,        // 设置零点
    CMD_GET_OFFSET = 0x54,        // 获取零点
    CMD_SET_POS_VALUE = 0x1E,     // 位置控制
    CMD_SET_CAN_ID = 0x2E,        // 设置CAN ID
    CMD_SAVE_TO_FLASH = 0x0E,     // 保存到flash
    CMD_SET_VEL_KP = 0x29,        // 设置速度环Kp
    CMD_SET_VEL_KI = 0x2A,        // 设置速度环Ki
    CMD_SET_POS_KP = 0x2B,        // 设置位置环Kp
    CMD_SET_POS_KD = 0x2D,        // 设置位置环Kd
    CMD_GET_CUR_VEL_POS = 0x41,   // 获取电流速度位置反馈
    CMD_SET_CUR_KP = 0x83,        // 设置电流环Kp
    CMD_SET_CUR_KI = 0x84         // 设置电流环Ki

};

#define Motor_Device_Init_TaiHu  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Motor_TaiHu *One_Motor = new Motor_TaiHu();\
                                Device->Device_Private_Class = (void *)One_Motor;\
                                if (Node != nullptr)\
                                    return One_Motor->Get_Motor_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Motor_Device_CallBack_TaiHu    [](shared_ptr<Device_class> Device, u8 *Msg) -> int\
                                    {\
                                        return ((Motor_TaiHu *)Device->Device_Private_Class)->Motor_Top_Frame_Analyze(Msg);\
                                    }\

#define Motor_Device_Delete_TaiHu  [](void *Device_Private_Class)\
                                {\
                                    delete ((Motor_TaiHu *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\


class Motor_TaiHu : private Robot_Hardware
{
private:
    int32_t target_P = 0;           // 目标位置
    int32_t target_V = 0;           // 目标速度
    int32_t target_F = 0;           // 目标电流
    int32_t pos_offset = 0;          // 位置偏置值(Cnt)
    float reduction_ratio = 0.0f;    // 减速比
    



public:
    std::shared_ptr<Device_class> s_device;

    /**
     * @brief 电机使能/失能
     * @param EN 1使能(无效，任意控制指令都可以使电机使能），0失能
    */
    int Motor_EN(shared_ptr<Device_class> Device_P, int EN);

    /**
     * @brief 设置电机位置偏置值 调整电机反馈的 0 位置在机械上对应的点位。
     * @param offest 偏置值(Cnt)
    */
    int Set_Pos_Offset(shared_ptr<Device_class> Device_P, int32_t offest);


      /**
     * @brief 获取电机位置偏置值
     * @param offest 偏置值(Cnt)
    */
    int Get_Pos_Offset(shared_ptr<Device_class> Device_P, int32_t *offest);


    /**
     * @brief 设置电机为位置模式，并设置当前位置
     * @param Rad 位置值，单位度
     * @param Speed_Rad_S （无效）
     * @param Force_N （无效）
     * @param P_N_Rad （无效）
     * @param D_N_Rad_s （无效）
    */
    int Send_MIT_PD_Control_Data(shared_ptr<Device_class> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);


    /**
     * @brief 读取电机反馈的位置
     * @param P 位置，单位度
     * @param V 速度(无效）
     * @param F 力矩(无效）
    */
    int Get_Motor_FB_Data(shared_ptr<Device_class> Device_P, float *P, float *V, float *F);


      /**
     * @brief Yaml配置文件读取电机参数并初始化
    */
    int Get_Motor_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);


      /**
     * @brief 回调函数，CAN帧数据解析
    */
    int Motor_Top_Frame_Analyze(volatile u8 *Can_Frame);

    // /**
    //  * @brief 设置电机CAN ID 保存到flash
    //  * @param new_id 新的CAN ID
    //  * @param save_to_flash 是否保存到flash  需要在停止状态下执行
    // */

    // int Set_Can_Id(shared_ptr<Device_Struct> Device_P, u32 new_id);

    /**
     * @brief 设置位置环KP
     * @param kp 比例增益
     */

    int Set_Pos_KP(shared_ptr<Device_class> device, int kp);

    /**
     * @brief 设置位置环KD
     * @param kd 微分增益
     */

    int Set_Pos_KD(shared_ptr<Device_class> device, int kd);

     /**
     * @brief 设置速度环KP
     * @param kp 比例增益
     */

    int Set_Vel_KP(shared_ptr<Device_class> device, int kp);

    /**
     * @brief 设置速度环KI
     * @param ki 积分增益
     */

    int Set_Vel_KI(shared_ptr<Device_class> device, int ki);

    /**
     * @brief 设置电流环KD
     * @param kd 微分增益
     */

    int Set_Cur_KP(shared_ptr<Device_class> device, int kp);

     /**
     * @brief 设置电流环Ki
     * @param ki 积分增益
     */

    int Set_Cur_KI(shared_ptr<Device_class> device, int ki);




    /**
     * @brief 设置电机在位置模式下的正向速度
     * @param Vel_Rad_S 速度值，单位度每秒
    */

    int Set_Forward_Vel(shared_ptr<Device_class> Device_P, float Vel_Rad_S);

    /**
     * @brief 设置电机在位置模式下的反向速度
     * @param Vel_Rad_S 速度值，单位度每秒
    */
    int Set_Backward_Vel(shared_ptr<Device_class> Device_P, float Vel_Rad_S);

    /**
     * @brief 保存设置参数到flash
    */

    int Save_To_Flash(shared_ptr<Device_class> device);





};

#endif
