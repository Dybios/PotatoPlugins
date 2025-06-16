#include "pch.h"

#include <shlobj.h>
#include "lpf.h"

LPFBiquadPlugin::LPFBiquadPlugin() {
	// Use this constructor to initialize any parameters that would be needed during process call.
	// This can be empty if none is needed.
    filter = std::make_unique<BiquadFilter>(sampleRate);

    // Load parameters from config file
    int ret = setParamsFromConfig();
    if (!ret) {
        filter->setLowPass(parameterTable[0].parameterVal, parameterTable[1].parameterVal);
    }
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

// This reads the config.json and sets all defined parameters. 
// No need to modify this unless absolutely necessary.
int LPFBiquadPlugin::setParamsFromConfig() {
	int ret = 0;
    PWSTR pszPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Public, 0, NULL, &pszPath);
    std::wstring wSysPublicPath(pszPath);
    std::string sysPublicPath(wSysPublicPath.begin(), wSysPublicPath.end());
    sysPublicPath += "\\PotatoEffects";
    std::filesystem::path configFilePath = sysPublicPath + "\\" + CONFIG_FILE;
    std::string name;

    try {
        std::ifstream configFile(configFilePath);
        if (!configFile.is_open()) {
            return 1;
        }

        nlohmann::json configJson;
        configFile >> configJson;
        configFile.close();

        // Check if the "plugins" array exists and is an array
        if (configJson.contains("plugins") && configJson["plugins"].is_array()) {
            for (const auto& pluginEntry : configJson["plugins"]) {
                if (pluginEntry.contains("name") && pluginEntry["name"].is_string()) {
                    name = pluginEntry["name"].get<std::string>();
                }

                if (!name.compare(PLUGIN_NAME)) {
                    // If this plugin found in config, retreive and set the values.
                    for (auto& parameter : parameterTable) {
                        if (pluginEntry.contains(parameter.parameterName) && pluginEntry[parameter.parameterName].is_number()) {
                            parameter.parameterVal = pluginEntry[parameter.parameterName].get<float>();
                        }
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