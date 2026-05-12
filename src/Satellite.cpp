

#include "Satellite.h"

Satellite::Satellite(){};

uint16_t Satellite::id()
{
    return m_idNode;
}
void Satellite::id(uint16_t id)
{
    this->m_idNode = id;
}

void Satellite::begin()
{
  TaskHandle_t task1Handle = NULL;
  xTaskCreate( watchDog, "watchDog",  3 * 1024, NULL, 2, &task1Handle );
}

//--- Task1 : Envoi à tous les sat l'ordre de sauvegarde sur flash des settings
void Satellite::watchDog(void *pvParameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  uint32_t tempo = 100UL;

  for (;;)
  {
    
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(tempo)); // toutes les x ms
  }
}
