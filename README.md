# Sistema di Gestione degli Interventi di Manutenzione Condominiale

> Progetto per il corso di **Programmazione e Strutture Dati (PSD)** — A.A. 2025/2026

## Gruppo di Lavoro — Traccia 1
| Nome | Cognome |
|------|---------|
| Joshua | Sarnelli |
| Raffaele Pio | Villani |
| Sabato Pio | Siano |

---

## Descrizione

Applicazione a riga di comando in **C (Standard C99)** per la gestione strutturata degli interventi di manutenzione in un condominio. Il sistema amministra l'intero ciclo di vita di una richiesta: dalla segnalazione del guasto, all'assegnazione a un tecnico specializzato, fino alla pianificazione e alla chiusura del ticket — con verifica automatica dei conflitti orari nell'agenda dei tecnici.

---

## Requisiti di Sistema

- Compilatore C compatibile con C89/C99 (es. **GCC**)
- Utility **Make**
- Sistema operativo: Linux, macOS, o Windows (tramite WSL, MSYS2 o MinGW)

---

## Compilazione ed Esecuzione

```bash
# Clona il repository
git clone <url-repository>
cd <nome-cartella>

# Compila il progetto
make

# Avvia il programma
./build/programma.exe

# Compila ed esegui i test
make test

# Rimuovi i file compilati
make clean
```

---

## Guida all'Uso

All'avvio, il programma mostra il **Pannello di Controllo Generale**. Inserisci il numero dell'opzione desiderata e premi INVIO.

```
 ________________________________________________________________________________________
|                                                                                        |
|                         PANNELLO DI CONTROLLO GENERALE                                 |
|___________________________________________|____________________________________________|
| [1] Nuovo Tecnico                         | [7] Pianifica Intervento                   |
| [2] Nuova Richiesta                       | [8] Ricerca Avanzata                       |
| [3] Assegnazione Auto                     | [9] Storico Conclusi                       |
| [4] Elenco Tecnici                        | [10] Statistiche Sistema                   |
| [5] Coda Attesa (Heap)                    | [11] Agenda Tecnico                        |
| [6] Aggiorna Stato                        | [0] Esci dal Programma                     |
|___________________________________________|____________________________________________|
```

---

### Flusso di Lavoro Consigliato

Il ciclo di vita tipico di un intervento segue questi passi nell'ordine:

```
[1] Registra Tecnico  →  [2] Inserisci Richiesta  →  [3] Assegnazione Auto
        →  [7] Pianifica Intervento  →  [6] Aggiorna Stato (CONCLUSA)
```

---

### `[1]` Registra Nuovo Tecnico

Aggiunge un tecnico al database. Campi richiesti:

| Campo | Esempio |
|-------|---------|
| Codice Identificativo | `T001` |
| Nome Completo | `Mario Rossi` |
| Specializzazione | `Idraulico` |

> ⚠️ Il codice deve essere univoco. Se esiste già, l'inserimento viene rifiutato.

---

### `[2]` Inserisci Nuova Richiesta

Registra una nuova segnalazione di guasto. Campi richiesti:

| Campo | Formato / Note |
|-------|----------------|
| Codice Richiesta | Es. `R001` — deve essere univoco |
| Appartamento / Area | Es. `Piano 1`, `Esterno` |
| Tipologia Problema | Es. `Idraulico`, `Elettrico`, `Varie` |
| Descrizione | Testo libero |
| Data | `GG/MM/AAAA` |
| Urgenza | Numero da `0` (minima) a `4` (massima) |

> La richiesta viene inserita automaticamente nella **coda di priorità** ordinata per urgenza.

---

### `[3]` Assegnazione Automatica

Estrae la richiesta **più urgente** dalla coda e cerca automaticamente il primo tecnico disponibile con la specializzazione corrispondente alla tipologia del problema.

- Se il match viene trovato, la richiesta passa allo stato **PIANIFICATA** e il tecnico risulta occupato.
- Se nessun tecnico compatibile è disponibile, la richiesta viene reinserita in coda.

> Eseguire questa opzione dopo aver registrato almeno un tecnico e una richiesta.

---

### `[4]` Visualizza Elenco Tecnici

Mostra tutti i tecnici registrati in **ordine alfabetico per codice**, con il loro stato di carico:

| Stato | Significato |
|-------|-------------|
| 🟢 `LIBERO (0)` | Nessun intervento in agenda |
| 🟡 `OCCUPATO (N)` | Meno di 3 interventi pianificati |
| 🔴 `PIENO (N)` | 3 o più interventi pianificati |

---

### `[5]` Visualizza Coda di Attesa

Mostra le richieste ancora in attesa di assegnazione, nell'ordine interno del max-heap. La richiesta con urgenza più alta è sempre in cima.

---

### `[6]` Aggiorna Stato Richiesta

Permette di far avanzare manualmente una richiesta nel suo ciclo di vita:

```
APERTA  →  PIANIFICATA  →  IN_LAVORAZIONE  →  CONCLUSA
                                           →  ANNULLATA
```

| Valore | Stato |
|--------|-------|
| `0` | APERTA |
| `1` | PIANIFICATA |
| `2` | IN_LAVORAZIONE |
| `3` | CONCLUSA |
| `4` | ANNULLATA |

> Quando si imposta lo stato a **CONCLUSA**, il sistema richiede la data di chiusura effettiva (`GG/MM/AAAA`) e libera automaticamente lo slot nell'agenda del tecnico.

---

### `[7]` Pianifica Intervento

Assegna una **data e fascia oraria** a una richiesta già assegnata a un tecnico. Il sistema verifica automaticamente che il tecnico non abbia già un impegno in quello slot.

Campi richiesti:

| Campo | Esempio |
|-------|---------|
| Codice Richiesta | `R001` |
| Data appuntamento | `20/05/2026` |
| Fascia oraria | `09:00-11:00` |

> Se lo slot è occupato, l'operazione viene rifiutata con un messaggio di conflitto.

---

### `[8]` Ricerca Avanzata

Sottomenu con 6 filtri di ricerca applicabili all'archivio:

| Opzione | Filtro | Input richiesto |
|---------|--------|-----------------|
| `[1]` | Per Stato | Numero 0–4 |
| `[2]` | Per Urgenza | Numero 0–4 |
| `[3]` | Per Tipologia | Stringa (es. `Idraulico`) |
| `[4]` | Per Appartamento/Area | Stringa (es. `Piano 1`) |
| `[5]` | Per Tecnico Assegnato | Codice tecnico (es. `T001`) |
| `[6]` | Per Codice Richiesta | Codice richiesta (es. `R002`) |

---

### `[9]` Storico Interventi Conclusi

Mostra tutti gli interventi con stato **CONCLUSA**, con data di chiusura e tecnico che ha gestito il lavoro.

---

### `[10]` Genera Report Statistiche

Sottomenu con 6 tipologie di report:

| Opzione | Report |
|---------|--------|
| `[1]` | Report completo (tutti i seguenti) |
| `[2]` | Stato globale interventi (aperti, in lavorazione, chiusi) |
| `[3]` | Distribuzione interventi per tipologia |
| `[4]` | Tempo medio di completamento (giorni) |
| `[5]` | Tecnico più attivo |
| `[6]` | Appartamenti/aree con più segnalazioni |

---

### `[11]` Visualizza Agenda Tecnico

Mostra tutti gli interventi pianificati per un tecnico specifico, ordinati **cronologicamente**.

Input richiesto: codice tecnico (es. `T001`).

---

## Caricamento Dati da File

È possibile pre-caricare tecnici e richieste da file CSV con separatore `;`.

**Formato `tecnici.txt`:**
```
T001;Mario Rossi;Idraulico
T002;Luigi Bianchi;Elettrico
```

**Formato `richieste.txt`:**
```
R001;Piano 1;Idraulico;Tubo rotto in bagno;10/05/2026;4
R002;Piano Terra;Elettrico;Cortocircuito;11/05/2026;3
```

I file di esempio si trovano in `test/data/`.

---

## Struttura della Repository

```
.
├── src/
│   ├── main/           # main.c, utilita.c, alberoTecnici.c, agendaTecnico.c
│   ├── entita/         # richiesta.c, tecnico.c
│   ├── codaPriorita.c
│   ├── archivioRichieste.c
│   └── report.c
├── include/
│   ├── main/           # utilita.h, alberoTecnici.h, agendaTecnico.h (*)
│   ├── entita/         # richiesta.h, tecnico.h
│   ├── codaPriorita.h
│   ├── archivioRichieste.h
│   └── report.h
├── test/
│   ├── main_test.c     # Suite di 8 test case con pattern Golden Master
│   ├── data/           # File CSV di input per i test
│   └── oracoli/        # File di output atteso per ogni test
├── docs/               # Report formale PDF con architettura e specifica ADT
├── build/              # File oggetto compilati (ignorato da git)
└── Makefile
```


## Esecuzione dei Test

La suite comprende **8 test case** automatizzati con pattern *Golden Master*: ogni test genera un file di output e lo confronta con un file oracolo pre-validato.

```bash
make test
```

| Test | Cosa verifica |
|------|---------------|
| Test 1 | Ordinamento max-heap dopo caricamento da file |
| Test 2 | Visita in-order del BST tecnici (ordine alfabetico) |
| Test 3 | Assegnazione automatica richiesta → tecnico |
| Test 4 | Rilevazione conflitti orari nell'agenda |
| Test 5 | Transizioni di stato del ciclo di vita |
| Test 6 | Ricerca per codice (caso trovato e non trovato) |
| Test 7 | Filtro storico interventi conclusi |
| Test 8 | Correttezza aggregazione statistiche |