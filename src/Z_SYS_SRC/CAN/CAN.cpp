#include "CAN.hpp"

using namespace std;


//合成CMD_Data结构体数据
CMD_Data* CMD_Data_F(u16 ID, u8 Flag, u16 CMD, u16 data0, u16 data1, u16 data2, u16 data3, u16 data4, CMD_Data* CMD_Datas)
{
    CMD_Datas->Can_Id = ID;
    CMD_Datas->Cmd    = CMD;
    CMD_Datas->Len    = CMD_Data_Struct_Len;
    CMD_Datas->Flag   = Flag;
    CMD_Datas->Data0  = data0;
    CMD_Datas->Data1  = data1;
    CMD_Datas->Data2  = data2;
    CMD_Datas->Data3  = data3;
    CMD_Datas->Data4  = data4;

    return CMD_Datas;
}

//废弃
int Main_CanData_2_CanFrame_Analyze(volatile u8 *Can_Data, int Len)
{
	volatile u8 Can_Frame[20];
	memcpy((void *)Get_Msg_Can_Data(Can_Frame), (void *)Can_Data, Len);

#ifdef Usr_CanFrame_Analyze
	Usr_CanFrame_Analyze(Can_Frame);
#endif

	return 0;
}

// CRC8查表法参数配置
#define CRC8_POLY        0x07  // 多项式（可根据需求修改）
#define CRC8_INIT        0x00  // 初始值
#define CRC8_XOR_OUT     0x00  // 结果异或值

// 预生成的CRC表（默认多项式0x07）
static uint8_t crc8_table[256];

// 生成CRC8查表
void generate_crc8_table(void) {
    for (uint16_t i = 0; i < 256; i++) {
        uint8_t crc = i;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLY;
            } else {
                crc <<= 1;
            }
        }
        crc8_table[i] = crc;
    }
}

// 计算CRC8（查表法）
uint8_t crc8_calculate(const uint8_t *data, uint32_t length) {
    uint8_t crc = CRC8_INIT;
    while (length--) {
        crc = crc8_table[crc ^ *data++];
    }
    return crc ^ CRC8_XOR_OUT;
}

// /*********************** 使用示例 **************************/
// // 在初始化时生成CRC表（或直接使用预计算好的表）
// generate_crc8_table();

// // 待校验数据
// uint8_t test_data[] = {0x01, 0x02, 0x03, 0x04};
// uint32_t data_len = sizeof(test_data);

// // 计算CRC
// uint8_t crc_result = crc8_calculate(test_data, data_len);








