#include "includes.h"
#include "spdlog/sinks/ringbuffer_sink.h"

TEST_CASE("ringbuffer invalid size", "[ringbuffer]") {
    REQUIRE_THROWS_AS(my_spdlog::sinks::ringbuffer_sink_mt(0), my_spdlog::spdlog_ex);
}

TEST_CASE("ringbuffer stores formatted messages", "[ringbuffer]") {
    my_spdlog::sinks::ringbuffer_sink_st sink(3);
    sink.set_pattern("%v");

    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "msg1"});
    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "msg2"});
    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "msg3"});

    auto formatted = sink.last_formatted();
    REQUIRE(formatted.size() == 3);
    using my_spdlog::details::os::default_eol;
    REQUIRE(formatted[0] == my_spdlog::fmt_lib::format("msg1{}", default_eol));
    REQUIRE(formatted[1] == my_spdlog::fmt_lib::format("msg2{}", default_eol));
    REQUIRE(formatted[2] == my_spdlog::fmt_lib::format("msg3{}", default_eol));
}

TEST_CASE("ringbuffer overrun keeps last items", "[ringbuffer]") {
    my_spdlog::sinks::ringbuffer_sink_st sink(2);
    sink.set_pattern("%v");

    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "first"});
    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "second"});
    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "third"});

    auto formatted = sink.last_formatted();
    REQUIRE(formatted.size() == 2);
    using my_spdlog::details::os::default_eol;
    REQUIRE(formatted[0] == my_spdlog::fmt_lib::format("second{}", default_eol));
    REQUIRE(formatted[1] == my_spdlog::fmt_lib::format("third{}", default_eol));
}

TEST_CASE("ringbuffer retrieval limit", "[ringbuffer]") {
    my_spdlog::sinks::ringbuffer_sink_st sink(3);
    sink.set_pattern("%v");

    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "A"});
    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "B"});
    sink.log(my_spdlog::details::log_msg{"test", my_spdlog::level::info, "C"});

    auto formatted = sink.last_formatted(2);
    REQUIRE(formatted.size() == 2);
    using my_spdlog::details::os::default_eol;
    REQUIRE(formatted[0] == my_spdlog::fmt_lib::format("B{}", default_eol));
    REQUIRE(formatted[1] == my_spdlog::fmt_lib::format("C{}", default_eol));
}
