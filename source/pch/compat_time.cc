#include "pch/compat.h"

#include <codecvt>

// 关于时间相关函数的说明：
// - time_t =0 的时候 其实是 1970-01-01 00:00:00 到UTC 0时区的秒数，
//   而不是 我们北京时间UTC+8；所以gmtime比localtime 小8小时。
// - std::chrono::system_clock::now()也是UTC时间。
//   Though unspecified by the standard, every implementation of
//   std::chrono::system_clock::now() is tracking Unix Time,
//   which is a very close approximation to UTC.
// - Unix Time 与UTC时间的关系
//   如第二点说明。

struct Tm : std::tm
{
    int tm_usecs; // [0, 999999] micros after the sec

    Tm(const int year, const int month, const int mday, const int hour,
       const int min, const int sec, const int usecs, const int isDST = -1)
        : tm_usecs{usecs}
    {
        tm_year = year - 1900; // [0, 60] since 1900
        tm_mon = month - 1;    // [0, 11] since Jan
        tm_mday = mday;        // [1, 31]
        tm_hour = hour;        // [0, 23] since midnight
        tm_min = min;          // [0, 59] after the hour
        tm_sec = sec;          // [0, 60] after the min
                               //         allows for 1 positive leap second
        tm_isdst = isDST;      // [-1...] -1 for unknown, 0 for not DST,
                               //         any positive value if DST.
    }

    template <typename Clock_t = std::chrono::system_clock,
              typename MicroSecond_t = std::chrono::microseconds>
    auto to_time_point() -> typename Clock_t::time_point
    {
        auto time_c = mktime(this);
        return Clock_t::from_time_t(time_c) + MicroSecond_t{tm_usecs};
    }
};

uint64_t get_current_time_us(bool utc) { return get_nano(utc) / 1000; }

uint64_t get_current_time_us_readable(bool utc)
{
    uint64_t timeResult;

    time_t rawtime;
    struct tm* p;
    time(&rawtime);
    p = localtime(&rawtime);
    timeResult = (uint64_t)(p->tm_hour) * 10000000000;
    // ORCHID_LOG(debug, "timeResult: {}",  timeResult << "tm_hour: {}",
    // p->tm_hour);
    timeResult += (uint64_t)(p->tm_min) * 100000000;
    // ORCHID_LOG(debug, "timeResult: {}",  timeResult << "tm_min: {}",
    // p->tm_min);
    timeResult += (uint64_t)(p->tm_sec) * 1000000;
    // ORCHID_LOG(debug, "timeResult: {}",  timeResult << "tm_sec: {}",
    // p->tm_sec);
    struct timeval tv;
    gettimeofday(&tv, NULL);
    timeResult += tv.tv_usec;
    // ORCHID_LOG(debug, "timeResult: {}",  timeResult << "localTime.tv_usec:
    // {}",  localTime.tv_usec);

    return timeResult;
}

uint64_t get_nano(const time_t& t, bool utc)
{
    auto p = std::chrono::system_clock::from_time_t(t);
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               p.time_since_epoch())
        .count();
}

// 利用steady_clock用于度量时间间隔，然后叠加到系统启动时间上，形成一个相对
// 与程序启动时间而言的单调的时间戳。
uint64_t get_nano(bool utc)
{
    static int64_t start_time_steady_ = 0;
    static int64_t start_time_since_epoch_ = 0;
    if (0 == start_time_steady_)
    {
        start_time_steady_ =
            std::chrono::steady_clock::now().time_since_epoch().count();
        auto now = std::chrono::system_clock::now();
        start_time_since_epoch_ =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                now.time_since_epoch())
                .count();
    }
    auto duration =
        std::chrono::steady_clock::now().time_since_epoch().count() -
        start_time_steady_;
    return start_time_since_epoch_ + duration;
}

time_t parse_date_str(std::string dateStr)
{
    // time_t current = time(NULL);
    replace_whole(dateStr, "-", "");
    if (dateStr.length() != 8) return 0;
    auto tm_year = strtol(dateStr.substr(0, 4).c_str(), nullptr, 10);
    auto tm_mon = strtol(dateStr.substr(4, 2).c_str(), nullptr, 10);
    auto tm_mday = strtol(dateStr.substr(6, 2).c_str(), nullptr, 10);
    auto tp = Tm(tm_year, tm_mon, tm_mday, 0, 0, 0, 0).to_time_point();
    return std::chrono::system_clock::to_time_t(tp);
}

uint64_t parse_milliseconds(int32_t date, int32_t timeWithMs, bool utc)
{
    time_t current = time(NULL);
    auto ltm = localtime(&current);
    auto gtm = gmtime(&current);
    auto ms = timeWithMs % 1000;
    struct tm t = *(!utc ? ltm : gtm);
    t.tm_year = date / 10000 - 1900;
    t.tm_mon = (date % 10000) / 100 - 1;
    t.tm_mday = (date % 100);
    t.tm_hour = timeWithMs / 10000000;
    t.tm_min = (timeWithMs % 10000000) / 100000;
    t.tm_sec = (timeWithMs % 100000) / 1000;
    uint64_t tim = (mktime(&t) * 1000 + ms) * 1000000;
    return tim;
}

time_t parse_today_time_str(const std::string& timestr, bool utc)
{
    time_t current = time(NULL);
    struct tm t = *(!utc ? localtime(&current) : gmtime(&current));
    auto svec = split(timestr, ':');
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    if (svec.empty()) return false;
    if (svec.size() > 0) t.tm_hour = atoi(svec[0].c_str());
    if (svec.size() > 1) t.tm_min = atoi(svec[1].c_str());
    if (svec.size() > 2) t.tm_sec = atoi(svec[2].c_str());
    return mktime(&t);
}

std::string today_time_str(time_t tv)
{
    char buf[60] = {0};
    struct tm* p;
    p = localtime(&tv); // 转换为struct tm结构的当地时间
    strftime(buf, 60, "%H%M%S000", p);
    return buf;
}

time_t time_t_from_hms(int hourMinSec, bool utc)
{
    time_t current = time(NULL);
    struct tm t;
    if (utc)
    {
        t = *gmtime(&current);
    }
    else
    {
        t = *localtime(&current);
    }

    t.tm_hour = hourMinSec / 10000;
    t.tm_min = hourMinSec / 100 % 100;
    t.tm_sec = hourMinSec % 100;
    return mktime(&t);
}

stdtp from_time_t(time_t tv)
{
    return std::chrono::system_clock::from_time_t(tv);
}

int get_current_date(bool utc)
{
    auto tt =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm;
    if (utc)
    {
        ptm = gmtime(&tt);
    }
    else
    {
        ptm = localtime(&tt);
    }
    return (ptm->tm_year + 1900) * 10000 + (ptm->tm_mon + 1) * 100 +
           ptm->tm_mday;
}

int get_current_date(time_t& t, bool utc)
{
    struct tm* ptm = nullptr;
    if (utc)
    {
        ptm = gmtime(&t);
    }
    else
    {
        ptm = localtime(&t);
    }
    return (1900 + ptm->tm_year) * 10000 + (ptm->tm_mon + 1) * 100 +
           ptm->tm_mday;
}

std::string get_current_system_time_s(const time_t& t, bool utc)
{
    char date[10] = {0};
    struct tm* ptm = nullptr;
    if (utc)
    {
        ptm = gmtime(&t);
    }
    else
    {
        ptm = localtime(&t);
    }
    sprintf(date, "%02d%02d%02d%03d", (int)ptm->tm_hour, (int)ptm->tm_min,
            (int)ptm->tm_sec, 0);
    return std::string(date);
}

std::string get_current_system_time_s(bool utc)
{
    return get_current_system_time_s(time(0), utc);
}

int get_current_system_time3(bool utc)
{
    return get_current_system_time3(time(0), utc);
}

int get_current_system_time3(const time_t& t, bool utc)
{
    struct tm* ptm = nullptr;
    if (utc)
    {
        ptm = gmtime(&t);
    }
    else
    {
        ptm = localtime(&t);
    }
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (ptm->tm_hour * 10000 + ptm->tm_min * 100 + ptm->tm_sec) * 1000 +
           (tv.tv_usec / 1000);
}

#ifdef WIN32
#include "winsock2.h"

int gettimeofday(struct timeval* tp, void* tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#else
#include "sys/time.h"
#endif
