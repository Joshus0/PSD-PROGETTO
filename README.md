# Gestione Interventi Condominio - Progetto PSD

**Corso:** Programmazione e Strutture Dati 2025/2026 (Traccia 1)  
**Team:** Joshua Sarnelli, Raffaele Pio Villani, Sabato Pio Siano  

---

## Descrizione

Applicazione a riga di comando scritta in **C99** per gestire le richieste di manutenzione condominiale. Supporta la registrazione di tecnici e interventi, l'assegnazione automatica in base a specializzazione e urgenza, la pianificazione di appuntamenti e il controllo dei conflitti orari nelle agende.

## Funzionalità principali

- registrazione di tecnici specializzati
- inserimento e archiviazione delle richieste di intervento
- coda prioritaria basata sull'urgenza
- assegnazione automatica al tecnico più idoneo
- pianificazione appuntamenti con controllo dei conflitti
- avanzamento dello stato delle richieste fino alla chiusura
- ricerche filtrate e report statistici
- visualizzazione dell'agenda tecnico

## Architettura

Il progetto utilizza strutture dati modulari per separare logica e dati:

- **Lista doppiamente concatenata** per l'archivio delle richieste
- **Max-Heap** per la coda di priorità delle richieste in attesa
- **Albero binario di ricerca (BST)** per l'anagrafica tecnici e la gestione delle agende

---

## Requisiti

- compilatore C compatibile con C99
- **Make**
- Windows oppure ambiente WSL/MSYS2/MinGW per `make`

---

## Compilazione e avvio

Dalla cartella principale del progetto, esegui:

```bash
make
make run
```

L'eseguibile viene creato in `build/programma.exe`.

---

## Uso rapido

All'avvio viene mostrato un menu numerico. Il flusso operativo tipico è:

1. registrare un nuovo tecnico
2. inserire una nuova richiesta
3. eseguire l'assegnazione automatica
4. pianificare l'intervento
5. aggiornare lo stato della richiesta fino alla chiusura

È possibile anche visualizzare l'elenco tecnici, la coda di attesa, lo storico degli interventi chiusi e l'agenda di un tecnico.

---

## Dati di esempio

I file di esempio per il caricamento dei dati si trovano in `test/data/`.

- `tecnici.txt`
- `richieste.txt`

I file utilizzano il separatore `;`.

---

## Struttura del repository

- `src/` — codice sorgente
  - `main/` — funzioni di menu e integrazione
  - `entita/` — gestione di tecnico e richiesta
- `include/` — header dei moduli
- `test/` — test automatici, file di input e oracoli
- `docs/` — documentazione del progetto
- `build/` — output di compilazione
- `Makefile` — regole di build e test

---

## Test automatici

Esegui i test con:

```bash
make test
```

La suite verifica il comportamento delle strutture dati, l'assegnazione automatica, la pianificazione e i report.

---

## Comandi utili

- `make` — compila il progetto
- `make run` — esegue il programma
- `make test` — compila ed esegue i test
- `make clean_w` — pulisce i file generati su Windows
- `make clean_l` — pulisce i file generati su Linux/macOS

---

## Note

Il progetto è progettato con un'architettura modulare che separa dati, logica e interfaccia. L'applicazione è pensata per un utilizzo didattico e per mostrare l'uso di strutture dati avanzate in un contesto gestionale.
