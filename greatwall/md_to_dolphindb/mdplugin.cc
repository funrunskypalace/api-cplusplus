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
            cfg["strategy"]["dolphindb_port"].get_to(dolphindb_port_);
        }

        for (auto inst : cfg["strategy"]["instruments"])
        {
            inst["insid"].get_to(temp);
            sub_instruments_.insert(std::make_pair(temp, std::regex(temp)));
            instrumentsstr += temp + ",";
        }
        log(framework::info, "Doing market data to dolphindb for instruments: {}.",
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
    DBConnection conn;
    bool ret = conn.connect(dolphindb_address_, dolphindb_port_);
    if (!ret)
    {
        log(framework::warn, "Failed to connect to the server {}:{}", dolphindb_address_,
            dolphindb_port_);
        return ;
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
