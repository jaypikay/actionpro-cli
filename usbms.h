#ifndef usbms_h
#define usbms_h

#include <stdint.h>

/* device information */
#define ACTION_VENDOR_ID                0x4255
#define ACTION_PRODUCT_ID               0x1000

/* usb configuration */
#define ACTIONPRO_USB_CONFIGURATION     1
#define ACTIONPRO_USB_INTERFACE         0

/* return codes */
#define ACTIONPRO_OK                    0
#define ACTIONPRO_ERROR                 1
#define ACTIONPRO_NOT_FOUND             2

#define ACTIONPRO_CMD_OK                0
#define ACTIONPRO_CMD_ERROR             1

/* vendor specific SPC-2 commands */
#define ACTIONPRO_OPCODE_SETSSID        0xfd
#define ACTIONPRO_OPCODE_SETPASSWORD    0xfe
#define ACTIONPRO_OPCODE_SETTIME        0xff

/* exported functions */
extern int open_device();
extern int close_device();
extern int send_command(const uint8_t opcode, const char *buf, const uint8_t buflen);

#endif /* usbms_h */
