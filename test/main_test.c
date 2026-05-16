/*
 * File: main_test.c
 * Scopo: Suite di testing automatizzata per il sistema di gestione del condominio.
 * Fornisce un'interfaccia a riga di comando per eseguire singoli casi di test
 * o l'intera suite, validando il comportamento delle strutture dati tramite assert
 * e pattern "Golden Master" (confronto con file Oracolo).
 * L'information hiding e' rigorosamente mantenuto.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "entita/richiesta.h"
#include "entita/tecnico.h"
#include "archivioRichieste.h"
#include "codaPriorita.h"
#include "alberoTecnici.h"
#include "main/utilita.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

/*
 * Variabile globale per indirizzare l'output dei visitatori su file.
 * Inizializzata a NULL e impostata prima di ogni visita, resettata dopo fclose.
 */
FILE* fileTempOracolo = NULL;

/* Prototipi */
void testCase1_RegistrazioneRichieste(void);
void testCase2_RegistrazioneTecnici(void);
void testCase3_AssegnazioneCorretta(void);
void testCase4_PianificazioneConflitti(void);

/*
 * Funzione: stampaRichiestaSuFile
 * -------------------------------
 * Visitatore per le richieste. Estrae i dati tramite getter (Information Hiding)
 * e li formatta scrivendoli nel file temporaneo puntato da fileTempOracolo.
 *
 * Parametri:
 * r - Puntatore alla Richiesta da stampare (puo' essere NULL).
 *
 * Pre-condizione:
 * La variabile globale fileTempOracolo deve puntare a un file aperto in
 * modalita' scrittura ("w").
 *
 * Post-condizione:
 * I dati vengono accodati nel file. La richiesta non viene modificata in
 * alcun modo.
 *
 * Ritorna:
 * Niente (void).
 */
void stampaRichiestaSuFile(Richiesta* r) {
    if (fileTempOracolo != NULL && r != NULL) {
        fprintf(fileTempOracolo, "Codice: %s, Appartamento: %s, Tipologia: %s, Urgenza: %d\n",
                getCodiceRichiesta(r), getAppartamentoRichiesta(r),
                getTipologiaProblemaRichiesta(r), getLivelloUrgenzaRichiesta(r));
    }
}

/*
 * Funzione: stampaTecnicoSuFile
 * ------------------------------
 * Visitatore compatibile con la visita in-order dell'albero dei tecnici.
 * Estrae i dati tramite getter (Information Hiding) e li formatta
 * scrivendoli nel file puntato dalla variabile globale fileTempOracolo.
 *
 * Parametri:
 * t - Puntatore al Tecnico da stampare (puo' essere NULL).
 *
 * Pre-condizione:
 * La variabile globale fileTempOracolo deve puntare a un file aperto in
 * modalita' scrittura ("w").
 *
 * Post-condizione:
 * I dati vengono accodati nel file. L'oggetto Tecnico e l'albero non
 * subiscono alcuna modifica.
 *
 * Ritorna:
 * Niente (void).
 */
void stampaTecnicoSuFile(Tecnico* t) {
    if (fileTempOracolo != NULL && t != NULL) {
        fprintf(fileTempOracolo, "Codice: %s | Nome: %s | Specializzazione: %s\n",
                getCodiceTecnico(t), getNomeTecnico(t), getSpecializzazioneTecnico(t));
    }
}

/*
 * Funzione: main
 * --------------
 * Punto di ingresso per la suite di testing. Gestisce il menu interattivo
 * per selezionare e avviare i singoli casi di test o l'esecuzione completa.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * L'ambiente deve supportare input/output standard e sequenze escape ANSI.
 *
 * Post-condizione:
 * Il programma termina con la selezione dello "0".
 *
 * Ritorna:
 * 0 al completamento corretto del programma.
 */
int main(void) {
    int scelta = -1;
    setvbuf(stdout, NULL, _IONBF, 0);

    while (scelta != 0) {
        pulisciSchermo();
        printf(CYAN BOLD " ________________________________________________________________________________________ \n");
        printf("|                                                                                        |\n");
        printf("|                  " RESET BOLD "SUITE DI TESTING AUTOMATIZZATA (CON ASSERT)" CYAN BOLD "                           |\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [1] Reg. Richieste (da file)              " CYAN BOLD "|" RESET " [5] Aggiornamento Stato                    " CYAN BOLD "|\n");
        printf("|" RESET " [2] Reg. Tecnici (da file)                " CYAN BOLD "|" RESET " [6] Ricerca e Filtri                       " CYAN BOLD "|\n");
        printf("|" RESET " [3] Assegnazione Corretta                 " CYAN BOLD "|" RESET " [7] Storico Interventi                     " CYAN BOLD "|\n");
        printf("|" RESET " [4] Pianificazione e Conflitti            " CYAN BOLD "|" RESET " [8] Generazione Report                     " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [9] Esegui TUTTI i Test in Sequenza       " CYAN BOLD "|" YELLOW BOLD " [0] Esci dalla Suite                       " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n" RESET);
        printf(BOLD YELLOW "\n>> Seleziona il caso di test da avviare: " RESET);

        if (scanf("%d", &scelta) != 1) {
            pulisciBuffer();
            scelta = -1;
            continue;
        }
        pulisciBuffer();

        switch (scelta) {
            case 1: pulisciSchermo(); testCase1_RegistrazioneRichieste(); pausaSchermo(); break;
            case 2: pulisciSchermo(); testCase2_RegistrazioneTecnici();   pausaSchermo(); break;
            case 3: pulisciSchermo(); testCase3_AssegnazioneCorretta();   pausaSchermo(); break;
            case 4: pulisciSchermo(); testCase4_PianificazioneConflitti(); pausaSchermo(); break;
            case 9:
                pulisciSchermo(); testCase1_RegistrazioneRichieste(); pausaSchermo();
                pulisciSchermo(); testCase2_RegistrazioneTecnici();   pausaSchermo();
                pulisciSchermo(); testCase3_AssegnazioneCorretta();   pausaSchermo();
                pulisciSchermo(); testCase4_PianificazioneConflitti(); pausaSchermo();
                break;
            case 5: case 6: case 7: case 8:
                printf(YELLOW "\n [ IN ATTESA ] Test %d da implementare...\n" RESET, scelta);
                pausaSchermo();
                break;
            case 0: break;
            default:
                printf(RED BOLD "\n [ ERRORE ] Opzione non valida.\n" RESET);
                pausaSchermo();
                break;
        }
    }
    return 0;
}

/*
 * Funzione: testCase1_RegistrazioneRichieste
 * ------------------------------------------
 * Testa il caricamento delle richieste da file e l'inserimento
 * nell'Archivio Lista e nella Coda di Priorita' (Max-Heap). Estrae gli
 * elementi ordinati, li scrive in un file temporaneo e li confronta
 * con il file Oracolo noto.
 *
 * FIX applicati:
 * - Ordine di distruzione corretto: heap svuotato prima, poi archivio.
 * - assert sostituito con if+return su operazioni di I/O/filesystem.
 * - File descriptor chiuso prima di ogni uscita anticipata.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_1.txt"
 * devono essere disponibili.
 *
 * Post-condizione:
 * Il file temporaneo generato "output_1.txt" viene eliminato. Tutte le
 * strutture allocate vengono distrutte senza provocare memory leak.
 *
 * Ritorna:
 * Niente (void).
 */
void testCase1_RegistrazioneRichieste(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 1 ] Verifica Registrazione Richieste (Oracolo)                  |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    ArchivioRichieste* archivio = creaArchivioRichieste();
    CodaPriorita*      coda     = creaCodaPriorita(50);

    if (archivio == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda)    distruggiCodaPriorita(coda);
        if (archivio) distruggiArchivioRichieste(archivio);
        return;
    }

    assert(getDimensioneArchivio(archivio) == 0);

    printf(CYAN "\n > Caricamento dati da 'richieste.txt'...\n" RESET);
    int caricate = caricaRichiesteDaFile(archivio, coda, "test/data/richieste.txt");

    if (caricate <= 0) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Nessuna richiesta caricata da file. Verificare il path.\n" RESET);
        /* Svuota l'heap prima di distruggere: l'archivio e' owner delle Richiesta* */
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(archivio);
        return;
    }

    printf(CYAN " > Estrazione dall'Heap e generazione output...\n" RESET);

    fileTempOracolo = fopen("test/oracoli/output_1.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_1.txt.\n" RESET);
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(archivio);
        return;
    }

    while (!isCodaPrioritaVuota(coda)) {
        Richiesta* r = estraiMaxDaCodaPriorita(coda);
        if (r != NULL) stampaRichiestaSuFile(r);
    }

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_1.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_1.txt", "test/oracoli/oracolo_test_1.txt");

    /* assert(match == 1): il programma SI BLOCCA se l'oracolo non combacia */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.1 superato con successo! L'ordine dell'Heap combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_1.txt");
    /* Ordine corretto: heap gia' vuoto dopo estrazione, poi archivio owner delle Richiesta* */
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(archivio);
}

/*
 * Funzione: testCase2_RegistrazioneTecnici
 * ----------------------------------------
 * Testa il caricamento dei tecnici e l'inserimento nell'Albero Binario (BST).
 * Genera un file di output navigando l'albero in-order e lo confronta
 * con il relativo Oracolo per validare il corretto ordinamento lessicografico.
 *
 * FIX applicati:
 * - assert sostituito con if+return su operazioni di I/O/filesystem.
 * - File descriptor chiuso prima di ogni uscita anticipata.
 * - fileTempOracolo resettato a NULL dopo ogni fclose.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/tecnici.txt" e "test/oracoli/oracolo_test_2.txt"
 * devono esistere ed essere leggibili.
 *
 * Post-condizione:
 * Il file temporaneo generato "output_2.txt" viene eliminato. L'albero
 * viene deallocato integralmente a fine test.
 *
 * Ritorna:
 * Niente (void).
 */
void testCase2_RegistrazioneTecnici(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 2 ] Verifica Registrazione Tecnici (Oracolo)                    |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    AlberoTecnici* db = creaAlberoTecnici();

    if (db == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione albero tecnici fallita.\n" RESET);
        return;
    }

    assert(getRadiceAlberoTecnici(db) == NULL);

    printf(CYAN "\n > Caricamento dati da 'tecnici.txt'...\n" RESET);
    int caricati = caricaTecniciDaFile(db, "test/data/tecnici.txt");

    if (caricati <= 0) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Nessun tecnico caricato da file. Verificare il path.\n" RESET);
        distruggiAlberoTecnici(db);
        return;
    }

    printf(CYAN " > Generazione dell'output e visita dell'albero...\n" RESET);

    fileTempOracolo = fopen("test/oracoli/output_2.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_2.txt.\n" RESET);
        distruggiAlberoTecnici(db);
        return;
    }

    visitaAlberoTecnici(db, stampaTecnicoSuFile);

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_2.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_2.txt", "test/oracoli/oracolo_test_2.txt");

    /* assert(match == 1): il programma SI BLOCCA se l'oracolo non combacia */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.2 superato con successo! L'output del BST combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_2.txt");
    distruggiAlberoTecnici(db);
}

/*
 * Funzione: testCase3_AssegnazioneCorretta
 * ----------------------------------------
 * Testa il flusso di assegnazione simulando l'estrazione della richiesta
 * a massima urgenza e la ricerca del tecnico compatibile nel BST.
 * Verifica stato post-assegnazione tramite oracolo. Il programma si blocca
 * con assert se il confronto fallisce (comportamento atteso dalla suite).
 *
 * FIX applicati:
 * - Logica assegnazione con setter diretti (identica all'originale).
 * - assert(match == 1): il programma si BLOCCA se l'oracolo non combacia.
 * - Ordine di distruzione corretto: heap svuotato, poi distrutto, poi archivio.
 * - if+return solo su allocazioni e fopen (errori di sistema, non logici).
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * Tutti i file di input (richieste/tecnici) e l'Oracolo devono esistere.
 *
 * Post-condizione:
 * Memoria e file deallocati a fine operazione, nessun dangling pointer.
 *
 * Ritorna:
 * Niente (void).
 */
void testCase3_AssegnazioneCorretta(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 3 ] Verifica Assegnazione Corretta (Oracolo)                    |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    AlberoTecnici*    db      = creaAlberoTecnici();
    ArchivioRichieste* archivio = creaArchivioRichieste();
    CodaPriorita*     coda    = creaCodaPriorita(50);

    if (db == NULL || archivio == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda)     distruggiCodaPriorita(coda);
        if (archivio) distruggiArchivioRichieste(archivio);
        if (db)       distruggiAlberoTecnici(db);
        return;
    }

    printf(CYAN "\n > Inizializzazione ambiente e caricamento dati...\n" RESET);
    caricaTecniciDaFile(db, "test/data/tecnici.txt");
    caricaRichiesteDaFile(archivio, coda, "test/data/richieste.txt");

    Richiesta* r = estraiMaxDaCodaPriorita(coda);
    Tecnico*   t = (r != NULL)
                   ? trovaTecnicoDisponibilePerSpecializzazione(db, getTipologiaProblemaRichiesta(r))
                   : NULL;

    /* Invarianti logiche: assert blocca il programma se i dati non sono disponibili */
    assert(r != NULL);
    assert(t != NULL);

    /* Assegnazione con setter diretti — identica alla logica originale */
    setCodiceTecnicoAssegnatoRichiesta(r, getCodiceTecnico(t));
    setStatoRichiesta(r, PIANIFICATA);
    setDisponibilitaTecnico(t, 0);

    /* Verifica invarianti post-assegnazione */
    assert(getStatoRichiesta(r) == PIANIFICATA);
    assert(getCodiceTecnicoAssegnatoRichiesta(r) != NULL);
    assert(isDisponibileTecnico(t) == 0);

    printf(CYAN " > Generazione stato post-assegnazione su file...\n" RESET);

    fileTempOracolo = fopen("test/oracoli/output_3.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_3.txt.\n" RESET);
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(archivio);
        distruggiAlberoTecnici(db);
        return;
    }

    /* Formato identico all'originale per garantire il match con l'oracolo */
    fprintf(fileTempOracolo,
            "Richiesta: %s | Assegnata a: %s | Stato: %s | Tecnico Disponibile: %d\n",
            getCodiceRichiesta(r),
            getCodiceTecnicoAssegnatoRichiesta(r),
            statoRichiestaToString(getStatoRichiesta(r)),
            isDisponibileTecnico(t));

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_3.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_3.txt", "test/oracoli/oracolo_test_3.txt");

    /*
     * assert(match == 1): il programma SI BLOCCA se l'oracolo non combacia.
     * Questo e' il comportamento richiesto dalla suite di testing.
     */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.3 superato con successo! L'assegnazione combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_3.txt");

    /*
     * Ordine di distruzione corretto (ownership):
     * 1. Svuota l'heap (puntatori rimasti sono owned dall'archivio).
     * 2. Distruggi il wrapper heap (ora vuoto, nessun dangling).
     * 3. Distruggi l'archivio (libera le Richiesta* effettive).
     * 4. Distruggi l'albero (libera i Tecnico*).
     */
    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(archivio);
    distruggiAlberoTecnici(db);
}

/*
 * Funzione: testCase4_PianificazioneConflitti
 * -------------------------------------------
 * Testa la logica di inserimento appuntamenti nell'Agenda del Tecnico (BST).
 * Verifica che il primo inserimento in una fascia oraria libera abbia successo,
 * e che un secondo tentativo nella stessa fascia (conflitto) venga respinto.
 * I risultati vengono scritti su file e confrontati con l'oracolo.
 *
 * FIX applicati:
 * - assert sostituito con if+return su operazioni di I/O/filesystem.
 * - Ownership di r1/r2 affidata ad arcTemp: distruzione sempre garantita.
 * - File descriptor chiuso e fileTempOracolo = NULL prima di ogni uscita.
 * - Ordine distruzione: arcTemp (owner delle Richiesta*) dopo albero.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/tecnici.txt" e "test/oracoli/oracolo_test_4.txt"
 * devono essere disponibili e corretti.
 *
 * Post-condizione:
 * Memoria allocata per richieste di test e albero liberata senza leak.
 *
 * Ritorna:
 * Niente (void).
 */
void testCase4_PianificazioneConflitti(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 4 ] Verifica Pianificazione e Conflitti Orari                   |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    AlberoTecnici*    db      = creaAlberoTecnici();
    ArchivioRichieste* arcTemp = creaArchivioRichieste();

    if (db == NULL || arcTemp == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (arcTemp) distruggiArchivioRichieste(arcTemp);
        if (db)       distruggiAlberoTecnici(db);
        return;
    }

    printf(CYAN "\n > Inizializzazione ambiente e caricamento dati...\n" RESET);
    caricaTecniciDaFile(db, "test/data/tecnici.txt");

    Tecnico* t = cercaTecnicoInAlbero(db, "T001");
    if (t == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Tecnico T001 non trovato. Verificare il file tecnici.txt.\n" RESET);
        distruggiArchivioRichieste(arcTemp);
        distruggiAlberoTecnici(db);
        return;
    }

    /*
     * Creazione manuale delle richieste di test.
     * L'ownership e' affidata ad arcTemp: distruggiArchivioRichieste
     * chiamera' distruggiRichiesta su r1 e r2 in ogni percorso di uscita.
     */
    Richiesta* r1 = creaRichiesta("R999", "App. 5",  "Idraulico", "Perdita", "16/05/2026", 3);
    Richiesta* r2 = creaRichiesta("R888", "App. 12", "Idraulico", "Tubo",    "16/05/2026", 2);

    if (r1 == NULL || r2 == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione richieste di test fallita.\n" RESET);
        if (r1) distruggiRichiesta(r1);
        if (r2) distruggiRichiesta(r2);
        distruggiArchivioRichieste(arcTemp);
        distruggiAlberoTecnici(db);
        return;
    }

    /* Inserimento in arcTemp: da questo punto la distruzione e' garantita */
    inserisciInCodaArchivio(arcTemp, r1);
    inserisciInCodaArchivio(arcTemp, r2);

    printf(CYAN " > Esecuzione test di pianificazione ed estrazione risultati...\n" RESET);

    fileTempOracolo = fopen("test/oracoli/output_4.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_4.txt.\n" RESET);
        distruggiArchivioRichieste(arcTemp);
        distruggiAlberoTecnici(db);
        return;
    }

    /* PRIMA PIANIFICAZIONE: Riuscita (fascia libera) */
    int p1 = pianificaIntervento(r1, t, "16/05/2026", "10:00-12:00");

    /* assert logici: non dipendono da I/O, safe */
    assert(p1 == 1);
    assert(getStatoRichiesta(r1) == PIANIFICATA);

    fprintf(fileTempOracolo,
            "Tentativo 1 | Richiesta: %s | Tecnico: %s | Fascia: 10:00-12:00 | Esito: SUCCESSO\n",
            getCodiceRichiesta(r1), getCodiceTecnico(t));

    /* SECONDA PIANIFICAZIONE: Fallita (conflitto agenda BST) */
    int p2 = pianificaIntervento(r2, t, "16/05/2026", "10:00-12:00");

    assert(p2 == 0);
    assert(getStatoRichiesta(r2) == APERTA); /* stato NON deve essere mutato */

    fprintf(fileTempOracolo,
            "Tentativo 2 | Richiesta: %s | Tecnico: %s | Fascia: 10:00-12:00 | Esito: CONFLITTO\n",
            getCodiceRichiesta(r2), getCodiceTecnico(t));

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_4.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_4.txt", "test/oracoli/oracolo_test_4.txt");

    /* assert(match == 1): il programma SI BLOCCA se l'oracolo non combacia */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.4 superato con successo! La gestione conflitti combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_4.txt");

    /*
     * Ordine di distruzione corretto:
     * 1. albero tecnici (non e' owner delle richieste).
     * 2. arcTemp (e' owner di r1 e r2, chiama distruggiRichiesta su entrambe).
     */
    distruggiAlberoTecnici(db);
    distruggiArchivioRichieste(arcTemp);
}
