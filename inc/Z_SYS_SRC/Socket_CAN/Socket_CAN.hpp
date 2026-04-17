//
// Created by lwz on 2026/4/14.
//

#ifndef SDK_SOCKET_CAN_HPP
#define SDK_SOCKET_CAN_HPP


#include <string>
#include <cstdint>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "syst.hpp"
#include "HARDWARE_TOP.hpp"

class Socket_CanFD
{
public:
    Socket_CanFD();
    ~Socket_CanFD();


    bool configCanFD(const char* can_instance, uint32_t bitrate, uint32_t d_bitrate);
    bool openCanFD(const std::string& can_instance);
    bool send(uint32_t id, const uint8_t* data, uint8_t len) const;
    bool receive(uint32_t& id, uint8_t* data, uint8_t& len) const;
    void close();

private:
    int m_socket;
    bool m_initialized;
};

#endif //SDK_SOCKET_CAN_HPP
