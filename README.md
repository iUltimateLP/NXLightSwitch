# NXLightSwitch
🎮 A Nintendo Switch Homebrew sysmodule to automatically switch the color scheme ☀🌑

# sysmodule

This template is for a sysmodule where the output .nsp is used by Atmosphère at: `sdmc:/atmosphere/contents/<titleid>/exefs.nsp`. To load the sysmodule at boot, the following file should exist: `sdmc:/atmosphere/contents/<titleid>/flags/boot2.flag`.

Where titleid is whatever you specify. Update the `$(TARGET).json` file with the titleid and any other changes.
