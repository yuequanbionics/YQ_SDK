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
#include "Phu_Motor_TOP.hpp"
#include "HARDWARE_TOP.hpp"
#include "IMU_YuanJi.hpp"
#include "Led_Device.hpp"
#include "Motor_BM_M1502D.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "syst.hpp"
#include <filesystem>
#include "GPIO.hpp"

using namespace std;

float Send_Data[4] =  {0};
float Send_Data1[4] =  {0};
float right_shoulder_pitch_N21_Send_Data[4] = {0};
float Lower_Limbs_Motor_Knee_N32_Send_Data[4] = {0};

#define TEST_Ctl_Pos_left_wrist_yaw_N15
#define TEST_Get_left_wrist_yaw_N15_Data
float   Get_right_shoulder_pitch_N21[4] = {0};
float   Get_left_shoulder_pitch_N11[4] = {0};
float   Get_left_wrist_yaw_N15[4] = {0};
float   Get_Lower_Limbs_Motor_Knee_N32[4] = {0};

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
shared_ptr<Device_class> Lower_Limbs_Motor_Ankel_N31;
shared_ptr<Device_class> Lower_Limbs_Motor_Knee_N32;
shared_ptr<Device_class> Lower_Limbs_Motor_Hip_N33;
shared_ptr<Device_class> Lower_Limbs_Motor_Waist_Roll_N34;
shared_ptr<Device_class> Lower_Limbs_Motor_Waist_Yaw_N35;
/**
 * @brief 左臂电机
 */
shared_ptr<Device_class> left_shoulder_pitch_N11;
shared_ptr<Device_class> left_shoulder_roll_N12;
shared_ptr<Device_class> left_shoulder_yaw_N13;
shared_ptr<Device_class> left_elbow_N14;
shared_ptr<Device_class> left_wrist_yaw_N15;
shared_ptr<Device_class> left_wrist_pitch_N16;
shared_ptr<Device_class> left_wrist_roll_N17;
/**
 * @brief 右臂电机
 */
shared_ptr<Device_class> right_shoulder_pitch_N21;
shared_ptr<Device_class> right_shoulder_roll_N22;
shared_ptr<Device_class> right_shoulder_yaw_N23;
shared_ptr<Device_class> right_elbow_N24;
shared_ptr<Device_class> right_wrist_yaw_N25;
shared_ptr<Device_class> right_wrist_pitch_N26;
shared_ptr<Device_class> right_wrist_roll_N27;
/**
 * @brief 头部电机
 */
// shared_ptr<Device_class> TaiHu_Device_15;
// shared_ptr<Device_class> TaiHu_Device_16;
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

Phu_Motor* Lower_Limbs_Motor_Ankel_N31_Ctl;
Phu_Motor* Lower_Limbs_Motor_Knee_N32_Ctl;
Phu_Motor* Lower_Limbs_Motor_Hip_N33_Ctl;
Phu_Motor* Lower_Limbs_Motor_Waist_Roll_N34_Ctl;
Phu_Motor* Lower_Limbs_Motor_Waist_Yaw_N35_Ctl;

Phu_Motor* left_shoulder_pitch_N11_Ctl;
Phu_Motor* left_shoulder_roll_N12_Ctl;
Phu_Motor* left_shoulder_yaw_N13_Ctl;
Phu_Motor* left_elbow_N14_Ctl;
Phu_Motor* left_wrist_yaw_N15_Ctl;
Phu_Motor* left_wrist_pitch_N16_Ctl;
Phu_Motor* left_wrist_roll_N17_Ctl;

Phu_Motor* right_shoulder_pitch_N21_Ctl;
Phu_Motor* right_shoulder_roll_N22_Ctl;
Phu_Motor* right_shoulder_yaw_N23_Ctl;
Phu_Motor* right_elbow_N24_Ctl;
Phu_Motor* right_wrist_yaw_N25_Ctl;
Phu_Motor* right_wrist_pitch_N26_Ctl;
Phu_Motor* right_wrist_roll_N27_Ctl;

Phu_Motor* TaiHu_Device_T15;
Phu_Motor* TaiHu_Device_T16;

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
    Test_Robot->Add_Device_Type(Phu_Custom_Type, Phu_Custom_Motor_Device_Init, Phu_Custom_Motor_Device_CallBack_F, Phu_Custom_Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type("YuanJi_Custom_IMU", IMU_YuanJi_Init, IMU_YuanJi_CallBack_F, IMU_YuanJi_Delete_F);
    Test_Robot->Add_Device_Type(Led_Device_TYPE, Led_Device_Init, Led_Device_CallBack_F, Led_Device_Delete_F);
    Test_Robot->Add_Device_Type(BenMo_Custom_Motor_Type, Motor_Device_Init_BM_M1502D, Motor_Device_CallBack_BM_M1502D, Motor_Device_Delete_BM_M1502D);
    Test_Robot->Add_Device_Type(Battery_BMS_V2_Custom_Type, Battery_BMS_V2_Device_Init, Battery_BMS_V2_Device_CallBack, Battery_BMS_V2_Device_Delete);
    // Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);

#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/W_Bot_EUF/out/TOP.yaml";
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

    for (auto Device = Test_Robot->Get_Device_Map()->begin(); Device != Test_Robot->Get_Device_Map()->end(); ++Device)
    {
        cout << "Device_Name: " << Device->first << " ID: " << Device->second->Device_Main_Data.Main_Can_ID << endl;
        shared_ptr<Device_class> Device_This = Device->second;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

    Main_Switch_Board         = Test_Robot->Get_Device_For_Name("Main_Switch_Board");
    Waist_Main_Switch_Board   = Test_Robot->Get_Device_For_Name("Waist_Main_Switch_Board");
    Chassis_Main_Switch_Board = Test_Robot->Get_Device_For_Name("Chassis_Main_Switch_Board");

    Lower_Limbs_Motor_Ankel_N31         = Test_Robot->Get_Device_For_Name("Eyou_Ankel_N31");
    Lower_Limbs_Motor_Knee_N32          = Test_Robot->Get_Device_For_Name("Eyou_Knee_N32");
    Lower_Limbs_Motor_Hip_N33           = Test_Robot->Get_Device_For_Name("Phu_Hip_N33");
    Lower_Limbs_Motor_Waist_Roll_N34    = Test_Robot->Get_Device_For_Name("Phu_Waist_Roll_N34");
    Lower_Limbs_Motor_Waist_Yaw_N35     = Test_Robot->Get_Device_For_Name("Phu_Waist_Yaw_N35");


    left_shoulder_pitch_N11 = Test_Robot->Get_Device_For_Name("left_shoulder_pitch_N11");
    left_shoulder_roll_N12  = Test_Robot->Get_Device_For_Name("left_shoulder_roll_N12");
    left_shoulder_yaw_N13   = Test_Robot->Get_Device_For_Name("left_shoulder_yaw_N13");
    left_elbow_N14          = Test_Robot->Get_Device_For_Name("left_elbow_N14");
    left_wrist_yaw_N15      = Test_Robot->Get_Device_For_Name("left_wrist_yaw_N15");
    left_wrist_pitch_N16    = Test_Robot->Get_Device_For_Name("left_wrist_pitch_N16");
    left_wrist_roll_N17     = Test_Robot->Get_Device_For_Name("left_wrist_roll_N17");

    right_shoulder_pitch_N21    = Test_Robot->Get_Device_For_Name("right_shoulder_pitch_N21");
    right_shoulder_roll_N22     = Test_Robot->Get_Device_For_Name("right_shoulder_roll_N22");
    right_shoulder_yaw_N23      = Test_Robot->Get_Device_For_Name("right_shoulder_yaw_N23");
    right_elbow_N24             = Test_Robot->Get_Device_For_Name("right_elbow_N24");
    right_wrist_yaw_N25         = Test_Robot->Get_Device_For_Name("right_wrist_yaw_N25");
    right_wrist_pitch_N26       = Test_Robot->Get_Device_For_Name("right_wrist_pitch_N26");
    right_wrist_roll_N27        = Test_Robot->Get_Device_For_Name("right_wrist_roll_N27");

    // TaiHu_Device_15 = Test_Robot->Get_Device_For_Name("head_pitch_N2");
    // TaiHu_Device_16 = Test_Robot->Get_Device_For_Name("head_yaw_N1");

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

    Lower_Limbs_Motor_Ankel_N31_Ctl         = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Ankel_N31));
    Lower_Limbs_Motor_Knee_N32_Ctl          = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Knee_N32));
    Lower_Limbs_Motor_Hip_N33_Ctl           = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Hip_N33));
    Lower_Limbs_Motor_Waist_Roll_N34_Ctl    = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Waist_Roll_N34));
    Lower_Limbs_Motor_Waist_Yaw_N35_Ctl     = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(Lower_Limbs_Motor_Waist_Yaw_N35));

    left_shoulder_pitch_N11_Ctl     = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_shoulder_pitch_N11));
    left_shoulder_roll_N12_Ctl      = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_shoulder_roll_N12));
    left_shoulder_yaw_N13_Ctl       = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_shoulder_yaw_N13));
    left_elbow_N14_Ctl              = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_elbow_N14));
    left_wrist_yaw_N15_Ctl          = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_wrist_yaw_N15));
    left_wrist_pitch_N16_Ctl        = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_wrist_pitch_N16));
    left_wrist_roll_N17_Ctl         = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(left_wrist_roll_N17));
    right_shoulder_pitch_N21_Ctl    = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_shoulder_pitch_N21));
    right_shoulder_roll_N22_Ctl     = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_shoulder_roll_N22));
    right_shoulder_yaw_N23_Ctl      = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_shoulder_yaw_N23));
    right_elbow_N24_Ctl             = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_elbow_N24));
    right_wrist_yaw_N25_Ctl         = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_wrist_yaw_N25));
    right_wrist_pitch_N26_Ctl       = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_wrist_pitch_N26));
    right_wrist_roll_N27_Ctl        = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(right_wrist_roll_N27));

    // TaiHu_Device_T15                = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(TaiHu_Device_15));
    // TaiHu_Device_T16                = static_cast<Phu_Motor*>(Test_Robot->Get_Control_Class(TaiHu_Device_16));

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

    double target  = 15;
    double step    = 0.0001;

    while (1) {

        // for(double i = 0; i <= 1.0; i += step){
        //     double k = 0.5 - 0.5 * cos(3.1415926 * i);
        //     #ifdef TEST_Ctl_Pos_left_wrist_yaw_N15
        //         right_shoulder_pitch_N21_Ctl->Get_Motor_FB_Data(right_shoulder_pitch_N21, &Get_right_shoulder_pitch_N21[0], &Get_right_shoulder_pitch_N21[1], &Get_right_shoulder_pitch_N21[2], &Get_right_shoulder_pitch_N21[3], NULL);
        //         left_shoulder_pitch_N11_Ctl->Get_Motor_FB_Data(left_shoulder_pitch_N11, &Get_left_shoulder_pitch_N11[0], &Get_left_shoulder_pitch_N11[1], &Get_left_shoulder_pitch_N11[2], &Get_left_shoulder_pitch_N11[3], NULL);
        //         left_wrist_yaw_N15_Ctl->Get_Motor_FB_Data(left_wrist_yaw_N15, &Get_left_wrist_yaw_N15[0], &Get_left_wrist_yaw_N15[1], &Get_left_wrist_yaw_N15[2], &Get_left_wrist_yaw_N15[3], NULL);
        //         Lower_Limbs_Motor_Knee_N32_Ctl->Get_Motor_FB_Data(Lower_Limbs_Motor_Knee_N32, &Get_Lower_Limbs_Motor_Knee_N32[0], &Get_Lower_Limbs_Motor_Knee_N32[1], &Get_Lower_Limbs_Motor_Knee_N32[2], &Get_Lower_Limbs_Motor_Knee_N32[3], NULL);

        //         Send_Data[0] = Get_left_wrist_yaw_N15[0] + (target - Get_left_wrist_yaw_N15[0]) * k;
        //         Send_Data1[0] = Get_left_shoulder_pitch_N11[0] + (target - Get_left_shoulder_pitch_N11[0]) * k;
        //         right_shoulder_pitch_N21_Send_Data[0] = Get_right_shoulder_pitch_N21[0] + (target - Get_right_shoulder_pitch_N21[0]) * k;
        //         Lower_Limbs_Motor_Knee_N32_Send_Data[0] = Get_Lower_Limbs_Motor_Knee_N32[0] + (target - Get_Lower_Limbs_Motor_Knee_N32[0]) * k;

        //         // right_shoulder_pitch_N21_Ctl->Send_MIT_PD_Control_Data(right_shoulder_pitch_N21, right_shoulder_pitch_N21_Send_Data[0], right_shoulder_pitch_N21_Send_Data[1], right_shoulder_pitch_N21_Send_Data[2], right_shoulder_pitch_N21_Send_Data[3], 0);
        //         // left_shoulder_pitch_N11_Ctl->Send_MIT_PD_Control_Data(left_shoulder_pitch_N11, Send_Data1[0], Send_Data1[1], Send_Data1[2], Send_Data1[3], 0);
        //         // left_wrist_yaw_N15_Ctl->Send_MIT_PD_Control_Data(left_wrist_yaw_N15, Send_Data[0], Send_Data[1], Send_Data[2], Send_Data[3], 0);
        //         Lower_Limbs_Motor_Knee_N32_Ctl->Send_MIT_PD_Control_Data(Lower_Limbs_Motor_Knee_N32, Lower_Limbs_Motor_Knee_N32_Send_Data[0], Lower_Limbs_Motor_Knee_N32_Send_Data[1], Lower_Limbs_Motor_Knee_N32_Send_Data[2], Lower_Limbs_Motor_Knee_N32_Send_Data[3], 0);

        //     #endif
        //     #ifdef TEST_Get_left_wrist_yaw_N15_Data
        //         // right_shoulder_pitch_N21_Ctl->Get_Motor_FB_Data(right_shoulder_pitch_N21, &Get_right_shoulder_pitch_N21[0], &Get_right_shoulder_pitch_N21[1], &Get_right_shoulder_pitch_N21[2], &Get_right_shoulder_pitch_N21[3], NULL);
        //         // left_shoulder_pitch_N11_Ctl->Get_Motor_FB_Data(left_shoulder_pitch_N11, &Get_left_shoulder_pitch_N11[0], &Get_left_shoulder_pitch_N11[1], &Get_left_shoulder_pitch_N11[2], &Get_left_shoulder_pitch_N11[3], NULL);
        //         // left_wrist_yaw_N15_Ctl->Get_Motor_FB_Data(left_wrist_yaw_N15, &Get_left_wrist_yaw_N15[0], &Get_left_wrist_yaw_N15[1], &Get_left_wrist_yaw_N15[2], &Get_left_wrist_yaw_N15[3], NULL);
        //         printf("PHU MOTOR ID: 15 !    pos: %f  speed: %f  force: %f  temp: %f\r\n", Get_Lower_Limbs_Motor_Knee_N32[0], Get_Lower_Limbs_Motor_Knee_N32[1], Get_Lower_Limbs_Motor_Knee_N32[2], Get_Lower_Limbs_Motor_Knee_N32[3]);
        //         printf("PHU MOTOR ID: 21 !    pos: %f  speed: %f  force: %f  temp: %f\r\n", Get_right_shoulder_pitch_N21[0], Get_right_shoulder_pitch_N21[1], Get_right_shoulder_pitch_N21[2], Get_right_shoulder_pitch_N21[3]);
        //         printf("PHU MOTOR ID: 11 !    pos: %f  speed: %f  force: %f  temp: %f\r\n", Get_left_shoulder_pitch_N11[0], Get_left_shoulder_pitch_N11[1], Get_left_shoulder_pitch_N11[2], Get_left_shoulder_pitch_N11[3]);
        //         printf("PHU MOTOR ID: 15 !    pos: %f  speed: %f  force: %f  temp: %f\r\n", Get_left_wrist_yaw_N15[0], Get_left_wrist_yaw_N15[1], Get_left_wrist_yaw_N15[2], Get_left_wrist_yaw_N15[3]);
        //     #endif
        //     usleep(9000);
        // }

        // left_wrist_yaw_N15_Ctl->Send_MIT_PD_Control_Data(left_wrist_yaw_N15, 0, 0, -1, 0, 0);
        // left_wrist_yaw_N15_Ctl->Get_Motor_FB_Data(left_wrist_yaw_N15, &Get_left_wrist_yaw_N15[0], &Get_left_wrist_yaw_N15[1], &Get_left_wrist_yaw_N15[2], &Get_left_wrist_yaw_N15[3], NULL);
        // printf("PHU MOTOR ID: 15 !    pos: %f  speed: %f  force: %f  temp: %f\r\n", Get_left_wrist_yaw_N15[0], Get_left_wrist_yaw_N15[1], Get_left_wrist_yaw_N15[2], Get_left_wrist_yaw_N15[3]);


        usleep(1000000);

    }
#endif

    return 0;
}
