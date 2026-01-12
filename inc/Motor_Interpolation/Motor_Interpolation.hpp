#ifndef Motor_Interpolation_H_
#define Motor_Interpolation_H_
#include "HARDWARE_TOP.hpp"

#define Motor_Interpolation_Device_Type "Motor_Interpolation_Device"

#define Motor_Interpolation_Device_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                                        {\
                                            Motor_Interpolation *One_Motor_Interpolation = new Motor_Interpolation();\
                                            Device->Device_Private_Class = (void *)One_Motor_Interpolation;\
                                            if (Node != nullptr)\
                                                return One_Motor_Interpolation->Get_Data_From_Yaml_And_Init(Device, *Node);\
                                            else\
                                                return 0;\
                                        }\

#define Motor_Interpolation_Device_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                                {\
                                                    return 0;\
                                                }\

#define Motor_Interpolation_Device_Delete_F  [](void *Device_Private_Class)\
                                            {\
                                                delete ((Motor_Interpolation *)Device_Private_Class);\
                                            }\

class Motor_Interpolation : private Robot_Hardware
{
public:
    typedef struct
    {
        string name;
        shared_ptr<Device_Struct> Device;

        float start_pos;
        float end_pos;
        float accel;
        float max_vel;
        float torque;

        float KP;
        float KD;
    } Motor_Interpolation_Struct;

    shared_ptr<std::map<std::string, shared_ptr<Motor_Interpolation_Struct>>> Map;
    Robot_Hardware *Robot_Hardware_P;
    bool LOG_COUT = true;

    int Init(const shared_ptr<Device_Struct>& Device, Robot_Hardware *Robot, float ACC, float KP, float KD);

    void To_Buff(const string& Name, float Pos, float max_vel, float torque, float Kp, float Kd) const;

    int INT_GO() const;

    static int Get_Data_From_Yaml_And_Init(const shared_ptr<Device_Struct>& Device, const YAML::Node& One_Node);
};

#endif
