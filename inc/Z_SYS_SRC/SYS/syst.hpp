#ifndef SYS_H
#define SYS_H
#include "stdint.h"
#include <thread>
#include <iostream>
#include <map>
#include <memory>
#include <atomic>
#include <string>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <cstdlib>
#include <sys/wait.h>

#include "spdlog/spdlog.h" 

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
typedef short s16;


// ANSI控制码重置：清除所有颜色和样式（必须搭配使用，避免颜色残留）
#define COLOR_RESET "\033[0m"

// -------------------------- 前景色（文字颜色） --------------------------
// 基础前景色（30-37）
#define FG_BLACK "\033[30m"       // 前景黑色
#define FG_RED "\033[31m"         // 前景红色
#define FG_GREEN "\033[32m"       // 前景绿色
#define FG_YELLOW "\033[33m"      // 前景黄色
#define FG_BLUE "\033[34m"        // 前景蓝色
#define FG_MAGENTA "\033[35m"     // 前景品红（紫色）
#define FG_CYAN "\033[36m"        // 前景青色（蓝绿色）
#define FG_WHITE "\033[37m"       // 前景白色

// 高亮前景色（亮色调，90-97）
#define FG_LIGHT_BLACK "\033[90m" // 前景亮黑（灰色）
#define FG_LIGHT_RED "\033[91m"   // 前景亮红
#define FG_LIGHT_GREEN "\033[92m" // 前景亮绿
#define FG_LIGHT_YELLOW "\033[93m"// 前景亮黄
#define FG_LIGHT_BLUE "\033[94m"  // 前景亮蓝
#define FG_LIGHT_MAGENTA "\033[95m"// 前景亮品红
#define FG_LIGHT_CYAN "\033[96m"  // 前景亮青色
#define FG_LIGHT_WHITE "\033[97m" // 前景亮白

// -------------------------- 背景色（文字背景） --------------------------
// 基础背景色（40-47）
#define BG_BLACK "\033[40m"       // 背景黑色
#define BG_RED "\033[41m"         // 背景红色
#define BG_GREEN "\033[42m"       // 背景绿色
#define BG_YELLOW "\033[43m"      // 背景黄色
#define BG_BLUE "\033[44m"        // 背景蓝色
#define BG_MAGENTA "\033[45m"     // 背景品红（紫色）
#define BG_CYAN "\033[46m"        // 背景青色（蓝绿色）
#define BG_WHITE "\033[47m"       // 背景白色

// 高亮背景色（亮色调，100-107）
#define BG_LIGHT_BLACK "\033[100m"// 背景亮黑（灰色）
#define BG_LIGHT_RED "\033[101m"  // 背景亮红
#define BG_LIGHT_GREEN "\033[102m"// 背景亮绿
#define BG_LIGHT_YELLOW "\033[103m"// 背景亮黄
#define BG_LIGHT_BLUE "\033[104m" // 背景亮蓝
#define BG_LIGHT_MAGENTA "\033[105m"// 背景亮品红
#define BG_LIGHT_CYAN "\033[106m" // 背景亮青色
#define BG_LIGHT_WHITE "\033[107m"// 背景亮白

// -------------------------- 文本样式（可选） --------------------------
#define STYLE_BOLD "\033[1m"      // 加粗（高亮）
#define STYLE_UNDERLINE "\033[4m" // 下划线
#define STYLE_REVERSE "\033[7m"   // 反显（前景/背景色互换）


std::string trim_file_path(const std::string& full_path);

// error 日志宏：红色标记
#define LOG_ERROR(...) \
    spdlog::get("LOG")->error(FG_RED "{}" COLOR_RESET, \
                  fmt::format(__VA_ARGS__))

// critical 日志宏：红色标记
#define LOG_CRITICAL(...) \
    spdlog::get("LOG")->critical(FG_YELLOW "[{}:{}]:\n" COLOR_RESET FG_RED "*-*-*-*-*-*-* {}" COLOR_RESET, \
                 trim_file_path(__FILE__), __LINE__, \
                 fmt::format(__VA_ARGS__))

// warn 日志宏：黄色标记
#define LOG_WARN(...) \
    spdlog::get("LOG")->warn(FG_YELLOW "{}" COLOR_RESET, \
                 fmt::format(__VA_ARGS__))

// debug 日志宏：蓝色标记
#define LOG_DEBUG(...) \
    spdlog::get("LOG")->debug(FG_BLUE "{}" COLOR_RESET, \
                  fmt::format(__VA_ARGS__))

// info 日志宏：白色标记
#define LOG_INFO(...) \
    spdlog::get("LOG")->info("{}", \
                  fmt::format(__VA_ARGS__))




#define LOG_INFO_COLOR(COLOR, ...) \
    spdlog::get("LOG")->info(COLOR "{}" COLOR_RESET, \
                  fmt::format(__VA_ARGS__))
 
#define LOG_INFO_2COLOR(COLOR1, COLOR2, DATA1, DATA2) \
    spdlog::get("LOG")->info(COLOR1 "{}" COLOR_RESET COLOR2 "{}" COLOR_RESET, \
                  DATA1, DATA2)
 
#define LOG_INFO_3COLOR(COLOR1, COLOR2, COLOR3, DATA1, DATA2, DATA3) \
    spdlog::get("LOG")->info(COLOR1 "{}" COLOR_RESET COLOR2 "{}" COLOR_RESET COLOR3 "{}" COLOR_RESET, \
                  DATA1, DATA2, DATA3)

#define LOG_INFO_4COLOR(COLOR1, COLOR2, COLOR3, COLOR4, DATA1, DATA2, DATA3, DATA4) \
    spdlog::get("LOG")->info(COLOR1 "{}" COLOR_RESET COLOR2 "{}" COLOR_RESET COLOR3 "{}" COLOR_RESET COLOR4 "{}" COLOR_RESET, \
                  DATA1, DATA2, DATA3, DATA4)



#define LOG_ERROR_COLOR(COLOR, ...) \
    spdlog::get("LOG")->error(COLOR "{}" COLOR_RESET, \
                  fmt::format(__VA_ARGS__))

#define LOG_ERROR_2COLOR(COLOR1, COLOR2, DATA1, DATA2) \
    spdlog::get("LOG")->error(COLOR1 "{}" COLOR_RESET COLOR2 "{}" COLOR_RESET, \
                  DATA1, DATA2)



#define ERR_DEBUG(name, info)                                                  \
    {                                                                          \
        LOG_ERROR("ERROR: " + std::string(name) + " -> " + std::string(info) + " Is_Not_find");          \
        return -1;                                                             \
    }


#define ERR_DEBUG_No_EXIT(name, info)                        \
    {                                                        \
        LOG_ERROR("ERROR: " + std::string(name) + " -> " + std::string(info));         \
    }


#endif

