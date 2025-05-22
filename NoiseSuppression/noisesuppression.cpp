#include "pch.h"
/** NOTE: This is the file which you will need to modify as per your processing logic. **/

#include "noisesuppression.h"

// Define your frame size here, if any.
#define FRAME_SIZE 480

NoiseSuppressionPlugin::NoiseSuppressionPlugin() {
	// Use this constructor to initialize any parameters that would be needed during process call.
	// This can be empty if none is needed.

	st = rnnoise_create(NULL);
}

NoiseSuppressionPlugin::~NoiseSuppressionPlugin() {
	// Use this to cleanup any parameters that would be needed when DLL is unloaded.
	// This can be empty if none is needed.

	rnnoise_destroy(st);
}

PluginStatus NoiseSuppressionPlugin::process(ProcessContext& context) {
	float processBuffer[FRAME_SIZE];
	unsigned int bufFrameCount = 0;

	for (int i = 0; i < context.validFrameCount; i++) {
		processBuffer[bufFrameCount] = context.inputFrames[i * context.numChannels];
		bufFrameCount++;

		if (i == FRAME_SIZE) {
			// Process a full frame
			rnnoise_process_frame(st, processBuffer, processBuffer);

			// Copy processed frame back to the output buffer
			context.outputFrames[i * context.numChannels] = processBuffer[i];

			bufFrameCount = 0; // reset the buffer index
		}
	}
	return PluginStatus::CONTINUE;
}


// Factory Functions for Dynamic Loading
extern "C" NOISESUPPRESSION_API IPotatoPlugin* createInstance() {
	return new NoiseSuppressionPlugin();
}

extern "C" NOISESUPPRESSION_API void destroyInstance(IPotatoPlugin* plugin) {
	delete plugin;
}