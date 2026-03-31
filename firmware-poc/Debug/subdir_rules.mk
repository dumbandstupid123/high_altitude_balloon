################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-146172893: ../launchpad_poc.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/Users/zanehensley/ti/sysconfig_1.26.2/sysconfig_cli.sh" -s "/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/.metadata/product.json" --script "/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/launchpad_poc.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-146172893 ../launchpad_poc.syscfg
device.opt: build-146172893
device.cmd.genlibs: build-146172893
ti_msp_dl_config.c: build-146172893
ti_msp_dl_config.h: build-146172893
Event.dot: build-146172893

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/src" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/bme68x" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/fatfs/source" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/Debug" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: /Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/src" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/bme68x" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/fatfs/source" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/Debug" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/src" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/bme68x" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/fatfs/source" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware-poc/Debug" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


