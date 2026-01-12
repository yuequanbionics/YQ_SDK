#include <filesystem>
#include <iostream>

#include "Auto_Set_Id.hpp"
#include "Custom_TOP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Hw_Pressure_Sensor.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "syst.hpp"

using namespace std;

Robot_Hardware* X_Hand;

shared_ptr<Device_Struct> Motor_1_D;
shared_ptr<Device_Struct> Motor_2_D;
shared_ptr<Device_Struct> Motor_3_D;
shared_ptr<Device_Struct> Motor_4_D;
shared_ptr<Device_Struct> Motor_5_D;
shared_ptr<Device_Struct> Motor_6_D;
shared_ptr<Device_Struct> Tactile_Sensor_D;
Motor* Motor_1_Control;
Motor* Motor_2_Control;
Motor* Motor_3_Control;
Motor* Motor_4_Control;
Motor* Motor_5_Control;
Motor* Motor_6_Control;
Hw_Pressure_Sensor* Tactile_Sensor_Control;

typedef struct X_hand_FB {
    float P;
    float V;
    float F;
    float temp[2];
    u16 error;
} X_hand_FB;

typedef struct X_hand_Send_Data {
    float P;
    float V;
    float F;
    float KP;
    float KD;
} X_hand_Send_Data;

int init_time_step = 1000 * 1000 / 500;
float Pos_Offest[6] = {0, 0, 0, 0, 0, 0};
float Motor_Mirror[6] = {-1, -1, 1, 1, 1, 1};
// float Motor_K[6] = {1, 1, 1, 1, 1, 1};
float Motor_K[6] = {3103 - 20, 421.953 - 5, 3779.49 - 20, 3833.81 - 20, 3739.36 - 20, 3780.85 - 20};

X_hand_Send_Data Send_Datas[6];
X_hand_FB FB_Datas[6];

map<u8, vector<u16>> g_sensor_data;

void Get_FB(void) {
    Motor_1_Control->Get_Motor_FB_Data(Motor_1_D, &FB_Datas[0].P, &FB_Datas[0].V, &FB_Datas[0].F, FB_Datas[0].temp, &FB_Datas[0].error);
    Motor_2_Control->Get_Motor_FB_Data(Motor_2_D, &FB_Datas[1].P, &FB_Datas[1].V, &FB_Datas[1].F, FB_Datas[1].temp, &FB_Datas[1].error);
    Motor_3_Control->Get_Motor_FB_Data(Motor_3_D, &FB_Datas[2].P, &FB_Datas[2].V, &FB_Datas[2].F, FB_Datas[2].temp, &FB_Datas[2].error);
    Motor_4_Control->Get_Motor_FB_Data(Motor_4_D, &FB_Datas[3].P, &FB_Datas[3].V, &FB_Datas[3].F, FB_Datas[3].temp, &FB_Datas[3].error);
    Motor_5_Control->Get_Motor_FB_Data(Motor_5_D, &FB_Datas[4].P, &FB_Datas[4].V, &FB_Datas[4].F, FB_Datas[4].temp, &FB_Datas[4].error);
    Motor_6_Control->Get_Motor_FB_Data(Motor_6_D, &FB_Datas[5].P, &FB_Datas[5].V, &FB_Datas[5].F, FB_Datas[5].temp, &FB_Datas[5].error);

    for (int i = 0; i < 6; i++) {
        FB_Datas[i].P = (FB_Datas[i].P - Pos_Offest[i]) * Motor_Mirror[i] / Motor_K[i];
        FB_Datas[i].F = FB_Datas[i].F * Motor_Mirror[i];
        FB_Datas[i].V = FB_Datas[i].V * Motor_Mirror[i] / Motor_K[i];
    }
}

void Send(void) {
    for (int i = 0; i < 6; i++) {
        if (Send_Datas[i].P > 1)
            Send_Datas[i].P = 1.0f;
        else if (Send_Datas[i].P < 0)
            Send_Datas[i].P = 0.0f;
    }

    Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, (Send_Datas[0].P * Motor_Mirror[0] * Motor_K[0] + Pos_Offest[0]), Send_Datas[0].V * Motor_Mirror[0] * Motor_K[0], Send_Datas[0].F * Motor_Mirror[0], Send_Datas[0].KP, Send_Datas[0].KD);
    Motor_2_Control->Send_MIT_PD_Control_Data(Motor_2_D, (Send_Datas[1].P * Motor_Mirror[1] * Motor_K[1] + Pos_Offest[1]), Send_Datas[1].V * Motor_Mirror[1] * Motor_K[1], Send_Datas[1].F * Motor_Mirror[1], Send_Datas[1].KP, Send_Datas[1].KD);
    Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, (Send_Datas[2].P * Motor_Mirror[2] * Motor_K[2] + Pos_Offest[2]), Send_Datas[2].V * Motor_Mirror[2] * Motor_K[2], Send_Datas[2].F * Motor_Mirror[2], Send_Datas[2].KP, Send_Datas[2].KD);
    Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, (Send_Datas[3].P * Motor_Mirror[3] * Motor_K[3] + Pos_Offest[3]), Send_Datas[3].V * Motor_Mirror[3] * Motor_K[3], Send_Datas[3].F * Motor_Mirror[3], Send_Datas[3].KP, Send_Datas[3].KD);
    Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, (Send_Datas[4].P * Motor_Mirror[4] * Motor_K[4] + Pos_Offest[4]), Send_Datas[4].V * Motor_Mirror[4] * Motor_K[4], Send_Datas[4].F * Motor_Mirror[4], Send_Datas[4].KP, Send_Datas[4].KD);
    Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, (Send_Datas[5].P * Motor_Mirror[5] * Motor_K[5] + Pos_Offest[5]), Send_Datas[5].V * Motor_Mirror[5] * Motor_K[5], Send_Datas[5].F * Motor_Mirror[5], Send_Datas[5].KP, Send_Datas[5].KD);
    X_Hand->Send_Buff_Data();
}

void Get_K(void) {
    float K_Buff[6];
    for (int i = 0; i < 6; i++) {
        Send_Datas[i].P = 0;
        Send_Datas[i].V = 0;
        Send_Datas[i].F = 0;
        Send_Datas[i].KP = 0;
        Send_Datas[i].KD = 0;
    }

    Send_Datas[0].F = 0.5;
    Send_Datas[1].F = 0.5;
    Send();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    Send_Datas[0].F = 0.3;
    Send_Datas[1].F = 0.3;

    for (int i = 0; i < 10; i++) {
        Send();
        usleep(init_time_step);
        Get_FB();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    K_Buff[0] = fabs(FB_Datas[0].P);
    K_Buff[1] = fabs(FB_Datas[1].P);

    for (int i = 0; i < 6; i++) {
        Send_Datas[i].P = 0;
        Send_Datas[i].V = 0;
        Send_Datas[i].F = 0.5;
        Send_Datas[i].KP = 0;
        Send_Datas[i].KD = 0;
    }
    Send_Datas[0].F = -0.5;
    Send_Datas[1].F = -0.5;
    Send();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    for (int i = 0; i < 6; i++) {
        Send_Datas[i].P = 0;
        Send_Datas[i].V = 0;
        Send_Datas[i].F = 0.15;
        Send_Datas[i].KP = 0;
        Send_Datas[i].KD = 0;
    }
    Send_Datas[0].F = -0.15;
    Send_Datas[1].F = -0.15;
    for (int i = 0; i < 10; i++) {
        Send();
        usleep(init_time_step);
        Get_FB();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    K_Buff[2] = fabs(FB_Datas[2].P);
    K_Buff[3] = fabs(FB_Datas[3].P);
    K_Buff[4] = fabs(FB_Datas[4].P);
    K_Buff[5] = fabs(FB_Datas[5].P);

    for (int i = 0; i < 6; i++)
        cout << K_Buff[i] << endl;
}

void X_hand_Init(void) {
    float kp = 0;
    float kd = 0;
    int times = 0;
    float mirror = 1;
    float Motor_Mirror_buff[6] = {-1, -1, 1, 1, 1, 1};
    float Motor_K_buff[6] = {-1, -1, 1, 1, 1, 1};
    for (int i = 0; i < 6; i++) {
        Motor_Mirror_buff[i] = Motor_Mirror[i];
        Pos_Offest[i] = 0;
        Motor_Mirror[i] = 1.0f;
        Motor_K_buff[i] = Motor_K[i];
        Motor_K[i] = 1;
    }

    Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, mirror * -0.3, kp, kd);
    Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, mirror * 0.3, kp, kd);
    Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, mirror * 0.3, kp, kd);
    Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, mirror * 0.3, kp, kd);
    Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, mirror * 0.3, kp, kd);
    X_Hand->Send_Buff_Data();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, mirror * 0.6, kp, kd);
    Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, mirror * -0.6, kp, kd);
    Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, mirror * -0.6, kp, kd);
    Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, mirror * -0.6, kp, kd);
    Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, mirror * -0.6, kp, kd);
    X_Hand->Send_Buff_Data();
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    while (1) {
        Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, mirror * 0.1, kp, kd);
        Motor_2_Control->Send_MIT_PD_Control_Data(Motor_2_D, 0, 0, mirror * 0.1, kp, kd);
        Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, mirror * -0.1, kp, kd);
        Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, mirror * -0.1, kp, kd);
        Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, mirror * -0.1, kp, kd);
        Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, mirror * -0.1, kp, kd);
        X_Hand->Send_Buff_Data();
        usleep(init_time_step);
        Get_FB();

        bool Flag = true;
        for (int i = 0; i < 6; i++) {
            if (fabs(FB_Datas[i].V) >= 100)
                Flag = false;
        }
        if (Flag)
            times++;
        else
            times = 0;
        if (Flag && times > 1000000 / init_time_step / 2)
            break;
    }

    times = 3;
    while (times--) {
        Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, 0, kp, kd);
        Motor_2_Control->Send_MIT_PD_Control_Data(Motor_2_D, 0, 0, 0, kp, kd);
        Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, 0, kp, kd);
        Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, 0, kp, kd);
        Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, 0, kp, kd);
        Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, 0, kp, kd);
        X_Hand->Send_Buff_Data();
        usleep(init_time_step);
    }

    for (int i = 0; i < 6; i++) {
        Motor_K[i] = Motor_K_buff[i];
        Motor_Mirror[i] = Motor_Mirror_buff[i];
        Pos_Offest[i] = FB_Datas[i].P + 90 * Motor_Mirror[i];
    }
    Pos_Offest[1] -= 90 * Motor_Mirror[1];
}

void Tactile_Sensor_Init(void) {
    static const vector<u8> sensor_ids = {HW_THUMB_ID, HW_INDEX_FINGER_ID, HW_MIDDLE_FINGER_ID, HW_RING_FINGER_ID, HW_LITTLE_FINGER_ID, HW_PALM_CENTER_ID};

    if (Tactile_Sensor_Control->Start_Periodic_Data_Collection(Tactile_Sensor_D, 40)) {
        cout << " Tactile Sensor Init Failed" << endl;
        return;
    }  // 50ms

    this_thread::sleep_for(std::chrono::milliseconds(350));

    bool all_sensors_ok = true;

    for (u8 sensor_id : sensor_ids) {
        vector<u16> data = Tactile_Sensor_Control->Get_Stored_Sensor_Data(sensor_id);  // 读取传感器数据
        if (data.empty()) {
            // cout << "sensor ID " << static_cast<int>(sensor_id) << " empty data" << endl;
            all_sensors_ok = false;
        } else {
            g_sensor_data[sensor_id] = data;
            // cout << "sensor ID " << static_cast<int>(sensor_id) << " data size : " << data.size() << endl;
        }
    }
    if (!all_sensors_ok) {
        cout << "warning: some sensors failed to init" << endl;
    }
}

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(string ADDR)
#endif
{
    X_Hand = new Robot_Hardware();
    X_Hand->Add_Device_Type("Switch_Board", Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    X_Hand->Add_Device_Type("Motor_Device", Motor_Device_Init, Motor_Device_CallBack_F, Motor_Device_Delete_F);
    X_Hand->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
    X_Hand->Add_Device_Type("Tactile_Sensor_Custom", Hw_Pressure_Sensor_Init, Hw_Pressure_Sensor_CallBack_F, Hw_Pressure_Sensor_Delete_F);

#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    filesystem::path dir_path = exe_path.parent_path();
    string ADDR = dir_path.string() + "/../config/YAML/X_Hand/out/TOP.yaml";
#endif
    if (X_Hand->Init_TOP(ADDR) != 0) {
        cout << "Init_ERR" << endl;
        return -1;
    }

    Motor_1_D = X_Hand->Get_Device_For_Name("Motor_1");
    Motor_2_D = X_Hand->Get_Device_For_Name("Motor_2");
    Motor_3_D = X_Hand->Get_Device_For_Name("Motor_3");
    Motor_4_D = X_Hand->Get_Device_For_Name("Motor_4");
    Motor_5_D = X_Hand->Get_Device_For_Name("Motor_5");
    Motor_6_D = X_Hand->Get_Device_For_Name("Motor_6");
    Tactile_Sensor_D = X_Hand->Get_Device_For_Name("Tactile_Sensor");

    Motor_1_Control = (Motor*)X_Hand->Get_Control_Class(Motor_1_D);
    Motor_2_Control = (Motor*)X_Hand->Get_Control_Class(Motor_2_D);
    Motor_3_Control = (Motor*)X_Hand->Get_Control_Class(Motor_3_D);
    Motor_4_Control = (Motor*)X_Hand->Get_Control_Class(Motor_4_D);
    Motor_5_Control = (Motor*)X_Hand->Get_Control_Class(Motor_5_D);
    Motor_6_Control = (Motor*)X_Hand->Get_Control_Class(Motor_6_D);
    Tactile_Sensor_Control = (Hw_Pressure_Sensor*)X_Hand->Get_Control_Class(Tactile_Sensor_D);

    X_hand_Init();

    Tactile_Sensor_Init();

#ifndef HAVE_ROS

    float loop_time_step = 1000 * 1000 / 500;
    int times = 0;
    float test = 0.5;
    while (1) {
        if (times % 500 * 2 == 0) {
            test = -test;
        }
        times++;

        for (int i = 0; i < 6; i++) {
            Send_Datas[i].P = 0.5 + test;
            Send_Datas[i].V = 0;
            Send_Datas[i].F = 0;
            Send_Datas[i].KP = 500;
            Send_Datas[i].KD = 5;
        }
        Send_Datas[0].P = 0.25 + test / 2;
        Send_Datas[1].P = 0.1 + test / 5;

        Send();
        usleep(loop_time_step);
        Get_FB();

        for (int i = 0; i < 6; i++) {
            cout << "num: " << i << " " << FB_Datas[i].P << endl;
        }
    }
#endif

    return 0;
}
