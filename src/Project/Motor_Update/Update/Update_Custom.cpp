#include "syst.hpp"
#include "UDP.hpp"
#include "HARDWARE_TOP.hpp"
#include "Custom_TOP.hpp"
#include "Motor_TOP.hpp"
#include "Switch_Board.hpp"
#include "Auto_Set_Id.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstring>
#include <math.h>
#include "unistd.h"
#include <filesystem>
#include "GPIO.hpp"
#include <fstream>
#include <vector>
using namespace std;


#define CS_CLK_PORT GPIOE
#define CS_CLK_PIN GPIO_PIN_15
#define SPI_MOSI_PORT GPIOB
#define SPI_MOSI_PIN GPIO_PIN_10
#define SPI_MISO_PORT GPIOB
#define SPI_MISO_PIN GPIO_PIN_14
#define SW_PORT GPIOC
#define SW_PIN GPIO_PIN_13
#define LP_POWER_ON_PORT GPIOB
#define LP_POWER_ON_PIN GPIO_PIN_0
#define NOR_POWER_ON_PORT GPIOA
#define NOR_POWER_ON_PORT_PIN GPIO_PIN_4

#define CMD_HANDSHAKE 0x01 // 握手命令
#define CMD_ERASE 0x02     // 擦除APP分区命令
#define CMD_DATA 0x03      // 固件数据包下发命令
#define CMD_VERIFY 0x04    // 固件CRC校验命令
#define CMD_JUMP 0x05      // 跳转APP运行命令

#define PACKET_SIZE 128                    // 单个数据包负载字节长度
#define FLASH_BOOT_START_ADDR 0x08000000UL // Bootloader起始地址(不可擦写)
#define FLASH_APP_START_ADDR 0x08004000UL  // APP固件分区起始地址
#define FLASH_APP_END_ADDR 0x08008000UL    // APP固件分区结束地址

#define HANDSHAKE_RETRY 20         // 每这么多次握手失败提醒一次检查电机
#define HANDSHAKE_INTERVAL 100000  // 两次握手之间总线保持空闲(us)，避免字节粘在一起
#define SPI_BIT_DELAY_US 20         // SPI_ReadByte 仍用 usleep，OTA 发送路径不用
#define SPI_SETUP_US 30             // MOSI 建立时间(us)，忙等
#define SPI_CLK_HIGH_US 40          // CLK 高电平宽度(us)，忙等
#define SPI_CLK_LOW_US 50           // CLK 低电平保持(us)，忙等
#define HANDSHAKE_ACK_TIMEOUT_MS 200 // 单次握手 ACK 窗口。30ms 经常先超时，下一字节 0x01 会把已握手的 boot 打出升级状态
#define NOR_BOOT_WAIT_US 500000     // 进入正常供电后先空闲，等 boot 起来再发第一个字节
#define CMD_GAP_US 15000            // 命令 ACK 之后、发送后续数据之前的间隔(us)
#define ACK_DEBOUNCE_CNT 3          // ACK信号防抖计数
#define BYTE_RETRY 50               // 单命令字节最大重试次数
#define PKT_RETRY 30                // 数据包最大重试次数
#define CMD_ACK_TIMEOUT_MS 400      // 普通命令ACK超时(ms)
#define ERASE_ACK_TIMEOUT_MS 3000   // 擦除命令ACK超时(ms)
#define PKT_ACK_TIMEOUT_MS 800      // 数据包ACK超时(ms)


Robot_Hardware *Test_Robot;
shared_ptr<Device_class> IO_Board;
Main_B *IO_Board_Control;

void Get_Buttons_State(const shared_ptr<Device_class> &Device, u8 *Res);
u8 R_IO[32];
u8 W_IO[32];

bool ReadBinFile(const string &file_path, vector<uint8_t> &out_buf);
string ReadBinPathFromYaml(const string &yaml_path);
uint16_t CalcCRC16(const uint8_t *data, uint32_t len);

void SPI_SetCS_CLK(bool level);                                                 // 设置软件SPI时钟引脚电平
void SPI_SetMOSI(bool level);                                                   // 设置软件SPI MOSI引脚电平
bool SPI_ReadMISO(void);                                                        // 读取MISO引脚电平
void SPI_WaitUs(uint32_t us);                                                   // 微秒级延时
static void SPI_DelayUs(uint32_t us);                                           // 忙等微秒，OTA 位时序用这个
static void SPI_ShiftOutByte(uint8_t tx_byte);                                  // 命令和负载共用的一位时序
bool CustomSPI_SendByte_WithAck(uint8_t tx_byte, int timeout_ms);               // SPI发送1字节并等待ACK
uint8_t SPI_ReadByte(void);                                                     // SPI读取1字节
bool SendByteRetry(uint8_t b, int max_retry, int timeout_ms);                   // 带重试的单字节命令发送
bool SPI_SendByte_WithoutAck(uint8_t tx_byte);                                  // SPI发送1字节，不等待ACK(传输负载数据)
bool SendDataPacket_New(uint8_t *data_buf, uint16_t data_len, uint32_t offset); // 发送固件数据包
bool SendVerify(uint32_t image_len, uint16_t image_crc);                        // 下发校验命令，通知boot校验固件
bool WaitMisoLevel(bool level, int debounce, int timeout_ms);                   // 等待MISO到达指定电平(带防抖+超时)
bool WaitAckDone(int timeout_ms);                                               // 等待boot返回ACK应答信号
bool AppOffsetValid(uint32_t offset, uint16_t len);                             // 校验APP偏移地址合法性
static uint8_t g_nor_mode = 0;
static void Power_Write(GPIO_TypeDef *port, u32 pin, bool high);
static bool Power_ReadSW(void);
static void Power_Apply(bool nor);
void Power_Init(void);
void Power_Poll(void);

#ifndef HAVE_ROS
enum class OtaStatus
{
    Ok,
    Failed,
    Cancelled
};
static void PrintOtaProgress(uint32_t sent, uint32_t total, uint32_t pkt_i, uint32_t pkt_n, const char *state);
static bool ConsumeButtonPress(bool &last_high);
static void WaitButtonPress(void);
static OtaStatus RunMotorOtaOnce(const vector<uint8_t> &bin_buffer);
static OtaStatus RunMotorOta(const vector<uint8_t> &bin_buffer);
#endif

#ifndef HAVE_ROS
int main(int argc, char *argv[])
#else
void ROS_Get_GPIOx_Status()
{
    if (IO_Board_Control == nullptr || IO_Board == nullptr)
    {
        cerr << "Error: IO_Board Device not initialized before calling ROS_Get_GPIOx_Status!" << endl;
        return;
    }
    Get_Buttons_State(IO_Board, R_IO);
}
int hardware_init(const string &ADDR, const string &Config)
#endif
{

    Test_Robot = new Robot_Hardware();

    Test_Robot->Add_Device_Type(Switch_Board_Type,
                                Switch_Board_Device_Init,
                                Switch_Board_Device_CallBack_F,
                                Switch_Board_Device_Delete_F);

#ifndef HAVE_ROS

    filesystem::path exe_path = filesystem::canonical("/proc/self/exe");
    filesystem::path dir_path = exe_path.parent_path();
    std::cout << "exe dir: " << dir_path << std::endl;
    string ADDR = dir_path.string() + "/../config/YAML/Update_Config/TOP.yaml";
    string Config = "None";
#endif

    if (Test_Robot->Init_TOP(ADDR, Config) != 0)
    {
        cout << "Init_ERR" << endl;
        return -1;
    }

    IO_Board = Test_Robot->Get_Device_For_Name("IO_Board");
    IO_Board_Control = static_cast<Main_B *>(Test_Robot->Get_Control_Class(IO_Board));
    Power_Init();


    // 初始化软件SPI时钟引脚，推挽输出
    GPIO_InitTypeDef gpio_spi_out = {};
    gpio_spi_out.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_spi_out.Pull = GPIO_NOPULL;
    gpio_spi_out.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_spi_out.Pin = CS_CLK_PIN;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, CS_CLK_PORT, &gpio_spi_out);
    SPI_SetCS_CLK(false);

    // 初始化MOSI引脚，推挽输出
    gpio_spi_out.Pin = SPI_MOSI_PIN;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, SPI_MOSI_PORT, &gpio_spi_out);
    SPI_SetMOSI(false);

    // 初始化MISO引脚，下拉输入
    GPIO_InitTypeDef gpio_miso_in = {};
    gpio_miso_in.Mode = GPIO_MODE_INPUT;
    gpio_miso_in.Pull = GPIO_PULLDOWN;
    gpio_miso_in.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_miso_in.Pin = SPI_MISO_PIN;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, SPI_MISO_PORT, &gpio_miso_in);

    vector<uint8_t> bin_buffer; // bin固件存放buffer
    string bin_path;
    bool bin_load_ok = false;

    // 从yaml读取bin文件路径，读取失败使用默认路径
    string config_bin_path = ReadBinPathFromYaml(ADDR);
    if (config_bin_path.empty())
    {
        cout << "Warning: bin_path not found in yaml, use default path" << endl;
        bin_path = "./motor_app.bin";
    }
    else
    {
        bin_path = config_bin_path;
        cout << "Read bin_path from yaml: " << bin_path << endl;
    }
    // 读取bin固件到内存buffer
    bin_load_ok = ReadBinFile(bin_path, bin_buffer);
    if (!bin_load_ok)
        cout << "Warning: bin file load failed, skip update" << endl;
    else
        cout << "Bin file load success, size = " << bin_buffer.size() << " bytes" << endl;

#ifndef HAVE_ROS

    if (!bin_load_ok)
    {
        cout << "firmware not loaded, abort" << endl;
        return 0;
    }
    // 校验bin文件长度，不能超过APP分区总大小
    if (bin_buffer.size() == 0 || bin_buffer.size() > (FLASH_APP_END_ADDR - FLASH_APP_START_ADDR))
    {
        cout << "bin size invalid for APP region, abort" << endl;
        return 0;
    }

    cout << endl
         << "===== OTA station =====" << endl;
    cout << "BOOT  0x" << hex << FLASH_BOOT_START_ADDR << " (do not touch)" << dec << endl;
    cout << "APP   0x" << hex << FLASH_APP_START_ADDR << " ~ 0x" << FLASH_APP_END_ADDR << dec << endl;
    cout << "addr field is OFFSET, first pkt = 00 00 00 00, flash = 0x08004000 + offset" << endl;
    cout << "流程: 关机状态下插上电机 -> 按一下按钮进入正常模式并开始OTA -> 完成后自动回到关机状态 -> 更换电机 -> 再按一下按钮开始下一台" << endl;

    SPI_SetCS_CLK(false);
    SPI_SetMOSI(false);

    int cycle = 0;
    while (true)
    {
        cycle++;
        if (g_nor_mode)
            Power_Apply(false);

        cout << endl
             << "===== cycle " << cycle << " =====" << endl;
        if (cycle == 1)
            cout << "当前关机状态。请先插上电机，再按一下按钮进入正常模式并开始 OTA。" << endl;
        else
            cout << "当前关机状态。请换上电机，再按一下按钮进入正常模式并开始 OTA。" << endl;

        WaitButtonPress();
        Power_Apply(true);
        usleep(NOR_BOOT_WAIT_US);

        OtaStatus st = RunMotorOta(bin_buffer);
        SPI_SetMOSI(false);
        SPI_SetCS_CLK(false);

        if (st == OtaStatus::Ok)
        {
            cout << "本台 OTA 完成，自动回到关机状态。请更换电机，然后按一下按钮开始下一台。" << endl;
        }
        else if (st == OtaStatus::Cancelled)
        {
            cout << "本次已取消。即将回到关机状态，换好电机后再按按钮开始。" << endl;
        }
        else
        {
            cout << "本台 OTA 失败。按一下按钮切换到关机状态，然后可重试或更换电机。" << endl;
            WaitButtonPress();
        }

        Power_Apply(false);
        cout << "已回到关机状态，可以拔插电机。" << endl;
    }
#endif
    return 0;
}

bool AppOffsetValid(uint32_t offset, uint16_t len)
{
    if ((offset % PACKET_SIZE) != 0)
        return false;
    if (offset >= (FLASH_APP_END_ADDR - FLASH_APP_START_ADDR))
        return false;
    if ((uint32_t)len > (FLASH_APP_END_ADDR - FLASH_APP_START_ADDR - offset))
        return false;
    return true;
}

bool ReadBinFile(const string &file_path, vector<uint8_t> &out_buf)
{
    ifstream fs(file_path, ios::in | ios::binary);
    if (!fs.is_open())
        return false;
    out_buf.assign(istreambuf_iterator<char>(fs), istreambuf_iterator<char>());
    fs.close();
    return true;
}

string ReadBinPathFromYaml(const string &yaml_path)
{
    ifstream fs(yaml_path);
    if (!fs.is_open())
    {
        cerr << "Open yaml failed: " << yaml_path << endl;
        return "";
    }
    string line;
    while (getline(fs, line))
    {
        size_t pos = line.find("bin_path:");
        if (pos != string::npos)
        {
            string path = line.substr(pos + string("bin_path:").length());
            size_t start = path.find_first_not_of(" \t\"");
            size_t end = path.find_last_not_of(" \t\"");
            if (start != string::npos && end != string::npos)
            {
                fs.close();
                return path.substr(start, end - start + 1);
            }
        }
    }
    fs.close();
    return "";
}

uint16_t CalcCRC16(const uint8_t *data, uint32_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < len; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void SPI_SetCS_CLK(bool level)
{
    if (level)
        IO_Board_Control->m_GPIO.GPIOx_Write(IO_Board, CS_CLK_PORT, CS_CLK_PIN, GPIO_PIN_SET);
    else
        IO_Board_Control->m_GPIO.GPIOx_Write(IO_Board, CS_CLK_PORT, CS_CLK_PIN, GPIO_PIN_RESET);
}

void SPI_SetMOSI(bool level)
{
    if (level)
        IO_Board_Control->m_GPIO.GPIOx_Write(IO_Board, SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_SET);
    else
        IO_Board_Control->m_GPIO.GPIOx_Write(IO_Board, SPI_MOSI_PORT, SPI_MOSI_PIN, GPIO_PIN_RESET);
}

bool SPI_ReadMISO(void)
{
    IO_Board_Control->m_GPIO.GPIOx_Read(IO_Board, SPI_MISO_PORT, SPI_MISO_PIN, 100);
    uint8_t val = IO_Board_Control->m_GPIO.Get_GPIOx_Value(SPI_MISO_PORT, SPI_MISO_PIN);
    return (val != 0);
}

void SPI_WaitUs(uint32_t us)
{
    usleep(us);
}

static void SPI_DelayUs(uint32_t us)
{
    if (us == 0)
        return;
    auto end = chrono::steady_clock::now() + chrono::microseconds(us);
    while (chrono::steady_clock::now() < end)
    {
    }
}

static void SPI_ShiftOutByte(uint8_t tx_byte)
{
    for (int i = 7; i >= 0; i--)
    {
        SPI_SetMOSI((tx_byte >> i) & 0x01);
        SPI_DelayUs(SPI_SETUP_US);
        SPI_SetCS_CLK(true);
        SPI_DelayUs(SPI_CLK_HIGH_US);
        SPI_SetCS_CLK(false);
        SPI_DelayUs(SPI_CLK_LOW_US);
    }
}

bool CustomSPI_SendByte_WithAck(uint8_t tx_byte, int timeout_ms)
{
    SPI_ShiftOutByte(tx_byte);
    return WaitAckDone(timeout_ms);
}

bool SendByteRetry(uint8_t b, int max_retry, int timeout_ms)
{
    for (int r = 0; r < max_retry; r++)
    {
        if (CustomSPI_SendByte_WithAck(b, timeout_ms))
            return true;
        usleep(5000);
    }
    return false;
}

uint8_t SPI_ReadByte(void)
{
    uint8_t rx_data = 0;
    SPI_WaitUs(200);
    for (int i = 7; i >= 0; i--)
    {
        SPI_SetCS_CLK(true);
        SPI_WaitUs(SPI_BIT_DELAY_US);
        uint8_t bit = SPI_ReadMISO() ? 1 : 0;
        rx_data = (rx_data << 1) | bit;
        if (i != 0)
        {
            SPI_SetCS_CLK(false);
            SPI_WaitUs(SPI_BIT_DELAY_US);
        }
    }
    SPI_SetCS_CLK(false);
    SPI_WaitUs(SPI_BIT_DELAY_US);
    return rx_data;
}

bool SPI_SendByte_WithoutAck(uint8_t tx_byte)
{
    SPI_ShiftOutByte(tx_byte);
    return true;
}

bool SendDataPacket_New(uint8_t *data_buf, uint16_t data_len, uint32_t offset)
{
    uint8_t frame[4 + PACKET_SIZE];
    if (data_len != PACKET_SIZE)
        return false;
    if (!AppOffsetValid(offset, data_len))
        return false;

    frame[0] = (uint8_t)(offset & 0xFF);
    frame[1] = (uint8_t)((offset >> 8) & 0xFF);
    frame[2] = (uint8_t)((offset >> 16) & 0xFF);
    frame[3] = (uint8_t)((offset >> 24) & 0xFF);
    memcpy(frame + 4, data_buf, data_len);

    if (!CustomSPI_SendByte_WithAck(CMD_DATA, CMD_ACK_TIMEOUT_MS))
    {
        cout << "  CMD_DATA ack fail, offset " << offset << endl;
        return false;
    }
    usleep(CMD_GAP_US);
    for (uint16_t i = 0; i < 4 + data_len; i++)
        SPI_SendByte_WithoutAck(frame[i]);
    uint16_t crc = CalcCRC16(frame, 4 + data_len);
    SPI_SendByte_WithoutAck((uint8_t)((crc >> 8) & 0xFF));
    SPI_SendByte_WithoutAck((uint8_t)(crc & 0xFF));
    if (!WaitAckDone(PKT_ACK_TIMEOUT_MS))
    {
        cout << "  payload ack fail, offset " << offset
             << " flash 0x" << hex << (FLASH_APP_START_ADDR + offset) << dec
             << " crc 0x" << hex << crc << dec << endl;
        return false;
    }
    return true;
}

bool SendVerify(uint32_t image_len, uint16_t image_crc)
{
    uint8_t frame[6];
    frame[0] = (uint8_t)(image_len & 0xFF);
    frame[1] = (uint8_t)((image_len >> 8) & 0xFF);
    frame[2] = (uint8_t)((image_len >> 16) & 0xFF);
    frame[3] = (uint8_t)((image_len >> 24) & 0xFF);
    frame[4] = (uint8_t)((image_crc >> 8) & 0xFF);
    frame[5] = (uint8_t)(image_crc & 0xFF);

    if (!CustomSPI_SendByte_WithAck(CMD_VERIFY, CMD_ACK_TIMEOUT_MS))
        return false;
    usleep(CMD_GAP_US);
    for (int i = 0; i < 6; i++)
        SPI_SendByte_WithoutAck(frame[i]);
    return WaitAckDone(PKT_ACK_TIMEOUT_MS);
}

bool WaitMisoLevel(bool level, int debounce, int timeout_ms)
{
    auto t0 = chrono::steady_clock::now();
    int cnt = 0;
    while (true)
    {
        if (SPI_ReadMISO() == level)
        {
            if (++cnt >= debounce)
                return true;
        }
        else
        {
            cnt = 0;
        }
        if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - t0).count() >= timeout_ms)
            return false;
        usleep(200);
    }
}

bool WaitAckDone(int timeout_ms)
{
    if (!WaitMisoLevel(true, ACK_DEBOUNCE_CNT, timeout_ms))
        return false;
    if (!WaitMisoLevel(false, ACK_DEBOUNCE_CNT, timeout_ms))
        return false;
    return true;
}

static void Power_Write(GPIO_TypeDef *port, u32 pin, bool high)
{
    IO_Board_Control->m_GPIO.GPIOx_Write(IO_Board, port, pin,
                                         high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static bool Power_ReadSW(void)
{
    IO_Board_Control->m_GPIO.GPIOx_Read(IO_Board, SW_PORT, SW_PIN, 100);
    usleep(2000);
    return IO_Board_Control->m_GPIO.Get_GPIOx_Value(SW_PORT, SW_PIN) != 0;
}

static void Power_Apply(bool nor)
{
    if (nor)
    {
        Power_Write(NOR_POWER_ON_PORT, NOR_POWER_ON_PORT_PIN, true);
        Power_Write(LP_POWER_ON_PORT, LP_POWER_ON_PIN, false);
        cout << "power : NOR" << endl;
    }
    else
    {
        // 关机：PA4 和 PB0 都拉低
        Power_Write(NOR_POWER_ON_PORT, NOR_POWER_ON_PORT_PIN, false);
        Power_Write(LP_POWER_ON_PORT, LP_POWER_ON_PIN, false);
        cout << "power : OFF" << endl;
    }
    g_nor_mode = nor ? 1 : 0;
}

void Power_Init(void)
{
    GPIO_InitTypeDef gpio = {};
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    gpio.Pin = LP_POWER_ON_PIN;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, LP_POWER_ON_PORT, &gpio);
    gpio.Pin = NOR_POWER_ON_PORT_PIN;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, NOR_POWER_ON_PORT, &gpio);

    GPIO_InitTypeDef sw = {};
    sw.Mode = GPIO_MODE_INPUT;
    sw.Pull = GPIO_PULLUP;
    sw.Speed = GPIO_SPEED_FREQ_LOW;
    sw.Pin = SW_PIN;
    IO_Board_Control->m_GPIO.GPIOx_Init(IO_Board, SW_PORT, &sw);

    Power_Apply(false);
}

// 只从关机状态切到正常供电。工位循环不调用它，避免 OTA 写到一半被按钮切断电源。
void Power_Poll(void)
{
    static bool last_high = true;
    bool new_high = Power_ReadSW();

    if (last_high && !new_high)
    {
        usleep(30000);
        if (!Power_ReadSW() && !g_nor_mode)
        {
            Power_Apply(true);
        }
    }
    last_high = new_high;
}

#ifndef HAVE_ROS

static void PrintOtaProgress(uint32_t sent, uint32_t total, uint32_t pkt_i, uint32_t pkt_n, const char *state)
{
    unsigned pct = (total == 0) ? 100u : (unsigned)(((unsigned long long)sent * 100ull) / total);
    const int width = 20;
    int filled = (int)((pct * (unsigned)width) / 100u);
    if (filled > width)
        filled = width;

    cout << dec << "[3/5] " << pct << "% [";
    for (int i = 0; i < width; ++i)
        cout << (i < filled ? '#' : '-');
    cout << "] pkt " << pkt_i << "/" << pkt_n
         << "  " << sent << "/" << total << " bytes  " << state << endl;
}

static bool ConsumeButtonPress(bool &last_high)
{
    bool now_high = Power_ReadSW();
    if (last_high && !now_high)
    {
        usleep(30000);
        if (!Power_ReadSW())
        {
            while (!Power_ReadSW())
                usleep(5000);
            usleep(20000);
            last_high = true;
            return true;
        }
        now_high = Power_ReadSW();
    }
    last_high = now_high;
    return false;
}

static void WaitButtonPress(void)
{
    bool last_high = Power_ReadSW();
    while (!ConsumeButtonPress(last_high))
        usleep(5000);
}

static void BusIdle(void)
{
    SPI_SetMOSI(false);
    SPI_SetCS_CLK(false);
}

static OtaStatus RunMotorOtaOnce(const vector<uint8_t> &bin_buffer)
{
    BusIdle();

    uint8_t bl_ver = 0;
    bool hs = false;
    bool sw_high = Power_ReadSW();
    int hs_try = 0;
    cout << "NOR on, waiting for motor" << endl;
    cout << "握手中若电机无应答，按一下按钮可放弃本次并回到关机状态。" << endl;

    while (!hs)
    {
        if (ConsumeButtonPress(sw_high))
        {
            cout << "[1/5] handshake cancelled by button" << endl;
            return OtaStatus::Cancelled;
        }
        if (CustomSPI_SendByte_WithAck(CMD_HANDSHAKE, HANDSHAKE_ACK_TIMEOUT_MS))
        {
            bl_ver = 0x88;
            hs = true;
            break;
        }
        hs_try++;
        BusIdle();
        usleep(HANDSHAKE_INTERVAL);
        if (hs_try == 1 || (hs_try % 5) == 0)
            cout << "[1/5] handshake waiting, try " << hs_try << endl;
        if ((hs_try % HANDSHAKE_RETRY) == 0)
            cout << "[1/5] still no motor after " << hs_try
                 << " tries. Check the plug, or press the button to turn power off." << endl;
    }
    cout << "[1/5] handshake ok (ver ref: 0x" << hex << (int)bl_ver << dec << ")" << endl;
    BusIdle();
    usleep(CMD_GAP_US);
    cout << "[2/5] erasing APP ..." << endl;
    if (!CustomSPI_SendByte_WithAck(CMD_ERASE, ERASE_ACK_TIMEOUT_MS))
    {
        cout << "[2/5] erase failed, abort" << endl;
        return OtaStatus::Failed;
    }
    cout << "[2/5] erase ok (APP pages only)" << endl;

    uint32_t total = (uint32_t)bin_buffer.size();
    uint32_t offset = 0;
    uint32_t pkt_total = (total + PACKET_SIZE - 1) / PACKET_SIZE;
    uint32_t pkt_index = 0;
    cout << "[3/5] transfer start, packets: " << pkt_total << ", bytes: " << total << endl;

    while (offset < total)
    {
        uint16_t have = (uint16_t)((total - offset) > PACKET_SIZE ? PACKET_SIZE : (total - offset));
        uint8_t pkt_buf[PACKET_SIZE] = {0};
        memcpy(pkt_buf, &bin_buffer[offset], have);

        if (!AppOffsetValid(offset, PACKET_SIZE))
        {
            cout << "illegal APP offset " << offset << ", abort" << endl;
            return OtaStatus::Failed;
        }

        uint32_t pkt_no = pkt_index + 1;
        PrintOtaProgress(offset, total, pkt_no, pkt_total, "sending");

        bool pkt_ok = false;
        for (int pr = 0; pr < PKT_RETRY; pr++)
        {
            if (pr > 0)
                PrintOtaProgress(offset, total, pkt_no, pkt_total, "retry");
            if (SendDataPacket_New(pkt_buf, PACKET_SIZE, offset))
            {
                pkt_ok = true;
                break;
            }
            cout << "pkt retry " << (pr + 1)
                 << " offset: " << offset
                 << " flash: 0x" << hex << (FLASH_APP_START_ADDR + offset) << dec << endl;
            SPI_SetMOSI(false);
            SPI_SetCS_CLK(false);
            usleep(80000);
        }
        if (!pkt_ok)
        {
            cout << "packet failed, offset: " << offset
                 << " flash: 0x" << hex << (FLASH_APP_START_ADDR + offset) << dec << endl;
            return OtaStatus::Failed;
        }

        offset += have;
        pkt_index++;
        PrintOtaProgress(offset, total, pkt_index, pkt_total, "ok");
    }
    cout << "[3/5] transfer ok" << endl;

    uint16_t local_crc = CalcCRC16(bin_buffer.data(), (uint32_t)bin_buffer.size());
    bool ver_ok = false;
    for (int r = 0; r < 3; r++)
    {
        if (SendVerify((uint32_t)bin_buffer.size(), local_crc))
        {
            ver_ok = true;
            break;
        }
        usleep(20000);
    }
    if (!ver_ok)
    {
        cout << "[4/5] verify failed, local=0x" << hex << local_crc << dec << endl;
        return OtaStatus::Failed;
    }
    cout << "[4/5] verify ok, crc=0x" << hex << local_crc << dec << endl;

    if (!SendByteRetry(CMD_JUMP, BYTE_RETRY, CMD_ACK_TIMEOUT_MS))
    {
        cout << "[5/5] jump ack failed" << endl;
        return OtaStatus::Failed;
    }
    cout << "[5/5] jump ok, update done" << endl;
    cout << "===== OTA End =====" << endl;
    return OtaStatus::Ok;
}

static OtaStatus RunMotorOta(const vector<uint8_t> &bin_buffer)
{
    OtaStatus st = RunMotorOtaOnce(bin_buffer);
    if (st != OtaStatus::Failed)
        return st;

    cout << "切断电机电源，重新上电后再握手、再擦除一次。" << endl;
    BusIdle();
    Power_Apply(false);
    usleep(400000);
    Power_Apply(true);
    usleep(NOR_BOOT_WAIT_US);
    return RunMotorOtaOnce(bin_buffer);
}

#endif