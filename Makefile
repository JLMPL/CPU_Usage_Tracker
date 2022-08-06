
WARNINGS_GCC = -Wall -Wextra
WARNINGS_CLANG = -Weverything
STANDARD = -std=gnu11 #gnu for sigaction

SRC_FILES = src/main.c src/sig_catcher.c src/reader.c src/analyzer.c src/printer.c src/logger.c src/threading.c

default:
ifeq ($(CC), gcc)
	$(CC) $(STANDARD) -g $(WARNINGS_GCC) $(SRC_FILES) -o cutter
else ifeq ($(CC), clang)
	$(CC) $(STANDARD) -g $(WARNINGS_CLANG) $(SRC_FILES) -o cutter
endif

all:
	gcc $(STANDARD) -g $(WARNINGS_GCC) $(SRC_FILES) -o cutter
	clang $(STANDARD) -g $(WARNINGS_CLANG) $(SRC_FILES) -o cutter_clang

test:
	@echo "Do test stuff!"

run:
	./cutter
