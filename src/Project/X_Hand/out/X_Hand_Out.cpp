#include <filesystem>
#include <iostream>

#include "Auto_Set_Id.hpp"
#include "Custom_TOP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Hw_Pressure_Sensor.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "Switch_Board_Orin.hpp"
#include "syst.hpp"

using namespace std;

Robot_Hardware* X_Hand;

shared_ptr<Device_class> Motor_1_D;
shared_ptr<Device_class> Motor_2_D;
shared_ptr<Device_class> Motor_3_D;
shared_ptr<Device_class> Motor_4_D;
shared_ptr<Device_class> Motor_5_D;
shared_ptr<Device_class> Motor_6_D;
shared_ptr<Device_class> Tactile_Sensor_D;
Motor* Motor_1_Control;
Motor* Motor_2_Control;
Motor* Motor_3_Control;
Motor* Motor_4_Control;
Motor* Motor_5_Control;
Motor* Motor_6_Control;
Hw_Pressure_Sensor* Tactile_Sensor_Control = nullptr;

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
// float Motor_K[6] = {3103 - 20, 421.953 - 5, 3779.49 - 20, 3833.81 - 20, 3739.36 - 20, 3780.85 - 20};
float Motor_K[6] = {
    2462.73 - 100,
    465.147,
    4207.49 - 200,
    4292.93 - 200,
    4163.54 - 200,
    4137.34 - 200,
};

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

    // 收
    Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, mirror * -0.5f, kp, kd);
    Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, mirror * 0.5f, kp, kd);
    Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, mirror * 0.5f, kp, kd);
    Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, mirror * 0.5f, kp, kd);
    Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, mirror * 0.5f, kp, kd);
    X_Hand->Send_Buff_Data();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 放
    Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, mirror * 0.5f, kp, kd);
    Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, mirror * -0.5f, kp, kd);
    Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, mirror * -0.5f, kp, kd);
    Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, mirror * -0.5f, kp, kd);
    Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, mirror * -0.5f, kp, kd);
    X_Hand->Send_Buff_Data();
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    while (true) {
        // 放
        Motor_1_Control->Send_MIT_PD_Control_Data(Motor_1_D, 0, 0, mirror * 0.15f, kp, kd);
        Motor_2_Control->Send_MIT_PD_Control_Data(Motor_2_D, 0, 0, mirror * 0.15f, kp, kd);
        Motor_3_Control->Send_MIT_PD_Control_Data(Motor_3_D, 0, 0, mirror * -0.15f, kp, kd);
        Motor_4_Control->Send_MIT_PD_Control_Data(Motor_4_D, 0, 0, mirror * -0.15f, kp, kd);
        Motor_5_Control->Send_MIT_PD_Control_Data(Motor_5_D, 0, 0, mirror * -0.15f, kp, kd);
        Motor_6_Control->Send_MIT_PD_Control_Data(Motor_6_D, 0, 0, mirror * -0.15f, kp, kd);
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
    if (Tactile_Sensor_Control->Start_Periodic_Data_Collection(Tactile_Sensor_D, 40)) {
        cout << " Tactile Sensor Init Failed" << endl;
        return;
    }  // 50ms

    this_thread::sleep_for(std::chrono::milliseconds(350));
}

void Tactile_Sensor_Get_Data(void) {
    static const vector<u8> sensor_ids = {HW_THUMB_ID, HW_INDEX_FINGER_ID, HW_MIDDLE_FINGER_ID, HW_RING_FINGER_ID, HW_LITTLE_FINGER_ID, HW_PALM_CENTER_ID};

    for (u8 sensor_id : sensor_ids) {
        vector<u16> data = Tactile_Sensor_Control->Get_Stored_Sensor_Data(sensor_id);  // 读取传感器数据
        if (!data.empty()) {
            g_sensor_data[sensor_id] = data;

            cout << "已存储 " << g_sensor_data[sensor_id].size() << " 个数据点: [";
            for (size_t i = 0; i < g_sensor_data[sensor_id].size(); ++i) {
                cout << g_sensor_data[sensor_id][i];
                if (i < g_sensor_data[sensor_id].size() - 1) {
                    cout << ", ";
                }
            }
            cout << "]" << endl;
        } else {
            cout << "数据为空或未收到数据。" << endl;
        }
    }
}

#ifndef HAVE_ROS
int main(int argc, char* argv[])
#else
int hardware_init(const string& ADDR, const string& Config)
#endif
{
    X_Hand = new Robot_Hardware();
    X_Hand->Add_Device_Type(Switch_Board_Type, Switch_Board_Device_Init, Switch_Board_Device_CallBack_F, Switch_Board_Device_Delete_F);
    X_Hand->Add_Device_Type(Switch_Board_Orin_Type, Switch_Board_Orin_Device_Init, Switch_Board_Orin_Device_CallBack_F, Switch_Board_Device_Orin_Delete_F);
    X_Hand->Add_Device_Type(Motor_Device_Type, Motor_Device_Init, Motor_Device_CallBack_F, Motor_Device_Delete_F);
    X_Hand->Add_Device_Type(Auto_Set_Id_Type, Auto_Set_Id_Init, Auto_Set_Id_CallBack_F, Auto_Set_Id_Delete_F);
    X_Hand->Add_Device_Type("Tactile_Sensor_Custom", Hw_Pressure_Sensor_Init, Hw_Pressure_Sensor_CallBack_F, Hw_Pressure_Sensor_Delete_F);

#ifndef HAVE_ROS
    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    filesystem::path dir_path = exe_path.parent_path();
    string ADDR = dir_path.string() + "/../config/YAML/X_Hand/out/mz_g_1.0/TOP.yaml";
    // string Config = "None";
    string Config = R"(
PC_IP: 192.168.3.245
SN: 1X1T2603005MG
Boards:
  - Id: 101
    IP: 192.168.3.105
    # Port: 19001
)";
#endif
    if (X_Hand->Init_TOP(ADDR, Config) != 0) {
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

    Motor_1_Control = static_cast<Motor*>(X_Hand->Get_Control_Class(Motor_1_D));
    Motor_2_Control = static_cast<Motor*>(X_Hand->Get_Control_Class(Motor_2_D));
    Motor_3_Control = static_cast<Motor*>(X_Hand->Get_Control_Class(Motor_3_D));
    Motor_4_Control = static_cast<Motor*>(X_Hand->Get_Control_Class(Motor_4_D));
    Motor_5_Control = static_cast<Motor*>(X_Hand->Get_Control_Class(Motor_5_D));
    Motor_6_Control = static_cast<Motor*>(X_Hand->Get_Control_Class(Motor_6_D));

    if (Tactile_Sensor_D != nullptr) {
        Tactile_Sensor_Control = static_cast<Hw_Pressure_Sensor*>(X_Hand->Get_Control_Class(Tactile_Sensor_D));

        Tactile_Sensor_Init();
    }

    X_hand_Init();

#ifndef HAVE_ROS

    constexpr __useconds_t loop_time_step = 3000 * 1000 / 500;
    int times = 0;
    float test = 0.5;

    /* ------------------- 反复握拳 ------------------- */
#if 1
    // ReSharper disable once CppDFAEndlessLoop
    while (true) {
        if (times % 500 * 2 == 0) {
            test = -test;
        }
        times++;

        for (int i = 0; i < 6; i++) {
            Send_Datas[i].P = 0.5f + test;
            Send_Datas[i].V = 0;
            Send_Datas[i].F = 0;
            Send_Datas[i].KP = 500;  // 800
            Send_Datas[i].KD = 10;   // 20
        }
        Send_Datas[0].P = 0.5f / 4 + test / 4;
        Send_Datas[1].P = 0.5f / 2 + test / 2;

        Send();
        usleep(loop_time_step);
        Get_FB();

        for (int i = 0; i < 6; i++) {
            cout << "num: " << i << " " << abs(FB_Datas[i].V * 100) << endl;
        }
        // Tactile_Sensor_Get_Data();
    }
#endif
    /* ------------------- 四指握力动作 ------------------- */
    // 需要把 yaml 电流放开 DI_I_MAX: 2.0
#if 0
    // ReSharper disable once CppDFAEndlessLoop
    while (true) {
        /* ------------------- 收 ------------------- */
        /* 拇指 */
        for (int i = 0; i < 2; i++) {
            Send_Datas[i].P = 0.5f + test;
            Send_Datas[i].V = 0;
            Send_Datas[i].F = 0;
            Send_Datas[i].KP = 800;
            Send_Datas[i].KD = 5;
        }
        Send_Datas[0].P = 0.5f/4 + test/4;
        Send_Datas[1].P = 1.2f/2 + test/2;

        Send();
        usleep(2000000);

        /* 四指 */
        for (int i = 2; i < 6; i++) {
            Send_Datas[i].P = 0.5f + test;
            Send_Datas[i].V = 0;
            Send_Datas[i].F = 0;
            Send_Datas[i].KP = 800;     // 800
            Send_Datas[i].KD = 5;      // 20
        }
        Send();
        usleep(4000000);

        /* ------------------- 放 ------------------- */
        test *= -1;

        /* 四指 */
        for (int i = 2; i < 6; i++) {
            Send_Datas[i].P = 0.5f + test;
            Send_Datas[i].V = 0;
            Send_Datas[i].F = 0;
            Send_Datas[i].KP = 800;     // 800
            Send_Datas[i].KD = 5;      // 20
        }
        Send();
        usleep(2000000);

        /* 拇指 */
        // for (int i = 0; i < 2; i++) {
        //     Send_Datas[i].P = 0.5f + test;
        //     Send_Datas[i].V = 0;
        //     Send_Datas[i].F = 0;
        //     Send_Datas[i].KP = 800;
        //     Send_Datas[i].KD = 20;
        // }
        // Send_Datas[0].P = 0.5f/4 + test/4;
        // Send_Datas[1].P = 1.2f/2 + test/2;
        // Send();
        usleep(1000000);

        test *= -1;

    }
#endif

#endif

    return 0;
}
