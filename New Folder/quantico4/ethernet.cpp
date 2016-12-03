#include <SPI.h>
#include "system.h"
//#include <Ethernet.h>
#include "ethernet.h"
#include "boot.h"
#include "lcd_fxaux.h"

/********************************************//**
 * @brief With version 1.0, the library supports DHCP. Using Ethernet.begin(mac) with the proper network setup,
 *          the Ethernet shield will automatically obtain an IP address
 * @param void
 * @return uint8_t;
 *          The DHCP version of this function, Ethernet.begin(mac), returns an int: 1 on a successful DHCP connection, 0 on failure.
 ***********************************************/
/*
uint8_t ethernet_init(void)
{
    uint8_t coderet=0;
    uint8_t MAC[6];

    lcd.clear();
    sysPrint_lcd_serialPrintln(1,STR_CENTER,"INTERNET CONNECTION");
    delay(2000);
    lcd.clear();
    sysPrint_lcd_serialPrintln(0,STR_CENTER,"ACQUIRING IP ADDRESS");

    MAC_String2HexArray(mainvar_getvalue("MAC"), MAC);

    if(!Ethernet.begin(MAC))
    {
        sysPrint_lcd_serialPrintln(2,0,"ERROR: NO IP with DHCP");
    }
    else
    {
        coderet = 1;
        sysPrint_lcd_serialPrintln(1,STR_CENTER,"Ethernet Configured!");
        sysPrint_lcd_serialPrintln(2,STR_CENTER,"IP address is at:");
        //Ethernet.localIP()

    }
    delay(2000);

    return coderet;
}
*/
uint8_t ethernet_uploadData_job(void)
{
//    if (client.connect(server, 80))
//    {
//       if (client.available())
//       {
//       }
//   }

}


/********************************************//**
 * @brief
 *
 * @param setting_value String
 * @param MAC uint8_t*
 * @return uint8_t
 *
 ***********************************************/
uint8_t MAC_String2HexArray(String setting_value, uint8_t *MAC)/**< uint8_t MAC[6] */
{
#define arr_leng 40
#define buff_length 10
#define _buff_clear() do{for (size_t i=0;i<buff_length;i++){buff[i]=0;} }while(0)

    uint8_t parseok=0;
    char arr[arr_leng];
    char buff[buff_length];
    uint8_t ca,cb,cm;
    //uint8_t MAC[6];
    ca=cb=cm=0x00;

    setting_value.toCharArray(arr,setting_value.length()+1);
    _buff_clear();

    while (arr[ca] != '\0')
    {
        if (arr[ca]==',')
        {
            MAC[cm++] =  (uint8_t) strtoul(buff, NULL, 16);
            ca++;//1++ for skip comma
            _buff_clear();
            cb=0;
        }
        buff[cb++]=arr[ca++];
    }
    MAC[5] = (uint8_t) strtoul(buff, NULL, 16);

    parseok = 1;

    return parseok;
//    for (int i=0; i<6; i++) Serial.println(MAC[i]);
}


/********************************************//**
 * @brief
 *
 * @param address IPAddress
 * @return String
 *
 ***********************************************/
/*
String DisplayAddress(IPAddress address)
{
    return String(address[0]) + "." +
           String(address[1]) + "." +
           String(address[2]) + "." +
           String(address[3]);
}
*/
