/* Name: main.c
 * Project: control an LCD from a USB command line
 * Author: Rick Knowles
 * Creation Date: 2011-10-06
 * Tabsize: 4
 * License: GNU GPL v2 (see License.txt)
 * (modified from v-usb example custom-class)
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "lcd.h"
#include "usbdrv.h"
#include "requests.h"

#define ROW_LENGTH  32

uchar bytesRemaining;

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (void *)data;
    if (rq->bRequest == CUSTOM_RQ_SHOW_MSG) {
        lcd_clrscr();
        bytesRemaining = rq->wLength.word;  // store the amount of data requested
        if (bytesRemaining > ROW_LENGTH) {
            // limit to buffer size
            bytesRemaining = ROW_LENGTH;
        }
        return USB_NO_MSG;
    } else if (rq->bRequest == CUSTOM_RQ_CLEAR) {
        lcd_clrscr();
        bytesRemaining = 0;
    }
    return 0;   /* default for not implemented requests: return no data back to host */
}

uchar usbFunctionWrite(uchar *data, uchar len)
{
    uchar i;
    if(len > bytesRemaining)                // if this is the last incomplete chunk
        len = bytesRemaining;               // limit to the amount we can store
    bytesRemaining -= len;
    for(i = 0; i < len; i++) {
        lcd_putc(data[i]);
    }
    return  ( bytesRemaining == 0);
}
/* ------------------------------------------------------------------------- */
int __attribute__((noreturn)) main(void)
{
    wdt_enable(WDTO_1S);

    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts("USB LCD");
    lcd_gotoxy(0,1);
    lcd_puts("Connect USB...");

    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */

    _delay_ms(250);
    wdt_reset();
    usbDeviceConnect();
    lcd_gotoxy(0,1);
    lcd_puts("Ready         ");

    sei();
    for(;;){                /* main event loop */
        wdt_reset();
        usbPoll();
    }
}

/* ------------------------------------------------------------------------- */
