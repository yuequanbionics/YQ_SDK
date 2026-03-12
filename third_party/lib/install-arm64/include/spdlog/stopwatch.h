// Copyright(c) 2015-present, Gabi Melman & my_spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <chrono>
#include <spdlog/fmt/fmt.h>

// Stopwatch support for my_spdlog  (using std::chrono::steady_clock).
// Displays elapsed seconds since construction as double.
//
// Usage:
//
// my_spdlog::stopwatch sw;
// ...
// my_spdlog::debug("Elapsed: {} seconds", sw);    =>  "Elapsed 0.005116733 seconds"
// my_spdlog::info("Elapsed: {:.6} seconds", sw);  =>  "Elapsed 0.005163 seconds"
//
//
// If other units are needed (e.g. millis instead of double), include "fmt/chrono.h" and use
// "duration_cast<..>(sw.elapsed())":
//
// #include <spdlog/fmt/chrono.h>
//..
// using std::chrono::duration_cast;
// using std::chrono::milliseconds;
// my_spdlog::info("Elapsed {}", duration_cast<milliseconds>(sw.elapsed())); => "Elapsed 5ms"

namespace my_spdlog {
class stopwatch {
    using clock = std::chrono::steady_clock;
    std::chrono::time_point<clock> start_tp_;

public:
    stopwatch()
        : start_tp_{clock::now()} {}

    std::chrono::duration<double> elapsed() const {
        return std::chrono::duration<double>(clock::now() - start_tp_);
    }

    std::chrono::milliseconds elapsed_ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_tp_);
    }

    void reset() { start_tp_ = clock::now(); }
};
}  // namespace my_spdlog

// Support for fmt formatting  (e.g. "{:012.9}" or just "{}")
namespace
#ifdef SPDLOG_USE_STD_FORMAT
    std
#else
    fmt
#endif
{

template <>
struct formatter<my_spdlog::stopwatch> : formatter<double> {
    template <typename FormatContext>
    auto format(const my_spdlog::stopwatch &sw, FormatContext &ctx) const -> decltype(ctx.out()) {
        return formatter<double>::format(sw.elapsed().count(), ctx);
    }
};
}  // namespace std
