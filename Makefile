TESTS=tests

BUILD_DIR=bin

FAIL_ON_WARNINGS?=False
ALL_WARNINGS?=False

LDLIBS+=-lm

INCLUDE_DIR_LIST=unity/src/ \
                 unity/extras/fixture/src

INCLUDES=$(foreach var,$(INCLUDE_DIR_LIST), -I$(var))

SRC_FILES = rbst.c uniquemap.c

TEST_SRC_FILES = unity/src/unity.c \
            unity/extras/fixture/src/unity_fixture.c \
            *_test.c \
            test_runner.c
      
ifeq ($(ALL_WARNINGS), True)
	WARNINGS_OPTION+=-Wall
endif

ifeq ($(FAIL_ON_WARNINGS), True)
	WARNINGS_OPTION+=-Werror
endif

CFLAGS = $(WARNINGS_OPTION) $(INCLUDES) -g -std=gnu99
CC=gcc

$(TESTS):
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) $(SRC_FILES) -o $(BUILD_DIR)/$(TESTS) $(LDLIBS)
	cd $(BUILD_DIR); ./$(TESTS)
