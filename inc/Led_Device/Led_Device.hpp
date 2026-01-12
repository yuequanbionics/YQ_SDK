#ifndef _LED_DEVICE_HPP_
#define _LED_DEVICE_HPP_

#include "HARDWARE_TOP.hpp"
#include "Switch_Board.hpp"

#define Led_Device_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                                {\
                                    Led_Device *Led_Device_C = new Led_Device();\
                                    Device->Device_Private_Class = (void *)Led_Device_C;\
                                    if (Node != nullptr)\
                                        return Led_Device_C->Get_Data_From_Yaml_And_Init(Device, *Node);\
                                    else\
                                        return 0;\
                                }\

#define Led_Device_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                        {\
                                            return ((Led_Device *)Device_Private_Class)->Top_Frame_Analyze(Msg);\
                                        }\

#define Led_Device_Delete_F  [](void *Device_Private_Class)\
                                    {\
                                        delete ((Led_Device *)Device_Private_Class);\
                                    }\


class Led_Device : private Robot_Hardware{
    public:
        int Get_Data_From_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
        int Top_Frame_Analyze(volatile u8 *Can_Frame);

        int Led_Set_The_Color(shared_ptr<Device_Struct> Device, uint8_t htim_index, uint8_t ch_index, uint16_t led_num, uint8_t C_R, uint8_t C_G, uint8_t C_B);

};

#endif
