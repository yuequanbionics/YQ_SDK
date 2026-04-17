#ifndef ADC_H_
#define ADC_H_

#include "HARDWARE_TOP.hpp"
#include <string>
#include <vector>
#include <mcu_config_map.hpp>

// 定义ADC通道配置的结构体
struct ADC_Channel_Config
{
    // 通道号
    uint32_t ADC_Channel;
    // 转换排名
    uint32_t ADC_Rank;
    // 采样时间
    uint32_t ADC_SamplingTime;
    // 单端/差分
    uint32_t ADC_SingleDiff;
    // 偏移编号
    uint32_t ADC_OffsetNumber;
    // 偏移值
    uint32_t ADC_Offset;
    // 偏移符号饱和使能
    uint32_t ADC_OffsetSignedSaturation;
};

// 定义单个ADC的配置结构体
struct ADC_Instance_Config
{
    // ADC实例
    uint32_t ADC_Instance;
    // ADC使能
    uint32_t ADC_EN;
    // 时钟预分频
    uint32_t ADC_ClockPrescaler;
    // 分辨率
    uint32_t ADC_Resolution;
    // 扫描模式
    uint32_t ADC_ScanConvMode;
    // EOC选择
    uint32_t ADC_EOCSelection;
    // 低功耗自动等待
    uint32_t ADC_LowPowerAutoWait;
    // 连续转换模式
    uint32_t ADC_ContinuousConvMode;
    // 转换通道数
    uint32_t ADC_NbrOfConversion;
    // 间断转换模式
    uint32_t ADC_DiscontinuousConvMode;
    // 外部触发转换
    uint32_t ADC_ExternalTrigConv;
    // 外部触发沿
    uint32_t ADC_ExternalTrigConvEdge;
    // 转换数据管理
    uint32_t ADC_ConversionDataManagement;
    // 溢出处理
    uint32_t ADC_Overrun;
    // 左移位数
    uint32_t ADC_LeftBitShift;
    // 过采样模式
    uint32_t ADC_OversamplingMode;
    // 过采样比率
    uint32_t ADC_Oversampling_Ratio;
    // ADC模式
    uint32_t ADC_Mode;
    // 该ADC的所有通道配置
    //std::vector<ADC_Channel_Config> ADC_ChannelConfig;
    ADC_Channel_Config ADC_ChannelConfig[8];
};

struct Global_ADC_Config
{
    // Head
	u16 Can_Id;
	u16 Len;
	u16 Flag;

	u8 Cmd;
    //std::vector<ADC_Instance_Config> ADC_Config;
    ADC_Instance_Config ADC_Config;
};

class ADC : private Robot_Hardware
{
    public:
        int Get_Yaml_And_Init(const shared_ptr<Device_class>& Device, const YAML::Node& One_Node);
};



#endif //ADC_H_