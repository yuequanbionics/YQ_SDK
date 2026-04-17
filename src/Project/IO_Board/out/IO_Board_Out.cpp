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


#define A1_PORT     GPIOD
#define A2_PORT     GPIOD
#define A3_PORT     GPIOD
#define A4_PORT     GPIOD
#define A5_PORT     GPIOD
#define A6_PORT     GPIOD

#define A1_PIN      GPIO_PIN_6
#define A2_PIN      GPIO_PIN_5
#define A3_PIN      GPIO_PIN_4
#define A4_PIN      GPIO_PIN_3
#define A5_PIN      GPIO_PIN_2
#define A6_PIN      GPIO_PIN_1


Robot_Hardware *Test_Robot;

/**
 * @brief IO_Board 设备
 */
shared_ptr<Device_class> IO_Board;
Main_B *IO_Board_Control;


/* Local function declaration */
void Get_Buttons_State(const shared_ptr<Device_class>& Device, u8 *Res);


u8 R_IO[32];
u8 W_IO[32];

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else

void ROS_Get_GPIOx_Status() {
    if (IO_Board_Control == nullptr || IO_Board == nullptr)
    {
        cerr << "Error: IO_Board Device not initialized before calling ROS_Get_GPIOx_Status!" << endl;
        return;
    }
    Get_Buttons_State(IO_Board, R_IO);
}

int hardware_init(const string& ADDR, const string& Config)
#endif
{
    /* 1. ------------- 初始化硬件对象 ------------- */
    Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);

    #ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/IO_Board/out/TOP.yaml";
    // string Config = "None";
        string Config = R"(
    PC_IP: 192.168.3.245
    SN: asdf1234567
    Boards:
      - Id: 101

        IP: 192.168.3.243
        # Port: 19001
    )";
    #endif

    /* 2. ------------- 初始化 TOP 配置 ------------- */
    if(Test_Robot->Init_TOP(ADDR, Config) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;


    /* 3. ------------- 获取 IO_Board 设备实例 ------------- */
    IO_Board = Test_Robot->Get_Device_For_Name("IO_Board");
    IO_Board_Control = static_cast<Main_B*>(Test_Robot->Get_Control_Class(IO_Board));


    /* ------------- GPIO 配置 ------------- */
#if 0
    // 方式一: 直接初始化
    GPIO_InitTypeDef GPIO_Init_Struct = {};
    GPIO_Init_Struct.Pin = GPIO_PIN_10;
    GPIO_Init_Struct.Mode = GPIO_MODE_INPUT;
    GPIO_Init_Struct.Pull = GPIO_PULLUP;
    GPIO_Init_Struct.Speed = GPIO_SPEED_FREQ_HIGH;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, GPIOA, &GPIO_Init_Struct);

    // 方式二: 调用现有接口(仅配置为输入模式)
    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, GPIOA, GPIO_PIN_9);
#endif

    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, A6_PORT, A6_PIN);  // A6
    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, A5_PORT, A5_PIN);  // A5
    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, A4_PORT, A4_PIN);  // A4
    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, A3_PORT, A3_PIN);  // A3
    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, A2_PORT, A2_PIN);  // A2
    IO_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(IO_Board, A1_PORT, A1_PIN);  // A1


    // IO_Board_Control->m_GPIO.GPIOx_Read(IO_Board, GPIOA, GPIO_PIN_10, 1000);

    /* ---------------- End ---------------- */



#ifndef HAVE_ROS
    // ReSharper disable CppDFAEndlessLoop
    while (true)
    // ReSharper restore CppDFAEndlessLoop
    {
#if 0
        // IO_Board_Control->m_GPIO.GPIOx_Read(IO_Board, GPIOA, GPIO_PIN_10, 1000);
        // IO_Board_Control->m_GPIO.GPIOx_Read(IO_Board, GPIOA, GPIO_PIN_9, 1000);

        Get_Buttons_State(IO_Board, R_IO);;

        cout << "PA10: " << R_IO[9]  << endl;
        cout << "PA9 : " << R_IO[10] << endl;
        cout << "--------" << endl;
#endif

        Get_Buttons_State(IO_Board, R_IO);

        cout << "A1: " << static_cast<int>(R_IO[1])  << endl;
        cout << "A2: " << static_cast<int>(R_IO[2])  << endl;
        cout << "A3: " << static_cast<int>(R_IO[3])  << endl;
        cout << "A4: " << static_cast<int>(R_IO[4])  << endl;
        cout << "A5: " << static_cast<int>(R_IO[5])  << endl;
        cout << "A6: " << static_cast<int>(R_IO[6])  << endl;
        cout << "--------" << endl;

        usleep(1000);
    }
#endif

    return 0;
}


void Get_Buttons_State(const shared_ptr<Device_class>& Device, u8 *Res)
{
    if (IO_Board_Control == nullptr || Device == nullptr || Res == nullptr)
    {
        cout << "Fun Get_Buttons_State() param invalid.";
        return;
    }

    IO_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_6,  1000);
    usleep(2000);
    Res[1] = IO_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_6);

    IO_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_5,  1000);
    usleep(2000);
    Res[2] = IO_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_5);

    IO_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_4,  1000);
    usleep(2000);
    Res[3] = IO_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_4);

    IO_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_3,  1000);
    usleep(2000);
    Res[4] = IO_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_3);

    IO_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_2,  1000);
    usleep(2000);
    Res[5] = IO_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_2);

    IO_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_1,  1000);
    usleep(2000);
    Res[6] = IO_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_1);
}

