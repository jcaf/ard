#ifndef MENU_STACKINDETAIL_E_H_
#define MENU_STACKINDETAIL_E_H_

    struct _stackInDetail_E
    {
        int8_t sm0;
        int8_t sm1;
    };
    extern struct _stackInDetail_E stackInDetail_E;
    void stackInDetail_E_job(void);

    //
    struct _stackInDetail_E_UnderVolt
    {
        int8_t sm0;
    };
    extern struct _stackInDetail_E_UnderVolt stackInDetail_E_UnderVolt;
    void stackInDetail_E_UnderVolt_job(void);

    //
    struct _stackInDetail_E_OverVolt
    {
        int8_t sm0;
    };
    extern struct _stackInDetail_E_OverVolt stackInDetail_E_OverVolt;
    void stackInDetail_E_OverVolt_job(void);

    //
    struct _stackInDetail_E_OpenWire
    {
        int8_t sm0;
    };
    extern struct _stackInDetail_E_OpenWire stackInDetail_E_OpenWire;
    void stackInDetail_E_OpenWire_job(void);


#endif // MENU_STACKINDETAIL_H_
