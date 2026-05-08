#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <math.h>

#include "Auto_Set_Id.hpp"
#include "Battery_BMS_V2.hpp"
#include "Custom_TOP.hpp"
#include "Eyou_Motor_TOP.hpp"
#include "HARDWARE_TOP.hpp"
#include "IMU_YuanJi.hpp"
#include "Led_Device.hpp"
#include "Motor_BM_M1502D.hpp"
#include "Motor_TOP.hpp"
#include "Motor_TaiHu.hpp"
#include "Switch_Board.hpp"
#include "syst.hpp"
#include <filesystem>
#include "GPIO.hpp"

using namespace std;

// #define W_BOT_ACTION 1

#define W_Bot_CB_P_N0       GPIOD
// #define W_Bot_CB_P_N1    GPIOD
// #define W_Bot_CB_P_N2    GPIOD
// #define W_Bot_CB_P_N3    GPIOD

#define W_Bot_CB_Pin_N0     GPIO_PIN_11
// #define W_Bot_CB_Pin_N1     GPIO_PIN_11
// #define W_Bot_CB_Pin_N2     GPIO_PIN_11
// #define W_Bot_CB_Pin_N3     GPIO_PIN_11



float Eyou_Speed = 1;
float Eyou_Acc = 1;
float Eyou_Dec = 1;
float qqqq = 0.0;
float pos, vel, cur;

float ppppos = 0;

int opasopa = 1000;

typedef struct W_Bot_Out_Data {
    float Eyou_Yaw;
    float Eyou_Roll;
    float Eyou_Hip;
    float Eyou_Knee;
    float Eyou_Ankel;

    float left_shoulder_pitch;
    float left_shoulder_roll;
    float left_shoulder_yaw;
    float left_elbow;
    float left_wrist_yaw;
    float left_wrist_pitch;
    float left_wrist_roll;

    float right_shoulder_pitch;
    float right_shoulder_roll;
    float right_shoulder_yaw;
    float right_elbow;
    float right_wrist_yaw;
    float right_wrist_pitch;
    float right_wrist_roll;

    float head_pitch;
    float head_yaw;
} W_Bot_Out_Data;

/**
 * @brief W_Bot数据结构体
 */
W_Bot_Out_Data W_Bot_OD_Get;
W_Bot_Out_Data W_Bot_OD_Set;

Robot_Hardware* Test_Robot;

/**
 * @brief 灯带数据结构体
 */
RGB_Data RGB_Datas[2];

shared_ptr<Device_class> Main_Switch_Board;     
shared_ptr<Device_class> Waist_Main_Switch_Board;
shared_ptr<Device_class> Chassis_Main_Switch_Board;

/**
 * @brief 灯带设备
 */
shared_ptr<Device_class> Led_Shoulder_Devices;
shared_ptr<Device_class> Led_Chassis_Devices;
/**
 * @brief 下肢电机
 */
shared_ptr<Device_class> Lower_Limbs_Motor_Waist_Roll;
shared_ptr<Device_class> Lower_Limbs_Motor_Waist_Yaw;
shared_ptr<Device_class> Lower_Limbs_Motor_Knee;
shared_ptr<Device_class> Lower_Limbs_Motor_Hip;
shared_ptr<Device_class> Lower_Limbs_Motor_Ankel;
shared_ptr<Device_class> Lower_Limbs_Motor_Ankel_Right;
/**
 * @brief 左臂电机
 */
shared_ptr<Device_class> TaiHu_Device_1;
shared_ptr<Device_class> TaiHu_Device_2;
shared_ptr<Device_class> TaiHu_Device_3;
shared_ptr<Device_class> TaiHu_Device_4;
shared_ptr<Device_class> TaiHu_Device_5;
shared_ptr<Device_class> TaiHu_Device_6;
shared_ptr<Device_class> TaiHu_Device_7;
/**
 * @brief 右臂电机
 */
shared_ptr<Device_class> TaiHu_Device_8;
shared_ptr<Device_class> TaiHu_Device_9;
shared_ptr<Device_class> TaiHu_Device_10;
shared_ptr<Device_class> TaiHu_Device_11;
shared_ptr<Device_class> TaiHu_Device_12;
shared_ptr<Device_class> TaiHu_Device_13;
shared_ptr<Device_class> TaiHu_Device_14;
/**
 * @brief 头部电机
 */
shared_ptr<Device_class> TaiHu_Device_15;
shared_ptr<Device_class> TaiHu_Device_16;
/**
 * @brief 底盘轮子
 */
shared_ptr<Device_class> Classis_Motor_1;
shared_ptr<Device_class> Classis_Motor_2;

/**
 * @brief 电池
 */
shared_ptr<Device_class> Battery_BMS_V2_1;
shared_ptr<Device_class> Battery_BMS_V2_2;
shared_ptr<Device_class> Battery_BMS_V2_3;
shared_ptr<Device_class> Battery_BMS_V2_4;

shared_ptr<Device_class> IMU_Device_1;
shared_ptr<Device_class> IMU_Device_2;

Main_B* Main_Switch_Board_Control;
Main_B* Waist_Main_Switch_Board_Control;
Main_B* Chassis_Main_Switch_Board_Control;

Eyou_Motor* Lower_Limbs_Motor_Waist_Roll_Ctl;
Eyou_Motor* Lower_Limbs_Motor_Waist_Yaw_Ctl;
Eyou_Motor* Lower_Limbs_Motor_Knee_Ctl;
Eyou_Motor* Lower_Limbs_Motor_Hip_Ctl;
Eyou_Motor* Lower_Limbs_Motor_Ankel_Ctl;

Motor_TaiHu* TaiHu_Device_T1;
Motor_TaiHu* TaiHu_Device_T2;
Motor_TaiHu* TaiHu_Device_T3;
Motor_TaiHu* TaiHu_Device_T4;
Motor_TaiHu* TaiHu_Device_T5;
Motor_TaiHu* TaiHu_Device_T6;
Motor_TaiHu* TaiHu_Device_T7;

Motor_TaiHu* TaiHu_Device_T8;
Motor_TaiHu* TaiHu_Device_T9;
Motor_TaiHu* TaiHu_Device_T10;
Motor_TaiHu* TaiHu_Device_T11;
Motor_TaiHu* TaiHu_Device_T12;
Motor_TaiHu* TaiHu_Device_T13;
Motor_TaiHu* TaiHu_Device_T14;

Motor_TaiHu* TaiHu_Device_T15;
Motor_TaiHu* TaiHu_Device_T16;

Led_Device* Led_Device_Shoulder_Ptr;
Led_Device* Led_Device_Chassis_Ptr;

IMU_YuanJi* IMU_Device_T1;
IMU_YuanJi* IMU_Device_T2;

Motor_BM_M1502D* Classis_Motor_T1;
Motor_BM_M1502D* Classis_Motor_T2;

BMS_V2_Protocol* Battery_BMS_V2_T1;
BMS_V2_Protocol* Battery_BMS_V2_T2;
BMS_V2_Protocol* Battery_BMS_V2_T3;
BMS_V2_Protocol* Battery_BMS_V2_T4;

Main_B *IO_Board_Control;

float jia_pos[3];
void Eyou_Thread(void) {
    while (1) {
        Lower_Limbs_Motor_Ankel_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Ankel, &jia_pos[0], &jia_pos[1], &jia_pos[2]);
        usleep(500);
        Lower_Limbs_Motor_Knee_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Knee, &jia_pos[0], &jia_pos[1], &jia_pos[2]);
        usleep(500);
        Lower_Limbs_Motor_Hip_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Hip, &jia_pos[0], &jia_pos[1], &jia_pos[2]);
        usleep(500);
        Lower_Limbs_Motor_Waist_Roll_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Waist_Roll, &jia_pos[0], &jia_pos[1], &jia_pos[2]);
        usleep(500);
        Lower_Limbs_Motor_Waist_Yaw_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Waist_Yaw, &jia_pos[0], &jia_pos[1], &jia_pos[2]);
        usleep(400000);
    }
}

// void Get_Collision_Bar_Gpio(const shared_ptr<Device_class>& Device, u8 *Res){
//     if (Chassis_Main_Switch_Board_Control == nullptr || Device == nullptr || Res == nullptr)
//     {
//         cout << "Fun Get_Buttons_State() param invalid.";
//         return;
//     }
//     Chassis_Main_Switch_Board_Control->m_GPIO.GPIOx_Read(Device, GPIOD, GPIO_PIN_11,  1000);
//     usleep(2000);
//     Res[0] = Chassis_Main_Switch_Board_Control->m_GPIO.Get_GPIOx_Value(GPIOD, GPIO_PIN_11);
//     usleep(100000);
// }


// void Collision_Bar_IO_Thread(void){
//     while(1){
//         Get_Collision_Bar_Gpio(Chassis_Main_Switch_Board, W_Bot_OD_Get.Collision_Bar);
//         // printf("%x, %x, %x, %x\r\n", Collision_Bar[0], Collision_Bar[1], Collision_Bar[2], Collision_Bar[3]);
//     }
// }


void Battery_BMS_V2_Init(void) {
    // 启动信息
    cout << "===== BMS V2.2协议测试程序启动 =====" << endl;
    cout << "BMS设备CAN ID: 0x" << hex << Battery_BMS_V2_T1->Get_BMS_CAN_ID() << dec << endl;

    // 唤醒BMS设备（如果处于低功耗模式）
    if (Battery_BMS_V2_T1->Is_Low_Power_Mode()) {
        cout << "检测到BMS处于低功耗模式,正在唤醒..." << endl;
        if (Battery_BMS_V2_T1->Wake_Up_BMS(Battery_BMS_V2_1) != 0) {
            cout << "BMS1 唤醒失败,程序退出" << endl;
        }
    }

    // 唤醒BMS设备（如果处于低功耗模式）
    if (Battery_BMS_V2_T2->Is_Low_Power_Mode()) {
        cout << "检测到BMS处于低功耗模式,正在唤醒..." << endl;
        if (Battery_BMS_V2_T2->Wake_Up_BMS(Battery_BMS_V2_2) != 0) {
            cout << "BMS2 唤醒失败,程序退出" << endl;
        }
    }

    // 唤醒BMS设备（如果处于低功耗模式）
    if (Battery_BMS_V2_T3->Is_Low_Power_Mode()) {
        cout << "检测到BMS处于低功耗模式,正在唤醒..." << endl;
        if (Battery_BMS_V2_T3->Wake_Up_BMS(Battery_BMS_V2_3) != 0) {
            cout << "BMS3 唤醒失败,程序退出" << endl;
        }
    }

    // 唤醒BMS设备（如果处于低功耗模式）
    if (Battery_BMS_V2_T4->Is_Low_Power_Mode()) {
        cout << "检测到BMS处于低功耗模式,正在唤醒..." << endl;
        if (Battery_BMS_V2_T4->Wake_Up_BMS(Battery_BMS_V2_4) != 0) {
            cout << "BMS4 唤醒失败,程序退出" << endl;
        }
    }

    if (Battery_BMS_V2_T1->Start_Data_Collection(Battery_BMS_V2_1, 1000) != 0) {
        cout << "❌ 数据采集线程启动失败" << endl;
        return;
    }
    if (Battery_BMS_V2_T2->Start_Data_Collection(Battery_BMS_V2_2, 1000) != 0) {
        cout << "❌ 数据采集线程启动失败" << endl;
        return;
    }
    if (Battery_BMS_V2_T3->Start_Data_Collection(Battery_BMS_V2_3, 1000) != 0) {
        cout << "❌ 数据采集线程启动失败" << endl;
        return;
    }
    if (Battery_BMS_V2_T4->Start_Data_Collection(Battery_BMS_V2_4, 1000) != 0) {
        cout << "❌ 数据采集线程启动失败" << endl;
        return;
    }

    sleep(2);  // 等待数据采集线程启动

    // this_thread::sleep_for(std::chrono::milliseconds(350));
}

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(const string& ADDR, const string& Config)
#endif
{
    Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    Test_Robot->Add_Device_Type("Eyou_Custom_Motor_TOP", Eyou_Custom_Motor_Device_Init, Eyou_Custom_Motor_Device_CallBack_F, Eyou_Custom_Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type("TaiHu_Custom_Motor", Motor_Device_Init_TaiHu, Motor_Device_CallBack_TaiHu, Motor_Device_Delete_TaiHu);
    Test_Robot->Add_Device_Type("YuanJi_Custom_IMU", IMU_YuanJi_Init, IMU_YuanJi_CallBack_F, IMU_YuanJi_Delete_F);
    Test_Robot->Add_Device_Type(Led_Device_TYPE, Led_Device_Init, Led_Device_CallBack_F, Led_Device_Delete_F);
    Test_Robot->Add_Device_Type(BenMo_Custom_Motor_Type, Motor_Device_Init_BM_M1502D, Motor_Device_CallBack_BM_M1502D, Motor_Device_Delete_BM_M1502D);
    Test_Robot->Add_Device_Type(Battery_BMS_V2_Custom_Type, Battery_BMS_V2_Device_Init, Battery_BMS_V2_Device_CallBack, Battery_BMS_V2_Device_Delete);
    // Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);

#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/W_Bot/out/TOP.yaml";
    string Config = "None";
    //     string Config = R"(
    // PC_IP: 192.168.3.245
    // SN: asdf1234567
    // Boards:
    //   - Id: 101

    //     IP: 192.168.3.243
    //     # Port: 19001
    // )";
#endif
    if (Test_Robot->Init_TOP(ADDR, Config) != 0) {
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

    Main_Switch_Board         = Test_Robot->Get_Device_For_Name("Main_Switch_Board");
    Waist_Main_Switch_Board   = Test_Robot->Get_Device_For_Name("Waist_Main_Switch_Board");
    Chassis_Main_Switch_Board = Test_Robot->Get_Device_For_Name("Chassis_Main_Switch_Board");

    Lower_Limbs_Motor_Waist_Roll = Test_Robot->Get_Device_For_Name("Eyou_Waist_Roll");
    Lower_Limbs_Motor_Waist_Yaw = Test_Robot->Get_Device_For_Name("Eyou_Waist_Yaw");
    Lower_Limbs_Motor_Knee = Test_Robot->Get_Device_For_Name("Eyou_Knee");
    Lower_Limbs_Motor_Hip = Test_Robot->Get_Device_For_Name("Eyou_Hip");
    Lower_Limbs_Motor_Ankel = Test_Robot->Get_Device_For_Name("Eyou_Ankel");

    TaiHu_Device_1 = Test_Robot->Get_Device_For_Name("left_shoulder_pitch");
    TaiHu_Device_2 = Test_Robot->Get_Device_For_Name("left_shoulder_roll");
    TaiHu_Device_3 = Test_Robot->Get_Device_For_Name("left_shoulder_yaw");
    TaiHu_Device_4 = Test_Robot->Get_Device_For_Name("left_elbow");
    TaiHu_Device_5 = Test_Robot->Get_Device_For_Name("left_wrist_yaw");
    TaiHu_Device_6 = Test_Robot->Get_Device_For_Name("left_wrist_pitch");
    TaiHu_Device_7 = Test_Robot->Get_Device_For_Name("left_wrist_roll");

    TaiHu_Device_8 = Test_Robot->Get_Device_For_Name("right_shoulder_pitch");
    TaiHu_Device_9 = Test_Robot->Get_Device_For_Name("right_shoulder_roll");
    TaiHu_Device_10 = Test_Robot->Get_Device_For_Name("right_shoulder_yaw");
    TaiHu_Device_11 = Test_Robot->Get_Device_For_Name("right_elbow");
    TaiHu_Device_12 = Test_Robot->Get_Device_For_Name("right_wrist_yaw");
    TaiHu_Device_13 = Test_Robot->Get_Device_For_Name("right_wrist_pitch");
    TaiHu_Device_14 = Test_Robot->Get_Device_For_Name("right_wrist_roll");

    TaiHu_Device_15 = Test_Robot->Get_Device_For_Name("head_pitch");
    TaiHu_Device_16 = Test_Robot->Get_Device_For_Name("head_yaw");

    Led_Shoulder_Devices = Test_Robot->Get_Device_For_Name("Led_GoGo");
    Led_Chassis_Devices = Test_Robot->Get_Device_For_Name("Led_Chassis_GoGo");

    IMU_Device_1 = Test_Robot->Get_Device_For_Name("YuanJi_IMU1");
    IMU_Device_2 = Test_Robot->Get_Device_For_Name("YuanJi_IMU2");

    Classis_Motor_1 = Test_Robot->Get_Device_For_Name("BenMo_Motor");
    Classis_Motor_2 = Test_Robot->Get_Device_For_Name("BenMo_Motor1");

    Battery_BMS_V2_1 = Test_Robot->Get_Device_For_Name("Battery_BMS_V2_1");
    Battery_BMS_V2_2 = Test_Robot->Get_Device_For_Name("Battery_BMS_V2_2");
    Battery_BMS_V2_3 = Test_Robot->Get_Device_For_Name("Battery_BMS_V2_3");
    Battery_BMS_V2_4 = Test_Robot->Get_Device_For_Name("Battery_BMS_V2_4");

    // IO_Board = Test_Robot->Get_Device_For_Name("IO_Board");

    Main_Switch_Board_Control         = static_cast<Main_B*>(Test_Robot->Get_Control_Class(Main_Switch_Board));
    Waist_Main_Switch_Board_Control   = static_cast<Main_B*>(Test_Robot->Get_Control_Class(Waist_Main_Switch_Board));
    Chassis_Main_Switch_Board_Control = static_cast<Main_B*>(Test_Robot->Get_Control_Class(Chassis_Main_Switch_Board));

    Lower_Limbs_Motor_Waist_Roll_Ctl = static_cast<Eyou_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Waist_Roll));
    Lower_Limbs_Motor_Waist_Yaw_Ctl = static_cast<Eyou_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Waist_Yaw));
    Lower_Limbs_Motor_Knee_Ctl = static_cast<Eyou_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Knee));
    Lower_Limbs_Motor_Hip_Ctl = static_cast<Eyou_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Hip));
    Lower_Limbs_Motor_Ankel_Ctl = static_cast<Eyou_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Ankel));

    TaiHu_Device_T1  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_1));
    TaiHu_Device_T2  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_2));
    TaiHu_Device_T3  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_3));
    TaiHu_Device_T4  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_4));
    TaiHu_Device_T5  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_5));
    TaiHu_Device_T6  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_6));
    TaiHu_Device_T7  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_7));
    TaiHu_Device_T8  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_8));
    TaiHu_Device_T9  = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_9));
    TaiHu_Device_T10 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_10));
    TaiHu_Device_T11 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_11));
    TaiHu_Device_T12 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_12));
    TaiHu_Device_T13 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_13));
    TaiHu_Device_T14 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_14));
    TaiHu_Device_T15 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_15));
    TaiHu_Device_T16 = static_cast<Motor_TaiHu*>(Test_Robot->Get_Control_Class(TaiHu_Device_16));

    Led_Device_Shoulder_Ptr = (Led_Device*)Test_Robot->Get_Control_Class(Led_Shoulder_Devices);
    Led_Device_Chassis_Ptr = (Led_Device*)Test_Robot->Get_Control_Class(Led_Chassis_Devices);

    IMU_Device_T1 = static_cast<IMU_YuanJi*>(Test_Robot->Get_Control_Class(IMU_Device_1));
    IMU_Device_T2 = static_cast<IMU_YuanJi*>(Test_Robot->Get_Control_Class(IMU_Device_2));

    Classis_Motor_T1 = static_cast<Motor_BM_M1502D*>(Test_Robot->Get_Control_Class(Classis_Motor_1));
    Classis_Motor_T2 = static_cast<Motor_BM_M1502D*>(Test_Robot->Get_Control_Class(Classis_Motor_2));

    Battery_BMS_V2_T1 = static_cast<BMS_V2_Protocol*>(Test_Robot->Get_Control_Class(Battery_BMS_V2_1));
    Battery_BMS_V2_T2 = static_cast<BMS_V2_Protocol*>(Test_Robot->Get_Control_Class(Battery_BMS_V2_2));
    Battery_BMS_V2_T3 = static_cast<BMS_V2_Protocol*>(Test_Robot->Get_Control_Class(Battery_BMS_V2_3));
    Battery_BMS_V2_T4 = static_cast<BMS_V2_Protocol*>(Test_Robot->Get_Control_Class(Battery_BMS_V2_4));

    Chassis_Main_Switch_Board_Control = static_cast<Main_B*>(Test_Robot->Get_Control_Class(Chassis_Main_Switch_Board));

    IMU_Device_T1->Start_AHRS_Mod_And_Init(IMU_Device_1);
    usleep(100);
    IMU_Device_T2->Start_AHRS_Mod_And_Init(IMU_Device_2);

    Battery_BMS_V2_Init();

    Chassis_Main_Switch_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(Chassis_Main_Switch_Board, W_Bot_CB_P_N0, W_Bot_CB_Pin_N0);
    // Chassis_Main_Switch_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(Chassis_Main_Switch_Board, W_Bot_CB_P_N1, W_Bot_CB_Pin_N1);
    // Chassis_Main_Switch_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(Chassis_Main_Switch_Board, W_Bot_CB_P_N2, W_Bot_CB_Pin_N2);
    // Chassis_Main_Switch_Board_Control->m_GPIO.Set_GPIOx_To_Input_Mode(Chassis_Main_Switch_Board, W_Bot_CB_P_N3, W_Bot_CB_Pin_N3);


#ifdef W_BOT_ACTION
    float SetPos = 0.0f;
    float ARmSetPos = 0.0f;
    vector<Motor_TaiHu*> Motor_vector;
    Motor_vector.push_back(TaiHu_Device_T2);
    Motor_vector.push_back(TaiHu_Device_T3);
    Motor_vector.push_back(TaiHu_Device_T4);
    Motor_vector.push_back(TaiHu_Device_T5);
    Motor_vector.push_back(TaiHu_Device_T6);
    Motor_vector.push_back(TaiHu_Device_T7);

    Motor_vector.push_back(TaiHu_Device_T9);
    Motor_vector.push_back(TaiHu_Device_T10);
    Motor_vector.push_back(TaiHu_Device_T11);
    Motor_vector.push_back(TaiHu_Device_T12);
    Motor_vector.push_back(TaiHu_Device_T13);
    Motor_vector.push_back(TaiHu_Device_T14);
    Motor_vector.push_back(TaiHu_Device_T15);
    Motor_vector.push_back(TaiHu_Device_T16);

    vector<shared_ptr<Device_class>> Device_vector;
    Device_vector.push_back(TaiHu_Device_2);
    Device_vector.push_back(TaiHu_Device_3);
    Device_vector.push_back(TaiHu_Device_4);
    Device_vector.push_back(TaiHu_Device_5);
    Device_vector.push_back(TaiHu_Device_6);
    Device_vector.push_back(TaiHu_Device_7);

    Device_vector.push_back(TaiHu_Device_9);
    Device_vector.push_back(TaiHu_Device_10);
    Device_vector.push_back(TaiHu_Device_11);
    Device_vector.push_back(TaiHu_Device_12);
    Device_vector.push_back(TaiHu_Device_13);
    Device_vector.push_back(TaiHu_Device_14);

    Device_vector.push_back(TaiHu_Device_15);
    Device_vector.push_back(TaiHu_Device_16);

    TaiHu_Device_T1->Send_MIT_PD_Control_Data(TaiHu_Device_1, -ARmSetPos, 0.0f, 0.0f, 0.0f, 0.0f);
    usleep(100);
    TaiHu_Device_T8->Send_MIT_PD_Control_Data(TaiHu_Device_8, ARmSetPos, 0.0f, 0.0f, 0.0f, 0.0f);
    usleep(100);
    for (int i = 0; i < Motor_vector.size(); i++) {
        if (i > 6 && i < 12)
            Motor_vector.at(i)->Send_MIT_PD_Control_Data(Device_vector.at(i), -SetPos, 0.0f, 0.0f, 0.0f, 0.0f);
        else
            Motor_vector.at(i)->Send_MIT_PD_Control_Data(Device_vector.at(i), SetPos, 0.0f, 0.0f, 0.0f, 0.0f);
        usleep(100);
    }

    // while(1){
    usleep(350);
    Lower_Limbs_Motor_Ankel_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Ankel, ppppos, 1, 1, 1, 1);
    usleep(350);
    Lower_Limbs_Motor_Knee_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Knee, ppppos, 1, 1, 1, 1);
    usleep(350);
    Lower_Limbs_Motor_Hip_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Hip, ppppos, 1, 1, 1, 1);
    usleep(350);
    Lower_Limbs_Motor_Waist_Roll_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Roll, ppppos, 1, 1, 1, 1);
    usleep(350);
    Lower_Limbs_Motor_Waist_Yaw_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Yaw, ppppos, 1, 1, 1, 1);

    // ppppos = -ppppos;
    // }
#endif

#ifndef HAVE_ROS

    for (int i = 0; i < 10; i++) {
        RGB_Datas[0].R_data[i] = 0;
        RGB_Datas[0].G_data[i] = 100;
        RGB_Datas[0].B_data[i] = 0;
    }
    for (int i = 0; i < 10; i++) {
        RGB_Datas[1].R_data[i] = 255;
        RGB_Datas[1].G_data[i] = 0;
        RGB_Datas[1].B_data[i] = 0;
    }
    // Led_Device_Chassis_Ptr ->Led_Set_Every_Color(Led_Chassis_Devices,  1, 1, 11, &RGB_Datas[0]);
    // Led_Device_Shoulder_Ptr->Led_Set_Every_Color(Led_Shoulder_Devices, 1, 1, 11, &RGB_Datas[1]);

    // Classis_Motor_T1->Send_MIT_PD_Control_Data(Classis_Motor_1, 0.0f, 60.0f, 0.0f, 0.0f, 1.0f);
    // Classis_Motor_T2->Send_MIT_PD_Control_Data(Classis_Motor_2, 0.0f, -60.0f, 0.0f, 0.0f, 1.0f);

    // Lower_Limbs_Motor_Ankel_Ctl->Read_KP_Data(Lower_Limbs_Motor_Ankel);
    thread eyou_th(Eyou_Thread);
    // thread io_th(Collision_Bar_IO_Thread);


    while (1) {
        // Lower_Limbs_Motor_Ankel_Ctl->Read_KP_Data(Lower_Limbs_Motor_Ankel);
        // usleep(1000);

        // Lower_Limbs_Motor_Ankel_Ctl ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Ankel,     0  , 1, 1, 1, 1);
        // usleep(10000);
        Lower_Limbs_Motor_Ankel_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Ankel, 2.1, 1, 1, 1, 1);
        // usleep(1000);
        Lower_Limbs_Motor_Waist_Roll_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Roll, 2.1, 1, 1, 1, 1);
        Lower_Limbs_Motor_Knee_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Knee, 2.1, 1, 1, 1, 1);
        Lower_Limbs_Motor_Hip_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Hip, 2.1, 1, 1, 1, 1);
        // Lower_Limbs_Motor_Ankel_Ctl     ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Ankel,         10 , 1, 1, 1, 1);

        // Lower_Limbs_Motor_Waist_Yaw_Ctl ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Yaw,     0, 1, 1, 1, 1);
        // Lower_Limbs_Motor_Waist_Roll_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Roll,    0, 1, 1, 1, 1);
        // Lower_Limbs_Motor_Knee_Ctl      ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Knee,          0, 1, 1, 1, 1);
        // Lower_Limbs_Motor_Hip_Ctl       ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Hip,           0, 1, 1, 1, 1);
        // Lower_Limbs_Motor_Ankel_Ctl     ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Ankel,         0, 1, 1, 1, 1);

        // Lower_Limbs_Motor_Waist_Yaw_Ctl ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Yaw,     W_Bot_OD_Set.Eyou_Yaw   , Eyou_Speed, 1, Eyou_Acc, Eyou_Dec);
        // Lower_Limbs_Motor_Waist_Roll_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Waist_Roll,    W_Bot_OD_Set.Eyou_Roll  , Eyou_Speed, 1, Eyou_Acc, Eyou_Dec);
        // Lower_Limbs_Motor_Knee_Ctl      ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Knee,          W_Bot_OD_Set.Eyou_Knee  , Eyou_Speed, 1, Eyou_Acc, Eyou_Dec);
        // Lower_Limbs_Motor_Hip_Ctl       ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Hip,           W_Bot_OD_Set.Eyou_Hip   , Eyou_Speed, 1, Eyou_Acc, Eyou_Dec);
        // Lower_Limbs_Motor_Ankel_Ctl     ->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Ankel,         W_Bot_OD_Set.Eyou_Ankel , Eyou_Speed, 1, Eyou_Acc, Eyou_Dec);

        // Lower_Limbs_Motor_Waist_Yaw_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Waist_Yaw, &W_Bot_OD_Get.Eyou_Yaw, &Eyou_Speed, &qqqq);
        Lower_Limbs_Motor_Knee_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Knee, &W_Bot_OD_Get.Eyou_Knee, &Eyou_Speed, &qqqq);
        Lower_Limbs_Motor_Waist_Roll_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Waist_Roll, &W_Bot_OD_Get.Eyou_Roll, &Eyou_Speed, &qqqq);
        Lower_Limbs_Motor_Hip_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Hip, &W_Bot_OD_Get.Eyou_Hip, &Eyou_Speed, &qqqq);
        Lower_Limbs_Motor_Ankel_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Ankel, &W_Bot_OD_Get.Eyou_Ankel, &Eyou_Speed, &qqqq);

        usleep(1000);

        // Lower_Limbs_Motor_Ankel_Ctl->Get_Error(Lower_Limbs_Motor_Ankel);
        // usleep(1000);

        // TaiHu_Device_T1->Send_MIT_PD_Control_Data(TaiHu_Device_1,W_Bot_OD_Set.left_shoulder_pitch,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T2->Send_MIT_PD_Control_Data(TaiHu_Device_2,W_Bot_OD_Set.left_shoulder_roll,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T3->Send_MIT_PD_Control_Data(TaiHu_Device_3,W_Bot_OD_Set.left_shoulder_yaw,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T4->Send_MIT_PD_Control_Data(TaiHu_Device_4,W_Bot_OD_Set.left_elbow,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T5->Send_MIT_PD_Control_Data(TaiHu_Device_5,W_Bot_OD_Set.left_wrist_yaw,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T6->Send_MIT_PD_Control_Data(TaiHu_Device_6,W_Bot_OD_Set.left_wrist_roll,0.0f, 0.0f, 0.0f, 0.0f);

        // TaiHu_Device_T7->Send_MIT_PD_Control_Data(TaiHu_Device_7,W_Bot_OD_Set.right_shoulder_pitch,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T8->Send_MIT_PD_Control_Data(TaiHu_Device_8,W_Bot_OD_Set.right_shoulder_roll,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T9->Send_MIT_PD_Control_Data(TaiHu_Device_9,W_Bot_OD_Set.right_shoulder_yaw,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T10->Send_MIT_PD_Control_Data(TaiHu_Device_10,W_Bot_OD_Set.right_elbow,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T11->Send_MIT_PD_Control_Data(TaiHu_Device_11,W_Bot_OD_Set.right_wrist_yaw,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T12->Send_MIT_PD_Control_Data(TaiHu_Device_12,W_Bot_OD_Set.right_wrist_pitch,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T13->Send_MIT_PD_Control_Data(TaiHu_Device_13,W_Bot_OD_Set.right_wrist_roll,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T14->Send_MIT_PD_Control_Data(TaiHu_Device_14,W_Bot_OD_Set.head_pitch,0.0f, 0.0f, 0.0f, 0.0f);
        // TaiHu_Device_T15->Send_MIT_PD_Control_Data(TaiHu_Device_15,W_Bot_OD_Set.head_yaw,0.0f, 0.0f, 0.0f, 0.0f);

        // TaiHu_Device_T1->Get_Motor_FB_Data(TaiHu_Device_1,&W_Bot_OD_Get.left_shoulder_pitch, &vel, &cur);
        // TaiHu_Device_T2->Get_Motor_FB_Data(TaiHu_Device_2,&W_Bot_OD_Get.left_shoulder_roll, &vel, &cur);
        // TaiHu_Device_T3->Get_Motor_FB_Data(TaiHu_Device_3,&W_Bot_OD_Get.left_shoulder_yaw, &vel, &cur);
        // TaiHu_Device_T4->Get_Motor_FB_Data(TaiHu_Device_4,&W_Bot_OD_Get.left_elbow, &vel, &cur);
        // TaiHu_Device_T5->Get_Motor_FB_Data(TaiHu_Device_5,&W_Bot_OD_Get.left_wrist_yaw, &vel, &cur);
        // TaiHu_Device_T6->Get_Motor_FB_Data(TaiHu_Device_6,&W_Bot_OD_Get.left_wrist_roll, &vel, &cur);

        // TaiHu_Device_T7->Get_Motor_FB_Data(TaiHu_Device_7,&W_Bot_OD_Get.right_shoulder_pitch, &vel, &cur);
        // TaiHu_Device_T8->Get_Motor_FB_Data(TaiHu_Device_8,&W_Bot_OD_Get.right_shoulder_roll, &vel, &cur);
        // TaiHu_Device_T9->Get_Motor_FB_Data(TaiHu_Device_9,&W_Bot_OD_Get.right_shoulder_yaw, &vel, &cur);
        // TaiHu_Device_T10->Get_Motor_FB_Data(TaiHu_Device_10,&W_Bot_OD_Get.right_elbow, &vel, &cur);
        // TaiHu_Device_T11->Get_Motor_FB_Data(TaiHu_Device_11,&W_Bot_OD_Get.right_wrist_yaw, &vel, &cur);
        // TaiHu_Device_T12->Get_Motor_FB_Data(TaiHu_Device_12,&W_Bot_OD_Get.right_wrist_pitch, &vel, &cur);
        // TaiHu_Device_T13->Get_Motor_FB_Data(TaiHu_Device_13,&W_Bot_OD_Get.right_wrist_roll, &vel, &cur);
        // TaiHu_Device_T14->Get_Motor_FB_Data(TaiHu_Device_14,&W_Bot_OD_Get.head_pitch, &vel, &cur);
        // TaiHu_Device_T15->Get_Motor_FB_Data(TaiHu_Device_15,&W_Bot_OD_Get.head_yaw, &vel, &cur);
        //     usleep(100);

        // IMU_Datas[0] = IMU_Device_T1->Get_IMU_Frame_Data();
        // IMU_Datas[1] = IMU_Device_T2->Get_IMU_Frame_Data();
    }
#endif

    return 0;
}
