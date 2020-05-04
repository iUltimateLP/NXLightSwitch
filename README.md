# NXLightSwitch

<p align="center">
    <img height="150" src="https://i.imgur.com/fQn2owp.png">
</p>

<p align="center">
    🎮 A Nintendo Switch Homebrew Application to automatically switch the color scheme ☀🌑
</p>

# What is it?
 + Changes the Switch's color theme based on the time of day
 + Manually set the light and dark theme times
 + Needs Homebrew (CFW) installed on your Switch

# Installing
Download the [latest release](https://github.com/iUltimateLP/NXLightSwitch/releases) and extract the .zip archive to the root of your SD card. You can also drag the files to your Switch via [ftpd](https://github.com/mtheall/ftpd). 

NXLightSwitch was tested working on [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere) 0.10.4, but should work above/below, if you know what you're doing.

# Building
Compiling this project requires a [Nintendo Switch Homebrew dev environment](https://switchbrew.org/wiki/Setting_up_Development_Environment) to be installed. After that, clone this repo and run `make all` in the root of this repo. You will find all compiled files in the `out/` folder.

# Credits
I've used the following libraries, without this project wouldn't have been possible:
 + [libnx](https://github.com/switchbrew/libnx)
 + [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
 + [@benhoyt/inih](https://github.com/benhoyt/inih)
 + [@elnormous/HTTPRequest](https://github.com/elnormous/HTTPRequest)

