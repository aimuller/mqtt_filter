# 基于物联网MQTT协议的报文过滤控制系统
1. 基于linux，开发netfilter模块，可以设置MQTT过滤规则

2. 对MQTT报文进行协议解析和过滤控制

3. 根据策略记录日志

# 实验环境
seedUbuntu虚拟机，版本16.04，32bit

虚拟机资源可以从：https://seedsecuritylabs.org/labsetup.html 获取

# 文件说明
内核开发模块： 基于Linux Netfilter框架进行内核模块开发，是本系统的核心模块。

              源码文件位于/kernel/mqtt_filter_mod.c
              
              相应的头文件位于/kernel/header.h 
              
              
用户配置程序： 使用QT开发的图形界面用户配置程序，可以对规则进行增删改。

              相关的工程源码位于/user/MQTT_Filter目录下
