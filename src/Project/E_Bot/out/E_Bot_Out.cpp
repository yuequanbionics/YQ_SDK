//
// Created by lwz on 2025/12/26.
//
#include <filesystem>
#include <iostream>
#include <iomanip>

#include "syst.hpp"
#include "UDP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Switch_Board.hpp"
#include "Feetech_SCS0009.hpp"
#include "E_Hand.hpp"

using namespace std;

Robot_Hardware *E_Hand;

shared_ptr<Device_Struct> Device_Servo_SCS0009;
Servo_SCS0009 *Servo_Ctrl;

#ifndef HAVE_ROS
int main(int argc, char *argv[])
#else
int hardware_init(string ADDR)
#endif
{
    E_Hand = new Robot_Hardware();

    E_Hand->Add_Device_Type("Switch_Board",
                        Switch_Board_Device_Init,
                        Switch_Board_Device_CallBack_F,
                        Switch_Board_Device_Delete_F);

    E_Hand->Add_Device_Type("Servo_Custom_Feetech_SCS0009",
                               Feetech_Custom_Servo_Device_Init,
                               Feetech_Custom_Servo_Device_CallBack_F,
                               Feetech_Custom_Servo_Device_Delete_F);

#ifndef HAVE_ROS
    const filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    const filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;

    const string ADDR = dir_path.string() + "/../config/YAML/E_Bot/li_wenzhi/Servo_Config/TOP.yaml";
#endif

    if (E_Hand->Init_TOP(ADDR) != 0)
    {
        cout << "Init_ERR" << endl;
        return -1;
    }

    Device_Servo_SCS0009 = E_Hand->Get_Device_For_Name("Servo_Feetech_SCS0009");
    Servo_Ctrl = static_cast<Servo_SCS0009 *>(E_Hand->Get_Control_Class(Device_Servo_SCS0009));

}

