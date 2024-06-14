#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "usbms.h"

#include "config.h"

#define VERSION "1.0.0"

static int setpassword(const char *password) {
  unsigned long slen = strlen(password);
  if (slen < 2 || slen > 12) {
    fprintf(
        stderr,
        "Password has to be at least 2 but no more than 12 characters long.\n");
    return 1;
  }

  for (int i = 0; i < slen; i++) {
    switch (password[i]) {
    case '/':
    case ':':
    case '@':
    case '[':
    case '`':
    case '{':
      fprintf(stderr,
              "Use of invalid character in password. Do not use `%c'.\n",
              password[i]);
      fprintf(stderr, "Invalid character for passwords are \"/:@[`{\".\n");
      return 1;
    }
  }
  printf("Updating password to `%s'... ", password);
  if (send_command(ACTIONPRO_OPCODE_SETPASSWORD, (const uint8_t *)password,
                   slen) == ACTIONPRO_CMD_OK)
    printf("OK\n");
  else
    printf("ERROR\n");

  return 0;
}

static int setssid(const char *ssid) {
  unsigned long slen = strlen(ssid);
  if (slen < 2 || slen > 12) {
    fprintf(stderr,
            "SSID has to be at least 2 but no more than 12 characters long.\n");
    return 1;
  }

  for (int i = 0; i < slen; i++) {
    switch (ssid[i]) {
    case ' ':
    case '~':
      fprintf(stderr, "Use of invalid character in SSID. Do not use `%c'.\n",
              ssid[i]);
      fprintf(stderr, "Invalid character for SSID are \" ~\".\n");
      return 1;
    }
  }
  printf("Updating SSID to `%s'... ", ssid);
  if (send_command(ACTIONPRO_OPCODE_SETSSID, (const uint8_t *)ssid, slen) ==
      ACTIONPRO_CMD_OK)
    printf("OK\n");
  else
    printf("ERROR\n");

  return 0;
}

static int settime() {
  printf("Setting device time... ");

  time_t curtime;
  struct tm *timeinfo;

  if (time(&curtime)) {
    timeinfo = localtime(&curtime);
    if (timeinfo) {
      uint8_t newtime[8] = {0};

      const uint16_t year =
          (timeinfo->tm_year + 1900) >> 8 | (timeinfo->tm_year + 1900) << 8;
      const uint8_t month = timeinfo->tm_mon + 1;

      memcpy(newtime + 0, &year, 2);
      memcpy(newtime + 2, &month, 1);
      memcpy(newtime + 3, &timeinfo->tm_mday, 1);
      memcpy(newtime + 4, &timeinfo->tm_hour, 1);
      memcpy(newtime + 5, &timeinfo->tm_min, 1);
      memcpy(newtime + 6, &timeinfo->tm_sec, 1);

      if (send_command(ACTIONPRO_OPCODE_SETTIME, newtime, 8) ==
          ACTIONPRO_CMD_OK)
        printf("OK\n");
      else
        printf("ERROR\n");

      return 0;
    }
  }

  return 1;
}

int main(int argc, char *argv[]) {
  int c;

  bool show_help = false;
  char *new_password = NULL;
  char *new_ssid = NULL;
  bool sync_time = false;
  bool show_version = false;

  printf("ACTIONPRO configuration utility\n");

  while (1) {
    static struct option long_options[] = {
        {"help", no_argument, 0, 'p'},
        {"password", required_argument, 0, 'p'},
        {"ssid", required_argument, 0, 's'},
        {"time", no_argument, 0, 't'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}};

    int option_index = 0;

    c = getopt_long(argc, argv, "hp:s:tv", long_options, &option_index);

    if (argc < 2) {
      fprintf(stderr, "At least one option is required. See -h, --help for "
                      "instructions.\n");
      exit(EXIT_FAILURE);
    }

    if (c == -1)
      break;

    switch (c) {
    case 'h':
      show_help = true;
      break;
    case 'p':
      new_password = optarg;
      break;
    case 's':
      new_ssid = optarg;
      break;
    case 't':
      sync_time = true;
      break;
    case 'v':
      show_version = true;
      break;
    case '?':
      fprintf(stderr, "Use option -h, --help for instructions.\n");
      exit(EXIT_FAILURE);
    default:
      abort();
    }
  }

  if (show_help) {
    printf("Usage: %s [OPTION]\n", argv[0]);
    printf("  -h, --help                 give this help list\n");
    printf("  -p, --password=PASSWORD    sets the access point authentication "
           "PASSWORD\n");
    printf("  -s, --ssid=SSID            sets the access point SSID\n");
    printf("  -t, --time                 synchronize the camera time\n");
    printf("  -v, --version              display version number\n");
    printf("\nThis program requires write access to usb devices and might be "
           "run as root.\n");

    exit(EXIT_SUCCESS);
  }

  if (show_version) {
    printf("Actionpro-cli version: %s\n", VERSION);
    exit(EXIT_SUCCESS);
  }

  if (open_device() == ACTIONPRO_OK) {
    if (new_password)
      setpassword(new_password);

    if (new_ssid)
      setssid(new_ssid);

    if (sync_time)
      settime();
  }
  close_device();

  exit(EXIT_SUCCESS);
}
