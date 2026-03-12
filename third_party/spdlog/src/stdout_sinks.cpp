// Copyright(c) 2015-present, Gabi Melman & my_spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef SPDLOG_COMPILED_LIB
    #error Please define SPDLOG_COMPILED_LIB to compile this file.
#endif

#include <mutex>

#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/stdout_sinks-inl.h>

template class SPDLOG_API my_spdlog::sinks::stdout_sink_base<my_spdlog::details::console_mutex>;
template class SPDLOG_API my_spdlog::sinks::stdout_sink_base<my_spdlog::details::console_nullmutex>;
template class SPDLOG_API my_spdlog::sinks::stdout_sink<my_spdlog::details::console_mutex>;
template class SPDLOG_API my_spdlog::sinks::stdout_sink<my_spdlog::details::console_nullmutex>;
template class SPDLOG_API my_spdlog::sinks::stderr_sink<my_spdlog::details::console_mutex>;
template class SPDLOG_API my_spdlog::sinks::stderr_sink<my_spdlog::details::console_nullmutex>;

template SPDLOG_API std::shared_ptr<my_spdlog::logger>
my_spdlog::stdout_logger_mt<my_spdlog::synchronous_factory>(const std::string &logger_name);
template SPDLOG_API std::shared_ptr<my_spdlog::logger>
my_spdlog::stdout_logger_st<my_spdlog::synchronous_factory>(const std::string &logger_name);
template SPDLOG_API std::shared_ptr<my_spdlog::logger>
my_spdlog::stderr_logger_mt<my_spdlog::synchronous_factory>(const std::string &logger_name);
template SPDLOG_API std::shared_ptr<my_spdlog::logger>
my_spdlog::stderr_logger_st<my_spdlog::synchronous_factory>(const std::string &logger_name);

template SPDLOG_API std::shared_ptr<my_spdlog::logger> my_spdlog::stdout_logger_mt<my_spdlog::async_factory>(
    const std::string &logger_name);
template SPDLOG_API std::shared_ptr<my_spdlog::logger> my_spdlog::stdout_logger_st<my_spdlog::async_factory>(
    const std::string &logger_name);
template SPDLOG_API std::shared_ptr<my_spdlog::logger> my_spdlog::stderr_logger_mt<my_spdlog::async_factory>(
    const std::string &logger_name);
template SPDLOG_API std::shared_ptr<my_spdlog::logger> my_spdlog::stderr_logger_st<my_spdlog::async_factory>(
    const std::string &logger_name);
