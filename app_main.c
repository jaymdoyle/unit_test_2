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

typedef enum
{
  TASK_LED,
  TASK_TEST,
  TASK_TX,

  NUM_TASKS_FOR_THIS_EXAMPLE
} TestTasks;


rtems_id   Task_id[ NUM_TASKS_FOR_THIS_EXAMPLE ];         /* array of task ids */
rtems_name Task_name[ NUM_TASKS_FOR_THIS_EXAMPLE ];       /* array of task naqmes */

extern rtems_task Test_led_task(
  rtems_task_argument task_index
);

extern rtems_task Test_test_task(
  rtems_task_argument task_index
);
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

//=======================LED TASK================================================
rtems_task Test_led_task(
  rtems_task_argument task_index
)
{
  rtems_interval    ticks;
  bool bln_LEDState = false;

  ticks = rtems_clock_get_ticks_per_second();

  for ( ; ; ) {

    if(bln_LEDState == false){
        BSP_LED_On(TEST_LED);
    } else {
        BSP_LED_Off(TEST_LED);
    }

    bln_LEDState = !bln_LEDState;

    (void) rtems_task_wake_after( ticks );
  }
}

//=====================UART==============================================

// -------------- UART DMA TX TASK -------------------------
rtems_task Test_uart_task(
  rtems_task_argument task_index
)
{
  rtems_interval    ticks;
  uint32_t counter = 0UL;
  uint8_t szTestTx[128];
  char UartName[32];
  const int TEST_UART = 3;
  int ret;

  ticks = rtems_clock_get_ticks_per_second() / 90;

  sprintf(UartName, "/dev/ttyS%d", TEST_UART);
  int filedesc = open(UartName, O_WRONLY | O_APPEND);

  while (true)
  {
    counter++;

    snprintf((char*) szTestTx, COUNTOF(szTestTx), "ttyS%d %lu 01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
            "\n\r", TEST_UART, counter);
    ret = write(filedesc, szTestTx, strlen((char*) szTestTx));

    while(ret != 0) {
        (void) rtems_task_wake_after( 1 );
        ret = write(filedesc, szTestTx, strlen((char*) szTestTx));
    }

    (void) rtems_task_wake_after( ticks );
  }
}


rtems_task Init(
  rtems_task_argument argument
)
{

  BSP_LED_Init(TEST_LED);

  Task_name[ TASK_LED ]     = rtems_build_name( 'T', 'L', 'E', 'D' );
  Task_name[ TASK_TEST ]    = rtems_build_name( 'U', 'N', 'I', 'T' );
  Task_name[ TASK_TX ]      = rtems_build_name( 'T', 'X', ' ', ' ' );

  (void) rtems_task_create(
    Task_name[ TASK_LED ], 255, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ TASK_LED ]
  );

  (void) rtems_task_create(
    Task_name[ TASK_TEST], 240, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ TASK_TEST ]
  );

  (void) rtems_task_create(
    Task_name[ TASK_TX], 240, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ TASK_TX ]
  );

  (void) rtems_task_start( Task_id[ TASK_LED ],  Test_led_task,  2 );
  //(void) rtems_task_start( Task_id[ TASK_TX ],   Test_uart_task,  3 );
  (void) rtems_task_start( Task_id[ TASK_TEST ], Test_test_task,  7 );

  printf("Starting shell...\r\n");
  start_shell();

  (void) rtems_task_delete( RTEMS_SELF );
}

/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
//#define CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS   32
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK    512
#define CONFIGURE_MAXIMUM_DRIVERS                  20

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_SWAPOUT_TASK_PRIORITY            10

#define CONFIGURE_MAXIMUM_TASKS                  20
#define CONFIGURE_INIT_TASK_STACK_SIZE           RTEMS_MINIMUM_STACK_SIZE
#define CONFIGURE_EXTRA_TASK_STACKS              RTEMS_MINIMUM_STACK_SIZE
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES             rtems_resource_unlimited (20)
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES         rtems_resource_unlimited (4)

#define CONFIGURE_MAXIMUM_PARTITIONS             rtems_resource_unlimited (2)
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_MAXIMUM_POSIX_KEYS             16
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS  16
#define CONFIGURE_MICROSECONDS_PER_TICK          1000
#define CONFIGURE_MEMORY_OVERHEAD                8

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#include <rtems/shellconfig.h>

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/
