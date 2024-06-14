# Actionpro-cli

A Command line interface for ACTIONPRO X7 to configure the Wi-Fi settings from the command line.

The ACTIONPRO X7 was produced by _CI IMAGEWEAR GmbH_ and is by now _end of life_ and now longer
supported. There was a Windows program called _Action Manager_, which allowed Windows users to
configure their action camera, or reset the credentials, if the credentials were forgotten.

This project uses parts of the [xusb.c](https://github.com/libusb/libusb/blob/master/examples/xusb.c)
example program provided by the libusb project.

This program is a result of my work on reverse engineering the _Action Manager_, the write up
of this project can be found on
[goatpr0n.farm](https://goatpr0n.farm/2021/08/reversing-an-eol-action-camera-usb-scsi-direct-access/).

## Requirements

**Libraries**:

- libusb-1.0

To access USB devices, root access is often required.

## Compiling

```
$ make config.h
$ $EDITOR config.h
$ make
```

### Configuration Options

Adjust settings in `config.h` before running make to apply changes.

The file `config.h` is created when running `make`, or by explicitly running `make config.h`.
If the file `config.h` does not exist while running make, the defaults are copied from `config.def.h`.

**Options**

- `RETRY_MAX` (_Default 5_) - Number of retries for sending a mass storage command.

## Options

```
Usage: ./actionpro [OPTION]
  -h, --help                 give this help list
  -p, --password=PASSWORD    sets the access point authentication PASSWORD
  -s, --ssid=SSID            sets the access point SSID
  -t, --time                 synchronize the camera time
  -v, --version              display version number
```

## Executing

To update the SSID to "newssid" and set the access point password of the cameras access
to "newpassword":

```
$ ./actionpro -s newssid -p newpassword
```
