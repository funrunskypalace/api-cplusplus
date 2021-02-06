#include "pch/compat.h"

#include "mdplugin.h"

using namespace framework;

#ifdef __linux__
extern "C" __attribute__((visibility("default"))) void
#else
extern "C" __declspec(dllexport) void __cdecl
#endif
make(market::FilterContextPtr& context, market::MarketPluginPtr& plugin)
{
    std::cerr << "[DLL Attached] Hdf5Md market plugin." << std::endl;
    plugin = std::make_shared<Md2DolphindbPlugin>(context);
}
