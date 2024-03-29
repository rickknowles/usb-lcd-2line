/* Name: set-lcd.c
 * Project: Send text to an LCD display
 * Author: Rick Knowles
 * Creation Date: 2011-10-07
 * Tabsize: 4
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id: set-led.c 692 2008-11-07 15:07:40Z cs $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>        /* this is libusb */
#include "opendevice.h" /* common code moved to separate module */

#include "../firmware/requests.h"   /* custom request numbers */
#include "../firmware/usbconfig.h"  /* device's VID/PID and names */

static void usage(char *name)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s .... Clear LCD\n", name);
    fprintf(stderr, "  %s <text> .... Set text on LCD\n", name);
}
const char SPACE = ' ';

static int getSizeOfDisplayArgs(int argc, char** argv) {
    int n, size = 0;
    for (n = 1; n < argc; n++) {
        size += (strlen(argv[n]) + 1);
    }
    return size;
}

static void buildConcatenatedDisplayArgString(char* output, int argc, char** argv) {
    int n;
    strcpy(output, argv[1]);
    for (n = 2; n < argc; n++) {
        strcat(output, &SPACE);
        strcat(output, argv[n]);
    }
}

int main(int argc, char **argv)
{
usb_dev_handle      *handle = NULL;
const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
char                vendor[] = {USB_CFG_VENDOR_NAME, 0};
char                product[] = {USB_CFG_DEVICE_NAME, 0};
int                 cnt, vid, pid;

    usb_init();
    if(argc < 1){   /* we need at least one argument */
        usage("set-lcd");
        exit(1);
    } else if (argc >= 2 && (strcmp(argv[1], "-h") == 0)) {
        usage(argv[0]);
        exit(0);
    }
    /* compute VID/PID from usbconfig.h so that there is a central source of information */
    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    /* The following function is in opendevice.c: */
    if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0){
        fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
        exit(1);
    }
    /* Since we use only control endpoint 0, we don't need to choose a
     * configuration and interface. Reading device descriptor and setting a
     * configuration and interface is done through endpoint 0 after all.
     * However, newer versions of Linux require that we claim an interface
     * even for endpoint 0. Enable the following code if your operating system
     * needs it: */
#if 0
    int retries = 1, usbConfiguration = 1, usbInterface = 0;
    if(usb_set_configuration(handle, usbConfiguration) && showWarnings){
        fprintf(stderr, "Warning: could not set configuration: %s\n", usb_strerror());
    }
    /* now try to claim the interface and detach the kernel HID driver on
     * Linux and other operating systems which support the call. */
    while((len = usb_claim_interface(handle, usbInterface)) != 0 && retries-- > 0){
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
        if(usb_detach_kernel_driver_np(handle, 0) < 0 && showWarnings){
            fprintf(stderr, "Warning: could not detach kernel driver: %s\n", usb_strerror());
        }
#endif
    }
#endif

    if(argc > 1) {
        int size = getSizeOfDisplayArgs(argc, argv);
        char* allargs = malloc(size);
        buildConcatenatedDisplayArgString(allargs, argc, argv);
        fprintf(stderr, "Setting LCD text to: %s\n", allargs);
        cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, CUSTOM_RQ_SHOW_MSG, size - 1, 0, allargs, size - 1, 5000);
        free(allargs);
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
        }
    }else{
        fprintf(stderr, "Clearing LCD\n");
        cnt = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT, CUSTOM_RQ_CLEAR, 0, 0, NULL, 0, 5000);
        if(cnt < 0){
            fprintf(stderr, "USB error: %s\n", usb_strerror());
        }
    }
    usb_close(handle);
    return 0;
}
