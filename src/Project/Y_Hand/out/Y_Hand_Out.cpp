#include "syst.hpp"
#include "UDP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Custom_TOP.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "Auto_Set_Id.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <math.h>
#include "unistd.h"
#include <filesystem>

using namespace std;

Robot_Hardware *Test_Robot;

shared_ptr<Device_Struct>  Motor1_Device;
shared_ptr<Device_Struct>  Motor2_Device;
shared_ptr<Device_Struct>  Motor3_Device;
shared_ptr<Device_Struct>  Motor4_Device;
shared_ptr<Device_Struct>  Motor5_Device;
shared_ptr<Device_Struct>  Motor6_Device;
shared_ptr<Device_Struct>  Motor7_Device;
shared_ptr<Device_Struct>  Motor8_Device;
shared_ptr<Device_Struct>  Motor9_Device;
shared_ptr<Device_Struct> Motor10_Device;
shared_ptr<Device_Struct> Motor11_Device;
shared_ptr<Device_Struct> Motor12_Device;
shared_ptr<Device_Struct> Motor13_Device;
shared_ptr<Device_Struct> Motor14_Device;
shared_ptr<Device_Struct> Motor15_Device;
shared_ptr<Device_Struct> Motor16_Device;
shared_ptr<Device_Struct> Motor17_Device;
shared_ptr<Device_Struct> Motor18_Device;
shared_ptr<Device_Struct> Motor19_Device;
shared_ptr<Device_Struct> Motor20_Device;
shared_ptr<Device_Struct> Motor21_Device;
shared_ptr<Device_Struct> Motor22_Device;
shared_ptr<Device_Struct> Motor23_Device;
shared_ptr<Device_Struct> Motor24_Device;
shared_ptr<Device_Struct> Motor25_Device;
shared_ptr<Device_Struct> Motor26_Device;
shared_ptr<Device_Struct> Motor27_Device;
shared_ptr<Device_Struct> Motor28_Device;

Motor * Motor1_Control;
Motor * Motor2_Control;
Motor * Motor3_Control;
Motor * Motor4_Control;
Motor * Motor5_Control;
Motor * Motor6_Control;
Motor * Motor7_Control;
Motor * Motor8_Control;
Motor * Motor9_Control;
Motor *Motor10_Control;
Motor *Motor11_Control;
Motor *Motor12_Control;
Motor *Motor13_Control;
Motor *Motor14_Control;
Motor *Motor15_Control;
Motor *Motor16_Control;
Motor *Motor17_Control;
Motor *Motor18_Control;
Motor *Motor19_Control;
Motor *Motor20_Control;
Motor *Motor21_Control;
Motor *Motor22_Control;
Motor *Motor23_Control;
Motor *Motor24_Control;
Motor *Motor25_Control;
Motor *Motor26_Control;
Motor *Motor27_Control;
Motor *Motor28_Control;

int hand_pretighten(float F, uint32_t time_ms)
{
    if(F > 0.1f)
        F = 0.1f;
    time_ms = time_ms/100;
    if(time_ms <= 1)
        time_ms = 5;
    while (time_ms--)
    {   
        float TT = -F; 
        float PP = 0;   
        float VV = 0;   
        float Kp = 0;   
        float Kd = 0; 
    
         Motor1_Control->Send_MIT_PD_Control_Data( Motor1_Device, PP, VV, TT, Kp, Kd);
         Motor2_Control->Send_MIT_PD_Control_Data( Motor2_Device, PP, VV, TT, Kp, Kd);
         Motor3_Control->Send_MIT_PD_Control_Data( Motor3_Device, PP, VV, TT, Kp, Kd);
         Motor4_Control->Send_MIT_PD_Control_Data( Motor4_Device, PP, VV, TT, Kp, Kd);
         Motor5_Control->Send_MIT_PD_Control_Data( Motor5_Device, PP, VV, TT, Kp, Kd);
         Motor6_Control->Send_MIT_PD_Control_Data( Motor6_Device, PP, VV, TT, Kp, Kd);
         Motor7_Control->Send_MIT_PD_Control_Data( Motor7_Device, PP, VV, TT, Kp, Kd);
         Motor8_Control->Send_MIT_PD_Control_Data( Motor8_Device, PP, VV, TT, Kp, Kd);
         Motor9_Control->Send_MIT_PD_Control_Data( Motor9_Device, PP, VV, TT, Kp, Kd);
        Motor10_Control->Send_MIT_PD_Control_Data(Motor10_Device, PP, VV, TT, Kp, Kd);
        Motor11_Control->Send_MIT_PD_Control_Data(Motor11_Device, PP, VV, TT, Kp, Kd);
        Motor12_Control->Send_MIT_PD_Control_Data(Motor12_Device, PP, VV, TT, Kp, Kd);
        Motor13_Control->Send_MIT_PD_Control_Data(Motor13_Device, PP, VV, TT, Kp, Kd);
        Motor14_Control->Send_MIT_PD_Control_Data(Motor14_Device, PP, VV, TT, Kp, Kd);
        Motor15_Control->Send_MIT_PD_Control_Data(Motor15_Device, PP, VV, TT, Kp, Kd);
        Motor16_Control->Send_MIT_PD_Control_Data(Motor16_Device, PP, VV, TT, Kp, Kd);
        Motor17_Control->Send_MIT_PD_Control_Data(Motor17_Device, PP, VV, TT, Kp, Kd);
        Motor18_Control->Send_MIT_PD_Control_Data(Motor18_Device, PP, VV, TT, Kp, Kd);
        Motor19_Control->Send_MIT_PD_Control_Data(Motor19_Device, PP, VV, TT, Kp, Kd);
        Motor20_Control->Send_MIT_PD_Control_Data(Motor20_Device, PP, VV, TT, Kp, Kd);
        Motor21_Control->Send_MIT_PD_Control_Data(Motor21_Device, PP, VV, TT, Kp, Kd);
        Motor22_Control->Send_MIT_PD_Control_Data(Motor22_Device, PP, VV, TT, Kp, Kd);
        Motor23_Control->Send_MIT_PD_Control_Data(Motor23_Device, PP, VV, TT, Kp, Kd);
        Motor24_Control->Send_MIT_PD_Control_Data(Motor24_Device, PP, VV, TT, Kp, Kd);
        Motor25_Control->Send_MIT_PD_Control_Data(Motor25_Device, PP, VV, TT, Kp, Kd);
        Motor26_Control->Send_MIT_PD_Control_Data(Motor26_Device, PP, VV, TT, Kp, Kd);
        Motor27_Control->Send_MIT_PD_Control_Data(Motor27_Device, PP, VV, TT, Kp, Kd);
        Motor28_Control->Send_MIT_PD_Control_Data(Motor28_Device, PP, VV, TT, Kp, Kd);

        // 发送控制缓冲区数据到硬件
        Test_Robot->Send_Buff_Data();

        // 控制周期延时
        usleep(100000);
    }

     Motor1_Control->Motor_EN( Motor1_Device, 0);
     Motor2_Control->Motor_EN( Motor2_Device, 0);
     Motor3_Control->Motor_EN( Motor3_Device, 0);
     Motor4_Control->Motor_EN( Motor4_Device, 0);
     Motor5_Control->Motor_EN( Motor5_Device, 0);
     Motor6_Control->Motor_EN( Motor6_Device, 0);
     Motor7_Control->Motor_EN( Motor7_Device, 0);
     Motor8_Control->Motor_EN( Motor8_Device, 0);
     Motor9_Control->Motor_EN( Motor9_Device, 0);
    Motor10_Control->Motor_EN(Motor10_Device, 0);
    Motor11_Control->Motor_EN(Motor11_Device, 0);
    Motor12_Control->Motor_EN(Motor12_Device, 0);
    Motor13_Control->Motor_EN(Motor13_Device, 0);
    Motor14_Control->Motor_EN(Motor14_Device, 0);
    Motor15_Control->Motor_EN(Motor15_Device, 0);
    Motor16_Control->Motor_EN(Motor16_Device, 0);
    Motor17_Control->Motor_EN(Motor17_Device, 0);
    Motor18_Control->Motor_EN(Motor18_Device, 0);
    Motor19_Control->Motor_EN(Motor19_Device, 0);
    Motor20_Control->Motor_EN(Motor20_Device, 0);
    Motor21_Control->Motor_EN(Motor21_Device, 0);
    Motor22_Control->Motor_EN(Motor22_Device, 0);
    Motor23_Control->Motor_EN(Motor23_Device, 0);
    Motor24_Control->Motor_EN(Motor24_Device, 0);
    Motor25_Control->Motor_EN(Motor25_Device, 0);
    Motor26_Control->Motor_EN(Motor26_Device, 0);
    Motor27_Control->Motor_EN(Motor27_Device, 0);
    Motor28_Control->Motor_EN(Motor28_Device, 0);
    usleep(1000000);
     Motor1_Control->Set_Zero( Motor1_Device, 0);
     Motor2_Control->Set_Zero( Motor2_Device, 0);
     Motor3_Control->Set_Zero( Motor3_Device, 0);
     Motor4_Control->Set_Zero( Motor4_Device, 0);
     Motor5_Control->Set_Zero( Motor5_Device, 0);
     Motor6_Control->Set_Zero( Motor6_Device, 0);
     Motor7_Control->Set_Zero( Motor7_Device, 0);
     Motor8_Control->Set_Zero( Motor8_Device, 0);
     Motor9_Control->Set_Zero( Motor9_Device, 0);
    Motor10_Control->Set_Zero(Motor10_Device, 0);
    Motor11_Control->Set_Zero(Motor11_Device, 0);
    Motor12_Control->Set_Zero(Motor12_Device, 0);
    Motor13_Control->Set_Zero(Motor13_Device, 0);
    Motor14_Control->Set_Zero(Motor14_Device, 0);
    Motor15_Control->Set_Zero(Motor15_Device, 0);
    Motor16_Control->Set_Zero(Motor16_Device, 0);
    Motor17_Control->Set_Zero(Motor17_Device, 0);
    Motor18_Control->Set_Zero(Motor18_Device, 0);
    Motor19_Control->Set_Zero(Motor19_Device, 0);
    Motor20_Control->Set_Zero(Motor20_Device, 0);
    Motor21_Control->Set_Zero(Motor21_Device, 0);
    Motor22_Control->Set_Zero(Motor22_Device, 0);
    Motor23_Control->Set_Zero(Motor23_Device, 0);
    Motor24_Control->Set_Zero(Motor24_Device, 0);
    Motor25_Control->Set_Zero(Motor25_Device, 0);
    Motor26_Control->Set_Zero(Motor26_Device, 0);
    Motor27_Control->Set_Zero(Motor27_Device, 0);
    Motor28_Control->Set_Zero(Motor28_Device, 0);  
    usleep(1000000);
     Motor1_Control->Motor_EN( Motor1_Device, 1);
     Motor2_Control->Motor_EN( Motor2_Device, 1);
     Motor3_Control->Motor_EN( Motor3_Device, 1);
     Motor4_Control->Motor_EN( Motor4_Device, 1);
     Motor5_Control->Motor_EN( Motor5_Device, 1);
     Motor6_Control->Motor_EN( Motor6_Device, 1);
     Motor7_Control->Motor_EN( Motor7_Device, 1);
     Motor8_Control->Motor_EN( Motor8_Device, 1);
     Motor9_Control->Motor_EN( Motor9_Device, 1);
    Motor10_Control->Motor_EN(Motor10_Device, 1);
    Motor11_Control->Motor_EN(Motor11_Device, 1);
    Motor12_Control->Motor_EN(Motor12_Device, 1);
    Motor13_Control->Motor_EN(Motor13_Device, 1);
    Motor14_Control->Motor_EN(Motor14_Device, 1);
    Motor15_Control->Motor_EN(Motor15_Device, 1);
    Motor16_Control->Motor_EN(Motor16_Device, 1);
    Motor17_Control->Motor_EN(Motor17_Device, 1);
    Motor18_Control->Motor_EN(Motor18_Device, 1);
    Motor19_Control->Motor_EN(Motor19_Device, 1);
    Motor20_Control->Motor_EN(Motor20_Device, 1);
    Motor21_Control->Motor_EN(Motor21_Device, 1);
    Motor22_Control->Motor_EN(Motor22_Device, 1);
    Motor23_Control->Motor_EN(Motor23_Device, 1);
    Motor24_Control->Motor_EN(Motor24_Device, 1);
    Motor25_Control->Motor_EN(Motor25_Device, 1);
    Motor26_Control->Motor_EN(Motor26_Device, 1);
    Motor27_Control->Motor_EN(Motor27_Device, 1);
    Motor28_Control->Motor_EN(Motor28_Device, 1);  

    return 0;
}


#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(string ADDR)
#endif
{
    Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type("Switch_Board", Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    Test_Robot->Add_Device_Type("Motor_Device", Motor_Device_Init, Motor_Device_CallBack_F, Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
 
#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/Y_Hand/out/TOP.yaml";
#endif
    if(Test_Robot->Init_TOP(ADDR) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

     Motor1_Device = Test_Robot->Get_Device_For_Name("Motor_1");
     Motor2_Device = Test_Robot->Get_Device_For_Name("Motor_2");
     Motor3_Device = Test_Robot->Get_Device_For_Name("Motor_3");
     Motor4_Device = Test_Robot->Get_Device_For_Name("Motor_4");
     Motor5_Device = Test_Robot->Get_Device_For_Name("Motor_5");
     Motor6_Device = Test_Robot->Get_Device_For_Name("Motor_6");
     Motor7_Device = Test_Robot->Get_Device_For_Name("Motor_7");
     Motor8_Device = Test_Robot->Get_Device_For_Name("Motor_8");
     Motor9_Device = Test_Robot->Get_Device_For_Name("Motor_9");
    Motor10_Device = Test_Robot->Get_Device_For_Name("Motor_10");
    Motor11_Device = Test_Robot->Get_Device_For_Name("Motor_11");
    Motor12_Device = Test_Robot->Get_Device_For_Name("Motor_12");
    Motor13_Device = Test_Robot->Get_Device_For_Name("Motor_13");
    Motor14_Device = Test_Robot->Get_Device_For_Name("Motor_14");
    Motor15_Device = Test_Robot->Get_Device_For_Name("Motor_15");
    Motor16_Device = Test_Robot->Get_Device_For_Name("Motor_16");
    Motor17_Device = Test_Robot->Get_Device_For_Name("Motor_17");
    Motor18_Device = Test_Robot->Get_Device_For_Name("Motor_18");
    Motor19_Device = Test_Robot->Get_Device_For_Name("Motor_19");
    Motor20_Device = Test_Robot->Get_Device_For_Name("Motor_20");
    Motor21_Device = Test_Robot->Get_Device_For_Name("Motor_21");
    Motor22_Device = Test_Robot->Get_Device_For_Name("Motor_22");
    Motor23_Device = Test_Robot->Get_Device_For_Name("Motor_23");
    Motor24_Device = Test_Robot->Get_Device_For_Name("Motor_24");
    Motor25_Device = Test_Robot->Get_Device_For_Name("Motor_25");
    Motor26_Device = Test_Robot->Get_Device_For_Name("Motor_26");
    Motor27_Device = Test_Robot->Get_Device_For_Name("Motor_27");
    Motor28_Device = Test_Robot->Get_Device_For_Name("Motor_28");


     Motor1_Control = (Motor*)Test_Robot->Get_Control_Class( Motor1_Device);
     Motor2_Control = (Motor*)Test_Robot->Get_Control_Class( Motor2_Device);
     Motor3_Control = (Motor*)Test_Robot->Get_Control_Class( Motor3_Device);
     Motor4_Control = (Motor*)Test_Robot->Get_Control_Class( Motor4_Device);
     Motor5_Control = (Motor*)Test_Robot->Get_Control_Class( Motor5_Device);
     Motor6_Control = (Motor*)Test_Robot->Get_Control_Class( Motor6_Device);
     Motor7_Control = (Motor*)Test_Robot->Get_Control_Class( Motor7_Device);
     Motor8_Control = (Motor*)Test_Robot->Get_Control_Class( Motor8_Device);
     Motor9_Control = (Motor*)Test_Robot->Get_Control_Class( Motor9_Device);
    Motor10_Control = (Motor*)Test_Robot->Get_Control_Class(Motor10_Device);
    Motor11_Control = (Motor*)Test_Robot->Get_Control_Class(Motor11_Device);
    Motor12_Control = (Motor*)Test_Robot->Get_Control_Class(Motor12_Device);
    Motor13_Control = (Motor*)Test_Robot->Get_Control_Class(Motor13_Device);
    Motor14_Control = (Motor*)Test_Robot->Get_Control_Class(Motor14_Device);
    Motor15_Control = (Motor*)Test_Robot->Get_Control_Class(Motor15_Device);
    Motor16_Control = (Motor*)Test_Robot->Get_Control_Class(Motor16_Device);
    Motor17_Control = (Motor*)Test_Robot->Get_Control_Class(Motor17_Device);
    Motor18_Control = (Motor*)Test_Robot->Get_Control_Class(Motor18_Device);
    Motor19_Control = (Motor*)Test_Robot->Get_Control_Class(Motor19_Device);
    Motor20_Control = (Motor*)Test_Robot->Get_Control_Class(Motor20_Device);
    Motor21_Control = (Motor*)Test_Robot->Get_Control_Class(Motor21_Device);
    Motor22_Control = (Motor*)Test_Robot->Get_Control_Class(Motor22_Device);
    Motor23_Control = (Motor*)Test_Robot->Get_Control_Class(Motor23_Device);
    Motor24_Control = (Motor*)Test_Robot->Get_Control_Class(Motor24_Device);
    Motor25_Control = (Motor*)Test_Robot->Get_Control_Class(Motor25_Device);
    Motor26_Control = (Motor*)Test_Robot->Get_Control_Class(Motor26_Device);
    Motor27_Control = (Motor*)Test_Robot->Get_Control_Class(Motor27_Device);
    Motor28_Control = (Motor*)Test_Robot->Get_Control_Class(Motor28_Device);


#ifndef HAVE_ROS
    float S = 0;                   // 正弦函数相位变量（随时间递增）
    float P[30], V[30], F[30];     // 存储电机反馈数据：位置(Position)、速度(Velocity)、力(电流 0-100 参数为百分比*100) //从1开始
    float Kp = 0;
    float Kd = 0;
    int times = 0;
    float PP_old = 0;
    constexpr int time_step = 1000*1000/250;
    // 主控制循环（持续运行）
    while (1)
    {   
        // 更新相位并计算目标控制量（正弦轨迹）
        S += 0.0125f;                                                                 // 位置步长, 角度
        float PP = sin(S) * 90.0f - 100;                                            // MIT 目标控制位置
        float VV = (PP - PP_old) * (static_cast<float>(1000000) / time_step);  // MIT 控制目标速度, 它是位置的微分
        float TT = -sin(S)*0.5f;   

        PP = 0; 
        VV = 0;     
        TT = 0;
    
         Motor1_Control->Send_MIT_PD_Control_Data( Motor1_Device, PP, VV, 0.0, Kp, Kd);
         Motor2_Control->Send_MIT_PD_Control_Data( Motor2_Device, PP, VV, 0.0, Kp, Kd);
         Motor3_Control->Send_MIT_PD_Control_Data( Motor3_Device, PP, VV, 0.0, Kp, Kd);
         Motor4_Control->Send_MIT_PD_Control_Data( Motor4_Device, PP, VV, 0.0, Kp, Kd);
         Motor5_Control->Send_MIT_PD_Control_Data( Motor5_Device, PP, VV, 0.0, Kp, Kd);
         Motor6_Control->Send_MIT_PD_Control_Data( Motor6_Device, PP, VV, 0.0, Kp, Kd);
         Motor7_Control->Send_MIT_PD_Control_Data( Motor7_Device, PP, VV, 0.0, Kp, Kd);
         Motor8_Control->Send_MIT_PD_Control_Data( Motor8_Device, PP, VV, 0.0, Kp, Kd);
         Motor9_Control->Send_MIT_PD_Control_Data( Motor9_Device, PP, VV, 0.0, Kp, Kd);
        Motor10_Control->Send_MIT_PD_Control_Data(Motor10_Device, PP, VV, 0.0, Kp, Kd);
        Motor11_Control->Send_MIT_PD_Control_Data(Motor11_Device, PP, VV, 0.0, Kp, Kd);
        Motor12_Control->Send_MIT_PD_Control_Data(Motor12_Device, PP, VV, 0.0, Kp, Kd);
        Motor13_Control->Send_MIT_PD_Control_Data(Motor13_Device, PP, VV, 0.0, Kp, Kd);
        Motor14_Control->Send_MIT_PD_Control_Data(Motor14_Device, PP, VV, 0.0, Kp, Kd);
        Motor15_Control->Send_MIT_PD_Control_Data(Motor15_Device, PP, VV, 0.0, Kp, Kd);
        Motor16_Control->Send_MIT_PD_Control_Data(Motor16_Device, PP, VV, 0.0, Kp, Kd);
        Motor17_Control->Send_MIT_PD_Control_Data(Motor17_Device, PP, VV, 0.0, Kp, Kd);
        Motor18_Control->Send_MIT_PD_Control_Data(Motor18_Device, PP, VV, 0.0, Kp, Kd);
        Motor19_Control->Send_MIT_PD_Control_Data(Motor19_Device, PP, VV, 0.0, Kp, Kd);
        Motor20_Control->Send_MIT_PD_Control_Data(Motor20_Device, PP, VV, 0.0, Kp, Kd);
        Motor21_Control->Send_MIT_PD_Control_Data(Motor21_Device, PP, VV, 0.0, Kp, Kd);
        Motor22_Control->Send_MIT_PD_Control_Data(Motor22_Device, PP, VV, 0.0, Kp, Kd);
        Motor23_Control->Send_MIT_PD_Control_Data(Motor23_Device, PP, VV, 0.0, Kp, Kd);
        Motor24_Control->Send_MIT_PD_Control_Data(Motor24_Device, PP, VV, 0.0, Kp, Kd);
        Motor25_Control->Send_MIT_PD_Control_Data(Motor25_Device, PP, VV, 0.0, Kp, Kd);
        Motor26_Control->Send_MIT_PD_Control_Data(Motor26_Device, PP, VV, 0.0, Kp, Kd);
        Motor27_Control->Send_MIT_PD_Control_Data(Motor27_Device, PP, VV, 0.0, Kp, Kd);
        Motor28_Control->Send_MIT_PD_Control_Data(Motor28_Device, PP, VV, 0.0, Kp, Kd);

        // 发送控制缓冲区数据到硬件
        Test_Robot->Send_Buff_Data();

        // 控制周期延时
        usleep(time_step);

        // 读取所有电机的反馈数据（存储到P、V、F数组）
        Motor1_Control->Get_Motor_FB_Data( Motor1_Device,  &P[1],  &V[1],  &F[1]); //从1开始
        Motor2_Control->Get_Motor_FB_Data( Motor2_Device,  &P[2],  &V[2],  &F[2]);
        Motor3_Control->Get_Motor_FB_Data( Motor3_Device,  &P[3],  &V[3],  &F[3]);
        Motor4_Control->Get_Motor_FB_Data( Motor4_Device,  &P[4],  &V[4],  &F[4]);
        Motor5_Control->Get_Motor_FB_Data( Motor5_Device,  &P[5],  &V[5],  &F[5]);
        Motor6_Control->Get_Motor_FB_Data( Motor6_Device,  &P[6],  &V[6],  &F[6]);
        Motor7_Control->Get_Motor_FB_Data( Motor7_Device,  &P[7],  &V[7],  &F[7]);
        Motor8_Control->Get_Motor_FB_Data( Motor8_Device,  &P[8],  &V[8],  &F[8]);
        Motor9_Control->Get_Motor_FB_Data( Motor9_Device,  &P[9],  &V[9],  &F[9]);
        Motor10_Control->Get_Motor_FB_Data(Motor10_Device, &P[10], &V[10], &F[10]);
        Motor11_Control->Get_Motor_FB_Data(Motor11_Device, &P[11], &V[11], &F[11]);
        Motor12_Control->Get_Motor_FB_Data(Motor12_Device, &P[12], &V[12], &F[12]);
        Motor13_Control->Get_Motor_FB_Data(Motor13_Device, &P[13], &V[13], &F[13]);
        Motor14_Control->Get_Motor_FB_Data(Motor14_Device, &P[14], &V[14], &F[14]);
        Motor15_Control->Get_Motor_FB_Data(Motor15_Device, &P[15], &V[15], &F[15]);
        Motor16_Control->Get_Motor_FB_Data(Motor16_Device, &P[16], &V[16], &F[16]);
        Motor17_Control->Get_Motor_FB_Data(Motor17_Device, &P[17], &V[17], &F[17]);
        Motor18_Control->Get_Motor_FB_Data(Motor18_Device, &P[18], &V[18], &F[18]);
        Motor19_Control->Get_Motor_FB_Data(Motor19_Device, &P[19], &V[19], &F[19]);
        Motor20_Control->Get_Motor_FB_Data(Motor20_Device, &P[20], &V[20], &F[20]);
        Motor21_Control->Get_Motor_FB_Data(Motor21_Device, &P[21], &V[21], &F[21]);
        Motor22_Control->Get_Motor_FB_Data(Motor22_Device, &P[22], &V[22], &F[22]);
        Motor23_Control->Get_Motor_FB_Data(Motor23_Device, &P[23], &V[23], &F[23]);
        Motor24_Control->Get_Motor_FB_Data(Motor24_Device, &P[24], &V[24], &F[24]);
        Motor25_Control->Get_Motor_FB_Data(Motor25_Device, &P[25], &V[25], &F[25]);
        Motor26_Control->Get_Motor_FB_Data(Motor26_Device, &P[26], &V[26], &F[26]);
        Motor27_Control->Get_Motor_FB_Data(Motor27_Device, &P[27], &V[27], &F[27]);
        Motor28_Control->Get_Motor_FB_Data(Motor28_Device, &P[28], &V[28], &F[28]);

        // // 打印电机反馈数据（当前仅打印P[0]和V[0]，此处可能存在索引偏移，实际应用中需根据需求调整）
        // cout << "Post:" << P[1] << " Speed:" << V[1] << endl; //从1开始
    }
#endif

    return 0;
}
