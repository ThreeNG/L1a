#pragma sfr
#include "r_macro.h"  /* System macro and standard type definition */
#include "r_spi_if.h" /* SPI driver interface */
#include "lcd.h"      /* LCD driver interface */
#include "api.h" /*API for this exercise*/
#include <string.h>

/******************************************************************************
Typedef definitions
******************************************************************************/
#define SETTING      		(1)
#define PAUSED       		(2)
#define COUNTING        	(3)
#define SW1_MASK 0x40	// Corresponding to bit 6 in register
#define SW2_MASK 0x10	// Corresponding to bit 4 in register
#define SW3_MASK 0x20	// Corresponding to bit 5 in register
#define ON 			(1)
#define OFF 			(0)
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void Wait1CentiSecond(void); // prototype of ASM subroutine

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

volatile int G_elapsedTime;   // Timer Counter
int second;
int minute;
int time_change_stt;
int status;
char string_shown_on_lcd[7];

int match_times;
boolean match_with_previous_input_data;
boolean previous, current, fixed_data, read_data;
boolean sw1, sw2, sw3;
boolean currentStatus, previousStatus;

/******************************************************************************
Private global variables and functions
******************************************************************************/
void r_main_userinit(void);
void Display_LCD(void);
void LCD_init(void);
void delay_1s(void);
int status_SW1 (void);
int status_SW2 (void);
int status_SW3 (void);
void pollingSW3 (void);
  
/******************************************************************************
* Function Name: main
* Description  : Main program
* Arguments    : none
* Return Value : none
******************************************************************************/
void main()
{	
	//init switchs
	PM7 = 0xFF;
	/* Initialize user system */
	r_main_userinit();
	second = 0;
	minute = 0;
	status = SETTING;
	time_change_stt = 0;
	
	LCD_init();

	while (time_change_stt == 0)
	{
		if (status_SW1())  //sw1 is pressed
		{
			if (status != PAUSED);
			{
				second ++;
				time_change_stt = 1;
			}
		}
		
		if (status_SW2())//sw2 is pressed
		{		
			if (status != PAUSED)
				minute++;
			if (status == PAUSED)
			{
				second = 0;
				minute = 0;
				status = SETTING;
			}
			time_change_stt = 1;
		}
		
		
		
		if (status_SW3())//sw3 is pressed
		{	
		pollingSW3();
		if (sw3 = ON)
		{
			if (status == COUNTING)
				status = PAUSED;
				
			if ((status != COUNTING)&&((minute!=0)||(second!=0)))
				status = COUNTING;
			time_change_stt = 1;
		}	
		}
		
		if (status == COUNTING)
		{
				second --;	
				if ((second < 0)||(second > 59))
				{
					second = 59;
					minute--;
				}
				if (minute > 59)
					minute = 0;
				if (minute < 0)
				{
					second = 0;
					minute = 0;
					status = SETTING;
				}
				
				time_change_stt = 1;
				delay_1s();
		}		
				
		if (status == SETTING)
		{
				if (second >= 60)
					second = 0;
				if (minute >= 60)
					minute = 0;
		}
		
		if (time_change_stt == 1)
		{
			Display_LCD();
			time_change_stt = 0;
		}
		
		Wait1CentiSecond();
	}	
}

void pollingSW3(void)
{	
	/*currentStatus = status_SW3();
	if (currentStatus&&(!previousStatus))
		current = ON;
		else current = OFF;
	if (currentStatus!=previousStatus)
		previousStatus = currentStatus;
		
	if (current&&(!previous))		
		match_with_previous_input_data = 1;
	
		//Read the value of switch port
		if (! match_with_previous_input_data) 
		{
			match_times = 0;
			previous = current;
		} else
		{
			match_times = match_times + 1;
			if (match_times = 3)
			{
				match_times = 0;
				sw3 = 1;
			}
		}*/
		
		if(status_SW3())
	{
		Wait1CentiSecond();
		match_times++;
		if(match_times>=6)
		{
			match_times=0;
			sw3 = ON;
		}
	}
	else 
	{	
		match_times=0;
	}
	sw3 = OFF;
}

int status_SW1 (void)
	{

		char ch;
		PM7 = (PM7 | SW1_MASK);
		ch = P7; // Read P7
		if (ch & SW1_MASK)
		return OFF; // High means switch is not pressed
		else return ON; // Low means switch is pressed
	}
int status_SW2 (void)
	{
		char ch;
		PM7 = (PM7 | SW2_MASK);
		ch = P7; // Read P7
		if (ch & SW2_MASK)
		return OFF; // High means switch is not pressed
		else return ON; // Low means switch is pressed
	}
int status_SW3 (void)
	{
		char ch;
		PM7 = (PM7 | SW3_MASK);
		ch = P7; // Read P7
		if (ch & SW3_MASK)
		return OFF; // High means switch is not pressed
		else return ON; // Low means switch is pressed
	}

void delay_1s()
{
	
	for ( G_elapsedTime = 0; G_elapsedTime<=100; G_elapsedTime++)
	{
		
		if (status_SW3())//sw3 is pressed
		{	
		pollingSW3();
		if (sw3 = ON)
		{
			if (status == COUNTING)
				status = PAUSED;
				
			if ((status != COUNTING)&&((minute!=0)||(second!=0)))
				status = COUNTING;
			time_change_stt = 1;
			break;
		}	
		}
		Wait1CentiSecond();
	}	
}	

void Display_LCD()
{
	if (status == 1)
	{
		DisplayLCD(LCD_LINE1,(unsigned char *)"Setting..."); 
	}
	if (status == 2)
	{
		DisplayLCD(LCD_LINE1,(unsigned char *)"Paused..."); 
	}
	if (status == 3)
	{
		DisplayLCD(LCD_LINE1,(unsigned char *)"Counting...");
	}
	
	string_shown_on_lcd[0]=(char)(minute/10)+'0';
        string_shown_on_lcd[1]=(char)(minute%10)+'0';
        string_shown_on_lcd[2]=':';
                        
        string_shown_on_lcd[3]=(char)(second/10)+'0';
        string_shown_on_lcd[4]=(char)(second%10)+'0';
        string_shown_on_lcd[5]=' ';
	string_shown_on_lcd[6]='\0';
	
                                              
        DisplayLCD(LCD_LINE2, (unsigned char *)string_shown_on_lcd);		
}

void LCD_init()
{
	/* Clear LCD display */
	ClearLCD();
	Display_LCD(status);
}

/******************************************************************************
* Function Name: r_main_userinit
* Description  : User initialization routine
* Arguments    : none
* Return Value : none
******************************************************************************/
void r_main_userinit(void)
{
	uint16_t i;

	/* Enable interrupt */
	EI();

	/* Output a logic LOW level to external reset pin*/
	P13_bit.no0 = 0;
	for (i = 0; i < 1000; i++)
	{
		NOP();
	}

	/* Generate a raising edge by ouput HIGH logic level to external reset pin */
	P13_bit.no0 = 1;
	for (i = 0; i < 1000; i++)
	{
		NOP();
	}

	/* Output a logic LOW level to external reset pin, the reset is completed */
	P13_bit.no0 = 0;
	
	/* Initialize SPI channel used for LCD */
	R_SPI_Init(SPI_LCD_CHANNEL);
	
	/* Initialize Chip-Select pin for LCD-SPI: P145 (Port 14, pin 5) */
	R_SPI_SslInit(
		SPI_SSL_LCD,             /* SPI_SSL_LCD is the index defined in lcd.h */
		(unsigned char *)&P14,   /* Select Port register */
		(unsigned char *)&PM14,  /* Select Port mode register */
		5,                       /* Select pin index in the port */
		0,                       /* Configure CS pin active state, 0 means active LOW level  */
		0                        /* Configure CS pin active mode, 0 means active per transfer */
	);
	
	/* Initialize LCD driver */
	InitialiseLCD();	
}

/******************************************************************************
End of file
******************************************************************************/
