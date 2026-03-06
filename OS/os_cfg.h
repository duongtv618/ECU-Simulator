#ifndef OS_CFG_H
#define OS_CFG_H

/**
 * @brief The IPC_REGION is for data that task unprivileged 
 * can access common use data
 * 
 */
#define IPC_REGION __attribute__((section(".ipc_region")))

/**
 * @brief SYSTEMCALL is for function that need to use system register
 * Notice that this function can be called by unprivileged task so i
 * use freertos_system_calls section then wrap with portRaisePrivileged
 * and portResetPriviledged
 * 
 */
#define SYSTEMCALL __attribute__((section("freertos_system_calls")))

/**
 * @brief USART ring buffer size
 * 
 */
#define USART_RING_BUFFER_SIZE 512U

/**
 * @brief MAX jitter internal instance
 * 
 */
#define OS_JITTER_MAX_TASK 5U

/**
 * @brief For logging (further dev)
 * 
 */
#define OS_USE_LOG 1

#endif