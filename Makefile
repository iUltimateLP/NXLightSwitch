#    NXLightSwitch for Nintendo Switch
#    Made with love by Jonathan Verbeek (jverbeek.de)

TITLE_ID = 4200000000001337

#---------------------------------------------------------------------------------
#	Scripts

# 	Phony target
.PHONY: all application sysmodule stage clean

# 	Build all
all: sysmodule
	@:

#	Build the sysmodule
sysmodule:
	@$(MAKE) -C $@
	@$(MAKE) stage

#	Stage the release into one single folder which can be copied on the SD card
stage:
	@mkdir -p out/atmosphere/contents/$(TITLE_ID)/flags
	@touch out/atmosphere/contents/$(TITLE_ID)/flags/boot2.flag
	@cp sysmodule/out/sysmodule.nsp out/atmosphere/contents/$(TITLE_ID)/exefs.nsp
	@mkdir -p out/config/NXLightSwitch
	@cp sysmodule/NXLightSwitch.ini out/config/NXLightSwitch/NXLightSwitch.ini

#	Cleans everything
clean:
	@rm -rf out/

%:
	@echo lol

#---------------------------------------------------------------------------------
