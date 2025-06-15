#pragma once
#include "pch.h"

#include <string>
#include "IPotatoPlugin.h"

#include <memory>
#include <filesystem>
#include "json.hpp" // For reading config file
#include <fstream>

#include "BiquadFilters.h"
#include "configWatcher.h"

#ifdef LOWPASSFILTER_EXPORTS
#define LOWPASSFILTER_API __declspec(dllexport)
#else
#define LOWPASSFILTER_API __declspec(dllimport)
#endif

const std::string CONFIG_FILE = "config.json";
const std::string PLUGIN_NAME = "LPFBiquadPlugin";

class LPFBiquadPlugin : public IPotatoPlugin {
public:
    LPFBiquadPlugin();
    ~LPFBiquadPlugin();

    // These two functions must be kept as is for APO to interact & process the plugin. 
    PluginStatus process(ProcessContext& context) override;
    std::string getName() const override { return "LPFBiquadPlugin"; }

    // ...You can define your own methods and fields from here on as needed for processing.

private:
    float sampleRate = 44100.0f; // TODO: Get this dynamically from the audio stream playing
    std::unique_ptr<BiquadFilter> filter;
    std::unique_ptr<ConfigWatcher> watcher;
    std::mutex paramMutex;

    float cutoffFreqVal = 0.0f;
    float qFactorVal = 0.0f;

    int setParamFromConfig();
};