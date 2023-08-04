#include "TM4C123.h"                    // Device header
// aim in this project is to use MQ2 smoke sensor 
//it uses ADC to detect change voltage btw 0-5 v
//we will use channel 0 AIN0 PE3
/**@brief delay function using Timer 0 */
void delayUs (int time );

/**@brief  function to initialize ADC and start conversion  */
void MQ2ADC_Init (void );

volatile static uint32_t adcresult ;
/**@brief  ADC handler function */
void ADC0SS3_Handler (void );
/**@brief function that initialize GPIO port E */
void GPIOF_Init (void);

int main ()
{
	GPIOF_Init();
	MQ2ADC_Init();
	delayUs(2000000);
	
	//NVIC_EnableIRQ(ADC0SS3_IRQn);  OR
	NVIC->ISER[0] |= 0x00020000; /* enable IRQ17 for ADC0SS3*/
	
	
	while (1)
	{
		if(adcresult>1048 )
		{
			GPIOF->DATA = 0x08 ;
			
		}
		else{
			GPIOF->DATA &= ~0x08 ;
			
		}
	}
	
	
}
void GPIOF_Init (void)
{
	SYSCTL->RCGCGPIO |= 0x20 ;
	GPIOF->DIR |= 0x08;
	GPIOF->DEN |= 0x08;
	
	
}
void MQ2ADC_Init (void )
{
   SYSCTL->RCGCGPIO |= (1U<<4);//clock for port E (AIN 0)
	 SYSCTL->RCGCADC |= (1U<<0);//clock for ADC 0

//GPIO Port Init
	GPIOE->DEN &= (~(1U<<3)); //disable Digital Functions of PE3
	GPIOE->AFSEL |= (1U<<3); //alternate mode select
	GPIOE->AMSEL |= (1U<<3);//Analog mode select

//SS configuration
	ADC0->ACTSS &= (~(1U<<3)) ;//SS3 is disabled 
	ADC0->EMUX = 0xF000;//selecting trigger source
	ADC0->SSMUX3 = 0x00 ; ////selecting input chanel - Get input from channel 0 ,get input from channel AIN0 ->PE3
	ADC0->SSCTL3 = 0x06;//***************************
	ADC0->IM = (1U<<3) ; //set Interrupt
	ADC0->ACTSS = (1U<<3);

  ADC0->PSSI |= (1<<3);        /* Enable SS3 conversion or start sampling data from AN0 */
	
}

void ADC0SS3_Handler (void )
{
	
	adcresult = ADC0->SSFIFO3 ;
	ADC0->ISC = (1U<<3);
	ADC0->PSSI |= (1<<3);        /* Enable SS3 conversion or start sampling data from AN0 */
	
}

void delayUs (int time )
{
	int i ;
	
	SYSCTL->RCGCTIMER = 0x01 ;
	TIMER0->CTL &= ~0x01 ;
	TIMER0->CFG = 0x04;
	TIMER0->TAMR = 0x02 ;
	TIMER0->TAILR = 16 - 1 ;
	TIMER0->ICR = 0x01 ;
	TIMER0->CTL = 0x01 ;
	
	for (i=0;i<time ; i++)
	{
		while ((TIMER0->RIS & 0x01 ) == 0 ){}
			TIMER0->ICR = 0x01 ;
		
	}
	
}
