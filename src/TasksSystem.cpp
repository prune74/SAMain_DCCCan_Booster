#include "TasksSystem.h"
#include "DccService.h"      // taskDcc + taskCan
#include "CanService.h"
#include "pins.h"
#include "Config.h"
#include <ACAN_ESP32.h>

// Handles FreeRTOS
TaskHandle_t taskDccHandle = nullptr;
TaskHandle_t taskCanHandle = nullptr;
TaskHandle_t taskCanRxHandle = nullptr;

// ---------------------------------------------------------------------------
//  TÂCHE CAN RX (CAN Booster → CAN Service)
// ---------------------------------------------------------------------------
void taskCanRx(void *pvParameters)
{
    CANMessage msg;

    for (;;)
    {
        if (ACAN_ESP32::can.receive(msg))
        {
            // Forward vers CAN Service (MCP2515)
            CanService::send(msg);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// ---------------------------------------------------------------------------
//  Lancement des tâches système
// ---------------------------------------------------------------------------
void startSystemTasks()
{
    // Tâches DCC (dans DccService.cpp)
    xTaskCreatePinnedToCore(taskDcc,   "DCC",    4096, nullptr, 2, &taskDccHandle,   0);
    xTaskCreatePinnedToCore(taskCan,   "CAN",    4096, nullptr, 3, &taskCanHandle,   1);

    // Tâche CAN RX (dans ce fichier)
    xTaskCreatePinnedToCore(taskCanRx, "CAN_RX", 4096, nullptr, 1, &taskCanRxHandle, 0);
}
