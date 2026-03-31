################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS

GEN_OPTS__FLAG := @"./device.opt" 
GEN_CMDS__FLAG := -Wl,-l"./device_linker.cmd" 

ORDERED_OBJS += \
"./empty.o" \
"./ti_msp_dl_config.o" \
"./startup_mspm0g350x_ticlang.o" \
"./src/hab_app.o" \
"./src/hab_bme680.o" \
"./src/hab_gps.o" \
"./src/hab_lora_sx1276.o" \
"./src/hab_math.o" \
"./src/hab_protocol.o" \
"./src/hab_sd_fatfs.o" \
"./src/main.o" \
"./src/platform_stub.o" \
"./src/platform/diskio_mspm0.o" \
"./src/platform/platform_mspm0_ccs.o" \
"./third_party/bme68x/bme68x.o" \
"./third_party/fatfs/source/ff.o" \
"./third_party/fatfs/source/ffsystem.o" \
"./third_party/fatfs/source/ffunicode.o" \
$(GEN_CMDS__FLAG) \
-Wl,-ldevice.cmd.genlibs \
-Wl,-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include src/platform/subdir_vars.mk
-include third_party/bme68x/subdir_vars.mk
-include third_party/fatfs/source/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include src/platform/subdir_rules.mk
-include third_party/bme68x/subdir_rules.mk
-include third_party/fatfs/source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
hab-v2.out 

EXE_OUTPUTS__QUOTED += \
"hab-v2.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(LDS_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "hab-v2.out"

# Tool invocations
hab-v2.out: $(OBJS) $(CMD_SRCS) $(LDS_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -gdwarf-3 -Wall -Wl,-m"hab-v2.map" -Wl,-i"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source" -Wl,-i"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware" -Wl,-i"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/Debug/syscfg" -Wl,-i"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/lib" -Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections -Wl,--xml_link_info="hab-v2_linkInfo.xml" -Wl,--rom_model -o "hab-v2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "empty.o" "ti_msp_dl_config.o" "startup_mspm0g350x_ticlang.o" "src/hab_app.o" "src/hab_bme680.o" "src/hab_gps.o" "src/hab_lora_sx1276.o" "src/hab_math.o" "src/hab_protocol.o" "src/hab_sd_fatfs.o" "src/main.o" "src/platform_stub.o" "src/platform/diskio_mspm0.o" "src/platform/platform_mspm0_ccs.o" "third_party/bme68x/bme68x.o" "third_party/fatfs/source/ff.o" "third_party/fatfs/source/ffsystem.o" "third_party/fatfs/source/ffunicode.o" 
	-$(RM) "empty.d" "ti_msp_dl_config.d" "startup_mspm0g350x_ticlang.d" "src/hab_app.d" "src/hab_bme680.d" "src/hab_gps.d" "src/hab_lora_sx1276.d" "src/hab_math.d" "src/hab_protocol.d" "src/hab_sd_fatfs.d" "src/main.d" "src/platform_stub.d" "src/platform/diskio_mspm0.d" "src/platform/platform_mspm0_ccs.d" "third_party/bme68x/bme68x.d" "third_party/fatfs/source/ff.d" "third_party/fatfs/source/ffsystem.d" "third_party/fatfs/source/ffunicode.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

