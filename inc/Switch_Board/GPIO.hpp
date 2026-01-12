//
// Created by lwz on 2025/12/16.
//

#ifndef RHS_GPIO_HPP
#define RHS_GPIO_HPP

#include "HARDWARE_TOP.hpp"
#include "mcu/stm32h743/stm32h7xx_hal_gpio.h"

/*
 * Head_Len(6) + CRC8(1) + Cmd(1) + GPIOx(4) + Init(20) = 32
 * ARM GCC 在 C++ 模式下, 会对包含 [外设指针] + [外设配置结构体] 的自定义结构体强制填充到 8 字节整数倍, 故补了 4 字节
 */
#define UDP_PACK_LEN    40

typedef struct
{
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

    // DATA
    u8 CRC8;
    u8 Cmd;

    GPIO_InitTypeDef Init;  // 20 字节
    u8 Reserved[4];
    GPIO_TypeDef *GPIOx;
}GPIO_Data;


class C_GPIO : private Robot_Hardware
{
public:
    static int Get_Yaml_And_Init(const shared_ptr<Device_Struct>& Device, const YAML::Node& One_Node);

    void GPIOx_Init(const shared_ptr<Device_Struct>& Device, GPIO_TypeDef *GPIOx, const GPIO_InitTypeDef *GPIO_Init_Struct);
};

#endif //RHS_GPIO_HPP