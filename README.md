# 基于物联网MQTT协议的报文过滤控制系统

## 项目说明
    该项目是本人的毕业设计课题项目
    项目地址：https://github.com/aimuller/mqtt_filter

## 课题要求

    1. 基于linux，开发netfilter模块，可以设置MQTT过滤规则

    2. 对MQTT报文进行协议解析和过滤控制

    3. 根据策略记录日志

## 实验环境
    seedUbuntu虚拟机，版本16.04，32bit

    虚拟机资源可以从：https://seedsecuritylabs.org/labsetup.html 获取
    
### 测试主机环境
    主机	    版本                        地址	            作用
    主机A	    SEEDUbuntu-16.04-32bit      192.168.222.128	    MQTT客户端
    主机B	    SEEDUbuntu-16.04-32bit      192.168.222.130	    MQTT客户端
    主机C	    SEEDUbuntu-16.04-32bit      192.168.222.131     MQTT服务器
    
### 测试MQTT环境
    身份	     安装软件    版本	            安装主机
    客户端     MQTT.fx     version 1.7.1    主机A
    客户端     MQTT.fx     version 1.7.1    主机B
    服务器     mosquito    version 1.4.8    主机C



## 重要文件说明
    |-- mqtt_filter
            |-- data文件夹（存放系统运行时产生的数据文件）
            |-- log.txt（日志文件）
            |-- ...
        |-- image文件夹（存放系统运行照片）
            |-- ...
        |-- kernel文件夹（存放系统内核模块代码）
            |-- mqtt_filter_mod.c（开发netfilter模块的内核源码）
            |-- Makefile（将mqtt_filter_mod.c编译成mf.ko，以模块方式安装到内核）
            |-- ...
        |-- kpcre文件夹（存放pcre正则库及其依赖库的内核模块源码，以模块的方式将pcre正则库安装到内核中）
            |-- libc文件夹（存放由用户态改写成的内核模块形式的libc函数库源码，属于pcre函数库的依赖项）
                |-- ...
            |-- pcre2文件夹（存放由用户态改写成的内核模块形式的pcre正则库源码）
                |-- ...
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
                |-- helper.cpp（定义一些辅助函数）
                |-- mainwindow.ui（设计主界面的ui文件）
                |-- commonruledialog.ui（设计规则对话框界面的ui文件）
                |-- ...
            |-- build-MQTT_Filter-Desktop_Qt_5_1_0_GCC_32bit-Debug文件夹（存放可执行程序）
                |-- MQTT_Filter（生成的可执行程序，即用户配置程序）
                |-- ...
            |-- ...
 
## 系统运行与卸载
### 运行
    1. 内核模块安装：进入sh目录下运行mf_on.sh脚本
    2. 用户程序使用：使用QT打开user文件夹下的MQTT_Filter.pro项目文件，然后编译运行可执行程序

### 卸载
    1. 确保已经退出用户配置程序
    2. 进入sh目录下运行mf_off.sh脚本卸载内核模块
    
## 运行截图
### 主界面
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E4%B8%BB%E7%95%8C%E9%9D%A2.png)
### 规则配置界面
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E8%A7%84%E5%88%99%E9%85%8D%E7%BD%AE%E7%95%8C%E9%9D%A2.png)
### 规则查看
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E8%A7%84%E5%88%99%E6%9F%A5%E7%9C%8B.png)
### 插入规则
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E6%8F%92%E5%85%A5%E8%A7%84%E5%88%99.png)
### 修改规则
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E4%BF%AE%E6%94%B9%E8%A7%84%E5%88%99.png)
### 删除规则
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E5%88%A0%E9%99%A4%E8%A7%84%E5%88%99.png)
### 清除规则
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E6%B8%85%E9%99%A4%E8%A7%84%E5%88%99.png)
### 导入导出规则文件
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E5%AF%BC%E5%85%A5%E5%AF%BC%E5%87%BA%E8%A7%84%E5%88%99%E6%96%87%E4%BB%B6.png)
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E5%AF%BC%E5%85%A5%E8%A7%84%E5%88%99%E6%96%87%E4%BB%B6.png)
### 会话查看
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E4%BC%9A%E8%AF%9D%E6%9F%A5%E7%9C%8B.png)
### 日志审查
![Image text](https://raw.githubusercontent.com/aimuller/mqtt_filter/master/image/%E6%97%A5%E5%BF%97%E5%AE%A1%E6%9F%A5.png)

