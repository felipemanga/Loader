# cross-platform directory manipulation
ifeq ($(shell echo $$OS),$$OS)
    MAKEDIR = if not exist "$(1)" mkdir "$(1)"
    RM = rmdir /S /Q "$(1)"
else
    MAKEDIR = '$(SHELL)' -c "mkdir -p \"$(1)\""
    RM = '$(SHELL)' -c "rm -rf \"$(1)\""
endif

OBJDIR := BUILD
SRCDIR := $(CURDIR)

.PHONY: clean

.SUFFIXES:
.SUFFIXES: .cpp .o

vpath %.cpp .
vpath %.c .
vpath %.h .

# Boiler-plate
###############################################################################
# Project settings

PROJECT := Loader

BPROJECT := BUILD/Loader
POP := ../BUILD/LOA.DER

# Project settings
###############################################################################
# Objects and Paths

OBJECTS += BUILD/SystemInit.s.o
OBJECTS += BUILD/main.o
OBJECTS += BUILD/fsapi.o
OBJECTS += BUILD/startup.o

OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/BusIn.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/BusInOut.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/BusOut.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/CAN.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/CallChain.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/Ethernet.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/FileBase.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/FileLike.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/FilePath.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/FileSystemLike.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/I2C.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/I2CSlave.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/InterruptIn.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/InterruptManager.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/LocalFileSystem.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/RawSerial.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/SPI.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/SPISlave.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/Serial.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/SerialBase.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/Stream.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/Ticker.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/Timeout.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/Timer.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/TimerEvent.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/assert.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/board.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/error.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/gpio.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/lp_ticker_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/mbed_interface.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/pinmap_common.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/retarget.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/rtc_time.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/semihost_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/ticker_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/us_ticker_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/common/wait_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X/cmsis_nvic.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X/system_LPC11U6x.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/analogin_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/gpio_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/gpio_irq_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/i2c_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/pinmap.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/pwmout_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/rtc_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/serial_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/sleep.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/spi_api.o
OBJECTS += BUILD/PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X/us_ticker.o
OBJECTS += BUILD/cardlib/SDFileSystem.o
OBJECTS += BUILD/cardlib/FATFileSystem/FATFileSystem.o
OBJECTS += BUILD/cardlib/FATFileSystem/FATFileHandle.o
OBJECTS += BUILD/cardlib/FATFileSystem/FATDirHandle.o
OBJECTS += BUILD/cardlib/FATFileSystem/ChaN/ccsbcs.o
OBJECTS += BUILD/cardlib/FATFileSystem/ChaN/diskio.o
OBJECTS += BUILD/cardlib/FATFileSystem/ChaN/ff.o


INCLUDE_PATHS += -I./.
INCLUDE_PATHS += -I./PokittoLib
INCLUDE_PATHS += -I./PokittoLib/POKITTO_CORE
INCLUDE_PATHS += -I./PokittoLib/POKITTO_HW
INCLUDE_PATHS += -I./PokittoLib/libpff
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/api
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/common
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/hal
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/cmsis
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X/TOOLCHAIN_GCC_ARM
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X/TOOLCHAIN_GCC_ARM/TARGET_LPC11U68
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TOOLCHAIN_GCC
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/hal
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP
INCLUDE_PATHS += -I./PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X
INCLUDE_PATHS += -I./cardlib
INCLUDE_PATHS += -I./cardlib/FATFileSystem
INCLUDE_PATHS += -I./cardlib/FATFileSystem/ChaN


LIBRARY_PATHS :=
LIBRARIES :=
LINKER_SCRIPT := loader005.ld

# Objects and Paths
###############################################################################
# Tools and Flags

CDB := cdb

AS      = $(CDB)	 'arm-none-eabi-gcc' '-x' 'assembler-with-cpp' '-c' '-Wall' '-Wextra' '-Wno-unused-parameter' '-Wno-missing-field-initializers' '-fmessage-length=0' '-fno-exceptions' '-fno-builtin' '-ffunction-sections' '-fdata-sections' '-funsigned-char' '-MMD' '-fno-delete-null-pointer-checks' '-fomit-frame-pointer' '-Os' '-g1' '-DMBED_RTOS_SINGLE_THREAD' '-mcpu=cortex-m0plus' '-mthumb'
CC      = $(CDB)	 'arm-none-eabi-gcc' '-std=gnu89' '-c' '-Wall' '-Wextra' '-Wno-unused-parameter' '-Wno-missing-field-initializers' '-fmessage-length=0' '-fno-exceptions' '-fno-builtin' '-ffunction-sections' '-fdata-sections' '-funsigned-char' '-MMD' '-fno-delete-null-pointer-checks' '-fomit-frame-pointer' '-Os' '-g1' '-DMBED_RTOS_SINGLE_THREAD' '-mcpu=cortex-m0plus' '-mthumb'
CPP     = $(CDB)	 'arm-none-eabi-g++' '-std=gnu++14' '-fno-rtti' '-Wvla' '-c' '-Wall' '-Wextra' '-Wno-unused-parameter' '-Wno-missing-field-initializers' '-fmessage-length=0' '-fno-exceptions' '-fno-builtin' '-ffunction-sections' '-fdata-sections' '-funsigned-char' '-MMD' '-fno-delete-null-pointer-checks' '-fomit-frame-pointer' '-Os' '-g1' '-DMBED_RTOS_SINGLE_THREAD' '-mcpu=cortex-m0plus' '-mthumb'
LD      = $(CDB)	 'arm-none-eabi-gcc' '-Os'
ELF2BIN = 'arm-none-eabi-objcopy'
PREPROC = 'arm-none-eabi-cpp' '-E' '-P' '-Wl,--gc-sections' '-Wl,--wrap,_malloc_r' '-Wl,--wrap,_free_r' '-Wl,--wrap,_realloc_r' '-Wl,--wrap,_memalign_r' '-Wl,--wrap,_calloc_r' '-Wl,--wrap,exit' '-Wl,--wrap,atexit' '-Wl,-n' '--specs=nano.specs' '-mcpu=cortex-m0plus' '-mthumb'


C_FLAGS += -flto
CXX_FLAGS += -flto
C_FLAGS += -DMINILOADHIGH
CXX_FLAGS += -DMINILOADHIGH
C_FLAGS += -D__SINGLE_THREAD__
CXX_FLAGS += -D__SINGLE_THREAD__

C_FLAGS += -std=gnu89
C_FLAGS += -DTARGET_LPC11U68
C_FLAGS += -D__MBED__=1
C_FLAGS += -DDEVICE_I2CSLAVE=1
C_FLAGS += -DTARGET_LIKE_MBED
C_FLAGS += -DTARGET_NXP
C_FLAGS += -D__MBED_CMSIS_RTOS_CM
C_FLAGS += -DDEVICE_RTC=1
C_FLAGS += -DTOOLCHAIN_object
C_FLAGS += -D__CMSIS_RTOS
C_FLAGS += -DTOOLCHAIN_GCC
C_FLAGS += -DTARGET_CORTEX_M
C_FLAGS += -DTARGET_M0P
C_FLAGS += -DTARGET_UVISOR_UNSUPPORTED
C_FLAGS += -DMBED_BUILD_TIMESTAMP=1526086019.89
C_FLAGS += -DDEVICE_SERIAL=1
C_FLAGS += -DDEVICE_INTERRUPTIN=1
C_FLAGS += -DTARGET_LPCTarget
C_FLAGS += -DTARGET_CORTEX
C_FLAGS += -DDEVICE_I2C=1
C_FLAGS += -D__CORTEX_M0PLUS
C_FLAGS += -DTARGET_FF_ARDUINO
C_FLAGS += -DTARGET_RELEASE
C_FLAGS += -DARM_MATH_CM0PLUS
C_FLAGS += -DTARGET_LPC11U6X
C_FLAGS += -DDEVICE_SLEEP=1
C_FLAGS += -DTOOLCHAIN_GCC_ARM
C_FLAGS += -DDEVICE_SPI=1
C_FLAGS += -DDEVICE_ANALOGIN=1
C_FLAGS += -DDEVICE_PWMOUT=1
C_FLAGS += -DTARGET_LIKE_CORTEX_M0


CXX_FLAGS += -std=gnu++14
CXX_FLAGS += -fno-rtti
CXX_FLAGS += -Wvla
CXX_FLAGS += -DTARGET_LPC11U68
CXX_FLAGS += -D__MBED__=1
CXX_FLAGS += -DDEVICE_I2CSLAVE=1
CXX_FLAGS += -DTARGET_LIKE_MBED
CXX_FLAGS += -DTARGET_NXP
CXX_FLAGS += -D__MBED_CMSIS_RTOS_CM
CXX_FLAGS += -DDEVICE_RTC=1
CXX_FLAGS += -DTOOLCHAIN_object
CXX_FLAGS += -D__CMSIS_RTOS
CXX_FLAGS += -DTOOLCHAIN_GCC
CXX_FLAGS += -DTARGET_CORTEX_M
CXX_FLAGS += -DTARGET_M0P
CXX_FLAGS += -DTARGET_UVISOR_UNSUPPORTED
CXX_FLAGS += -DMBED_BUILD_TIMESTAMP=1526086019.89
CXX_FLAGS += -DDEVICE_SERIAL=1
CXX_FLAGS += -DDEVICE_INTERRUPTIN=1
CXX_FLAGS += -DTARGET_LPCTarget
CXX_FLAGS += -DTARGET_CORTEX
CXX_FLAGS += -DDEVICE_I2C=1
CXX_FLAGS += -D__CORTEX_M0PLUS
CXX_FLAGS += -DTARGET_FF_ARDUINO
CXX_FLAGS += -DTARGET_RELEASE
CXX_FLAGS += -DARM_MATH_CM0PLUS
CXX_FLAGS += -DTARGET_LPC11U6X
CXX_FLAGS += -DDEVICE_SLEEP=1
CXX_FLAGS += -DTOOLCHAIN_GCC_ARM
CXX_FLAGS += -DDEVICE_SPI=1
CXX_FLAGS += -DDEVICE_ANALOGIN=1
CXX_FLAGS += -DDEVICE_PWMOUT=1
CXX_FLAGS += -DTARGET_LIKE_CORTEX_M0

ASM_FLAGS += -x
ASM_FLAGS += assembler-with-cpp
ASM_FLAGS += -D__CMSIS_RTOS
ASM_FLAGS += -D__MBED_CMSIS_RTOS_CM
ASM_FLAGS += -D__CORTEX_M0PLUS
ASM_FLAGS += -DARM_MATH_CM0PLUS
ASM_FLAGS += -I./.
ASM_FLAGS += -I./PokittoLib
ASM_FLAGS += -I./PokittoLib/POKITTO_CORE
ASM_FLAGS += -I./PokittoLib/POKITTO_CORE/FONTS
ASM_FLAGS += -I./PokittoLib/POKITTO_CORE/PALETTES
ASM_FLAGS += -I./PokittoLib/POKITTO_HW
ASM_FLAGS += -I./PokittoLib/POKITTO_LIBS
ASM_FLAGS += -I./PokittoLib/POKITTO_LIBS/ImageFormat
ASM_FLAGS += -I./PokittoLib/POKITTO_LIBS/Synth
ASM_FLAGS += -I./PokittoLib/POKITTO_XTERNALS
ASM_FLAGS += -I./PokittoLib/POKITTO_XTERNALS/Arduino
ASM_FLAGS += -I./PokittoLib/libpff
ASM_FLAGS += -I./PokittoLib/mbed-pokitto
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/api
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/common
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/hal
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/cmsis
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X/TOOLCHAIN_GCC_ARM
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TARGET_NXP/TARGET_LPC11U6X/TOOLCHAIN_GCC_ARM/TARGET_LPC11U68
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/cmsis/TOOLCHAIN_GCC
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/hal
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP
ASM_FLAGS += -I./PokittoLib/mbed-pokitto/targets/hal/TARGET_NXP/TARGET_LPC11U6X


LD_FLAGS :=-Wl,--gc-sections -Wl,-n --specs=nano.specs -Wl,--wrap,_sbrk -mcpu=cortex-m0plus -mthumb -flto -Wl,--undefined=g_pfnVectors -Wl,--undefined=boot
LD_SYS_LIBS :=-Wl,--start-group -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys  -Wl,--end-group

# Tools and Flags
###############################################################################
# Rules

.PHONY: all lst size


all: $(BPROJECT).bin $(BPROJECT).hex size
	+@$(call MAKEDIR,$(OBJDIR))
	size $(BPROJECT).elf


BUILD/%.s.o: %.s
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Assemble: $(notdir $<)"
	@$(AS) -c $(ASM_FLAGS) -o $@ $<


BUILD/%.S.o: %.S
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Assemble: $(notdir $<)"
	@$(AS) -c $(ASM_FLAGS) -o $@ $<

BUILD/%.o : %.c
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Compile: $(notdir $<)"
	@$(CC) $(C_FLAGS) $(INCLUDE_PATHS) -o $@ $<

BUILD/%.o : %.cpp
	+@$(call MAKEDIR,$(dir $@))
	+@echo "Compile: $(notdir $<)"
	@$(CPP) $(CXX_FLAGS) $(INCLUDE_PATHS) -o $@ $<


$(BPROJECT).link_script.ld: $(LINKER_SCRIPT)
	@$(PREPROC) $< -o $@



$(BPROJECT).elf: $(OBJECTS) $(SYS_OBJECTS) $(BPROJECT).link_script.ld 
	+@echo "link: $(notdir $@)"
	@$(LD) $(LD_FLAGS) -T $(filter-out %.o, $^) $(LIBRARY_PATHS) --output $@ $(filter %.o, $^) $(LIBRARIES) $(LD_SYS_LIBS)


$(BPROJECT).bin: $(BPROJECT).elf
	$(ELF2BIN) -O binary $< $@
	+@echo "===== bin file ready to flash: $@ =====" 

$(BPROJECT).hex: $(BPROJECT).elf
	$(ELF2BIN) -O ihex $< $@

$(POP): $(BPROJECT).elf
	+@../pop.js
	+@echo "===== POP goes the $(PROJECT)! ====="


# Rules
###############################################################################
# Dependencies

DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)
# endif

# Dependencies
###############################################################################

clean :
	$(call RM,$(OBJDIR))
