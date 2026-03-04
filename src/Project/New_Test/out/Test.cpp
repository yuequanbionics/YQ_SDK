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

int ERRORFB(shared_ptr<Device_class> Device, std::string Info, Err_Level Level)
{
    for (auto ERR_MAP = Error_Map(Device).begin(); ERR_MAP != Error_Map(Device).end(); ERR_MAP++)
    {
        cout << ERR_MAP->first << " Level:" << ERR_MAP->second << endl;
    }
    return 0;
}

// 测试
int main(int argc, char *argv[])
{
    UDP_Def UDP_Def_one;
    Init_One_UDP_Connect(&UDP_Def_one, inet_addr("127.0.0.1"), htons(16201));

    Robot_Hardware *Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type("Switch_Board", Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    Test_Robot->Add_Device_Type("Motor_Device", Motor_Device_Init, Motor_Device_CallBack_F, Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
 

    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;

string Config = R"(
PC_IP: 192.168.3.245
SN: qwer8523578
Boards:
  - Id: 101
    IP: 192.168.3.105
    # Port: 19001
)";

    string ADDR = dir_path.string() + "/../config/YAML/New_Test/out/TOP.yaml";
    if(Test_Robot->Init_TOP(ADDR, Config) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // cout << (Test_Robot->Get_Yaml_Node())["Log_Max_Size"] << endl;

    // string ADDR_OTA = "/home/toko/SP/rhsyd/config/OTA_BAG/OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

    shared_ptr<Device_class> Sw_Device = Test_Robot->Get_Device_For_Name("Main_Switch_Board");
    Main_B *Sw_Control = (Main_B*)Test_Robot->Get_Control_Class(Sw_Device);
    Sw_Device->Error_FB = ERRORFB;
    
    shared_ptr<std::map<Motor *, shared_ptr<Device_class>>> Motor_Map = make_shared<std::map<Motor *, shared_ptr<Device_class>>>();
    for (auto Device = Test_Robot->Get_Device_Map()->begin(); Device != Test_Robot->Get_Device_Map()->end(); ++Device)
    {
        cout << "Device_Name: " << Device->first << " ID: " << Device->second->Device_Main_Data.Main_Can_ID << endl;

        shared_ptr<Device_class> Motor_Device = Device->second;
        if(*Motor_Device->DeviceType == "Motor_Device")
        {
            Motor *Motor_Control = static_cast<Motor*>(Test_Robot->Get_Control_Class(Device->second));
            Motor_Map->insert(std::make_pair(Motor_Control, Motor_Device));
            Motor_Device->Have_New_Msg = 0;
        }
    }

    float S = 0;
    float P, V, F;
    float temp[2];
    u16 error;

    int times = 0;
    float PP_old = 0;

    /* 时间步长, 主控 CANFD 1M+2M, 收发一条 CAN 消息耗时约 350μs, 24 个电机乘 24, 实测可减至 22.5, 即每增加一个电机步长增加 328μs */
    constexpr int time_step = 1000*1000/500;

    while (1)
    {   
        S += 0.0125f;                                                                 // 位置步长, 角度
        const float PP = sin(S) * 180.0f;                                            // MIT 目标控制位置
        const float VV = (PP - PP_old) * (static_cast<float>(1000000) / time_step);  // MIT 控制目标速度, 它是位置的微分
        float TT = -sin(S)*0.5f;                                                     // 电流前馈 A

        PP_old = PP;

        std::ostringstream oss;
        // oss << PP << ",";
        for (auto Device = Motor_Map->begin(); Device != Motor_Map->end(); ++Device)
        {
            // Device->first->Motor_Cal_Start(Device->second);
            // while(1);
            // Device->first->IC_MU_CAL(Device->second, &UDP_Def_one, 1);
            // while(1);
            Device->first->Send_MIT_PD_Control_Data(Device->second, 0, 50, 0, 0, 500);
            // cout <<  Device->second->Have_New_Msg << endl;
            Device->first->Get_Motor_FB_Data(Device->second, &P, &V, &F, temp, &error);
            // oss << PP << ",";
            // oss << P << ",";
            // oss << VV << ",";
            // oss << V;
            cout << V << " I:" << F << endl;
        } 

        Test_Robot->Send_Buff_Data();
        usleep(time_step);

        oss << "\r\n";
        std::string message = oss.str();
        UDP_Send_Data(&UDP_Def_one, (uint8_t*)message.c_str(), message.length());
    }

    return 0;
}
