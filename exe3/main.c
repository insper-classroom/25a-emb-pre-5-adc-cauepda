#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sendToUART(int value) {
    printf("%d\n", value);
}

void process_task(void *p) {
    int data = 0;
    int len_mean = 5;
    double fila_dividendo[len_mean];
    int index1 = 0;
    int sum = 0;
    int index2 = 0;


    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
                if (index1 < len_mean) {
                    fila_dividendo[index1] = data;
                    sum += data;
                    index1++;

                }
                else {
                    sendToUART(sum / len_mean);
                    sum -= fila_dividendo[index2];
                    sum += data;
                    fila_dividendo[index2] = data;
                    index2++;
                    if (index2 >= len_mean)
                        index2 = 0;
                }
            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}