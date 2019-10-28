/*
 * Using_EEPROM.c
 *
 * Created: 6/11/2014 1:54:35 AM
 *  Author: Saroj Shakya
 */ 
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "LCD Code (8-bit).c"
#define MOTOR PORTD
#define MOTOR_DDR DDRD
#define KEY_PAD_ROW PORTC
#define KEY_PAD_COLUMN PINC
#define KEY_PAD_DDR DDRC
unsigned char entered_password[16];
unsigned char set_password[16];
unsigned char keyPressed_count,skip_compare,n,chage_password_value,first_time,col,OK=1,abc;
unsigned char eeprom_data_a=0;
unsigned char eeprom_data_b;
int main();

void port_initialize()
{
	KEY_PAD_DDR = 0x0F;		//Key-board port, higer nibble - input, lower nibble - output
	KEY_PAD_ROW = 0xFF;
	MOTOR_DDR=0xFF;
}
void buzzer()
{
	PORTB|=0x10;
	_delay_ms(100);
	PORTB&=0x07;
}
void led_blink(unsigned char n)
{
	unsigned char i;
	for(i=0;i<n;i++)
	{
		PORTB|=0x30;
		_delay_ms(150);
		PORTB&=0x07;
		_delay_ms(150);
	}
}
void dc_motor_forward()
{
	MOTOR=0x10;
	_delay_ms(3000);
	MOTOR=0x00;
}
void dc_motor_backward()
{
	MOTOR=0x20;
	_delay_ms(3000);
	MOTOR=0x00;
}
void stepper_clockwise(unsigned char fanko)
{
	unsigned char i;
	for(i=0;i<fanko;i++)
	{
	MOTOR=0x08;
	_delay_ms(50);
	MOTOR=0x04;
	_delay_ms(50);
	MOTOR=0x02;
	_delay_ms(50);
	MOTOR=0x01;
	_delay_ms(50);
	}
}
void stepper_anticlockwise(unsigned char fanko)
{
	unsigned char i;
	for(i=0;i<fanko;i++)
	{
	MOTOR=0x03;
	_delay_ms(50);
	MOTOR=0x00;
	_delay_ms(50);
	MOTOR=0x04;
	_delay_ms(50);
	MOTOR=0x02;
	_delay_ms(50);
	}
}
void write_eeprom_data_a(unsigned char value_a)
{
	EEAR=0x50;
	while(EECR&(1<<EEWE));
	EEDR=value_a;
	EECR|=(1<<EEMWE);
	EECR|=(1<<EEWE);
}
void write_eeprom_data_b(unsigned char value_b)
{	
	EEAR=0x51;
	while(EECR&(1<<EEWE));
	EEDR=value_b;
	EECR|=(1<<EEMWE);
	EECR|=(1<<EEWE);
}
/*void write_eeprom()
{
	unsigned char i;
	for(i=0;i<9;i++)
	{
		EEAR=i+0x00;
		while(EECR&(1<<EEWE));
		EEDR='A';
		EECR|=(1<<EEMWE);
		EECR|=(1<<EEWE);
		write_eeprom_data_b(i);
	}
}*/
void read_eeprom_data_a()
{
	EEAR=0x50;
	while(EECR&(1<<EEWE));
	EECR|=(1<<EERE);
	eeprom_data_a=EEDR;
}
void read_eeprom_data_b()
{
	EEAR=0x51;
	while(EECR&(1<<EEWE));
	EECR|=(1<<EERE);
	eeprom_data_b=EEDR;
}
void read_eeprom()
{ 
	unsigned char i=0;
	for(i=0;i<=eeprom_data_b;i++)
	{		
		EEAR=i+0x00;
		while(EECR&(1<<EEWE));
		EECR|=(1<<EERE);
		set_password[i]=EEDR;
	}
}
void save_password()
{
	unsigned char i;
	for (i=0;i<eeprom_data_b;i++)
	{
		EEAR=i+0x00;
		while(EECR&(1<<EEWE));
		EECR|=(1<<EERE);
		set_password[i]=EEDR;
	}
}
void compare_password()
{
	if(skip_compare==8)
	{
		unsigned char i,j,k;
		j=0;
		if(keyPressed_count==eeprom_data_b)
		{
			for(i=0;i<eeprom_data_b;i++)
			{
				if(set_password[i]==entered_password[i])
				j++;
			}
			_delay_ms(100);
			skip_compare=5;
			if(j==eeprom_data_b)
			{
				chage_password_value++;
				if(chage_password_value==5) main();
				else
				{
					LCD_clear();
					LCD_print("Unlocked!!!");
					_delay_ms(1000);
					stepper_anticlockwise(2);
					_delay_ms(2000);
					dc_motor_backward();
					_delay_ms(1000);
					for(k=0;k<3;k++)
					{
						LCD_clear();
						_delay_ms(400);
						LCD_set_curser(1,4);
						LCD_print("WelCome!!!");
						LCD_set_curser(2,4);
						LCD_print("WelCome!!!");
						_delay_ms(600);
					}
					_delay_ms(5000);
					dc_motor_forward();
					_delay_ms(1000);
					stepper_clockwise(2);
					LCD_clear();
					LCD_set_curser(1,5);
					LCD_print("Locked!!!");
					_delay_ms(1000);
					
				}
				main();
			}
			else
			{
				LCD_clear();
				LCD_print("INVALID Password");
				_delay_ms(500);
				led_blink(3);
				LCD_set_curser(2,3);
				LCD_print("Try Again...");
				_delay_ms(1000);
				main();
			}
		}
		else;
	}
	else;
}
void input()
{
	unsigned char i,column_value,keyCode,keyPressed=0;
	column_value = 0xFF;
	for(i=0; i<4; i++)
	{
		_delay_ms(1);
		KEY_PAD_ROW= ~(0x01 << i);
		_delay_ms(1);  		  	 		  //delay for port o/p settling
		column_value = KEY_PAD_COLUMN | 0x0F;
		
		if (column_value != 0xFF)
		{
			_delay_ms(20); 		  		 //for de-bounce
			column_value = KEY_PAD_COLUMN | 0x0F;
			if(column_value == 0xFF) goto OUT;
			
			keyCode = (column_value & 0xF0) | (0x0F & ~(0x01 << i));
			
			while (column_value != 0xFF)
			column_value = KEY_PAD_COLUMN | 0x0F;
			
			_delay_ms(20);   			   //for de-bounce
			
			switch (keyCode)
			{
				case (0xEE): keyPressed = '*';
				break;
				case (0xED): keyPressed = '7';
				break;
				case (0xEB): keyPressed = '4';
				break;
				case (0xE7): keyPressed = '1';
				break;
				case (0xDE): keyPressed = '0';
				break;
				case (0xDD): keyPressed = '8';
				break;
				case (0xDB): keyPressed = '5';
				break;
				case (0xD7): keyPressed = '2';
				break;
				case (0xBE): keyPressed = '#';
				break;
				case (0xBD): keyPressed = '9';
				break;
				case (0xBB): keyPressed = '6';
				break;
				case (0xB7): keyPressed = '3';
				break;
				case (0x7E): keyPressed = 'D';
				break;
				case (0x7D): keyPressed = 'C';
				break;
				case (0x7B): keyPressed = 'B';
				break;
				case (0x77): keyPressed = 'A';
				break;
				default	   : keyPressed = 'X';
			}
			if (keyPressed=='1')
			{
				entered_password[keyPressed_count]='1';
				LCD_data('1');
				buzzer();
				keyPressed_count++;
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='2')
			{
				entered_password[keyPressed_count]='2';
				LCD_data('2');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='3')
			{
				entered_password[keyPressed_count]='3';
				LCD_data('3');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='4')
			{
				entered_password[keyPressed_count]='4';
				LCD_data('4');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='5')
			{
				entered_password[keyPressed_count]='5';
				LCD_data('5');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='6')
			{
				entered_password[keyPressed_count]='6';
				LCD_data('6');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='7')
			{
				entered_password[keyPressed_count]='7';
				LCD_data('7');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='8')
			{
				entered_password[keyPressed_count]='8';
				LCD_data('8');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='9')
			{
				entered_password[keyPressed_count]='9';
				LCD_data('9');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='0')
			{
				entered_password[keyPressed_count]='0';
				LCD_data('0');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='A')
			{
				entered_password[keyPressed_count]='A';
				LCD_data('A');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='B')
			{
				entered_password[keyPressed_count]='B';
				LCD_data('B');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='C')
			{
				entered_password[keyPressed_count]='C';
				LCD_data('C');
				keyPressed_count++;
				buzzer();
				LCD_cmnd(0x10);
				LCD_data('*');
				compare_password();
			}
			if (keyPressed=='D')
			{
				LCD_clear();
				LCD_print("T0 CHANGE THE");
				LCD_set_curser(2,3);
				LCD_print("Password");					
				_delay_ms(500);
				OK=1;					
				chage_password_value=4;
				main();
			}
			if (keyPressed=='#')
			{
				if(keyPressed_count!=0)
				{
					buzzer();
					LCD_cmnd(0x0E);
					LCD_cmnd(0x10);
					keyPressed_count--;
				}
			}
			if (keyPressed=='*')
			{
				if( keyPressed_count!=0 && abc!=1 )
				{
					LCD_clear();
					LCD_set_curser(1,7);
					LCD_print("OK");
					PORTB|=0x30;
					_delay_ms(300);
					PORTB&=0x07;
					OK=2;
				}
			}
		}
		OUT:;
	}
}
int main()
{	
	unsigned char a,b,i;
	port_initialize();
	LCD_initialize();
	LCD_cmnd(0x38);
	_delay_ms(20);
	LCD_cmnd(0x01);
	_delay_ms(20);
	LCD_cmnd(0x0C);
	//_delay_ms(2000);
	//save_password();
	if(first_time==0) 
	{
		LCD_cmnd(0x0C);
		LCD_set_curser(1,4);
		LCD_print("WELCOME!!!");
		_delay_ms(1000);
		/*for (j=0;j<9;j++)
		{
			LCD_cmnd(0x1C);
			_delay_ms(100);
		}
		_delay_ms(500);
		for (j=0;j<14;j++)
		{
			LCD_cmnd(0x18);
			_delay_ms(100);
		}
		_delay_ms(500);
		for (j=0;j<4;j++)
		{
			LCD_cmnd(0x1C);
			_delay_ms(100);
		}
		_delay_ms(1000);
		//for (j=0;j<3;j++)
		//{*//*
			LCD_clear();	
			LCD_print("DIGITAL SECURITY");
			LCD_set_curser(2,5);
			LCD_print("SYSTEM");
			_delay_ms(1000);
		//}
		LCD_clear();
		//LCD_set_curser(1,1);
		LCD_print("Initializing....");
		LCD_set_curser(2,1);
		LCD_print("Please Wait.....");
		_delay_ms(3000);*/
		//write_eeprom_data_a(0);
		//write_eeprom_data_b(0);
		//write_eeprom();
		read_eeprom_data_b();
		read_eeprom_data_a();
		read_eeprom();
		LCD_clear();
		LCD_print("Saved data is:");
		LCD_clear();
		LCD_print("b=");
		LCD_data(0x30+eeprom_data_b);
		LCD_print(" a=");
		LCD_data(0x30+eeprom_data_a);
		_delay_ms(1000);
		for(i=0;i<eeprom_data_b;i++)
		{
			b=set_password[i];
			LCD_data(b);
		}
		_delay_ms(1000);
	}
	first_time=1;
	keyPressed_count=0;
	if(chage_password_value==4) goto change_password;
	if(chage_password_value==5) goto set_next_password;
	if(eeprom_data_a==0)
	{
		write_eeprom_data_a(1);
		read_eeprom_data_a();
		set_next_password:
		chage_password_value=0;
		abc=0;
		LCD_clear();
		LCD_cmnd(0x80);
		LCD_print("Set Password:");
		_delay_ms(200);
		LCD_set_curser(2,1);
		KEY_PAD_DDR=0x0F;
		KEY_PAD_ROW=0xFF;
		do
		{
			input();
		}while(OK==1);
		n=keyPressed_count;
		OK=2;
		_delay_ms(100);
		LCD_clear();
		LCD_print("Ur Password is:");
		LCD_set_curser(2,1);
		write_eeprom_data_b(0);
		read_eeprom_data_b();
		for(i=0;i<n;i++)
		{
			EEAR=i+0x00;
			while(EECR&(1<<EEWE));
			set_password[i]=EEDR=entered_password[i];
			EECR|=(1<<EEMWE);
			EECR|=(1<<EEWE);
			write_eeprom_data_b(i+1);
			read_eeprom_data_b();
			a=set_password[i];
			LCD_data(a);
		}
		_delay_ms(2000);
	}
	change_password:
	skip_compare=8;
	keyPressed_count=0;
	LCD_clear();
	LCD_print("Enter Password:");
	LCD_set_curser(2,1);
	KEY_PAD_ROW=0xFF;
	while(1)
	{
		abc=1;
		input();
	}
}
