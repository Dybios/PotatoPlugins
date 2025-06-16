#pragma once
#include "pch.h"

#include <string>
#include "IPotatoPlugin.h"

#include <filesystem>
#include "json.hpp"
#include <fstream>

#include "BiquadFilters.h"

#ifdef LOWPASSFILTER_EXPORTS
#define LOWPASSFILTER_API __declspec(dllexport)
#else
#define LOWPASSFILTER_API __declspec(dllimport)
#endif

const std::string CONFIG_FILE = "config.json";
const std::string PLUGIN_NAME = "LPFBiquadPlugin";

struct Parameter {
    const char* parameterName;
    float parameterVal;
};

class LPFBiquadPlugin : public IPotatoPlugin {
public:
    LPFBiquadPlugin();
    ~LPFBiquadPlugin();

    // These two functions must be kept as is for APO to interact & process the plugin. 
    PluginStatus process(ProcessContext& context) override;
    std::string getName() const override { return PLUGIN_NAME; }

    // Set all processing parameters with their default values here (only float for now)
    Parameter parameterTable[2] = {
        { "cutoff_frequency", 0.0f },
        { "q_factor", 0.0f },
    };

private:
    float sampleRate = 44100.0f; // TODO: Get this dynamically from the audio stream playing
    std::unique_ptr<BiquadFilter> filter;

    int setParamsFromConfig();
};