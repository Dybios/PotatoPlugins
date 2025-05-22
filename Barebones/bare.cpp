#include "pch.h"
/** NOTE: This is the file which you will need to modify as per your processing logic. **/

#include "bare.h"

// Define your frame size here, if any.

// Define any globally loaded parameters here on DLL load

BarePlugin::BarePlugin() {
	// Use this constructor to initialize any parameters that would be needed during process call.
	// This can be empty if none is needed.

}

BarePlugin::~BarePlugin() {
	// Use this to cleanup any parameters that would be needed when DLL is unloaded.
	// This can be empty if none is needed.

}

PluginStatus BarePlugin::process(ProcessContext& context) {
	// Copy input to output as is.
	memcpy(context.outputFrames, context.inputFrames, context.validFrameCount);

	return PluginStatus::CONTINUE;
}


// Factory Functions for Dynamic Loading
extern "C" BAREBONES_API IPotatoPlugin* createInstance() {
	return new BarePlugin();
}

extern "C" BAREBONES_API void destroyInstance(IPotatoPlugin* plugin) {
	delete plugin;
}