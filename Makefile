CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude

# 1. Trova e prepara tutti i file
OBJS = $(patsubst src/%.c, build/%.o, $(wildcard src/*.c src/*/*.c))
TEST_OBJS = $(patsubst test/%.c, build/test_%.o, $(wildcard test/*.c))

# 2. Controllo OS vitale SOLO per la pulizia
ifeq ($(OS),Windows_NT)
    CLEAN_CMD = del /Q /S build\*.o bin\*.exe 2>nul
else
    CLEAN_CMD = rm -f build/*.o build/*/*.o build/test_*.o bin/*
endif

# 3. Regole App Principale
all: bin/condominio_app.exe

bin/condominio_app.exe: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 4. Regole Test (Filtra via il main.o dell'app)
test: $(filter-out %main.o, $(OBJS)) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o bin/test_runner.exe $^
	./bin/test_runner.exe

build/test_%.o: test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 5. Comandi
run: all
	./bin/condominio_app.exe

clean:
	-$(CLEAN_CMD)

.PHONY: all test run clean