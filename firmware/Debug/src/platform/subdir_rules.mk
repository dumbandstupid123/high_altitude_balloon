################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/platform/%.o: ../src/platform/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/include" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/include/platform" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/src" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/src/platform" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/bme68x" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/third_party/fatfs/source" -I"/Users/zanehensley/code/classes/classelec327/high_altitude_balloon/firmware/Debug" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"/Users/zanehensley/ti/mspm0_sdk_2_10_00_04/source" -gdwarf-3 -Wall -MMD -MP -MF"src/platform/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


