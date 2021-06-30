```
├─README.md
├─QT串口程序
│  └─SerialPort                             <!--开发环境：QT-->
│      ├─build-SerialPort-Desktop_Qt_5_13_0_MinGW_64_bit-Debug
│      │  ├─debug
│      │  └─release
│      ├─build-SerialPort-Desktop_Qt_5_13_0_MinGW_64_bit-Release
│      │  ├─debug
│      │  └─release
│      └─SerialPort                         <!--程序源码-->
|   <!--后续开发方向：数据的图形化显示-->
└─SGCMG
    ├─matlab                                <!--CVP仿真用的代码-->  
    ├─原理图和PCB                           <!--原理图继承于SADA-->  
    │  └─SADA_motor
    │      ├─datasheet
    │      ├─SADA_controller_PCB        
    │      └─SADA_controller_SCH
    ├─学习资料                              <!--嵌入式程序设计的参考资料-->  
    ├─嵌入式程序                            <!--开发环境：keil-->
    │  └─GimbalMotor                        <!--stm32f1xx HAL程序-->  
    │      ├─CORE
    │      ├─HALLIB
    │      │  └─STM32F1xx_HAL_Driver
    │      ├─HARDWARE
    │      │  ├─ENCODER
    │      │  ├─KEY
    │      │  ├─LED
    │      |  ├─PULSE						<!--框架电机控制程序|主要思路是定时器产生脉冲信号控制步进电机-->
    │      |  ├─PWM						    <!--转子电机控制程序|主要思路是定时器产生PWM信号控制无刷电机，带有增量式PID反馈调节功能-->
    │      |  ├─SPI
    │      |  ├─TIM
    │      |  ├─TMC260						<!--框架电机驱动程序-->
    │      │  └─W25QXX
    │      ├─OBJ
    │      ├─SYSTEM
    │      │  ├─delay
    │      │  ├─sys
    │      │  └─usart
    │      ├─USER
    │      │  └─DebugConfig
    │      └─USMART
    ├─机械结构                              <!--改版联系负责机械的同事-->  
    │  └─新电机手册和结构
    └─辅助程序                              <!--框架电机加减速辅助计算程序--> 
``` 
# SGCMG后续开发方向
1. 框架电机需要更换型号，实际的减速比1526过大，目前是按照14计算的（甚至不用步进电机）
2. 框架电机的角度和角速度需要考虑测量方法
3. 机械结构需要小型化
4. 如果准备使用FPGA开发，参考学习资料中的程序思路
5. 构型的话还需要考虑怎么测角动量或者力矩