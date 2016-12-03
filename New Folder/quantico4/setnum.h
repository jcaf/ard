#ifndef SETNUM_H_INCLUDED
#define SETNUM_H_INCLUDED

#define BLINK_TOGGLE_BLANK 0
#define BLINK_TOGGLE_NOBLANK 1

#define DTOSTRF_SIGN_POS 1

#define PARAM_STR_LENGTH (6+1)//6+\0
#define PARAM_STR_DIG_POS_DP 3
#define PARAM_STR_DIG_POS_MAX (PARAM_STR_LENGTH-2)

#define PARAM_STR_DP_PRECISION 2

extern char PARAM0str[7+1];//="0000.00";
extern char PARAM1str[7+1];//="0000.00";
extern char PARAM2str[7+1];//="0000.00";

struct _setnum
{
    uint8_t param;

    struct _digit
    {
        uint8_t position;

        struct _blink
        {
            uint8_t sm0;

            unsigned long last_millis;

            struct _bf
            {
                unsigned toggle:1;
                unsigned __a:7;
            } bf;

        } blink;
    } digit;

    union
    {
        struct //_bf
        {
            unsigned check_range_limit:1;
            unsigned error_consistency_lenght:1;
            unsigned error_consistency_volume:1;
            unsigned error_equalzero_length:1;
            unsigned error_equalzero_volume:1;
            unsigned __a:3;
        } bf;
        uint8_t bitflag;
    };

    float n_entered;

};
extern struct _setnum setnum;

void setnum_digit_blink_set2blank(void);
void setnum_digit_blink_set2noblank(void);

#endif // SETNUM_H_INCLUDED
