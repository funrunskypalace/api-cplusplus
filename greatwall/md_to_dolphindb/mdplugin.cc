#include "pch/compat.h"

#include "framework/common/time.h"

#include "mdplugin.h"
#include <ctime>
#include "ort/ORDataStruct.h"
#include <regex>
#include "DolphinDB.h"
#include "Util.h"
#include "framework/market/market.h"

using namespace dolphindb;
using namespace cycnoches::ort;

int product_class_to_market_data_type(or_productclass_e product_class)
{
    switch (product_class)
    {
    case '3':
        return 100;
    case '1':
        return 500;
    case '2':
        return 300;
    default:
        return 0;
    }
}

or_productclass_e market_data_type_to_product_class(int market_data_type)
{
    switch (market_data_type)
    {
    case 100:
        return '3';
    case 500:
        return '1';
    case 300:
        return '2';
    default:
        return '0';
    }
}

Md2DolphindbPlugin::~Md2DolphindbPlugin()
{
    log(framework::info, "market plugin {} unloaded!", globalUniqueName());
}

int Md2DolphindbPlugin::load(const std::string& jsonContent)
{
    std::string temp, insid, exid;
    std::string instrumentsstr;
    bool enableregex = false;
    or_productclass_e pc;
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
            inst["exid"].get_to(exid);
            inst["insid"].get_to(insid);
            sub_instruments_.insert(std::make_pair(insid, std::regex(insid)));
            instrumentsstr += insid + ",";
            inst["enable_regex"].get_to(enableregex);
            inst["product_class"].get_to(temp);
            if (temp.length() > 0)
            {
                pc = temp[0];
            }
            else
            {
                pc = OR_PC_Stock;
            }
            auto predicator =
                std::make_shared<market::PredicatorObject>(enableregex, exid, insid);
            context_->subscribe(product_class_to_market_data_type(pc), exid,
                                 insid, predicator);
            log(framework::info, "Subscribe market data for {}.{}", exid,
                insid);
        }
        log(framework::info,
            "Doing market data to dolphindb for instruments: {}.",
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
    try
    {
        bool ret = conn.connect(dolphindb_address_, dolphindb_port_, "admin", "123456");
        if (!ret)
        {
            log(framework::critical, "Failed to connect to dolphindb server {}:{}",
                dolphindb_address_, dolphindb_port_);
            return;
        }
    }
    catch (const std::exception& ex)
    {
        log(framework::critical, "Failed to connect to dolphindb server {}:{}, {}",
            dolphindb_address_, dolphindb_port_, ex.what());
        return;
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
