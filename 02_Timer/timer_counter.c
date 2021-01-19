#include <lpc214x.h>

#define PLOCK 0x00000400
#define PRESCALE 60000   //60000 PCLK clock cycles to increment TC by 1 

void delayMS(unsigned int milliseconds);
void initClocks(void);
void initTimer1(void);
void setupPLL0(void);
void feedSeq(void);
void connectPLL0(void);


int main(void)
{
	initClocks(); //Initialize CPU and Peripheral Clocks @ 60Mhz
	initTimer1(); //Initialize Timer0
    IO1DIR|=(1<<25);
   
    while(1)
    {
        IO1SET=(1<<25);
        delayMS(500); //0.5 Second(s) Delay
        IO1CLR=(1<<25);
        delayMS(500);
    }
    //return 0; //normally this wont execute ever :P
	
}

void initTimer1(void)
{
    /*Assuming that PLL0 has been setup with CCLK = 60Mhz and PCLK also = 60Mhz.*/
   
    T1CTCR = 0x0;
    T1PR = PRESCALE-1; //(Value in Decimal!) - Increment T0TC at every 60000 clock cycles
                     //Count begins from zero hence subtracting 1
                     //60000 clock cycles @60Mhz = 1 mS

	T1TCR = 0x02; //Reset Timer
}

void delayMS(unsigned int milliseconds) //Using Timer0
{
	T1TCR = 0x02; //Reset Timer

    T1TCR = 0x01; //Enable timer
   
    while(T1TC < milliseconds); //wait until timer counter reaches the desired delay
   
    T1TCR = 0x00; //Disable timer
}

void initClocks(void)
{
    setupPLL0();
    feedSeq(); //sequence for locking PLL to desired freq.
    connectPLL0();
    feedSeq(); //sequence for connecting the PLL as system clock
   
    //SysClock is now ticking @ 60Mhz!
       
    VPBDIV = 0x01; // PCLK is same as CCLK i.e 60Mhz
    
    //Using PLL settings as shown in : http://www.ocfreaks.com/lpc214x-pll-tutorial-for-cpu-and-peripheral-clock/
	//PLL0 Now configured!
}


//---------PLL Related Functions :---------------

void setupPLL0(void)
{
    //Note : Assuming 12Mhz Xtal is connected to LPC2148.
	
	PLL0CON = 0x01; // PPLE=1 & PPLC=0 so it will be enabled
                    // but not connected after FEED sequence
    PLL0CFG = 0x24; // set the multipler to 5 (i.e actually 4)
                    // i.e 12x5 = 60 Mhz (M - 1 = 4)!!!
                    // Set P=2 since we want FCCO in range!!!
                    // So , Assign PSEL =01 in PLL0CFG as per the table.
}

void feedSeq(void)
{
    PLL0FEED = 0xAA;
    PLL0FEED = 0x55;
}

void connectPLL0(void)
{
    // check whether PLL has locked on to the  desired freq by reading the lock bit
    // in the PPL0STAT register

    while( !( PLL0STAT & PLOCK ));

    // now enable(again) and connect
    PLL0CON = 0x03;
}