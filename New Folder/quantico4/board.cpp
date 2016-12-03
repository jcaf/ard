#include <Arduino.h>
#include "board.h"

#include "LTC68042.h"
#include "ltc6804.h"

int8_t num_board;

struct _board board[NUM_BOARD];
struct _boardD boardD;

static uint8_t BOARDd_SetUnionMode(uint8_t num_board);

void BOARDd(void)
{
    static int8_t sm0;
    uint8_t address = 0;

    if ( sm0 == 0 )
    {
        for (uint8_t num_board = 0; num_board < NUM_BOARD; num_board++)
        {
            board[num_board].stack[StackLower].address = address++;
            board[num_board].stack[StackUpper].address = address++;
            //
            board[num_board].stack[StackLower].ltc_connected_last = ltc_connected [ board[num_board].stack[StackLower].address ];
            board[num_board].stack[StackUpper].ltc_connected_last = ltc_connected [ board[num_board].stack[StackUpper].address ];

            board[num_board].stacks.num_max_stacks = BOARDd_SetUnionMode(num_board);
            board[num_board].unionMode_last = board[num_board].unionMode;
        }
        boardD.AchangeInBoard = 1;//Provoca un cambio por unica vez para q se actualice la pantalla

        sm0 = -1;
    }

    //1.- Stack Lower+Upper ?
    board[num_board].stacks.num_max_stacks = BOARDd_SetUnionMode(num_board);

    //if unionMode changed?
    if ( board[num_board].unionMode_last != board[num_board].unionMode)
    {
        board[num_board].unionMode_last = board[num_board].unionMode;
        boardD.AchangeInBoard = 1;//Provoca un cambio
    }

    //if ltc_connected/disconnected?
    //uint8_t num_ic = board[num_board].stack[ board[num_board].stacks.num_stack ].address;
    if (board[num_board].stack[board[num_board].stacks.num_stack].ltc_connected_last !=  ltc_connected[BOARD_get_num_ic_currentboard()])
    {
        board[num_board].stack[board[num_board].stacks.num_stack].ltc_connected_last =  ltc_connected[BOARD_get_num_ic_currentboard()];
        boardD.AchangeInBoard = 1;//Provoca un cambio
    }
}

//Return StackFormados_num_max for current board.
static uint8_t BOARDd_SetUnionMode(uint8_t num_board)
{
    uint8_t StackFormados_num_max = 2;//numero de opciones por cada board: stack lower y stack upper

    if (ltc_connected[ board[num_board].stack[StackUpper].address ] == LTC_CONNECTED)
    {
        if (stack_union[ board[num_board].stack[StackUpper].address ] == 1)
        {
            if (ltc_connected[ board[num_board].stack[StackLower].address ] == LTC_CONNECTED)
            {
                StackFormados_num_max = 1;
                board[num_board].unionMode =  StackLowerUpper_UnitedByWire;
            }
            else
                board[num_board].unionMode =  StackLowerUpper_UnitedByWire_but_LowerNoExist;//trabaja a upper como si no estuviera unido a lower
        }
    }

    if (StackFormados_num_max == 2)
        board[num_board].unionMode =  StackLowerUpper_NO_UnitedByWire;

    return StackFormados_num_max;
}

//inline

uint8_t BOARD_get_num_ic_currentboard(void)//x NoUnited
{
    return board[num_board].stack[board[ num_board].stacks.num_stack ].address;
}
