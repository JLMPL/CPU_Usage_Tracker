
WARNINGS_GCC = -Wall -Wextra
WARNINGS_CLANG = -Weverything
STANDARD = -std=gnu11 #gnu for sigaction

SRC_FILES = src/main.c src/sig_catcher.c src/reader.c src/analyzer.c src/printer.c src/logger.c src/threading.c src/watchdog.c

default:
ifeq ($(CC), gcc)
	$(CC) $(STANDARD) -g $(WARNINGS_GCC) $(SRC_FILES) -o cutter
else ifeq ($(CC), clang)
	$(CC) $(STANDARD) -g $(WARNINGS_CLANG) $(SRC_FILES) -o cutter
endif

all:
	gcc $(STANDARD) -g $(WARNINGS_GCC) $(SRC_FILES) -o cutter
	clang $(STANDARD) -g $(WARNINGS_CLANG) $(SRC_FILES) -o cutter_clang

TEST_FILES = src/test_main.c src/sig_catcher.c src/reader.c src/analyzer.c src/printer.c src/logger.c src/threading.c src/watchdog.c

test:
	clang $(STANDARD) -g -DBUILD_TEST $(WARNINGS_CLANG) $(TEST_FILES) -o cutter_test
	./cutter_test

run:
	./cutter_clang
