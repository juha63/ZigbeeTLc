/*
 * lcd_zth05.c
 *
 *      Created: pvvx, 28.05.2023 
 *      Edited by: FaBjE
 *		Adaption for ZTH05: juha63
 *
 *  https://github.com/pvvx/ATC_MiThermometer/issues/339
 */

#include "tl_common.h"
#if BOARD == BOARD_ZTH05
#include "chip_8258/timer.h"

#include "i2c_drv.h"
#include "lcd.h"
#include "device.h"

#define _LCD_SPEED_CODE_SEC_ _attribute_ram_code_sec_
#define ZHT05_I2C_ADDR		0x3E // 2332 NT unknown display controller

/*
 *  ZTH05 LCD buffer:  byte.bit

         --0.4--         --1.4--            --2.4--	     BAT
  |    |         |     |         |        |         |	     3.5
  |   0.5       0.0   1.5       1.0      2.5       2.0
  |    |         |     |         |        |         |      ° 3.6
 0.3     --0.1--         --1.1--            --2.1--          ---- 3.6
  |    |         |     |         |        |         |     3.6|
  |   0.6       0.2   1.6       1.2      2.6       2.2       ---- 3.7
  |    |         |     |         |        |         |     3.6|
         --0.7--         --1.7--     *      --2.7--          ---- 2.3
                                    1.3

                                         --4.4--         --5.4--
                                        |       |       |       |
      3.0         3.0                  4.5     4.0     5.5     5.0
      / \         / \                   |       |       |       |
3.4(  \ /   3.1   \ /  )3.4              --4.1--         --5.1--
      3.1   / \	  3.1                   |       |       |       |  
            \_/	                       4.6     4.2     5.6     5.2    %
            3.0                         |       |       |       |    5.3
                                         --4.7--         --5.7--

                                oo 4.3

None: 3.2. 3.3
*/

u8 lcd_i2c_addr;
u8 display_buff[LCD_BUF_SIZE]; // buff size = 0xB
u8 display_cmp_buff[LCD_BUF_SIZE];
u8 i2c_address_lcd; // = 0x7C
u8 * real_buff = display_buff+5; // pointer to data part of display_buff. See below!


//#define lcd_send_i2c_byte(a)  send_i2c_byte(lcd_i2c_addr, a)
#define lcd_send_i2c_buf(b, a)  send_i2c_bytes(i2c_address_lcd, (u8 *) b, a)

#define LCD_SYM_H	0b01100111	// "H"
#define LCD_SYM_i	0b01000000	// "i"
#define LCD_SYM_L	0b11100000	// "L"
#define LCD_SYM_o	0b11000110	// "o"

#define LCD_SYM_CON	BIT(3)	// CON connect
#define LCD_SYM_BAT	BIT(5)	// battery

/*
  Every message to LCD follows this structure
  command part ??? (5 bytes)
  0x6b // 0
  0xfc // 1
  0xc8 // 2
  0xe8 // 3
  0x08 // 4
  display data part (6 bytes)
  0xXX // 5 -> LCD digit 0
  0xXX // 6 -> LCD digit 1
  0xXX // 7 -> LCD digit 2
  0xXX // 8 -> LCD digit 3
  0xXX // 9 -> LCD digit 4
  0xXX // a -> LCD digit 5
*/

const u8 lcd_init_cmd[] =	{0xb6, 0xfc, 0xc8, 0xe8, 0x08, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff};
const u8 lcd_init_clr[] =	{0xb6, 0xfc, 0xc8, 0xe8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



/* 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F*/
const u8 display_numbers[] = {
		0b011110101, // 0
		0b000000101, // 1
		0b011010011, // 2
		0b010010111, // 3
		0b000100111, // 4
		0b010110110, // 5
		0b011110110, // 6
		0b000010101, // 7 
		0b011110111, // 8
		0b010110111, // 9
		0b001110111, // A
		0b011100110, // B
		0b011110000, // C
		0b011000111, // D 
		0b011110010, // E
		0b001110010}; // F


const u8 display_small_numbers[] = {
		0b011110101, // 0
		0b000000101, // 1
		0b011010011, // 2
		0b010010111, // 3
		0b000100111, // 4
		0b010110110, // 5
		0b011110110, // 6
		0b000010101, // 7 
		0b011110111, // 8
		0b010110111, // 9
		0b001110111, // A
		0b011100110, // B
		0b011110000, // C
		0b011000111, // D 
		0b011110010, // E
		0b001110010}; // F
	


_LCD_SPEED_CODE_SEC_
static void send_to_lcd(void){
	unsigned int buff_index;
	u8 * p = display_buff;
	if (i2c_address_lcd) {
		unsigned char r = irq_disable();
		if ((reg_clk_en0 & FLD_CLK0_I2C_EN)==0)
			init_i2c();
		else {
			gpio_setup_up_down_resistor(I2C_SCL, PM_PIN_PULLUP_10K);
			gpio_setup_up_down_resistor(I2C_SDA, PM_PIN_PULLUP_10K);
		}
		reg_i2c_id = i2c_address_lcd;
		reg_i2c_ctrl = FLD_I2C_CMD_START | FLD_I2C_CMD_ID ;
		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		for(buff_index = 0; buff_index < sizeof(display_buff); buff_index++) {
			reg_i2c_do = *p++;
			reg_i2c_ctrl = FLD_I2C_CMD_DO;
			while (reg_i2c_status & FLD_I2C_CMD_BUSY);
		}
		reg_i2c_ctrl = FLD_I2C_CMD_STOP;

		while (reg_i2c_status & FLD_I2C_CMD_BUSY);

		irq_restore(r);
	}
}
		

void init_lcd(void){
	i2c_address_lcd = scan_i2c_addr(ZTH05_I2C_ADDR << 1);
	if (i2c_address_lcd) {
		lcd_send_i2c_buf((u8 *) lcd_init_cmd, sizeof(lcd_init_cmd));
		pm_wait_us(1000);
		lcd_send_i2c_buf((u8 *) lcd_init_clr, sizeof(lcd_init_clr));
		
//		void show_reboot_screen(void);
	}
		
}

_LCD_SPEED_CODE_SEC_
void update_lcd(void){
	if(g_zcl_thermostatUICfgAttrs.display_off)
		return;
	if (memcmp(display_cmp_buff, display_buff, sizeof(display_buff))) {
		send_to_lcd();
		memcpy(display_cmp_buff, display_buff, sizeof(display_buff));
	}
}


/*0=off, 1=happy, 2=sad*/

void show_smiley(u8 state){
	real_buff[3] &= ~BIT(4);
	real_buff[3] &= ~(BIT(0) | BIT(1));

	if(state == 1) {
		real_buff[3] |= BIT(0); //Happy mouth
		real_buff[3] |= BIT(4);
	} 
	else if(state == 2) {
		real_buff[3] |= BIT(1); //Sad mouth
		real_buff[3] |= BIT(4);
	}
}

void show_ble_symbol(bool state){
	if (state)
		real_buff[4] |= LCD_SYM_CON;
	else 
		real_buff[4] &= ~LCD_SYM_CON;
}

void show_battery_symbol(bool state){
	if (state)
		real_buff[3] |= LCD_SYM_BAT;
	else 
		real_buff[3] &= ~LCD_SYM_BAT;
}

/* number in 0.1 (-995..19995), Show: -99 .. -9.9 .. 199.9 .. 1999 */
_LCD_SPEED_CODE_SEC_
__attribute__((optimize("-Os"))) void show_big_number_x10(s16 number, u8 symbol){
	real_buff[0] = 0;
	real_buff[1] = 0;
	real_buff[2] = 0;

	real_buff[3] &= ~(BIT(6) | BIT(7));
	real_buff[2] &= ~(BIT(3));

	if (symbol == 1) {
		real_buff[3] |= BIT(6); //"C"
		real_buff[2] |= BIT(3);
	}
	if (symbol == 2) {
		real_buff[3] |= BIT(6) | BIT(7); //"F"
		real_buff[2] &= ~BIT(3);
	}

	if (number > 19995) {
   		real_buff[0] |= LCD_SYM_H; // "H"
   		real_buff[1] |= LCD_SYM_i; // "i"
	} else if (number < -995) {
   		real_buff[0] |= LCD_SYM_L; // "L"
   		real_buff[1] |= LCD_SYM_o; // "o"
	} else {
		/* number: -995..19995 */
		if (number > 1995 || number < -95) {
			if (number < 0){
				number = -number;
				real_buff[0] |= BIT(1); // "-"
			}
			number = (number + 5) / 10; // round(div 10)
		} else { // show: -9.9..199.9
			real_buff[1] = BIT(3); // point
			if (number < 0){
				number = -number;
				real_buff[0] |= BIT(1); // "-"
			}
		}
		/* number: -99..1999 */
		if (number > 999) real_buff[0] |= BIT(3); // "1" 1000..1999
		if (number > 99) real_buff[0] |= display_numbers[number / 100 % 10];
		if (number > 9) real_buff[1] |= display_numbers[number / 10 % 10];
		else real_buff[1] |= display_numbers[0]; // "0"
	    real_buff[2] |= display_numbers[number %10];
	}
}

/* -9 .. 99 */
_LCD_SPEED_CODE_SEC_
__attribute__((optimize("-Os"))) void show_small_number(s16 number, bool percent){
	
	real_buff[4] &= LCD_SYM_CON; //Clear digit (except COONECT symbol)
	real_buff[5] = 0; //Clear digit

	
	if (number > 99) {
		real_buff[4] |= BIT(0) | BIT(1) | BIT(2) | BIT(5) | BIT(6) ; // "H"
		real_buff[5] |= BIT(6); // "i"
	} else if (number < -9) {
		real_buff[4] |= BIT(5) | BIT(6) | BIT(7); // "L"
		real_buff[5] |= BIT(1) | BIT(2) | BIT(6) | BIT(7); // "o"
	} else {
		if (number < 0) {
			number = -number;
			real_buff[4] |= BIT(1); // "-"
		}
		if (number > 9) 
			real_buff[4] |= display_small_numbers[number / 10 % 10];

		real_buff[5] |= display_small_numbers[number %10];
	}
	
	if (percent)
		real_buff[5] |= BIT(3); // %
	else
		real_buff[5] &= ~BIT(3); // no %

}

void show_ota_screen(void) {
	memset(&real_buff, 0, sizeof(real_buff));
	real_buff[4] = BIT(3); // "connect"
	real_buff[0] = BIT(7); // "_"
	real_buff[1] = BIT(7); // "_"
	real_buff[2] = BIT(7); // "_"
	update_lcd();
}

// #define SHOW_REBOOT_SCREEN()
void show_reboot_screen(void) {
	memset(&real_buff, 0xff, sizeof(real_buff)); // all digits on
	update_lcd();
	
}

void show_blink_screen(void) {
	memset(&real_buff, 0, sizeof(real_buff));
	real_buff[4] = BIT(3); // "connect"
	real_buff[3] = BIT(7); // "_"
	real_buff[4] = BIT(7); // "_"
	real_buff[5] = BIT(7); // "_"
	update_lcd();
}

#endif // BOARD == BOARD_ZTH05
