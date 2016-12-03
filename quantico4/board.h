#ifndef BOARD_H_
#define BOARD_H_

    #define NUM_BOARD 4//4// MAINBOARD+ # STACKS
    #define NUM_STACKS_IN_BOARD 2

    #define StackLower 0
    #define StackUpper 1

    #define StackLowerUpper_NO_UnitedByWire 0
    #define StackLowerUpper_UnitedByWire 1
    #define StackLowerUpper_UnitedByWire_but_LowerNoExist 2


    struct _boardD
    {
        uint8_t AchangeInBoard;
        uint8_t set_text;
    };//daemon


    struct _board
    {
        struct
        {
            uint8_t address;
            uint8_t ltc_connected_last;

            //uint8_t union2up;
            //uint8_t union2down;
        } stack[NUM_STACKS_IN_BOARD]; //0=lower 1=upper

        struct
        {
            int8_t num_stack;//low_upp; //contador de stack formados
            int8_t num_max_stacks;//limit_sup;//# stacks formados
        } stacks;

        int8_t unionMode;
        int8_t unionMode_last;

    };
    extern int8_t num_board;
    extern struct _boardD boardD;
    extern struct _board board[NUM_BOARD];
    void BOARDd(void);
    //inline
    uint8_t BOARD_get_num_ic_currentboard(void);
#endif // BOARD_H_

