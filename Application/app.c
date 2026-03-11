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

extern uint32_t __ipc_region_start__[];
extern uint32_t __ipc_region_end__[];
extern uint32_t __ipc_region_size__[];

/**
 * Global variable definitions
 */
struct sensor_data_s g_sensor_data IPC_REGION;

uint32_t g_alive IPC_REGION;

/** Private variable */
/** Task stack, need to aligne because of MPU port */
StackType_t sensor_task_stack[APP_SENSOR_TASK_STACK_SIZE] __ALIGNED(APP_SENSOR_TASK_STACK_SIZE * sizeof(StackType_t));
StackType_t comm_task_stack[APP_COMM_TASK_STACK_SIZE] __ALIGNED(APP_COMM_TASK_STACK_SIZE * sizeof(StackType_t));
StackType_t supervisor_task_stack[APP_SUPERVISOR_TASK_STACK_SIZE] __ALIGNED(APP_SUPERVISOR_TASK_STACK_SIZE * sizeof(StackType_t));
StackType_t control_task_stack[APP_CONTROL_TASK_STACK_SIZE] __ALIGNED(APP_CONTROL_TASK_STACK_SIZE * sizeof(StackType_t));
StackType_t monitor_task_stack[APP_MONITOR_TASK_STACK_SIZE] __ALIGNED(APP_MONITOR_TASK_STACK_SIZE * sizeof(StackType_t));

/** Pointer to TCB */
TaskHandle_t sensor_task_handle = NULL;
TaskHandle_t comm_task_handle = NULL;
TaskHandle_t supervisor_task_handle = NULL;
TaskHandle_t control_task_handle = NULL;
TaskHandle_t monitor_task_handle = NULL;

/** Static TCB */
StaticTask_t sensor_task_buffer;
StaticTask_t comm_task_buffer;
StaticTask_t supervisor_task_buffer;
StaticTask_t control_task_buffer;
StaticTask_t monitor_task_buffer;

/** Task param */
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
        {(void *)__ipc_region_start__, (uint32_t)__ipc_region_size__, portMPU_REGION_READ_WRITE}}};

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
        {(void *)__ipc_region_start__, (uint32_t)__ipc_region_size__, portMPU_REGION_READ_WRITE}}};

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
        {(void *)__ipc_region_start__, (uint32_t)__ipc_region_size__ , portMPU_REGION_READ_WRITE}}};

TaskParameters_t control_task_params = {
    .pvTaskCode = control_task,
    .pcName = "Control task",
    .usStackDepth = APP_CONTROL_TASK_STACK_SIZE,
    .pvParameters = NULL,
    .uxPriority = APP_CONTROL_TASK_PRIORITY,
    .puxStackBuffer = control_task_stack,
    .pxTaskBuffer = &control_task_buffer,
    .xRegions = {
        {control_task_stack, sizeof(control_task_stack) * sizeof(StackType_t), portMPU_REGION_READ_WRITE},
        {(void *)FLASH_BASE, FLASH_SIZE, portMPU_REGION_READ_ONLY},
        {(void *)__ipc_region_start__, (uint32_t)__ipc_region_size__, portMPU_REGION_READ_WRITE}}};

TaskParameters_t monitor_task_params = {
    .pvTaskCode = monitor_task,
    .pcName = "Monitor task",
    .usStackDepth = APP_MONITOR_TASK_STACK_SIZE,
    .pvParameters = NULL,
    .uxPriority = APP_MONITOR_TASK_PRIORITY,
    .puxStackBuffer = monitor_task_stack,
    .pxTaskBuffer = &monitor_task_buffer,
    .xRegions = {
        {monitor_task_stack, sizeof(monitor_task_stack) * sizeof(StackType_t), portMPU_REGION_READ_WRITE},
        {(void *)FLASH_BASE, FLASH_SIZE, portMPU_REGION_READ_ONLY},
        {(void *)__ipc_region_start__, (uint32_t)__ipc_region_size__, portMPU_REGION_READ_WRITE}}};

/** Buffer for queue, sem ... */
static StaticSemaphore_t g_sensor_data_mutex;
static uint16_t adc_queue_buffer[10];
static StaticQueue_t g_adc_queue_buffer;

/**
 * @brief Init application
 * 
 */
void app_init(void)
{
    /** First is os */
    os_init();
    
    // Initialize application resources, tasks, and synchronization primitives here
    g_sensor_data.data_mutex = xSemaphoreCreateMutexStatic(&g_sensor_data_mutex);
    g_adc_queue = xQueueCreateStatic(10U, sizeof(uint16_t), (uint8_t *)adc_queue_buffer, &g_adc_queue_buffer);

    /** Then task create */
    xTaskCreateRestrictedStatic(&sensor_task_params, &sensor_task_handle);
    xTaskCreateRestrictedStatic(&comm_task_params, &comm_task_handle);
    xTaskCreateRestrictedStatic(&supervisor_task_params, &supervisor_task_handle);
    xTaskCreateRestrictedStatic(&control_task_params, &control_task_handle);
    xTaskCreateRestrictedStatic(&monitor_task_params, &monitor_task_handle);

    /** Register task for jitter measuring */
    os_registerJitterTask(comm_task_handle, APP_COMM_TASK_PERIOD_MS);
    os_registerJitterTask(supervisor_task_handle, APP_SUPERVISOR_TASK_PERIOD_MS);
    os_registerJitterTask(sensor_task_handle, APP_SENSOR_TASK_PERIOD_MS);
    os_registerJitterTask(control_task_handle, APP_CONTROL_TASK_PERIOD_MS);

    g_alive = 0xFFFF;
}

/** Start the app */
void app_start(void)
{
    adc_start_sampling();
    // Start application tasks or processes here
    vTaskStartScheduler();
}