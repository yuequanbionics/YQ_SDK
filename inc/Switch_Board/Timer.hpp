#ifndef _Timer_HPP_
#define _Timer_HPP_

#include "HARDWARE_TOP.hpp"

enum Timer_Son_Cmd_Data_Enum{
    Timer_Instance = 1,
    Timer_Prescaler,
    Timer_CounterMode,
    Timer_Period,
    Timer_En,
    Timer_Ch,
    Timer_Ch_OCMode,
    Timer_Ch_Pulse,
    Timer_Ch_OCPolarity,
    Timer_Ch_OCFastMode,
    Timer_Ch_GPIO,
    Timer_Ch_Pin, 
    Timer_Ch_GPIO_Alternate,
    Timer_Ch_DMA_Instance,
    Timer_Ch_DMA_Request,
    Timer_Ch_DMA_Direction,
    Timer_Ch_DMA_PeriphInc,
    Timer_Ch_DMA_MemInc,
    Timer_Ch_DMA_PeriphDataAlignment,
    Timer_Ch_DMA_MemDataAlignment,
    Timer_Ch_DMA_Mode,
    Timer_Ch_DMA_Priority,
    Timer_Ch_DMA_FIFOMode,
    Timer_Ch_DMA_Handle_index,
    Timer_Ch_OutMode,
    Timer_Set_Duty_Ratio,
    Timer_Set_Frequency,
    Timer_DMA_Send_Data,
    WS2812B_Change_Color,
};

class Timer : private Robot_Hardware
{
    public:
        int Get_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
        int Timer_Call_Back(volatile u8 *Can_Frame);

        int Send_Timer_Config(const shared_ptr<Device_Struct>& Device_P, u16 CMD, u16 data1, u16 data2, u32 DATA);
        int Timer_Set_Duty_Ratio_F(const shared_ptr<Device_Struct>& Device_P, uint16_t Timer_Index, uint16_t Timer_ChX, uint32_t Duty_Ratio_Data);
        int Timer_Set_Frequency_F(const shared_ptr<Device_Struct>& Device_P, uint16_t Timer_Index, uint16_t Timer_ChX, uint32_t Frequency);
        int Timer_DMA_Send(const shared_ptr<Device_Struct>& Device_P, uint8_t Timer_Index, uint8_t Timer_ChX, uint32_t DATA, uint16_t DATA_Size);
        int Set_LED_Color(const shared_ptr<Device_Struct>& Device_P, uint8_t htim_index, uint8_t ch_index, uint16_t led_num, uint8_t C_R, uint8_t C_G, uint8_t C_B);

};

#endif
