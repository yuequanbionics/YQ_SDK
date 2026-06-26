#include "syst.hpp"
#include "UDP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Custom_TOP.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "Auto_Set_Id.hpp"
#include "Led_Device.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <math.h>
#include "unistd.h"
#include <filesystem>
#include <iomanip> 

using namespace std;

Robot_Hardware *Test_Robot;

shared_ptr<Device_class>  Led_Devices;
shared_ptr<Device_class>  Motor1_Device;
shared_ptr<Device_class>  Motor2_Device;
shared_ptr<Device_class>  Motor3_Device;
shared_ptr<Device_class>  Motor4_Device;
shared_ptr<Device_class>  Motor5_Device;
shared_ptr<Device_class>  Motor6_Device;
shared_ptr<Device_class>  Motor7_Device;
shared_ptr<Device_class>  Motor8_Device;
shared_ptr<Device_class>  Motor9_Device;
shared_ptr<Device_class> Motor10_Device;
shared_ptr<Device_class> Motor11_Device;
shared_ptr<Device_class> Motor12_Device;
shared_ptr<Device_class> Motor13_Device;
shared_ptr<Device_class> Motor14_Device;


Led_Device *Led_Device_A_Ptr;
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


int hand_pretighten(float F, uint32_t time_ms)
{
    float P[30], V[30], FF[30], TEMP[30][2];
    u16 errorno = 0;

    F = -clamp(fabs(F), 0.0f, 0.25f);
    
    Motor1_Control->Send_MIT_PD_Control_Data( Motor1_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor2_Control->Send_MIT_PD_Control_Data( Motor2_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor3_Control->Send_MIT_PD_Control_Data( Motor3_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor4_Control->Send_MIT_PD_Control_Data( Motor4_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor5_Control->Send_MIT_PD_Control_Data( Motor5_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor6_Control->Send_MIT_PD_Control_Data( Motor6_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor7_Control->Send_MIT_PD_Control_Data( Motor7_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor8_Control->Send_MIT_PD_Control_Data( Motor8_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor9_Control->Send_MIT_PD_Control_Data( Motor9_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor10_Control->Send_MIT_PD_Control_Data(Motor10_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor11_Control->Send_MIT_PD_Control_Data(Motor11_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor12_Control->Send_MIT_PD_Control_Data(Motor12_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor13_Control->Send_MIT_PD_Control_Data(Motor13_Device, 0.0, 0.0, F, 0.0, 0.0);
    Motor14_Control->Send_MIT_PD_Control_Data(Motor14_Device, 0.0, 0.0, F, 0.0, 0.0);

    Test_Robot->Send_Buff_Data();
    usleep(10000);
    Motor1_Control->Set_Zero( Motor1_Device, 0.0f);
    Motor2_Control->Set_Zero( Motor2_Device, 0.0f);
    Motor3_Control->Set_Zero( Motor3_Device, 0.0f);
    Motor4_Control->Set_Zero( Motor4_Device, 0.0f);
    Motor5_Control->Set_Zero( Motor5_Device, 0.0f);
    Motor6_Control->Set_Zero( Motor6_Device, 0.0f);
    Motor7_Control->Set_Zero( Motor7_Device, 0.0f);
    Motor8_Control->Set_Zero( Motor8_Device, 0.0f);
    Motor9_Control->Set_Zero( Motor9_Device, 0.0f);
    Motor10_Control->Set_Zero(Motor10_Device, 0.0f);
    Motor11_Control->Set_Zero(Motor11_Device, 0.0f);
    Motor12_Control->Set_Zero(Motor12_Device, 0.0f);
    Motor13_Control->Set_Zero(Motor13_Device, 0.0f);
    Motor14_Control->Set_Zero(Motor14_Device, 0.0f);


    // 读取所有电机的反馈数据（存储到P、V、F数组）
    Motor1_Control->Get_Motor_FB_Data( Motor1_Device,  &P[1],  &V[1],  &FF[1],  TEMP[1],  &errorno); //从1开始
    Motor2_Control->Get_Motor_FB_Data( Motor2_Device,  &P[2],  &V[2],  &FF[2],  TEMP[2],  &errorno);
    Motor3_Control->Get_Motor_FB_Data( Motor3_Device,  &P[3],  &V[3],  &FF[3],  TEMP[3],  &errorno);
    Motor4_Control->Get_Motor_FB_Data( Motor4_Device,  &P[4],  &V[4],  &FF[4],  TEMP[4],  &errorno);
    Motor5_Control->Get_Motor_FB_Data( Motor5_Device,  &P[5],  &V[5],  &FF[5],  TEMP[5],  &errorno);
    Motor6_Control->Get_Motor_FB_Data( Motor6_Device,  &P[6],  &V[6],  &FF[6],  TEMP[6],  &errorno);
    Motor7_Control->Get_Motor_FB_Data( Motor7_Device,  &P[7],  &V[7],  &FF[7],  TEMP[7],  &errorno);
    Motor8_Control->Get_Motor_FB_Data( Motor8_Device,  &P[8],  &V[8],  &FF[8],  TEMP[8],  &errorno);
    Motor9_Control->Get_Motor_FB_Data( Motor9_Device,  &P[9],  &V[9],  &FF[9],  TEMP[9],  &errorno);
    Motor10_Control->Get_Motor_FB_Data(Motor10_Device, &P[10], &V[10], &FF[10], TEMP[10], &errorno);
    Motor11_Control->Get_Motor_FB_Data(Motor11_Device, &P[11], &V[11], &FF[11], TEMP[11], &errorno);
    Motor12_Control->Get_Motor_FB_Data(Motor12_Device, &P[12], &V[12], &FF[12], TEMP[12], &errorno);
    Motor13_Control->Get_Motor_FB_Data(Motor13_Device, &P[13], &V[13], &FF[13], TEMP[13], &errorno);
    Motor14_Control->Get_Motor_FB_Data(Motor14_Device, &P[14], &V[14], &FF[14], TEMP[14], &errorno);


    // // 打印电机反馈数据（当前仅打印P[0]和V[0]，此处可能存在索引偏移，实际应用中需根据需求调整）
    // cout << "Post:" << P[1] << " Speed:" << V[1] << endl; //从1开始
    for (uint8_t i = 1; i < 15; i++) {
        // 核心格式控制：固定宽度8位 + 保留3位小数 + 右对齐（保证正负号对齐）
        cout << "motor_"       << setw(2) << setfill(' ') << static_cast<int>(i)  // 电机编号占2位，补空格
                << ": Pos = "     << setw(8) << fixed << setprecision(3) << P[i]   // 数值固定8位宽，3位小数
                << ", Vel = "     << setw(8) << fixed << setprecision(3) << V[i]
                << ", Current = " << setw(8) << fixed << setprecision(3) << FF[i]
                << ", Temp = "    << setw(8) << fixed << setprecision(3) << TEMP[i][1]
                << endl;
        // 重置格式（避免影响后续输出，可选但建议保留）
        cout << resetiosflags(ios::fixed) << setprecision(6);

        if (i == 14)
            cout << "------------------" << endl;
    }
    return 0;
}


#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(const string& ADDR, const string& Config)
#endif
{
    Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    Test_Robot->Add_Device_Type(Led_Device_TYPE, Led_Device_Init, Led_Device_CallBack_F, Led_Device_Delete_F);
    Test_Robot->Add_Device_Type(Motor_Device_Type, Motor_Device_Init, Motor_Device_CallBack_F, Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
 
#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/Y_Hand_M1/out/TOP.yaml";
    string Config = "None";
//     string Config = R"(
// PC_IP: 192.168.3.245
// SN: asdf1234567
// Boards:
//   - Id: 101
//     IP: 192.168.3.105
//     # Port: 19001
// )";
#endif
    if(Test_Robot->Init_TOP(ADDR, Config) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;
    Led_Devices = Test_Robot->Get_Device_For_Name("Led_GoGo");
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


    Led_Device_A_Ptr = (Led_Device*)Test_Robot->Get_Control_Class( Led_Devices);
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



#ifndef HAVE_ROS
    float S = 0;                   // 正弦函数相位变量（随时间递增）
    float P[30], V[30], F[30];     // 存储电机反馈数据：位置(Position)、速度(Velocity)、力(电流 0-100 参数为百分比*100) //从1开始
    float Kp = 0;
    float Kd = 0;
    int times = 0;
    float PP_old = 0;
    constexpr int time_step = 5000*1000/500;
    // 主控制循环（持续运行）
    while (1)
    {   
        hand_pretighten(0.05, 30);

        // 更新相位并计算目标控制量（正弦轨迹）
        // S += 0.0125f;                                                                 // 位置步长, 角度
        // float PP = sin(S) * 90.0f - 100;                                            // MIT 目标控制位置
        // float VV = (PP - PP_old) * (static_cast<float>(1000000) / time_step);  // MIT 控制目标速度, 它是位置的微分
        // float TT = -sin(S)*0.5f;   

        // PP = 0;
        // VV = 0;
        // TT = 0;
    
        //  Motor1_Control->Send_MIT_PD_Control_Data( Motor1_Device, PP, VV, 0, Kp, Kd);
        //  Motor2_Control->Send_MIT_PD_Control_Data( Motor2_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor3_Control->Send_MIT_PD_Control_Data( Motor3_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor4_Control->Send_MIT_PD_Control_Data( Motor4_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor5_Control->Send_MIT_PD_Control_Data( Motor5_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor6_Control->Send_MIT_PD_Control_Data( Motor6_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor7_Control->Send_MIT_PD_Control_Data( Motor7_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor8_Control->Send_MIT_PD_Control_Data( Motor8_Device, PP, VV, 0.0, Kp, Kd);
        //  Motor9_Control->Send_MIT_PD_Control_Data( Motor9_Device, PP, VV, 0.0, Kp, Kd);
        // Motor10_Control->Send_MIT_PD_Control_Data(Motor10_Device, PP, VV, 0.0, Kp, Kd);
        // Motor11_Control->Send_MIT_PD_Control_Data(Motor11_Device, PP, VV, 0.0, Kp, Kd);
        // Motor12_Control->Send_MIT_PD_Control_Data(Motor12_Device, PP, VV, 0.0, Kp, Kd);
        // Motor13_Control->Send_MIT_PD_Control_Data(Motor13_Device, PP, VV, 0.0, Kp, Kd);
        // Motor14_Control->Send_MIT_PD_Control_Data(Motor14_Device, PP, VV, 0.0, Kp, Kd);

        // // 发送控制缓冲区数据到硬件
        // Test_Robot->Send_Buff_Data();

        // // 控制周期延时
        // usleep(time_step);

        // // 读取所有电机的反馈数据（存储到P、V、F数组）
        // Motor1_Control->Get_Motor_FB_Data( Motor1_Device,  &P[1],  &V[1],  &F[1]); //从1开始
        // Motor2_Control->Get_Motor_FB_Data( Motor2_Device,  &P[2],  &V[2],  &F[2]);
        // Motor3_Control->Get_Motor_FB_Data( Motor3_Device,  &P[3],  &V[3],  &F[3]);
        // Motor4_Control->Get_Motor_FB_Data( Motor4_Device,  &P[4],  &V[4],  &F[4]);
        // Motor5_Control->Get_Motor_FB_Data( Motor5_Device,  &P[5],  &V[5],  &F[5]);
        // Motor6_Control->Get_Motor_FB_Data( Motor6_Device,  &P[6],  &V[6],  &F[6]);
        // Motor7_Control->Get_Motor_FB_Data( Motor7_Device,  &P[7],  &V[7],  &F[7]);
        // Motor8_Control->Get_Motor_FB_Data( Motor8_Device,  &P[8],  &V[8],  &F[8]);
        // Motor9_Control->Get_Motor_FB_Data( Motor9_Device,  &P[9],  &V[9],  &F[9]);
        // Motor10_Control->Get_Motor_FB_Data(Motor10_Device, &P[10], &V[10], &F[10]);
        // Motor11_Control->Get_Motor_FB_Data(Motor11_Device, &P[11], &V[11], &F[11]);
        // Motor12_Control->Get_Motor_FB_Data(Motor12_Device, &P[12], &V[12], &F[12]);
        // Motor13_Control->Get_Motor_FB_Data(Motor13_Device, &P[13], &V[13], &F[13]);
        // Motor14_Control->Get_Motor_FB_Data(Motor14_Device, &P[14], &V[14], &F[14]);


        // // // 打印电机反馈数据（当前仅打印P[0]和V[0]，此处可能存在索引偏移，实际应用中需根据需求调整）
        //     // for (int i = 0; i < 14; i++) {
        //     //     int motorNo = i + 1;
        //     //     cout << "Motor: "
        //     //         << setw(2) << setfill('0') << motorNo // 核心：两位宽，补0
        //     //         << " Post:" << P[i+1]
        //     //         << " F:" << F[i+1]
        //     //         << endl;
        //     // }
    }
#endif

    return 0;
}
