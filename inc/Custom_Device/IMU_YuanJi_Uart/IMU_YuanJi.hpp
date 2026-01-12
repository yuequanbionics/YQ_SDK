#ifndef IMU_YUANJI_H_
#define IMU_YUANJI_H_
//#include "IMU_YuanJi_Uart.hpp"
#include "HARDWARE_TOP.hpp"
#include "crc32_tab.h"

#define IMU_YuanJi_Init  [](shared_ptr<Device_Struct> Device, YAML::Node *Node) -> int\
                            {\
                                IMU_YuanJi *One_IMU_YuanJi = new IMU_YuanJi();\
                                Device->Device_Private_Class = (void *)One_IMU_YuanJi;\
                                if (Node != nullptr)\
                                    return One_IMU_YuanJi->IMU_YuanJi_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define IMU_YuanJi_CallBack_F    [](void *Device_Private_Class, volatile u8 *Msg) -> int\
                                    {\
                                        return ((IMU_YuanJi *)Device_Private_Class)->IMU_YuanJi_Frame_Analyze(Msg);\
                                    }\

#define IMU_YuanJi_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((IMU_YuanJi *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\


#pragma pack(push, 1)

typedef struct IMU_cmd_data
{ 
    uint8_t fame_header1;     //帧头
    uint8_t fame_header2;
    uint16_t id;              //传感器ID
    uint16_t length;          //数据长度

    float param1;             //参数1
    float param2;             //参数2
    uint32_t param3;          //参数3
    uint32_t param4;          //参数4
    int32_t param5;           //参数5
    int32_t param6;           //参数6

    uint32_t crc32;           //CRC32校验码

} IMU_cmd_data;

typedef struct IMU_out_data
{ 
    uint8_t fame_header1;    //帧头
    uint8_t fame_header2;
    uint16_t id;             //传感器ID
    uint16_t length;         //数据长度

    uint32_t timer;          //时间戳
    float pitch;             //俯仰角
    float roll;              //横滚角
    float yaw;               //航向角
    float acc_x;             //X轴加速度
    float acc_y;             //Y轴加速度
    float acc_z;             //Z轴加速度
    float gyro_x;            //X轴角速度
    float gyro_y;            //Y轴角速度
    float gyro_z;            //Z轴角速度
    float temp;              //IMU芯片温度

    uint32_t crc32;          //CRC32校验码


} IMU_out_data;

typedef struct Serial_Data
{ 
    // Head
    u16 Can_Id;
    u16 Len;
    u16 Flag;

   // DATA
u8 data[1024];

} Serial_Data;

#pragma pack(pop) // 恢复默认的对齐设置

typedef struct IMU_frame_data
{
    uint32_t time_us;  // 时间戳（微秒）
    float pitch;       // 俯仰角
    float roll;        // 横滚角
    float yaw;         // 航向角
    float acc_x;       // X轴加速度
    float acc_y;       // Y轴加速度
    float acc_z;       // Z轴加速度
    float gyro_x;      // X轴角速度
    float gyro_y;      // Y轴角速度
    float gyro_z;      // Z轴角速度

}IMU_frame_data;

typedef struct quaternion_data {
    double w, x, y, z;
} quaternion_data;

class IMU_YuanJi : private Robot_Hardware
{
public:
	int IMU_YuanJi_Data_From_Yaml_And_Init(shared_ptr<Device_Struct> Device, YAML::Node One_Node);
	int IMU_YuanJi_Frame_Analyze(volatile u8 *Can_Frame);

    int Serial_Send(shared_ptr<Device_Struct> Device_P, u8 *Data, u32 len);

	int (*IMU_YuanJi_Msg_CallBack)(volatile u8 *);

    void Send_CMD_LONG(shared_ptr<Device_Struct> Device_P, u16 id, float cm1, float cm2, u32 cm3, u32 cm4, int32_t cm5, int32_t cm6);

    void Start_AHRS_Mod_And_Init(shared_ptr<Device_Struct> Device_P);

    IMU_frame_data Get_IMU_Frame_Data();

    quaternion_data Get_IMU_Quaternion_Data();

    

    //void Stop_AHRS_Mod(shared_ptr<Device_Struct> Device_P);


private:
    IMU_cmd_data CMD_Data_Out;
    IMU_frame_data IMU_Frame_Data;
};

#endif