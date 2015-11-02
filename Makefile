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
CXXSRC_CPPUNIT = tester_task.cpp \
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

CXXSRC_TEST = $(TEST_DIR)/uart/uart_unit.cpp    \
              $(TEST_DIR)/uart/uart_fit.cpp     \
              $(TEST_DIR)/sdram/sdram_fit.cpp   \
              $(TEST_DIR)/sdram/sdram_unit.cpp  \
              $(TEST_DIR)/cmsis/cmsis_unit.cpp  \
              $(TEST_DIR)/can/can_fit.cpp       \
              $(TEST_DIR)/can/can_unit.cpp      \
  #           $(TEST_DIR)/ethernet/ethernet_fit.cpp 


                 
CXXSRC_TEST_FILENAMES  = $(notdir $(CXXSRC_TEST))
 
COBJS                  = $(CSRCS:%.c=${ARCH}/%.o) 
CXXOBJS_CPPUNIT        = $(CXXSRC_CPPUNIT:%.cpp=${ARCH}/%.o) 
CXXOBJS_TEST           = $(CXXSRC_TEST_FILENAMES:%.cpp=${ARCH}/%.o)

CXXOBJS = $(CXXOBJS_CPPUNIT) $(CXXOBJS_TEST)
CXXSRCS = $(CXXSRC_CPPUNIT) $(CXXSRC_TEST)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CXXOBJS_TEST: $(CXXSRC_TEST)

COMMON_FLAGS += -I$(RTEMS_BSP_INCLUDE_PATH)
COMMON_FLAGS += -I. 
COMMON_FLAGS += -DCPPUNIT
COMMON_FLAGS += -I$(CPPUNIT_INCLUDE)
COMMON_FLAGS += -I/other/rtems/sources/rtems/c/src/lib/libbsp/arm/shared/stm32fxxxx/uart
COMMON_FLAGS += -I/other/rtems/sources/rtems/c/src/lib/libbsp/arm/stm32f7x/uart
COMMON_FLAGS += -I/other/rtems/bsps/arm-rtems4.11/stm32f7x/lwip/include
COMMON_FLAGS += -I/other/rtems/sources/rtems/c/src/lib/libbsp/arm/shared/stm32fxxxx/test/include

COMMON_FLAGS += -D$(RTEMS_TARGET_PROCESSOR) \
                -DTARGET_STM_PROCESSOR_PREFIX=$(TARGET_STM_PROCESSOR_PREFIX) \
                -DTARGET_STM_PROCESSOR=$(TARGET_STM_PROCESSOR)

#COMMON_FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
AM_LDFLAGS   += -L/other/firmware/cpputest/cpputest_build/lib 
LINK_LIBS    += -L /other/rtems/bsps/arm-rtems4.11/stm32f7x/lwip/lib  -lstdc++ -llwip -Wl,-Map=${ARCH}/unit_test.map 

#COMMON_FLAGS += -fprofile-arcs -ftest-coverage
#LINK_LIBS    += -lstdc++ -lm -Wl,-Map=${ARCH}/unit_test.map -fprofile-arcs

CFLAGS   += $(COMMON_FLAGS)
CPPFLAGS += $(COMMON_FLAGS) -DCPPUTEST_OUTPUT_DEVICE=\"/dev/ttyS3\"

$(CPPOUTDIR):
	mkdir ${ARCH}/src
	mkdir ${ARCH}/$(CPPUNIT_SRC)

OBJS = $(COBJS) $(CXXOBJS)  $(ASOBJS) 

all:    ${ARCH} $(CPPOUTDIR) $(PGM) $(PGM_BIN)

${ARCH}/%.o: $(TEST_DIR)/uart/%.cpp
	${COMPILE.cc} $(AM_CPPFLAGS) $(AM_CXXFLAGS) -o $@ $<
	
${ARCH}/%.o: $(TEST_DIR)/sdram/%.cpp
	${COMPILE.cc} $(AM_CPPFLAGS) $(AM_CXXFLAGS) -o $@ $<
	
${ARCH}/%.o: $(TEST_DIR)/can/%.cpp
	${COMPILE.cc} $(AM_CPPFLAGS) $(AM_CXXFLAGS) -o $@ $<
	
${ARCH}/%.o: $(TEST_DIR)/cmsis/%.cpp
	${COMPILE.cc} $(AM_CPPFLAGS) $(AM_CXXFLAGS) -o $@ $<

${ARCH}/%.o: $(TEST_DIR)/ethernet/%.cpp
	${COMPILE.cc} $(AM_CPPFLAGS) $(AM_CXXFLAGS) -o $@ $<
	
${ARCH}/%.o: $(TEST_DIR)/util/%.cpp
	${COMPILE.cc} $(AM_CPPFLAGS) $(AM_CXXFLAGS) -o $@ $<

$(PGM): $(OBJS)
	$(make-exe)

$(PGM_BIN): $(PGM)
	$(OBJ_COPY) -O binary $(PGM) $(PGM_BIN)