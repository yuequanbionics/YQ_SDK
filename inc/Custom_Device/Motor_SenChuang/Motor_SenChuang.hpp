#ifndef MOTOR_SENCHUANG_H_
#define MOTOR_SENCHUANG_H_
#include "syst.hpp"
#include "Motor.hpp"
#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"

#pragma pack(push, 1) 

typedef struct
{
	// Head
	uint16_t Can_Id;
	uint16_t Len;
	uint16_t Flag;

	// DATA
    uint8_t  sdo_cmd;
    uint16_t sdo_index;
    uint8_t  sdo_subindex;
    uint32_t sdo_data;
} CMD_Data_Sc;

typedef struct
{
	// Head
	uint16_t Can_Id;
	uint16_t Len;
	uint16_t Flag;

	// DATA

    uint16_t bc_data;
} CMD_Data_Bc;


#pragma pack(pop) // 恢复默认的对齐设置

enum e_sdo_cmd : uint8_t
{

    //主站→从站
    CMD_WRITE_4_BYTE = 0x23,   //写 4 字节
    CMD_WRITE_3_BYTE = 0x27,   //写 3 字节
    CMD_WRITE_2_BYTE = 0x2B,   //写 2 字节
    CMD_WRITE_1_BYTE = 0x2F,   //写 1 字节

    CMD_READ_4_BYTE = 0x40,    //读 4 字节
    // CMD_READ_3_BYTE = 0x47,    //读 3 字节
    // CMD_READ_2_BYTE = 0x4B,    //读 2 字节
    // CMD_READ_1_BYTE = 0x4F,    //读 1 字节



    //从站→主站
    CMD_READ_RES_4_BYTE = 0x43,    //读应答：4 字节
    CMD_READ_RES_3_BYTE = 0x47,    //读应答：3 字节
    CMD_READ_RES_2_BYTE = 0x4B,    //读应答：2 字节
    CMD_READ_RES_1_BYTE = 0x4F,    //读应答：1 字节
    CMD_WRITE_OK = 0x60        //写成功应答

};

enum e_sdo_index : uint16_t
{


    CMD_CONTROL_WORD = 0X6040,  //控制字
    CMD_RUN_MODE = 0x6060,      //运行模式
    CMD_ACTUAL_POS = 0X6064,    //实际位置值
    CMD_ACTUAL_VEL = 0X606C,    //实际速度值
    CMD_TARGRT_POS = 0x607A,    //设置目标位置
    CMD_PRO_VEL = 0x6081,       //设置轮廓速度
    CMD_PRO_ACC = 0x6083,       //设置加速度
    CMD_PRO_DEC = 0x6084,       //设置减速度
    CMD_GEAR_RATIO = 0x6093,    //设置电子齿轮比
    CMD_TARGET_VEL = 0x60FF     //目标速度
};

enum e_sdo_subindex : uint8_t
{
    CMD_NUM_EN = 0X00,         //条目数
    CMD_CTOS = 0X01,           //SDO接收
    CMD_STOC = 0X02,           //SDO发送
    CMD_ENABLE = 0X03          //禁用/使能SDO
};



#define Motor_Device_Init_SenChuang  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Motor_SenChuang *One_Motor = new Motor_SenChuang();\
                                Device->Device_Private_Class = (void *)One_Motor;\
                                if (Node != nullptr)\
                                    return One_Motor->Get_Motor_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Motor_Device_CallBack_SenChuang    [](shared_ptr<Device_class> Device, u8 *Msg) -> int\
                                    {\
                                        return ((Motor_SenChuang *)Device->Device_Private_Class)->Motor_Top_Frame_Analyze(Msg);\
                                    }\

#define Motor_Device_Delete_SenChuang  [](void *Device_Private_Class)\
                                {\
                                    delete ((Motor_SenChuang *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\


class Motor_SenChuang : private Robot_Hardware
{
private:
    int32_t m_target_P = 0;            // 目标位置
    int32_t m_target_V = 0;            // 目标速度

    uint32_t m_pos_pro_vel = 1000;     //位置模式轮廓速度
    uint32_t m_pos_pro_acc = 100;      //位置模式轮廓加速度
    uint32_t m_pos_pro_dec = 100;      //位置模式轮廓减速度
    uint32_t m_gear_num = 1;           //齿轮比分子
    uint32_t m_gear_Den = 1;           //齿轮比分母

    uint32_t m_vel_pro_acc = 1000;     //速度模式轮廓加速度
    uint32_t m_vel_pro_dec = 1000;     //速度模式轮廓减速度
    
    



public:
    std::shared_ptr<Device_class> s_device;

     /**
     * @brief 上电初始化 依次写入 0x06，0x07，0x0F
    */
    int PowerOn_Init(shared_ptr<Device_class> Device_P);

    /**
     * @brief 电机使能/失能
     * @param EN 1使能，0失能
    */
    int Motor_EN(shared_ptr<Device_class> Device_P, int EN);


     /**
     * @brief 设置位置模式
    */
    int Set_Pos_Mode(shared_ptr<Device_class> Device_P);

     /**
     * @brief 设置速度模式
    */
    int Set_Vel_Mode(shared_ptr<Device_class> Device_P);


    /**
     * @brief 设置电机为位置模式，并设置当前位置
     * @param Rad 位置值，单位度
     * @param Speed_Rad_S 速度值，单位度/秒
     * @param Force_N 力矩值，单位牛米
     * @param P_N_Rad 位置环KP（无效）
     * @param D_N_Rad_s 位置环KD（无效）
    */
    int Send_MIT_PD_Control_Data(shared_ptr<Device_class> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);

      /**
     * @brief 读取电机反馈的位置
     * @param P 位置，单位度
     * @param V 速度(无效）
     * @param F 力矩(无效）
    */
    int Get_Motor_FB_Data(shared_ptr<Device_class> Device_P, float *P, float *V, float *F, float temp[2], uint16_t *error);

    /**
     * @brief 读取电机反馈的位置
     * @param P 位置，单位度
     * @param V 速度(无效）
     * @param F 力矩(无效）
    */
    int Get_Motor_FB_Data(shared_ptr<Device_class> Device_P, float *P, float *V, float *F);

    //  /**
    //  * @brief 读取电机温度
    //  * @param Temp 温度值，单位摄氏度
    // */
    // int Get_Motor_Temp(shared_ptr<Device_class> Device_P, float *Temp);


    /**
     * @brief Yaml配置文件读取电机参数并初始化
    */
    int Get_Motor_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);


    /**
     * @brief 回调函数，CAN帧数据解析
    */
    int Motor_Top_Frame_Analyze(volatile u8 *Can_Frame);

    // // /**
    // //  * @brief 设置电机CAN ID 保存到flash
    // //  * @param new_id 新的CAN ID
    // //  * @param save_to_flash 是否保存到flash  需要在停止状态下执行
    // // */

    // // int Set_Can_Id(shared_ptr<Device_Struct> Device_P, u32 new_id);

    // /**
    //  * @brief 设置位置环KP
    //  * @param kp 比例增益
    //  */
    // int Set_Pos_KP(shared_ptr<Device_class> device, int kp);

    // /**
    //  * @brief 设置位置环KD
    //  * @param kd 微分增益
    //  */
    // int Set_Pos_KD(shared_ptr<Device_class> device, int kd);

    //  /**
    //  * @brief 设置速度环KP
    //  * @param kp 比例增益
    //  */
    // int Set_Vel_KP(shared_ptr<Device_class> device, int kp);

    // /**
    //  * @brief 设置速度环KI
    //  * @param ki 积分增益
    //  */
    // int Set_Vel_KI(shared_ptr<Device_class> device, int ki);

    // /**
    //  * @brief 设置电流环KD
    //  * @param kd 微分增益
    //  */
    // int Set_Cur_KP(shared_ptr<Device_class> device, int kp);

    //  /**
    //  * @brief 设置电流环Ki
    //  * @param ki 积分增益
    //  */
    // int Set_Cur_KI(shared_ptr<Device_class> device, int ki);


    // /**
    //  * @brief 设置电机在位置模式下的正向速度
    //  * @param Vel_Rad_S 速度值，单位度每秒
    // */
    // int Set_Forward_Vel(shared_ptr<Device_class> Device_P, float Vel_Rad_S);

    // /**
    //  * @brief 设置电机在位置模式下的反向速度
    //  * @param Vel_Rad_S 速度值，单位度每秒
    // */
    // int Set_Backward_Vel(shared_ptr<Device_class> Device_P, float Vel_Rad_S);

    // /**
    //  * @brief 保存设置参数到flash
    // */
    // int Save_To_Flash(shared_ptr<Device_class> device);


    // /*
    //     * @brief 获取电机报错状态
    //     * @param error_status 报错状态，0表示无报错，非0表示有报错  
    // */
    // int Get_Motor_Error_Status(shared_ptr<Device_class> device, uint32_t *error_status);


    // /*
    //     * @brief 清除电机报错状态
    //     * @param device 设备指针
    // */
    // int Clear_Motor_Error_Status(shared_ptr<Device_class> Device_P);


    //  /*
    //     * @brief 检查电机连通状态
    //     * @param status 连通状态  
    // */
    // int Check_Motor_Link(shared_ptr<Device_class> Device_P);


    ~Motor_SenChuang();


};

#endif
