/*
// *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>
#include <irq-generic.h>
#include <stdio.h>
#include <stdlib.h>
#include <main.h>

#ifdef STM32F746xx
#include <stm32f7xx_hal_conf.h>
#include <stm32f7xx_hal.h>
#include <stm32f746xx.h>
#define TEST_LED LED1
#elif STM32F407xx
#include <stm32f4xx_hal_conf.h>
#include <stm32f4xx_hal.h>
#include <stm32f407xx.h>
#define TEST_LED LED3
#endif

#include <rtems/shell.h>
#include <fcntl.h>
#include <bsp.h>
#include <stm32f-processor-specific.h>
#include <hal-utils.h>
#include <dev/can/can.h>
#include <hal-fatal-error-handler.h>

typedef enum
{
  TASK_LED,
  TASK_TEST,
  TASK_ECHO,

  NUM_TASKS_FOR_THIS_EXAMPLE
} TestTasks;


rtems_id   Task_id[ NUM_TASKS_FOR_THIS_EXAMPLE ];         /* array of task ids */
rtems_name Task_name[ NUM_TASKS_FOR_THIS_EXAMPLE ];       /* array of task names */

extern rtems_task Test_led_task (rtems_task_argument task_index);
extern rtems_task Test_test_task(rtems_task_argument task_index);
extern rtems_task Test_echo_task(rtems_task_argument task_index);

//====================CONSOLE UART=============================================

void start_shell(void)
{
    rtems_status_code ret = RTEMS_SUCCESSFUL;

    printf(" =========================\n");
    printf(" starting shell\n");
    printf(" =========================\n");
    ret = rtems_shell_init(
            "SHLL", /* task name */
            RTEMS_MINIMUM_STACK_SIZE * 4, /* task stack size */
            100, /* task priority */
            "/dev/console", /* device name */
            false, /* run forever */
            true, /* wait for shell to terminate */
            rtems_shell_login_check /* login check function,
        use NULL to disable a login check */
    );

    if(ret != RTEMS_SUCCESSFUL){
        printf("Failed to start shell (%s)\r\n", rtems_status_text(ret));
    }
}


rtems_task Init(
  rtems_task_argument argument
)
{

  stm32_bsp_register_uart();
  stm32_bsp_register_can();
  stm32_bsp_register_i2c();
  stm32_bsp_register_spi();
  stm32f_initialize_user_extensions();

  Task_name[ TASK_TEST ]    = rtems_build_name( 'U', 'N', 'I', 'T' );
  (void) rtems_task_create(
    Task_name[ TASK_TEST], 100, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ TASK_TEST ]
  );
  (void) rtems_task_start( Task_id[ TASK_TEST ], Test_test_task,  7 );

  start_shell();

  (void) rtems_task_delete( RTEMS_SELF );
}

//==================== POSIX_Init ====================================
void* POSIX_Init()
{
  // This function is necessary in able to call CONFIGURE_POSIX_INIT_THREAD_TABLE
  return NULL;
}


/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
//#define CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS   64
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK    512
#define CONFIGURE_MAXIMUM_DRIVERS                  20

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_SWAPOUT_TASK_PRIORITY            10

#define CONFIGURE_INIT_TASK_STACK_SIZE           RTEMS_MINIMUM_STACK_SIZE
#define CONFIGURE_EXTRA_TASK_STACKS              RTEMS_MINIMUM_STACK_SIZE
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS                  rtems_resource_unlimited (20)
#define CONFIGURE_MAXIMUM_BARRIERS               rtems_resource_unlimited (10)
#define CONFIGURE_MAXIMUM_SEMAPHORES             rtems_resource_unlimited (20)
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES         rtems_resource_unlimited (4)
#define CONFIGURE_MAXIMUM_PARTITIONS             rtems_resource_unlimited (2)
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS            8
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_MAXIMUM_POSIX_KEYS                 16
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS      16
#define CONFIGURE_MAXIMUM_POSIX_THREADS              10
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  20
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              40

#define CONFIGURE_MICROSECONDS_PER_TICK          1000

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#include <rtems/shellconfig.h>

#define CONFIGURE_INIT
#include <rtems/confdefs.h>


/****************  END OF CONFIGURATION INFORMATION  ****************/

