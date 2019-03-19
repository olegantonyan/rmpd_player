#include "storage/sd.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

static const char *TAG = "sdcard";

/*                   | micro sd
GPIO14 (MTMS) | CLK  | 5
GPIO15 (MTDO) | CMD  | 3
GPIO2         | D0   | 7
GPIO4         | D1   | 8
GPIO12 (MTDI) | D2   | 1
GPIO13 (MTCK) | D3   | 2
*/

bool sd_init() {
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();

  // only D2 requires internal pullup, others have external resistors, but GPIO12 cannot have it
  gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);

  // This initializes the slot without card detect (CD) and write protect (WP) signals.
  // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  // Options for mounting the filesystem.
  // If format_if_mount_failed is set to true, SD card will be partitioned and
  // formatted in case when mounting fails.
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 15,
    .allocation_unit_size = 16 * 1024
  };

  // Use settings defined above to initialize SD card and mount FAT filesystem.
  // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
  // Please check its source code and implement error recovery when developing
  // production applications.
  sdmmc_card_t* card;
  esp_err_t ret = esp_vfs_fat_sdmmc_mount(STORAGE_SD_MOUNTPOINT, &host, &slot_config, &mount_config, &card);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "failed to initialize sd card: %s", esp_err_to_name(ret));
    return false;
  }

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  /*FILE *f = fopen(STORAGE_SD_MOUNTPOINT "/hello.txt", "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return false;
  }
  char line[64];
  fgets(line, sizeof(line), f);
  fclose(f);
  // strip newline
  char* pos = strchr(line, '\n');
  if (pos) {
      *pos = '\0';
  }
  ESP_LOGI(TAG, "Read from file: '%s'", line);
*/

/*  DIR *dp = opendir(STORAGE_SD_MOUNTPOINT);
  if (dp == NULL) {
    ESP_LOGE(TAG, "error opening directory");
  } else {
    while(true) {
      struct dirent *ep = readdir(dp);
      if (!ep) {
        break;
      }
      ESP_LOGI(TAG, "%s", ep->d_name);
    }
    closedir(dp);
  }*/

  return true;
}

uint64_t sd_bytes_free() {
  FATFS *fs = NULL;
  long unsigned int fre_clust = 0;

  /* Get volume information and free clusters of drive 0 */
  int res = f_getfree("0:", &fre_clust, &fs);
  if (res != 0) {
    ESP_LOGW(TAG, "error getting free space %d", res);
    return 0;
  }

  /* Get total sectors and free sectors */
  //size_t tot_sect = (fs->n_fatent - 2) * fs->csize;
  uint64_t fre_sect = fre_clust * fs->csize;

  /* Print the free space (assuming 512 bytes/sector) */
  //printf("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect / 2, fre_sect / 2);
  return (fre_sect / 2) * 1024;
}
