//#include <stdio.h>
#include <stdint.h>
//using namespace std;


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define eof 0x66
//uint8_t v[50] = {1, '+', 2, '+', 3, eof};
//uint8_t v[50] = {1, '+', 2, '+', '(', 4, '+', 5, ')', eof};
//uint8_t v[50] = {1, '+', 2, '+', '(', 3, '+', 4, ')','+', 5, '+', 6, eof};
//uint8_t v[50] = {1, '+', 2, '+', '(', 3, '+', 4, '+','(', 5,'+',6, ')', '+', 7, '+', 8, ')', eof};

//uint8_t v[50] = { 1, '*', 2, '+', '(', 3, '*', 4, '+','(', 5,'*',6, ')', '-', 7, '+', 8, ')', eof};

uint8_t v[50] = { 1, '*', 2, '+', '(', 3, '*', 4, '+','(', 5,'*',6, ')', '-', '(', 7, '+', 8, ')', ')', eof};

uint8_t logical(uint8_t lval, uint8_t op, uint8_t rval)
{
    if (op == '+')
        return lval + rval;
    else if (op == '*')
        return lval * rval;
    else if (op == '-')
        return lval - rval;
}
uint8_t fr(uint8_t *p, uint8_t *counter)
{
    uint8_t sm0 = 0;
    uint8_t lval, rval, op;
    uint8_t _counter=0;
    
    while (1)
    {
        if ( *p == '(' )    //pto.1
        {
            p++;
            (*counter)++;
            
            rval = fr(p, &_counter );
                                      //p = (p+ _counter);
            sm0 = 3;
        }
        
        if (sm0 == 0)
        {
            lval = *p;
            sm0++;
            
            p++;
            (*counter)++;
        }
        if (sm0 > 0)
        {
            if ( ( *p == ')' ) || (*p == eof) ) {break;}
        
            //++- van a pto.1 para ver si hay un nuevo '('
            if (sm0 == 1)
            {
                op = *p;
                sm0++;
            }
            else if (sm0 == 2)
            {
                rval = *p;
                sm0++;
            }
            //++-
            if (sm0 == 3)//execute direct x ya fue examinado los errores correspondientes previamente
            {
                lval = logical(lval, op, rval);    
                sm0 = 1;
            }
        
            //p++;
            //(*counter)++;
            p = (p+ _counter +1);
            *counter = *counter + _counter +1;
            //            
            _counter=0;
        }
    }
    return lval;
}

void setup() 
{
    uint8_t counter=0;
    //printf("\nSUMA TOTAL IS: %u \n", fr(v, &counter) );
    uint8_t lval = fr(v, &counter);
    Serial.begin(230400);
    Serial.print(lval);
    
    //printf("COUNTER TOTAL IS: %u \n", counter );

}

void loop() 
{

}



