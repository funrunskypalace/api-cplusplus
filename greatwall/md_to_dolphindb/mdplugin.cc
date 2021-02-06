#include "pch/compat.h"

#include "framework/common/time.h"

#include "mdplugin.h"
#include <ctime>
#include "ort/ORDataStruct.h"
#include <regex>
#include "DolphinDB.h"
#include "Util.h"

using namespace dolphindb;
using namespace cycnoches::ort;

Md2DolphindbPlugin::~Md2DolphindbPlugin()
{
    log(framework::info, "Md2DolphindbPlugin unloaded!");
}

int Md2DolphindbPlugin::load(const std::string& jsonContent)
{
    std::string temp;
    std::string instrumentsstr;
    try
    {
        nlohmann::json cfg = nlohmann::json::parse(jsonContent);
        if (cfg["strategy"].contains("dolphindb_address"))
        {
            cfg["strategy"]["dolphindb_address"].get_to(dolphindb_address_);
        }

        for (auto inst : filecfg["strategy"]["instruments"])
        {
            inst["insid"].get_to(temp);
            sub_instruments_.insert(std::make_pair(temp, std::regex(temp)));
            instrumentsstr += temp + ",";
        }
        log(info, "Doing market data to dolphindb for instruments: {}.",
            instrumentsstr);
    }
    catch (...)
    {
        log(framework::err, "JSON Parse error");
        return -1;
    }

    return framework::F_NOERR;
}

void Md2DolphindbPlugin::onLoaded()
{
    for (auto plugin : any_other_plugins_)
    {
        log(framework::info, "Start with plugin:{}", plugin.second->id());
        plugin.second->doCommandAsync(
            commands_[plugin.first],
            [&](int32_t retval, const std::string& cmdName,
                const std::string& retmsg) -> int {
                if ("loadticks" == cmdName)
                {
                    if (retmsg.size() != sizeof(ORDepthMarketDataField))
                    {
                        if (0 != retval)
                        {
                            log(framework::err,
                                "Failed command {} execution, {}", cmdName,
                                retmsg);
                            return retval;
                        }
                        else
                        {
                            log(framework::info, "{} callback: {}", cmdName,
                                retmsg);
                            return retval;
                        }
                    }
                    auto depth =
                        reinterpret_cast<const ORDepthMarketDataField*>(
                            retmsg.data());
                    // @todo: writing data to dolphindb
                    return 0;
                }
                return 0;
            });
    }
}

void Md2DolphindbPlugin::subscribe(int dataType, const std::string& exId,
                                   const std::string& insId)
{
}

void Md2DolphindbPlugin::unsubscribe(int dataType, const std::string& exId,
                                     const std::string& insId)
{
}

void Md2DolphindbPlugin::onData(const int32_t dataType, const char* data,
                                size_t datasiz)
{
}

void Md2DolphindbPlugin::onUnload(std::string& reason)
{
    log(framework::info, "Unload market plugin {}", instanceId());
    // only release if is inited.
    if (is_inited_)
    {
    }
};
