# Sistema di Gestione degli Interventi di Manutenzione in un Condominio

Progetto per il corso di Programmazione e Strutture Dati (PSD) - Anno Accademico 2025/2026

## Gruppo di Lavoro (Traccia 1)
* Joshua Sarnelli
* Raffaele Pio Villani
* Sabato Pio Siano

## Descrizione del Progetto
Il progetto consiste in un'applicazione a linea di comando sviluppata in linguaggio C (Standard C99) per la gestione strutturata degli interventi di manutenzione all'interno di un condominio. 

Il sistema permette di amministrare l'intero ciclo di vita di una richiesta di intervento: dalla segnalazione del problema all'assegnazione a un tecnico specializzato, fino alla pianificazione e al tracciamento dello stato di risoluzione. Il software è progettato per garantire l'assenza di conflitti di orario nell'agenda dei tecnici.

## Funzionalità Implementate
* **Gestione Richieste**: inserimento di nuove segnalazioni, aggiornamento dello stato (Aperta, Pianificata, In lavorazione, Conclusa, Annullata) e monitoraggio dei livelli di urgenza.
* **Anagrafica Tecnici**: registrazione del personale con relative specializzazioni, disponibilità e codici identificativi.
* **Pianificazione interventi**: associazione tra richieste e tecnici compatibili con verifica automatica di conflitti orari.
* **Ricerca Avanzata**: interrogazione del sistema per stato, urgenza, tipologia del problema, appartamento o tecnico specifico.
* **Storico Interventi**: visualizzazione di tutti gli interventi conclusi con relativa documentazione.
* **Reportistica**: generazione di statistiche su tempi di completamento, carico di lavoro dei tecnici e distribuzione dei problemi.
* **Agenda Tecnico**: visualizzazione degli interventi pianificati per ogni tecnico, con verifica dei conflitti orari.

## Requisiti di Sistema
* Compilatore C compatibile con lo standard C89/C99 (es. GCC).
* Utility Make per la compilazione automatizzata.
* Ambiente operativo: Linux, macOS o Windows (tramite WSL, MSYS2 o MinGW).

## Istruzioni di Compilazione ed Esecuzione
Il progetto include un Makefile per la gestione automatizzata del processo di build. Dal terminale, posizionandosi nella root del progetto, è possibile utilizzare i seguenti comandi:

Per compilare l'applicazione principale:
    make

Per avviare il sistema:
    ./bin/condominio_app

Per compilare ed eseguire i casi di test (49 test case):
    make test

Per ripulire l'ambiente (rimozione file oggetto ed eseguibili):
    make clean

## Struttura della Repository
* src/ : Contiene il codice sorgente principale del programma (file .c).
* include/ : Contiene i file di intestazione per le definizioni delle strutture e delle firme delle funzioni (file .h).
* test/ : Contiene il file main_test.c e i moduli per eseguire il razionale dei casi di test.
* docs/ : Contiene il report formale in PDF con l'architettura del sistema e la specifica degli ADT.
* build/ : Cartella temporanea per i file oggetto compilati (ignorata da git).
* bin/ : Cartella di destinazione per i file eseguibili finali (ignorata da git).

## Menu Principale dell'Applicazione
1. **Registra Nuovo Tecnico** - Aggiunge un nuovo tecnico al database con codice, nome e specializzazione
2. **Inserisci Nuova Richiesta** - Crea una nuova richiesta di intervento con tutti i dettagli
3. **Assegna Richiesta Prioritaria** - Assegna automaticamente la richiesta più urgente a un tecnico disponibile
4. **Visualizza Elenco Tecnici** - Mostra tutti i tecnici registrati in ordine alfabetico
5. **Visualizza Coda di Attesa** - Visualizza la coda di priorità dei richieste (heap)
6. **Aggiornamento Stato Richiesta** - Modifica lo stato di una richiesta esistente
7. **Pianifica Intervento** - Assegna data e fascia oraria, con verifica automatica dei conflitti
8. **Ricerca Avanzata** - Filtra le richieste per urgenza, tipologia, appartamento, tecnico o stato
9. **Visualizza Storico Interventi Conclusi** - Mostra tutti gli interventi già completati
10. **Genera Report Statistiche** - Produce statistiche globali sull'andamento del sistema
11. **Visualizza Agenda Tecnico** - Mostra gli interventi pianificati per un tecnico specifico

---
