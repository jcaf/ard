#ifndef MENU_STACKINDETAIL_E_H_
#define MENU_STACKINDETAIL_E_H_

    struct _stackInDetail_E
    {
        int8_t sm0;
        int8_t sm1;
    };
    extern struct _stackInDetail_E stackInDetail_E;

void mBkground_stackInDetail_E_job(void);
void mBkground_stackInDetail_E_UnderVolt_job(void);
void mBkground_stackInDetail_E_OverVolt_job(void);
void mBkground_stackInDetail_E_OpenWire_job(void);

#endif // MENU_STACKINDETAIL_H_

