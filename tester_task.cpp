/*
 * tester_task.cpp
 *
 *  Created on: Aug 14, 2015
 *      Author: jay.doyle
 */

#include <rtems.h>
#include <CppUTest/CommandLineTestRunner.h>

extern "C"
{
  const int argc = 4;
  const char program_name[16]   = "unittest";
  const char verbose_option[16] = "-v";
  const char output_option[3] = "-o";
  const char output_type[16] = "normal";
  const char* argv[argc] = {(char*) program_name,
                            (char*) verbose_option,
                            (char*) output_option,
                            (char*) output_type};

  //======================= Test task================================================

  void done_with_testing(void) {
    rtems_task_delete(RTEMS_SELF);
  }


  rtems_task Test_test_task(
    rtems_task_argument task_index
  )
  {
    // (void) rtems_task_wake_after( 100);
    CommandLineTestRunner::RunAllTests(argc, (char**)argv);

    // This function is used for gdb breakpoint
    done_with_testing();

  }
}
