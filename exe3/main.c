#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "data.h"

QueueHandle_t xQueueData;

// Não mexer! Alimenta a fila com os dados do sinal
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

void process_task(void *p) {
    int data = 0;
    const int len_mean = 5;
    double fila_dividendo[len_mean];
    int index1 = 0;
    int sum = 0;
    int index2 = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100))) {
            // Implementar filtro: inicialmente acumula os dados
            if (index1 < len_mean) {
                fila_dividendo[index1] = data;
                sum += data;
                index1++;
                printf("%d\n", sum / len_mean);
            }
            else {
                printf("%d\n", sum / len_mean);
                // Atualiza a média com o novo valor, descartando o valor mais antigo
                sum -= fila_dividendo[index2];
                sum += data;
                fila_dividendo[index2] = data;
                index2++;
                if (index2 >= len_mean)
                    index2 = 0;
            }
            // Deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task", 256, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}