#pragma once

void startSystemTasks();

void taskDcc(void *pvParameters);
void taskCan(void *pvParameters);
void taskSave(void *pvParameters);
