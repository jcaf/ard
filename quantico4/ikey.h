#ifndef IKEY_H_
#define IKEY_H_

    //#include "system.h"

    #define KB_NUM_KEYS 5

    typedef uint8_t (*PTRFX_retUINT8_T)(void);
    typedef void (*PTRFX_retVOID)(void);

    struct _key
    {
        PTRFX_retUINT8_T keyRead;

        PTRFX_retVOID keyDo;

        uint8_t sm0;        //maquina de estados
        uint8_t counter0;//contador

        uint8_t sm1;
        uint16_t counter1;

        uint8_t  num_group_x;

        struct _bf
        {
            unsigned state:1;

            unsigned OnKeyPressed:1;
            unsigned OnKeyPressedAndKeyRelease:1;
            unsigned AtTimeExpired:1;
            unsigned Reptt:1;

            unsigned ReadyToRead:1;		//soft-populate
            unsigned AtTimeExpired_BeforeOrAfter:1;//para usar a nivel de app
            unsigned OwnerOfGroup:1;
            unsigned InProcessing:1;

        } bf;

        struct _repp
        {
            uint16_t to_start;
            uint8_t after_start;
        } repp;

    };



    void key_initialization(void);

    void key_scan(void);
    void key_parsing(void);


    //fx expuestas x app
    uint8_t kb_get_ready2read(uint8_t k);
    void    kb_set_ready2read(uint8_t k);
    void    kb_clear_ready2read(uint8_t k);
    void    kb_execfunct(uint8_t k);

    uint8_t AllKeysReleased(void);//a nivel de hardware...
    uint8_t AllKeysCompletedProcessing(void);

    extern volatile _key key[];


#endif
