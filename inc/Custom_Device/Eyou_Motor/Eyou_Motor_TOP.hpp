#ifndef MOTOR_TOP_H_
#define MOTOR_TOP_H_
#include "syst.hpp"
#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"

#define Eyou_Custom_Type "Eyou_Custom_Motor_TOP"

#define Eyou_Custom_Motor_Device_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Eyou_Motor *One_Eyou_Motor = new Eyou_Motor();\
                                Device->Device_Private_Class = (void *)One_Eyou_Motor;\
                                if (Node != nullptr)\
                                    return One_Eyou_Motor->Get_Eyou_Custom_Motor_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Eyou_Custom_Motor_Device_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int \
                                    {\
                                        return ((Eyou_Motor *)Device->Device_Private_Class)->Eyou_Custom_Motor_Top_Frame_Analyze(Device, Msg);\
                                    }\

#define Eyou_Custom_Motor_Device_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Eyou_Motor *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\

#define Eyou_Motor_Data_Len         Head_Len + 8
#define Reduction_Ratio             121
#define Can_Id_Send_Offset          0x600
#define Can_Id_Back_Offset          0x580
#define R_T_A                       (float)57.29577

#define Big_end   0
#define Small_end 1

#define Normal_Mode         0
#define Set_Status_To_Zero  1
#define Set_Status_To_Offest  2
#define Error_Code  3

enum Eyou_CS_Data{
    Write_4_Byte_Data = 0x23,
    Write_3_Byte_Data = 0x27,
    Write_2_Byte_Data = 0x2B,
    Write_1_Byte_Data = 0x2F,

    Read_Back = 0x40,
    Read_Back_With_Code = 0x80,
};

enum Eyou_Dictionary_Mode{
    Set_Can_Id = 0x01,
    Save_To_Motor = 0x10,
    Control_Word = 0x40,
    Mode_Of_Work = 0x60,
    Read_Motor_Pos = 0x64,
    Read_Motor_Speed = 0x6C,
    Set_Zero_Mode = 0x7C,
    Planning_Speed = 0x81,
    Planning_Acceleration = 0x83,
    Planning_Deceleration,
    Target_Pos = 0x7A,
};

enum Eyou_Call_Back_Index{
    /* 获取错误码命令 */
    Error_Call_back = 0x603f,

    /* 以下为错误码具体指令 */
    /* 过压保护错误码 */
    Error_Code_OV   = 0x3210,
    /* 欠压保护错误码 */
    Error_Code_UV   = 0x3220,
    /* 过载保护错误码 */
    Error_Code_OL   = 0x3230,
    /* 过温保护错误码 */
    Error_Code_OT   = 0x4210,
    /* 堵转错误码 */
    Error_Code_LR   = 0x7121,
    /* 超速错误码 */
    Error_Code_S    = 0x7310,
    /* 心跳掉线错误码 */
    Error_Code_HD   = 0x8130,
    /* 速度误差过大错误码 */
    Error_Code_ES   = 0x8500,
    /* 位置误差过大错误码 */
    Error_Code_EP   = 0x8611,
};

typedef struct
{ // CMD数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
    u8 CS;
    u8 Dictionary[2];
    u8 Subindex;
    u8 data[4];
} Eyou_Motor_Data;

class Eyou_Motor : private Robot_Hardware
{
private:

public:
    double Eyou_Motor_Speed_Now = 0.0;
    double Eyou_Motor_Pos_Now = 0.0;
    float motor_pos = 0.0;
    int Get_Eyou_Custom_Motor_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
    int Eyou_Custom_Motor_Top_Frame_Analyze(shared_ptr<Device_class> Device, volatile u8 *Can_Frame);
    
    /**
     * @brief eyou电机读取错误码线程 1hz
    */
    void Eyou_ErrorCode_Readthread(shared_ptr<Device_class> Device_P);

    /**
     * @brief 电机使能/失能
    */
    int Motor_EN(shared_ptr<Device_class> Device_P, int EN);

    /**
     * @brief 电机停止，但不下使能
    */
    int Motor_Stop(shared_ptr<Device_class> Device_P);

    int Send_MIT_PD_Control_Data(shared_ptr<Device_class> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    int Send_MIT_PD_Control_Data_(shared_ptr<Device_class> Device_P, float Rad, float Speed_Rad_S, float Acceleration, float Deceleration);
    int Send_MIT_PD_Control_Data_old(shared_ptr<Device_class> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    /**
     * @brief 写零点与偏执
     * @param offest 0为设置零点
    */
    int Set_Zero(shared_ptr<Device_class> Device_P, int en);

    int Set_Offest(shared_ptr<Device_class> Device_P, float Offest_Datasss);

    /**
     * @brief 读取Eyou电机当前速度、位置。
    */
    int Get_Motor_FB_Data(shared_ptr<Device_class> Device_P, float *P, float *V, float *F);

    /**
     * @brief 设置Eyou电机的ID
    */
    int Set_ID(shared_ptr<Device_class> Device_P, u8 new_id);
    void Read_KP_Data(shared_ptr<Device_class> Device_P);
    void Read_KI_Data(shared_ptr<Device_class> Device_P);
    void Write_KP_Data(shared_ptr<Device_class> Device_P, uint16_t data);
    void Write_KI_Data(shared_ptr<Device_class> Device_P, uint16_t data);

    void Get_Error(shared_ptr<Device_class> Device_P);
    void Write_set_perr_max(shared_ptr<Device_class> Device_P, u8 *data);
    void Send_Data_Assemble(shared_ptr<Device_class> Device_P, Eyou_Motor_Data *Eyou_Motor_Data_point, u16 SDL, u8 CS, u16 index, u8 Sub, u8 *data, u8 BOS);
    void Eyou_Get_ErrorCode(shared_ptr<Device_class> Device_P);
    void Eyou_Add_Error(shared_ptr<Device_class> Device_P, const string& Error_string);
private:
    std::shared_ptr<Device_class> s_device;
    int Call_Back_Status = 0;
    int Write_Flag = 0;
    u8 yuan_data[4] = {0};
    /* 减速比 */
    int Motor_Reduction_Ratio = 0;

    /* 编码器输出分辨率 */
    float Encoder_Res = 0.0f;
    
    uint16_t Eyou_Kp = 0;
    float Speed_Angle_S = 0.0;
    float Acceleration = 0.0;
    float Deceleration = 0.0;
    float Offest_From_Yaml = 0.0;
    float Offest_Data = 0.0;
    Eyou_Motor_Data Data_Init(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device); 
    Eyou_Motor_Data Data_Save_To_Motor(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device);
    Eyou_Motor_Data Write_1_Data(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device, u8 now_, u8 cmd, u8 index, u8 data);
    Eyou_Motor_Data Write_2_Data(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device, u8 now_, u8 cmd, u8 index, u8 data0, u8 data1);
    Eyou_Motor_Data Write_Angel_And_Speed_Data(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device, u8 now_, u8 cmd, u8 index, float in_data);
    Eyou_Motor_Data Write_Zero_Data(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device, u8 cmd, u8 index, float in_data);
    Eyou_Motor_Data Read_Motor_Data(Eyou_Motor_Data *Eyou_Motor_Datas, std::shared_ptr<Device_class> Device, u8 cmd, u8 index);
    Eyou_Motor_Data Set_Zero_Call_Back(shared_ptr<Device_class> Device_P, Eyou_Motor_Data *Eyou_Motor_Data_point);
    Eyou_Motor_Data Set_Offest_Call_Back(shared_ptr<Device_class> Device_P, Eyou_Motor_Data *Eyou_Motor_Data_point);
    void Print_Send_Data(Eyou_Motor_Data *Eyou_Motor_Datas);
    void Print_Back_Data(Eyou_Motor_Data *Eyou_Motor_Datas);
};

#endif
