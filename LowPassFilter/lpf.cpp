#include "pch.h"

#include <mutex>
#include "lpf.h"
#include <shlobj.h>

LPFBiquadPlugin::LPFBiquadPlugin() {
	// Use this constructor to initialize any parameters that would be needed during process call.
	// This can be empty if none is needed.
    filter = std::make_unique<BiquadFilter>(sampleRate);

    // Load parameters from config file
    setParamFromConfig();
    filter->setLowPass(cutoffFreqVal, qFactorVal);    
}

LPFBiquadPlugin::~LPFBiquadPlugin() {
	// Use this to cleanup any parameters that would be needed when DLL is unloaded.
	// This can be empty if none is needed.
	filter.release();
}

PluginStatus LPFBiquadPlugin::process(ProcessContext& context) {
	for (unsigned i = 0; i < context.validFrameCount; i++)
	{
		for (unsigned j = 0; j < context.numChannels; j++)
		{
			context.outputFrames[i * context.numChannels + j] = filter->processSample(context.inputFrames[i * context.numChannels + j]);
		}
	}

	return PluginStatus::CONTINUE;
}

int LPFBiquadPlugin::setParamFromConfig() {
	int ret = 0;
    std::filesystem::path configFilePath = "C:\\Users\\Public\\PotatoEffects\\config.json";
    std::string name;

    try {
        std::ifstream configFile(configFilePath);
        if (!configFile.is_open()) {
            return 1;
        }

        nlohmann::json configJson;
        configFile >> configJson; // Parse the JSON from the file
        configFile.close();

        // Check if the "plugins" array exists and is an array
        if (configJson.contains("plugins") && configJson["plugins"].is_array()) {
            for (const auto& pluginEntry : configJson["plugins"]) {
                // Safely read "name"
                if (pluginEntry.contains("name") && pluginEntry["name"].is_string()) {
                    name = pluginEntry["name"].get<std::string>();
                }
                else {
                    continue;
                }

                if (!name.compare(PLUGIN_NAME)) {
                    // Safely read "cutoff_freq"
                    if (pluginEntry.contains("cutoff_frequency") && pluginEntry["cutoff_frequency"].is_number()) {
                        cutoffFreqVal = pluginEntry["cutoff_frequency"].get<float>();
                    }

                    // Safely read "q_factor"
                    if (pluginEntry.contains("q_factor") && pluginEntry["q_factor"].is_number()) {
                        qFactorVal = pluginEntry["q_factor"].get<float>();
                    }
                }
            }
        }
        else {
            return 1;
        }
    }
    catch (const nlohmann::json::exception& e) {
        return 1;
    }
    catch (const std::exception& e) {
        return 1;
    }
	return ret;
}

// Factory Functions for Dynamic Loading
extern "C" LOWPASSFILTER_API IPotatoPlugin* createInstance() {
	return new LPFBiquadPlugin();
}

extern "C" LOWPASSFILTER_API void destroyInstance(IPotatoPlugin* plugin) {
	delete plugin;
}