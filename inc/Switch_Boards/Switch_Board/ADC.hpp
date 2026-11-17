#ifndef ADC_H_
#define ADC_H_

#include "HARDWARE_TOP.hpp"
#include "mcu_config_map.hpp"
#include <array>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>

/* SDK 与 MCU 使用相同字段顺序和固定宽度类型，按小端序传输结构体。 */

// 包类型和控制动作，与 MCU 的 ADC_Plug_In.h 对应。
#define ADCW_CMD_CONFIG 27u
#define ADCW_CMD_CONTROL 28u

/* Control_Cmd：ADCW_READ_ONCE 触发一批采样，ADCW_CONFIG 标识配置回执。 */
enum ADCW_ControlCmd
{
    ADCW_CONFIG = 1u,
    ADCW_READ_ONCE = 2u
};

enum ADCW_Status
{
    ADCW_OK = 0u,
    ADCW_INVALID = 1u, /* SDK 或 MCU 检测到无效参数。 */
    ADCW_UNSUPPORTED = 2u,
    ADCW_HAL = 3u,
    ADCW_TIMEOUT = 4u, /* SDK 等待回包超时，不表示 MCU 已取消请求。 */
    ADCW_UNAVAILABLE = 5u, /* MCU 实例未使能或未开启采样中断。 */
    ADCW_BUSY = 6u /* MCU 正在采样，或 SDK 发送函数返回失败。 */
};

// 固定数组容量。RankCount/GpioCount 指定本次配置中使用的项数。
#define ADCW_INSTANCE_COUNT 3u
#define ADCW_RANK_MAX 16u
#define ADCW_SAMPLE_MAX ADCW_RANK_MAX
#define ADCW_GPIO_MAX ADCW_RANK_MAX /* 每个 Rank 最多关联一个独立引脚。 */
#define ADCW_REPLY_ID_OFFSET 1024u

typedef struct
{
    uint32_t Channel;
    uint32_t SamplingTime;
    uint32_t SingleDiff; // SDK 组包时固定为 ADC_SINGLE_ENDED。
    uint32_t OffsetNumber;
    uint32_t Offset;
} ADCW_RankItem;

typedef struct
{
    uint8_t Port; /* 'A'~'K' */
    uint8_t Pin;  /* 0~15 */
    uint16_t Pad; // 对齐填充，组包时置零。
} ADCW_GpioItem;

/* 公共包头 8 字节，与全机帧头 Can_Id/Len/Flag/CRC8/Cmd 对齐。 */

/* ADC_Config：SDK → MCU，Cmd=ADCW_CMD_CONFIG，424 字节。
 * Ranks 数组顺序即 Rank 1~N；同一通道可占多个 Rank。 */
typedef struct
{
    uint16_t Can_Id;       /* 0 */
    uint16_t Len;          /* 2 */
    uint16_t Flag;         /* 4 */
    uint8_t CRC8;          /* 6，发送流程补 */
    uint8_t Cmd;           /* 7 */
    uint8_t ReservedHeader; /* 8，保留以对齐后续字段 */
    uint8_t Instance;      /* 9，1~3 */
    uint8_t Enable;        /* 10 */
    uint8_t RankCount;     /* 11，1~16 */
    uint8_t GpioCount;     /* 12，0~16 */
    uint8_t IrqPreemptPriority; /* 13，ADC1/2 共用 ADC_IRQn */
    uint8_t IrqSubPriority;     /* 14 */
    uint8_t IrqEnable;          /* 15，0 关闭，1 开启 */
    uint32_t RequestId;    /* 16 */
    uint32_t ClockPrescaler;   /* 20 */
    uint32_t Resolution;       /* 24 */
    uint32_t LeftBitShift;     /* 28 */
    uint32_t OversamplingMode; /* 32 */
    uint32_t OversamplingRatio; /* 36 */
    ADCW_RankItem Ranks[ADCW_RANK_MAX]; /* 40..359 */
    ADCW_GpioItem Gpios[ADCW_GPIO_MAX]; /* 360..423 */
} ADC_Config;

/* ADC_Control：84 字节，用于采样请求、采样结果和配置回执。
 * 采样请求/结果使用 Cmd=ADCW_CMD_CONTROL，配置回执使用 Cmd=ADCW_CMD_CONFIG。
 * MCU 逐 EOC 读取 DR；采样成功时按 Rank 顺序返回全部样本，失败时 SampleCount 为 0。
 * 配置回执不携带样本；读取请求的结果字段置零。 */
typedef struct
{
    uint16_t Can_Id;       /* 0 */
    uint16_t Len;          /* 2 */
    uint16_t Flag;         /* 4 */
    uint8_t CRC8;          /* 6 */
    uint8_t Cmd;           /* 7 */
    uint8_t ReservedHeader; /* 8，保留以对齐后续字段 */
    uint8_t Instance;      /* 9 */
    uint8_t Status;        /* 10，回包状态 */
    uint8_t Control_Cmd;   /* 11，control_cmd */
    uint8_t RankCount;     /* 12，本批 Rank 总数 */
    uint8_t SampleCount;   /* 13，有效样本数 */
    uint8_t Overrun;       /* 14，MCU 置零；溢出通过 Status=ADCW_HAL 报告 */
    uint8_t Reserved;      /* 15 */
    uint32_t RequestId;    /* 16 */
    uint32_t Samples[ADCW_SAMPLE_MAX]; /* 20..83，Rank 1~16 */
} ADC_Control;

#define ADCW_CONFIG_SIZE ((uint16_t)sizeof(ADC_Config))
#define ADCW_CONTROL_SIZE ((uint16_t)sizeof(ADC_Control))
#define ADCW_MAX ADCW_CONFIG_SIZE

// index 从 0 开始，调用方取值范围为 0~15。
uint32_t ADC_RankValue(unsigned index);

// 应用配置类型；Configure 将配置编码为 ADC_Config 协议包。
struct ADC_RankInit
{
    uint32_t Rank = ADC_REGULAR_RANK_1; // 描述性字段，不参与编码；MCU 按数组位置分配 Rank 1~N。
    uint32_t Channel = 0;
    uint32_t SamplingTime = 0;
    uint32_t OffsetNumber = ADC_OFFSET_NONE;
    uint32_t Offset = 0;
};

struct ADC_GpioInit
{
    uint8_t Port = 0; // 'A'..'K'
    uint8_t Pin = 0;  // 0..15
};

struct ADC_InitConfig
{
    uint8_t Instance = 1;
    uint8_t Enable = 1;
    uint8_t IrqPreemptPriority = 5; // ADC1/2 共用 IRQ，配置时采用同一优先级。
    uint8_t IrqSubPriority = 0;
    uint8_t IrqEnable = 1;
    uint32_t ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
    uint32_t Resolution = ADC_RESOLUTION_16B;
    uint32_t LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    uint32_t OversamplingMode = 0; /* DISABLE */
    uint32_t OversamplingRatio = 1;
    uint8_t RankCount = 0;
    uint8_t GpioCount = 0;
    std::array<ADC_RankInit, ADCW_RANK_MAX> Ranks{}; // 有效项为前 RankCount 项。
    std::array<ADC_GpioInit, ADCW_GPIO_MAX> Gpios{}; // 重复通道只需初始化一次对应引脚。
};

// 编码配置，不校验参数；调用方须保证 RankCount 为 1~16、GpioCount 为 0~16。
void ADC_EncodePacket(const ADC_InitConfig &config, ADC_Config &packet);

struct ADC_Result
{
    uint16_t device_id = 0;
    uint16_t flag = 0;
    uint8_t instance = 0;
    uint8_t status = 0;
    uint8_t rank_count = 0;   // 本次采样的 Rank 数；配置回执和启动前错误回执为 0。
    uint8_t sample_count = 0; // 有效样本数，仅 samples 的前 sample_count 项有效。
    uint8_t overrun = 0;      // MCU 置零；采样溢出通过 status=ADCW_HAL 报告。
    uint32_t request_id = 0;
    std::array<uint32_t, ADCW_SAMPLE_MAX> samples{};
};

// 应用通过 Configure/ADCx_Read 使用 ADC；接收入口将 MCU 回包交给 ADCx_Call_Back。
class ADC
{
public:
    // 配置与采集同步等待 MCU 回包；返回 0 或负的 ADCW_Status。
    // YAML 解析及枚举查找异常向调用方传播。
    // ADC_N 按列表顺序配置实例，首个失败即返回。
    int Get_Yaml_And_Init(const shared_ptr<Device_class> &device, const YAML::Node &nodes);
    // ADC_S 固定配置 ADC1 / ADC_CHANNEL_2 / PF11，使用 Rank 1。
    int Init_Simple(const shared_ptr<Device_class> &device, const YAML::Node &node);
    // 提交完整配置并等待回执；MCU 按配置重新初始化实例，不支持局部更新。
    int Configure(const shared_ptr<Device_class> &device, const ADC_InitConfig &config,
                  uint32_t timeout_ms = 1000);
    // 软件触发一批规则序列转换并等待结果；重复采样需由应用再次调用。
    int ADCx_Read(const shared_ptr<Device_class> &device, uint8_t instance, ADC_Result &result,
                  uint32_t timeout_ms = 1000);
    // 仅匹配当前请求且采样成功时通知，在接收线程执行。
    // 回调须快速返回，不得同步等待 ADC 请求或向外抛异常。
    void SetDataCallback(std::function<void(const ADC_Result &)> callback);
    // 接收分发入口；0 表示 ADC 帧（含未匹配回包），1 表示交给其他插件。
    int ADCx_Call_Back(const volatile u8 *frame);

private:

    // 同一 ADC 对象同时等待一个请求，接收线程填写 result 并置 completed。
    struct PendingRequest
    {
        uint32_t request_id = 0;
        uint16_t device_id = 0;
        uint16_t flag = 0;
        uint8_t command = 0;
        uint8_t operation = 0;
        uint8_t instance = 0;
        bool completed = false;
        ADC_Result result;
    };

    // 请求锁串行化同步调用；状态锁只保护收发线程共享的数据。
    std::mutex request_mutex_;
    std::mutex state_mutex_;
    std::condition_variable reply_received_;
    uint32_t next_request_id_ = 0;
    PendingRequest pending_;
    std::function<void(const ADC_Result &)> data_callback_;

    // 两种包共用的发送和等待流程，调用方持有 request_mutex_。
    template <class Packet>
    int Exchange(const shared_ptr<Device_class> &device, Packet &packet, uint8_t operation,
                 uint32_t timeout_ms, ADC_Result &result);
};

#endif
