//
// Created by lwz on 2026/4/14.
//
#include "../../../inc/Z_SYS_SRC/Socket_CAN/Socket_CAN.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cerrno>
#include "poll.h"

using namespace std;

Socket_CanFD::Socket_CanFD()
    : m_socket(-1), m_initialized(false)
{}

Socket_CanFD::~Socket_CanFD()
{
    close();
}


/**
 * @brief Linux Socket CANFD 配置: can 名和波特率
 * @param can_instance can0 或 can1
 * @param bitrate 仲裁域波特率, 单位 k Bd
 * @param d_bitrate 数据域波特率
 * @param sample_point 仲裁域采样点
 * @param d_sample_point 数据域采样点
 * @return
 */
bool Socket_CanFD::configCanFD(const char* can_instance,
                               const uint32_t bitrate,
                               const uint32_t d_bitrate,
                               float sample_point = 0.875f,    // 仲裁采样点默认87.5%
                               float d_sample_point = 0.75f)    // FD采样点默认75%
{
    char cmd[256];

    cout << "=== 配置 " << can_instance << " ===" << endl;
    cout << "仲裁波特率: " << bitrate / 1000 << "k" << endl;
    cout << "数据波特率: " << d_bitrate / 1000 << "k" << endl;
    cout << "仲裁采样点: " << sample_point * 100 << "%" << endl;
    cout << "数据采样点: " << d_sample_point * 100 << "%" << endl;

    // 1. down
    snprintf(cmd, sizeof(cmd), "sudo ip link set %s down", can_instance);
    system(cmd);

    // 2. 配置 CAN FD + 采样点
    snprintf(cmd, sizeof(cmd),
        "sudo ip link set %s type can bitrate %d sample-point %.3f dbitrate %d dsample-point %.3f fd on berr-reporting off listen-only off restart-ms 0",
        can_instance, bitrate, sample_point, d_bitrate, d_sample_point);
    system(cmd);

    // 3. up
    snprintf(cmd, sizeof(cmd), "sudo ip link set %s up", can_instance);
    system(cmd);

    cout << can_instance << " 配置完成!\n" << endl;

    // 配置完自动打印采样点
    getCanSamplePoint(can_instance);
    return true;
}


/**
 * @brief 读取并打印 CAN 接口的采样点 (sample-point / dsample-point)
 * @param can_instance 如 "can0"
 */
bool Socket_CanFD::getCanSamplePoint(const char* can_instance)
{
    char cmd[256];
    char buffer[1024];
    FILE* fp;

    // 拼接命令: ip -d link show can0
    snprintf(cmd, sizeof(cmd), "ip -details link show %s 2>/dev/null", can_instance);

    // 执行命令并读取输出
    fp = popen(cmd, "r");
    if (!fp) {
        cerr << "执行命令失败: " << cmd << endl;
        return false;
    }

    // 读取输出
    cout << "=== " << can_instance << " 采样点配置 ===" << endl;
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        // 查找并打印 仲裁段采样点
        if (strstr(buffer, "sample-point") != nullptr) {
            buffer[strcspn(buffer, "\n")] = 0;
            cout << buffer << endl;
        }
        // 查找并打印 FD 数据段采样点
        if (strstr(buffer, "dsample-point") != nullptr) {
            buffer[strcspn(buffer, "\n")] = 0;
            cout << buffer << endl;
        }
    }

    pclose(fp);
    return true;
}


/**
 * @brief 启动 CAN
 * @param can_instance
 * @return
 */
bool Socket_CanFD::openCanFD(const std::string& can_instance)
{
    // 1. 创建 CAN 原始套接字
    m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (m_socket < 0) {
        std::cerr << "CAN FD socket 创建失败" << std::endl;
        return false;
    }

    // ====================== 开启非阻塞模式 ======================
    const int flags = fcntl(m_socket, F_GETFL, 0);
    fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);
    // ==========================================================

    // 2. 必须启用 CAN FD 帧支持 (不开启只能收8字节 )
    const int can_fd_enable = 1;
    if (setsockopt(m_socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
                   &can_fd_enable, sizeof(can_fd_enable)) < 0) {
        std::cerr << "CAN FD setsockopt 启用失败" << std::endl;
        ::close(m_socket);
        m_socket = -1;
        return false;
                   }

    // 3. 获取 CAN 接口索引 (can0 / can1 )
    struct ifreq ifr{};
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, can_instance.c_str(), IFNAMSIZ - 1);

    if (ioctl(m_socket, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "CAN 接口索引获取失败: " << can_instance << std::endl;
        ::close(m_socket);
        m_socket = -1;
        return false;
    }

    // 4. 绑定到 CAN 总线
    struct sockaddr_can addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(m_socket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "CAN bind 绑定失败: " << can_instance << std::endl;
        ::close(m_socket);
        m_socket = -1;
        return false;
    }

    // 5. 初始化成功
    m_initialized = true;
    std::cout << "✅ CAN FD 初始化成功: " << can_instance << std::endl;
    return true;
}


/**
 * @brief 发 CAN 包
 * @param id canid
 * @param data can 数据
 * @param len 包长
 * @return
 */
bool Socket_CanFD::send(const uint32_t id, const uint8_t* data, const uint8_t len) const
{
    // 1. 基础合法性检查
    if (!m_initialized || m_socket < 0 || len == 0 || len > 64) {
        printf("send参数错误: initialized=%d socket=%d len=%d\n", m_initialized, m_socket, len);
        return false;
    }

    // 2. 完整清零CAN FD帧 (关键! 杜绝脏数据) 
    struct canfd_frame frame = {};

    // 3. 配置帧ID + 数据长度
    frame.can_id = id;
    frame.len    = len;

    // 4. 关键: 必须同时设置 FDF(声明CAN FD帧) + BRS(波特率切换)
    frame.flags  = 0x01; // CANFD_FDF | CANFD_BRS; 旧内核不支持 CANFD_FDF 宏, 改用兼容写法

    // 5. 拷贝数据
    memcpy(frame.data, data, len);

    // 6. 发送数据 + 错误打印 (定位失败原因 ) 
    ssize_t ret = write(m_socket, &frame, sizeof(frame));
    if (ret != sizeof(frame)) {
        // 打印系统错误码, 一眼定位问题
        printf("CAN FD发送失败! ret=%zd errno=%d: %s\n", ret, errno, strerror(errno));
        return false;
    }

    return true;
}

/**
 * @brief 接收 CAN FD 帧
 * @param id canid
 * @param data 数据
 * @param len 包长
 * @return
 */
bool Socket_CanFD::receive(uint32_t& id, uint8_t* data, uint8_t& len) const
{
    if (!m_initialized || m_socket < 0 || !data)
        return false;

    struct pollfd fds{};
    fds.fd = m_socket;
    fds.events = POLLIN;

    // 超时时间
    int poll_ret = poll(&fds, 1, 100);

    // poll 出错
    if (poll_ret < 0) {
        return false;
    }

    // 超时 → 没数据
    if (poll_ret == 0) {
        return false;
    }
    
    // 必须判断 POLLIN 事件 !!!
    if (!(fds.revents & POLLIN)) {
        return false;
    }

    // 真正有数据了，再 read
    struct canfd_frame frame{};
    const ssize_t n = read(m_socket, &frame, sizeof(frame));

    if (n != sizeof(struct canfd_frame)) {
        return false;
    }

    id  = frame.can_id;
    len = std::min(frame.len, static_cast<uint8_t>(64));
    memcpy(data, frame.data, len);

    return true;
}



// bool Socket_CanFD::receive(uint32_t& id, uint8_t* data, uint8_t& len) const
// {
//     if (!m_initialized || m_socket < 0 || !data)
//         return false;

//     // ================= 直接读，不用 poll!=================
//     struct canfd_frame frame{};
//     ssize_t n = read(m_socket, &frame, sizeof(frame));

//     if (n <= 0) {
//         return false;
//     }

//     id  = frame.can_id;
//     len = frame.len;
//     memcpy(data, frame.data, len);

//     return true;
// }


void Socket_CanFD::close()
{
    if (m_socket >= 0) {
        ::close(m_socket);
        m_socket = -1;
    }
    m_initialized = false;
}



