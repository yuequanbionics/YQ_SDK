//
// Created by lwz on 2026/4/15.
//

#ifndef SDK_SWITCH_BOARD_ORIN_HPP
#define SDK_SWITCH_BOARD_ORIN_HPP

#include "HARDWARE_TOP.hpp"
#include "Serial.hpp"
#include "GPIO.hpp"

#define Device_IS_Online_In_Init 1
#define Device_IS_NO_Online 0
#define Device_Do_Not_Test_Online 2
#define Device_Do_Not_Test_Online_Custom 3

#define Switch_Board_Orin_Type "Switch_Board_Orin"

#define Switch_Board_Orin_Device_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                                        {\
                                            Main_B_Orin *One_Main_B = new Main_B_Orin();\
                                            Device->Device_Private_Class = (void *)One_Main_B;\
                                            if (Node != nullptr)\
                                            return One_Main_B->Get_Main_B_Orin_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                            else\
                                            return 0;\
                                        }\

#define Switch_Board_Orin_Device_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int\
                                                {\
                                                    return ((Main_B_Orin *)Device->Device_Private_Class)->Main_B_Orin_Top_Frame_Analyze(Msg);\
                                                }\

#define Switch_Board_Device_Orin_Delete_F  [](void *Device_Private_Class)\
                                            {\
                                                delete ((Main_B_Orin *)Device_Private_Class);\
                                                Device_Private_Class = nullptr; \
                                            }\


class Main_B_Orin : private Robot_Hardware
{
    public:
    Socket_CanFD SocketCanFD;
    C_GPIO  m_GPIO;
    Serial  Serials;

    int Get_Main_B_Orin_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
    int Main_B_Orin_Top_Frame_Analyze(volatile u8* Can_Frame);
};

int Send_F_Orin_CanFD(shared_ptr<Device_class> Device, u8* Data);
int Rec_F_Pr_Orin_CanFD(shared_ptr<Device_class> Device, u8* Data);

#endif //SDK_SWITCH_BOARD_ORIN_HPP
