//
// Created by lwz on 2025/10/24.
//

#ifndef SDK_FD_CAN_HPP
#define SDK_FD_CAN_HPP
#include "HARDWARE_TOP.hpp"


/* 向主控发送 Can 配置命令, 用于确定要配置的参数 */
enum Can_CMD_Data_Enum
{
    Can_Set_Instance = 0,
    Can_Set_EN,
    Can_Set_FrameFormat,
    Can_Set_Mode,
    Can_Set_AutoRetransmission,
    Can_Set_TransmitPause,
    Can_Set_ProtocolException,
    Can_Set_Nominal_Prescaler,
    Can_Set_Nominal_SyncJumpWidth,
    Can_Set_Nominal_TimeSeg1,
    Can_Set_Nominal_TimeSeg2,
    Can_Set_Data_Prescaler,
    Can_Set_Data_SyncJumpWidth,
    Can_Set_Data_TimeSeg1,
    Can_Set_Data_TimeSeg2,
    Can_Set_MessageRAMOffset,
    Can_Set_StdFiltersNbr,
    Can_Set_ExtFiltersNbr,
    Can_Set_RxFifo0ElmtsNbr,
    Can_Set_RxFifo0ElmtSize,
    Can_Set_RxFifo1ElmtsNbr,
    Can_Set_RxFifo1ElmtSize,
    Can_Set_RxBuffersNbr,
    Can_Set_RxBufferSize,
    Can_Set_TxEventsNbr,
    Can_Set_TxBuffersNbr,
    Can_Set_TxFifoQueueElmtsNbr,
    Can_Set_TxFifoQueueMode,
    Can_Set_TxElmtSize,
    Can_Set_Complete
};


class FDCan : private Robot_Hardware
{
    public:
        int Get_Yaml_And_Init(const shared_ptr<Device_Struct>& Device, const YAML::Node& One_Node);
        int Set_Can_Param(const shared_ptr<Device_Struct>& Device, Can_CMD_Data_Enum cmd, u16 Value);
};




#endif //SDK_FD_CAN_HPP
