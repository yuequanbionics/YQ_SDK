#ifndef TOP_H_
#define TOP_H_

// script auto start -> IN_USE
// script auto end -> IN_USE

// script auto start -> OUT_USE
#ifndef IN_USE
#include "syst.hpp"
#include "UDP.hpp"
#include "CAN.hpp"
#include <yaml-cpp/yaml.h>

using std::shared_ptr;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::map;

enum Err_Level {
    info_no_error = 0,    // 信息
    warn,        // 警告
    err,         // 错误
    critical     // 严重错误
};

class Device_class
{
public:
    u16 Head_Flag_CHX; 
    u16 Rec_Id_Offest[32];
    shared_ptr<string> Name;
    shared_ptr<string> Range_Name;
    shared_ptr<string> DeviceType;
    Main_Data Device_Main_Data;
    Main_Data Device_Main_Data_FB;
    
    void *Device_Private_Class;
    void (*DeviceType_Delete)(void *);

    int (*Sen_F)(shared_ptr<Device_class> Device, u8 *Data);
    int (*Rec_F_Pr)(shared_ptr<Device_class> Device, u8 *Data);
    int (*Rec_F_Base)(shared_ptr<Device_class> Device, u8 *Data);

    shared_ptr<Device_class> Parent_Device;
    shared_ptr<Device_class> Child_Device_List[1024];
    shared_ptr<std::map<std::string, shared_ptr<Device_class>>> All_Device_Map;

    shared_ptr<std::map<std::string, Err_Level>> Error_Map;
    int (*Error_FB)(shared_ptr<Device_class> Device, std::string Info, Err_Level Level);

    YAML::Node Yaml_Node;
    YAML::Node This_Node;

    u32 IS_Online_In_Init;
    u32 Have_New_Msg;
};

class Robot_Hardware
{
public:
    Robot_Hardware(void);
    ~Robot_Hardware(void);

    shared_ptr<std::map<std::string, shared_ptr<Device_class>>> Get_Device_Map(void);
    shared_ptr<std::map<std::string, shared_ptr<Device_class>>> Get_ETH_Device_Map(void);

    int Add_Device_Type(string Device_Type_Name, int (Init_F)(shared_ptr<Device_class>, YAML::Node *), int (CallBack_F)(shared_ptr<Device_class>, u8 *), void (Delete_F)(void *));
    int Init_TOP(string File);
    int Init_TOP(string File, string Config);
    int OTA_GO(string folder);
    shared_ptr<Device_class> Get_Device_For_Name(string Name);
    void *Get_Control_Class(shared_ptr<Device_class> Device_P);
    YAML::Node Get_Yaml_Node(void);
    int Send_Buff_Data(void);
    int Send_Buff_Data(string Range_Name);
    int REBOOT_F(shared_ptr<Device_class> Device_P);

    int UDP_Broadcast_Send(shared_ptr<Device_class> Device, u8 *Data);
    void CAN_Msg_To_UDP_Msg(shared_ptr<Device_class> Device, u8 *CAN_Msg);
    int Wait_FB(shared_ptr<Device_class> Device, int Msg_Num, int time);

private:
    class Robot;
    Robot *One_Robot;
};
#endif
// script auto end -> OUT_USE

#endif
