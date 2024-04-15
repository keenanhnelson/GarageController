include MakefileUserSpecificInfo.mk

CC = xtensa-lx106-elf-gcc
CXX = xtensa-lx106-elf-g++
PROG_SIZE = xtensa-lx106-elf-size
BUILD_LOC = ./Build
SRC_CPP = $(shell $(FIND_CMD) . -name "*.cpp")
SRC_S = $(shell $(FIND_CMD) . -name "*.S")
OBJ_CPP = $(patsubst %.cpp,$(BUILD_LOC)/%.cpp.o,$(SRC_CPP))
OBJ_S = $(patsubst %.S,$(BUILD_LOC)/%.S.o,$(SRC_S))
DEP_CPP = $(patsubst %.cpp,$(BUILD_LOC)/%.cpp.d,$(SRC_CPP))
DEP_S = $(patsubst %.S,$(BUILD_LOC)/%.S.d,$(SRC_S))
#DEFINES = -D__ets__ -DICACHE_FLASH -DF_CPU=80000000L -DLWIP_OPEN_SRC -DTCP_MSS=536 -DARDUINO=10809 -DARDUINO_ESP8266_NODEMCU -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=\"ESP8266_NODEMCU\" -DESP8266 -DLWIP_IPV6=0 -DLWIP_FEATURES=1 -DNONOSDK221=1 -DFLASHMODE_DIO
DEFINES = -DDEBUG_ESP_OTA -DDEBUG_ESP_PORT=Serial -D__ets__ -DICACHE_FLASH -DF_CPU=80000000L -DLWIP_OPEN_SRC -DTCP_MSS=536 -DARDUINO=10809 -DARDUINO_ESP8266_NODEMCU -DARDUINO_ARCH_ESP8266 -DARDUINO_BOARD=\"ESP8266_NODEMCU\" -DESP8266 -DLWIP_IPV6=0 -DLWIP_FEATURES=1 -DNONOSDK221=1 -DFLASHMODE_DIO
INCLUDES = -I./Arduino/libraries/Ticker -I./Arduino/libraries/ESP8266HTTPClient/src -I./Arduino/libraries/ESP8266WiFi/src -I./Arduino/libraries/ESP8266WebServer/src -I./Arduino/libraries/ESP8266mDNS/src -I./Arduino/libraries/ArduinoOTA -I. -I./Arduino/tools/sdk/include -I./Arduino/tools/sdk/lwip2/include -I./Arduino/tools/sdk/libc/xtensa-lx106-elf/include -I./Arduino/cores/esp8266 -I./Arduino/variants/nodemcu
COMPILE_OPTIONS = -Os -g -Wl,-EL -fno-exceptions -fno-rtti -mlongcalls -mtext-section-literals -falign-functions=4 -MP -MD -std=c++11 -ffunction-sections -fdata-sections

OUT_NAME = out
LINK_OPTIONS = -fno-exceptions -Wl,-Map -Wl,$(BUILD_LOC)/$(OUT_NAME).map -g -w -Os -nostdlib -Wl,--no-check-sections -u app_entry -u _printf_float -u _scanf_float -Wl,-static -Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,spi_flash_read
LINKER = -T./Arduino/tools/sdk/ld/eagle.flash.4m.ld
LIB_LOC = -L./Arduino/tools/sdk/lib -L./Arduino/tools/sdk/ld -L./Arduino/tools/sdk/libc/xtensa-lx106-elf/lib -L./Arduino/tools/sdk/lib/NONOSDK221
LIBS = -lhal -lphy -lpp -lnet80211 -llwip2-536-feat -lwpa -lcrypto -lmain -lwps -lbearssl -laxtls -lespnow -lsmartconfig -lairkiss -lwpa2 -lstdc++ -lm -lc -lgcc

BIN_BUILD_OPTIONS = --flash_mode dio --flash_freq 40 --flash_size 4M

all: $(BUILD_LOC)/$(OUT_NAME).bin
	$(PROG_SIZE) $(BUILD_LOC)/$(OUT_NAME).elf

$(BUILD_LOC)/$(OUT_NAME).bin: $(OBJ_CPP) $(OBJ_S)
	@$(MKDIR_CMD) -p $(BUILD_LOC)
	@echo Linking
	@$(CC) -CC -E -P -DVTABLES_IN_FLASH ./Arduino/tools/sdk/ld/eagle.app.v6.common.ld.h -o ./Arduino/tools/sdk/ld/local.eagle.app.v6.common.ld
	$(CC) $(LIB_LOC) $(LINKER) $(LINK_OPTIONS) -Wl,--start-group $(LIBS) $^ -Wl,--end-group -o $(BUILD_LOC)/$(OUT_NAME).elf
	@python ./Arduino/tools/elf2bin.py --eboot ./Arduino/bootloaders/eboot/eboot.elf --app $(BUILD_LOC)/$(OUT_NAME).elf $(BIN_BUILD_OPTIONS) --path $(XTENSA_LOC) --out $@

-include $(DEP_CPP)

$(BUILD_LOC)/%.cpp.o: %.cpp
	@$(MKDIR_CMD) -p $(@D)
	@echo Building $< 
	$(CXX) $(WARNINGS) $(COMPILE_OPTIONS) $(DEFINES) $(INCLUDES) -o $@ -c $<

$(BUILD_LOC)/%.S.o: %.S
	@$(MKDIR_CMD) -p $(@D)
	@echo Building $<
	$(CXX) $(WARNINGS) $(COMPILE_OPTIONS) $(DEFINES) $(INCLUDES) -o $@ -c $<

#1500000 115200 500000
flashSerial: FLASH_OPTIONS = -b 500000 -p $(COM_PORT) -a soft_reset write_flash --flash_size 4MB-c1
flashSerial: $(BUILD_LOC)/$(OUT_NAME).bin
	esptool.py $(FLASH_OPTIONS) 0x00000 $(BUILD_LOC)/$(OUT_NAME).bin

flashOTA: $(BUILD_LOC)/$(OUT_NAME).bin
	python ./Arduino/tools/espota.py -i $(ESP_IP_ADDRESS) -p $(OTA_PORT) --auth=$(OTA_AUTH) -f $(BUILD_LOC)/$(OUT_NAME).bin

clean:
	rm -r $(BUILD_LOC)/*
