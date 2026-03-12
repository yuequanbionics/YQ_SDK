// Copyright(c) 2015-present, Gabi Melman & my_spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <mutex>
#include <spdlog/details/null_mutex.h>

namespace my_spdlog {
namespace details {

struct console_mutex {
    using mutex_t = std::mutex;
    static mutex_t &mutex() {
        static mutex_t s_mutex;
        return s_mutex;
    }
};

struct console_nullmutex {
    using mutex_t = null_mutex;
    static mutex_t &mutex() {
        static mutex_t s_mutex;
        return s_mutex;
    }
};
}  // namespace details
}  // namespace my_spdlog
