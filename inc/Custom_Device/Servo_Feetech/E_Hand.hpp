//
// Created by lwz on 2025/12/22.
//

#ifndef RHS_E_HAND_HPP
#define RHS_E_HAND_HPP

#include "HARDWARE_TOP.hpp"
#include "Feetech_SCS0009.hpp"

/*
 * 手指关节 / 自由度 定义
 * 手指从指尖到掌根依次是:
 *  - 远侧指间关节 (Distal Interphalangeal Joint, DIP),
 *  - 近侧指间关节 (Proximal Interphalangeal Joint, PIP),
 *  - 掌指关节 (Metacarpophalangeal Joint, MCP)
 */
/* --------------- E-Hand 右手不同位置舵机 ID --------------- */
#define ID_R_INDEX_ABD          1       // 食指 - 内外展
#define ID_R_INDEX_MCP          2       // 330~500, 增大屈曲, 食指 - 掌指关节
#define ID_R_INDEX_DIP          3       // 620~760, 增大屈曲, 食指 - 远侧指间关节  (注: E-Hand 只有两节手指, 没有 PIP 关节)

#define ID_R_MIDDLE_ABD         8       // 中指 - 内外展
#define ID_R_MIDDLE_MCP         4       // 中指 - 掌指关节
#define ID_R_MIDDLE_DIP         12      // 300~160, 减小屈曲, 中指 - 远侧指间关节

#define ID_R_RING_ABD           5       // 无名指 - 内外展
#define ID_R_RING_MCP           6       // 468~598, 增大屈曲, 无名指 - 掌指关节
#define ID_R_RING_DIP           7       // 315~415, 增大屈曲, 无名指 - 远侧指间关节

#define ID_R_LITTLE_ABD         9       // 小指 - 内外展
#define ID_R_LITTLE_MCP         10      // 111~220, 增大屈曲, 小指 - 掌指关节
#define ID_R_LITTLE_DIP         11      // 173~273, 增大屈曲, 小指 - 远侧指间关节

#define ID_R_THUMB_OPP          13      // 拇指 - 对掌
#define ID_R_THUMB_ABD          14      // 拇指 - 内外展

#define ID_R_THUMB_MCP          15      // 拇指 - 掌指关节
#define ID_R_THUMB_DIP          16      // 拇指 - 远侧指间关节g

/* --------------- E-Hand 左手不同位置舵机 ID --------------- */
#define ID_L_INDEX_ABD          4
#define ID_L_INDEX_MCP          3
#define ID_L_INDEX_DIP          2

#define ID_L_MIDDLE_ABD         5
#define ID_L_MIDDLE_MCP         1
#define ID_L_MIDDLE_DIP         9

#define ID_L_RING_ABD           8
#define ID_L_RING_MCP           7
#define ID_L_RING_DIP           6

#define ID_L_LITTLE_ABD         12
#define ID_L_LITTLE_MCP         11
#define ID_L_LITTLE_DIP         10

#define ID_L_THUMB_OPP          13
#define ID_L_THUMB_ABD          14
#define ID_L_THUMB_MCP          15
#define ID_L_THUMB_DIP          16


/* --------------- 舵机最大可动范围 --------------- */
#define INDEX_MCP_MAX_R     130
#define INDEX_DIP_MAX_R     100
#define MIDDLE_MCP_MAX_R    (-170)  // 减小屈曲
#define MIDDLE_DIP_MAX_R    (-140)  // 减小屈曲
#define RING_MCP_MAX_R      130
#define RING_DIP_MAX_R      100
#define LITTLE_MCP_MAX_R    120
#define LITTLE_DIP_MAX_R    100
#define THUMB_OPP_MAX_R     211
#define THUMB_MCP_MAX_R     (-170)  // 减小屈曲
#define THUMB_DIP_MAX_R     (-95)   // 减小屈曲


#define INDEX_MCP_MAX_L     113
#define INDEX_DIP_MAX_L     100
#define MIDDLE_MCP_MAX_L    123
#define MIDDLE_DIP_MAX_L    90
#define RING_MCP_MAX_L      (-130)  // 减小屈曲
#define RING_DIP_MAX_L      120
#define LITTLE_MCP_MAX_L    100
#define LITTLE_DIP_MAX_L    100
#define THUMB_OPP_MAX_L     211
#define THUMB_MCP_MAX_L     70
#define THUMB_DIP_MAX_L     0


void Get_Finger_Start_Pos(const shared_ptr<Device_Struct>& Device,  Servo_SCS0009 * Ctrl);
void Restore(const shared_ptr<Device_Struct>& Device,  Servo_SCS0009 * Ctrl);
void Movement_1_Yeah(const shared_ptr<Device_Struct>& Device,  Servo_SCS0009 * Ctrl);
void Movement_2_Point(const shared_ptr<Device_Struct>& Device,  Servo_SCS0009 * Ctrl);
void Movement_3_Fist(const shared_ptr<Device_Struct>& Device,  Servo_SCS0009 * Ctrl);
void Movement_4_Pinch(const shared_ptr<Device_Struct>& Device,  Servo_SCS0009 * Ctrl);


#endif //RHS_E_HAND_HPP