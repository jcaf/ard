#include <Arduino.h>
#include "vscroll.h"
#include "lcd_fxaux.h"
void VScroll::set_db_dbsize(VSCROLLDT db_dbsize)
{
    db.dbsize   = db_dbsize;
}
void VScroll::set_dispShowBuff_numline(VSCROLLDT  dispShowBuff_numline)
{
    vscroll_dispShowBuff.numline = dispShowBuff_numline;
}

void VScroll::init(char (*db_data)[LCDAN_STR_MAX_SIZE],
                   VSCROLLDT db_dbsize,
                   char (*dispShowBuff_data)[LCDAN_STR_MAX_SIZE],
                   VSCROLLDT  dispShowBuff_numline,
                   uint8_t mark,//feature
                   VSCROLLDT base,
                   VSCROLLDT offset
                  )
{
	this->set_db(db_data);//db.data = db_data;
	db.dbsize   = db_dbsize;

	vscroll_dispShowBuff.data = dispShowBuff_data ;

	//vscroll_dispShowBuff.numline =  dispShowBuff_numline;
	if (db.dbsize <= LCD_ROW)//4
	{
		if (dispShowBuff_numline <= db.dbsize)
			vscroll_dispShowBuff.numline = dispShowBuff_numline;
		else //error-> truncar
			vscroll_dispShowBuff.numline = db.dbsize;
	}
	else
	{
		if (dispShowBuff_numline <= LCD_ROW)
			vscroll_dispShowBuff.numline = dispShowBuff_numline;
		else
			vscroll_dispShowBuff.numline = LCD_ROW;
	}

	db.base     = base;
	db.offset   = offset;
	feature.bf.mark = mark;//on off mark "<"
	dispShowBuff_filldata();

	//if (feature.bf.mark)
	//    add_mark( &vscroll_dispShowBuff.data[db.offset] );//at Offset actual
}
void VScroll::set_db(char (*db_data)[LCDAN_STR_MAX_SIZE])//Util for local-variable
{
	db.data = db_data;
}

void VScroll::feat_mark_on(void)
{
	feature.bf.mark = 1;//on off mark "<"
}

void VScroll::feat_mark_off(void)
{
	feature.bf.mark = 0;//on off mark "<"
}

//condition: arr[20] = '\0' null terminator
void VScroll::add_mark(char *arr)
{
	arr[LCD_COL - 1] = '<';
}

//condition: arr[20] = '\0' null terminator
void VScroll::del_mark(char *arr)
{
	arr[LCD_COL - 1] = ' ';
}


void VScroll::dispShowBuff_filldata(void)
{
	uint8_t i;
	VSCROLLDT db_base = db.base;
	uint8_t col;


	for (i = 0; i < vscroll_dispShowBuff.numline; i++)
	{
		col=0;
		while (db.data[db_base][col] != '\0')
		{
			vscroll_dispShowBuff.data[i][col] = db.data[db_base][col];
			col++;
		}
		vscroll_dispShowBuff.data[i][col] = '\0';

		if (++db_base == db.dbsize)
			db_base = 0x00;
	}

	if (feature.bf.mark)
		add_mark( &vscroll_dispShowBuff.data[db.offset][0]);
}


void VScroll::process_signal(void)
{
	VSCROLLDT limit_sup = vscroll_dispShowBuff.numline - 1;

	if (signal == SIGN_UP)
	{
		if (db.dbsize <= vscroll_dispShowBuff.numline)
		{
			if (feature.bf.mark)
				del_mark(&vscroll_dispShowBuff.data[db.offset][0]);

			if (--db.offset < 0 )
				db.offset = limit_sup;
		}
		else
		{
			db.offset--;
			if ( db.offset < 0)
			{
				db.offset = 0;

				if (--db.base < 0)
					db.base = db.dbsize - 1;
			}
			else
			{
				if (feature.bf.mark)
					del_mark(&vscroll_dispShowBuff.data[db.offset + 1][0]);
			}
		}
	}
	else
	{
		if (db.dbsize <= vscroll_dispShowBuff.numline)
		{
			if (feature.bf.mark)
				del_mark(&vscroll_dispShowBuff.data[db.offset][0]);

			if (++db.offset > limit_sup)
				db.offset = 0;
		}
		else
		{
			db.offset++;
			if ( db.offset > limit_sup )
			{
				db.offset = limit_sup ;

				if (++db.base >= db.dbsize)
					db.base = 0x00;
			}
			else
			{
				if (feature.bf.mark)
					del_mark(&vscroll_dispShowBuff.data[db.offset - 1][0]);
			}
		}
	}
}


VSCROLLDT VScroll::get_markPosc(void)
{
	VSCROLLDT r =  db.base + db.offset ;
	if ( r >= db.dbsize )
		r = r - db.dbsize;
	return r;
}

void VScroll::stack_push(void)
{
	vscrollst.stack[vscrollst.sp].base = db.base;       //vscrollst.base;
	vscrollst.stack[vscrollst.sp].offset = db.offset;   //vscrollst.offset;

	if (++vscrollst.sp >= VSCROLL_STACK_DEEP)
		vscrollst.sp = 0;
}
void VScroll::stack_pop(uint8_t n)//1 pop x default
{
	for (uint8_t i = 0; i < n; i++)
	{
		if (--vscrollst.sp < 0)
			vscrollst.sp = VSCROLL_STACK_DEEP;
	}

	base = vscrollst.stack[vscrollst.sp].base;
	offset = vscrollst.stack[vscrollst.sp].offset;
}
void VScroll::base_offset_reset(void)
{
	base = 0;
	offset = 0;
}

void VScroll::sign_up(void)
{
	signal = SIGN_UP;
}
void VScroll::sign_down(void)
{
	signal = SIGN_DOWN;
}
void VScroll::clear_sign(void)
{
	signal = SIGN_CLEAR;
}
void VScroll::job(void)
{
	if (signal != SIGN_CLEAR)
	{
		process_signal();
		clear_sign();
	}

	dispShowBuff_filldata();
}

//hay 2 tipos de jobs...statics y dynamics
VScroll vscroll;
char vscroll_dispShowBuff[DISP_SHOW_BUFF_LENGTH][LCDAN_STR_MAX_SIZE];


//se toma por defecto.. pero es mejor cambiarlo!
void vscroll_lcd_show_disp(uint8_t fil_ini, uint8_t fil_end)
{
    if (fil_end == 0)
    {
        fil_end = LCD_ROW_3;//x defect
    }
    else
    {
        if (fil_end > LCD_ROW_3)
            fil_end = LCD_ROW_3;
    }

    uint8_t cr=0;
    for (uint8_t r=fil_ini; r<= fil_end; r++)
    {
        lcd.setCursor(0, r);
        lcd.print(&vscroll_dispShowBuff[cr][0]);
        cr++;
    }
}

