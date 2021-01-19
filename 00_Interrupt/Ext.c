#include<lpc214x.h>
char ch;
int flag;
void init()
{
	PINSEL0 = 0x05;
	U0LCR = 0x83;
	U0DLL = 0xc3;
	U0LCR = 0x03;

}

void uart_recv_isr()__irq
{
	if(U0IIR==0x04)
	{
		ch = U0RBR;
		flag = 1;
	}
	//U0IER = 0x01;
	U0IER = 0x00; //0x00;
//	U0IER = 0x02;
	VICVectAddr0 = 0x00;
}

/*void uart_trns()__irq
{
	while(!(U0LSR&0x20));
	U0THR = ch;
} */

void delay()
{
	int i=0;
	for(i=0;i<600000;i++);
}

int main()
{
	
	VPBDIV = 0x02;
	 //U0IER = 0x00;
	 U0IER = 0x01; //(RBR interrupt enable) bit 0
	 init();
	 VICIntEnable |= (1<<6);//enabled uart0 interrupt which is on bit 6
	 VICVectCntl0 = 0x20|6;
	 VICVectAddr0 = (unsigned long)uart_recv_isr;
	 //U0IER = 0x02;
	 if(flag == 1)
	 {
	 	 U0THR = ch;
		 flag = 0;
	 }

	

	while(1);
}