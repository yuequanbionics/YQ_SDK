/*
 * This content is released under the MIT License as specified in
 * https://raw.githubusercontent.com/gabime/spdlog/master/LICENSE
 */
#include "includes.h"

#define TEST_FILENAME "test_logs/file_helper_test.txt"

using my_spdlog::details::file_helper;

static void write_with_helper(file_helper &helper, size_t howmany) {
    my_spdlog::memory_buf_t formatted;
    my_spdlog::fmt_lib::format_to(std::back_inserter(formatted), "{}", std::string(howmany, '1'));
    helper.write(formatted);
    helper.flush();
}

TEST_CASE("file_helper_filename", "[file_helper::filename()]") {
    prepare_logdir();

    file_helper helper;
    my_spdlog::filename_t target_filename = SPDLOG_FILENAME_T(TEST_FILENAME);
    helper.open(target_filename);
    REQUIRE(helper.filename() == target_filename);
}

TEST_CASE("file_helper_size", "[file_helper::size()]") {
    prepare_logdir();
    my_spdlog::filename_t target_filename = SPDLOG_FILENAME_T(TEST_FILENAME);
    size_t expected_size = 123;
    {
        file_helper helper;
        helper.open(target_filename);
        write_with_helper(helper, expected_size);
        REQUIRE(static_cast<size_t>(helper.size()) == expected_size);
    }
    REQUIRE(get_filesize(TEST_FILENAME) == expected_size);
}

TEST_CASE("file_helper_reopen", "[file_helper::reopen()]") {
    prepare_logdir();
    my_spdlog::filename_t target_filename = SPDLOG_FILENAME_T(TEST_FILENAME);
    file_helper helper;
    helper.open(target_filename);
    write_with_helper(helper, 12);
    REQUIRE(helper.size() == 12);
    helper.reopen(true);
    REQUIRE(helper.size() == 0);
}

TEST_CASE("file_helper_reopen2", "[file_helper::reopen(false)]") {
    prepare_logdir();
    my_spdlog::filename_t target_filename = SPDLOG_FILENAME_T(TEST_FILENAME);
    size_t expected_size = 14;
    file_helper helper;
    helper.open(target_filename);
    write_with_helper(helper, expected_size);
    REQUIRE(helper.size() == expected_size);
    helper.reopen(false);
    REQUIRE(helper.size() == expected_size);
}

static void test_split_ext(const my_spdlog::filename_t::value_type *fname,
                           const my_spdlog::filename_t::value_type *expect_base,
                           const my_spdlog::filename_t::value_type *expect_ext) {
    my_spdlog::filename_t filename(fname);
    my_spdlog::filename_t expected_base(expect_base);
    my_spdlog::filename_t expected_ext(expect_ext);

    my_spdlog::filename_t basename;
    my_spdlog::filename_t ext;
    std::tie(basename, ext) = file_helper::split_by_extension(filename);
    REQUIRE(basename == expected_base);
    REQUIRE(ext == expected_ext);
}

TEST_CASE("file_helper_split_by_extension", "[file_helper::split_by_extension()]") {
    test_split_ext(SPDLOG_FILENAME_T("mylog.txt"), SPDLOG_FILENAME_T("mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T(".mylog.txt"), SPDLOG_FILENAME_T(".mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T(".mylog"), SPDLOG_FILENAME_T(".mylog"), SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("/aaa/bb.d/mylog"), SPDLOG_FILENAME_T("/aaa/bb.d/mylog"),
                   SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("/aaa/bb.d/mylog.txt"), SPDLOG_FILENAME_T("/aaa/bb.d/mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T("aaa/bbb/ccc/mylog.txt"),
                   SPDLOG_FILENAME_T("aaa/bbb/ccc/mylog"), SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T("aaa/bbb/ccc/mylog."), SPDLOG_FILENAME_T("aaa/bbb/ccc/mylog."),
                   SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("aaa/bbb/ccc/.mylog.txt"),
                   SPDLOG_FILENAME_T("aaa/bbb/ccc/.mylog"), SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T("/aaa/bbb/ccc/mylog.txt"),
                   SPDLOG_FILENAME_T("/aaa/bbb/ccc/mylog"), SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T("/aaa/bbb/ccc/.mylog"),
                   SPDLOG_FILENAME_T("/aaa/bbb/ccc/.mylog"), SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("../mylog.txt"), SPDLOG_FILENAME_T("../mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T(".././mylog.txt"), SPDLOG_FILENAME_T(".././mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T(".././mylog.txt/xxx"), SPDLOG_FILENAME_T(".././mylog.txt/xxx"),
                   SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("/mylog.txt"), SPDLOG_FILENAME_T("/mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T("//mylog.txt"), SPDLOG_FILENAME_T("//mylog"),
                   SPDLOG_FILENAME_T(".txt"));
    test_split_ext(SPDLOG_FILENAME_T(""), SPDLOG_FILENAME_T(""), SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("."), SPDLOG_FILENAME_T("."), SPDLOG_FILENAME_T(""));
    test_split_ext(SPDLOG_FILENAME_T("..txt"), SPDLOG_FILENAME_T("."), SPDLOG_FILENAME_T(".txt"));
}

TEST_CASE("file_event_handlers", "[file_helper]") {
    enum class flags { before_open, after_open, before_close, after_close };
    prepare_logdir();

    my_spdlog::filename_t test_filename = SPDLOG_FILENAME_T(TEST_FILENAME);
    // define event handles that update vector of flags when called
    std::vector<flags> events;
    my_spdlog::file_event_handlers handlers;
    handlers.before_open = [&](my_spdlog::filename_t filename) {
        REQUIRE(filename == test_filename);
        events.push_back(flags::before_open);
    };
    handlers.after_open = [&](my_spdlog::filename_t filename, std::FILE *fstream) {
        REQUIRE(filename == test_filename);
        REQUIRE(fstream);
        fputs("after_open\n", fstream);
        events.push_back(flags::after_open);
    };
    handlers.before_close = [&](my_spdlog::filename_t filename, std::FILE *fstream) {
        REQUIRE(filename == test_filename);
        REQUIRE(fstream);
        fputs("before_close\n", fstream);
        events.push_back(flags::before_close);
    };
    handlers.after_close = [&](my_spdlog::filename_t filename) {
        REQUIRE(filename == test_filename);
        events.push_back(flags::after_close);
    };
    {
        my_spdlog::details::file_helper helper{handlers};
        REQUIRE(events.empty());

        helper.open(test_filename);
        REQUIRE(events == std::vector<flags>{flags::before_open, flags::after_open});

        events.clear();
        helper.close();
        REQUIRE(events == std::vector<flags>{flags::before_close, flags::after_close});
        REQUIRE(file_contents(TEST_FILENAME) == "after_open\nbefore_close\n");

        helper.reopen(true);
        events.clear();
    }
    // make sure that the file_helper destructor calls the close callbacks if needed
    REQUIRE(events == std::vector<flags>{flags::before_close, flags::after_close});
    REQUIRE(file_contents(TEST_FILENAME) == "after_open\nbefore_close\n");
}

TEST_CASE("file_helper_open", "[file_helper]") {
    prepare_logdir();
    my_spdlog::filename_t target_filename = SPDLOG_FILENAME_T(TEST_FILENAME);
    file_helper helper;
    helper.open(target_filename);
    helper.close();

    target_filename += SPDLOG_FILENAME_T("/invalid");
    REQUIRE_THROWS_AS(helper.open(target_filename), my_spdlog::spdlog_ex);
}
