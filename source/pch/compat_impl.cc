#include "pch/compat.h"

#include <iterator>
#if (defined WINDOWS) || (defined WIN32)
#include <WinSock2.h>

#include <direct.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#else
#include <dirent.h>
#include <dlfcn.h>
#endif
#include <algorithm>
#include <cctype>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <spdlog/spdlog.h>

#include "framework/common/common.h"

std::string gb2312toutf8(const std::string& gb2312str)
{
#ifdef WIN32
    const char* GBK_LOCALE_NAME = ".936"; // GBK在windows下的locale name
#else
    const char* GBK_LOCALE_NAME = "zh_CN.GBK"; // GBK在windows下的locale name
#endif

    struct codecvt : std::codecvt_byname<wchar_t, char, mbstate_t>
    {
        codecvt(const char* loc)
            : std::codecvt_byname<wchar_t, char, mbstate_t>(loc)
        {
        }
        ~codecvt() {}
    };

    //构造GBK与wstring间的转码器（wstring_convert在析构时会负责销毁codecvt_byname，所以不用自己delete）
    // LWG issue 721
    // http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#721
    std::wstring_convert<codecvt> cv1(new codecvt(GBK_LOCALE_NAME));
    std::wstring tmp_wstr = cv1.from_bytes(gb2312str);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cv2;
    std::string utf8_str = cv2.to_bytes(tmp_wstr);
    return utf8_str;
}

std::string utf8togb2312(const std::string& utf8str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
    std::wstring wTemp;
    try
    {
        wTemp = cutf8.from_bytes(utf8str);
    }
    catch (const std::exception&)
    {
        return utf8str;
    }
#ifdef _MSC_VER
    std::locale loc("zh-CN");
#else
    std::locale loc("zh_CN.GB18030");
#endif
    const wchar_t* pwszNext = nullptr;
    char* pszNext = nullptr;
    mbstate_t state = {};

    std::vector<char> buff(wTemp.size() * 2);
    int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t>>(loc).out(
        state, wTemp.data(), wTemp.data() + wTemp.size(), pwszNext, buff.data(),
        buff.data() + buff.size(), pszNext);

    if (std::codecvt_base::ok == res)
    {
        return std::string(buff.data(), pszNext);
    }
    return "";
}

std::string wstr2str(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
std::wstring str2wstr(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string convert_path_backslash(std::string pathStr)
{
    if (!pathStr.empty())
    {
        while (pathStr.find(R"(\)") != pathStr.npos)
        {
            pathStr[pathStr.find(R"(\)")] = '/';
        }
    }
    return pathStr;
}

void convert_path(char* target, const char* source)
{
    const char* s;
    char* t;
    for (s = source, t = target; ((s - source) < MAX_PATH_LEN) && (*s != '\0');
         s++, t++)
    {
        if (strchr(ALL_SPLITS, *s) != NULL)
        {
            *t = PATH_SPLIT;
        }
        else
        {
            *t = *s;
        }
    }
    *t = '\0';
}

FILE* mfopen(const char* filename, const char* mode)
{
    char actualName[MAX_PATH_LEN + 1];
    convert_path(actualName, filename);
    return fopen(actualName, mode);
}

void assign(char* r, const char* v, int n)
{
    if (v == nullptr)
    {
        r[0] = 0;
        return;
    }
    size_t siz = strnlen(v, n - 1);
    // size_t siz = std::min(size_t(n - 1), strlen(v));
    strncpy(r, v, siz);
    r[siz] = 0;
}

void assign(char* r, int v, int n)
{
    char szTmp[20] = {0};
    sprintf(szTmp, "%d", v);
    assign(r, szTmp, n);
}

void assign(char* r, unsigned int v, int n)
{
    char szTmp[20] = {0};
    sprintf(szTmp, "%d", v);
    assign(r, szTmp, n);
}

void assign(char* r, char v, int n)
{
    char szTmp[20] = {0};
    sprintf(szTmp, "%c", v);
    assign(r, szTmp, n);
}

void assign(char* r, const std::string& v, int n) { assign(r, v.c_str(), n); }

#ifdef __linux__
char* itoa(int value, char* result, int base)
{
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ =
            "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"
                [35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}
#endif

std::string& replace_whole(std::string& data, const std::string& s,
                           const std::string& t)
{
    std::string::size_type n = 0;
    while ((n = data.find(s, n)) != std::string::npos)
    {
        data.replace(n, s.size(), t);
        n += t.size() -
             1; // “-1”是为了退回一个字符，这样\t\t就可以被循环替换为\t0\t
    }
    return data;
}

std::string& replace(std::string& data, const std::string& s,
                     const std::string& t)
{
    std::string::size_type n = 0;
    while ((n = data.find(s, n)) != std::string::npos)
    {
        data.replace(n, s.size(), t);
        n += t.size();
    }
    return data;
}

// trim from start (in place)
void ltrim(std::string& s, char token)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [token](int ch) {
                // return !std::isspace(ch);
                return token != ch;
            }));
}

// trim from end (in place)
void rtrim(std::string& s, char token)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [token](int ch) { return token != ch; })
                .base(),
            s.end());
}

// trim from both ends (in place)
void trim(std::string& s, char token)
{
    ltrim(s, token);
    rtrim(s, token);
}

// trim from start (copying)
std::string ltrim_copy(std::string s, char token)
{
    ltrim(s, token);
    return s;
}

// trim from end (copying)
std::string rtrim_copy(std::string s, char token)
{
    rtrim(s, token);
    return s;
}

// trim from both ends (copying)
std::string trim_copy(std::string s, char token)
{
    trim(s, token);
    return s;
}

bool is_string_nullorempty(char* str)
{
    return (str == nullptr || strlen(str) == 0);
}

/**
 * \brief
 * \tparam Out
 * \param s
 * \param delim
 * \param result
 */
template <typename Out> void split(const std::string& s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        trim(item, ' ');
        if (item.length() == 0) continue;
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void abs_path(const char* path, std::string& out)
{
#ifdef _WIN32
    char resolved_path[_MAX_PATH] = {0};
    auto ret = _fullpath(nullptr, path, 0);
    if (ret != nullptr)
    {
        assign(resolved_path, ret, _MAX_PATH);
        out = resolved_path;
    }
#else
    // linux release有个坑，需要大点的空间
#define max_path 40960
    char resolved_path[max_path];
    auto ret = realpath(path, resolved_path);
    if (ret == nullptr)
    {
        spdlog::error("Failed to get realpath of {}, errorno = {}", path,
                      errno);
    }
    out = resolved_path;
#endif
}

void str_path(const std::string& pathName, std::string& path, std::string& name)
{
    std::string full = pathName;
    std::replace(full.begin(), full.end(), '\\', '/');
    auto it = full.find_last_of('/');
    if (it == std::string::npos)
    {
        path = "./";
        name = full;
    }
    else
    {
        path = full.substr(0, it + 1);
        name = full.substr(std::min(it + 1, full.size()));
    }
}

void* x_load_lib(const char* pLibPath)
{
    if (pLibPath == nullptr) return nullptr;

#if (defined WINDOWS) || (defined WIN32)
    return LoadLibraryExA(pLibPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
#else
    return dlopen(pLibPath, RTLD_NOW | RTLD_GLOBAL);
#endif
}

void* x_get_function(void* pLib, const char* ProcName)
{
    if (pLib == nullptr) return nullptr;
#if (defined WINDOWS) || (defined WIN32)
    return GetProcAddress((HMODULE)pLib, ProcName);
#else
    return (dlsym(pLib, ProcName));
#endif
}

void x_free_lib(void* pLib)
{
    if (pLib == nullptr) return;

#if (defined WINDOWS) || (defined WIN32)
    FreeLibrary((HMODULE)pLib);
#else
    dlclose(pLib);
#endif
}

bool is_file_exists(const char* fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

bool is_directory(std::string path)
{
#ifdef WIN32
    return PathIsDirectoryA(path.c_str()) ? true : false;
#else
    DIR* pdir = opendir(path.c_str());
    if (pdir == NULL)
    {
        return false;
    }
    else
    {
        closedir(pdir);
        pdir = NULL;
        return true;
    }
#endif
}

std::string& fix_path(std::string& path)
{
    if (path.empty())
    {
        return path;
    }
    for (std::string::iterator iter = path.begin(); iter != path.end(); ++iter)
    {
        if (*iter == '\\')
        {
            *iter = '/';
        }
    }
    if (path.at(path.length() - 1) != '/')
    {
        path += "/";
    }
    return path;
}

// dirPath is treated as directory only! and will be add '/' postfix
bool create_recursion_dir(const std::string& dirPath)
{
    auto path = dirPath;
    if (path.length() == 0) return true;
    std::string sub;
    fix_path(path);

    std::string::size_type pos = path.find('/');
    while (pos != std::string::npos)
    {
        std::string cur = path.substr(0, pos - 0);
        if (cur.length() > 0 && !is_directory(cur))
        {
            bool ret = false;
#ifdef WIN32
            ret = CreateDirectoryA(cur.c_str(), nullptr) ? true : false;
#else
            ret = (mkdir(cur.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
            if (!ret)
            {
                return false;
            }
        }
        pos = path.find('/', pos + 1);
    }

    return true;
}

/**
 * \brief create directories before last '\\' or '/'
 * \param path if not ends with '\\' or '/', will not create directory according
 * to last section in path. \return
 */
int make_directory_tree(const char* pPath)
{
    auto path = std::string(pPath);
    std::string p, n;
    str_path(path, p, n);
    path = p;
    return create_recursion_dir(path) ? 0 : 1;
}

int strcicmp(char const* a, char const* b)
{
    if (a == nullptr)
    {
        return -1;
    }
    if (b == nullptr)
    {
        return 1;
    }
    for (;; a++, b++)
    {
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a) return d;
    }
}

std::string& to_lower(std::string& data)
{
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return data;
}

std::string& to_upper(std::string& data)
{
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return data;
}

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode,
                          unsigned int in_len)
{
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                              ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                              ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++) ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++) char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] =
            ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] =
            ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];

        while ((i++ < 3)) ret += '=';
    }

    return ret;
}

std::string base64_decode(std::string const& encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') &&
           is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] =
                (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) +
                              ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++) ret += char_array_3[i];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 4; j++) char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] =
            (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] =
            ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

std::string read_whole_file(const std::string& filePathName)
{
    if (!is_file_exists(filePathName.c_str()))
    {
        return "";
    }
    std::ifstream t(filePathName);
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

int32_t atodate(const char* datestr)
{
    std::string dateobj(datestr);
    replace_whole(dateobj, "/", "");
    replace_whole(dateobj, "-", "");
    trim(dateobj, ' ');
    if (dateobj.length() < 8)
    {
        return 19700101;
    }
    return atoi(dateobj.substr(0, 8).c_str());
}

int32_t atotime(const char* timestr)
{
    std::string timeobj(timestr);
    replace_whole(timeobj, ":", "");
    replace_whole(timeobj, ".", "");
    trim(timeobj, ' ');
    if (timeobj.length() < 6)
    {
        return 0;
    }
    else if (timeobj.length() < 9)
    {
        return atoi(timeobj.substr(0, 6).c_str()) * 1000;
    }
    else
        return atoi(timeobj.c_str());
}