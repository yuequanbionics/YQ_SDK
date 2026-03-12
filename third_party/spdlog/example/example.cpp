//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// my_spdlog usage example

#include <cstdio>
#include <chrono>

void load_levels_example();
void stdout_logger_example();
void basic_example();
void rotating_example();
void daily_example();
void callback_example();
void async_example();
void binary_example();
void vector_example();
void stopwatch_example();
void trace_example();
void multi_sink_example();
void user_defined_example();
void err_handler_example();
void syslog_example();
void udp_example();
void custom_flags_example();
void file_events_example();
void replace_default_logger_example();
void mdc_example();

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"   // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h"  // support for user defined types

int main(int, char *[]) {
    try {
        // Log levels can be loaded from argv/env using "SPDLOG_LEVEL"
        load_levels_example();

        my_spdlog::info("Welcome to my_spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR,
                     SPDLOG_VER_PATCH);

        my_spdlog::warn("Easy padding in numbers like {:08d}", 12);
        my_spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        my_spdlog::info("Support for floats {:03.2f}", 1.23456);
        my_spdlog::info("Positional args are {1} {0}..", "too", "supported");
        my_spdlog::info("{:>8} aligned, {:<8} aligned", "right", "left");

        // Runtime log levels
        my_spdlog::set_level(my_spdlog::level::info);  // Set global log level to info
        my_spdlog::debug("This message should not be displayed!");
        my_spdlog::set_level(my_spdlog::level::trace);  // Set specific logger's log level
        my_spdlog::debug("This message should be displayed..");

        // Customize msg format for all loggers
        my_spdlog::set_pattern("[%H:%M:%S %z] [%^%L%$] [thread %t] %v");
        my_spdlog::info("This an info message with custom format");
        my_spdlog::set_pattern("%+");  // back to default format
        my_spdlog::set_level(my_spdlog::level::info);

        // Backtrace support
        // Loggers can store in a ring buffer all messages (including debug/trace) for later
        // inspection. When needed, call dump_backtrace() to see what happened:
        my_spdlog::enable_backtrace(10);  // create ring buffer with capacity of 10  messages
        for (int i = 0; i < 100; i++) {
            my_spdlog::debug("Backtrace message {}", i);  // not logged..
        }
        // e.g. if some error happened:
        my_spdlog::dump_backtrace();  // log them now!

        stdout_logger_example();
        basic_example();
        rotating_example();
        daily_example();
        callback_example();
        async_example();
        binary_example();
        vector_example();
        multi_sink_example();
        user_defined_example();
        err_handler_example();
        trace_example();
        stopwatch_example();
        udp_example();
        custom_flags_example();
        file_events_example();
        replace_default_logger_example();
        mdc_example();

        // Flush all *registered* loggers using a worker thread every 3 seconds.
        // note: registered loggers *must* be thread safe for this to work correctly!
        my_spdlog::flush_every(std::chrono::seconds(3));

        // Apply some function on all registered loggers
        my_spdlog::apply_all([&](std::shared_ptr<my_spdlog::logger> l) { l->info("End of example."); });

        // Release all my_spdlog resources, and drop all loggers in the registry.
        // This is optional (only mandatory if using windows + async log).
        my_spdlog::shutdown();
    }

    // Exceptions will only be thrown upon failed logger or sink construction (not during logging).
    catch (const my_spdlog::spdlog_ex &ex) {
        std::printf("Log initialization failed: %s\n", ex.what());
        return 1;
    }
}

#include "spdlog/sinks/stdout_color_sinks.h"
// or #include "spdlog/sinks/stdout_sinks.h" if no colors needed.
void stdout_logger_example() {
    // Create color multi threaded logger.
    auto console = my_spdlog::stdout_color_mt("console");
    // or for stderr:
    // auto console = my_spdlog::stderr_color_mt("error-logger");
}

#include "spdlog/sinks/basic_file_sink.h"
void basic_example() {
    // Create basic file logger (not rotated).
    auto my_logger = my_spdlog::basic_logger_mt("file_logger", "logs/basic-log.txt", true);
}

#include "spdlog/sinks/rotating_file_sink.h"
void rotating_example() {
    // Create a file rotating logger with 5mb size max and 3 rotated files.
    auto rotating_logger =
        my_spdlog::rotating_logger_mt("some_logger_name", "logs/rotating.txt", 1048576 * 5, 3);
}

#include "spdlog/sinks/daily_file_sink.h"
void daily_example() {
    // Create a daily logger - a new file is created every day on 2:30am.
    auto daily_logger = my_spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30);
}

#include "spdlog/sinks/callback_sink.h"
void callback_example() {
    // Create the logger
    auto logger = my_spdlog::callback_logger_mt("custom_callback_logger",
                                             [](const my_spdlog::details::log_msg & /*msg*/) {
                                                 // do what you need to do with msg
                                             });
}

#include "spdlog/cfg/env.h"
void load_levels_example() {
    // Set the log level to "info" and mylogger to "trace":
    // SPDLOG_LEVEL=info,mylogger=trace && ./example
    my_spdlog::cfg::load_env_levels();
    // or specify the env variable name:
    // MYAPP_LEVEL=info,mylogger=trace && ./example
    // my_spdlog::cfg::load_env_levels("MYAPP_LEVEL");
    // or from command line:
    // ./example SPDLOG_LEVEL=info,mylogger=trace
    // #include "spdlog/cfg/argv.h" // for loading levels from argv
    // my_spdlog::cfg::load_argv_levels(args, argv);
}

#include "spdlog/async.h"
void async_example() {
    // Default thread pool settings can be modified *before* creating the async logger:
    // my_spdlog::init_thread_pool(32768, 1); // queue with max 32k items 1 backing thread.
    auto async_file =
        my_spdlog::basic_logger_mt<my_spdlog::async_factory>("async_file_logger", "logs/async_log.txt");
    // alternatively:
    // auto async_file =
    // my_spdlog::create_async<my_spdlog::sinks::basic_file_sink_mt>("async_file_logger",
    // "logs/async_log.txt");

    for (int i = 1; i < 101; ++i) {
        async_file->info("Async message #{}", i);
    }
}

// Log binary data as hex.
// Many types of std::container<char> types can be used.
// Iterator ranges are supported too.
// Format flags:
// {:X} - print in uppercase.
// {:s} - don't separate each byte with space.
// {:p} - don't print the position on each line start.
// {:n} - don't split the output to lines.

#if !defined SPDLOG_USE_STD_FORMAT || defined(_MSC_VER)
    #include "spdlog/fmt/bin_to_hex.h"
void binary_example() {
    std::vector<char> buf;
    for (int i = 0; i < 80; i++) {
        buf.push_back(static_cast<char>(i & 0xff));
    }
    my_spdlog::info("Binary example: {}", my_spdlog::to_hex(buf));
    my_spdlog::info("Another binary example:{:n}",
                 my_spdlog::to_hex(std::begin(buf), std::begin(buf) + 10));
    // more examples:
    // logger->info("uppercase: {:X}", my_spdlog::to_hex(buf));
    // logger->info("uppercase, no delimiters: {:Xs}", my_spdlog::to_hex(buf));
    // logger->info("uppercase, no delimiters, no position info: {:Xsp}", my_spdlog::to_hex(buf));
    // logger->info("hexdump style: {:a}", my_spdlog::to_hex(buf));
    // logger->info("hexdump style, 20 chars per line {:a}", my_spdlog::to_hex(buf, 20));
}
#else
void binary_example() {
    // not supported with std::format yet
}
#endif

// Log a vector of numbers
#ifndef SPDLOG_USE_STD_FORMAT
    #include "spdlog/fmt/ranges.h"
void vector_example() {
    std::vector<int> vec = {1, 2, 3};
    my_spdlog::info("Vector example: {}", vec);
}

#else
void vector_example() {}
#endif

// ! DSPDLOG_USE_STD_FORMAT

// Compile time log levels.
// define SPDLOG_ACTIVE_LEVEL to required level (e.g. SPDLOG_LEVEL_TRACE)
void trace_example() {
    // trace from default logger
    SPDLOG_TRACE("Some trace message.. {} ,{}", 1, 3.23);
    // debug from default logger
    SPDLOG_DEBUG("Some debug message.. {} ,{}", 1, 3.23);

    // trace from logger object
    auto logger = my_spdlog::get("file_logger");
    SPDLOG_LOGGER_TRACE(logger, "another trace message");
}

// stopwatch example
#include "spdlog/stopwatch.h"
#include <thread>
void stopwatch_example() {
    my_spdlog::stopwatch sw;
    std::this_thread::sleep_for(std::chrono::milliseconds(123));
    my_spdlog::info("Stopwatch: {} seconds", sw);
}

#include "spdlog/sinks/udp_sink.h"
void udp_example() {
    my_spdlog::sinks::udp_sink_config cfg("127.0.0.1", 11091);
    auto my_logger = my_spdlog::udp_logger_mt("udplog", cfg);
    my_logger->set_level(my_spdlog::level::debug);
    my_logger->info("hello world");
}

// A logger with multiple sinks (stdout and file) - each with a different format and log level.
void multi_sink_example() {
    auto console_sink = std::make_shared<my_spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(my_spdlog::level::warn);
    console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

    auto file_sink =
        std::make_shared<my_spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(my_spdlog::level::trace);

    my_spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(my_spdlog::level::debug);
    logger.warn("this should appear in both console and file");
    logger.info("this message should not appear in the console, only in the file");
}

// User defined types logging
struct my_type {
    int i = 0;
    explicit my_type(int i)
        : i(i) {}
};

#ifndef SPDLOG_USE_STD_FORMAT  // when using fmtlib
template <>
struct fmt::formatter<my_type> : fmt::formatter<std::string> {
    auto format(my_type my, format_context &ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "[my_type i={}]", my.i);
    }
};

#else  // when using std::format
template <>
struct std::formatter<my_type> : std::formatter<std::string> {
    auto format(my_type my, format_context &ctx) const -> decltype(ctx.out()) {
        return std::format_to(ctx.out(), "[my_type i={}]", my.i);
    }
};
#endif

void user_defined_example() { my_spdlog::info("user defined type: {}", my_type(14)); }

// Custom error handler. Will be triggered on log failure.
void err_handler_example() {
    // can be set globally or per logger(logger->set_error_handler(..))
    my_spdlog::set_error_handler([](const std::string &msg) {
        printf("*** Custom log error handler: %s ***\n", msg.c_str());
    });
}

// syslog example (linux/osx/freebsd)
#ifndef _WIN32
    #include "spdlog/sinks/syslog_sink.h"
void syslog_example() {
    std::string ident = "my_spdlog-example";
    auto syslog_logger = my_spdlog::syslog_logger_mt("syslog", ident, LOG_PID);
    syslog_logger->warn("This is warning that will end up in syslog.");
}
#endif

// Android example.
#if defined(__ANDROID__)
    #include "spdlog/sinks/android_sink.h"
void android_example() {
    std::string tag = "my_spdlog-android";
    auto android_logger = my_spdlog::android_logger_mt("android", tag);
    android_logger->critical("Use \"adb shell logcat\" to view this message.");
}
#endif

// Log patterns can contain custom flags.
// this will add custom flag '%*' which will be bound to a <my_formatter_flag> instance
#include "spdlog/pattern_formatter.h"
class my_formatter_flag : public my_spdlog::custom_flag_formatter {
public:
    void format(const my_spdlog::details::log_msg &,
                const std::tm &,
                my_spdlog::memory_buf_t &dest) override {
        std::string some_txt = "custom-flag";
        dest.append(some_txt.data(), some_txt.data() + some_txt.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override {
        return my_spdlog::details::make_unique<my_formatter_flag>();
    }
};

void custom_flags_example() {
    using my_spdlog::details::make_unique;  // for pre c++14
    auto formatter = make_unique<my_spdlog::pattern_formatter>();
    formatter->add_flag<my_formatter_flag>('*').set_pattern("[%n] [%*] [%^%l%$] %v");
    // set the new formatter using my_spdlog::set_formatter(formatter) or
    // logger->set_formatter(formatter) my_spdlog::set_formatter(std::move(formatter));
}

void file_events_example() {
    // pass the my_spdlog::file_event_handlers to file sinks for open/close log file notifications
    my_spdlog::file_event_handlers handlers;
    handlers.before_open = [](my_spdlog::filename_t filename) {
        my_spdlog::info("Before opening {}", filename);
    };
    handlers.after_open = [](my_spdlog::filename_t filename, std::FILE *fstream) {
        my_spdlog::info("After opening {}", filename);
        fputs("After opening\n", fstream);
    };
    handlers.before_close = [](my_spdlog::filename_t filename, std::FILE *fstream) {
        my_spdlog::info("Before closing {}", filename);
        fputs("Before closing\n", fstream);
    };
    handlers.after_close = [](my_spdlog::filename_t filename) {
        my_spdlog::info("After closing {}", filename);
    };
    auto file_sink = std::make_shared<my_spdlog::sinks::basic_file_sink_mt>("logs/events-sample.txt",
                                                                         true, handlers);
    my_spdlog::logger my_logger("some_logger", file_sink);
    my_logger.info("Some log line");
}

void replace_default_logger_example() {
    // store the old logger so we don't break other examples.
    auto old_logger = my_spdlog::default_logger();

    auto new_logger = my_spdlog::basic_logger_mt("new_default_logger", "logs/somelog.txt", true);
    my_spdlog::set_default_logger(std::move(new_logger));
    my_spdlog::set_level(my_spdlog::level::info);
    my_spdlog::debug("This message should not be displayed!");
    my_spdlog::set_level(my_spdlog::level::trace);
    my_spdlog::debug("This message should be displayed..");
    my_spdlog::set_default_logger(std::move(old_logger));
}

// Mapped Diagnostic Context (MDC) is a map that stores key-value pairs (string values) in thread
// local storage. Each thread maintains its own MDC, which loggers use to append diagnostic
// information to log outputs. Note: it is not supported in asynchronous mode due to its reliance on
// thread-local storage.

#ifndef SPDLOG_NO_TLS
    #include "spdlog/mdc.h"
void mdc_example() {
    my_spdlog::mdc::put("key1", "value1");
    my_spdlog::mdc::put("key2", "value2");
    // if not using the default format, you can use the %& formatter to print mdc data as well
    my_spdlog::set_pattern("[%H:%M:%S %z] [%^%L%$] [%&] %v");
    my_spdlog::info("Some log message with context");
}
#else
void mdc_example() {
    // if TLS feature is disabled
}
#endif
