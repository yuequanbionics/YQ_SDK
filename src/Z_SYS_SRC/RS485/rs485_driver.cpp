/**
 * @file rs485_driver.cpp
 * @brief RS485 串口驱动实现
 */

#include "rs485_driver.hpp"

#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/**
 * @brief RS485设备句柄结构体
 */
struct rs485_device {
    int serial_fd;                           /**< 串口文件描述符 */
    serial_config_t config;                  /**< 串口配置 */
    uint8_t rx_buffer[RS485_RX_BUFFER_SIZE]; /**< 接收缓冲区 */
    pthread_mutex_t mutex;                   /**< 互斥锁,保证线程安全 */
};

/**
 * @brief 设置串口阻塞模式
 */
static int set_blocking(int fd, int blocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        return -1;
    }

    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }

    return fcntl(fd, F_SETFL, flags);
}

/**
 * @brief 配置串口参数
 */
static int configure_port(int fd, const serial_config_t* config) {
    struct termios options;

    if (tcgetattr(fd, &options) != 0) {
        return -1;
    }

    /* 设置波特率 */
    speed_t baudrate;
    switch (config->baudrate) {
        case 9600:
            baudrate = B9600;
            break;
        case 19200:
            baudrate = B19200;
            break;
        case 38400:
            baudrate = B38400;
            break;
        case 57600:
            baudrate = B57600;
            break;
        case 115200:
            baudrate = B115200;
            break;
        case 230400:
            baudrate = B230400;
            break;
        case 460800:
            baudrate = B460800;
            break;
        case 921600:
            baudrate = B921600;
            break;
        default:
            baudrate = B115200;
            break;
    }
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    /* 设置数据位 */
    options.c_cflag &= ~CSIZE;
    if (config->databits == 7) {
        options.c_cflag |= CS7;
    } else {
        options.c_cflag |= CS8;
    }

    /* 设置校验位 */
    if (config->parity == 1) {
        options.c_cflag |= PARENB | PARODD;
    } else if (config->parity == 2) {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
    } else {
        options.c_cflag &= ~PARENB;
    }

    /* 设置停止位 */
    options.c_cflag &= ~CSTOPB;
    if (config->stopbits == 2) {
        options.c_cflag |= CSTOPB;
    }

    /* 硬件流控制 */
    options.c_cflag &= ~CRTSCTS;
    if (config->flowctrl == 1) {
        options.c_cflag |= CRTSCTS;
    }

    /* 使能接收器和本地模式 */
    options.c_cflag |= (CLOCAL | CREAD);

    /* 关闭软件流控制 */
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* 原始输入模式 */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* 关闭输出处理 */
    options.c_oflag &= ~OPOST;

    /* 设置读取超时 */
    if (config->timeout_ms == 0) {
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 0;
    } else {
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = static_cast<cc_t>(config->timeout_ms / 100);
    }

    /* 应用配置 */
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        return -1;
    }

    /* 刷新缓冲区 */
    tcflush(fd, TCIOFLUSH);

    return 0;
}

/**
 * @brief 打开串口
 */
static rs485_device_t* rs485_open(const serial_config_t* config) {
    rs485_device_t* dev;

    if (config == nullptr || config->device == nullptr) {
        return nullptr;
    }

    /* 分配设备句柄 */
    dev = static_cast<rs485_device_t*>(malloc(sizeof(rs485_device_t)));
    if (dev == nullptr) {
        return nullptr;
    }
    std::memset(dev, 0, sizeof(rs485_device_t));

    /* 保存配置 */
    std::memcpy(&dev->config, config, sizeof(serial_config_t));

    /* 打开串口设备 */
    dev->serial_fd = open(config->device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (dev->serial_fd < 0) {
        free(dev);
        return nullptr;
    }

    /* 设置为阻塞模式 */
    if (set_blocking(dev->serial_fd, 1) < 0) {
        close(dev->serial_fd);
        free(dev);
        return nullptr;
    }

    /* 配置串口参数 */
    if (configure_port(dev->serial_fd, config) < 0) {
        close(dev->serial_fd);
        free(dev);
        return nullptr;
    }

    /* 初始化互斥锁 */
    pthread_mutex_init(&dev->mutex, nullptr);

    return dev;
}

/**
 * @brief 关闭串口
 */
static int rs485_close(rs485_device_t* dev) {
    if (dev == nullptr) {
        return -1;
    }

    if (dev->serial_fd >= 0) {
        close(dev->serial_fd);
    }

    pthread_mutex_destroy(&dev->mutex);
    free(dev);

    return 0;
}

/**
 * @brief 发送数据
 */
static int rs485_send(rs485_device_t* dev, const uint8_t* data, uint32_t len) {
    if (dev == nullptr || data == nullptr || len == 0) {
        return -1;
    }

    pthread_mutex_lock(&dev->mutex);
    int ret = static_cast<int>(write(dev->serial_fd, data, len));
    pthread_mutex_unlock(&dev->mutex);

    return ret;
}

/**
 * @brief 接收数据
 */
static int rs485_recv(rs485_device_t* dev, uint8_t* data, uint32_t len, uint32_t timeout_ms) {
    int ret;
    fd_set readfds;
    struct timeval tv;

    if (dev == nullptr || data == nullptr || len == 0) {
        return -1;
    }

    pthread_mutex_lock(&dev->mutex);

    FD_ZERO(&readfds);
    FD_SET(dev->serial_fd, &readfds);

    if (timeout_ms > 0) {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        ret = select(dev->serial_fd + 1, &readfds, nullptr, nullptr, &tv);
    } else {
        ret = select(dev->serial_fd + 1, &readfds, nullptr, nullptr, nullptr);
    }

    if (ret < 0) {
        pthread_mutex_unlock(&dev->mutex);
        return -1;
    } else if (ret == 0) {
        pthread_mutex_unlock(&dev->mutex);
        return 0;
    }

    if (FD_ISSET(dev->serial_fd, &readfds)) {
        ret = static_cast<int>(read(dev->serial_fd, data, len));
        pthread_mutex_unlock(&dev->mutex);
        return ret;
    }

    pthread_mutex_unlock(&dev->mutex);
    return 0;
}

/**
 * @brief 刷新串口缓冲区
 */
static int rs485_flush(rs485_device_t* dev) {
    if (dev == nullptr) {
        return -1;
    }

    pthread_mutex_lock(&dev->mutex);
    int ret = tcflush(dev->serial_fd, TCIOFLUSH);
    pthread_mutex_unlock(&dev->mutex);

    return ret;
}

/**
 * @brief RS485驱动操作接口实例
 */
static const rs485_driver_ops_t rs485_driver_ops = {
    .open = rs485_open,
    .close = rs485_close,
    .send = rs485_send,
    .recv = rs485_recv,
    .flush = rs485_flush,
};

/**
 * @brief 获取RS485驱动操作接口实例
 */
const rs485_driver_ops_t* rs485_driver_get_ops(void) {
    return &rs485_driver_ops;
}
