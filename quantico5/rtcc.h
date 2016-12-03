#ifndef RTCC_H_
#define RTCC_H_

struct _rtcc
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t ampm;



    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;


};
#define TIME_STR_LENGTH 12
#define DATE_STR_LENGTH 9
extern char time_str[TIME_STR_LENGTH];//hh:mm:ss am0
extern char date_str[DATE_STR_LENGTH];//dd/mm/yy0

//rtc.time_str
extern struct _rtcc rtcc;

void rtc_job(void);

#endif // RTCC_H_

