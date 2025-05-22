# PotatoPlugins
This is a supplimentary repository which holds all the effect plugins that will work with the PotatoAPO. These plugins allow user to create hotpluggable DLLs which when placed in the correct directory, can be directly read by PotatoAPO to apply the desired effects.

**NOTE:** Make sure PotatoAPO has been installed first with steps provided from [here](https://github.com/Dybios/PotatoAPO) before trying to install this plugin. Otherwise...the steps below don't really make sense at all.

**Steps to build and install the plugin:**
1. Build the project (x64 only).
2. Create a the folder named `PotatoEffects` at path `C:\Users\Public`.
3. Copy all the generated `.dll` files from the x64 (Release or Debug) directory to the path `C:\Users\Public\PotatoEffects`.

Once above steps are done, launch any audio playback or recording to verify that the effect has been applied.

**Removal steps:**
To remove the effect, just delete all `.dll` files present in the `PotatoEffects` directory and restart the playback or recording streams.


**Development Guide:**

* If you wish to develop new plugins, you can then refer to the project named [Barebones](https://github.com/Dybios/PotatoPlugins/tree/main/Barebones) which has all the boilerplate that you can use in your new project. As a basic set of steps, you will need to do the following:
    * Create a new Windows DLL project under the PotatoPlugin solution.
    * Right click on Project. Go to Configuration Properties > C/C++ > General > Additional Include Directories. Add the path `..\include` to include the necessary interface and context header files.
    * Copy the source files `bare.cpp` and `bare.h` from the Barebones example folder location onto your project.
    * Expand the new project and right click on Source Files. Click on Add > Existing Item. Select the `.h` and `.cpp` files from the previous step and add them.
    * Once added, you can continue editing the source files as per your processing logic.
* As a starter example, you can refer to the project named [Silence](https://github.com/Dybios/PotatoPlugins/tree/main/Silence) for creating a simple "mute" plugin for PotatoAPO. Refer to the `silence.cpp` file for the processing logic.
