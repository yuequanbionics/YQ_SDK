#include "includes.h"

static const char *const tested_logger_name = "null_logger";
static const char *const tested_logger_name2 = "null_logger2";

#ifndef SPDLOG_NO_EXCEPTIONS
TEST_CASE("register_drop", "[registry]") {
    my_spdlog::drop_all();
    my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name);
    REQUIRE(my_spdlog::get(tested_logger_name) != nullptr);
    // Throw if registering existing name
    REQUIRE_THROWS_AS(my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name),
                      my_spdlog::spdlog_ex);
}

TEST_CASE("explicit register", "[registry]") {
    my_spdlog::drop_all();
    auto logger = std::make_shared<my_spdlog::logger>(tested_logger_name,
                                                   std::make_shared<my_spdlog::sinks::null_sink_st>());
    my_spdlog::register_logger(logger);
    REQUIRE(my_spdlog::get(tested_logger_name) != nullptr);
    // Throw if registering existing name
    REQUIRE_THROWS_AS(my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name),
                      my_spdlog::spdlog_ex);
}
#endif

TEST_CASE("register_or_replace", "[registry]") {
    my_spdlog::drop_all();
    auto logger1 = std::make_shared<my_spdlog::logger>(
        tested_logger_name, std::make_shared<my_spdlog::sinks::null_sink_st>());
    my_spdlog::register_logger(logger1);
    REQUIRE(my_spdlog::get(tested_logger_name) == logger1);

    auto logger2 = std::make_shared<my_spdlog::logger>(
        tested_logger_name, std::make_shared<my_spdlog::sinks::null_sink_st>());
    my_spdlog::register_or_replace(logger2);
    REQUIRE(my_spdlog::get(tested_logger_name) == logger2);
}

TEST_CASE("apply_all", "[registry]") {
    my_spdlog::drop_all();
    auto logger = std::make_shared<my_spdlog::logger>(tested_logger_name,
                                                   std::make_shared<my_spdlog::sinks::null_sink_st>());
    my_spdlog::register_logger(logger);
    auto logger2 = std::make_shared<my_spdlog::logger>(
        tested_logger_name2, std::make_shared<my_spdlog::sinks::null_sink_st>());
    my_spdlog::register_logger(logger2);

    int counter = 0;
    my_spdlog::apply_all([&counter](std::shared_ptr<my_spdlog::logger>) { counter++; });
    REQUIRE(counter == 2);

    counter = 0;
    my_spdlog::drop(tested_logger_name2);
    my_spdlog::apply_all([&counter](std::shared_ptr<my_spdlog::logger> l) {
        REQUIRE(l->name() == tested_logger_name);
        counter++;
    });
    REQUIRE(counter == 1);
}

TEST_CASE("drop", "[registry]") {
    my_spdlog::drop_all();
    my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name);
    my_spdlog::drop(tested_logger_name);
    REQUIRE_FALSE(my_spdlog::get(tested_logger_name));
}

TEST_CASE("drop-default", "[registry]") {
    my_spdlog::set_default_logger(my_spdlog::null_logger_st(tested_logger_name));
    my_spdlog::drop(tested_logger_name);
    REQUIRE_FALSE(my_spdlog::default_logger());
    REQUIRE_FALSE(my_spdlog::get(tested_logger_name));
}

TEST_CASE("drop_all", "[registry]") {
    my_spdlog::drop_all();
    my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name);
    my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name2);
    my_spdlog::drop_all();
    REQUIRE_FALSE(my_spdlog::get(tested_logger_name));
    REQUIRE_FALSE(my_spdlog::get(tested_logger_name2));
    REQUIRE_FALSE(my_spdlog::default_logger());
}

TEST_CASE("drop non existing", "[registry]") {
    my_spdlog::drop_all();
    my_spdlog::create<my_spdlog::sinks::null_sink_mt>(tested_logger_name);
    my_spdlog::drop("some_name");
    REQUIRE_FALSE(my_spdlog::get("some_name"));
    REQUIRE(my_spdlog::get(tested_logger_name));
    my_spdlog::drop_all();
}

TEST_CASE("default logger", "[registry]") {
    my_spdlog::drop_all();
    my_spdlog::set_default_logger(my_spdlog::null_logger_st(tested_logger_name));
    REQUIRE(my_spdlog::get(tested_logger_name) == my_spdlog::default_logger());
    my_spdlog::drop_all();
}

TEST_CASE("set_default_logger(nullptr)", "[registry]") {
    my_spdlog::set_default_logger(nullptr);
    REQUIRE_FALSE(my_spdlog::default_logger());
}

TEST_CASE("disable automatic registration", "[registry]") {
    // set some global parameters
    my_spdlog::level::level_enum log_level = my_spdlog::level::level_enum::warn;
    my_spdlog::set_level(log_level);
    // but disable automatic registration
    my_spdlog::set_automatic_registration(false);
    auto logger1 = my_spdlog::create<my_spdlog::sinks::daily_file_sink_st>(
        tested_logger_name, SPDLOG_FILENAME_T("filename"), 11, 59);
    auto logger2 = my_spdlog::create_async<my_spdlog::sinks::stdout_color_sink_mt>(tested_logger_name2);
    // loggers should not be part of the registry
    REQUIRE_FALSE(my_spdlog::get(tested_logger_name));
    REQUIRE_FALSE(my_spdlog::get(tested_logger_name2));
    // but make sure they are still initialized according to global defaults
    REQUIRE(logger1->level() == log_level);
    REQUIRE(logger2->level() == log_level);
    my_spdlog::set_level(my_spdlog::level::info);
    my_spdlog::set_automatic_registration(true);
}
