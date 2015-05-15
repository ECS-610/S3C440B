嵌入式实验代码样例
===============
##相关资料：
- [实验报告模板+实验手册](http://pan.baidu.com/s/1hqgrkeK)  
- [PPT课件](http://pan.baidu.com/s/1gdgtoAn)   

![谁的钥匙忘在实验室了](http://img5.douban.com/view/photo/thumb/public/p2242709728.jpg)    
  谁的钥匙忘在实验室了   
  请去实验室认领  

####  05/15
```
第三次实验 矩阵键盘(key.c)  
评分标准：     
1分：参加实验     
3分：按下键盘 显示对应符号     
5分：依次按下键盘1~8 显示各自学号     
6分：自由发挥 实现更加高级的功能      
```


-----------------------------------------------------------------------------------
```
第二次实验 数码管显示(leddemo.c)  
评分标准：  
1分：参加实验  
3分：在数码管上显示后四位学号 
5分：滚动显示全部学号   
6分：自由发挥 实现更加高级的功能    
```
###注意：
##### 1.当运行代码提示更新J-link驱动时 务必选【否】   
##### 2.win7及以上系统需用管理员权限运行安装和注册程序

04/24 注意： 
 - 由于部分操作符被修改成'?'  编译出错时 双击出错项将跳转到出错行 将'?'修改正确或注释代码 使代码编译通过   
 - 通过调试代码最终实现来回依次闪烁的流水灯效果    

1.IAR下载:
http://files.iar.com/ftp/pub/box/CD-EWARM-6301-3142.zip

2.IAR注册码生成:
Keygen6.40.exe

3.右侧 [Download Zip] 下载包含源码的压缩包 




##运行IAR
- 1.打开一个工程
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











