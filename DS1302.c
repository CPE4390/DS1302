#include <xc.h>
#include "DS1302.h"
#include <stdio.h>

void InitDS1302(void) {
    //Set initial outputs
    CE = 0; //CE low (off)
    SCLK = 0; //Clock idles low
    //Set pins as inputs/outputs
    DAT_TRIS = 0; //Out
    SCLK_TRIS = 0; //Out
    CE_TRIS = 0; //Out
}

unsigned char ReadClockRegister(unsigned char address) {
    unsigned char byte = 0;
    int i;
    unsigned char bitFlag = 1;
    address |= 0b00000001; //Set bit 0 for read
    CE = 1;
    __delay_us(4); //CE to SCLK set up time is 4us
    //Write the command byte
    for (i = 0; i < 8; ++i) {
        DAT = address;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        address >>= 1;
    }
    //Read a byte
    DAT_TRIS = 1; //change DAT to input during read
    for (i = 0; i < 8; ++i) {
        __delay_us(1);
        if (DAT) {
            byte |= bitFlag;
        }
        bitFlag <<= 1;
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
    }
    DAT_TRIS = 0; //change DAT back to output
    CE = 0;
    return byte;
}

void WriteClockRegister(unsigned char address, unsigned char data) {
    int i;
    address &= 0b11111110;  //Clear low bit for write
    CE = 1;
    __delay_us(4); //CE to SCLK set up time is 4us
    //Write the command byte 
    for (i = 0; i < 8; ++i) {
        DAT = address;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        address >>= 1;
    }
    //Write the data
    for (i = 0; i < 8; ++i) {
        DAT = data;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        data >>= 1;
    }
    __delay_us(1);
    CE = 0;
}

void ReadClockBurst(unsigned char *buffer) {
    unsigned char command = 0xbf;
    unsigned int i;
    unsigned int bytes;
    unsigned char bitFlag = 1;
    CE = 1;
    __delay_us(4); //CE to SCLK set up time is 4us
    //Write the command byte
    for (i = 0; i < 8; ++i) {
        DAT = command;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        command >>= 1;
    }
    //Read 7 bytes
    DAT_TRIS = 1; //change DAT to input during read
    for (bytes = 0; bytes < 8; ++bytes) {
        buffer[bytes] = 0;
        bitFlag = 1;
        for (i = 0; i < 8; ++i) {
            __delay_us(1);
            if (DAT) {
                buffer[bytes] |= bitFlag;
            }
            bitFlag <<= 1;
            SCLK = 1;
            __delay_us(1);
            SCLK = 0;
        }
    }
    DAT_TRIS = 0; //change DAT back to output
    CE = 0;
}

void WriteClockBurst(unsigned char *buffer) {
    int i;
    unsigned char b;
    unsigned char command = 0xbe;
    unsigned int bytes;

    CE = 1;
    __delay_us(4); //CE to SCLK set up time is 4us
    //Write the command byte 
    for (i = 0; i < 8; ++i) {
        DAT = command;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        command >>= 1;
    }
    //Write the data
    for (bytes = 0; bytes < 8; ++bytes) {
        b = buffer[bytes];
        for (i = 0; i < 8; ++i) {
            DAT = b;
            __delay_us(1);
            SCLK = 1;
            __delay_us(1);
            SCLK = 0;
            b >>= 1;
        }
    }
    __delay_us(1);
    CE = 0;
}

void ReadRamBurst(unsigned char *buffer, char len) {
    unsigned char command = 0xff;
    unsigned int i;
    unsigned int bytes;
    unsigned char bitFlag = 1;
    CE = 1;
    __delay_us(4); //CE to SCLK set up time is 4us
    //Write the command byte
    for (i = 0; i < 8; ++i) {
        DAT = command;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        command >>= 1;
    }
    //Read len bytes
    DAT_TRIS = 1; //change DAT to input during read
    for (bytes = 0; bytes < len; ++bytes) {
        buffer[bytes] = 0;
        bitFlag = 1;
        for (i = 0; i < 8; ++i) {
            __delay_us(1);
            if (DAT) {
                buffer[bytes] |= bitFlag;
            }
            bitFlag <<= 1;
            SCLK = 1;
            __delay_us(1);
            SCLK = 0;
        }
    }
    DAT_TRIS = 0; //change DAT back to output
    CE = 0;
}

void WriteRamBurst(unsigned char *buffer, char len) {
        int i;
    unsigned char b;
    unsigned char command = 0xfe;
    unsigned int bytes;

    CE = 1;
    __delay_us(4); //CE to SCLK set up time is 4us
    //Write the command byte 
    for (i = 0; i < 8; ++i) {
        DAT = command;
        __delay_us(1);
        SCLK = 1;
        __delay_us(1);
        SCLK = 0;
        command >>= 1;
    }
    //Write the data
    for (bytes = 0; bytes < len; ++bytes) {
        b = buffer[bytes];
        for (i = 0; i < 8; ++i) {
            DAT = b;
            __delay_us(1);
            SCLK = 1;
            __delay_us(1);
            SCLK = 0;
            b >>= 1;
        }
    }
    __delay_us(1);
    CE = 0;
}

void ReadClock(DateTime *data) {
    ReadClockBurst(data->bytes);
}

void SetClock(DateTime *data) {
    WriteClockBurst(data->bytes);
}

void WriteProtect(bool wp) {
    if (wp) {
        WriteClockRegister(0x8e, 0x80);
    } else {
        WriteClockRegister(0x8e, 0x00);
    }
}

void FormatDate(DateTime date, char *str, bool useDOW) {
    const char *dow_str[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; 
    if (useDOW) {
        sprintf(str, "%d%d/%d%d/%d%d %s", date.month10, date.month, date.date10, date.date, date.year10, date.year, dow_str[date.day - 1]);
    } else {
        sprintf(str, "%d%d/%d%d/%d%d", date.month10, date.month, date.date10, date.date, date.year10, date.year);
    }
}

void FormatTime(DateTime time, char *str) {
    if (time._12_24) {
        if (time.AM_PM == 0) {
            sprintf(str, "%d%d:%d%d:%d%d am", time.hour10, time.hour, time.minutes10, time.minutes, time.seconds10, time.seconds);
        } else {
            sprintf(str, "%d%d:%d%d:%d%d pm", time.hour10, time.hour, time.minutes10, time.minutes, time.seconds10, time.seconds);
        }
    } else {
        sprintf(str, "%d%d:%d%d:%d%d", time.hour10 + (time.AM_PM * 2), time.hour, time.minutes10, time.minutes, time.seconds10, time.seconds);
    }
}

void FillTime(DateTime *dt, int hour, int minutes, int seconds, bool pm, bool _12hour) {

    if (_12hour) {
        dt->hour10 = hour / 10;
        dt->_12_24 = 1;
        if (pm) {
            dt->AM_PM = 1;
        } else {
            dt->AM_PM = 0;
        }
    } else {
        dt->hour10 = hour / 10;
        dt->AM_PM = (hour / 10) >> 1;
        dt->_12_24 = 0;
    }
    dt->hour = hour % 10;
    dt->minutes10 = minutes / 10;
    dt->minutes = minutes % 10;
    dt->seconds10 = seconds / 10;
    dt->seconds = seconds % 10;
}

void FillDate(DateTime *dt, int month, int date, int year, int dayOfWeek) {
    dt->month10 = month / 10;
    dt->month = month % 10;
    dt->date10 = date / 10;
    dt->date = date % 10;
    dt->year10 = year / 10;
    dt->year = year % 10;
    dt->day = dayOfWeek;
}