#ifndef ETHERNET_H_
#define ETHERNET_H_

    uint8_t ethernet_init(void);
    uint8_t MAC_String2HexArray(String setting_value, uint8_t *MAC);
    uint8_t ethernet_uploadData_job(void);
#endif // ETHERNET_H_

