#ifndef Serial_HPP_
#define Serial_HPP_

#include "HARDWARE_TOP.hpp"

#define Serial_Open     1
#define Serial_Close    2

#define S_TRUE  1
#define S_FALSE 0

class Serial : private Robot_Hardware
{
    public:
        int Get_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
        int Serial_Task_Switch(shared_ptr<Device_Struct> Device_P, u16 EN);
        int Set_ID(shared_ptr<Device_Struct> Device_P, u16 ID);
        int Serial_Set_New_Baud_Rate(shared_ptr<Device_Struct> Device_P, u32 Baud_Rate);
        int Serial_Call_Back(volatile u8 *Can_Frame);

        int Is_EN(shared_ptr<Device_Struct> Device_P, u16 en);
        int Set_Instance(shared_ptr<Device_Struct> Device_P, u32 Instance_s);
        int Set_BaudRate(shared_ptr<Device_Struct> Device_P, u32 BaudRate_s);
        int Set_WordLength(shared_ptr<Device_Struct> Device_P, u32 WordLength_s);
        int Set_Stm_Tx_Rx_GPIO(shared_ptr<Device_Struct> Device_P, u32 Stm_TxT_GPIO, u32 Stm_Rx_GPIO);
        int Set_Stm_Pin(shared_ptr<Device_Struct> Device_P, u16 Stm_Pin_tx, u16 Stm_Pin_rx);
        int Set_Other_Config(shared_ptr<Device_Struct> Device_P, u32 dma_Tx_Request, u32 dma_Rx_Request, u32 GPIO_Alternate, u16 Serial_IQR);
        int Set_Serial_menu(shared_ptr<Device_Struct> Device_P, u16 menu);
        int Set_Serial_ID(shared_ptr<Device_Struct> Device_P, u16 ID);
        int Set_Serial_DMA_Stream(shared_ptr<Device_Struct> Device_P, u32 dma_stream_tx, u32 dma_stream_rx);
        
};

enum Serial_CMD_Data_Enum{
    Serial_Start_Task = 1,
    Serial_Stop_Task,
    Serial_Set_Baud_Rate,
    Serial_Set_ID
};

enum Serial_Config_Data{
    En = 1,
    Instance,
    BaudRate,
    WordLength,
    Stm_Tx_Rx_GPIO,
    Stm_Pin,
    Other_Config,
    Serial_menu,
    Serial_ID,
    Serial_DMA_Stream,
};

#endif