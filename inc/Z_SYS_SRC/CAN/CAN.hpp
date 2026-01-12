#ifndef CAN_DATA_
#define CAN_DATA_
#include "syst.hpp"

#define Main_CMD_IO_Mod_ETH_To_CAN 0  //网络到CAN转发
#define Main_CMD_IO_Mod_ETH_OR_CAN 1  //网络 OR CAN
#define Main_CMD_IO_Mod_CAN_ONLY 2    //只有CAN

#define ETH_Device_CAN_Broadcast_ID 1023 //网络设备广播ID
#define CAN_Device_CAN_Broadcast_ID 1022 //CAN设备广播ID
#define ETH_AND_CAN_Device_CAN_Broadcast_ID 1021 //CAN设备和网路设备同步广播ID

#define Head_Len 6 //帧头长度
#define CMD_Data_Struct_Len 18 //CMD_Data_Struc 长度

#define Get_Msg_Can_Id(A) ({ ((u16 *)A)[0]; }) //获取数据帧的ID
#define Get_Msg_CMD(A) ({ ((u8 *)A)[7]; })     //获取数据帧的CMD命令
#define Get_Msg_Can_Data(A) ({ ((u8 *)A) + 6; }) //获取数据帧的 Data
#define Get_Msg_Can_Data_Len(A) ({ ((u8 *)A)[2] - Head_Len; }) //获取数据帧的 Can_Data_Len
#define Get_Msg_Data_Len(A) ({ ((u16 *)A)[1]; }) //获取数据帧的 总长度
#define Get_Msg_Data_Flag(A) ({ ((u16 *)A)[2]; }) //获取数据帧的 Flag

#define CRC_ADDER 6

#define CMD_READ 0 //读取参数
#define CMD_WRITE 1 //写入参数

enum Main_CMD_Data_Enum
{
	Main_CMD_None = 0,

	// Falsh+RAM
	Main_CMD_Get_Config_Data = 1,
	Main_CMD_Save_Config_To_Flash,
	Main_CMD_Get_Info,

	// ID,IP只能广播设置 Mod可以既广播设置 又 Can通信设置
	Main_CMD_IO_Mod,
	Main_CMD_Can_ID,
	Main_CMD_ETH_IP,
	Main_CMD_PC_IP,
	Main_CMD_Port,
	Main_CMD_UDP_Send_Speed,

	Main_CMD_REBOOT,

	Main_CMD_GO_TO_BOOT_MOD,
	Main_CMD_IS_BOOT_MOD,
	Main_CMD_START_OTA,
	Main_CMD_OTA_DATA,
	Main_CMD_OTA_END,
	Main_CMD_BOOT_MENU,
	Main_CMD_BOOT,
	
	Main_SN_DATA, //用于定义产品线的SN码 来判断最终产品类型
	Main_CMD_EX,
};

enum Main_CMD_EX_Enum
{
	Debug_Printf_Data = 0,
	Debug_Printf_EN,
	PrintAllTaskStack_EN,
	Main_CMD_ID_For_UID, //收到广播上报UID CMD后 使用指定CAN_ID 上报UID // 收到广播设置CAN_ID CMD 后 对比接收的UID设置CAN_ID
};

enum Head_Flag_Enum
{
	Head_Flag_CH0 = 0,
	Head_Flag_Switch_CH1,
	Head_Flag_Switch_CH2,
	Head_Flag_Switch_CH3,
	Head_Flag_Switch_CH4,
	Head_Flag_Switch_CH5,
	Head_Flag_Switch_CH6,
	Head_Flag_Switch_CH7,
	Head_Flag_Switch_CH8,
	Head_Flag_Switch_CH9,
	Head_Flag_Switch_CH10,
	Head_Flag_Switch_CH11,
	Head_Flag_Switch_CH12,
	Head_Flag_Switch_CH13,
	Head_Flag_Switch_CH14,
	Head_Flag_Switch_CH15,
	Head_Flag_Switch_CH16,
	Head_Flag_Switch_CH17,
	Head_Flag_Switch_CH18,
	Head_Flag_Switch_CH19,
	Head_Flag_Switch_CH20,
	Head_Flag_Switch_CH_ALL = 255,
};

typedef struct
{ // CMD数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	u16 Data0;
	u16 Data1;
	u16 Data2;
	u16 Data3;
	u16 Data4;
} CMD_Data;

typedef struct
{
	u16 Device_1;
	u16 Device_2;
	u16 Device_3;
	u16 version_1;
	u16 version_2;
	u32 Main_IO_Mod;
	u32 Main_Can_ID;
	u32 Main_ETH_IP;
	u32 Main_PC_IP;
	u32 Main_Port;
	u32 Main_UDP_Send_Speed;
	u32 Main_Broadcast_IP;
	u32 BOOT_MENU;
	int OTA_BAG_LEN;

	bool FLAG_UDP_RUN;
	bool FLAG_Main_CMD_Save_Config_To_Flash;//Save_Config_To_Flash反馈标志位
	bool FLAG_Main_CMD_IS_BOOT_MOD;//IS_BOOT_MOD反馈标志位
	bool FLAG_Main_CMD_START_OTA;//START_OTA反馈标志位
	bool FLAG_Main_CMD_OTA_END; //OTA_END反馈标志位
	bool Main_CMD_OTA_DATA_FB_Flag; //OTA_DATA收到反馈标志位
} Main_Data;

// #define Usr_CanFrame_Analyze(Can_Frame)
#define CAN_SendMsg(ID, CAN_Data, Len) 0  //废弃

//合成CMD_Data结构体数据
CMD_Data* CMD_Data_F(u16 ID, u8 Flag, u16 CMD, u16 data0, u16 data1, u16 data2, u16 data3, u16 data4, CMD_Data* CMD_Datas);
// 生成CRC8查表
void generate_crc8_table(void);
// 计算CRC8（查表法）
uint8_t crc8_calculate(const uint8_t *data, uint32_t length);

#endif

