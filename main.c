#include "main.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


typedef struct {
    float tan_so;   // Hz
    float chu_ky;        // percent 0..100
} thamsoLed_t;


static QueueHandle_t xLedQueue = 0;
static TaskHandle_t xBlinkTaskHandle = 0;
static TaskHandle_t xSignalTaskHandle = 0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void Error_Handler(void);

void BlinkTask(void *pvParameters);
void SignalTask(void *pvParameters);

/* ---------- main ---------- */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    xLedQueue = xQueueCreate(8, sizeof(thamsoLed_t));
    if (xLedQueue == 0) {
        Error_Handler();
    }

    if (xTaskCreate(BlinkTask, "BlinkTask", 256, 0, 2, &xBlinkTaskHandle) != pdPASS) {
        Error_Handler();
    }
    if (xTaskCreate(SignalTask, "SignalTask", 256, 0, 3, &xSignalTaskHandle) != pdPASS) {
        Error_Handler();
    }

    vTaskStartScheduler();

    while (1) { }
}

void BlinkTask(void *pvParameters)
{
    thamsoLed_t current = {1.0f, 50.0f}; 
    uint32_t period_ms, on_ms, off_ms;
    BaseType_t xResult;

   while (1) {
       
        xResult = xQueueReceive(xLedQueue, &current, 0);
        (void)xResult; 
        if (current.tan_so <= 0.001f) {
            current.tan_so = 1.0f;
        }
        if (current.chu_ky < 0.0f) current.chu_ky = 0.0f;
        if (current.chu_ky > 100.0f) current.chu_ky = 100.0f;

        period_ms = (uint32_t)(1000.0f / current.tan_so);
        on_ms = (uint32_t)((period_ms * current.chu_ky) / 100.0f);
        off_ms = (period_ms > on_ms) ? (period_ms - on_ms) : 0;

        if (on_ms == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
            vTaskDelay(pdMS_TO_TICKS(period_ms));
            continue;
        }
        if (off_ms == 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
            vTaskDelay(pdMS_TO_TICKS(period_ms));
     
            continue;
        }

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(on_ms));

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        vTaskDelay(pdMS_TO_TICKS(off_ms));
    }
}

void SignalTask(void *pvParameters)
{
    const TickType_t xDelay5s = pdMS_TO_TICKS(5000UL);
    thamsoLed_t seq[] = {
        {1.0f, 50.0f}, /* 0s..5s: 1 Hz, 50% */
        {2.0f, 70.0f}, /* 5s..10s: 2 Hz, 70% */
        {4.0f, 30.0f}, /* 10s..15s: 4 Hz, 30% */
        {1.0f, 50.0f}  /* 15s..20s: back to 1 Hz, 50% */
    };
    const size_t seq_len = sizeof(seq) / sizeof(seq[0]);
    size_t idx = 0;

    while (1) {
        xQueueSend(xLedQueue, &seq[idx], 0);

        idx = (idx + 1) % seq_len;

        vTaskDelay(xDelay5s);
    }
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) { }
}
