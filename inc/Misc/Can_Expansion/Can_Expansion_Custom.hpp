#ifndef Can_Expansion_Custom_H_
#define Can_Expansion_Custom_H_

#include "HARDWARE_TOP.hpp"

#include <vector>
#include <numeric>
using namespace std;

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



class Can_Expansion_Custom : private Robot_Hardware
{
public:
	int Can_Expansion_Custom_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
	int Can_Expansion_Custom_Frame_Analyze(volatile u8 *Can_Frame);

    int Serial_Send(shared_ptr<Device_class> Device_P);
	int (*Custom_Msg_CallBack)(volatile u8 *);

	Can_Expansion_Custom(void);

    void Build_CanFrame(uint8_t canch, uint32_t canid, uint8_t len, uint8_t *data);
    vector<uint8_t> Get_CanFrame();
    vector<CanFrame> send_list;
};

#endif