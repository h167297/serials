# serials

**The serials is a very useful serial port debug tool.**

[一、程序介绍](#1) <br />
[二、目录结构](#2) <br />
[三、编译方法](#3) <br />
[四、安装体验](#4) <br />
[五、修改记录](#5) <br />


<h2 id='1'> 一、程序介绍 </h2>

1、该程序是一个串口调试工具，支持丰富的串口操作。包含linux和windows两个版本，体验一致。linux版使用dbus实现程序的免密特权操作。

2、linux版本的理念、功能介绍、开发环境、安装方式、系统支持情况等参见：http://blog.csdn.net/he_ning/article/details/48272395

3、windows版本开发环境为windows7,Qt5.5.0。

4、linux和windows版本共用源码，部分操作系统差异实现使用Q_OS_WIN和Q_OS_LINUX宏隔离。

<h2 id='2'> 二、目录结构 </h2>

1、serials/ 目录下为serial.pro主工程内容(相当于visual studio的解决方案)，使用Qt Creator 打开可以看到目录树，
   包含serials.pro和serialsdbus.pro两个子工程。
   
2、serials/serials/ 目录下为serials.pro子工程内容，包含实现该软件前台功能的资源，UI文件，源码；windows版本仅涉及该部分代码。

3、serials/serialsdbus/ 目录下为serialsdbus.pro子工程内容，linux专用，包含dbus守护进程源码，进程以root权限自动运行，用于特权操作。
   dbus介绍：http://blog.csdn.net/he_ning/article/details/42614937
   
4、serials/debian/ 目录下包含deb包的配置信息和规定的目录结构。deb包介绍：http://blog.csdn.net/he_ning/article/details/46341443

<h2 id='3'> 三、编译方法 </h2>

1、推荐使用Qt Creator打开工程直接编译，linux和windows下都可以装Qt Creator。

2、如果不用IDE（不推荐，本就是个图形界面软件），可以使用qmake serial.pro 根据Qt工程生成makefile，再make即可。

<h2 id='4'> 四、安装体验 </h2>

1、在线安装：支持ubuntu14.04，ubuntu15.04，ubuntu16.04,ubuntu16.10，ubuntu17.04, ubuntu17.10 <br />
   sudo add-apt-repository ppa:h167297/hening<br />
   sudo apt-get update<br /> 
   sudo apt-get install serials<br />
   
2、离线安装：支持ubuntu17.04 http://download.csdn.net/download/h167297/9838997

3、装好后会在桌面生成程序图标，双击打开即可，也可以在终端输入serials启动（不推荐，该程序的宗旨是完全摆脱命令）。

<h2 id='5'> 五、修改记录 </h2>

V1.0.4 更新内容：
1、增加了串口状态指示灯
2、增加了十六进制发送时自动输入空格的功能
3、帮助信息中增加更新日志
4、增加了屏幕适配
5、增加了窗口顶层显示
6、增加了自动打开串口
7、增加了自动保存串口设置和窗口状态

V1.0.3更新内容：
1、美化了UI
2、修复了连续快速接收时UI卡死的bug
3、增加了保存内容时生成默认文件名的功能
4、修复了十六进制发送时必须输入空格的bug

V1.0.2更新内容：
1、增加了MODBUS协议使用的CRC校验
2、增加了鼠标中键清空发送区功能
3、增加了Ctrl+Enter组合键发送功能
4、默认使能十六进制发送和接收

V1.0.1更新内容：
1、修复了因其它串口的断开，导致当前串口不可用的bug
2、修复了串口打开状态下，新串口接入时导致锁定的串口名变化的bug
3、增加了显示缓冲区清空的功能

V1.0.0更新内容：
1、ASCII码、十六进制和中文收发
2、自动扫描可用串口
3、文件发送和接收
4、缓冲区自动清空
5、在线参数更改
6、内容存储
7、定时发送
8、互动显示
