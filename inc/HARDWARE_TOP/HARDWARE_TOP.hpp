#ifndef HARDWARE_TOP_H_
#define HARDWARE_TOP_H_
#include "syst.hpp"
#include "UDP.hpp"
#include "CAN.hpp"
#include <yaml-cpp/yaml.h>

using std::shared_ptr;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::map;

typedef struct Device_Struct
{
    shared_ptr<string> Name;
    shared_ptr<string> Range_Name;
    shared_ptr<string> DeviceType;
    u8 Head_Flag_CHX;
    u16 Rec_Id_Offest[32];
    u32 This_Send_UDP_CH;

    Main_Data Device_Main_Data;
    Main_Data Device_Main_Data_FB;

    void *Device_Private_Class;
    int (*Device_Private_CallBack)(void *, volatile u8 *);
    void (*DeviceType_Delete)(void *);

    shared_ptr<int> UDP_Def_Broadcast;
    shared_ptr<int> UDP_Def_Nomal;
    std::thread *thread_UDP_Def_Nomal_Rec;
    std::thread *thread_UDP_Def_Broadcast_Rec;
    shared_ptr<int> UDP_Send_Data_Buff;
    
    shared_ptr<std::map<std::string, shared_ptr<Device_Struct>>> Device_Map;
    shared_ptr<Device_Struct> Device_List[1024];
    shared_ptr<Device_Struct> Parent_Device;
    shared_ptr<int> Debug_Printf;

    YAML::Node Base_Node;
    YAML::Node This_Node;

    u32 IS_Online_In_Init;
    u32 Have_New_Msg;
} Device_Struct;

class Robot_Hardware
{
public:
    Robot_Hardware(void);
    ~Robot_Hardware(void);

	shared_ptr<std::map<std::string, shared_ptr<Device_Struct>>> Get_Device_Map(void);
    shared_ptr<std::map<std::string, shared_ptr<Device_Struct>>> Get_ETH_Device_Map(void);

    int Add_Device_Type(string Device_Type_Name, int (Init_F)(shared_ptr<Device_Struct>, YAML::Node *), int (CallBack_F)(void *, volatile u8 *), void (Delete_F)(void *));
    int Init_TOP(string File);
    int OTA_GO(string folder);
    shared_ptr<Device_Struct> Get_Device_For_Name(string Name);
    void *Get_Control_Class(shared_ptr<Device_Struct> Device_P);
    int Send_Buff_Data(void);
	int Send_Buff_Data(string Range_Name);
    int REBOOT_F(shared_ptr<Device_Struct> Device_P);

    int UDP_Broadcast_Send(u8 *Data, shared_ptr<Device_Struct> Device);
    int UDP_Nomal_Send(u8 *Data, shared_ptr<Device_Struct> Device);
    void CAN_Msg_To_UDP_Msg(shared_ptr<Device_Struct> Device, u8 *CAN_Msg);
    int Wait_FB(shared_ptr<Device_Struct> Device, int Msg_Num, int time);

private:
    class Robot;
    Robot *One_Robot;
};

#endif
