#include "pch.h"
/** NOTE: This is the file which you will need to modify as per your processing logic. **/

#include "silence.h"

// Define your frame size here, if any.

// Define any globally loaded parameters here on DLL load

SilencePlugin::SilencePlugin() {
	// Use this constructor to initialize any parameters that would be needed during process call.
	// This can be empty if none is needed.

}

SilencePlugin::~SilencePlugin() {
	// Use this to cleanup any parameters that would be needed when DLL is unloaded.
	// This can be empty if none is needed.

}

PluginStatus SilencePlugin::process(ProcessContext& context) {
	// Zero out all outputFrames.
	for (unsigned i = 0; i < context.validFrameCount; i++)
	{
		for (unsigned j = 0; j < context.numChannels; j++)
		{
			context.outputFrames[i * context.numChannels + j] = 0.0;
		}
	}

	return PluginStatus::CONTINUE;
}


// Factory Functions for Dynamic Loading
extern "C" SILENCE_API IPotatoPlugin* createInstance() {
	return new SilencePlugin();
}

extern "C" SILENCE_API void destroyInstance(IPotatoPlugin* plugin) {
	delete plugin;
}