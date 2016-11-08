RC522读卡器 M1卡学习总结（四）
四、单片机stm8s与读卡器RC522通讯----软件部分
      单片机与读卡器的接口
      RC522读卡器提供三种接口，分别是UART,I2C,SPI这三种接口。我们这里选择了spi接口。关于stm8s为主机，rc522读卡器为从机。spi使用4个管脚与从设备相连，MISO,MOSI,SCK,NSS.
       这里有一点需要注意：MOSI管脚相互连接，MISO脚相互连接。这样，数据在主和从之间串行地传输（MSB位在前）。通信总是由主设备发起。主设备通过MOSI脚把数据发送给从设备，从设备通过MISO引脚回传数据。这意味全双工通信的数据输出和数据输入是用同一个时钟信号同步的：时钟信号由主设备通过SCK脚提供。
（1）SPI初始化如下：
void init_SPI_Master(void)
{        
        SPI_CR1 = 0x04;          //fMaster/2, 配置为主设备，空闲低电平，第一个时钟周期开始采样
        SPI_CR2 = 0x03;           //内部指定主模式，NSS上电平决定该位值
        SPI_CRCPR = 0x07;
        SPI_ICR = 0x00;            //中断禁止
        SPI_CR1 |= 0x40;          //开启SPI

}
（2）UART初始化，这里为了调试方便，我们使用uart接口来观察执行情况。
void init_UART(void)
{
       UART1_CR2=0x00;//使发送禁用TEN=0；
       UART1_CR1=0x00;//设置M字长，8位数据位
       UART1_CR3=0x00;//1位停止位
 
       UART1_BRR2=0X00;      //波特率9600，分频系数=2000000/9600=208
       UART1_BRR1=0X0D;       //208=0X0D;
       UART1_CR2=0X2C;       //b3=1,允许发送  b2=1,允许接收  b5=1,允许产生接收中断
}
 （3）UART发送数据函数
void UARTSend(uchar aa)
{
  UART1_CR2=0x00;
  UART1_DR=aa;                   
  UART1_CR2=0x08;
  while((UART1_SR & 0x40) ==0);
  UART1_CR2=0x2c;//0x2c;//0x08; 2位 rev使能 3位 发送使能 5--rien 
}
（4）UART接收中断处理函数
@far @interrupt void UART1_RX_IRQHandler(void)  //UART1_Receiver(void)
{
     unsigned char i;
     _asm("sim");    //关总中断
      a=UART1_DR;      //将接收的字符放到a中
     _asm("rim");    //开总中断
    return;
}
（5）SPI接收发送字节
//spi接口在写的时候要先发送，发送不一定要传送内容，只是为了
//给从机提供时钟，因为sck是主机来发出的，所以写函数里面要有发送指令，目的是提供从机写时钟
uchar SPIWriteByte(uchar spiData)
{
   uchar retry=0;
  while((SPI_SR & 0X02)==0)  //等待发送区空
  {
   retry++;
   if(retry>200) return 0;  //防止死机
  }
 
  SPI_DR=spiData;             //发送一个byte
  retry=0;
  while((SPI_SR|0X80)==0X80);  //等待通信结束
 
  while((SPI_SR & 0X01)==0);  //等待接收缓冲区空
  {
     retry++;
    if(retry>200) return 0;
  }
  return SPI_DR;    //返回受到的数
 
}
（6）main.c主程序函数
void main()
{
 uchar i;
 uchar Card_Type1[2];
 uchar Card_ID[4];
 uchar Card_KEY[6] = {0xff,0xff,0xff,0xff,0xff,0xff};  
 uchar Card_Data[16];
 uchar status;
  //fmaster=fcpu=2MHz
 CLK_ECKR=0x00;
 CLK_ICKR=0x01;
 CLK_CMSR=0xe1;
 
 CLK_SWR=0xe1;
 CLK_CKDIVR=0x18;
 init_IO();
 init_UART();
 init_SPI_Master();
 
 _asm("rim");    //开总中断,便于产生串口接收中断
 status=PcdReset();
 PcdAntennaOff();
 delay(20);
 PcdAntennaOn();
 M500PcdConfigISOType( 'A' );
 a=0x63;
 nblock=4;//kuan 0
 t=0;
 while(1)
 {
  
        //    PcdAntennaOff();
        //测试一下接口是否正确
        status = CheckMCURC522interface();
        if(status != MI_OK)
        {
             UARTSend(0x04);
             continue;
        }
    
         status = PcdRequest(PICC_REQALL, g_ucTempbuf);
           if (status != MI_OK)
          {    
              continue;
           }
         //  UARTSend(a);//0x11);
         //  for(i=0;i<8;i++)
         //  UARTSend(g_ucTempbuf[i]);//0400 e52b 0000000000
       
         delay(20);//
         status = PcdAnticoll(g_ucTempbuf);
         if (status != MI_OK)
         {    continue;    }
         //   UARTSend(0x12);
         delay(20);//delay_ms(2);
         status = PcdSelect(g_ucTempbuf);
         if (status != MI_OK)
         {    continue;    }
        //   UARTSend(0x13);
         delay(20);//
       //   UARTSend(nblock);
       //认证卡
         status = PcdAuthState(PICC_AUTHENT1A, nblock, DefaultKey, g_ucTempbuf);
         if (status != MI_OK)
         {    continue;    }
        //   UARTSend(0x14);
        delay(20);//
       if(a==b) continue;
       else b=a;
       UARTSend(a);  //发送执行的命令代码
     switch(a)
     {
     case 0x91://写电子钱包数据到块nblock
       status = PcdWrite(nblock, data1);
         if(status==MI_OK)
       {
        UARTSend(0x88);
        
       }
       else UARTSend(0x44);
       break;
     case 0x92://写普通数据到块nblock
       status = PcdWrite(nblock, data3);
         if(status==MI_OK)
       {
        UARTSend(0x88);
        
       }
       else UARTSend(0x44);
         break;
     case 0x93://从块n中读取数据
       status = PcdRead(nblock, g_ucTempbuf);//PcdRead(2, g_ucTempbuf);
        if (status == MI_OK)
        {   
        UARTSend(0x88);
        for(i=0;i<16;i++)
         UARTSend(g_ucTempbuf[i]);//把写进去的数
        }
        else  UARTSend(0x44);
        break;
     case 0x94://从块n中读取数据
       status = PcdRead(nblock, g_ucTempbuf);//PcdRead(2, g_ucTempbuf);
        if (status == MI_OK)
        {   
        UARTSend(0x88);
        for(i=0;i<16;i++)
         UARTSend(g_ucTempbuf[i]);//把写进去的数
        }
        else  UARTSend(0x44);
        break;
     case 0x95://操作电子钱包
       status = PcdValue(PICC_DECREMENT,nblock,data2);
       if (status == MI_OK)
       {  
         UARTSend(0x88);
         for(i=0;i<4;i++)
           UARTSend(data2[i]);//
       }
        else  UARTSend(0x44);
       break;
     case 0x96: //备份电子钱包
       status = PcdBakValue(nblock, nblock+1);
        if (status == MI_OK)
       {  
         UARTSend(0x88);
        
       }
        else  UARTSend(0x44);
        break;
     default:break;
     
     }
    
       
  //    PcdHalt();
  //    PcdAntennaOff();
 }
}
(7)RC522的库函数和网上都基本一样，就不写了。
以上程序本人已经测试过了。