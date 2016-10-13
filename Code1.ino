#include <SPI.h>
/*數位訊號*/
byte Ddata0=0;//數位訊號
byte oldDdata0=0;
byte Ddata1=0;
byte oldDdata1=0;
byte Ddata2=0;
byte oldDdata2=0;
byte Ddata3=0;
byte oldDdata3=0;
byte Ddata4=0;
byte oldDdata4=0;
byte Ddata5=0;
byte oldDdata5=0;
byte Ddata6=0;
byte oldDdata6=0;
byte Ddata7=0;
byte oldDdata7=0;
byte Ddata8=0;
byte oldDdata8=0;

int P8s0=0x1;
int P8s1=0x4;
int P8s2=0x40;
int P16s0=0x2;
int P16s1=0x8;
int P16s2=0x200;
int P16s3=0x400;

byte PZTcheck0=0;//檢查是否需要讀取電壓
byte PZTcheck1=0;
byte PZTcheck2=0;
byte PZTcheck3=0;
byte PZTcheck4=0;
byte PZTcheck5=0;
byte PZTcheck6=0;
byte PZTcheck7=0;
byte PZTcheck8=0;

byte PZTtransfer0=0;//檢查是否讀取到最大值
byte PZTtransfer1=0;
byte PZTtransfer2=0;
byte PZTtransfer3=0;
byte PZTtransfer4=0;
byte PZTtransfer5=0;
byte PZTtransfer6=0;
byte PZTtransfer7=0;
byte PZTtransfer8=0;

boolean pinB26=false;//check是否需要再寫一次digitalwrite
boolean pinA15=false;//用來check是否可以傳輸

unsigned int PZTvalue=0x0;
const int PZTHIGH=10;
const int PZTLOW=5;

byte PZTval0[8]={0,0,0,0,0,0,0,0};//存放壓電的電壓直
byte PZTval1[8]={0,0,0,0,0,0,0,0};
byte PZTval2[8]={0,0,0,0,0,0,0,0};
byte PZTval3[8]={0,0,0,0,0,0,0,0};
byte PZTval4[8]={0,0,0,0,0,0,0,0};
byte PZTval5[8]={0,0,0,0,0,0,0,0};
byte PZTval6[8]={0,0,0,0,0,0,0,0};
byte PZTval7[8]={0,0,0,0,0,0,0,0};
byte PZTval8[8]={0,0,0,0,0,0,0,0};

byte oldPZTval0[8]={0,0,0,0,0,0,0,0};//用來和新讀到的電壓比較
byte oldPZTval1[8]={0,0,0,0,0,0,0,0};
byte oldPZTval2[8]={0,0,0,0,0,0,0,0};
byte oldPZTval3[8]={0,0,0,0,0,0,0,0};
byte oldPZTval4[8]={0,0,0,0,0,0,0,0};
byte oldPZTval5[8]={0,0,0,0,0,0,0,0};
byte oldPZTval6[8]={0,0,0,0,0,0,0,0};
byte oldPZTval7[8]={0,0,0,0,0,0,0,0};
byte oldPZTval8[8]={0,0,0,0,0,0,0,0};

boolean noteon0[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon1[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon2[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon3[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon4[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon5[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon6[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon7[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間
boolean noteon8[8]={0,0,0,0,0,0,0,0}; //用來暫存讀到的時間

boolean noteoff0[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff1[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff2[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff3[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff4[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff5[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff6[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff7[8]={0,0,0,0,0,0,0,0}; 
boolean noteoff8[8]={0,0,0,0,0,0,0,0}; 

byte transfer[288]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int VTH=3;


unsigned int transnumber=0;  //代表現在要存入transfer矩陣的第幾個位置

byte getPZTval()
{
  boolean checkbreak=false;
  byte PZTval=0;
  unsigned int PZTtemp=0;
  PZTvalue=REG_PIOC_PDSR;
 // Serial.println(REG_PIOC_PDSR,BIN);
 // Serial.println(REG_PIOD_ODSR,BIN);



  for (int i=0;i<8;i++)
  {
    if ((PZTvalue&(1<<(i+12)))==(1<<(i+12)))
    {
      PZTtemp=PZTtemp+1;
    }
  }
  for (int i=0;i<9;i++)
  {  
    if ((PZTvalue&(1<<(9-i)))==(1<<(9-i)))
    {
      PZTtemp=PZTtemp+1;
    }
  }   
  if((PZTvalue&(1<<28))==(1<<28))
  {
  PZTtemp=PZTtemp+1;
  }
  
  for (int i=0;i<5;i++)
  {
    if ((PZTvalue&(1<<(25-i)))==(1<<(25-i)))
    {
      PZTtemp=PZTtemp+1;
    }
  }
  //Serial.println(PZTtemp);
  return(PZTtemp);
}

void setup()
{
  REG_PIOA_OER=0x80;//pin31 A7 用來控制PL
  REG_PIOD_OER=0x64F;//把所有需要用來操控分工器的腳位弄成OUTPUTmulti 16P switch:D1(26)s0 D3(28)s1 D9(30)s2 D10(32)s3 multi 8P switch:D0(25)s0 D2(27)s1 D6(29)s2
  pinMode(51,INPUT);//把所有需要用來讀比較器的腳位都變成input
  pinMode(50,INPUT);
  pinMode(49,INPUT);
  pinMode(48,INPUT);
  pinMode(47,INPUT);
  pinMode(46,INPUT);
  pinMode(45,INPUT);
  pinMode(44,INPUT);
  pinMode(41,INPUT);
  pinMode(40,INPUT);
  pinMode(39,INPUT);
  pinMode(38,INPUT);
  pinMode(37,INPUT);
  pinMode(36,INPUT);
  pinMode(35,INPUT);
  pinMode(34,INPUT);
  pinMode(33,INPUT);
  pinMode(3,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT);
  pinMode(43,INPUT);//用來當作ARDUINO 通訊的讀取CHECK
  SPI.begin(52);
  SPI.setClockDivider(52,5); 
  Serial.begin (9600);
  Serial1.begin(2000000);
  Serial.print ("Begin STATE test.");
  Serial.print ("\n");
  REG_PIOA_SODR=0x80;
  Ddata0=SPI.transfer(52,0);
  /*Ddata1=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata2=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata3=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata4=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata5=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata6=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata7=SPI.transfer(52,0,SPI_CONTINUE);
  Ddata8=SPI.transfer(52,0);*/
  oldDdata0=Ddata0;
  oldDdata1=Ddata1;
  oldDdata2=Ddata2;
  oldDdata3=Ddata3;
  oldDdata4=Ddata4;
  oldDdata5=Ddata5;
  oldDdata6=Ddata6;
  oldDdata7=Ddata7;
  oldDdata8=Ddata8;
  Serial.print("pin state:\n");
  Serial.print("shift register0:");
  Serial.print(Ddata0,BIN);
  Serial.print("\n");
  Serial.print("shift register1:");
  Serial.print(Ddata1,BIN);
  Serial.print("\n");
  Serial.print("shift register2:");
  Serial.print(Ddata2,BIN);
  Serial.print("\n");
  Serial.print("shift register3:");
  Serial.print(Ddata3,BIN);
  Serial.print("\n");
  Serial.print("shift register4:");
  Serial.print(Ddata4,BIN);
  Serial.print("\n");
  Serial.print("shift register5:");
  Serial.print(Ddata5,BIN);
  Serial.print("\n");
  Serial.print("shift register6:");
  Serial.print(Ddata6,BIN);
  Serial.print("\n");
  Serial.print("shift register7:");
  Serial.print(Ddata7,BIN);
  Serial.print("\n");
  Serial.print("shift register8:");
  Serial.print(Ddata8,BIN);
  Serial.print("\n");
  Serial.print("\n");
}

void loop()
{
    for(int z=0;z>-1;z=z)
  {
  readShift();


    //把資料存到transfer裡面等待傳輸
    for (int i=0;i<8;i++)
    {
      //把新讀到的電壓儲存到old裡面可以用來比較，還有如果已經讀到最大值那麼就傳輸
      if (((PZTcheck0&(1<<i))==(1<<i))&&((PZTtransfer0&(1<<i)!=(1<<i))))
      {
        if (noteoff0[i]==true)
        {
          PZTcheck0=PZTcheck0&(255-(1<<i));
        }
        oldPZTval0[0]=PZTval0[i];
      }
      if(((PZTtransfer0&(1<<i))==(1<<i))&&(PZTval0[i]<oldPZTval0[i]))
      {
        PZTcheck0=PZTcheck0&(255-(1<<i));//放棄掉已經讀取到最大值的那個
        PZTtransfer0=PZTtransfer0&(255-(1<<i));
        transfer[transnumber]=i+25;
        transnumber=transnumber+1;
        transfer[transnumber]=oldPZTval0[i];
        transnumber=transnumber+1;
        PZTval0[i]=0;
        oldPZTval0[i]=0;
      }
      //存放shift register的資料
      if (noteoff0[i]!=0)//第0個shift register的資料
      {
        transfer[transnumber]=i+128;
        transnumber=transnumber+1;
        noteoff0[i]=false;
      }
    }
    if (((REG_PIOA_PDSR&0x8000)==0x8000) && (pinA15!=true))
    {
      pinA15=true;
    }
    if (pinA15==true)
    {
      for (int k=0;k<transnumber;k++)
      {
        Serial1.write(transfer[k]);
      }
      Serial1.write(255);
      transnumber=0;
    }
    pinA15=false;
  }
} 

boolean compareval(int i){
   if((PZTval0[i]<oldPZTval0[i])&(PZTval0[i]>VTH)){
      return true;
        }
    else{
      return false;
        }
}



void readShift(){
    REG_PIOA_CODR=0x80;
    REG_PIOA_SODR=0x80;
    Ddata0=SPI.transfer(52,0);
    /*Ddata1=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata2=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata3=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata4=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata5=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata6=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata7=SPI.transfer(52,0,SPI_CONTINUE);
    Ddata8=SPI.transfer(52,0);*/
    if (oldDdata0!= Ddata0)
    {
      PZTcheck0|=Ddata0;
      Serial.print("shift register0 Change to:");
      Serial.print(Ddata0,BIN);
      Serial.print("\n");
      for (int i=0;i<8;i++)
      {
        if (Ddata0&(1<<i)==(1<<i))
        {
          noteon0[i]=true;  
        }
        if (((~(Ddata0)&(1<<i))==(1<<i)) && noteon0[i]!=0) //確認是否已經note on
        {
          noteoff0[i]=true;
          noteon0[i]=false;
        }
      }
      oldDdata0= Ddata0;
    }

    if (PZTcheck0!=0)//讀取第0個分工器
    {
      if ((PZTcheck0&1)==1)
      {
        REG_PIOD_SODR=P16s3;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s0+P8s1+P8s2;
        PZTval0[0]=getPZTval();
        if(compareval(0)) 
        { 
          PZTtransfer0=PZTtransfer0+1;
        }
      }
      if ((PZTcheck0&(1<<1))==(1<<1))
      {
        REG_PIOD_SODR=P16s3+P8s0;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s1+P8s2;
        PZTval0[1]=getPZTval();
        if(compareval(1)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<1;
        }
      }
      if ((PZTcheck0&(1<<2))==(1<<2))
      {
        REG_PIOD_SODR=P16s3+P8s1;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s0+P8s2;
        PZTval0[2]=getPZTval();
        REG_PIOD_CODR=0x200;
        if(compareval(2)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<2;
        }
      }
      if ((PZTcheck0&(1<<3))==(1<<3))
      {
        REG_PIOD_SODR=P16s3+P8s0+P8s1;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s2;
        PZTval0[3]=getPZTval();
        if(compareval(3)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<3;
        }
      }
      if ((PZTcheck0&(1<<4))==(1<<4))
      {
        REG_PIOD_SODR=P16s3+P8s2;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s0+P8s1;
        PZTval0[4]=getPZTval();
        if(compareval(4)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<4;
        }
      }
      if ((PZTcheck0&(1<<5))==(1<<5))
      {
        REG_PIOD_SODR=P16s3+P8s0+P8s2;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s1;
        PZTval0[5]=getPZTval();
        if(compareval(5)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<5;
        }
      }
      if ((PZTcheck0&(1<<6))==(1<<6))
      {
        REG_PIOD_SODR=P16s3+P8s1+P8s2;
        REG_PIOD_CODR=P16s0+P16s1+P16s2+P8s0;
        PZTval0[6]=getPZTval();
        if(compareval(6)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<6;
        }
      }
      if ((PZTcheck0&(1<<7))==(1<<7))
      {
        REG_PIOD_SODR=P16s3+P8s0+P8s1+P8s2;
        REG_PIOD_CODR=P16s0+P16s1+P16s2;
        PZTval0[7]=getPZTval();
        if(compareval(7)) 
        { 
          PZTtransfer0=PZTtransfer0+1<<7;
        }
      }
    }
}





