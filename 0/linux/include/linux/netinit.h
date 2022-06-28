#include <asm/io.h>

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

struct ne ne2k;
#define NE_PAGE_SIZE 256    // NE2000 内存缓存的每一页的大小是256B
#define NE_TXBUF_SIZE 4     // 发送缓存尺寸（以页page为单位）
#define NE_TX_BUFERS 2      // 发送缓存数量
#define NE_IOBASE 0x10
#define NE_DATAPORT 0xc02
#define NE_IRQ  92  // 暂时定义中断号为92

int ne2k_init()
{
    ne2k.iobase = NE_IOBASE;
    ne2k.irq = NE_IRQ;
    ne2k.membase = 16 * 1024; // BUFFER RAM(包括接受缓存和发送缓存)的开始位置
    ne2k.memsize = 16 * 1024; // BUFFER RAM的大小
    ne2k.rx_page_start = ne2k.membase/NE_PAGE_SIZE; // 接收缓存开始的位置
    ne2k.rx_page_stop = ne2k.rx_page_start + ne2k.memsize/NE_PAGE_SIZE - NE_TXBUF_SIZE * NE_TX_BUFERS; //接收缓存结束的位置
    ne2k.next_packet = ne2k.rx_page_start + 1;
    ne2k.paddr.bytes[0]=0xb0;
    ne2k.paddr.bytes[1]=0xc4;
    ne2k.paddr.bytes[2]=0x20;
    ne2k.paddr.bytes[3]=0x00;
    ne2k.paddr.bytes[4]=0x00;
    ne2k.paddr.bytes[5]=0x01;
    return 1;
}

// 分割好BUFFER RAM，接下来设置寄存器页上的各种寄存器
#define NE_PO_CR 0x00 // COMMAND 寄存器端口地址偏移
#define NE_CR_STP 0X01 // 关闭网卡，不允许收发数据包
#define NE_CR_STA 0X02 // 开启网卡，允许收发数据包
#define NE_CR_RD2 0X20 // 终止远程DMA
#define NE_PO_PSTART 0X01 // PSTART 寄存器端口地址偏移
#define NE_PO_PSTOP 0X02 // PSTOP 寄存器端口地址偏移
#define NE_PO_BNRY 0X03  // BNRY 寄存器端口地址偏移
#define NE_CR_PAGE1 0X40 // 将PSO位设置为1
#define NE_P1_CURR 0X07 // CURR 寄存器端口地址偏移




#define NE_P1_PARO 0X01
#define NE_P1_MARO 0X08
#define NE_PO_RCR 0X0C
#define NE_RCR_AB 0X04

#define NE_PO_DCR 0x0E
#define NE_RCR_WTS 0x01
#define NE_DCR_BOS 0X02

#define NE_PO_RBCR0 0X0A
#define NE_PO_RBCR1 0X0B


int net_outb()
{
outb(NE_CR_RD2|NE_CR_STP, ne2k.iobase+NE_PO_CR);
outb(ne2k.rx_page_start, ne2k.iobase+NE_PO_PSTART);
outb(ne2k.rx_page_stop, ne2k.iobase + NE_PO_PSTOP);
outb(ne2k.rx_page_start, ne2k.iobase+NE_PO_BNRY);
outb(NE_CR_PAGE1|NE_CR_RD2|NE_CR_STP , ne2k.iobase+NE_PO_CR);
outb(ne2k.next_packet,ne2k.iobase+NE_P1_CURR);
int i;
for (i=0;i<6;i++)
{
    outb(ne2k.paddr.bytes[i], ne2k.iobase + NE_P1_PARO + i);
}
for (i = 0; i < 8; i++)
{
    outb(0,ne2k.iobase + NE_P1_MARO + i);
}
outb(NE_CR_RD2|NE_CR_STP, ne2k.iobase+NE_PO_CR);
outb(NE_RCR_AB, ne2k.iobase + NE_PO_RCR);
outb(NE_RCR_WTS, ne2k.iobase + NE_PO_DCR);

outb(0, ne2k.iobase + NE_PO_RBCR0);
outb(0, ne2k.iobase + NE_PO_RBCR1);
outb(NE_CR_RD2|NE_CR_STA, ne2k.iobase + NE_PO_CR);
}
