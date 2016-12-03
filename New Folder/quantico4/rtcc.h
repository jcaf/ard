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

extern struct _rtcc rtcc;

void rtc_job(void);

#endif // RTCC_H_
