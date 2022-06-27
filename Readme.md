## 实验选题：网卡驱动与网络协议的设计与实现

参考文献:《操作系统原理、实现与实践》-李治军-哈工大



### 0x01 创建git仓库

这里记录一些常用的命令，或者说是基本命令

```shell
// 初始化创建一个git仓库
git init

// 把文件添加到仓库
git add x.file

// 提交到版本库
git commit -m "ZhuShi"

// 查看文件状态
git status

// 查看修改记录
git log --pretty=oneline

// 回退版本，上个版本是HEAD^,上上个是HEAD^^,100个之前是HEAD~100
// 也可以输入版本id号具体回退，注意倒退后再想回来就只能通过版本号了
git reset --hard HEAD^
git reset --hard 1e5d(id号)

git push origin master 推到github里
```

那么试着记录现在的状态到git里吧，然后我们就可以正式开始了

版本记录号采用0.0.0这样

修改某个文件具体代码变动第三位

修改一组文件实现某个具体功能变动第二位

修改实现新的一些功能变动第一位

开始吧！



### 0x02 阅读书籍

#### 1.项目的基本任务

运行Linux 0.11 的Bochs机器和运行Linux的宿主机之间能相互发送网络连接测试请求（ping请求）并能正确应答

+ 驱动网卡并实现两台机器之间的数据收发
+ 实现地址解析协议ARP
+ 实现因特网控制消息协议ICMP
+ 基于因特网控制消息协议实现ping命令



#### 2. 网卡驱动

修改Bochs配置文件bochsrc.bxrc

pci:enabled=1,chipset=i440fx,slot2=ne2k
ne2k:ioaddr=0x300,irq=10,mac=b0:c4:20:00:00:01,ethmod=linux,ethdev=eth0



pci:enabled=1,chipset=i440fx 用来允许PCI总线的（驱动PCI工作的芯片组为i440fx）。

slot2=ne2k 表示将网卡ne2k插到PCI插槽2上

设置ne2k：

ioaddr设置了NE2000芯片芯片寄存器组的初始地址

irq设置了NE2000中断请求对应的中断号

mac设置网卡的物理地址，每个网卡都有唯一的48位物理地址

后面的ethmod和ethdev用来设置虚拟网线

当网卡驱动检测到ping的目标IP地址就是本机IP时，会直接在驱动程序中用软件完成对ping命令的处理，数据包无法发往网卡接口eth0

这里采用TAP虚拟网卡去解决这个问题

改为

ne2k:ioaddr=0x300,irq=10,mac=b0:c4:20:00:00:01,ethmod=tuntap,ethdev=/dev/net/tun,script=/path/tunconfig

其中/path/tunconfig为一个可执行脚本

```bash
#!/bin/bash
/sbin/ifconfig ${1##/*/}192.168.1.11
```

即将tap0网络接口IP设置为192.168.1.11

本次环境中Bochs为2.6.9版本

执行

```shell
./configure -enable-ne2000=yes -with-x11
make
```

