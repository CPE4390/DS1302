/* 
 * File:   DS1302.h
 * Author: bmcgarvey
 *
 * Created on October 16, 2017, 2:26 PM
 */

#ifndef DS1302_H
#define	DS1302_H

#include <stdbool.h>

/*
 *Connections
 * CLK = RJ2
 * DAT = RJ1
 * RST = RJ3 
 */

#define     DAT         PORTJbits.RJ1
#define     SCLK        LATJbits.LATJ2
#define     CE          LATJbits.LATJ3
#define     DAT_TRIS    TRISJbits.TRISJ1
#define     SCLK_TRIS   TRISJbits.TRISJ2
#define     CE_TRIS     TRISJbits.TRISJ3

#define _XTAL_FREQ 32000000L

typedef union {
    struct {
        
        unsigned int seconds : 4;
        unsigned int seconds10 : 3;
        unsigned int CH : 1;
        unsigned int minutes : 4;
        unsigned int minutes10 : 3;
        unsigned int : 1;
        unsigned int hour : 4;
        unsigned int hour10 : 1;
        unsigned int AM_PM : 1;
        unsigned int : 1;
        unsigned int _12_24 : 1;
        unsigned int date : 4;
        unsigned int date10 : 2;
        unsigned int : 2;
        unsigned int month : 4;
        unsigned int month10 : 1;
        unsigned int : 3;
        unsigned int day : 3;
        unsigned int : 5;
        unsigned int year : 4;
        unsigned int year10 : 4;
        unsigned int : 7;
        unsigned int wp : 1;
    };
    unsigned char bytes[8];
} DateTime;

#ifdef	__cplusplus
extern "C" {
#endif

    //Clock functions
    void InitDS1302(void);
    unsigned char ReadClockRegister(unsigned char address);
    void WriteClockRegister(unsigned char address, unsigned char data);
    void ReadClockBurst(unsigned char *buffer);
    void WriteClockBurst(unsigned char *buffer);
    void ReadRamBurst(unsigned char *buffer, char len);
    void WriteRamBurst(unsigned char *buffer, char len);
    void ReadClock(DateTime *data);
    void SetClock(DateTime *data);
    void FormatDate(DateTime date, char *str, bool useDOW);
    void FormatTime(DateTime time, char *str);
    void FillTime(DateTime *dt, int hour, int minutes, int seconds, bool pm, bool _12hour);
    void FillDate(DateTime *dt, int month, int date, int year, int dayOfWeek);
    void WriteProtect(bool wp);

#ifdef	__cplusplus
}
#endif

#endif	/* DS1302_H */

