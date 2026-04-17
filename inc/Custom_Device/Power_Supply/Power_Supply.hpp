#ifndef MOTOR_TOP_H_
#define MOTOR_TOP_H_
#include "syst.hpp"
#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"

#define Power_Supply_Type "Power_Custom_Supply"

#define Power_Supply_Custom_Init  [](shared_ptr<Device_class> Device, YAML::Node *Node) -> int\
                            {\
                                Power_Supply *One_Power_Supply = new Power_Supply();\
                                Device->Device_Private_Class = (void *)One_Power_Supply;\
                                if (Node != nullptr)\
                                    return One_Power_Supply->Get_Power_Supply_Custom_Data_From_Yaml_And_Init(Device, *Node);\
                                else\
                                    return 0;\
                            }\

#define Power_Supply_Custom_CallBack_F    [](shared_ptr<Device_class> Device, u8 *Msg) -> int \
                                    {\
                                        return ((Power_Supply *)Device->Device_Private_Class)->Power_Supply_Custom_Top_Frame_Analyze(Device, Msg);\
                                    }\

#define Power_Supply_Custom_Delete_F  [](void *Device_Private_Class)\
                                {\
                                    delete ((Power_Supply *)Device_Private_Class);\
                                    Device_Private_Class = nullptr;\
                                }\


#define S_EN        1
#define S_DISEN     0


/**
 * @brief 用户命令
*/
enum User_Cmd{
    Device_Id = 1,          /* 返回设备的ID字符串等信息 */
    Device_Rst,             /* 设备恢复出场设置 */
    Device_Now_Data,        /* 查询设备通道输出电压、电流、运行状态 */
    Device_Out_En,          /* 设备输出使能控制 */
    Device_Out_DISEn,       /* 设备输出失能控制 */
    Device_Out_OCP_LIM,     /* 设备输出过流保护值的设置 */
    Device_Out_VOL,         /* 设备设置输出电压 */
    Device_Out_VOL_LIM,     /* 设备输出过压保护值的设置 */
    Device_Get_Out_VOL_LIM, /* 获取设备输出电压的Limit值 */
    Device_Out_CURR,        /* 设备设置输出电流 */
    Device_Out_CURR_LIM,    /* 设备输出电流Limit的值 */
    Device_Get_Out_CURR_LIM,/* 获取设备输出电流Limit的值 */
    Device_Get_VOL,         /* 获取设备目前设置电压的值 */
    Device_Get_CURR,        /* 获取设备目前设置电流的值 */
};


typedef struct serial_transmission{
    float now_vol_data;
    float set_vol_data;
    float vol_limit_data;

    float now_curr_data;
    float set_curr_data;
    float curr_limit_data;
    float max_curr_data;

    float power_data;
    float max_power_data;

    int device_run_state;
    int lock;
}serial_transmission;


typedef struct serial_data{
    u8    en_cmd; 
    u8    state;
    float value;

    serial_transmission ptr;
}serial_data;


typedef void (*cmd_handler_t)(serial_data *ptr);


typedef struct cmd_entry{
    int   user_cmd;
    u8    en_cmd;
    u8    state;

    cmd_handler_t handler;
} cmd_entry;

extern serial_data serial_data_ptr;

class Power_Supply : private Robot_Hardware
{
    private:
        
    public:
        int Get_Power_Supply_Custom_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);
        int Power_Supply_Custom_Top_Frame_Analyze(shared_ptr<Device_class> Device, volatile u8 *Can_Frame);


        /**
         * @brief 打开串口
         * @param dev      设备号 ls /dev/tty* 处获取
         * @param baudrate 此参数输入应该为：已经完成115200->B115200的映射关系
        */
        static void S_open(const char* dev, int baudrate);
        /**
        * @brief 关闭串口
        */
        static void S_close();
        /**
        * @brief 串口发送
        */
        static void S_write(const char* arg, u8 len);
        /**
         * @brief 浮点数转字符串
        */
        static void float_to_str(float num, const char* prefix, char* str, int *len);
        /**
         * @brief 接受数据拆分
        */
        static std::vector<std::string> parseDelimitedString(const char* buf);
        /**
         * @brief 接收线程
        */
        void receiveThread();


        /**
         * @brief CMD实现函数
         * @param input_cmd 控制命令见 Power_Supply.hpp 中 User_Cmd
         * @param data      不需要设置值的功能不需要在意此参数，可设为任意值
        */
        int Cmd_Come_True(int input_cmd, float data);
        /**
         * @brief 读取设备相关信息
        */
        static void About_Device(serial_data *ptr); 
        /**
         * @brief 控制设备输出的开关
        */
        static void Device_OE(serial_data *ptr); 
        /**
         * @brief 设置设备输出电压
        */
        static void Device_VOL(serial_data *ptr); 
        /**
         * @brief 设置设备输出的最大电压
        */
        static void Device_VOL_LIM(serial_data *ptr); 
        /**
         * @brief   获取设备输出的最大电压
        */
        static void Device_Get_VOL_LIM(serial_data *ptr);
        /**
         * @brief 设置设备输出电流
        */
        static void Device_CURR(serial_data *ptr); 
        /**
         * @brief 设置设备输出最大电流
        */
        static void Device_CURR_LIM(serial_data *ptr);
        /**
         * @brief 获取设备输出最大电流
        */
        static void Device_Get_CURR_LIM(serial_data *ptr);
        /**
         * @brief   查询设备输出电压、电流、功率、错误码、模式
        */
        static void Device_ASK_Data(serial_data *ptr);
        /**
         * @brief   获取设备目前设置的电压数据
        */
        static void Device_ASK_VOL_Data(serial_data *ptr);
        /**
         * @brief   获取设备目前设置的电流数据
        */
        static void Device_ASK_CURR_Data(serial_data *ptr);
};

cmd_entry cmd_table[] = {
    /*|设备信息 |                |使能控制|     |状态位 |                                                       */
    { Device_Id,                (u8)NULL,     (u8)NULL,                   Power_Supply::About_Device         },
    /*| 设备输出使能 |            |使能控制|     |状态位 |                                                       */
    { Device_Out_En,            (u8)S_EN,     (u8)NULL,                   Power_Supply::Device_OE            },
    /*| 设备输出失能 |            |使能控制|     |状态位 |                                                       */
    { Device_Out_DISEn,         (u8)S_DISEN,  (u8)NULL,                   Power_Supply::Device_OE            },
    /*|设备输出电压设置|           |使能控制|     |状态位 |                                                       */
    { Device_Out_VOL,           (u8)NULL,     (u8)NULL,                   Power_Supply::Device_VOL           },
    /*|设备输出电压限制设置|        |使能控制|     |状态位 |                                                       */
    { Device_Out_VOL_LIM,       (u8)NULL,     (u8)NULL,                   Power_Supply::Device_VOL_LIM       },
    /*|查询设备输出电压限制设置|     |使能控制|     |状态位 |                                                       */
    { Device_Get_Out_VOL_LIM,   (u8)NULL,     (u8)Device_Get_Out_VOL_LIM, Power_Supply::Device_Get_VOL_LIM   },
    /*|设备输出电流设置|           |使能控制|     |状态位 |                                                       */
    { Device_Out_CURR,          (u8)NULL,     (u8)NULL,                   Power_Supply::Device_CURR          },
    /*|设备输出电流限制设置|        |使能控制|     |状态位 |                                                       */
    { Device_Out_CURR_LIM,      (u8)NULL,     (u8)NULL,                   Power_Supply::Device_CURR_LIM      },
    /*|查询设备输出电流限制设置|     |使能控制|     |状态位 |                                                       */
    { Device_Get_Out_CURR_LIM,  (u8)NULL,     (u8)Device_Get_Out_CURR_LIM,Power_Supply::Device_Get_CURR_LIM  },
    /*|访问设备目前状态|           |使能控制|     |状态位 |                                                       */
    { Device_Now_Data,          (u8)NULL,     (u8)Device_Now_Data,        Power_Supply::Device_ASK_Data      },
    /*|查询设备目前设置电压|        |使能控制|     |状态位 |                                                       */
    { Device_Get_VOL,           (u8)NULL,     (u8)Device_Get_VOL,         Power_Supply::Device_ASK_VOL_Data  },
    /*|查询设备目前设置电压|        |使能控制|     |状态位 |                                                       */
    { Device_Get_CURR,          (u8)NULL,     (u8)Device_Get_CURR,        Power_Supply::Device_ASK_CURR_Data },
};

#define TRANS_COUNT  (sizeof(cmd_table) / sizeof(cmd_table[0]))

#endif



