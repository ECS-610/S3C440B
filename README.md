嵌入式实验代码样例
===============

1.IAR下载:
http://files.iar.com/ftp/pub/box/CD-EWARM-6301-3142.zip

2.IAR注册码生成:
Keygen6.40.exe

3.右侧 [Download Zip] 下载包含源码的压缩包 

4.源码中需要debug处已用  " //<------------- " 格式注释  


##运行IAR
- 1. 打开一个工程
在菜单栏中 File->open->Workspace     
![open](http://img5.douban.com/view/photo/photo/public/p2240044626.jpg)  
选中工程文件    
![slect](http://img5.douban.com/view/photo/photo/public/p2240044628.jpg)  

- 2.在左侧菜单里选中项目 右键展开菜单 点选option项   
![option](http://img3.douban.com/view/photo/photo/public/p2240044630.jpg)       

- 3.在option左侧栏中选**Linker**项 勾选**Override default**项  并将默认.icf文件替换为本项目.icf文件     
![icf](http://img3.douban.com/view/photo/photo/public/p2240044631.jpg)     
![change](http://img3.douban.com/view/photo/photo/public/p2240044634.jpg)     

- 4.在option左侧栏中选**Debugger**项 确保Setup项中的**Driver**值为**J-Linker/J-Trace**      
![driver](http://img3.douban.com/view/photo/photo/public/p2240044635.jpg)   

- 5.根据需要 去掉项目中的configure.h中本次实验内容的注释   
![define](http://img3.douban.com/view/photo/photo/public/p2240044632.jpg)   

- 6.用J-link将开发板和电脑连接      

- 7.依次点击右上侧菜单中的**Make**和**Download and Debugge**编译和刷写程序到开发板   

![m](http://img3.douban.com/view/photo/photo/public/p2239702970.jpg) 

![d](http://img3.douban.com/view/photo/photo/public/p2239702974.jpg)   

- 8.点击左上侧的控制按钮运行代码 分别有进入函数 单步运行 顺序运行等    
![ctl](http://img5.douban.com/view/photo/photo/public/p2239702966.jpg)  


- 9.注意：当提示更新J-link驱动时 务必选【否】 






