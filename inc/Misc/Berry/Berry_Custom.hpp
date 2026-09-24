#ifndef Berry_Custom_H_
#define Berry_Custom_H_

#include "HARDWARE_TOP.hpp"
#include <map>
#include <memory>
#include <functional>

#include <vector>
#include <numeric>
using namespace std;

#define Device_Do_Not_Test_Online_Custom 3

using SenF_Func = std::function<int(shared_ptr<Device_class>, u8*)>;

#define Berry_Custom_Type "Berry_Custom"

#define Berry_Custom_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Berry_Custom *One_Berry_Custom = new Berry_Custom();\
                                Device->Device_Private_Class = (void *)One_Berry_Custom;\
                                if (Node != nullptr)\
                                    return One_Berry_Custom->Berry_Custom_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Berry_Custom_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int \
                                    {\
                                        return ((Berry_Custom *)Device->Device_Private_Class)->Berry_Custom_Frame_Analyze(Msg);\
                                    }\

#define Berry_Custom_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Berry_Custom *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\

enum CmdType
{
    CMD_UNKNOWN,
    CMD_QUIT,
    CMD_MKDIR,
    CMD_DELETE,
    CMD_CD,
    CMD_RETRIEVE,
    CMD_SEND,
    CMD_BERRY
};                               

typedef struct Serial_Data
{ 
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

    // DATA
    u8 data[1024];
} Serial_Data;

typedef struct Berrt_Frame
{ 
    int mode;
    vector<uint8_t> Berry_data;
    string Berry_dir;
} Berrt_Frame;

class Berry_Custom : private Robot_Hardware
{
public:
    Berry_Custom(void);
	int Berry_Custom_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
    int Berry_Custom_Send(shared_ptr<Device_class> Device_P, u8 *Data, u32 len);
	int Berry_Custom_Frame_Analyze(volatile u8 *Can_Frame);



    shared_ptr<Device_class> Device_my;
    int open_file(const std::string& path);

    Berrt_Frame Berry_frme;
    void Berry_Send_Timeout();
    void Berry_Send();
    bool Berry_stop_flag = false;
    const uint32_t Berry_PERIOD_MS = 1000;
    uint32_t Berry_send_tick = 0;
    uint32_t Berry_r_tick = 0;
    string mcu_path = "/";

};



#endif