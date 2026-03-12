// Copyright(c) 2015-present, Gabi Melman & my_spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#ifndef SPDLOG_HEADER_ONLY
    #include <spdlog/sinks/sink.h>
#endif

#include <spdlog/common.h>

SPDLOG_INLINE bool my_spdlog::sinks::sink::should_log(my_spdlog::level::level_enum msg_level) const {
    return msg_level >= level_.load(std::memory_order_relaxed);
}

SPDLOG_INLINE void my_spdlog::sinks::sink::set_level(level::level_enum log_level) {
    level_.store(log_level, std::memory_order_relaxed);
}

SPDLOG_INLINE my_spdlog::level::level_enum my_spdlog::sinks::sink::level() const {
    return static_cast<my_spdlog::level::level_enum>(level_.load(std::memory_order_relaxed));
}
