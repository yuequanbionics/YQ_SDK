#ifdef _WIN32  // to prevent fopen warning on windows
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include "includes.h"
#include "test_sink.h"

template <class T>
std::string log_info(const T& what, my_spdlog::level::level_enum logger_level = my_spdlog::level::info) {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<my_spdlog::sinks::ostream_sink_mt>(oss);

    my_spdlog::logger oss_logger("oss", oss_sink);
    oss_logger.set_level(logger_level);
    oss_logger.set_pattern("%v");
    oss_logger.info(what);

    return oss.str().substr(0, oss.str().length() - strlen(my_spdlog::details::os::default_eol));
}

TEST_CASE("basic_logging ", "[basic_logging]") {
    // const char
    REQUIRE(log_info("Hello") == "Hello");
    REQUIRE(log_info("").empty());

    // std::string
    REQUIRE(log_info(std::string("Hello")) == "Hello");
    REQUIRE(log_info(std::string()).empty());

    // Numbers
    REQUIRE(log_info(5) == "5");
    REQUIRE(log_info(5.6) == "5.6");

    // User defined class
    // REQUIRE(log_info(some_logged_class("some_val")) == "some_val");
}

TEST_CASE("log_levels", "[log_levels]") {
    REQUIRE(log_info("Hello", my_spdlog::level::err).empty());
    REQUIRE(log_info("Hello", my_spdlog::level::critical).empty());
    REQUIRE(log_info("Hello", my_spdlog::level::info) == "Hello");
    REQUIRE(log_info("Hello", my_spdlog::level::debug) == "Hello");
    REQUIRE(log_info("Hello", my_spdlog::level::trace) == "Hello");
}

TEST_CASE("level_to_string_view", "[convert_to_string_view]") {
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::trace) == "trace");
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::debug) == "debug");
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::info) == "info");
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::warn) == "warning");
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::err) == "error");
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::critical) == "critical");
    REQUIRE(my_spdlog::level::to_string_view(my_spdlog::level::off) == "off");
}

TEST_CASE("to_short_c_str", "[convert_to_short_c_str]") {
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::trace)) == "T");
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::debug)) == "D");
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::info)) == "I");
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::warn)) == "W");
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::err)) == "E");
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::critical)) == "C");
    REQUIRE(std::string(my_spdlog::level::to_short_c_str(my_spdlog::level::off)) == "O");
}

TEST_CASE("to_level_enum", "[convert_to_level_enum]") {
    REQUIRE(my_spdlog::level::from_str("trace") == my_spdlog::level::trace);
    REQUIRE(my_spdlog::level::from_str("debug") == my_spdlog::level::debug);
    REQUIRE(my_spdlog::level::from_str("info") == my_spdlog::level::info);
    REQUIRE(my_spdlog::level::from_str("warning") == my_spdlog::level::warn);
    REQUIRE(my_spdlog::level::from_str("warn") == my_spdlog::level::warn);
    REQUIRE(my_spdlog::level::from_str("error") == my_spdlog::level::err);
    REQUIRE(my_spdlog::level::from_str("critical") == my_spdlog::level::critical);
    REQUIRE(my_spdlog::level::from_str("off") == my_spdlog::level::off);
    REQUIRE(my_spdlog::level::from_str("null") == my_spdlog::level::off);
}

TEST_CASE("periodic flush", "[periodic_flush]") {
    using my_spdlog::sinks::test_sink_mt;
    auto logger = my_spdlog::create<test_sink_mt>("periodic_flush");
    auto test_sink = std::static_pointer_cast<test_sink_mt>(logger->sinks()[0]);

    my_spdlog::flush_every(std::chrono::seconds(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(1250));
    REQUIRE(test_sink->flush_counter() == 1);
    my_spdlog::flush_every(std::chrono::seconds(0));
    my_spdlog::drop_all();
}

TEST_CASE("clone-logger", "[clone]") {
    using my_spdlog::sinks::test_sink_mt;
    auto test_sink = std::make_shared<test_sink_mt>();
    auto logger = std::make_shared<my_spdlog::logger>("orig", test_sink);
    logger->set_pattern("%v");
    auto cloned = logger->clone("clone");

    REQUIRE(cloned->name() == "clone");
    REQUIRE(logger->sinks() == cloned->sinks());
    REQUIRE(logger->level() == cloned->level());
    REQUIRE(logger->flush_level() == cloned->flush_level());
    logger->info("Some message 1");
    cloned->info("Some message 2");

    REQUIRE(test_sink->lines().size() == 2);
    REQUIRE(test_sink->lines()[0] == "Some message 1");
    REQUIRE(test_sink->lines()[1] == "Some message 2");

    my_spdlog::drop_all();
}

TEST_CASE("clone async", "[clone]") {
    using my_spdlog::sinks::test_sink_mt;
    my_spdlog::init_thread_pool(4, 1);
    auto test_sink = std::make_shared<test_sink_mt>();
    auto logger = std::make_shared<my_spdlog::async_logger>("orig", test_sink, my_spdlog::thread_pool());
    logger->set_pattern("%v");
    auto cloned = logger->clone("clone");

    REQUIRE(cloned->name() == "clone");
    REQUIRE(logger->sinks() == cloned->sinks());
    REQUIRE(logger->level() == cloned->level());
    REQUIRE(logger->flush_level() == cloned->flush_level());

    logger->info("Some message 1");
    cloned->info("Some message 2");

    my_spdlog::details::os::sleep_for_millis(100);

    REQUIRE(test_sink->lines().size() == 2);
    REQUIRE(test_sink->lines()[0] == "Some message 1");
    REQUIRE(test_sink->lines()[1] == "Some message 2");

    my_spdlog::drop_all();
}

TEST_CASE("default logger API", "[default logger]") {
    std::ostringstream oss;
    auto oss_sink = std::make_shared<my_spdlog::sinks::ostream_sink_mt>(oss);

    my_spdlog::set_default_logger(std::make_shared<my_spdlog::logger>("oss", oss_sink));
    my_spdlog::set_pattern("*** %v");

    my_spdlog::default_logger()->set_level(my_spdlog::level::trace);
    my_spdlog::trace("hello trace");
    REQUIRE(oss.str() == "*** hello trace" + std::string(my_spdlog::details::os::default_eol));

    oss.str("");
    my_spdlog::debug("hello debug");
    REQUIRE(oss.str() == "*** hello debug" + std::string(my_spdlog::details::os::default_eol));

    oss.str("");
    my_spdlog::info("Hello");
    REQUIRE(oss.str() == "*** Hello" + std::string(my_spdlog::details::os::default_eol));

    oss.str("");
    my_spdlog::warn("Hello again {}", 2);
    REQUIRE(oss.str() == "*** Hello again 2" + std::string(my_spdlog::details::os::default_eol));

    oss.str("");
    my_spdlog::error(123);
    REQUIRE(oss.str() == "*** 123" + std::string(my_spdlog::details::os::default_eol));

    oss.str("");
    my_spdlog::critical(std::string("some string"));
    REQUIRE(oss.str() == "*** some string" + std::string(my_spdlog::details::os::default_eol));

    oss.str("");
    my_spdlog::set_level(my_spdlog::level::info);
    my_spdlog::debug("should not be logged");
    REQUIRE(oss.str().empty());
    my_spdlog::drop_all();
    my_spdlog::set_pattern("%v");
}

#if (defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT) || defined(SPDLOG_WCHAR_FILENAMES)) && defined(_WIN32)
TEST_CASE("utf8 to utf16 conversion using windows api", "[windows utf]") {
    my_spdlog::wmemory_buf_t buffer;

    my_spdlog::details::os::utf8_to_wstrbuf("", buffer);
    REQUIRE(std::wstring(buffer.data(), buffer.size()) == std::wstring(L""));

    my_spdlog::details::os::utf8_to_wstrbuf("abc", buffer);
    REQUIRE(std::wstring(buffer.data(), buffer.size()) == std::wstring(L"abc"));

    my_spdlog::details::os::utf8_to_wstrbuf("\xc3\x28", buffer);  // Invalid UTF-8 sequence.
    REQUIRE(std::wstring(buffer.data(), buffer.size()) == std::wstring(L"\xfffd("));

    my_spdlog::details::os::utf8_to_wstrbuf("\xe3\x81\xad\xe3\x81\x93",
                                         buffer);  // "Neko" in hiragana.
    REQUIRE(std::wstring(buffer.data(), buffer.size()) == std::wstring(L"\x306d\x3053"));
}
#endif

struct auto_closer {
    FILE* fp = nullptr;
    explicit auto_closer(FILE* f)
        : fp(f) {}
    auto_closer(const auto_closer&) = delete;
    auto_closer& operator=(const auto_closer&) = delete;
    ~auto_closer() {
        if (fp != nullptr) (void)std::fclose(fp);
    }
};

TEST_CASE("os::fwrite_bytes", "[os]") {
    using my_spdlog::details::os::create_dir;
    using my_spdlog::details::os::fwrite_bytes;
    const char* filename = "log_tests/test_fwrite_bytes.txt";
    const char* msg = "hello";
    prepare_logdir();
    REQUIRE(create_dir(SPDLOG_FILENAME_T("log_tests")) == true);
    {
        auto_closer closer(std::fopen(filename, "wb"));
        REQUIRE(closer.fp != nullptr);
        REQUIRE(fwrite_bytes(msg, std::strlen(msg), closer.fp) == true);
        REQUIRE(fwrite_bytes(msg, 0, closer.fp) == true);
        std::fflush(closer.fp);
        REQUIRE(my_spdlog::details::os::filesize(closer.fp) == 5);
    }
    // fwrite_bytes should return false on write failure
    auto_closer closer(std::fopen(filename, "r"));
    REQUIRE(closer.fp != nullptr);
    REQUIRE_FALSE(fwrite_bytes("Hello", 5, closer.fp));
}
