#include <LiquidCrystal.h>
#include "system.h"
#include "LTC68042.h"
#include "ltc6804.h"
#include "ikey.h"
#include "kb.h"
#include "lcd_bklight.h"
#include "lcd_fxaux.h"
#include "vscroll.h"

#include "rele.h"
#include "digital_input.h"
#include "analog_input.h"
#include "leds.h"

#include "errorsys.h"
#include "boot.h"

#include "ethernet.h"
#include "DS3231.h"
#include "I2C.h"

#include "time.h"
#include "date.h"
#include "rtcc.h"
#include "fuelsensor.h"
#include "fuelsensor_jt606x.h"

#include "menu.h"
#include "menu_mainMenu.h"
#include "menu_bkground.h"
#include <SPI.h>
#include "sdcard.h"

mylcd lcd(22, 23, 24, 25, 26, 27);//LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

//void IP_String2int(String serverIP, uint8_t *IP);

void setup()
{
unsigned long last_millis = millis();

//Serial.begin(230400);
//uint8_t IP[4];
//IP_String2int(mainvar_getvalue("Server1IP"), IP);
//while (1)

	fuelsensor_inhibit_comm();

    lcd.begin(LCD_COL, LCD_ROW);
    lcd.setCursor(0,0);
    digitalWrite(LCD_PIN_BACKLIGHT,LOW);
    pinMode(LCD_PIN_BACKLIGHT,OUTPUT);
	lcd_bklight_ON();//lcd_bklight_OFF();

    Init_I2C_unimaster();
    InitDS3231();

    SPI_deselectAllperipherals();
	SPI.begin();

	if (sdcard_init())
	{
		if (system_set_functionmode())
		{
			if (system.funct_mode == FUNCT_MODE_DEBUG)
			{
				Serial.begin(230400);
			}
		}
	}
	sdcard_runtest_H10();

	eth_init();

	digital_input_init();
	out_leds_init();
	rele_init();

	fuelsensor_init();//load param from EEPROM
	jt606_init(last_millis);
	fuelsensor_set_comm();

	vscroll.base_offset_reset();

	while (!ltc6804_job());

	InitTCNT1();
    key_initialization();
    TIMSK1 = (1<<OCIE1A);
}

void loop()
{
static uint8_t sm0;
 	rtc_job();
    ltc6804_job();
	BOARDd();
    kb_processKeyRead_D();

    menu_detect_changeMenu_changeSubMenu();

	//////////////////////////////////////////////////
	if (menu_getMenu() == MGO_MBKGROUND)//0
	{
		board_event();
		menu_v[MGO_MBKGROUND][submen]();
	}
	else//menu_getMenu() = MGO_MAIN_MENU(-1) or [1,2..8]
	{
		if (menu_getMenu() == MGO_MAIN_MENU)//-1
			mainMenuSummary_job();
		else	//[1,2..8]
			menu_v[men][submen]();
	}
	//////////////////////////////////////////////////
	if(boardD.AchangeInBoard == 1)
        boardD.AchangeInBoard = 0;
	//////////////////////////////////////////////////

	lcd_bklight_job();//display off/on

    analog_input_job();
    digital_input_job();
    out_leds_job();
    rele_job();
	fuelsensor_job();

	eth_manage_DHCP_IP();
    eth_scheduler_uploadData_job();
    eth_rxDataFromServer() ;

}

ISR(TIMER1_COMPA_vect)
{
    #ifdef LCD_BACKLIGHT_TIMMING
    lcd_bklight_timming();
    #endif

	kb_job();
}



