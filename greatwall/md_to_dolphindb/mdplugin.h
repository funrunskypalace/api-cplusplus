#pragma once

#include <iostream>
#include <memory>
#include <regex>

#include "absl/synchronization/notification.h"
#include "framework/common/common.h"
#include "framework/market/mkt_plugin.h"
#include "implugin/plugin.h"
#include "ort/ORDataStruct.h"

using namespace framework;
using namespace imsimple;
using namespace cycnoches;

class Md2DolphindbPlugin : public market::MarketPlugin
{
    std::atomic_bool is_inited_ = {false};

public:
    Md2DolphindbPlugin(market::FilterContextPtr& context)
        : market::MarketPlugin(context)
    {
    }

    ~Md2DolphindbPlugin();
    virtual MarketPluginId globalUniqueName() override
    {
        return "md_to_dolphindb";
    };
    bool isInited() { return is_inited_; }
    virtual int load(const std::string& jsonContent = "") override;
    virtual void subscribe(int dataType, const std::string& exId,
                           const std::string& insId) override;
    virtual void unsubscribe(int dataType, const std::string& exId,
                             const std::string& insId) override;

protected:
    virtual void onLoaded() override;
    virtual void onData(const int32_t dataType, const char* data,
                        size_t datasiz) override;
    virtual void onUnload(std::string& reason) override;

private:
    std::unordered_map<std::string, std::regex> sub_instruments_;
    std::string dolphindb_address_ = {"127.0.0.1"};
    int32_t dolphindb_port_ = {8503};
};
