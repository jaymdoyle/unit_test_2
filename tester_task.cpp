/*
 * tester_task.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: jay.doyle
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
#include <CppUTest/CommandLineTestRunner.h>

extern "C"
{
  const int argc = 2;
  const char program_name[16]   = "unittest";
  const char verbose_option[16] = "-v";
  const char* argv[argc] = {(char*) program_name, (char*) verbose_option};

  //=======================LED TAKS================================================
  rtems_task Test_test_task(
    rtems_task_argument task_index
  )
  {
    (void) rtems_task_wake_after( 100);
    CommandLineTestRunner::RunAllTests(argc, (char**)argv);

    (void) rtems_task_delete( RTEMS_SELF );
  }
}
