################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../comm.c \
../graph.c \
../main.c \
../nwcli.c \
../topologies.c 

OBJS += \
./comm.o \
./graph.o \
./main.o \
./nwcli.o \
./topologies.o 

C_DEPS += \
./comm.d \
./graph.d \
./main.d \
./nwcli.d \
./topologies.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


