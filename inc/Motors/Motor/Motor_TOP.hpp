#ifndef MOTOR_TOP_H_
#define MOTOR_TOP_H_
#include "syst.hpp"
#include "Motor.hpp"
#include "HARDWARE_TOP.hpp"

#define Motor_Device_Type "Motor_Device"

#define Motor_Device_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Motor *One_Motor = new Motor();\
                                Device->Device_Private_Class = (void *)One_Motor;\
                                if (Node != nullptr)\
                                    return One_Motor->Get_Motor_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Motor_Device_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int\
                                    {\
                                        return ((Motor *)Device->Device_Private_Class)->Motor_Top_Frame_Analyze(Device, Msg);\
                                    }\

#define Motor_Device_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Motor *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\

// script auto start -> IN_USE
// script auto end -> IN_USE

class Motor : private Robot_Hardware
{
public:
    int Motor_EN(const shared_ptr<Device_class> &Device_P, int EN);
    int Set_Mod(const shared_ptr<Device_class> &Device_P, int Mod);
    int Set_Zero(const shared_ptr<Device_class> &Device_P, float offest);
    int IC_MU_CAL(const shared_ptr<Device_class> &Device_P, int EN);
    int IC_MU_CAL(const shared_ptr<Device_class> &Device_P,UDP_Def *UDP_Def_one, int EN);
    int Motor_Cal_Start(const shared_ptr<Device_class> &Device_P);
    void Motor_Calibration_Data_Send(const shared_ptr<Device_class> &Device_P);
    int Send_Current_Control_Data(const shared_ptr<Device_class> &Device_P, float Current_A);
    int Send_Force_Control_Data(const shared_ptr<Device_class> &Device_P, float Force_N);
    int Send_Speed_Control_Data(const shared_ptr<Device_class> &Device_P, float Speed_Rad_S, float Force_N, float Force_MAX_N);
    int Send_Pos_Control_Data(const shared_ptr<Device_class> &Device_P, float Rad, float Speed_Rad_S, float Force_N, float Speed_MAX_Rad_S, float Force_MAX_N);
    int Send_MIT_PD_Control_Data(const shared_ptr<Device_class> &Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    int Send_MIT_PD_Control_Data_Delay(const shared_ptr<Device_class> &Device_P, u32 us, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);
    int Get_Motor_FB_Data(const shared_ptr<Device_class>& Device_P, float *P, float *V, float *F);
    int Get_Motor_FB_Data(const shared_ptr<Device_class>& Device_P, float *P, float *V, float *F, float temp[2], u16 *error);

    int Get_Motor_Device_Data_From_Yaml_And_Init(const shared_ptr<Device_class>& Device, YAML::Node One_Node);
    int Motor_Top_Frame_Analyze(const shared_ptr<Device_class>& Device, volatile u8 *Can_Frame) const;

    static bool Range_Check(int64_t target, const string &str);
    static void Motor_Add_Err(const shared_ptr<Device_class>& Device_P, const string& Error_string, Err_Level Level);

public:
    shared_ptr<GaussianFilter1D> V_filter= make_shared<GaussianFilter1D>(15, 5);
    shared_ptr<GaussianFilter1D> F_filter= make_shared<GaussianFilter1D>(31, 15);
    
// script auto start -> IN_USE
// script auto end -> IN_USE

// script auto start -> OUT_USE
#ifndef IN_USE
    shared_ptr<void> Motor_Private_Data;
#endif
// script auto end -> OUT_USE

};

#endif
