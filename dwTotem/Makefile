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

PROJECT := dwHelloWorld

BPROJECT := BUILD/dwHelloWorld
POP := ../$(PROJECT).pop

# Project settings
###############################################################################
# Objects and Paths

OBJECTS += BUILD/main.o
OBJECTS += BUILD/startup.o

INCLUDE_PATHS += -I./.
INCLUDE_PATHS += -I../includes/
LIBRARY_PATHS :=
LIBRARIES :=
LINKER_SCRIPT := boot.ld

# Objects and Paths
###############################################################################
# Tools and Flags

CDB := cdb

AS      = $(CDB)	 'arm-none-eabi-gcc' '-x' 'assembler-with-cpp' '-c' '-Wall' '-Wextra' '-Wno-unused-parameter' '-Wno-missing-field-initializers' '-fmessage-length=0' '-fno-exceptions' '-fno-builtin' '-ffunction-sections' '-fdata-sections' '-funsigned-char' '-MMD' '-fno-delete-null-pointer-checks' '-fomit-frame-pointer' '-Os' '-g1' '-DMBED_RTOS_SINGLE_THREAD' '-mcpu=cortex-m0plus' '-mthumb'
CC      = $(CDB)	 'arm-none-eabi-gcc' '-std=gnu89' '-c' '-Wall' '-Wextra' '-Wno-unused-parameter' '-Wno-missing-field-initializers' '-fmessage-length=0' '-fno-exceptions' '-fno-builtin' '-ffunction-sections' '-fdata-sections' '-funsigned-char' '-MMD' '-fno-delete-null-pointer-checks' '-fomit-frame-pointer' '-Os' '-g1' '-DMBED_RTOS_SINGLE_THREAD' '-mcpu=cortex-m0plus' '-mthumb'
CPP     = $(CDB)	 'arm-none-eabi-g++' '-std=gnu++14' '-fno-rtti' '-Wvla' '-c' '-Wall' '-Wextra' '-Wno-unused-parameter' '-Wno-missing-field-initializers' '-fmessage-length=0' '-fno-exceptions' '-fno-builtin' '-ffunction-sections' '-fdata-sections' '-funsigned-char' '-MMD' '-fno-delete-null-pointer-checks' '-fomit-frame-pointer' '-Os' '-g1' '-DMBED_RTOS_SINGLE_THREAD' '-mcpu=cortex-m0plus' '-mthumb'
LD      = $(CDB)	 'arm-none-eabi-gcc' '-Os' '-g'
ELF2BIN = 'arm-none-eabi-objcopy'
PREPROC = 'arm-none-eabi-cpp' '-E' '-P' '-Wl,--gc-sections' '-Wl,--wrap,_malloc_r' '-Wl,--wrap,_free_r' '-Wl,--wrap,_realloc_r' '-Wl,--wrap,_memalign_r' '-Wl,--wrap,_calloc_r' '-Wl,--wrap,exit' '-Wl,--wrap,atexit' '-Wl,-n' '--specs=nano.specs' '-mcpu=cortex-m0plus' '-mthumb'


C_FLAGS += -flto
CXX_FLAGS += -flto

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


CXX_FLAGS += -std=gnu++17
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

all: $(BPROJECT).bin $(POP) size
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

$(POP): $(BPROJECT).elf pop.json
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
