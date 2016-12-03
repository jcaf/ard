#ifndef SDCARD_H_
#define SDCARD_H_

#define SDCARD_STR_VAR_PROPERTIES_MAX_SIZE 30
//#define SDCARD_STR_VAR_PROPERTIES_MAX_SIZE SDCARD_STR_VAR_PROPERTIES_MAX_SIZE//30
//#define SDCARD_STR_VAR_PROPERTIES_MAX_SIZE SDCARD_STR_VAR_PROPERTIES_MAX_SIZE//50

#define SDCARD_STR_NAME_FILE_MAX_SIZE (8+1+3+1) //fat format 8.3 + \0
#define SDCARD_STR_PATH_DIR_MAX_SIZE (10)

struct _variable
{
    struct _bf
    {
        unsigned int sintax:1;
        //unsigned int savevalue:1;
        unsigned int lcdprint:1;
        unsigned int __u:6;
    } bf;
};

struct _sdcard
{
    struct _lowlevel
    {
        uint8_t error;

    } lowlevel;

    struct _card
    {
        uint8_t errorCode;
    } card;

    struct _volume
    {
        //uint8_t errorCode;
        uint8_t fatType;
    } volume;

    uint8_t begin;
};
extern struct _sdcard sdcard;


uint8_t SD_get_setting_value_PSTR(const char *filepath, const char *setting_name_toread, char *setting_value_dest);
#endif // SDCARD_H_


