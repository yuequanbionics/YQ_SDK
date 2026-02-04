#include <filesystem>
#include <iostream>

#include "Auto_Set_Id.hpp"
#include "Custom_TOP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Eyou_Motor_TOP.hpp"
#include "Motor_TaiHu.hpp"
#include "Motor_BM_M1502D.hpp"
#include "Led_Device.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "syst.hpp"

using namespace std;

Robot_Hardware *Test_Robot;

/**
 * @brief 下肢电机
*/
shared_ptr<Device_Struct>  Lower_Limbs_Motor_Waist_Roll;
shared_ptr<Device_Struct>  Lower_Limbs_Motor_Waist_Yaw;
shared_ptr<Device_Struct>  Lower_Limbs_Motor_Knee;
shared_ptr<Device_Struct>  Lower_Limbs_Motor_Hip;
shared_ptr<Device_Struct>  Lower_Limbs_Motor_Ankel_Left;
shared_ptr<Device_Struct>  Lower_Limbs_Motor_Ankel_Right;

/**
 * @brief 左臂电机
*/
shared_ptr<Device_Struct>  Arm_left_shoulder_pitch;
shared_ptr<Device_Struct>  Arm_left_shoulder_roll;
shared_ptr<Device_Struct>  Arm_left_shoulder_yaw;
shared_ptr<Device_Struct>  Arm_left_elbow;
shared_ptr<Device_Struct>  Arm_left_wrist_yaw;
shared_ptr<Device_Struct>  Arm_left_wrist_pitch;
shared_ptr<Device_Struct>  Arm_left_wrist_roll;

/**
 * @brief 右臂电机
*/
shared_ptr<Device_Struct>  Arm_right_shoulder_pitch;
shared_ptr<Device_Struct>  Arm_right_shoulder_roll;
shared_ptr<Device_Struct>  Arm_right_shoulder_yaw;
shared_ptr<Device_Struct>  Arm_right_elbow;
shared_ptr<Device_Struct>  Arm_right_wrist_yaw;
shared_ptr<Device_Struct>  Arm_right_wrist_pitch;
shared_ptr<Device_Struct>  Arm_right_wrist_roll;

/**
 * @brief 底盘轮子
*/
shared_ptr<Device_Struct>  Classis_Motor_1;
shared_ptr<Device_Struct>  Classis_Motor_2;

/**
 * @brief 灯带设备
*/
shared_ptr<Device_Struct>  Led_Tape_Lights;

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(string ADDR)
#endif
{
    Test_Robot = new Robot_Hardware();
    Test_Robot->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    Test_Robot->Add_Device_Type("Eyou_Custom_Motor_TOP", Eyou_Custom_Motor_Device_Init, Eyou_Custom_Motor_Device_CallBack_F, Eyou_Custom_Motor_Device_Delete_F);
    Test_Robot->Add_Device_Type("TaiHu_Custom_Motor", Motor_Device_Init_TaiHu, Motor_Device_CallBack_TaiHu, Motor_Device_Delete_TaiHu);
    Test_Robot->Add_Device_Type("BenMo_Custom_Motor", Motor_Device_Init_BM_M1502D, Motor_Device_CallBack_BM_M1502D, Motor_Device_Delete_BM_M1502D);
    Test_Robot->Add_Device_Type("Led_Device", Led_Device_Init, Led_Device_CallBack_F, Led_Device_Delete_F);
    Test_Robot->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
 
#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");  
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "程序所在目录: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/W_Bot/out/TOP.yaml";
#endif
    if(Test_Robot->Init_TOP(ADDR) != 0)
    { 
        cout << "Init_ERR" << endl;
        return -1;
    }

    // string ADDR_OTA = "/home/toko/SP/sdk_2/config/OTA_BAG/Y_Hand_OTA";
    // Test_Robot->OTA_GO(ADDR_OTA);
    // return 0;

    Lower_Limbs_Motor_Waist_Roll    = Test_Robot->Get_Device_For_Name("Eyou_Waist_Roll");
    Lower_Limbs_Motor_Waist_Yaw     = Test_Robot->Get_Device_For_Name("Eyou_Waist_Yaw");
    Lower_Limbs_Motor_Knee          = Test_Robot->Get_Device_For_Name("Eyou_Knee");
    Lower_Limbs_Motor_Hip           = Test_Robot->Get_Device_For_Name("Eyou_Hip");
    Lower_Limbs_Motor_Ankel_Left    = Test_Robot->Get_Device_For_Name("Eyou_Ankel_Left");
    Lower_Limbs_Motor_Ankel_Right   = Test_Robot->Get_Device_For_Name("Eyou_Ankel_Right");

    Arm_left_shoulder_pitch = Test_Robot->Get_Device_For_Name("left_shoulder_pitch");
    Arm_left_shoulder_roll  = Test_Robot->Get_Device_For_Name("left_shoulder_roll");
    Arm_left_shoulder_yaw   = Test_Robot->Get_Device_For_Name("left_shoulder_yaw");
    Arm_left_elbow          = Test_Robot->Get_Device_For_Name("left_elbow");
    Arm_left_wrist_yaw      = Test_Robot->Get_Device_For_Name("left_wrist_yaw");
    Arm_left_wrist_pitch    = Test_Robot->Get_Device_For_Name("left_wrist_pitch");
    Arm_left_wrist_roll     = Test_Robot->Get_Device_For_Name("left_wrist_roll");

    Arm_right_shoulder_pitch = Test_Robot->Get_Device_For_Name("right_shoulder_pitch");
    Arm_right_shoulder_roll  = Test_Robot->Get_Device_For_Name("right_shoulder_roll");
    Arm_right_shoulder_yaw   = Test_Robot->Get_Device_For_Name("right_shoulder_yaw");
    Arm_right_elbow          = Test_Robot->Get_Device_For_Name("right_elbow");
    Arm_right_wrist_yaw      = Test_Robot->Get_Device_For_Name("right_wrist_yaw");
    Arm_right_wrist_pitch    = Test_Robot->Get_Device_For_Name("right_wrist_pitch");
    Arm_right_wrist_roll     = Test_Robot->Get_Device_For_Name("right_wrist_roll");

    Classis_Motor_1     = Test_Robot->Get_Device_For_Name("BenMo_Motor");
    Classis_Motor_2     = Test_Robot->Get_Device_For_Name("BenMo_Motor1");

    Led_Tape_Lights     = Test_Robot->Get_Device_For_Name("Led_Tape_Lights");

#ifndef HAVE_ROS

    while (1)
    {   
    }
#endif

    return 0;
}






