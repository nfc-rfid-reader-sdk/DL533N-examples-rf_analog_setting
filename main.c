/*
 ============================================================================
 Name        : main.c
 Author      : Digital Logic
 Version     :
 Copyright   : www.d-logic.net
 Description : PN533 RF settings for typeA targets example
 ============================================================================
 */

 /*
 For Windows is used libnfc.dll compiled from branch https://github.com/nfc-tools/libnfc/tree/pn53x_transceive
 because it is used pn53x_transceive function which can not be used by libnfc.dll from master branch https://github.com/nfc-tools/libnfc/tree/master
 When compiling a dll, the debug option is disabled if uses MinGW gcc compiler, because debug uses err.h, which do not exist into MinGW.

 MinGW compiler.
 err.h header file do not exist, so you can create empty err.h header file and added this into include folder of MinGW.
 */

#include <stdio.h>

#include <nfc/nfc.h>
#include <utils/nfc-utils.h>

#define MAX_DEVICE_COUNT 16
#define MAX_TARGET_COUNT 16

const uint8_t pncmd_rf_setting_typeA[] = {0x32, 0x0A, 0x7A, 0xF4, 0x3F, 0x11, 0x4D, 0x85, 0x61, 0x6F, 0x26, 0x62, 0x87};
const uint8_t pncmd_rf_setting_typeA_default[] = {0x32, 0x0A, 0x5A, 0xF4, 0x3F, 0x11, 0x4D, 0x85, 0x61, 0x6F, 0x26, 0x62, 0x87};
uint8_t  abtRx[255];
size_t  szRx = sizeof(abtRx);

nfc_modulation nm;
nfc_device *pnd = NULL;
nfc_context *context;
nfc_target ant[MAX_TARGET_COUNT];
bool verbose = false;

void
print_nfc_target(const nfc_target *pnt, bool verbose)
{
  char *s;
  str_nfc_target(&s, pnt, verbose);
  printf("%s", s);
  nfc_free(s);
}

void main_menu(void)
{
    printf("\r\n\rMAIN MENU\n\r\n");
    printf("1 - SET THE MAXIMUM GAIN (RFCfg Register = 0x7A)\r\n");
    printf("2 - SET THE DEFAULT GAIN (RFCfg Register = 0x5A)\r\n");
    printf("3 - DETECT CARD\r\n");
    printf("4 - EXIT\r\n");
    printf("\r\nCHOOSE AN OPERATION (1 - 4):");
}

void rf_setting_typeA_max_gain(void)
{
    int res;
    res = pn53x_transceive(pnd, pncmd_rf_setting_typeA, sizeof(pncmd_rf_setting_typeA), abtRx, szRx, 0);
    if (res == 0) {
      printf(" RF settings OK");
    } else {
      nfc_perror(pnd, "pn53x_transceive: cannot RF settings");
      nfc_exit(context);
      exit(EXIT_FAILURE);
    }
}

void rf_setting_typeA_default_gain(void)
{
    int res;
    res = pn53x_transceive(pnd, pncmd_rf_setting_typeA_default, sizeof(pncmd_rf_setting_typeA_default), abtRx, szRx, 0);
    if (res == 0) {
      printf(" RF settings OK");
    } else {
      nfc_perror(pnd, "pn53x_transceive: cannot RF settings");
      nfc_exit(context);
      exit(EXIT_FAILURE);
    }
}

void card_detection(void)
{
    int res;
    nm.nmt = NMT_ISO14443A;
    nm.nbr = NBR_106;

    if (nfc_initiator_init(pnd) < 0) {
      nfc_perror(pnd, "nfc_initiator_init");
      nfc_exit(context);
      exit(EXIT_FAILURE);
    }
    // List ISO14443A targets
    if ((res = nfc_initiator_list_passive_targets(pnd, nm, ant, MAX_TARGET_COUNT)) >= 0) {
      int n;
      if (verbose || (res > 0)) {
        printf("%d ISO14443A passive target(s) found%s\n", res, (res == 0) ? ".\n" : ":");
        for (n = 0; n < res; n++) {
          print_nfc_target(&ant[n], verbose);
          printf("\n");
        }
      }
      else
         printf("no ISO14443A passive target found\n");
    }
}

int
main()
{
  const char *acLibnfcVersion;
  char menu_choice;
  char menu_str[3];
  char *tp;

  nfc_init(&context);
  if (context == NULL) {
    printf("Unable to init libnfc (malloc)");
    exit(EXIT_FAILURE);
  }

  // Display libnfc version
  acLibnfcVersion = nfc_version();
  printf("uses libnfc %s\n", acLibnfcVersion);

  nfc_connstring connstrings[MAX_DEVICE_COUNT];
  size_t szFound = nfc_list_devices(context, connstrings, MAX_DEVICE_COUNT);

  if (szFound == 0) {
    printf("No NFC device found.\n");
  }

   pnd = nfc_open(context, connstrings[0]);

   if (pnd == NULL) {
      printf("%s", "Unable to open NFC device.");
      nfc_exit(context);
      exit(EXIT_FAILURE);
    }
   printf("NFC device: %s opened\n", nfc_device_get_name(pnd));


    while(1) {
      main_menu();
      scanf("%s", menu_str);
      menu_choice = strtoul(menu_str, &tp, 10);

      switch(menu_choice) {
        case 1:
          rf_setting_typeA_max_gain();
          break;
        case 2:
          rf_setting_typeA_default_gain();
          break;
        case 3:
          card_detection();
          break;
        case 4:
          nfc_close(pnd);
          nfc_exit(context);
          exit(EXIT_SUCCESS);
          break;
      }
    }
}
