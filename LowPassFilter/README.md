# LowPassFilter

This is a configuration file based plugin for a Second Order Biquad Low Pass Filter.

1. Copy the `config.json` file to `C:\Users\Public\PotatoEffects` directory.
2. Run all the steps in the parent [README.md](https://github.com/Dybios/PotatoPlugins/blob/main/README.md) from the section _"Steps to build and install the plugin"_.
3. Keep only the `LowPassFilter.dll` in `C:\Users\Public\PotatoEffects` for now. Delete all the unneeded DLLs.
4. Open `config.json` and update values in `cutoff_frequency` and `q_factor` fields as you desire.
5. If Capture endpoint was setup, restart the recording stream. If Render endpoint was setup, close and reopen the audio stream for audible effects.
