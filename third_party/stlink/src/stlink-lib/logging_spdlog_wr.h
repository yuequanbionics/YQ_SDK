/*
 * File: logging_spdlog_wr.h
 *
 * my_spdlog: Very fast, header-only/compiled, C++ logging library.
 * This is a wrapper file for my_spdlog.
 */

#ifndef _SPDLOG_WRAPPER_
#define _SPDLOG_WRAPPER_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int spdlogLog(int level, const char *str, ...);

#undef EXTERNC

#endif