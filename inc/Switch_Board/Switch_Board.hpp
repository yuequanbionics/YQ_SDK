#ifndef switch_Board_H_
#define switch_Board_H_
#include "HARDWARE_TOP.hpp"
#include "Serial.hpp"
#include "FD_Can.hpp"
#include "GPIO.hpp"
#include "Timer.hpp"

#define Switch_Board_Type "Switch_Board"

#define Switch_Board_Device_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                                {\
                                    Main_B *One_Main_B = new Main_B();\
                                    Device->Device_Private_Class = (void *)One_Main_B;\
                                    if (Node != nullptr)\
                                        return One_Main_B->Get_Main_B_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                    else\
                                        return 0;\
                                }\

#define Switch_Board_Device_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                        {\
                                            return ((Main_B *)Device_Private_Class)->Main_B_Top_Frame_Analyze(Msg);\
                                        }\

#define Switch_Board_Device_Delete_F  [](void *Device_Private_Class)\
                                    {\
                                        delete ((Main_B *)Device_Private_Class);\
                                    }\

class Main_B : private Robot_Hardware
{
public:
    Serial  Serials;
    FDCan   m_fdCan;
    C_GPIO  m_GPIO;
    Timer   Timers;
    
    int Get_Main_B_Device_Data_From_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
    int Main_B_Top_Frame_Analyze(volatile u8 *Can_Frame);
    int Main_CMD_EX_Config(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
};

enum Main_B_CMD_Data_Enum
{
    Main_Board_Power_EN = 20,
    Main_CMD_USART_CONFIG,
    Main_CMD_USART_DATA,
    Main_CMD_Timer_Config,
    Main_CMD_CAN_CONFIG,
    Main_CMD_GPIO_CONFIG,
    Main_CMD_GPIO_CONTROL,
};

#endif
