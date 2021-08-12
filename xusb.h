#ifndef xusb_h
#define xusb_h

#include <stdint.h>
#include <libusb-1.0/libusb.h>

#define REQUEST_SENSE_LENGTH    0x12
#define CDB_MAX_LENGTH          0x10

extern const uint8_t cdb_length[256];
extern int send_mass_storage_command(libusb_device_handle *handle, uint8_t endpoint, uint8_t lun,
        uint8_t *cdb, uint8_t direction, int data_length, uint32_t *ret_tag);

#endif /* xusb_h */
