├─CORE
├─HALLIB
│  └─STM32F1xx_HAL_Driver
│      ├─Inc
│      │  └─Legacy
│      └─Src
│          └─Legacy
├─HARDWARE
│  ├─ENCODER
│  ├─KEY
│  ├─LED
│  ├─PULSE						<!--框架电机控制程序|主要思路是定时器产生脉冲信号控制步进电机-->
│  ├─PWM						<!--转子电机控制程序|主要思路是定时器产生PWM信号控制无刷电机，带有增量式PID反馈调节功能-->
│  ├─SPI
│  ├─TIM
│  ├─TMC260						<!--框架电机驱动程序-->
│  └─W25QXX
├─OBJ
├─SYSTEM
│  ├─delay
│  ├─sys
│  └─usart						
├─USER
│  └─DebugConfig
└─USMART
