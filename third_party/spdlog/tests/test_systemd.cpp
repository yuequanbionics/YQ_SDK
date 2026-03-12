#include "includes.h"
#include "spdlog/sinks/systemd_sink.h"

TEST_CASE("systemd", "[all]") {
    auto systemd_sink = std::make_shared<my_spdlog::sinks::systemd_sink_st>();
    my_spdlog::logger logger("spdlog_systemd_test", systemd_sink);
    logger.set_level(my_spdlog::level::trace);
    logger.trace("test my_spdlog trace");
    logger.debug("test my_spdlog debug");
    SPDLOG_LOGGER_INFO((&logger), "test my_spdlog info");
    SPDLOG_LOGGER_WARN((&logger), "test my_spdlog warn");
    SPDLOG_LOGGER_ERROR((&logger), "test my_spdlog error");
    SPDLOG_LOGGER_CRITICAL((&logger), "test my_spdlog critical");
}
