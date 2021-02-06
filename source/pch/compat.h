#pragma once

#ifdef __cplusplus
////////////////////////////////////////////////////
///	支持平台：windows，linux
////////////////////////////////////////////////////
#if !defined(__LINUX__) && (defined(__linux__) || defined(__KERNEL__) ||       \
                            defined(_LINUX) || defined(LINUX))
#undef LINUX
#define LINUX (1)
#elif !defined(__WINDOWS__) && (defined(_WIN32) || defined(WIN32) ||           \
                                defined(__WIN32__) || defined(__Win32__))
#undef WIN32
#define WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4005)
#endif

////////////////////////////////////////////////////
///	平台通用头文件
////////////////////////////////////////////////////
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////
///	STL头文件
////////////////////////////////////////////////////
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

////////////////////////////////////////////////////
///	Windows平台头文件
////////////////////////////////////////////////////
#if defined(WIN32)
#define FD_SETSIZE 2048

#include <ws2tcpip.h>

// make a line
#include <winsock2.h>

#pragma comment(lib, "ws2_32")

#include <direct.h>
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>

#undef GetMessage

#endif

////////////////////////////////////////////////////
///	Linux平台头文件
////////////////////////////////////////////////////
#if defined(LINUX)

#include <arpa/inet.h>
#include <fcntl.h>
#include <iconv.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
// windows下 使用libev
// io中存储的是file discriptor 而 socket调用返回的是 socket discriptor,
// 需要调用_open_osfhandle 进行转换， 而linux下完全不需要
#define _open_osfhandle(v) (v)
#define INVALID_SOCKET (~0)
#endif

#ifdef DEBUG
#define NTS_ASSERT(x) assert(x)
#else
#define NTS_ASSERT
#endif

#include "compat_time.h"

////////////////////////////////////////////////////
///	数据类型差异
////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///实现了跨平台的路径管理
////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define PATH_SPLIT '\\'
#endif

#ifdef LINUX
#define PATH_SPLIT '/'
#endif

#define ALL_SPLITS "\\/$"

#define MAX_PATH_LEN 256

/////////////////////////////////////////////////////////////////////////
/// Difference for fpos_t
/////////////////////////////////////////////////////////////////////////
#ifdef LINUX
#define FPOS_SET(fposValue, longValue) (fposValue).__pos = (longValue)
#define FPOS_GET(fposValue) (fposValue).__pos
#else
#define FPOS_SET(fposValue, longValue) (fposValue) = (longValue)
#define FPOS_GET(fposValue) (fposValue)
#endif

#ifdef WIN32
#define ACCESS(fileName, accessMode) _access(fileName, accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName, accessMode) access(fileName, accessMode)
#define MKDIR(path) mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

///转换路径
///@param	target	目标
///@param	source	源
void convert_path(char* target, const char* source);

///替换标准的fopen函数
FILE* mfopen(const char* filename, const char* mode);

bool is_file_exists(const char* fileName);
bool is_directory(std::string path);
std::string& fix_path(std::string& path);
bool create_recursion_dir(const std::string& path);
int make_directory_tree(const char* pPath);
void abs_path(const char* path, std::string& out);
std::vector<std::string> split(const std::string& s, char delim);
void str_path(const std::string& pathName, std::string& path,
              std::string& name);
int strcicmp(char const* a, char const* b);
std::string& replace_whole(std::string& data, const std::string& s,
                           const std::string& t);
std::string& replace(std::string& data, const std::string& s,
                     const std::string& t);
void ltrim(std::string& s, char token = ' ');
void rtrim(std::string& s, char token = ' ');
void trim(std::string& s, char token = ' ');
std::string ltrim_copy(std::string s, char token = ' ');
std::string rtrim_copy(std::string s, char token = ' ');
std::string trim_copy(std::string s, char token = ' ');
bool is_string_nullorempty(char* str);
void assign(char* r, const char* v, int n);
void assign(char* r, const std::string& v, int n);
std::string& to_lower(std::string& str);
std::string& to_upper(std::string& str);
std::string gb2312toutf8(const std::string& gb2312str);
std::string utf8togb2312(const std::string& utf8str);
std::string wstr2str(const std::wstring& wstr);
std::wstring str2wstr(const std::string& str);
std::string base64_encode(unsigned char const* bytes_to_encode,
                          unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);
std::string read_whole_file(const std::string& filePathName);

void* x_load_lib(const char* pLibPath);
void* x_get_function(void* pLib, const char* ProcName);
void x_free_lib(void* pLib);
/////////////////////////////////////////////////////////////////////////
/// Difference of endian
/////////////////////////////////////////////////////////////////////////
#define CHANGE_ENDIAN(value) value = value;

#define DOUBLE_EQ(a, b) (a - b > -1e-5 && a - b < 1e-5)

//编码转换
#define ICONV_MAX_LEN 255

#if defined(_MSC_VER) && _MSC_VER < 1800 // VC++ 8.0 and below
#define snprintf _snprintf
#endif

#ifdef __linux__
#define LIBGreatWallRunner_API __attribute__((visibility("default")))
#else
#ifdef LIBGreatWallRunner_STATIC
#define LIBGreatWallRunner_API
#else
#if defined(GreatWallRunner_EXPORTS)
#define LIBGreatWallRunner_API __declspec(dllexport)
#else
#define LIBGreatWallRunner_API __declspec(dllimport)
#endif
#endif
#endif

#include "nlohmann/json.hpp"
using json = nlohmann::json;

#if __cplusplus < 201703L
template <class...> using void_t = void;
#endif

#if defined(LINUX)
#if __cplusplus <= 201103L
#include <memory>
namespace std
{
template <class T> struct _Unique_if
{
    typedef unique_ptr<T> _Single_object;
};

template <class T> struct _Unique_if<T[]>
{
    typedef unique_ptr<T[]> _Unknown_bound;
};

template <class T, size_t N> struct _Unique_if<T[N]>
{
    typedef void _Known_bound;
};

template <class T, class... Args>
typename _Unique_if<T>::_Single_object make_unique(Args&&... args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T> typename _Unique_if<T>::_Unknown_bound make_unique(size_t n)
{
    typedef typename remove_extent<T>::type U;
    return unique_ptr<T>(new U[n]());
}

template <class T, class... Args>
typename _Unique_if<T>::_Known_bound make_unique(Args&&...) = delete;
} // namespace std
#endif
#endif

int32_t atodate(const char* datestr);
int32_t atotime(const char* timestr);

#endif // __cplusplus
