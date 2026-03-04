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
#include "GPIO.hpp"

using namespace std;

Robot_Hardware *Test_Robot;

int R_IO[32];
int W_IO[32];

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(string ADDR, string Config)
#endif
{
    Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);

#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/IO_Board/out/TOP.yaml";
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
    if(Test_Robot->Init_TOP(ADDR) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

    /* ------------- Switch Board ------------- */
    const shared_ptr<Device_class> Main_Switch_Board = Test_Robot->Get_Device_For_Name("Main_Switch_Board");
    Main_B *Main_Switch_Board_Control = static_cast<Main_B*>(Test_Robot->Get_Control_Class(Main_Switch_Board));

    /* ------------- GPIO 配置 ------------- */
    // 方式一: 直接初始化
    GPIO_InitTypeDef GPIO_Init_Struct = {};
    GPIO_Init_Struct.Pin = GPIO_PIN_10;
    GPIO_Init_Struct.Mode = GPIO_MODE_INPUT;
    GPIO_Init_Struct.Pull = GPIO_PULLUP;
    GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH;
    Main_Switch_Board_Control->m_GPIO.GPIOx_Init(Main_Switch_Board, GPIOA, &GPIO_Init_Struct);

    // 方式二: 调用现有接口(仅配置为输入模式)
    Main_Switch_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(Main_Switch_Board, GPIOA, GPIO_PIN_9);


    // Main_Switch_Board_Control->m_GPIO.GPIOx_Read(Main_Switch_Board, GPIOA, GPIO_PIN_10, 1000);

    /* ---------------- End ---------------- */



#ifndef HAVE_ROS
    // ReSharper disable CppDFAEndlessLoop
    while (true)
    // ReSharper restore CppDFAEndlessLoop
    {   
        // Main_Switch_Board_Control->m_GPIO.GPIOx_Read(Main_Switch_Board, GPIOA, GPIO_PIN_10, 1000);
        // Main_Switch_Board_Control->m_GPIO.GPIOx_Read(Main_Switch_Board, GPIOA, GPIO_PIN_9, 1000);

        Main_Switch_Board_Control->m_GPIO.Get_Buttons_Value(Main_Switch_Board, R_IO);

        cout << "PA10: " << R_IO[9]  << endl;
        cout << "PA9 : " << R_IO[10] << endl;
        cout << "--------" << endl;
        usleep(1000);
    }
#endif

    return 0;
}
