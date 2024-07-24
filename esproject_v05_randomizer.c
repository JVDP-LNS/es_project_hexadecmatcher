#include<lpc17xx.h>
#include<stdio.h>
//CNB Keyboard	P1.23 - 26 Col(i/p), P2.10 - 13 Row(o/p)
//CNC LED		P0.15 - 22
//CND LCD		P0.23 - 28

//Function prototypes
void randomizer(void);
void randomizer() {
	rv = 0x1000000;
    while (1) {	//START STATE wait for input, depending on how long it took to get input from keyboard, random number is selected for matching
        for (row = 1; row < 5; row++) {	//keyboard scan to check for input
			rv++;
            if (row == 1) var1 = 0x00000400;
            else if (row == 2) var1 = 0x00000800;
            else if (row == 3) var1 = 0x00001000;
            else if (row == 4) var1 = 0x00002000;
            temp = var1;
            LPC_GPIO2 -> FIOCLR = 0x00003C00;
            LPC_GPIO2 -> FIOSET = var1;
            flag1 = 0;
            index1++;	//cycle through list of random hexadecs
            if (index1 == 12)
                index1 = 0;
            scan2();	//check for any input
            if (flag1 == 1)	//end START state if input
                break;
        }
        if (flag1 == 1)
            break;
    }
	sprintf(msg,"%7x", rv);			//get random hexadec
	for(row = 0; row < 7; row++) {	//convert to uppercase
		if(msg[i] >= 'a' && msg[i] <= 'z') {
			msg[i] -= 32;
		}
	}
}
void end_game(void);
void win_game(void);
void lcd_init(void);
void write(int, int);
void lcd_comdata(int, int);
void lcd_puts(unsigned char * );
void delay_lcd(unsigned int);
void scan(void);
void scan2(void);
void timer_init(void);
void delay_timer(unsigned int);

//LCD Messages
unsigned int rv = 0x1000000;
unsigned char msg[16];		//Strings list containing random hexadec
unsigned int index1 = 0;	//cycle through random hexadec
unsigned char wlcmsg[17] = "PRESS ANY KEY"; //Initial Start message
unsigned char msg3[13] = "YOU LOSE"; //Message displayed after losing
unsigned char msg4[13] = "YOU WIN"; //Message displayed after correct matching
unsigned char input[13]; //String to store player input
int ind = 0; //track current input index

//Keyboard related variables
unsigned char row,
var, flag, key;
unsigned int flag1;
unsigned long int i, var1, temp, temp1, temp2, temp3;
unsigned char SCAN_CODE[16] = {
    0x11,
    0x21,
    0x41,
    0x81,
    0x12,
    0x22,
    0x42,
    0x82,
    0x14,
    0x24,
    0x44,
    0x84,
    0x18,
    0x28,
    0x48,
    0x88
};
unsigned char ASCII_CODE[16] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F'
};
//LED related variables
unsigned int LED = 0xFF;
unsigned int LEDcount = 0;	//actual counter for matching, fail when it == 8
unsigned int LEDi = 0;		//fast counter for LED

//START MAIN --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
int main(void) {
    LPC_GPIO2 -> FIODIR |= 0x00003C00;	//P2.10 - 13 rows(o/p)
    LPC_GPIO1 -> FIODIR &= 0xF87FFFFF;	//P1.23 - 26 col(i/p)	Keyboard
    LPC_GPIO0 -> FIODIR |= 0x0F << 23 | 1 << 27 | 1 << 28;	//P0.23 - 28 LCD
    LPC_GPIO0 -> FIODIR |= 0xFF << 15;	//P0.15 - 22 LEDS
	
	//Timer init
	timer_init();
	
	//LCD init
    //clearing ports LCD
    LPC_GPIO0 -> FIOCLR = 0x0F << 23;	
    LPC_GPIO0 -> FIOCLR = 1 << 27;
    LPC_GPIO0 -> FIOCLR = 1 << 28;
    delay_lcd(3200);
    lcd_init();
    lcd_comdata(0x80, 0);
    delay_lcd(800);
	
	//Entry
    lcd_puts(wlcmsg);	//display start message
    delay_lcd(800);

    while (1) {	//START STATE wait for input, depending on how long it took to get input from keyboard, random number is selected for matching
        for (row = 1; row < 5; row++) {	//keyboard scan to check for input
            if (row == 1) var1 = 0x00000400;
            else if (row == 2) var1 = 0x00000800;
            else if (row == 3) var1 = 0x00001000;
            else if (row == 4) var1 = 0x00002000;
            temp = var1;
            LPC_GPIO2 -> FIOCLR = 0x00003C00;
            LPC_GPIO2 -> FIOSET = var1;
            flag1 = 0;
            index1++;	//cycle through list of random hexadecs
            if (index1 == 12)
                index1 = 0;
            scan2();	//check for any input
            if (flag1 == 1)	//end START state if input
                break;
        }
        if (flag1 == 1)
            break;
    }
	
    lcd_comdata(0x01, 0);	//reseting LCD
    delay_lcd(800);
    lcd_comdata(0x80, 0);
    delay_lcd(800);
    lcd_puts(msg);	//display selected random string
    delay_lcd(1000000);
    while (1) {	//RUNNING STATE looks for keyboard input
        while (1) {
            for (row = 1; row < 5; row++) {	//usual keyboard scan
                if (row == 1) var1 = 0x00000400;
                else if (row == 2) var1 = 0x00000800;
                else if (row == 3) var1 = 0x00001000;
                else if (row == 4) var1 = 0x00002000;
                temp = var1;
                LPC_GPIO2 -> FIOCLR = 0x00003C00;
                LPC_GPIO2 -> FIOSET = var1;
                flag = 0;
                scan();
                if (flag == 1) break;
            }
            LEDi++;
            if (LEDi > 1000) {
                LEDi = 0;
                LED >>= 1;
                LEDcount++;	//counts time passed
            }
            if (LEDcount >= 8) {
                end_game();	//fail if time ends
            }
            LPC_GPIO0 -> FIOCLR |= 0xFF << 15;	//LED update
            LPC_GPIO0 -> FIOSET |= LED << 15;
            if (flag == 1) break;
			timer_delay(2);
        } //2nd while
		
        for (i = 0; i < 16; i++) {	//Scan keyboard for input
            if (key == SCAN_CODE[i]) {	//input detected, get corresponding hexadec, append to input
                key = ASCII_CODE[i];	
                input[ind++] = key;		
                input[ind] = '\0';		
                break;					
            }
        }
        lcd_comdata(0xC0, 0);
        delay_lcd(800);
        lcd_puts(input);	//update 2nd line with current input
        if (input[ind - 1] != msg[index1][ind - 1]) {	//compare most recent input hexadec with corresponding hexadec of random number
            end_game();	//fail if not matching
        }
        if (ind == 7) win_game();	//if all hexadecs are matched
		
        delay_timer(200);
    }
}
//END MAIN --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---


void end_game() {//what happens when u lose
    lcd_comdata(0x1, 0);
    delay_lcd(800);
    lcd_comdata(0x80, 0);
    delay_lcd(800);
    lcd_puts( & msg3[0]);
    while (1) {
        delay_lcd(800);
        //nothing
    }
}

void win_game() {//hexadec matched within time
    lcd_comdata(0x1, 0);
    delay_lcd(800);
    lcd_comdata(0x80, 0);
    delay_lcd(800);
    lcd_puts( & msg4[0]);
    while (1) {
        delay_lcd(800);
        //nothing
    }
}

//LCD functions
void lcd_init() {
    int l;
    LPC_PINCON -> PINSEL1 &= 0xFC003FFF;	//P0.23-28
    LPC_GPIO0 -> FIODIR |= 0x0F << 23 | 1 << 27 | 1 << 28;
    //clearing ports
    LPC_GPIO0 -> FIOCLR = 0x0F << 23;
    LPC_GPIO0 -> FIOCLR = 1 << 27;
    LPC_GPIO0 -> FIOCLR = 1 << 28;
	//initialization commands
	delay_lcd(3200);
    lcd_comdata(0x33, 0);
	delay_lcd(30000);
    lcd_comdata(0x32, 0);	
	delay_lcd(30000);
    lcd_comdata(0x28, 0);	
	delay_lcd(30000);
    lcd_comdata(0x0C, 0);
	delay_lcd(800);
    lcd_comdata(0x06, 0);
	delay_lcd(800);
    lcd_comdata(0x01, 0);
	delay_lcd(1000);
}

void write(int temp2, int type) {
    int l;
    //clearing ports
    LPC_GPIO0 -> FIOCLR = 0x0F << 23;
    LPC_GPIO0 -> FIOCLR = 1 << 27;
    LPC_GPIO0 -> FIOCLR = 1 << 28;
    LPC_GPIO0 -> FIOSET |= temp2;
    if (type == 0)
        LPC_GPIO0 -> FIOCLR = 1 << 27;
    else
        LPC_GPIO0 -> FIOSET = 1 << 27;
    LPC_GPIO0 -> FIOSET = 1 << 28;
    //delay
    for (l = 0; l < 25; l++);
    LPC_GPIO0 -> FIOCLR = 1 << 28;
}
void lcd_comdata(int tmp, int type) {
    int l;
    int temp2 = tmp & 0xF0;
    temp2 = temp2 << 19;
    write(temp2, type);
    temp2 = tmp & 0X0F;
    temp2 = temp2 << 23;
    write(temp2, type);
    //delay
    for (l = 0; l < 1000; l++);
}

void lcd_puts(unsigned char * buf1) {
    unsigned int i = 0;
    unsigned int temp3;
    while (buf1[i] != '\0') {
        temp3 = buf1[i];
        lcd_comdata(temp3, 1);
        i++;
        if (i == 16) lcd_comdata(0xc0, 0);
    }
}

void delay_lcd(unsigned int del) {
    int k;
    for (k = 0; k < del; k++);
}

//Keyboard functions
void scan(void) {//find which key was pressed
    temp3 = LPC_GPIO1 -> FIOPIN;
    temp3 &= 0x07800000;
    if (temp3 != 0x00000000) {
        flag = 1;
        temp3 >>= 19;
        temp >>= 10;
        key = temp3 | temp;
    }
}

void scan2(void) {//find if atleast 1 key was pressed
    temp3 = LPC_GPIO1 -> FIOPIN;
    temp3 &= 0x07800000;
    if (temp3 != 0x00000000) {
        flag1 = 1;
    }
}

void timer_init() {
	LPC_SC->PCONP |= (1<<1);
	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->TCR = 0x2;
	LPC_TIM0->PR = 599;
	LPC_TIM0->TCR = 0x2;
}

void delay_timer(unsigned int delay) {
	LPC_TIM0->TCR = 0x1;
	while(LPC_TIM0->TC < delay);
	LPC_TIM0->TCR = 0x2;
}