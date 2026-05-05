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
* Gestione Richieste: inserimento di nuove segnalazioni, aggiornamento dello stato (Aperta, Pianificata, In lavorazione, Conclusa, Annullata) e monitoraggio dei livelli di urgenza.
* Anagrafica Tecnici: registrazione del personale con relative specializzazioni, disponibilità e codici identificativi.
* Pianificazione: associazione tra richieste e tecnici compatibili, con controlli logici per evitare sovrapposizioni nelle fasce orarie.
* Ricerca e Filtri: interrogazione del sistema per stato, urgenza, tipologia del problema, appartamento o tecnico specifico.
* Reportistica: generazione di statistiche su tempi medi di completamento, carico di lavoro dei tecnici (tecnico più attivo) e storicizzazione degli interventi completati.

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

Per compilare ed eseguire i casi di test:
    make test
    ./bin/test_runner

Per ripulire l'ambiente (rimozione file oggetto ed eseguibili):
    make clean

## Struttura della Repository
* src/ : Contiene il codice sorgente principale del programma (file .c).
* include/ : Contiene i file di intestazione per le definizioni delle strutture e delle firme delle funzioni (file .h).
* test/ : Contiene il file main_test.c e i moduli per eseguire il razionale dei casi di test.
* docs/ : Contiene il report formale in PDF con l'architettura del sistema e la specifica degli ADT.
* build/ : Cartella temporanea per i file oggetto compilati (ignorata da git).
* bin/ : Cartella di destinazione per i file eseguibili finali (ignorata da git).

---
