#ifndef DATE_H_
#define DATE_H_
//void set_date_job(void);
void mainMenu_setDate_job(void);

struct _taskman_setDate
{
    int8_t sm0;
    int8_t sign;
    int8_t reInitJob;

};
extern struct _taskman_setDate taskman_setDate;

struct _menuSetDate
{
    int8_t sm0;
};
struct _menuSetDate_save
{
    int8_t sm0;
};

#endif // DATE_H_
