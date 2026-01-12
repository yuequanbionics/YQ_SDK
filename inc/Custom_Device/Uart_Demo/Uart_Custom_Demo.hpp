#ifndef Uart_Custom_Demo_H_
#define Uart_Custom_Demo_H_
#include "HARDWARE_TOP.hpp"

#define Uart_Custom_Dome_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                            {\
                                Uart_Custom_Dome *One_Uart_Custom_Dome = new Uart_Custom_Dome();\
                                Device->Device_Private_Class = (void *)One_Uart_Custom_Dome;\
                                if (Node != nullptr)\
                                    return One_Uart_Custom_Dome->Uart_Custom_Dome_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Uart_Custom_Dome_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                    {\
                                        return ((Uart_Custom_Dome *)Device_Private_Class)->Uart_Custom_Dome_Frame_Analyze(Msg);\
                                    }\

#define Uart_Custom_Dome_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Uart_Custom_Dome *)Device_Private_Class);\
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

class Uart_Custom_Dome : private Robot_Hardware
{
public:
	int Uart_Custom_Dome_Data_From_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
	int Uart_Custom_Dome_Frame_Analyze(volatile u8 *Can_Frame);

    int Serial_Send(shared_ptr<Device_Struct> Device_P, u8 *Data, u32 len);
	int (*Custom_Msg_CallBack)(volatile u8 *);

	Uart_Custom_Dome(void);
};

#endif