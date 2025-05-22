#pragma once
#include "pch.h"

#include <string>
#include "IPotatoPlugin.h"

#ifdef SILENCE_EXPORTS
#define SILENCE_API __declspec(dllexport)
#else
#define SILENCE_API __declspec(dllimport)
#endif

class SilencePlugin : public IPotatoPlugin {
public:
    SilencePlugin();
    ~SilencePlugin();

    // These two functions must be kept as is for APO to interact & process the plugin. 
    PluginStatus process(ProcessContext& context) override;
    std::string getName() const override { return "SilencePlugin"; }

    // ...You can define your own methods and fields from here on as needed for processing. 

private:

};