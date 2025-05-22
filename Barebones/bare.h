#pragma once
#include "pch.h"

#include <string>
#include "IPotatoPlugin.h"

#ifdef BAREBONES_EXPORTS
#define BAREBONES_API __declspec(dllexport)
#else
#define BAREBONES_API __declspec(dllimport)
#endif

class BarePlugin : public IPotatoPlugin {
public:
    BarePlugin();
    ~BarePlugin();

    // These two functions must be kept as is for APO to interact & process the plugin. 
    PluginStatus process(ProcessContext& context) override;
    std::string getName() const override { return "BarePlugin"; }

    // ...You can define your own methods and fields from here on as needed for processing. 

private:

};