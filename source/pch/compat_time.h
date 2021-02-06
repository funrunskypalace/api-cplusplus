#pragma once

#include <chrono>
#include <ctime>
#include <string.h>
#include <string>
#include <time.h>

#ifdef WIN32
typedef SOCKET fd_t;
int gettimeofday(struct timeval* tp, void* tzp);
int strcasecmp(const char* s1, const char* s2);
int clock_gettime(int X, struct timeval* tv);
LARGE_INTEGER getFILETIMEoffset();
#endif
#if defined(LINUX)
#include <sys/time.h>
#include <sys/timeb.h>
#endif

#define SleepMs(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

time_t parse_date_str(std::string dateStr);
uint64_t parse_milliseconds(int32_t date, int32_t timeWithMs, bool utc);
time_t parse_today_time_str(const std::string& timestr, bool utc);
std::string today_time_str(time_t tv);
time_t time_t_from_hms(int hourMinSec, bool utc);
using stdtp = std::chrono::system_clock::time_point;
stdtp from_time_t(time_t tv);
uint64_t get_current_time_us(bool utc);
uint64_t get_current_time_us_readable(bool utc);
uint64_t get_nano(const time_t& t, bool utc);
uint64_t get_nano(bool utc);
int get_current_date(bool utc);
int get_current_date(time_t& t, bool utc);
std::string get_current_system_time_s(const time_t& t, bool utc);
std::string get_current_system_time_s(bool utc);
int get_current_system_time3(bool utc);
int get_current_system_time3(const time_t& t, bool utc);
