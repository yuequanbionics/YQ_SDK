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

int main(int argc, char *argv[])
{
    Robot_Hardware *Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type("Switch_Board", Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    Test_Robot->Add_Device_Type("Motor_Device", Motor_Device_Init, Motor_Device_CallBack_F, Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
 

    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;

    string ADDR = dir_path.string() + "/../config/YAML/Y_Hand/out/TOP.yaml";
    if(Test_Robot->Init_TOP(ADDR) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

    shared_ptr<Device_Struct>  Motor1_Device = Test_Robot->Get_Device_For_Name("Motor_1");
    shared_ptr<Device_Struct>  Motor2_Device = Test_Robot->Get_Device_For_Name("Motor_2");
    shared_ptr<Device_Struct>  Motor3_Device = Test_Robot->Get_Device_For_Name("Motor_3");
    shared_ptr<Device_Struct>  Motor4_Device = Test_Robot->Get_Device_For_Name("Motor_4");
    shared_ptr<Device_Struct>  Motor5_Device = Test_Robot->Get_Device_For_Name("Motor_5");
    shared_ptr<Device_Struct>  Motor6_Device = Test_Robot->Get_Device_For_Name("Motor_6");
    shared_ptr<Device_Struct>  Motor7_Device = Test_Robot->Get_Device_For_Name("Motor_7");
    shared_ptr<Device_Struct>  Motor8_Device = Test_Robot->Get_Device_For_Name("Motor_8");
    shared_ptr<Device_Struct>  Motor9_Device = Test_Robot->Get_Device_For_Name("Motor_9");
    shared_ptr<Device_Struct> Motor10_Device = Test_Robot->Get_Device_For_Name("Motor_10");
    shared_ptr<Device_Struct> Motor11_Device = Test_Robot->Get_Device_For_Name("Motor_11");
    shared_ptr<Device_Struct> Motor12_Device = Test_Robot->Get_Device_For_Name("Motor_12");
    shared_ptr<Device_Struct> Motor13_Device = Test_Robot->Get_Device_For_Name("Motor_13");
    shared_ptr<Device_Struct> Motor14_Device = Test_Robot->Get_Device_For_Name("Motor_14");
    shared_ptr<Device_Struct> Motor15_Device = Test_Robot->Get_Device_For_Name("Motor_15");
    shared_ptr<Device_Struct> Motor16_Device = Test_Robot->Get_Device_For_Name("Motor_16");
    shared_ptr<Device_Struct> Motor17_Device = Test_Robot->Get_Device_For_Name("Motor_17");
    shared_ptr<Device_Struct> Motor18_Device = Test_Robot->Get_Device_For_Name("Motor_18");
    shared_ptr<Device_Struct> Motor19_Device = Test_Robot->Get_Device_For_Name("Motor_19");
    shared_ptr<Device_Struct> Motor20_Device = Test_Robot->Get_Device_For_Name("Motor_20");
    shared_ptr<Device_Struct> Motor21_Device = Test_Robot->Get_Device_For_Name("Motor_21");
    shared_ptr<Device_Struct> Motor22_Device = Test_Robot->Get_Device_For_Name("Motor_22");
    shared_ptr<Device_Struct> Motor23_Device = Test_Robot->Get_Device_For_Name("Motor_23");
    shared_ptr<Device_Struct> Motor24_Device = Test_Robot->Get_Device_For_Name("Motor_24");
    shared_ptr<Device_Struct> Motor25_Device = Test_Robot->Get_Device_For_Name("Motor_25");
    shared_ptr<Device_Struct> Motor26_Device = Test_Robot->Get_Device_For_Name("Motor_26");
    shared_ptr<Device_Struct> Motor27_Device = Test_Robot->Get_Device_For_Name("Motor_27");
    shared_ptr<Device_Struct> Motor28_Device = Test_Robot->Get_Device_For_Name("Motor_28");


    Motor * Motor1_Control = (Motor*)Test_Robot->Get_Control_Class( Motor1_Device);
    Motor * Motor2_Control = (Motor*)Test_Robot->Get_Control_Class( Motor2_Device);
    Motor * Motor3_Control = (Motor*)Test_Robot->Get_Control_Class( Motor3_Device);
    Motor * Motor4_Control = (Motor*)Test_Robot->Get_Control_Class( Motor4_Device);
    Motor * Motor5_Control = (Motor*)Test_Robot->Get_Control_Class( Motor5_Device);
    Motor * Motor6_Control = (Motor*)Test_Robot->Get_Control_Class( Motor6_Device);
    Motor * Motor7_Control = (Motor*)Test_Robot->Get_Control_Class( Motor7_Device);
    Motor * Motor8_Control = (Motor*)Test_Robot->Get_Control_Class( Motor8_Device);
    Motor * Motor9_Control = (Motor*)Test_Robot->Get_Control_Class( Motor9_Device);
    Motor *Motor10_Control = (Motor*)Test_Robot->Get_Control_Class(Motor10_Device);
    Motor *Motor11_Control = (Motor*)Test_Robot->Get_Control_Class(Motor11_Device);
    Motor *Motor12_Control = (Motor*)Test_Robot->Get_Control_Class(Motor12_Device);
    Motor *Motor13_Control = (Motor*)Test_Robot->Get_Control_Class(Motor13_Device);
    Motor *Motor14_Control = (Motor*)Test_Robot->Get_Control_Class(Motor14_Device);
    Motor *Motor15_Control = (Motor*)Test_Robot->Get_Control_Class(Motor15_Device);
    Motor *Motor16_Control = (Motor*)Test_Robot->Get_Control_Class(Motor16_Device);
    Motor *Motor17_Control = (Motor*)Test_Robot->Get_Control_Class(Motor17_Device);
    Motor *Motor18_Control = (Motor*)Test_Robot->Get_Control_Class(Motor18_Device);
    Motor *Motor19_Control = (Motor*)Test_Robot->Get_Control_Class(Motor19_Device);
    Motor *Motor20_Control = (Motor*)Test_Robot->Get_Control_Class(Motor20_Device);
    Motor *Motor21_Control = (Motor*)Test_Robot->Get_Control_Class(Motor21_Device);
    Motor *Motor22_Control = (Motor*)Test_Robot->Get_Control_Class(Motor22_Device);
    Motor *Motor23_Control = (Motor*)Test_Robot->Get_Control_Class(Motor23_Device);
    Motor *Motor24_Control = (Motor*)Test_Robot->Get_Control_Class(Motor24_Device);
    Motor *Motor25_Control = (Motor*)Test_Robot->Get_Control_Class(Motor25_Device);
    Motor *Motor26_Control = (Motor*)Test_Robot->Get_Control_Class(Motor26_Device);
    Motor *Motor27_Control = (Motor*)Test_Robot->Get_Control_Class(Motor27_Device);
    Motor *Motor28_Control = (Motor*)Test_Robot->Get_Control_Class(Motor28_Device);

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

        // PP = 0; 
        VV = 0;     
        TT = -0.03;
    
        // int Send_MIT_PD_Control_Data(shared_ptr<Device_Struct> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);        // 上面的指令向发送缓冲区添加 MIT PD控制指令（参数：设备实例、目标位置、目标速度、目标力、KP增益、KD增益）
        /**
         * @brief Send_MIT_PD_Control_Data 发送MIT PD控制指令到指定电机，实现基于位置和速度的闭环控制
         * 
         * 该函数是Motor类提供的MIT（麻省理工学院）风格PD控制器接口，通过输入目标状态（位置、速度）、
         * 控制参数（比例增益、微分增益）和力指令，计算并发送控制信号到电机，实现高精度闭环控制。
         * MIT PD控制策略常用于机器人关节、精密电机等场景，核心是通过比例项（位置误差）和微分项（速度误差）
         * 抑制系统震荡，提高响应速度和控制精度。
         * 
         * @param Device_P 电机设备结构体智能指针（shared_ptr<Device_Struct>）
         *                 - 用于指定目标电机，包含电机硬件地址、设备ID、通信参数等设备信息
         *                 - 由Robot_Hardware类通过Get_Device_For_Name()方法获取，与配置文件中的电机名称绑定
         * 
         * @param Rad 目标位置（单位：弧度，rad）
         *            - 电机需要达到的期望位置，在示例中通过正弦函数动态生成（PP = sin(S)*2.8f）
         *            - 需与电机实际可转动范围匹配，超出范围可能导致机械限位或控制失效
         * 
         * @param Speed_Rad_S 目标速度（单位：弧度/秒，rad/s）
         *                    - 电机在运动过程中的期望速度，示例中通过余弦函数生成（VV = cos(S)*2.8f）
         *                    - 与目标位置配合，决定电机运动的平滑度和动态特性
         * 
         * @param Force_N 前馈力/电流指令（单位：牛顿·米，N·m，此处参数名Force_N可能简化标注）
         *                - 用于补偿系统已知负载或摩擦力的前馈控制量，示例中设为0（不启用前馈）
         *                - 合理设置可减少PD控制器的负担，提高动态响应速度
         * 
         * @param P_N_Rad 比例增益（单位：牛/弧度，N/rad）
         *                - 用于放大位置误差（目标位置 - 实际位置），比例项输出 = P_N_Rad * (Rad - 实际位置)
         *                - 示例中设置为800，数值越大，位置误差的修正力度越强，但过大会导致系统震荡
         * 
         * @param D_N_Rad_s 微分增益（单位：牛·秒/弧度，N·s/rad）
         *                  - 用于抑制速度变化，微分项输出 = D_N_Rad_s * (Speed_Rad_S - 实际速度)
         *                  - 示例中设置为10，数值越大，系统阻尼越强，可抑制震荡，但过大会导致响应迟缓
         * 
         * @return int 暂无意义
         * 
         * 控制逻辑说明：
         * 电机实际输出的控制量（力/电流）计算公式约为：
         * 控制量 = Force_N + P_N_Rad*(Rad - 实际位置) + D_N_Rad_s*(Speed_Rad_S - 实际速度)
         * 其中，比例项（P）负责减小位置偏差，微分项（D）负责抑制速度波动，前馈力（Force_N）用于预先补偿已知干扰。
         * 
         * 示例中应用场景：
         * 所有24个电机均使用相同的目标轨迹（正弦位置+余弦速度）、零前馈力、固定P=800和D=10，实现同步周期性运动，
         * 常用于测试多电机协同控制性能或硬件一致性。
         */
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
    
    return 0;
}
