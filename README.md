# PotatoPlugins
This is a supplimentary repository which holds all the effect plugins that will work with the PotatoAPO. These plugins allow user to create hotpluggable DLLs which when placed in the correct directory, can be directly read by PotatoAPO to apply the desired effects.

**NOTE:** Make sure PotatoAPO has been installed first with steps provided from [here](https://github.com/Dybios/PotatoAPO) before trying to install this plugin. Otherwise...the steps below don't really make sense at all.

**Steps to install the plugin:**
1. Build the project (x64 architecture only).
2. Create a the folder named `PotatoEffects` at path `C:\Users\Public`.
3. Copy all the generated files (.dll, .exp, .lib and .pdb) from the x64 (Release or Debug) directory to the path `C:\Users\Public\PotatoEffects`.

Once above steps are done, launch any audio playback or recording to verify that the effect has been applied.

**Removal steps:**
To remove the effect, just delete all files present in the `PotatoEffects` directory and restart the playback or recording streams.


**Development Guide:**

* If you wish to develop new plugins, you can refer to the [silence-example](https://github.com/Dybios/PotatoPlugins/tree/silence-example) branch as a starter example for creating a simple plugin for PotatoAPO. Refer to the `silence_demo.cpp` file for the processing logic.
* The [barebones](https://github.com/Dybios/PotatoPlugins/tree/barebones) branch provides the boilerplate that can be used by developers to start creating plugins by editing the source file `bare.cpp` directly. 
