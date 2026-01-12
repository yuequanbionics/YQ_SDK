#ifndef TXRX_H_
#define TXRX_H_

#include "syst.hpp"
#include "CAN.hpp"

#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>

#define Motor_FOC_Start 1
#define Motor_FOC_Stop 0

#define Motor_CMD_READ 0
#define Motor_CMD_WRITE 1

#define Current_Control_Data_Struct_Len sizeof(Motor_Current_Control_Data)
#define Force_Control_Data_Struct_Len sizeof(Motor_Force_Control_Data)
#define Speed_Control_Data_Struct_Len sizeof(Motor_Speed_Control_Data)
#define Pos_Control_Data_Struct_Len sizeof(Motor_Pos_Control_Data)
#define MIT_Control_Data_Struct_Len sizeof(Motor_MIT_PD_Control_Data)
#define Motor_FB_Data_Struct_Len sizeof(Motor_FB_Data)
#define Motor_CMD_Data_Struct_Len sizeof(Motor_CMD_Data)

enum Motor_CMD_Data_Enum
{	
	CMD_Auto_Set_Id = 20,
	// Falsh+RAM
	Motor_CMD_Get_All_Config_Data,
	Motor_CMD_Save_Config_To_Flash,

	// FOC_Start+FB
	Motor_CMD_Start_Foc,
	Motor_CMD_FOC_FB,

	// Motor_Hardware_Config
	Motor_CMD_GD,
	Motor_CMD_Motor_MOD,
	Motor_CMD_Foc_Lock,
	Motor_CMD_A_B_C_Mirror,
	Motor_CMD_Set_Zero,
	Motor_CMD_ADC_Gain,
	Motor_CMD_I_To_Force_K,
	Motor_CMD_Max_Hardware_Current,
	Motor_CMD_multiple,
	Motor_CMD_Gear_reduction_ratio,
	Motor_CMD_Groove_torque_Gain,
	Motor_CMD_Groove_torque_Speed,
	Motor_CMD_Groove_torque_error_threshold,
	MD_K, 			//阻尼补偿系数
	IC_MU_FORCE,
	IC_MU_SPEED,
	MOTOR_correct_Force,
	MOTOR_correct_Speed,
	Motor_CMD_DT, //reboot
	Motor_CMD_CT, 
	Motor_CMD_DI,
	Motor_CMD_ENC_MOD, //reboot
	Motor_CMD_Heart_Beat,

	// Electricity PID
	Motor_CMD_electricity_D_P,
	Motor_CMD_electricity_D_I,
	Motor_CMD_electricity_D_OUT_MAX,
	Motor_CMD_electricity_D_Integration_MAX,
	Motor_CMD_electricity_Q_P,
	Motor_CMD_electricity_Q_I,
	Motor_CMD_electricity_Q_OUT_MAX,
	Motor_CMD_electricity_Q_Integration_MAX,

	// Speed PID
	Motor_CMD_speed_P,
	Motor_CMD_speed_I,
	Motor_CMD_speed_D,
	Motor_CMD_speed_OUT_MAX,
	Motor_CMD_speed_Integration_MAX,

	// Post PID
	Motor_CMD_Post_Speed_Limit,
	Motor_CMD_Post_P,
	Motor_CMD_Post_I,
	Motor_CMD_Post_OUT_MAX,
	Motor_CMD_Post_Integration_MAX,

	// calibration
	Motor_CMD_IC_MU_CAL,
	Motor_CMD_Motor_Calibration,
	Motor_CMD_Motor_Calibration_array,
	Motor_CMD_Reduction_gear_Calibration,
	Motor_CMD_Groove_torque_Calibration,
	Motor_CMD_Groove_torque_Calibration_array,
	Motor_CMD_MD_Calibration,

	// FOC_Control_Data
	Motor_CMD_Current_Control_Data,
	Motor_CMD_Force_Control_Data,
	Motor_CMD_Speed_Control_Data,
	Motor_CMD_Pos_Control_Data,
	Motor_CMD_MIT_Control_Data,

	// FOC_temp
	Motor_CMD_Get_temp,

	Motor_CMD_Debugint0,
	Motor_CMD_Debugint1,
	Motor_CMD_Debugfloat2,
	Motor_CMD_Debugfloat3,

	Motor_CMD_FB_MOD,
};

enum Motor_CONTROL_MOD
{
	Motor_Current_Control = 10,
	Motor_MIT_Control = 11,
	Motor_Pos_Control = 12,
	Motor_Force_Control = 13,
	Motor_Speed_Control = 14,
	
	Motor_MOD_END = 100,
};

typedef struct
{ // FOC反馈数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	u16 State;
	s16 Pos_Data;
	s16 Speed_Data;
	s16 Force_Data;
	char TMP[2];

} Motor_FB_Data;

typedef struct
{ // 电流环控制数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	s16 Current_Data;  //单位A 乘以100  保留两位小数

} Motor_Current_Control_Data;

typedef struct
{ // 力矩环控制数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	s16 Force_Data; //单位N 乘以100  保留两位小数

} Motor_Force_Control_Data;

typedef struct
{ // 速度环控制数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	s16 Speed_Data;
	s16 Force_FF_Data;
	u16 Force_Max_Data;
} Motor_Speed_Control_Data;

typedef struct
{ // 位置环控制数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	s16 Pos_Data;
	s16 Speed_FF_Data;
	s16 Force_FF_Data;
	u16 Speed_Max_Data;
	u16 Force_Max_Data;
} Motor_Pos_Control_Data;

typedef struct
{ // MIT控制数据
	// Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	// DATA
	u8 CRC8;
	u8 Cmd;
	s16 P;  //位置
	s16 V;  //单位转每分钟 乘以100  保留两位小数
	u16 Kp; //乘以1000  保留3位小数  
	u16 Kd; //乘以1000  保留3位小数  
	s16 T_F;//单位N 乘以100  保留两位小数

} Motor_MIT_PD_Control_Data;

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

} Motor_CMD_Data;

///////////////////////////////////////////////////////////////////////
typedef struct
{
	int P;
	int I;
	int D;
	int64_t Integration_MAX;
	int64_t Out_MAX;
} PI_control_Config;

typedef struct
{
	int P;
	int I;
	int64_t Integration_MAX;
	int64_t Out_MAX;
} electricity_PI;

typedef struct
{
	u16 REG0;
	u16 REG1;
	u16 REG2;
	u16 REG3;
	u16 REG4;
	u16 REG5;
	u16 REG6;

	int gain;
} DRV83XX_Config;

typedef struct
{
	float I_Max;
	s16 st_offest;

	s16 GD;
	int MOD;
	int EN;

	int Groove_torque_Gain;
	int Groove_torque_error_threshold;
	int Groove_torque_Speed;

	int st_to_Integration_Second;
	int st_to_Integration_Second_old;

	int gear_reduction_ratio;
	u32 Post_Speed_Max;
	int A_B_C_Mirror;
	float tmp[2];
} foc_data;

typedef struct
{
	int ST_NUM;
	int ST_array[8192];
	int ENC_ST_NUM;
	int ENC_ST_array[8192];
} Calibration_Data;

typedef struct
{
	int st_Target[65536];
	int st[65536];
	int I_Q[65536];
	int st_Target_st[65536];
} Groove_torque_Calibration_Data;

typedef struct
{ // Motor_Info
	Main_Data Main_Datas;
	foc_data M0_IQ_datas;

	float Gear_Ratio;
	int Scaling_Of_Reduction_Ratio;

	float DI_I_MAX;
	float I_Hardware_Max;

	DRV83XX_Config M0_DRV8328_Config;
	electricity_PI electricity_PI_Data_D;
	electricity_PI electricity_PI_Data_Q;
	PI_control_Config M0_Speed_PI_control_Config;
	PI_control_Config M0_Post_PI_control_Config;

	Motor_FB_Data Motor_FB_Datas;

	u16 ERR_Code;
	float TMP[2];

	Calibration_Data Calibration_array;
	Groove_torque_Calibration_Data Groove_torque_Calibration_array;
} Motor_Info;

Motor_CMD_Data *Motor_CMD_Data_F(u16 ID, u8 Flag, u16 CMD, u16 data0, u16 data1, u16 data2, u16 data3, u16 data4, Motor_CMD_Data *Motor_CMD_Datas);
Motor_Current_Control_Data *Motor_Current_Control_Data_F(u16 ID, u8 Flag, u16 Current_Data, Motor_Current_Control_Data *Datas);
Motor_Force_Control_Data *Motor_Force_Control_Data_F(u16 ID, u8 Flag, u16 Force_Data, Motor_Force_Control_Data *Datas);
Motor_Speed_Control_Data *Motor_Speed_Control_Data_F(u16 ID, u8 Flag, u16 Speed_Data, u16 Force_Max_Data, Motor_Speed_Control_Data *Datas);
Motor_Pos_Control_Data *Motor_Pos_Control_Data_F(u16 ID, u8 Flag, u16 Pos_Data, u16 Speed_Max_Data, u16 Force_Max_Data, Motor_Pos_Control_Data *Datas);
Motor_MIT_PD_Control_Data *Motor_MIT_Control_Data_F(u16 ID, u8 Flag, u16 P, u16 V, u16 Kp, u16 Kd, u16 T_F, Motor_MIT_PD_Control_Data *Datas);


// #define Make_Motor_CMD_Get_All_Config_Data(ID, Motor_CMD_Datas) Motor_CMD_Data_F(ID, Motor_CMD_Get_All_Config_Data, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, Motor_CMD_Datas)
// #define Make_Motor_CMD_Save_Config_To_Flash(ID, Motor_CMD_Datas) Motor_CMD_Data_F(ID, Motor_CMD_Save_Config_To_Flash, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, Motor_CMD_Datas)
// #define Make_Motor_CMD_Start_Foc(ID, EN, RW, Motor_CMD_Datas) Motor_CMD_Data_F(ID, Motor_CMD_Start_Foc, EN, 0xFFFF, 0xFFFF, 0xFFFF, RW, Motor_CMD_Datas)


class GaussianFilter1D {
private:
    std::vector<double> kernel;         // 高斯核
    std::deque<double> buffer;          // 滑动窗口缓冲区
    std::vector<int> reflection_map;    // 镜像反射映射表
    int kernel_size;                    // 核大小
    int pad;                            // 边界填充大小
    int half_kernel;                    // 半核大小（用于对称性优化）
    bool use_reflection;                // 是否使用镜像反射

public:
    // 构造函数：初始化高斯核和反射映射表
    GaussianFilter1D(int kernel_size = 5, double sigma = 1.0, bool reflect = true)
        : kernel_size(kernel_size), use_reflection(reflect) {
        
        // 确保核大小为奇数
        if (kernel_size % 2 == 0) {
            kernel_size++;
            this->kernel_size = kernel_size;
        }
        
        pad = kernel_size / 2;
        half_kernel = pad;
        kernel = generate_gaussian_kernel(kernel_size, sigma);
        reflection_map.resize(2 * kernel_size - 1);
        
        // 预计算镜像反射映射表
        for (int i = 0; i < 2 * kernel_size - 1; ++i) {
            int mirror_idx = i;
            if (i < pad) {
                mirror_idx = pad - i;
            } else if (i >= kernel_size + pad) {
                mirror_idx = 3 * pad - (i - kernel_size);
            } else {
                mirror_idx = i - pad;
            }
            reflection_map[i] = mirror_idx;
        }
        
        // 初始化缓冲区
        reset();
    }

    // 生成一维高斯核（优化版本）
    std::vector<double> generate_gaussian_kernel(int size, double sigma) {
        std::vector<double> kernel(size, 0.0);
        int center = size / 2;
        double sum = 0.0;
        double sigma_sq = 2.0 * sigma * sigma;
        double denom = sqrt(2.0 * M_PI) * sigma;

        // 利用对称性，只计算一半
        for (int i = 0; i <= center; ++i) {
            double x = static_cast<double>(i);
            double val = exp(-(x * x) / sigma_sq) / denom;
            kernel[center + i] = val;
            if (i > 0) {
                kernel[center - i] = val;
            }
            sum += (i == 0) ? val : 2.0 * val;
        }

        // 归一化
        for (auto& val : kernel) {
            val /= sum;
        }

        return kernel;
    }

    // 优化后的滤波函数（减少条件判断和重复计算）
    double filter(double new_data) {
        // 将新数据添加到缓冲区尾部
        buffer.push_back(new_data);
        
        // 如果缓冲区大小超过核大小，移除最旧的数据
        if ((u32)buffer.size() > (u32)kernel_size) {
            buffer.pop_front();
        }

        // 应用高斯滤波（利用对称性优化计算）
        double result = 0.0;
        const int valid_size = buffer.size();
        const int buffer_offset = pad - (kernel_size - valid_size);
        
        // 中心点单独处理
        const int center_idx = buffer_offset + pad;
        if (center_idx >= 0 && center_idx < valid_size) {
            result += buffer[center_idx] * kernel[pad];
        } else if (use_reflection) {
            result += buffer[reflection_map[center_idx + pad]] * kernel[pad];
        }
        
        // 对称点对处理（减少一半乘法）
        for (int i = 1; i <= half_kernel; ++i) {
            const int left_idx = center_idx - i;
            const int right_idx = center_idx + i;
            
            double left_val = 0.0;
            double right_val = 0.0;
            
            if (left_idx >= 0 && left_idx < valid_size) {
                left_val = buffer[left_idx];
            } else if (use_reflection) {
                left_val = buffer[reflection_map[left_idx + pad]];
            }
            
            if (right_idx >= 0 && right_idx < valid_size) {
                right_val = buffer[right_idx];
            } else if (use_reflection) {
                right_val = buffer[reflection_map[right_idx + pad]];
            }
            
            // 利用对称性，k[pad-i] == k[pad+i]
            result += (left_val + right_val) * kernel[pad + i];
        }

        return result;
    }

    // 重置滤波器状态
    void reset() {
        buffer.clear();
        for (int i = 0; i < kernel_size - 1; ++i) {
            buffer.push_back(0.0);
        }
    }
};

#endif
