#ifndef Auto_Set_Id_H_
#define Auto_Set_Id_H_
#include "HARDWARE_TOP.hpp"

#define Auto_Set_Id_Type "Auto_Set_Id"

#define Auto_Set_Id_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                                {\
                                    Auto_Set_Id *Auto_Set_Id_B = new Auto_Set_Id();\
                                    Device->Device_Private_Class = (void *)Auto_Set_Id_B;\
                                    if (Node != nullptr)\
                                        return Auto_Set_Id_B->Get_Data_From_Yaml_And_Init(Device, *Node);\
                                    else\
                                        return 0;\
                                }\

#define Auto_Set_Id_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                        {\
                                            return ((Auto_Set_Id *)Device_Private_Class)->Top_Frame_Analyze(Msg);\
                                        }\

#define Auto_Set_Id_Delete_F  [](void *Device_Private_Class)\
                                    {\
                                        delete ((Auto_Set_Id *)Device_Private_Class);\
                                    }\

class Auto_Set_Id : private Robot_Hardware
{
public:
    YAML::Node Device_List_Node;
    u16  Start_Id = 0;
    u16  End_Id = 0;
    int deviceCount;
    shared_ptr<Device_Struct> Device_this;

    int Get_Data_From_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
    int Top_Frame_Analyze(volatile u8 *Can_Frame);
};

#endif
