#ifndef _PHU_MOTOR_TOP_H_
#define _PHU_MOTOR_TOP_H_

#include "syst.hpp"
#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"

#define Phu_Custom_Type "Phu_Custom_Motor_TOP"

#define Eyou_Motor_Data_Len         Head_Len + 8
#define Can_Id_Send_Offset          0x600
#define Can_Id_Back_Offset          0x580
#define R_T_A                       (float)57.29577

#define SDO_WR_Delayus              100
#define PDO_WR_Delayus              100

#define Phu_Custom_Motor_Device_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Phu_Motor *One_Phu_Motor = new Phu_Motor();\
                                Device->Device_Private_Class = (void *)One_Phu_Motor;\
                                if (Node != nullptr)\
                                    return One_Phu_Motor->Get_Phu_Custom_Motor_Device_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Phu_Custom_Motor_Device_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int \
                                    {\
                                        return ((Phu_Motor *)Device->Device_Private_Class)->Phu_Custom_Motor_Top_Frame_Analyze(Device, Msg);\
                                    }\

#define Phu_Custom_Motor_Device_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Phu_Motor *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\

enum Phu_CS_Data{
    Write_4_Byte_Data = 0x23,
    Write_3_Byte_Data = 0x27,
    Write_2_Byte_Data = 0x2B,
    Write_1_Byte_Data = 0x2F,

    Read_Data         = 0x40,
};

enum Phu_Motor_Status{
    Phu_Motor_En            = 0x01,
    Phu_Motor_Disen         = 0x02,
    Phu_Motor_Release_Break = 0x03,
    Phu_Motor_Close_Break   = 0x04,
};

enum Phu_CMD{
    /* 抱闸输出控制 */
    Break_Modectl = 0x2110,
    /* 抱闸强制输出控制 */
    Break_Ctl     = 0x2111,
    /* 驱动器功率模块温度 */
    Read_temp     = 0x277A,
    /* 错误代码 */
    Err_code      = 0x603F,
    /* 控制字 */
    Ctl_word      = 0x6040,
    /* 状态字 */
    rCtlmode      = 0x6041,
    /* 运行模式 */
    Ctlmode       = 0x6060,
    /* 实际位置值 */
    Read_Pos      = 0x6064,
    /* 实际速度值 */
    Read_Speed    = 0x606C,
    /* 电机额定力矩 */
    Rated_Torque  = 0x6076,
    /* 实际力矩值 */
    Actual_Torque = 0x6077,
    /* 目标位置 */
    Goal_Pos      = 0x607A,
    /* 插值周期 */
    Time_Period   = 0x60C2,
    /* 回零方式 */
    Home_Zero     = 0x6098,

    /* 同步报文 */
    SYNC_message                = 0x1005,
    /* 同步循环周期 */
    Communication_cycle_period  = 0x1006,
    /* 保存参数 */
    Save_parameters             = 0x1010,

    /* RxPDO 映射数量 */
    RxPDO_N0                    =   0x1400,
    RxPDO_N1                    =   0x1401,
    RxPDO_N2                    =   0x1402,
    RxPDO_N3                    =   0x1403,
    Mapping_Rxpdo_entry_N0      =   0x1600,
    Mapping_Rxpdo_entry_N1      =   0x1601,
    Mapping_Rxpdo_entry_N2      =   0x1602,
    Mapping_Rxpdo_entry_N3      =   0x1603,

    /* TxPDO 映射数量 */
    TxPDO_N0                    =   0x1800,
    TxPDO_N1                    =   0x1801,
    TxPDO_N2                    =   0x1802,
    TxPDO_N3                    =   0x1803,
    Mapping_Txpdo_entry_N0      =   0x1A00,
    Mapping_Txpdo_entry_N1      =   0x1A01,
    Mapping_Txpdo_entry_N2      =   0x1A02,
    Mapping_Txpdo_entry_N3      =   0x1A03,

    Save_Para                   =   0x2130,
};

typedef struct Phu_Motor_SDO
{
    u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
    u8 Command_code;
    u8 Object_index[2];
    u8 Object_sub_index;
    u8 data[4];
}Phu_Motor_SDO;

class Phu_Motor : private Robot_Hardware
{
public:

    float   Phu_Pos;
    float   Phu_Speed;

    int     Phu_PosY;

    char    Phu_temp;

    u16     Phu_ctlword;

    int     start_lock      = 0;

    bool    wait_zero_lock  = false;

    /* 减速比 */
    int Motor_Reduction_Ratio = 0;
    /* 编码器输出分辨率 */
    float Encoder_Res = 0.0f;

    Phu_Motor_SDO Phu_Motor_SDO_Read;

    int Get_Phu_Custom_Motor_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
    int Phu_Custom_Motor_Top_Frame_Analyze(shared_ptr<Device_class> Device, volatile u8 *Can_Frame);


    /**
     * @brief 获取电机 位置 速度 温度 数据
    */
    int Get_Motor_FB_Data(const shared_ptr<Device_class>& Device_P, float *P, float *V, float *F, float temp[2], u16 *error);


    /**
     * @brief 发送位置
     *  @param Rad          目标位置 单位-角度 
     *  @param Speed_Rad_S  NULL
     *  @param Force_N      NULL
     *  @param P_N_Rad      NULL
     *  @param D_N_Rad_s    NULL
    */
    int Send_MIT_PD_Control_Data(shared_ptr<Device_class> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);


    /**
     * @brief PDO同步指令发送线程函数
    */
    int PDO_Sync_Send(shared_ptr<Device_class> Device_P);


    /**
     * @brief   读取错误码
    */
    int Phu_ReadErr(shared_ptr<Device_class> Device_P);


    /**
     * @brief   PHU电机状态控制
     *  @param  status  参数见：
     *                      enum Phu_Motor_Status{
     *                           Phu_Motor_En            = 0x01,
     *                           Phu_Motor_Disen         = 0x02,
     *                           Phu_Motor_Release_Break = 0x03,
     *                           Phu_Motor_Close_Break   = 0x04,
     *                       };
    */
    int Phu_Motor_Status_Ctrl(shared_ptr<Device_class> Device_P, int status);


    /**
     * @brief PHU电机将当前位置设置为零点
    */
    int Phu_Set_Zero(shared_ptr<Device_class> Device_P, bool save);


    /**
     * @brief PHU电机
    */
    int Phu_Set_Angel(shared_ptr<Device_class> Device_P, float target_angel);
    
private:
    /**
     * @brief   SDO模式下的数据命令读写
     *  @param  Object_index     要读写的地址
     *  @param  Object_sub_index 要读写地址的子索引
     *  @param  data             写入的数据
     *  @param  data_len         写入数据的长度(data的长度) 
     *                           写入0为读取命令
    */
    int SDO_WR(shared_ptr<Device_class> Device_P, u16 Object_index, u8 Object_sub_index, int data, u8 data_len);


    /**
     * @brief   用于PDO下的数据发送
     *  @param  canid   期望地址
     *  @param  data    数据指针
     *  @param  len     数据长度
    */
    int PDO_WR(shared_ptr<Device_class> Device_P, u16 canid, u8 *data, int len);


    /**
     * @brief   SDO设置CSP模式 （禁止使用 无法使用）
    */
    int SDO_Set_CSPmode(shared_ptr<Device_class> Device_P);


    /**
     * @brief   PHU SDO 读取 位置 速度 温度数据
    */
    int SDO_Read_PST(shared_ptr<Device_class> Device_P);


    /**
     * @brief   SDO设置PDO 设置为CSP模式
    */
    int Set_CSP_PDO(shared_ptr<Device_class> Device_P);
};


#endif
