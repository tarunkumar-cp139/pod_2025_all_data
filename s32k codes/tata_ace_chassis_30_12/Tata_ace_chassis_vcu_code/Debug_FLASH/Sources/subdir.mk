################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/can0.c \
../Sources/can1.c \
../Sources/can2.c \
../Sources/lptimer.c \
../Sources/main.c \
../Sources/pid.c \
../Sources/pin_config.c \
../Sources/spi0.c \
../Sources/syst_clk.c \
../Sources/system_states.c \
../Sources/uart0.c 

OBJS += \
./Sources/can0.o \
./Sources/can1.o \
./Sources/can2.o \
./Sources/lptimer.o \
./Sources/main.o \
./Sources/pid.o \
./Sources/pin_config.o \
./Sources/spi0.o \
./Sources/syst_clk.o \
./Sources/system_states.o \
./Sources/uart0.o 

C_DEPS += \
./Sources/can0.d \
./Sources/can1.d \
./Sources/can2.d \
./Sources/lptimer.d \
./Sources/main.d \
./Sources/pid.d \
./Sources/pin_config.d \
./Sources/spi0.d \
./Sources/syst_clk.d \
./Sources/system_states.d \
./Sources/uart0.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/can0.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


