################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gluedthread/glthread.c 

OBJS += \
./gluedthread/glthread.o 

C_DEPS += \
./gluedthread/glthread.d 


# Each subdirectory must supply rules for building sources it contributes
gluedthread/%.o: ../gluedthread/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


