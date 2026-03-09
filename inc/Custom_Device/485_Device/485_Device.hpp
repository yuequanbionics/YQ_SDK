#ifndef _485_DEVICE_H_
#define _485_DEVICE_H_

#include "HARDWARE_TOP.hpp"

#define Uart_Custom_Dome_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Uart_Custom_Dome *One_Uart_Custom_Dome = new Uart_Custom_Dome();\
                                Device->Device_Private_Class = (void *)One_Uart_Custom_Dome;\
                                if (Node != nullptr)\
                                    return One_Uart_Custom_Dome->Uart_Custom_Dome_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Uart_Custom_Dome_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int \
                                    {\
                                        return ((Uart_Custom_Dome *)Device->Device_Private_Class)->Uart_Custom_Dome_Frame_Analyze(Msg);\
                                    }\

#define Uart_Custom_Dome_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Uart_Custom_Dome *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\

class t485_Device : private Robot_Hardware
{
public:
	int t485_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
	int t485_Device_Frame_Analyze(volatile u8 *Can_Frame);

};

#endif
