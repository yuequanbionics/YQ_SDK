#ifndef Berry_H_
#define Berry_H_

#include "HARDWARE_TOP.hpp"
#include <map>
#include <memory>
#include <functional>

#include <vector>
#include <numeric>
using namespace std;

#define Berry_Head_Len 8

void printHexAndText(volatile const unsigned char* recvbuff, int data_len);
enum CmdType
{
    CMD_UNKNOWN,
    CMD_QUIT,
    CMD_MKDIR,
    CMD_DELETE,
    CMD_CD,
    CMD_RETRIEVE,
    CMD_SEND,
    CMD_BERRY,
    CMD_MKFS
};                               

typedef struct Berry_CMD
{ 
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

	u8 CRC8;
	u8 Cmd;
    // DATA
    u8 data[1024];
} Berry_CMD;


typedef struct Berry_Data
{ 
    // Head
	u8 Cmd;
	u8 flag;

    // DATA
    u8 data[512];
} Berry_Data;


typedef struct Berrt_Frame
{ 
    int mode;
    vector<uint8_t> Berry_data;
    string Berry_dir;
} Berrt_Frame;

class Berry : private Robot_Hardware
{
public:
    Berry(void);
	int Get_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
    int Berry_Custom_Send(shared_ptr<Device_class> Device_P, u8 *Data, u32 len);
	int Berry_Call_Back(volatile u8 *Can_Frame);



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