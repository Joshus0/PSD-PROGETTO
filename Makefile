CC     = gcc
CFLAGS = -Wall -Iinclude -Iinclude/entita -Iinclude/main

# ── File oggetto ──────────────────────────────────────────────────────────────
OBJ = build/codaPriorita.o        \
      build/archivioRichieste.o   \
      build/main/utilita.o        \
      build/entita/richiesta.o    \
      build/entita/tecnico.o      \
      build/main/agendaTecnico.o  \
      build/main/alberoTecnici.o  \
      build/main/main.o

# ── Eseguibile finale ─────────────────────────────────────────────────────────
programma: $(OBJ)
	$(CC) $(OBJ) -o build/programma.exe

# ── build/ ────────────────────────────────────────────────────────────────────
build/codaPriorita.o: src/codaPriorita.c include/codaPriorita.h
	$(CC) $(CFLAGS) -c src/codaPriorita.c -o build/codaPriorita.o

build/archivioRichieste.o: src/archivioRichieste.c include/archivioRichieste.h
	$(CC) $(CFLAGS) -c src/archivioRichieste.c -o build/archivioRichieste.o

# ── build/entita/ ─────────────────────────────────────────────────────────────
build/entita/richiesta.o: src/entita/richiesta.c include/entita/richiesta.h include/main/utilita.h
	$(CC) $(CFLAGS) -c src/entita/richiesta.c -o build/entita/richiesta.o

build/entita/tecnico.o: src/entita/tecnico.c include/entita/tecnico.h include/main/utilita.h
	$(CC) $(CFLAGS) -c src/entita/tecnico.c -o build/entita/tecnico.o

# ── build/main/ ───────────────────────────────────────────────────────────────
build/main/utilita.o: src/main/utilita.c include/main/utilita.h
	$(CC) $(CFLAGS) -c src/main/utilita.c -o build/main/utilita.o

build/main/agendaTecnico.o: src/main/agendaTecnico.c include/agendaTecnico.h include/main/utilita.h
	$(CC) $(CFLAGS) -c src/main/agendaTecnico.c -o build/main/agendaTecnico.o

build/main/alberoTecnici.o: src/main/alberoTecnici.c include/alberoTecnici.h include/entita/tecnico.h
	$(CC) $(CFLAGS) -c src/main/alberoTecnici.c -o build/main/alberoTecnici.o

build/main/main.o: src/main/main.c \
                   include/entita/richiesta.h include/entita/tecnico.h \
                   include/main/utilita.h include/agendaTecnico.h \
                   include/alberoTecnici.h include/archivioRichieste.h \
                   include/codaPriorita.h
	$(CC) $(CFLAGS) -c src/main/main.c -o build/main/main.o

# ── Esecuzione ────────────────────────────────────────────────────────────────
run: programma
	./build/programma.exe

# ── Pulizia ───────────────────────────────────────────────────────────────────
clean:
	-del /S /Q /F src\*.o 2>nul
	-del /Q /F programma.exe 2>nul

# ── Test ──────────────────────────────────────────────────────────────────────
build/main_test.o: test/main_test.c
	$(CC) $(CFLAGS) -c test/main_test.c -o build/main_test.o

test: build/codaPriorita.o build/archivioRichieste.o build/main/utilita.o \
      build/entita/richiesta.o build/entita/tecnico.o \
      build/main/agendaTecnico.o build/main/alberoTecnici.o \
      build/main_test.o
	$(CC) $^ -o build/test_runner.exe
	./build/test_runner.exe