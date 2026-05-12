

#include "Task.h"



void Task::begin()
{
  TaskHandle_t task1Handle = NULL;
  xTaskCreate( task1, "Task1",  3 * 1024, NULL, 2, &task1Handle );
}

//--- Task1 : Envoi à tous les sat l'ordre de sauvegarde sur flash des settings
void Task::task1(void *pvParameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  uint32_t tempo = 60UL * 1000UL;

  for (;;)
  {
    // CANMessage frameOut;
    // for (byte i = 0; i < 20; i++)
    // {
    //   frameOut.id |= 3 << 27; // Priorite 0, 1 ou 2
    //   frameOut.id |= 254 << 19;    // ID expediteur
    //   frameOut.id |= i << 11;      // ID destinataire
    //   frameOut.id |= 0xBF << 3;    // fonction
    //   frameOut.ext = true;
    //   delay(10);
    //   ACAN_ESP32::can.tryToSend (frameOut);
    // }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(tempo)); // toutes les x ms
  }
}
