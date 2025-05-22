#pragma once
#include "pch.h"

#include <string>
#include "IPotatoPlugin.h"
#include "rnnoise.h"

#ifdef NOISESUPPRESSION_EXPORTS
#define NOISESUPPRESSION_API __declspec(dllexport)
#else
#define NOISESUPPRESSION_API __declspec(dllimport)
#endif

class NoiseSuppressionPlugin : public IPotatoPlugin {
public:
    NoiseSuppressionPlugin();
    ~NoiseSuppressionPlugin();

    // These two functions must be kept as is for APO to interact & process the plugin. 
    PluginStatus process(ProcessContext& context) override;
    std::string getName() const override { return "NoiseSuppressionPlugin"; }

    // ...You can define your own methods and fields from here on as needed for processing. 

private:
    DenoiseState* st = nullptr;
};