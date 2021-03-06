/**
  ******************************************************************************
  * File Name          : log_controller.c
  * Description        : This file provides code for the logging to the SD Card
  ******************************************************************************
*/

#include "log.h"
#include "rtc.h"
#include "usart.h"

//SD card objects declarations
FATFS FS;
FIL fil;
char buffer[50];

//SD card
#define SD_BUFFER_LENGTH 500
uint8_t SD_buffer[SD_BUFFER_LENGTH];

// Open to the csv file on the SDcard that will be used for logging
void write_log_file(void){
    if(f_mount(&FS,"SD:",1) == FR_OK) {
        //flush to SD, see: http://elm-chan.org/fsw/ff/00index_e.html for more functions
        f_mount(&FS, "SD:", 1);
        f_open(&fil,"SD:BATDATA.CSV", FA_OPEN_APPEND|FA_WRITE);
        f_puts(SD_buffer,&fil);
        f_close(&fil);
        f_mount(NULL, "SD:", 1);
        SD_buffer[0] = 0; //reset buffer
    }
}

// Parse the battery data and format it for writing into the log file
void log_data(Battery *const battery){
    //Note:read the date after reading time or the clock would freeze.
    if((HAL_RTC_GetTime(&hrtc,&RTCtime,RTC_FORMAT_BIN) == HAL_OK && HAL_RTC_GetDate(&hrtc,&RTCdate,RTC_FORMAT_BIN)) == HAL_OK)
    {
        sprintf(buffer,"%02d:%02d:%02d:%04d,",
                RTCtime.Hours,
                RTCtime.Minutes,
                RTCtime.Seconds,
                RTCtime.SubSeconds);
        strcat(SD_buffer,buffer);
    }
    
    sprintf(buffer, "%i,", (int)(battery->cells[0].state_of_charge *1000.0));
    strcat(SD_buffer,buffer);
    sprintf(buffer, "%i,", (int)(battery->cells[1].state_of_charge * 1000.0));
    strcat(SD_buffer,buffer);
    sprintf(buffer, "%i,", (int)(battery->cells[2].state_of_charge*1000.0));
    strcat(SD_buffer,buffer);
    sprintf(buffer, "%i", (int)battery->cells[3].state_of_charge*1000.0);
    strcat(SD_buffer,buffer);
    strcat(SD_buffer,"\n\r");
    write_log_file();

}