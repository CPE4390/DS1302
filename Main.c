#include <xc.h>
#include "LCD.h"
#include "DS1302.h"

#pragma config FOSC=HSPLL
#pragma config WDTEN=OFF
#pragma config XINST=OFF


void InitPins(void);
void ConfigInterrupts(void);
unsigned int ReadPot(void);
int GetValue(const char *prompt, int min, int max);

char str[17];
DateTime dt;
volatile char state;

//TODO Test read/write register functions
//TODO Test ram burst functions

void main(void) {
    state = 0;
    int lastSeconds = -1;

    OSCTUNEbits.PLLEN = 1;
    InitDS1302();
    LCDInit();
    lprintf(0, "DS1302 Demo");
    InitPins();
    ConfigInterrupts();
    while (1) {
        if (state == 1) {
           
            int hours, minutes, dow, date, month, year;
            bool pm, _12_24;
            _12_24 = GetValue("1=12hr 0=24hr", 0, 1);
            if (_12_24) {
                hours = GetValue("Hours:", 1, 12);
                pm = GetValue("1=pm 0=am", 0, 1);
            } else {
                hours = GetValue("Hours:", 0, 23);
            }
            minutes = GetValue("Minutes", 0, 59);
            dow = GetValue("DOW: 1=Sunday", 1, 7);
            month = GetValue("Month:", 1, 12);
            date = GetValue("Day:", 1, 31);
            year = GetValue("Year:", 0, 99);
            FillTime(&dt, hours, minutes, 0, pm, _12_24);
            FillDate(&dt, month, date, year, dow);
            dt.CH = 0; //Turn clock on
            dt.wp = 0;
            if (GetValue("1=set 0=cancel", 0, 1)) {
                WriteProtect(false);
                SetClock(&dt);
                WriteProtect(true);
            }
            state = 0;
        } else {
            ReadClock(&dt);
            //Check for seconds change
            if (dt.seconds != lastSeconds) {
                lastSeconds = dt.seconds;
                FormatTime(dt, str);
                lprintf(0, str);
                FormatDate(dt, str, true);
                lprintf(1, str);
            }
            __delay_ms(1);
        }
    }
}

void InitPins(void) {
    LATD = 0; //Turn off all LED's
    TRISD = 0; //LED's are outputs
    //Set TRIS bits for any required peripherals here.
    TRISB = 0b00000001; //Button0 is input;
    INTCON2bits.RBPU = 0; //enable weak pullups on port B
    //Set up for ADC
    TRISA = 0b00000001;
    ADCON1 = 0b10111010; //Right justify, No calibration, 20 Tad, FOSC/32
    WDTCONbits.ADSHR = 1; //Switch to alternate address to access ANCON0
    ANCON0 = 0b11111110; //AN0 analog - all others digital
    WDTCONbits.ADSHR = 0; //Back to default address
}

void ConfigInterrupts(void) {

    RCONbits.IPEN = 0; //no priorities.  This is the default.

    //Configure your interrupts here

    //set up INT0 to interrupt on falling edge
    INTCON2bits.INTEDG0 = 0; //interrupt on falling edge
    INTCONbits.INT0IE = 1; //Enable the interrupt
    //note that we don't need to set the priority because we disabled priorities (and INT0 is ALWAYS high priority when priorities are enabled.)
    INTCONbits.INT0IF = 0; //Always clear the flag before enabling interrupts


    INTCONbits.GIE = 1; //Turn on interrupts
}

void __interrupt(high_priority) HighIsr(void) {
    //Check the source of the interrupt
    if (INTCONbits.INT0IF == 1) {
        //Set clock
        state = 1;
        INTCONbits.INT0IF = 0; //must clear the flag to avoid recursive interrupts
    }
}

unsigned int ReadPot(void) {
    ADCON0bits.CHS = 0; //channel 0
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO == 1);
    ADCON0bits.ADON = 0;
    return ADRES;
}

int GetValue(const char *prompt, int min, int max) {
    long knob;
    while (PORTBbits.RB0 == 0);
    __delay_ms(10);
    lprintf(0, prompt);
    while (PORTBbits.RB0 == 1) {
        knob = ReadPot();
        knob *= (max - min) + 1;
        knob /= 1024;
        knob += min;
        lprintf(1, "%d", (int) knob);
        __delay_ms(100);
    }
    __delay_ms(10);
    while (PORTBbits.RB0 == 0);
    __delay_ms(10);
    return (int) knob;
}