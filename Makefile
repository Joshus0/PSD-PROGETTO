.SILENT:

# --- Configurazione Compilatore ---
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude

# --- RILEVAMENTO UNIVERSALE DEL SISTEMA OPERATIVO ---
ifeq ($(OS),Windows_NT)
	TARGET_EXT = .exe
	# Obbliga mingw32-make a usare il Prompt di Windows (cmd.exe) per tutti
	SHELL = cmd.exe
	# Comando di pulizia nativo di Windows (elimina i .o anche nelle sottocartelle in silenzio)
	CLEAN_CMD = del /Q /S build\*.o bin\*.exe 2>nul
else
	TARGET_EXT = 
	CLEAN_CMD = rm -f build/*.o build/entita/*.o build/main/*.o bin/*
endif
# ----------------------------------------------------

# --- Variabili Cartelle ed Eseguibili ---
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build
BIN_DIR = bin
TEST_DIR = test

TARGET = $(BIN_DIR)/condominio_app$(TARGET_EXT)
TEST_TARGET = $(BIN_DIR)/test_runner$(TARGET_EXT)

# --- Gestione File Sorgente e Oggetto ---
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/entita/*.c) $(wildcard $(SRC_DIR)/main/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

SRCS_NO_MAIN = $(filter-out $(SRC_DIR)/main/main.c, $(SRCS))
OBJS_NO_MAIN = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS_NO_MAIN))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(OBJ_DIR)/test_%.o, $(TEST_SRCS))

# --- Regole Principali ---
all: dirs $(TARGET)

# --- CREAZIONE CARTELLE (NATIVA WINDOWS) ---
ifeq ($(OS),Windows_NT)
dirs:
	@if not exist $(OBJ_DIR)\entita mkdir $(OBJ_DIR)\entita
	@if not exist $(OBJ_DIR)\main mkdir $(OBJ_DIR)\main
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
else
dirs:
	@mkdir -p $(OBJ_DIR)/entita $(OBJ_DIR)/main $(BIN_DIR)
endif

# Crea l'eseguibile principale
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compila i file .c in .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Regole per Test e Pulizia ---
test: dirs $(TEST_TARGET)

# Crea l'eseguibile dei test
$(TEST_TARGET): $(OBJS_NO_MAIN) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compila i file di test in .o
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Svuota le cartelle di build
clean:
	$(CLEAN_CMD)

run: all
	./$(TARGET)

.PHONY: all dirs test clean run