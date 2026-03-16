 /**
 * @file X_Hand_Control_Demo.cpp
 * @brief 调用 X_Hand_Protocol 驱动，实现对 6 个电机角度的控制示例
 *        使用左手 CAN ID 0x201~0x206,归一化角度 0~1 控制
 *        Ctrl+C 退出时先让 6 电机伸直再正常退出
 */

#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <thread>

#include "Auto_Set_Id.hpp"
#include "X_Hand_Protocol.hpp"
#include "HARDWARE_TOP.hpp"
#include "Switch_Board.hpp"
#include "syst.hpp"

using namespace std;

static volatile int g_need_exit = 0;
float angle;

Robot_Hardware* robot;
shared_ptr<Device_class> X_Hand_1_D;
shared_ptr<Device_class> X_Hand_2_D;
shared_ptr<Device_class> X_Hand_3_D;
shared_ptr<Device_class> X_Hand_4_D;
shared_ptr<Device_class> X_Hand_5_D;
shared_ptr<Device_class> X_Hand_6_D;
X_Hand_Protocol* X_Hand_1_Control;
X_Hand_Protocol* X_Hand_2_Control;
X_Hand_Protocol* X_Hand_3_Control;
X_Hand_Protocol* X_Hand_4_Control;
X_Hand_Protocol* X_Hand_5_Control;
X_Hand_Protocol* X_Hand_6_Control;

static void sigint_handler(int) {
    g_need_exit = 1;
}

inline void reset (){
    angle = 0.0f;
    X_Hand_1_Control->Set_Motor_Angle(angle, 0.0f);
    X_Hand_2_Control->Set_Motor_Angle(angle, 0.0f);
    X_Hand_3_Control->Set_Motor_Angle(angle, 0.0f);
    X_Hand_4_Control->Set_Motor_Angle(angle, 0.0f);
    X_Hand_5_Control->Set_Motor_Angle(angle, 0.0f);
    X_Hand_6_Control->Set_Motor_Angle(angle, 0.0f);
    robot->Send_Buff_Data();
    sleep(1);
}

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(string ADDR, string Config)
#endif
{
    robot = new Robot_Hardware();
    robot->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init,
                          Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    robot->Add_Device_Type(X_Hand_Protocol_Device_Type,
                          X_Hand_Protocol_Device_Init,
                          X_Hand_Protocol_Device_CallBack_F,
                          X_Hand_Protocol_Device_Delete_F);

#ifndef HAVE_ROS                         
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    filesystem::path dir_path = exe_path.parent_path();
    string ADDR = dir_path.string() + "/../config/YAML/X_Hand_Brushed/out/TOP.yaml";
    string Config = "None";
#endif
    if (robot->Init_TOP(ADDR, Config) != 0) {
        cout << "Init_ERR" << endl;
        return -1;
    }

    X_Hand_1_D = robot->Get_Device_For_Name("X_Hand_Protocol_1");
    X_Hand_2_D = robot->Get_Device_For_Name("X_Hand_Protocol_2");
    X_Hand_3_D = robot->Get_Device_For_Name("X_Hand_Protocol_3");
    X_Hand_4_D = robot->Get_Device_For_Name("X_Hand_Protocol_4");
    X_Hand_5_D = robot->Get_Device_For_Name("X_Hand_Protocol_5");
    X_Hand_6_D = robot->Get_Device_For_Name("X_Hand_Protocol_6");

    X_Hand_1_Control = (X_Hand_Protocol*)robot->Get_Control_Class(X_Hand_1_D);
    X_Hand_2_Control = (X_Hand_Protocol*)robot->Get_Control_Class(X_Hand_2_D);
    X_Hand_3_Control = (X_Hand_Protocol*)robot->Get_Control_Class(X_Hand_3_D);
    X_Hand_4_Control = (X_Hand_Protocol*)robot->Get_Control_Class(X_Hand_4_D);
    X_Hand_5_Control = (X_Hand_Protocol*)robot->Get_Control_Class(X_Hand_5_D);
    X_Hand_6_Control = (X_Hand_Protocol*)robot->Get_Control_Class(X_Hand_6_D);

    X_Hand_1_Control->Set_CanId(0x201);
    X_Hand_2_Control->Set_CanId(0x202);
    X_Hand_3_Control->Set_CanId(0x203);
    X_Hand_4_Control->Set_CanId(0x204);
    X_Hand_5_Control->Set_CanId(0x205);
    X_Hand_6_Control->Set_CanId(0x206);

    signal(SIGINT, sigint_handler);
    cout << "X_Hand 6-motor control demo (left hand, 0x201~0x206). angle 0=straight, 1=bent. Ctrl+C to exit (hand will straighten)." << endl;

#ifndef HAVE_ROS

    const int loop_hz = 1;
    const int loop_us = 1000000 / loop_hz;
    float t = 0.0f;

    reset();
    
    while (!g_need_exit) {
        t += 0.01f;
        if (t > 6.28318f)
            t -= 6.28318f;

        angle = 0.5f + 0.4f * sin(t + 1 * 0.5f);
        X_Hand_1_Control->Set_Motor_Angle(angle, 0.0f);
        angle = 0.5f + 0.4f * sin(t + 2 * 0.5f);
        X_Hand_2_Control->Set_Motor_Angle(angle, 0.0f);
        angle = 0.5f + 0.4f * sin(t + 3 * 0.5f);
        X_Hand_3_Control->Set_Motor_Angle(angle, 0.0f);
        angle = 0.5f + 0.4f * sin(t + 4 * 0.5f);
        X_Hand_4_Control->Set_Motor_Angle(angle, 0.0f);
        angle = 0.5f + 0.4f * sin(t + 5 * 0.5f);
        X_Hand_5_Control->Set_Motor_Angle(angle, 0.0f);
        angle = 0.5f + 0.4f * sin(t + 6 * 0.5f);
        X_Hand_6_Control->Set_Motor_Angle(angle, 0.0f);

        robot->Send_Buff_Data();
        usleep(loop_us);
    }
#endif

    /* Ctrl+C 后：6 电机伸直，等 1 秒再退出 */
    reset();

    return 0;
}
