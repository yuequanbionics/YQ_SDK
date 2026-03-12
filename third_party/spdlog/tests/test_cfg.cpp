
#include "includes.h"
#include "test_sink.h"

#include <spdlog/cfg/env.h>
#include <spdlog/cfg/argv.h>

using my_spdlog::cfg::load_argv_levels;
using my_spdlog::cfg::load_env_levels;
using my_spdlog::sinks::test_sink_st;

TEST_CASE("env", "[cfg]") {
    my_spdlog::drop("l1");
    auto l1 = my_spdlog::create<test_sink_st>("l1");
#ifdef CATCH_PLATFORM_WINDOWS
    _putenv_s("SPDLOG_LEVEL", "l1=warn");
#else
    setenv("SPDLOG_LEVEL", "l1=warn", 1);
#endif
    load_env_levels();
    REQUIRE(l1->level() == my_spdlog::level::warn);

#ifdef CATCH_PLATFORM_WINDOWS
    _putenv_s("MYAPP_LEVEL", "l1=trace");
#else
    setenv("MYAPP_LEVEL", "l1=trace", 1);
#endif
    load_env_levels("MYAPP_LEVEL");
    REQUIRE(l1->level() == my_spdlog::level::trace);

    my_spdlog::set_default_logger(my_spdlog::create<test_sink_st>("cfg-default"));
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::info);
}

TEST_CASE("argv1", "[cfg]") {
    my_spdlog::drop("l1");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=l1=warn"};
    load_argv_levels(2, argv);
    auto l1 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l1");
    REQUIRE(l1->level() == my_spdlog::level::warn);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::info);
}

TEST_CASE("argv2", "[cfg]") {
    my_spdlog::drop("l1");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=l1=warn,trace"};
    load_argv_levels(2, argv);
    auto l1 = my_spdlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == my_spdlog::level::warn);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::trace);
}

TEST_CASE("argv3", "[cfg]") {
    my_spdlog::set_level(my_spdlog::level::trace);

    my_spdlog::drop("l1");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=junk_name=warn"};
    load_argv_levels(2, argv);
    auto l1 = my_spdlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == my_spdlog::level::trace);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::trace);
}

TEST_CASE("argv4", "[cfg]") {
    my_spdlog::set_level(my_spdlog::level::info);
    my_spdlog::drop("l1");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=junk"};
    load_argv_levels(2, argv);
    auto l1 = my_spdlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == my_spdlog::level::info);
}

TEST_CASE("argv5", "[cfg]") {
    my_spdlog::set_level(my_spdlog::level::info);
    my_spdlog::drop("l1");
    const char *argv[] = {"ignore", "ignore", "SPDLOG_LEVEL=l1=warn,trace"};
    load_argv_levels(3, argv);
    auto l1 = my_spdlog::create<test_sink_st>("l1");
    REQUIRE(l1->level() == my_spdlog::level::warn);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::trace);
    my_spdlog::set_level(my_spdlog::level::info);
}

TEST_CASE("argv6", "[cfg]") {
    my_spdlog::set_level(my_spdlog::level::err);
    const char *argv[] = {""};
    load_argv_levels(1, argv);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::err);
    my_spdlog::set_level(my_spdlog::level::info);
}

TEST_CASE("argv7", "[cfg]") {
    my_spdlog::set_level(my_spdlog::level::err);
    const char *argv[] = {""};
    load_argv_levels(0, argv);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::err);
    my_spdlog::set_level(my_spdlog::level::info);
}

TEST_CASE("level-not-set-test1", "[cfg]") {
    my_spdlog::drop("l1");
    const char *argv[] = {"ignore", ""};
    load_argv_levels(2, argv);
    auto l1 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l1");
    l1->set_level(my_spdlog::level::trace);
    REQUIRE(l1->level() == my_spdlog::level::trace);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::info);
}

TEST_CASE("level-not-set-test2", "[cfg]") {
    my_spdlog::drop("l1");
    my_spdlog::drop("l2");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=l1=trace"};

    auto l1 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l1");
    l1->set_level(my_spdlog::level::warn);
    auto l2 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l2");
    l2->set_level(my_spdlog::level::warn);

    load_argv_levels(2, argv);

    REQUIRE(l1->level() == my_spdlog::level::trace);
    REQUIRE(l2->level() == my_spdlog::level::warn);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::info);
}

TEST_CASE("level-not-set-test3", "[cfg]") {
    my_spdlog::drop("l1");
    my_spdlog::drop("l2");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=l1=trace"};

    load_argv_levels(2, argv);

    auto l1 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l1");
    auto l2 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l2");

    REQUIRE(l1->level() == my_spdlog::level::trace);
    REQUIRE(l2->level() == my_spdlog::level::info);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::info);
}

TEST_CASE("level-not-set-test4", "[cfg]") {
    my_spdlog::drop("l1");
    my_spdlog::drop("l2");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=l1=trace,warn"};

    load_argv_levels(2, argv);

    auto l1 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l1");
    auto l2 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l2");

    REQUIRE(l1->level() == my_spdlog::level::trace);
    REQUIRE(l2->level() == my_spdlog::level::warn);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::warn);
}

TEST_CASE("level-not-set-test5", "[cfg]") {
    my_spdlog::drop("l1");
    my_spdlog::drop("l2");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=l1=junk,warn"};

    load_argv_levels(2, argv);

    auto l1 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l1");
    auto l2 = my_spdlog::create<my_spdlog::sinks::test_sink_st>("l2");

    REQUIRE(l1->level() == my_spdlog::level::warn);
    REQUIRE(l2->level() == my_spdlog::level::warn);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::warn);
}

TEST_CASE("restore-to-default", "[cfg]") {
    my_spdlog::drop("l1");
    my_spdlog::drop("l2");
    const char *argv[] = {"ignore", "SPDLOG_LEVEL=info"};
    load_argv_levels(2, argv);
    REQUIRE(my_spdlog::default_logger()->level() == my_spdlog::level::info);
}
