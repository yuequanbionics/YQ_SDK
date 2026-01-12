#ifndef MOTOR_BM_M1502D_H_
#define MOTOR_BM_M1502D_H_

#include <memory>

#include "HARDWARE_TOP.hpp"
#include "UDP.hpp"
#include "syst.hpp"

#define BM_Motor_FOC_Start 1
#define BM_Motor_FOC_Stop 0

#pragma pack(push, 1)

// BM_M1502D电机控制数据结构
typedef struct {
    // Head
    u16 Can_Id;  // CAN标识符：ID 1-4使用0x32，ID 5-8使用0x33
    u16 Len;     // 数据长度
    u16 Flag;    // 标志位

    // DATA
    u8 Data[8];  // 8字节数据域
} CMD_Data_BM_M1502D;

// BM_M1502D电机反馈数据结构
typedef struct {
    // Head
    u16 Can_Id;  // 反馈CAN ID (0x96 + ID)
    u16 Len;     // 数据长度
    u16 Flag;    // 标志位

    // DATA - 反馈数据
    u8 Data[8];  // 8字节反馈数据
} FB_Data_BM_M1502D;

#pragma pack(pop)

// BM_M1502D电机指令定义
enum BM_M1502D_Command : uint16_t {
    BM_CMD_DRIVE_MOTORS_1_4 = 0x32,  // 驱动电机1-4控制指令（ID 1-4使用）
    BM_CMD_DRIVE_MOTORS_5_8 = 0x33,  // 驱动电机5-8控制指令（ID 5-8使用）
    BM_CMD_QUERY_OFFSET_ID = 0x96,   // 反馈报文标识符
    BM_CMD_SET_MODE = 0x105,         // 设置模式指令(多电机)
    BM_CMD_SET_FEEDBACK = 0x106,     // 设置反馈方式指令(多电机)
    BM_CMD_QUERY_STATUS = 0x107,     // 查询状态指令(单电机)
    BM_CMD_SET_MOTOR_ID = 0x108,     // 设置电机ID指令
    BM_CMD_SAVE_TO_FLASH = 0x10C     // 参数保存
};

// 电机运行模式定义
enum BM_Control_Mode : uint8_t {
    BM_MODE_CURRENT = 0x01,   // 电流环模式
    BM_MODE_SPEED = 0x02,     // 速度环模式
    BM_MODE_POSITION = 0x03,  // 位置环模式
    BM_MODE_DISABLE = 0x09,   // 失能电机
    BM_MODE_ENABLE = 0x0A     // 使能电机（默认使能）
};

// 反馈模式定义
enum BM_Feedback_Mode : uint16_t {
    BM_FEEDBACK_QUERY = 0x80,  // 查询模式（默认）
    BM_FEEDBACK_1MS = 0x01,    // 主动上报间隔1ms
    BM_FEEDBACK_10MS = 0x0A,   // 主动上报间隔10ms
    BM_FEEDBACK_50MS = 0x32,   // 主动上报间隔50ms
    BM_FEEDBACK_64MS = 0x40,   // 主动上报间隔64ms
    BM_FEEDBACK_127MS = 0x7F   // 主动上报间隔127ms
};

// M1502D故障代码枚举
enum BM_Fault_Code : uint8_t {
    BM_Fault_None = 0x00,                        // 无故障
    BM_Fault_Undervoltage2 = 0x01,               // 欠压 2 (母线电压 < 18V)
    BM_Fault_Undervoltage1 = 0x02,               // 欠压 1 (18V < 母线电压 < 22V)
    BM_Fault_Overvoltage = 0x03,                 // 过压 (母线电压 > 63V)
    BM_Fault_Overcurrent = 0x0A,                 // 过流 (默认：母线电流 > 20A)
    BM_Fault_Overspeed = 0x14,                   // 过速 (转速 > 300RPM)
    BM_Fault_Overtemp1 = 0x20,                   // 过温 1 (电机绕组温度 > 80℃)
    BM_Fault_Overtemp2 = 0x1F,                   // 过温 2 (电机绕组温度 > 120℃)
    BM_Fault_SampleResistor = 0x29,              // 采样电阻故障
    BM_Fault_PositionSensor = 0x2A,              // 位置传感器自身故障
    BM_Fault_PositionSensorInterference = 0x2B,  // 位置传感器信号被干扰
    BM_Fault_TempSensorRange = 0x2D,             // 温度传感器超出量程
    BM_Fault_CommunicationTimeout =
        0x3C,              // 通信超时(默认无保护，需用户自行开启)
    BM_Fault_Stall = 0x62  // 堵转(默认:电流 > 5A 并且 转速为0)
};

// 查询目标内容枚举
enum BM_Query_Target : uint8_t {
    BM_QUERY_SPEED = 0x01,        // 查询速度
    BM_QUERY_CURRENT = 0x02,      // 查询母线电流
    BM_QUERY_TEMPERATURE = 0x03,  // 查询绕组温度
    BM_QUERY_POSITION = 0x04,     // 查询位置
    BM_QUERY_FAULT = 0x05,        // 查询故障值
    BM_QUERY_MODE = 0x06          // 查询当前模式
};

/********************************* */
// PI参数调节模式枚举
enum BM_PI_Adjust_Mode : uint16_t {
    BM_PI_MODE_CURRENT = 0x01,         // 修改电流环PI参数
    BM_PI_MODE_SPEED = 0x02,           // 修改速度环PI参数
    BM_PI_MODE_POSITION = 0x03,        // 修改位置环PI参数
    BM_PI_MODE_CURRENT_LIMIT = 0x11,   // 修改电流环PI参数输出限幅
    BM_PI_MODE_SPEED_LIMIT = 0x22,     // 修改速度环PI参数输出限幅
    BM_PI_MODE_POSITION_LIMIT = 0x33,  // 修改位置环PI参数输出限幅
    BM_PI_MODE_RESET_ALL = 0xFF        // 复位所有的环路PID参数
};

// PI参数结构体
typedef struct {
    uint16_t p_numerator;  // P被除数
    uint16_t i_numerator;  // I被除数
    uint8_t p_divisor;     // P除数（2的n次幂，n值）
    uint8_t i_divisor;     // I除数（2的n次幂，n值）
} BM_PI_Parameters;

// PI输出限幅结构体
typedef struct {
    int16_t max_output;  // 最大输出值
    int16_t min_output;  // 最小输出值
} BM_PI_Output_Limit;

/********************************* */
#define Motor_Device_Init_BM_M1502D                                            \
    [](shared_ptr<Device_Struct> Device, YAML::Node* Node) -> int {            \
        Motor_BM_M1502D* One_Motor = new Motor_BM_M1502D();                    \
        Device->Device_Private_Class = (void*)One_Motor;                       \
        if (Node != nullptr)                                                   \
            return One_Motor->Get_Motor_Device_Data_From_Yaml_And_Init(Device, \
                                                                       *Node); \
        else                                                                   \
            return 0;                                                          \
    }

#define Motor_Device_CallBack_BM_M1502D                       \
    [](void* Device_Private_Class, volatile u8* Msg) -> int { \
        return ((Motor_BM_M1502D*)Device_Private_Class)       \
            ->Motor_Top_Frame_Analyze(Msg);                   \
    }

#define Motor_Device_Delete_BM_M1502D                    \
    [](void* Device_Private_Class) {                     \
        delete ((Motor_BM_M1502D*)Device_Private_Class); \
        Device_Private_Class = nullptr;                  \
    }

class Motor_BM_M1502D : private Robot_Hardware {
   private:
    float current_P = 0.0f;        // 当前位置（度）
    float current_V = 0.0f;        // 当前速度（RPM）
    float current_F = 0.0f;        // 当前力矩（Nm）
    float current_temp = 0.0f;     // 当前温度（℃）
    float current_current = 0.0f;  // 当前电流（A）

    BM_Fault_Code fault_code = BM_Fault_None;           // 当前故障代码
    BM_Control_Mode current_mode = BM_MODE_SPEED;       // 当前运行模式
    BM_Control_Mode control_mode = BM_MODE_SPEED;       // 控制模式
    BM_Feedback_Mode feedback_mode = BM_FEEDBACK_10MS;  // 反馈模式

    u8 motor_id = 1;               // 电机ID（1-8）
    u16 can_identifier_tx = 0x32;  // 发送CAN标识符
    u16 can_identifier_rx = 0x97;  // 接收CAN标识符（0x96 + ID）

    bool is_enabled = false;        // 电机使能状态
    bool is_saving_params = false;  // 参数保存状态

    /******************************************** */
    static u8 group_1_4_data[8];   // 1-4号电机数据缓存
    static u8 group_5_8_data[8];   // 5-8号电机数据缓存
    static bool data_initialized;  // 数据初始化状态
    static std::mutex data_mutex;

   public:
    std::shared_ptr<Device_Struct> s_device;

    /**
     * @brief 电机使能/失能
     * @param EN 1使能，0失能
     */
    int Motor_EN(std::shared_ptr<Device_Struct> Device_P, int EN);

    /**
     * @brief 设置电机控制模式
     * @param mode 控制模式（电流环/速度环/位置环）
     */
    int Set_Control_Mode(std::shared_ptr<Device_Struct> Device_P, BM_Control_Mode mode);

    /**
     * @brief 设置电机反馈模式
     * @param mode 反馈模式
     */
    int Set_Feedback_Mode(std::shared_ptr<Device_Struct> Device_P, BM_Feedback_Mode mode);

    /**
     * @brief MIT控制数据发送（速度环控制）
     * @param Rad 位置值（度）- 在速度环模式下此参数无效
     * @param Speed_Rad_S 速度值（度/秒）- 转换为RPM
     * @param Force_N 力矩值（N）- 在速度环模式下此参数无效
     * @param P_N_Rad P参数 - 在速度环模式下此参数无效
     * @param D_N_Rad_s D参数 - 在速度环模式下此参数无效
     */
    int Send_MIT_PD_Control_Data(std::shared_ptr<Device_Struct> Device_P, float Rad, float Speed_Rad_S, float Force_N, float P_N_Rad, float D_N_Rad_s);

    /**
     * @brief 获取电机反馈数据
     * @param P 位置（度）
     * @param V 速度（RPM）
     * @param F 力矩（Nm）
     */
    int Get_Motor_FB_Data(std::shared_ptr<Device_Struct> Device_P, float* P, float* V, float* F);

    /**
     * @brief 查询电机状态（发送查询指令）
     */
    int Query_Motor_Status(std::shared_ptr<Device_Struct> Device_P, BM_Query_Target targets[], u8 target_count);

    /**
     * @brief 从YAML配置文件读取电机参数并初始化
     */
    int Get_Motor_Device_Data_From_Yaml_And_Init(std::shared_ptr<Device_Struct> Device, YAML::Node One_Node);

    /**
     * @brief CAN帧数据解析回调函数
     */
    int Motor_Top_Frame_Analyze(volatile u8* Can_Frame);

    /**
     * @brief 设置电机ID
     * @param id 电机ID（1-8）
     */
    int Set_Motor_ID(std::shared_ptr<Device_Struct> Device_P, u8 id);

    /**
     * @brief 保存参数到Flash（自动处理使能状态）
     */
    int Save_Parameters_To_Flash_Safe(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 直接保存参数到Flash（不检查使能状态）
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Save_Parameters_To_Flash_Direct(std::shared_ptr<Device_Struct> Device_P);

    /***********************************************状态获取*******************************************************
    /**
     * @brief 获取电机故障状态
     * @return 故障代码
     */
    BM_Fault_Code Get_Fault_Code() const { return fault_code; }

    /**
     * @brief 获取电机当前运行模式
     * @return 当前模式
     */
    BM_Control_Mode Get_Current_Mode() const { return current_mode; }

    /**
     * @brief 获取电机温度
     * @return 温度值（℃）
     */
    float Get_Temperature() const { return current_temp; }

    /**
     * @brief 获取电机使能状态
     * @return 使能状态
     */
    bool Is_Enabled() const { return is_enabled; }

    /**
     * @brief 获取电机ID
     * @return 电机ID
     */
    u8 Get_Motor_ID() const { return motor_id; }

    /**
     * @brief 获取电机电流
     * @return 电流值（A）
     */
    float Get_Current() const { return current_current; }

    /**
     * @brief 获取参数保存状态
     * @return 参数保存状态
     */
    bool Is_Saving_Params() const { return is_saving_params; }

    /******************** 组数据管理 ************************************ */
    static void Initialize_Group_Data();

    static u8* Get_Group_Data(u8 motor_id);

    static void Update_Group_Data(u8 motor_id, const u8* data, size_t data_size);

    /******************** 数据转换 ************************************* */
    /**
     * @brief RPM转换为数据值（-21000~21000对应-210RPM~210RPM）
     * @param rpm 速度值（RPM）
     * @return 转换后的数据值
     */
    s16 RPM_To_Data(float rpm);

    /**
     * @brief 数据值转换为RPM
     * @param data 数据值
     * @return 转换后的速度值（RPM）
     */
    float Data_To_RPM(s16 data);

    /**
     * @brief 度转换为位置数据值
     * @param degree 角度值（度）
     * @return 转换后的位置数据值
     */
    u16 Degree_To_Position_Data(float degree);

    /**
     * @brief 位置数据值转换为度
     * @param data 位置数据值
     * @return 转换后的角度值（度）
     */
    float Position_Data_To_Degree(u16 data);

    /**
     * @brief 故障代码转换为字符串描述
     */
    const char* Fault_Code_To_String(BM_Fault_Code fault);

    /**
     * @brief 模式代码转换为字符串描述
     */
    const char* Mode_To_String(BM_Control_Mode mode);

    /**
     * @brief 反馈模式转换为字符串描述
     */
    const char* Feedback_Mode_To_String(BM_Feedback_Mode mode);

    /**************** PI 参数设置 **************** */
    /**
     * @brief 设置电机PI参数
     * @param Device_P 设备结构体指针
     * @param mode PI参数模式（电流环/速度环/位置环）
     * @param pi_params PI参数结构体
     * @return 执行结果
     */
    int Set_PI_Parameters(std::shared_ptr<Device_Struct> Device_P, BM_PI_Adjust_Mode mode, const BM_PI_Parameters& pi_params);

    /**
     * @brief 设置电机PI参数输出限幅
     * @param Device_P 设备结构体指针
     * @param mode PI限幅模式（电流环/速度环/位置环）
     * @param limit 输出限幅结构体
     * @return 执行结果
     */
    int Set_PI_Output_Limit(std::shared_ptr<Device_Struct> Device_P, BM_PI_Adjust_Mode mode, const BM_PI_Output_Limit& limit);

    /**
     * @brief 复位所有环路PID参数
     * @param Device_P 设备结构体指针
     * @return 执行结果
     */
    int Reset_All_PID_Parameters(std::shared_ptr<Device_Struct> Device_P);

    // 辅助函数
    /**
     * @brief 获取模式描述字符串
     * @param mode PI调节模式
     * @return 模式描述字符串
     */
    static const char* PI_Mode_To_String(BM_PI_Adjust_Mode mode);

    /**
     * @brief 浮点数转换为PI参数格式
     * @param value 浮点数值
     * @param divisor 除数
     * @return 转换后的被除数
     */
    static uint16_t Float_To_PI_Value(float value, uint8_t divisor);

    /**
     * @brief PI参数格式转换为浮点数
     * @param numerator 被除数
     * @param divisor 除数
     * @return 转换后的浮点数值
     */
    static float PI_Value_To_Float(uint16_t numerator, uint8_t divisor);

   private:
    /******************************************************* */
    // 私有成员变量
    BM_PI_Parameters current_pi_params;   // 当前PI参数
    BM_PI_Parameters speed_pi_params;     // 速度环PI参数
    BM_PI_Parameters position_pi_params;  // 位置环PI参数
    BM_PI_Output_Limit current_limit;     // 电流环输出限幅
    BM_PI_Output_Limit speed_limit;       // 速度环输出限幅
    BM_PI_Output_Limit position_limit;    // 位置环输出限幅

    // 私有辅助函数
    /**
     * @brief 解析PI参数反馈
     * @param fb_data 反馈数据指针
     * @return 解析结果
     */
    int Parse_PI_Parameters_Feedback(FB_Data_BM_M1502D* fb_data);
    /*********************************************************** */
    /**
     * @brief 发送CAN指令
     * @param data 8字节数据
     */
    int Send_CAN_Command(std::shared_ptr<Device_Struct> Device_P, u8 data[8]);

    /**
     * @brief 初始化电机控制模式
     */
    int Init_Control_Mode(std::shared_ptr<Device_Struct> Device_P);

    /**
     * @brief 解析主动上报数据
     * @param fb_data 反馈数据指针
     * @return 解析结果
     */
    int Parse_Active_Feedback(FB_Data_BM_M1502D* fb_data);

    /**
     * @brief 解析查询反馈数据
     * @param fb_data 反馈数据指针
     * @return 解析结果
     */
    int Parse_Query_Feedback(FB_Data_BM_M1502D* fb_data);

    /**
     * @brief 获取系统时间戳,此函数不会修改对象状态，声明为const
     */
    u32 Get_System_Tick() const;
    /**
     * @brief 等待电机完全停止
     * @param timeout_ms 超时时间（毫秒）
     * @return 是否成功停止
     */
    bool Wait_For_Motor_Stop(std::shared_ptr<Device_Struct> Device_P, u32 timeout_ms = 1000);

    /**
     * @brief 检查数据有效性
     * @param data 数据指针
     * @param size 数据大小
     * @return 是否有效
     */
    bool Check_Data_Validity(const void* data, size_t size) const;
};

#endif
