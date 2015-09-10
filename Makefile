#
#  RTEMS_MAKEFILE_PATH is typically set in an environment variable
#

PGM=${ARCH}/unit_test.exe

PGM_BIN = $(PGM:%.exe=%.bin)
OBJ_COPY= arm-rtems4.11-objcopy

# optional managers required
MANAGERS=all

CPPUNIT_SRC=src/CppUTest
CPPUNIT_INCLUDE=include

CPPOUTDIR = ${ARCH}/$(CPPUNIT_SRC)
TEST_DIR  = /other/rtems/sources/rtems/c/src/lib/libbsp/arm/shared/stm32fxxxx/test

# C source names
CSRCS   = app_main.c
CXXSRCS = tester_task.cpp \
$(CPPUNIT_SRC)/CommandLineTestRunner.cpp \
$(CPPUNIT_SRC)/CommandLineArguments.cpp \
$(CPPUNIT_SRC)/JUnitTestOutput.cpp \
$(CPPUNIT_SRC)/MemoryLeakDetector.cpp \
$(CPPUNIT_SRC)/MemoryLeakWarningPlugin.cpp \
$(CPPUNIT_SRC)/SimpleMutex.cpp \
$(CPPUNIT_SRC)/SimpleString.cpp \
$(CPPUNIT_SRC)/TestFailure.cpp \
$(CPPUNIT_SRC)/TestFilter.cpp \
$(CPPUNIT_SRC)/TestHarness_c.cpp \
$(CPPUNIT_SRC)/TestMemoryAllocator.cpp \
$(CPPUNIT_SRC)/TestOutput.cpp \
$(CPPUNIT_SRC)/TestPlugin.cpp \
$(CPPUNIT_SRC)/TestRegistry.cpp \
$(CPPUNIT_SRC)/TestResult.cpp \
$(CPPUNIT_SRC)/Utest.cpp \
$(CPPUNIT_SRC)/UtestPlatform.cpp 

TEST_SRC = $(TEST_DIR)/uart/uart_test.cpp \
           $(TEST_DIR)/sdram/sdram_fit.cpp 
                 
COBJS   = $(CSRCS:%.c=${ARCH}/%.o) 
CXXOBJS = $(CXXSRCS:%.cpp=${ARCH}/%.o) 

TESTFILENAMES = $(notdir $(TEST_SRC))
TESTOBJS      = $(TESTFILENAMES:%.cpp=${ARCH}/%.o) 

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

COMMON_FLAGS += -I$(RTEMS_BSP_INCLUDE_PATH)
COMMON_FLAGS += -I. 
COMMON_FLAGS += -DCPPUNIT
COMMON_FLAGS += -I$(CPPUNIT_INCLUDE)
COMMON_FLAGS += -I/other/rtems/sources/rtems/c/src/lib/libbsp/arm/shared/stm32fxxxx/uart
COMMON_FLAGS += -I/other/rtems/sources/rtems/c/src/lib/libbsp/arm/stm32f7x/uart

COMMON_FLAGS += -D$(RTEMS_TARGET_PROCESSOR) -DTARGET_STM_PROCESSOR_PREFIX=$(TARGET_STM_PROCESSOR_PREFIX) -DTARGET_STM_PROCESSOR=$(TARGET_STM_PROCESSOR)
COMMON_FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
AM_LDFLAGS   += -L/other/firmware/cpputest/cpputest_build/lib 
LINK_LIBS    += -lstdc++ -lm -Wl,-Map=${ARCH}/unit_test.map 

CFLAGS   += $(COMMON_FLAGS)
CPPFLAGS += $(COMMON_FLAGS) -DCPPUTEST_OUTPUT_DEVICE=\"/dev/ttyS3\"

$(CPPOUTDIR):
	mkdir ${ARCH}/src
	mkdir ${ARCH}/$(CPPUNIT_SRC)

OBJS = $(COBJS) $(CXXOBJS) $(ASOBJS) $(TESTOBJS)

all:    ${ARCH} $(CPPOUTDIR) $(PGM) $(PGM_BIN)

$(PGM): $(OBJS)
	$(make-exe)

$(PGM_BIN): $(PGM)
	$(OBJ_COPY) -O binary $(PGM) $(PGM_BIN)