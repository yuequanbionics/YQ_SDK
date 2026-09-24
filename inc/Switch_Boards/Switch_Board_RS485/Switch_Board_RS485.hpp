
#ifndef SDK_SWITCH_BOARD_RS485_HPP
#define SDK_SWITCH_BOARD_RS485_HPP

#include <array>
#include <atomic>
#include <cstdint>
#include <string>
#include <thread>

#include "HARDWARE_TOP.hpp"
#include "rs485_driver.hpp"    // RS485 串口驱动接口(打开/关闭/收发/刷新)
#include "rs485_protocol.hpp"  // RS485 通信协议(打包/解包/命令构建)

/* ===================== 设备类型标识 ===================== */
#define Switch_Board_RS485_Type "Switch_Board_Custom_RS485"

#define Switch_Board_RS485_Device_Init [](shared_ptr<Device_class> Device, YAML::Node* Node) -> int { \
    Main_B_RS485* One_Main_B = new Main_B_RS485();                                                    \
    Device->Device_Private_Class = (void*)One_Main_B;                                                 \
    if (Node != nullptr)                                                                              \
        return One_Main_B->Get_Main_B_RS485_Device_Data_From_Yaml_And_Init(Device, *Node);            \
    else                                                                                              \
        return 0;                                                                                     \
}

#define Switch_Board_RS485_Device_CallBack_F [](shared_ptr<Device_class> Device, u8* Msg) -> int { \
    return ((Main_B_RS485*)Device->Device_Private_Class)->Main_B_RS485_Top_Frame_Analyze(Msg);     \
}

#define Switch_Board_RS485_Device_Delete_F [](void* Device_Private_Class) { \
    delete ((Main_B_RS485*)Device_Private_Class);                           \
    Device_Private_Class = nullptr;                                         \
}

/* ===================== 配置默认值 ===================== */
#define RS485_DEFAULT_DEVICE "/dev/ttyUSB0"  // 默认串口设备
#define RS485_DEFAULT_BAUDRATE 115200        // 默认波特率
#define RS485_DEFAULT_DATABITS 8             // 默认数据位
#define RS485_DEFAULT_STOPBITS 1             // 默认停止位
#define RS485_DEFAULT_PARITY 0               // 默认无校验
#define RS485_DEFAULT_FLOWCTRL 0             // 默认无流控
#define RS485_DEFAULT_TIMEOUT 100            // 默认接收超时(ms)

class Main_B_RS485 : private Robot_Hardware {
   public:
    Main_B_RS485();
    ~Main_B_RS485();

    /**
     * @brief 从 YAML 节点中读取 RS485 相关参数, 并完成驱动初始化
     */
    int Get_Main_B_RS485_Device_Data_From_Yaml_And_Init(shared_ptr<Device_class> Device, YAML::Node One_Node);

    /**
     * @brief 协议帧顶层解析入口
     */
    int Main_B_RS485_Top_Frame_Analyze(volatile u8* RS485_Frame);

    /* ===================== 公共方法 ===================== */

    /**
     * @brief 判断 RS485 设备是否已打开
     * @return true 设备已打开, false 设备未打开
     */
    bool isOpen() const { return m_rs485_dev != nullptr; }

    /**
     * @brief 获取当前默认 device_id (8 字节, 小端)
     * @return 指向内部 8 字节缓冲区的指针
     */
    const uint8_t* getDefaultDeviceId() const;

    /**
     * @brief 发送一帧协议数据 (pack + flush + send)
     * @param frame 协议帧 (cmd / data / data_length 字段会被使用)
     * @return 实际发送字节数, < 0 表示失败
     */
    int sendFrame(protocol_frame_t* frame);

    /**
     * @brief 接收一帧协议数据 (recv + unpack)
     * @param[out] frame      解包后的协议帧
     * @param[in]  timeout_ms 接收超时时间 (毫秒)
     * @return > 0 接收字节数, 0 超时, < 0 失败
     */
    int recvFrame(protocol_frame_t* frame, uint32_t timeout_ms);

    /**
     * @brief 获取协议层操作接口
     */
    const protocol_ops_t* getProtocolOps() const;

   private:
    /* ===================== 内部: 接收线程管理 ===================== */
    void startRs485ReceiveThread(shared_ptr<Device_class> Device);
    void stopRs485ReceiveThread();
    void rs485RecvThread(shared_ptr<Device_class> Device) const;

    std::thread m_recv_thread;
    std::atomic<bool> m_running{false};
    rs485_device_t* m_rs485_dev;
    serial_config_t m_serial_cfg;
    std::string m_device_path;
};

int Send_F_RS485(shared_ptr<Device_class> Device, u8* Data);

/**
 * @brief RS485 协议帧解析与分发入口
 */
int RS485_Data_Analysis(shared_ptr<Device_class> Device, protocol_frame_t* frame);

#endif  // SDK_SWITCH_BOARD_RS485_HPP
