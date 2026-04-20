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
	Main_CMD_For_SN = 0,
	Main_CMD_Heart_Beat,
	Debug_Printf_Data,
	Debug_Printf_EN,
	PrintAllTaskStack_EN,
	IO_MAP,
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
	Head_Flag_Switch_CH21,
	Head_Flag_Switch_CH22,
	Head_Flag_Switch_CH23,
	Head_Flag_Switch_CH24,
	Head_Flag_Switch_CH25,
	Head_Flag_Switch_CH26,
	Head_Flag_Switch_CH27,
	Head_Flag_Switch_CH28,
	Head_Flag_Switch_CH29,
	Head_Flag_Switch_CH30,
	Head_Flag_Switch_CH31,
	Head_Flag_Switch_CH32,
	Head_Flag_Switch_CH33,
	Head_Flag_Switch_CH34,
	Head_Flag_Switch_CH35,
	Head_Flag_Switch_CH36,
	Head_Flag_Switch_CH37,
	Head_Flag_Switch_CH38,
	Head_Flag_Switch_CH39,
	Head_Flag_Switch_CH40,
	Head_Flag_Switch_CH41,
	Head_Flag_Switch_CH42,
	Head_Flag_Switch_CH43,
	Head_Flag_Switch_CH44,
	Head_Flag_Switch_CH45,
	Head_Flag_Switch_CH46,
	Head_Flag_Switch_CH47,
	Head_Flag_Switch_CH48,
	Head_Flag_Switch_CH49,
	Head_Flag_Switch_CH50,
	Head_Flag_Switch_CH51,
	Head_Flag_Switch_CH52,
	Head_Flag_Switch_CH53,
	Head_Flag_Switch_CH54,
	Head_Flag_Switch_CH55,
	Head_Flag_Switch_CH56,
	Head_Flag_Switch_CH57,
	Head_Flag_Switch_CH58,
	Head_Flag_Switch_CH59,
	Head_Flag_Switch_CH60,
	Head_Flag_Switch_CH61,
	Head_Flag_Switch_CH62,
	Head_Flag_Switch_CH63,
	Head_Flag_Switch_CH64,
	Head_Flag_Switch_CH65,
	Head_Flag_Switch_CH66,
	Head_Flag_Switch_CH67,
	Head_Flag_Switch_CH68,
	Head_Flag_Switch_CH69,
	Head_Flag_Switch_CH70,
	Head_Flag_Switch_CH71,
	Head_Flag_Switch_CH72,
	Head_Flag_Switch_CH73,
	Head_Flag_Switch_CH74,
	Head_Flag_Switch_CH75,
	Head_Flag_Switch_CH76,
	Head_Flag_Switch_CH77,
	Head_Flag_Switch_CH78,
	Head_Flag_Switch_CH79,
	Head_Flag_Switch_CH80,
	Head_Flag_Switch_CH81,
	Head_Flag_Switch_CH82,
	Head_Flag_Switch_CH83,
	Head_Flag_Switch_CH84,
	Head_Flag_Switch_CH85,
	Head_Flag_Switch_CH86,
	Head_Flag_Switch_CH87,
	Head_Flag_Switch_CH88,
	Head_Flag_Switch_CH89,
	Head_Flag_Switch_CH90,
	Head_Flag_Switch_CH91,
	Head_Flag_Switch_CH92,
	Head_Flag_Switch_CH93,
	Head_Flag_Switch_CH94,
	Head_Flag_Switch_CH95,
	Head_Flag_Switch_CH96,
	Head_Flag_Switch_CH97,
	Head_Flag_Switch_CH98,
	Head_Flag_Switch_CH99,
	Head_Flag_Switch_CH100,
	Head_Flag_Switch_CH101,
	Head_Flag_Switch_CH102,
	Head_Flag_Switch_CH103,
	Head_Flag_Switch_CH104,
	Head_Flag_Switch_CH105,
	Head_Flag_Switch_CH106,
	Head_Flag_Switch_CH107,
	Head_Flag_Switch_CH108,
	Head_Flag_Switch_CH109,
	Head_Flag_Switch_CH110,
	Head_Flag_Switch_CH111,
	Head_Flag_Switch_CH112,
	Head_Flag_Switch_CH113,
	Head_Flag_Switch_CH114,
	Head_Flag_Switch_CH115,
	Head_Flag_Switch_CH116,
	Head_Flag_Switch_CH117,
	Head_Flag_Switch_CH118,
	Head_Flag_Switch_CH119,
	Head_Flag_Switch_CH120,
	Head_Flag_Switch_CH121,
	Head_Flag_Switch_CH122,
	Head_Flag_Switch_CH123,
	Head_Flag_Switch_CH124,
	Head_Flag_Switch_CH125,
	Head_Flag_Switch_CH126,
	Head_Flag_Switch_CH127,
	Head_Flag_Switch_CH128,
	Head_Flag_Switch_CH129,
	Head_Flag_Switch_CH130,
	Head_Flag_Switch_CH131,
	Head_Flag_Switch_CH132,
	Head_Flag_Switch_CH133,
	Head_Flag_Switch_CH134,
	Head_Flag_Switch_CH135,
	Head_Flag_Switch_CH136,
	Head_Flag_Switch_CH137,
	Head_Flag_Switch_CH138,
	Head_Flag_Switch_CH139,
	Head_Flag_Switch_CH140,
	Head_Flag_Switch_CH141,
	Head_Flag_Switch_CH142,
	Head_Flag_Switch_CH143,
	Head_Flag_Switch_CH144,
	Head_Flag_Switch_CH145,
	Head_Flag_Switch_CH146,
	Head_Flag_Switch_CH147,
	Head_Flag_Switch_CH148,
	Head_Flag_Switch_CH149,
	Head_Flag_Switch_CH150,
	Head_Flag_Switch_CH151,
	Head_Flag_Switch_CH152,
	Head_Flag_Switch_CH153,
	Head_Flag_Switch_CH154,
	Head_Flag_Switch_CH155,
	Head_Flag_Switch_CH156,
	Head_Flag_Switch_CH157,
	Head_Flag_Switch_CH158,
	Head_Flag_Switch_CH159,
	Head_Flag_Switch_CH160,
	Head_Flag_Switch_CH161,
	Head_Flag_Switch_CH162,
	Head_Flag_Switch_CH163,
	Head_Flag_Switch_CH164,
	Head_Flag_Switch_CH165,
	Head_Flag_Switch_CH166,
	Head_Flag_Switch_CH167,
	Head_Flag_Switch_CH168,
	Head_Flag_Switch_CH169,
	Head_Flag_Switch_CH170,
	Head_Flag_Switch_CH171,
	Head_Flag_Switch_CH172,
	Head_Flag_Switch_CH173,
	Head_Flag_Switch_CH174,
	Head_Flag_Switch_CH175,
	Head_Flag_Switch_CH176,
	Head_Flag_Switch_CH177,
	Head_Flag_Switch_CH178,
	Head_Flag_Switch_CH179,
	Head_Flag_Switch_CH180,
	Head_Flag_Switch_CH181,
	Head_Flag_Switch_CH182,
	Head_Flag_Switch_CH183,
	Head_Flag_Switch_CH184,
	Head_Flag_Switch_CH185,
	Head_Flag_Switch_CH186,
	Head_Flag_Switch_CH187,
	Head_Flag_Switch_CH188,
	Head_Flag_Switch_CH189,
	Head_Flag_Switch_CH190,
	Head_Flag_Switch_CH191,
	Head_Flag_Switch_CH192,
	Head_Flag_Switch_CH193,
	Head_Flag_Switch_CH194,
	Head_Flag_Switch_CH195,
	Head_Flag_Switch_CH196,
	Head_Flag_Switch_CH197,
	Head_Flag_Switch_CH198,
	Head_Flag_Switch_CH199,
	Head_Flag_Switch_CH200,
	Head_Flag_Switch_CH201,
	Head_Flag_Switch_CH202,
	Head_Flag_Switch_CH203,
	Head_Flag_Switch_CH204,
	Head_Flag_Switch_CH205,
	Head_Flag_Switch_CH206,
	Head_Flag_Switch_CH207,
	Head_Flag_Switch_CH208,
	Head_Flag_Switch_CH209,
	Head_Flag_Switch_CH210,
	Head_Flag_Switch_CH211,
	Head_Flag_Switch_CH212,
	Head_Flag_Switch_CH213,
	Head_Flag_Switch_CH214,
	Head_Flag_Switch_CH215,
	Head_Flag_Switch_CH216,
	Head_Flag_Switch_CH217,
	Head_Flag_Switch_CH218,
	Head_Flag_Switch_CH219,
	Head_Flag_Switch_CH220,
	Head_Flag_Switch_CH221,
	Head_Flag_Switch_CH222,
	Head_Flag_Switch_CH223,
	Head_Flag_Switch_CH224,
	Head_Flag_Switch_CH225,
	Head_Flag_Switch_CH226,
	Head_Flag_Switch_CH227,
	Head_Flag_Switch_CH228,
	Head_Flag_Switch_CH229,
	Head_Flag_Switch_CH230,
	Head_Flag_Switch_CH231,
	Head_Flag_Switch_CH232,
	Head_Flag_Switch_CH233,
	Head_Flag_Switch_CH234,
	Head_Flag_Switch_CH235,
	Head_Flag_Switch_CH236,
	Head_Flag_Switch_CH237,
	Head_Flag_Switch_CH238,
	Head_Flag_Switch_CH239,
	Head_Flag_Switch_CH240,
	Head_Flag_Switch_CH241,
	Head_Flag_Switch_CH242,
	Head_Flag_Switch_CH243,
	Head_Flag_Switch_CH244,
	Head_Flag_Switch_CH245,
	Head_Flag_Switch_CH246,
	Head_Flag_Switch_CH247,
	Head_Flag_Switch_CH248,
	Head_Flag_Switch_CH249,
	Head_Flag_Switch_CH250,
	Head_Flag_Switch_CH251,
	Head_Flag_Switch_CH252,
	Head_Flag_Switch_CH253,
	Head_Flag_Switch_CH254,
	Head_Flag_Switch_CH_ALL = 255,
};

typedef struct
{
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	u16 EX_CMD;
	u16 SN_CMD;
	u16 Speed;
	u16 Id;
	u32 SN1;
	u32 SN2;
	u32 PC_IP;
	u32 MCU_IP;
	u32 POART;
} Main_CMD_For_SN_Data;

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
	int Heart_Beat_NUM;
	long SN;

	bool FLAG_UDP_RUN;
	bool Heart_Beat_RUN;
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

