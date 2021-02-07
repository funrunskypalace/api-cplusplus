#include "pch/compat.h"

#include "mdplugin.h"

using namespace framework;

#ifdef __linux__
extern "C" __attribute__((visibility("default"))) void
#else
extern "C" __declspec(dllexport) void __cdecl
#endif
make_mkt_plugin(market::FilterContextPtr& context, market::MarketPluginPtr& plugin)
{
    std::cerr << "[DLL Attached] Md2DolphinDB market plugin." << std::endl;
    plugin = std::make_shared<Md2DolphindbPlugin>(context);
}
