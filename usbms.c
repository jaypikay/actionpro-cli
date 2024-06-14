/* This file is part of actionpro-cli.
 *
 * Actionpro-cli is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#include <libusb-1.0/libusb.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbms.h"
#include "xusb.h"

#include "config.h"

static struct libusb_device_handle *handle = NULL;

int send_command(const uint8_t opcode, const uint8_t *buf,
                 const uint8_t buflen) {
  int rc;
  uint32_t expected_tag = 0;
  int size;
  uint8_t cdb[CDB_MAX_LENGTH];
  uint8_t sense[REQUEST_SENSE_LENGTH];

  memset(cdb, 0, sizeof(cdb));
  cdb[0] = 0x03; // Request Sense
  cdb[4] = REQUEST_SENSE_LENGTH;

  send_mass_storage_command(handle, 0x01, 0, cdb, LIBUSB_ENDPOINT_IN,
                            REQUEST_SENSE_LENGTH, &expected_tag);
  rc = libusb_bulk_transfer(handle, 0x81, (unsigned char *)&sense,
                            REQUEST_SENSE_LENGTH, &size, 1000);
  if (rc < 0) {
    printf("libusb_bulk_transfer failed: %s\n", libusb_error_name(rc));
    return ACTIONPRO_CMD_ERROR;
  }

  rc = libusb_bulk_transfer(handle, 0x81, (unsigned char *)&sense,
                            REQUEST_SENSE_LENGTH, &size, 1000);
  if (rc < 0) {
    printf("libusb_bulk_transfer failed: %s\n", libusb_error_name(rc));
    return ACTIONPRO_CMD_ERROR;
  }

  memset(cdb, 0, sizeof(cdb));
  cdb[0] = opcode;
  memcpy(cdb + 2, buf, buflen);

  send_mass_storage_command(handle, 0x01, 0, cdb, LIBUSB_ENDPOINT_IN,
                            REQUEST_SENSE_LENGTH, &expected_tag);
  rc = libusb_bulk_transfer(handle, 0x81, (unsigned char *)&sense,
                            REQUEST_SENSE_LENGTH, &size, 1000);
  if (rc < 0) {
    printf("libusb_bulk_transfer failed: %s\n", libusb_error_name(rc));
    return ACTIONPRO_CMD_ERROR;
  }

  return ACTIONPRO_CMD_OK;
}

int open_device() {
  int rc;

  if (handle)
    return ACTIONPRO_OK;

  rc = libusb_init(NULL);
  if (rc < 0) {
    printf("\nFailed to initialize libusb\n");
    return ACTIONPRO_ERROR;
  }

  handle = libusb_open_device_with_vid_pid(NULL, ACTION_VENDOR_ID,
                                           ACTION_PRODUCT_ID);
  if (handle == NULL) {
    fprintf(stderr, "Cannot open camera device %04X:%04X.\n", ACTION_VENDOR_ID,
            ACTION_PRODUCT_ID);
    return ACTIONPRO_NOT_FOUND;
  }

  libusb_set_configuration(handle, ACTIONPRO_USB_CONFIGURATION);
  if (libusb_kernel_driver_active(handle, ACTIONPRO_USB_INTERFACE) == 1) {
    rc = libusb_detach_kernel_driver(handle, ACTIONPRO_USB_INTERFACE);
    if (rc != 0) {
      fprintf(stderr, "Failed to detach kernel driver.\n");
      return ACTIONPRO_ERROR;
    }
  }

  rc = libusb_claim_interface(handle, ACTIONPRO_USB_INTERFACE);
  if (rc != 0) {
    fprintf(stderr, "Failed to claim usb interface.\n");
    return ACTIONPRO_ERROR;
  }

  return ACTIONPRO_OK;
}

int close_device() {
  if (handle) {
    libusb_release_interface(handle, 0);
    libusb_reset_device(handle);
    libusb_close(handle);
    handle = NULL;
  }

  libusb_exit(NULL);

  return ACTIONPRO_OK;
}
