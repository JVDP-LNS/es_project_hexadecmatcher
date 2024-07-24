#include<lpc17xx.h>

//Function prototypes
void end_game(void);
void win_game(void);
void lcd_init(void);
void write(int, int);
void lcd_comdata(int, int);
void lcd_puts(unsigned char * );
void delay_lcd(unsigned int);
void scan(void);
void scan2(void);

//LCD Messages
unsigned char msg[13][20] = {
    "7AB5EF8",
    "856BAB7",
    "4567ABC",
    "6879ABC",
    "ABC1234",
    "D12345A"
};	//Strings list containing random hexadec
unsigned char wlcmsg[17] = "PRESS ANY KEY"; //Initial Start message
unsigned char msg3[13] = "YOU LOSE"; //Message displayed after losing
unsigned char msg4[13] = "YOU WIN"; //Message displayed after correct matching
unsigned char input[13]; //String to store player input
int ind = 0; //track current input index

//Keyboard related variables
unsigned int index1 = 0;
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
unsigned int LEDtemp;
unsigned int LEDcount = 0;
unsigned int LEDi = 0;

//START MAIN --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
int main(void) {
    LPC_GPIO2 -> FIODIR |= 0x00003C00;
    LPC_GPIO1 -> FIODIR &= 0xF87FFFFF;
    LPC_GPIO0 -> FIODIR |= 0x0F << 23 | 1 << 27 | 1 << 28;
    LPC_GPIO0 -> FIODIR |= 0xFF << 15;
    //clearing ports
    LPC_GPIO0 -> FIOCLR = 0x0F << 23;
    LPC_GPIO0 -> FIOCLR = 1 << 27;
    LPC_GPIO0 -> FIOCLR = 1 << 28;
    delay_lcd(3200);
    lcd_init();
    lcd_comdata(0x80, 0);
    delay_lcd(800);
    lcd_puts(wlcmsg);
    delay_lcd(800);

    while (1) {
        for (row = 1; row < 5; row++) {
            if (row == 1) var1 = 0x00000400;
            else if (row == 2) var1 = 0x00000800;
            else if (row == 3) var1 = 0x00001000;
            else if (row == 4) var1 = 0x00002000;
            temp = var1;
            LPC_GPIO2 -> FIOCLR = 0x00003C00;
            LPC_GPIO2 -> FIOSET = var1;
            flag1 = 0;
            index1++;
            if (index1 == 5)
                index1 = 0;
            scan2();
            if (flag1 == 1)
                break;
        }
        if (flag1 == 1)
            break;
    }
    lcd_comdata(0x01, 0);
    delay_lcd(800);
    lcd_comdata(0x80, 0);
    delay_lcd(800);
    lcd_puts(msg[index1]);
    delay_lcd(1000000);
    while (1) {
        while (1) {
            for (row = 1; row < 5; row++) {
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
            if (LEDi > 10000) {
                LEDi = 0;
                LED >>= 1;
                LEDcount++;
            }
            if (LEDcount >= 8) {
                end_game();
            }
            LPC_GPIO0 -> FIOCLR |= 0xFF << 15;
            LPC_GPIO0 -> FIOSET |= LED << 15;
            if (flag == 1) break;
        } //2nd while
        for (i = 0; i < 30000; i++);
        for (i = 0; i < 16; i++) {
            if (key == SCAN_CODE[i]) {
                key = ASCII_CODE[i];
                input[ind++] = key;
                input[ind] = '\0';
                break;
            }
        }
        lcd_comdata(0xC0, 0);
        delay_lcd(800);
        lcd_puts(input);
        if (input[ind - 1] != msg[index1][ind - 1]) {
            end_game();
        }
        if (ind == 7) win_game();
        for (i = 0; i < 50000; i++);
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
    LPC_PINCON -> PINSEL1 &= 0xFC003FFF;
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