#include "app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#ifndef FLASH_BASE
#define FLASH_BASE 0x08000000U
#endif

#ifndef FLASH_SIZE
#define FLASH_SIZE 0x080000U
#endif

extern uint32_t __ipc_region_start__;

/**
 * Global variable definitions
 */
struct sensor_data_s g_sensor_data IPC_REGION;

/** Private variable */
StackType_t sensor_task_stack[APP_SENSOR_TASK_STACK_SIZE] __attribute__((aligned(APP_SENSOR_TASK_STACK_SIZE * sizeof(StackType_t))));
StackType_t comm_task_stack[APP_COMM_TASK_STACK_SIZE] __attribute__((aligned(APP_COMM_TASK_STACK_SIZE * sizeof(StackType_t))));
StackType_t supervisor_task_stack[APP_SUPERVISOR_TASK_STACK_SIZE] __attribute__((aligned(APP_SUPERVISOR_TASK_STACK_SIZE * sizeof(StackType_t))));

TaskHandle_t sensor_task_handle = NULL;
TaskHandle_t comm_task_handle = NULL;
TaskHandle_t supervisor_task_handle = NULL;

StaticTask_t sensor_task_buffer;
StaticTask_t comm_task_buffer;
StaticTask_t supervisor_task_buffer;

TaskParameters_t sensor_task_params = {
    .pvTaskCode = sensor_task,
    .pcName = "SensorTask",
    .usStackDepth = APP_SENSOR_TASK_STACK_SIZE,
    .pvParameters = NULL,
    .uxPriority = APP_SENSOR_TASK_PRIORITY,
    .puxStackBuffer = sensor_task_stack,
    .pxTaskBuffer = &sensor_task_buffer,
    .xRegions = {
        {sensor_task_stack, sizeof(sensor_task_stack) * sizeof(StackType_t), portMPU_REGION_READ_WRITE},
        {(void *)FLASH_BASE, FLASH_SIZE, portMPU_REGION_READ_ONLY},
        {(void *)0x2001F000, 0x1000, portMPU_REGION_READ_WRITE}}};

TaskParameters_t comm_task_params = {
    .pvTaskCode = comm_task,
    .pcName = "CommTask",
    .usStackDepth = APP_COMM_TASK_STACK_SIZE,
    .pvParameters = NULL,
    .uxPriority = APP_COMM_TASK_PRIORITY,
    .puxStackBuffer = comm_task_stack,
    .pxTaskBuffer = &comm_task_buffer,
    .xRegions = {
        {comm_task_stack, sizeof(comm_task_stack) * sizeof(StackType_t), portMPU_REGION_READ_WRITE},
        {(void *)FLASH_BASE, FLASH_SIZE, portMPU_REGION_READ_ONLY},
        {(void *)0x2001F000, 0x1000, portMPU_REGION_READ_WRITE}}};

TaskParameters_t supervisor_task_params = {
    .pvTaskCode = supervisor_task,
    .pcName = "SupervisorTask",
    .usStackDepth = APP_SUPERVISOR_TASK_STACK_SIZE,
    .pvParameters = NULL,
    .uxPriority = APP_SUPERVISOR_TASK_PRIORITY,
    .puxStackBuffer = supervisor_task_stack,
    .pxTaskBuffer = &supervisor_task_buffer,
    .xRegions = {
        {supervisor_task_stack, sizeof(supervisor_task_stack) * sizeof(StackType_t), portMPU_REGION_READ_WRITE},
        {(void *)FLASH_BASE, FLASH_SIZE, portMPU_REGION_READ_ONLY},
        {(void *)0x2001F000, 0x1000, portMPU_REGION_READ_WRITE}}};

/** Buffer for queue, sem ... */
static StaticSemaphore_t g_sensor_data_mutex;
static uint16_t adc_queue_buffer[10];
static StaticQueue_t g_adc_queue_buffer;

void app_init(void)
{
    HAL_Init();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();
    // MX_IWDG_Init();
    MX_TIM3_Init();
    xTaskCreateRestrictedStatic(&sensor_task_params, &sensor_task_handle);

    xTaskCreateRestrictedStatic(&comm_task_params, &comm_task_handle);

    xTaskCreateRestrictedStatic(&supervisor_task_params, &supervisor_task_handle);

    // Initialize application resources, tasks, and synchronization primitives here
    g_sensor_data.data_mutex = xSemaphoreCreateMutexStatic(&g_sensor_data_mutex);
    g_adc_queue = xQueueCreateStatic(10U, sizeof(uint16_t), (uint8_t *)adc_queue_buffer, &g_adc_queue_buffer);
}

void app_start(void)
{
    adc_start_sampling();
    // Start application tasks or processes here
    vTaskStartScheduler();
}