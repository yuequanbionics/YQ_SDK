#ifndef Can_Expansion_Custom_H_
#define Can_Expansion_Custom_H_

#include "HARDWARE_TOP.hpp"
#include <map>
#include <memory>
#include <functional>

#include <vector>
#include <numeric>
using namespace std;

#define Device_Do_Not_Test_Online_Custom 3

using SenF_Func = std::function<int(shared_ptr<Device_class>, u8*)>;

#define Can_Expansion_Custom_Type "Can_Expansion_Custom"

#define Can_Expansion_Custom_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Can_Expansion_Custom *One_Can_Expansion_Custom = new Can_Expansion_Custom();\
                                Device->Device_Private_Class = (void *)One_Can_Expansion_Custom;\
                                if (Node != nullptr)\
                                    return One_Can_Expansion_Custom->Can_Expansion_Custom_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Can_Expansion_Custom_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int \
                                    {\
                                        return ((Can_Expansion_Custom *)Device->Device_Private_Class)->Can_Expansion_Custom_Frame_Analyze(Msg);\
                                    }\

#define Can_Expansion_Custom_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Can_Expansion_Custom *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\

typedef struct Serial_Data
{ 
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

    // DATA
    u8 data[1024];
} Serial_Data;


struct CanFrame {
    uint8_t canch;       // 通道
    uint32_t canid;      // ID（4字节）
    uint8_t len;         // 数据长度
    uint8_t data[64];     // 数据（最长64字节）
};

typedef struct Set_OTA
{
    u8 Head = 0xAA;
    u8 Len;
    u8 cnt = 1;
    u8 flag = 0xC2;
    u8 result = 1;
    u32 num ;
    u8 data[1024];
    u8 check_num;
}Set_OTA;

typedef struct response_Data
{
    u8 flag;
    u32 num;
}response_Data;

typedef struct Set_FDCAN
{ 
    u8 Head = 0xAA;
    u8 Len = sizeof(Set_FDCAN);
    u8 cnt = 1;
    u8 flag = 0xC3;
    u8 CH;
    u8 BRS_EN;
    u8 NominalPrescaler;
    u8 NominalSyncJumpWidth;
    u8 Nominal_TimeSeg1;
    u8 Nominal_TimeSeg2;
    u8 DataPrescaler;
    u8 DataSyncJumpWidth;
    u8 DataTimeSeg1;
    u8 DataTimeSeg2;
    u8 check_num;

} Set_FDCAN;



class Can_Expansion_Custom : private Robot_Hardware
{
public:
    Can_Expansion_Custom(void);
	int Can_Expansion_Custom_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);


	int Can_Expansion_Custom_Frame_Analyze(volatile u8 *Can_Frame);
    bool Analyze_One_Frame(vector<uint8_t>& buffer);
    void Handle_Device_Response();
    vector<uint8_t> recv_buf;
    vector<CanFrame> g_can_list; 
    vector<response_Data> response_data; 

    int Send_F_Orin_CanFD(shared_ptr<Device_class> Device, u8 *Data);
    int Can_Expansion_Custom_Serial_Send();
    void Build_CanFrame(uint8_t canch, uint32_t canid, uint8_t len, uint8_t *data);
    vector<uint8_t> Get_CanFrame();
    vector<CanFrame> send_list;

    std::map<shared_ptr<Device_class>, SenF_Func> m_dev_old_senf;
    shared_ptr<Device_class> Device_my;
    
    Set_OTA Set_ota;
    Set_FDCAN Set_FDCan[3];
    Serial_Data Serial_Datas;

    
    void OAT_GO_Timeout();
    void OAT_GO();
    uint32_t m_last_ota_send_tick = 0;
    uint32_t m_last_ota_r_tick = 0;
    const uint32_t OAT_PERIOD_MS = 1000;
    bool ota_stop_flag =    false;
    vector<uint8_t> flash_data;

    void w_flash_data(vector<uint8_t>Data);
};


int Can_Data_Analysis(shared_ptr<Device_class> Device, u8* Data);

#endif