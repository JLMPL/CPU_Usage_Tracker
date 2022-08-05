
all: gcc_target clang_target

SRC_FILES = src/main.c src/sig_catcher.c src/reader.c src/analyzer.c src/printer.c src/logger.c

#gnu99 for struct sigaction
gcc_target:
	gcc -std=gnu99 -g -Wall -Wextra $(SRC_FILES) -o cutter

clang_target:
	clang -std=gnu99 -g -Weverything $(SRC_FILES) -o cutter_clang

test:
	@echo "Do test stuff!"

run:
	./cutter
