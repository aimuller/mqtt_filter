# 基于物联网MQTT协议的报文过滤控制系统
## 课题要求：

    1. 基于linux，开发netfilter模块，可以设置MQTT过滤规则

    2. 对MQTT报文进行协议解析和过滤控制

    3. 根据策略记录日志

## 实验环境
    seedUbuntu虚拟机，版本16.04，32bit

    虚拟机资源可以从：https://seedsecuritylabs.org/labsetup.html 获取

## 重要文件说明
    |-- data文件夹（存放系统运行时产生的数据文件）
    	|-- log.txt（日志文件）
        |-- ...
    |-- kernel文件夹（存放系统内核模块代码）
        |-- mqtt_filter_mod.c（开发netfilter模块的内核源码）
        |-- Makefile（将mqtt_filter_mod.c编译成mf.ko，以模块方式安装到内核）
        |-- ...
    |-- kpcre文件夹（存放pcre正则库源码，并以模块的方式安装到内核中，过滤时可以调用）
    	|-- ...
    |-- rule文件夹（存放用户规则文件）
        |-- ...
    |-- sh文件夹（存放系统安装和运行时所需要的shell脚本）
        |-- log.sh（该脚本功能是调用dmesg指令，并将输出重定向到data文件夹下的log.txt文件下）
        |-- mf_on.sh（安装内核模块和相关依赖项）
        |-- mf_off.sh（卸载内核模块和清除相关依赖项）
        |-- ...
    |-- user文件夹（存放用户配置程序的源码和可执行程序）
        |-- MQTT_Filter（存放用户配置程序的源码）
            |-- MQTT_Filter.pro（QT配置文件）
            |-- main.cpp（主函数，程序入口）
            |-- mainwindow.cpp（主界面类定义）
            |-- commonruledialog.cpp（对话窗口类定义）
            |-- helper.cpp（辅助函数）
            |-- mainwindow.ui（主界面设计ui文件）
            |-- commonruledialog.ui（对话窗口设计ui文件）
            |-- ...
        |-- build-MQTT_Filter-Desktop_Qt_5_1_0_GCC_32bit-Debug（存放可执行程序）
    	|-- ...
 
