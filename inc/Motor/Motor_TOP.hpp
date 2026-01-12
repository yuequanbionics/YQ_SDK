#ifndef MOTOR_TOP_H_
#define MOTOR_TOP_H_
#include "syst.hpp"
#include "Motor.hpp"
#include "HARDWARE_TOP.hpp"

#define Motor_Device_Type "Motor"

#define Motor_Device_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                            {\
                                Motor *One_Motor = new Motor();\
                                Device->Device_Private_Class = (void *)One_Motor;\
                                if (Node != nullptr)\
                                    return One_Motor->Get_Motor_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Motor_Device_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                    {\
                                        return ((Motor *)Device_Private_Class)->Motor_Top_Frame_Analyze(Msg);\
                                    }\

#define Motor_Device_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Motor *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\


class Motor : private Robot_Hardware
{
public:
    typedef struct
    {
        Motor_Info Motor_Info_Data;
        Motor_Info Motor_Info_Data_FB;

        bool FLAG_Motor_CMD_Save_Config_To_Flash;
        bool FLAG_Motor_CMD_Start_Foc;
        bool FLAG_Motor_CMD_Set_Zero;
        bool FLAG_Motor_CMD_Motor_MOD;
    } Motor_Private_Struct;

    shared_ptr<Motor_Private_Struct> Motor_Private_Data = make_shared<Motor_Private_Struct>();

    shared_ptr<GaussianFilter1D> V_filter= make_shared<GaussianFilter1D>(15, 5);
    shared_ptr<GaussianFilter1D> F_filter= make_shared<GaussianFilter1D>(15, 5);

public:
    int Motor_EN(shared_ptr<Device_Struct> Device_P, int EN);
    int Set_Mod(shared_ptr<Device_Struct> Device_P, int Mod);
    int Set_Zero(shared_ptr<Device_Struct> Device_P, float offest);
    int IC_MU_CAL(shared_ptr<Device_Struct> Device_P, int EN);
    int IC_MU_CAL(shared_ptr<Device_Struct> Device_P,UDP_Def *UDP_Def_one, int EN);
    int Motor_Cal_Start(shared_ptr<Device_Struct> Device_P);
    void Motor_Calibration_Data_Send(shared_ptr<Device_Struct> Device_P);
    int Send_Current_Control_Data(shared_ptr<Device_Struct> Device_P, float Current_A);
    int Send_Force_Control_Data(shared_ptr<Device_Struct> Device_P, float Force_N);
    int Send_Speed_Control_Data(shared_ptr<Device_Struct> Device_P, float Speed_Rad_S, float Force_N, float Force_MAX_N);
    int Send_Pos_Control_Data(shared_ptr<Device_Struct> Device_P, float Rad, float Speed_Rad_S, float Force_N, float Speed_MAX_Rad_S, float Force_MAX_N);
    int Send_MIT_PD_Control_Data(shared_ptr<Device_Struct> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    int Send_MIT_PD_Control_Data_Delay(shared_ptr<Device_Struct> Device_P, u32 us, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    int Get_Motor_FB_Data(shared_ptr<Device_Struct> Device_P, float *P, float *V, float *F);
    int Get_Motor_FB_Data(shared_ptr<Device_Struct> Device_P, float *P, float *V, float *F, float temp[2], u16 *error);

    int Get_Motor_Device_Data_From_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
    int Motor_Top_Frame_Analyze(volatile u8 *Can_Frame);
};

#endif
