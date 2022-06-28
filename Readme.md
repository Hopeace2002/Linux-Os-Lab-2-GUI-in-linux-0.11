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



### 0x02 实验操作

#### 1.项目的基本任务

运行Linux 0.11 的Bochs机器和运行Linux的宿主机之间能相互发送网络连接测试请求（ping请求）并能正确应答

+ 驱动网卡并实现两台机器之间的数据收发
+ 实现地址解析协议ARP
+ 实现因特网控制消息协议ICMP
+ 基于因特网控制消息协议实现ping命令



#### 2. 网卡驱动配置

修改Bochs配置文件bochsrc.bxrc（本实验环境名为0.11.bxrc）

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

本次环境中Bochs为2.6.8版本

执行

```shell
./configure -enable-ne2000=yes -with-x11
make
```



测试一下网卡


```bash
nudt@uvm:~$ sudo tcpdump -i eth0
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on eth0, link-type EN10MB (Ethernet), capture size 65535 bytes
02:03:29.431371 IP 192.168.1.11.netbios-ns > 192.168.1.255.netbios-ns: NBT UDP PACKET(137): REGISTRATION; REQUEST; BROADCAST
02:03:29.431536 IP 192.168.1.11.netbios-dgm > 192.168.1.255.netbios-dgm: NBT UDP PACKET(138)
02:03:29.431824 IP 192.168.1.11.netbios-dgm > 192.168.1.255.netbios-dgm: NBT UDP PACKET(138)
02:03:29.431934 IP 192.168.1.11.netbios-dgm > 192.168.1.255.netbios-dgm: NBT UDP PACKET(138)

```



NE2000 分为三个寄存器页

由PS0和PS1控制选择哪个寄存器页

应用程序发送一个数据包

操作系统将主存里的数据包复制到BUFFER RAM中，由NE2000发出“发送数据包命令” => 由网卡硬件从BUFFER RAM中逐个字节/字取出要发送的内容，放到FIFO控制器上 => FIFO将这些内容放到网线上

所以NE2000涉及两种类型的内存访问

+ 对网卡内部缓存的访问
+ 对系统主存的访问

通过本地DMA（BUFFER RAM）和远程DMA（MAIN RAM）来完成（直接存储器存储）



#### 读取网卡的MAC地址

MAC地址放在网卡片上的PROM

添加读取字函数inw（linux 0.11 中没有）

```c
static unsigned short inline inw( unsigned short port )
{
   unsigned short _v;
   
   __asm__ volatile ("inw %1,%0"
		     :"=a" (_v):"d" ((unsigned short) port));
   return _v;
}
```



调用

```c
	unsigned short prom[6];
	int j;
	for(j = 0; j < 6; j++)
	{
		prom[j] = inw(NE_IOBASE + NE_DATAPORT);
	}
```



按理说可以得到mac地址

可是不行



#### 网卡初始化

创建文件include/liux/netinit.h

定义网卡管理数据结构

```C
struct macaddr
{
    unsigned char bytes[6];
};
struct ne
{
    unsigned short iobase;
    unsigned short irq;
    unsigned short membase; // 缓存区首地址
    unsigned short memsize; // 缓存区的大小
    unsigned char rx_page_start; // 发送缓存区的开始页号
    unsigned char rx_page_stop; // 发送缓存区的结束页号
    unsigned char next_packet; // 下一个未读的接收到的数据包
    struct macaddr paddr;   // 网卡的MAC地址，将来收发数据包时要用到
};
```

