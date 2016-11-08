RC522������ M1��ѧϰ�ܽᣨ�ģ�
�ġ���Ƭ��stm8s�������RC522ͨѶ----�������
      ��Ƭ����������Ľӿ�
      RC522�������ṩ���ֽӿڣ��ֱ���UART,I2C,SPI�����ֽӿڡ���������ѡ����spi�ӿڡ�����stm8sΪ������rc522������Ϊ�ӻ���spiʹ��4���ܽ�����豸������MISO,MOSI,SCK,NSS.
       ������һ����Ҫע�⣺MOSI�ܽ��໥���ӣ�MISO���໥���ӡ����������������ʹ�֮�䴮�еش��䣨MSBλ��ǰ����ͨ�����������豸�������豸ͨ��MOSI�Ű����ݷ��͸����豸�����豸ͨ��MISO���Żش����ݡ�����ζȫ˫��ͨ�ŵ����������������������ͬһ��ʱ���ź�ͬ���ģ�ʱ���ź������豸ͨ��SCK���ṩ��
��1��SPI��ʼ�����£�
void init_SPI_Master(void)
{        
        SPI_CR1 = 0x04;          //fMaster/2, ����Ϊ���豸�����е͵�ƽ����һ��ʱ�����ڿ�ʼ����
        SPI_CR2 = 0x03;           //�ڲ�ָ����ģʽ��NSS�ϵ�ƽ������λֵ
        SPI_CRCPR = 0x07;
        SPI_ICR = 0x00;            //�жϽ�ֹ
        SPI_CR1 |= 0x40;          //����SPI

}
��2��UART��ʼ��������Ϊ�˵��Է��㣬����ʹ��uart�ӿ����۲�ִ�������
void init_UART(void)
{
       UART1_CR2=0x00;//ʹ���ͽ���TEN=0��
       UART1_CR1=0x00;//����M�ֳ���8λ����λ
       UART1_CR3=0x00;//1λֹͣλ
 
       UART1_BRR2=0X00;      //������9600����Ƶϵ��=2000000/9600=208
       UART1_BRR1=0X0D;       //208=0X0D;
       UART1_CR2=0X2C;       //b3=1,������  b2=1,�������  b5=1,������������ж�
}
 ��3��UART�������ݺ���
void UARTSend(uchar aa)
{
  UART1_CR2=0x00;
  UART1_DR=aa;                   
  UART1_CR2=0x08;
  while((UART1_SR & 0x40) ==0);
  UART1_CR2=0x2c;//0x2c;//0x08; 2λ revʹ�� 3λ ����ʹ�� 5--rien 
}
��4��UART�����жϴ�����
@far @interrupt void UART1_RX_IRQHandler(void)  //UART1_Receiver(void)
{
     unsigned char i;
     _asm("sim");    //�����ж�
      a=UART1_DR;      //�����յ��ַ��ŵ�a��
     _asm("rim");    //�����ж�
    return;
}
��5��SPI���շ����ֽ�
//spi�ӿ���д��ʱ��Ҫ�ȷ��ͣ����Ͳ�һ��Ҫ�������ݣ�ֻ��Ϊ��
//���ӻ��ṩʱ�ӣ���Ϊsck�������������ģ�����д��������Ҫ�з���ָ�Ŀ�����ṩ�ӻ�дʱ��
uchar SPIWriteByte(uchar spiData)
{
   uchar retry=0;
  while((SPI_SR & 0X02)==0)  //�ȴ���������
  {
   retry++;
   if(retry>200) return 0;  //��ֹ����
  }
 
  SPI_DR=spiData;             //����һ��byte
  retry=0;
  while((SPI_SR|0X80)==0X80);  //�ȴ�ͨ�Ž���
 
  while((SPI_SR & 0X01)==0);  //�ȴ����ջ�������
  {
     retry++;
    if(retry>200) return 0;
  }
  return SPI_DR;    //�����ܵ�����
 
}
��6��main.c��������
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
 
 _asm("rim");    //�����ж�,���ڲ������ڽ����ж�
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
        //����һ�½ӿ��Ƿ���ȷ
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
       //��֤��
         status = PcdAuthState(PICC_AUTHENT1A, nblock, DefaultKey, g_ucTempbuf);
         if (status != MI_OK)
         {    continue;    }
        //   UARTSend(0x14);
        delay(20);//
       if(a==b) continue;
       else b=a;
       UARTSend(a);  //����ִ�е��������
     switch(a)
     {
     case 0x91://д����Ǯ�����ݵ���nblock
       status = PcdWrite(nblock, data1);
         if(status==MI_OK)
       {
        UARTSend(0x88);
        
       }
       else UARTSend(0x44);
       break;
     case 0x92://д��ͨ���ݵ���nblock
       status = PcdWrite(nblock, data3);
         if(status==MI_OK)
       {
        UARTSend(0x88);
        
       }
       else UARTSend(0x44);
         break;
     case 0x93://�ӿ�n�ж�ȡ����
       status = PcdRead(nblock, g_ucTempbuf);//PcdRead(2, g_ucTempbuf);
        if (status == MI_OK)
        {   
        UARTSend(0x88);
        for(i=0;i<16;i++)
         UARTSend(g_ucTempbuf[i]);//��д��ȥ����
        }
        else  UARTSend(0x44);
        break;
     case 0x94://�ӿ�n�ж�ȡ����
       status = PcdRead(nblock, g_ucTempbuf);//PcdRead(2, g_ucTempbuf);
        if (status == MI_OK)
        {   
        UARTSend(0x88);
        for(i=0;i<16;i++)
         UARTSend(g_ucTempbuf[i]);//��д��ȥ����
        }
        else  UARTSend(0x44);
        break;
     case 0x95://��������Ǯ��
       status = PcdValue(PICC_DECREMENT,nblock,data2);
       if (status == MI_OK)
       {  
         UARTSend(0x88);
         for(i=0;i<4;i++)
           UARTSend(data2[i]);//
       }
        else  UARTSend(0x44);
       break;
     case 0x96: //���ݵ���Ǯ��
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
(7)RC522�Ŀ⺯�������϶�����һ�����Ͳ�д�ˡ�
���ϳ������Ѿ����Թ��ˡ�