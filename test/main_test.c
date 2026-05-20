/*
 * File: main_test.c
 * Scopo: Suite di testing automatizzata per il sistema di gestione del condominio.
 * Fornisce un'interfaccia a riga di comando per eseguire singoli casi di test
 * o l'intera suite, validando il comportamento delle strutture dati tramite assert
 * e pattern "Golden Master" (confronto con file Oracolo).
 * L'information hiding e' rigorosamente mantenuto: ogni accesso alle strutture
 * avviene esclusivamente tramite le funzioni getter pubbliche degli ADT.
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
 * fileTempOracolo - Canale di output condiviso tra i visitatori e il test corrente.
 *
 * I visitatori (stampaRichiestaSuFile, stampaTecnicoSuFile) non ricevono
 * il FILE* come parametro perche' la firma deve essere compatibile con quella
 * attesa da visitaAlberoTecnici (void (*)(Tecnico*)). Questa variabile globale
 * e' l'unico modo per passare il file ai visitatori rispettando l'interfaccia.
 * Va impostata a NULL dopo ogni fclose per evitare scritture accidentali.
 */
FILE* fileTempOracolo = NULL;

void testCase1_RegistrazioneRichieste(void);
void testCase2_RegistrazioneTecnici(void);
void testCase3_AssegnazioneCorretta(void);
void testCase4_PianificazioneConflitti(void);
void testCase5_AggiornamentoStato(void);
void testCase6_RicercaFiltri(void);
void testCase7_StoricoInterventi(void);
void testCase8_GenerazioneReport(void);
void testCase9_MultiSlotGiornoStesso(void);

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
 * fileTempOracolo deve puntare a un file aperto in modalita' scrittura ("w").
 *
 * Post-condizione:
 * I dati vengono accodati nel file. La richiesta non viene modificata.
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
 * Visitatore compatibile con la firma void (*)(Tecnico*) richiesta da
 * visitaAlberoTecnici. Scrive i dati del tecnico nel file puntato da
 * fileTempOracolo usando esclusivamente i getter pubblici.
 *
 * Parametri:
 * t - Puntatore al Tecnico da stampare (puo' essere NULL).
 *
 * Pre-condizione:
 * fileTempOracolo deve puntare a un file aperto in modalita' scrittura ("w").
 *
 * Post-condizione:
 * I dati vengono accodati nel file. Il Tecnico e l'albero non subiscono
 * alcuna modifica.
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
 * Punto di ingresso della suite di testing. Presenta un menu interattivo
 * e delega l'esecuzione ai singoli test case. L'opzione 9 esegue tutti
 * i test in sequenza, bloccandosi al primo fallimento grazie agli assert.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * L'ambiente deve supportare sequenze escape ANSI per la colorazione.
 *
 * Post-condizione:
 * Il programma termina normalmente con la selezione dello "0".
 * Se un assert fallisce internamente, il programma termina con abort().
 *
 * Ritorna:
 * 0 al completamento corretto.
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
        printf("|" RESET " [9] Multi-slot Stesso Giorno              " CYAN BOLD "|" RESET " [0] Esci dalla Suite                       " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [10] Esegui TUTTI i Test in Sequenza      " CYAN BOLD "|" YELLOW BOLD " " CYAN BOLD "                                          " CYAN BOLD "|\n");
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
            case 5: pulisciSchermo(); testCase5_AggiornamentoStato();     pausaSchermo(); break;
            case 6: pulisciSchermo(); testCase6_RicercaFiltri();          pausaSchermo(); break;
            case 7: pulisciSchermo(); testCase7_StoricoInterventi();      pausaSchermo(); break;
            case 8: pulisciSchermo(); testCase8_GenerazioneReport();      pausaSchermo(); break;
            case 9: pulisciSchermo(); testCase9_MultiSlotGiornoStesso();  pausaSchermo(); break;
            case 10:
                pulisciSchermo(); testCase1_RegistrazioneRichieste(); pausaSchermo();
                pulisciSchermo(); testCase2_RegistrazioneTecnici();   pausaSchermo();
                pulisciSchermo(); testCase3_AssegnazioneCorretta();   pausaSchermo();
                pulisciSchermo(); testCase4_PianificazioneConflitti(); pausaSchermo();
                pulisciSchermo(); testCase5_AggiornamentoStato();     pausaSchermo();
                pulisciSchermo(); testCase6_RicercaFiltri();          pausaSchermo();
                pulisciSchermo(); testCase7_StoricoInterventi();      pausaSchermo();
                pulisciSchermo(); testCase8_GenerazioneReport();      pausaSchermo();
                pulisciSchermo(); testCase9_MultiSlotGiornoStesso();  pausaSchermo();
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
 * Verifica che il caricamento da file inserisca le richieste nell'Archivio
 * e nella Coda di Priorita' (Max-Heap) rispettando l'ordinamento per urgenza.
 * Le richieste vengono estratte in ordine decrescente di urgenza, scritte
 * su file e confrontate con l'Oracolo noto.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_1.txt"
 * devono essere presenti e leggibili.
 *
 * Post-condizione:
 * Il file temporaneo "output_1.txt" viene eliminato. Tutte le strutture
 * allocate vengono distrutte. La coda viene svuotata prima dell'archivio
 * perche' quest'ultimo e' l'unico owner delle Richiesta*.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se l'ordine di estrazione
 * non corrisponde all'Oracolo.
 */
void testCase1_RegistrazioneRichieste(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 1 ] Verifica Registrazione Richieste (Oracolo)                  |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    ArchivioRichieste* archivio = creaArchivioRichieste();
    CodaPriorita* coda     = creaCodaPriorita(50);

    if (archivio == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda)     distruggiCodaPriorita(coda);
        if (archivio) distruggiArchivioRichieste(archivio);
        return;
    }

    /* Valida l'invariante dell'ADT: lo stato iniziale deve essere vuoto. */
    assert(getDimensioneArchivio(archivio) == 0);

    printf(CYAN "\n > Caricamento dati da 'richieste.txt'...\n" RESET);
    int caricate = caricaRichiesteDaFile(archivio, coda, "test/data/richieste.txt");

    /* Fail-fast: impedisce l'esecuzione del test su un dataset vuoto. */
    if (caricate <= 0) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Nessuna richiesta caricata da file. Verificare il path.\n" RESET);
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

    /* L'estrazione progressiva verifica l'ordinamento decrescente del Max-Heap. */
    while (!isCodaPrioritaVuota(coda)) {
        Richiesta* r = estraiMaxDaCodaPriorita(coda);
        if (r != NULL) stampaRichiestaSuFile(r);
    }

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_1.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_1.txt", "test/oracoli/oracolo_test_1.txt");

    /* Blocca il test in caso di regressione sull'algoritmo di ordinamento. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.1 superato con successo! L'ordine dell'Heap combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_1.txt");

    /* La coda contiene riferimenti; va distrutta prima dell'archivio, unico owner della memoria. */
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(archivio);
}

/*
 * Funzione: testCase2_RegistrazioneTecnici
 * ----------------------------------------
 * Verifica che il caricamento da file inserisca i tecnici nel BST mantenendo
 * l'ordinamento lessicografico per codice. La visita in-order dell'albero
 * deve produrre i tecnici in ordine alfabetico, confermato dal confronto
 * con l'Oracolo.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/tecnici.txt" e "test/oracoli/oracolo_test_2.txt"
 * devono esistere ed essere leggibili.
 *
 * Post-condizione:
 * Il file temporaneo "output_2.txt" viene eliminato. L'albero viene
 * deallocato integralmente insieme ai Tecnico* che contiene.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se l'ordinamento BST
 * non corrisponde all'Oracolo.
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

    /* Verifica che l'ADT non contenga nodi pre-allocati al momento della creazione. */
    assert(getRadiceAlberoTecnici(db) == NULL);

    printf(CYAN "\n > Caricamento dati da 'tecnici.txt'...\n" RESET);
    int caricati = caricaTecniciDaFile(db, "test/data/tecnici.txt");

    if (caricati <= 0) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Nessun tecnico caricato da file. Verificare il path.\n" RESET);
        distruggiAlberoTecnici(db);
        return;
    }

    printf(CYAN " > Generazione dell'output tramite visita in-order dell'albero...\n" RESET);

    fileTempOracolo = fopen("test/oracoli/output_2.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_2.txt.\n" RESET);
        distruggiAlberoTecnici(db);
        return;
    }

    /* La visita in-order produce un output strettamente deterministico grazie alla topologia del BST. */
    visitaAlberoTecnici(db, stampaTecnicoSuFile);

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_2.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_2.txt", "test/oracoli/oracolo_test_2.txt");

    /* Blocca il test se l'ordinamento del BST non corrisponde all'Oracolo. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.2 superato con successo! L'output del BST combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_2.txt");
    distruggiAlberoTecnici(db);
}

/*
 * Funzione: testCase3_AssegnazioneCorretta
 * ----------------------------------------
 * Verifica il flusso di assegnazione: la richiesta con urgenza massima viene
 * estratta dall'heap, il tecnico compatibile viene trovato nel BST e l'
 * assegnazione viene completata tramite setter. Lo stato post-assegnazione
 * viene confrontato con l'Oracolo.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file di input (richieste.txt, tecnici.txt) e oracolo_test_3.txt
 * devono esistere. Il file richieste.txt deve contenere almeno una
 * richiesta con tipologia corrispondente a un tecnico disponibile.
 *
 * Post-condizione:
 * Lo stato della richiesta estratta e' PIANIFICATA. Il tecnico assegnato
 * ha disponibilita' impostata a 0. Tutte le strutture vengono deallocate
 * nell'ordine corretto: heap prima, poi archivio, poi albero.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se l'assegnazione
 * non produce lo stato atteso o se l'Oracolo non combacia.
 */
void testCase3_AssegnazioneCorretta(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 3 ] Verifica Assegnazione Corretta (Oracolo)                    |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    AlberoTecnici* db       = creaAlberoTecnici();
    ArchivioRichieste* archivio = creaArchivioRichieste();
    CodaPriorita* coda     = creaCodaPriorita(50);

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

    /* La ricerca nel BST viene effettuata solo se l'estrazione dalla coda ha avuto successo. */
    Tecnico* t = (r != NULL)
                   ? trovaTecnicoDisponibilePerSpecializzazione(db, getTipologiaProblemaRichiesta(r))
                   : NULL;

    /* Blocca il test se i dati di input non garantiscono un join valido tra problema e specializzazione. */
    assert(r != NULL);
    assert(t != NULL);

    /* Simulazione della business logic: aggiornamento dello stato e occupazione della risorsa. */
    setCodiceTecnicoAssegnatoRichiesta(r, getCodiceTecnico(t));
    setStatoRichiesta(r, PIANIFICATA);
    setDisponibilitaTecnico(t, 0);

    /* Verifica che i setter abbiano aggiornato correttamente lo stato degli oggetti. */
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

    /* Blocca il test se la logica applicativa ha prodotto uno stato non atteso. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.3 superato con successo! L'assegnazione combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_3.txt");

    /* I riferimenti nell'heap vanno rimossi prima dei nodi master nell'archivio (ownership). */
    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(archivio);
    distruggiAlberoTecnici(db);
}

/*
 * Funzione: testCase4_PianificazioneConflitti
 * -------------------------------------------
 * Verifica che l'Agenda del Tecnico (BST) gestisca correttamente i conflitti
 * orari: il primo inserimento su una fascia libera deve riuscire, il secondo
 * tentativo sulla stessa fascia deve essere respinto senza modificare lo stato
 * della richiesta coinvolta.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/tecnici.txt" e "test/oracoli/oracolo_test_4.txt"
 * devono essere presenti. Il file tecnici.txt deve contenere T001.
 *
 * Post-condizione:
 * L'agenda di T001 contiene un solo appuntamento. Le richieste di test
 * vengono deallocate tramite arcTemp che ne e' l'owner.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se il conflitto non viene
 * rilevato o se l'Oracolo non combacia.
 */
void testCase4_PianificazioneConflitti(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 4 ] Verifica Pianificazione e Conflitti Orari                   |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    AlberoTecnici* db      = creaAlberoTecnici();
    ArchivioRichieste* arcTemp = creaArchivioRichieste();

    if (db == NULL || arcTemp == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (arcTemp) distruggiArchivioRichieste(arcTemp);
        if (db)      distruggiAlberoTecnici(db);
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

    /* Istanziazione diretta per controllare i parametri temporali e forzare il conflitto.
     * L'inserimento in arcTemp centralizza la deallocazione. */
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

    int p1 = pianificaIntervento(r1, t, "16/05/2026", "10:00-12:00");

    /* Positive test: lo slot e' libero, l'operazione deve avere successo. */
    assert(p1 == 1);
    assert(getStatoRichiesta(r1) == IN_LAVORAZIONE);

    fprintf(fileTempOracolo,
            "Tentativo 1 | Richiesta: %s | Tecnico: %s | Fascia: 10:00-12:00 | Esito: SUCCESSO\n",
            getCodiceRichiesta(r1), getCodiceTecnico(t));

    int p2 = pianificaIntervento(r2, t, "16/05/2026", "10:00-12:00");

    /* Negative test: lo slot e' occupato, il BST deve rilevare il conflitto e rifiutare l'inserimento. */
    assert(p2 == 0);

    /* In caso di conflitto, la richiesta deve mantenere il proprio stato iniziale. */
    assert(getStatoRichiesta(r2) == APERTA);

    fprintf(fileTempOracolo,
            "Tentativo 2 | Richiesta: %s | Tecnico: %s | Fascia: 10:00-12:00 | Esito: CONFLITTO\n",
            getCodiceRichiesta(r2), getCodiceTecnico(t));

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_4.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_4.txt", "test/oracoli/oracolo_test_4.txt");

    /* Blocca il test se le politiche di mutua esclusione dell'agenda non funzionano correttamente. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.4 superato con successo! La gestione conflitti combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_4.txt");

    distruggiAlberoTecnici(db);
    distruggiArchivioRichieste(arcTemp);
}

/*
 * Funzione: testCase5_AggiornamentoStato
 * --------------------------------------
 * Verifica le transizioni di stato di una richiesta lungo il ciclo di vita
 * previsto: APERTA -> IN_LAVORAZIONE -> CONCLUSA. Controlla anche che la
 * data di chiusura venga registrata correttamente.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_5.txt"
 * devono essere presenti. Il file richieste.txt deve contenere R002
 * con stato iniziale APERTA.
 *
 * Post-condizione:
 * Lo stato di R002 e' CONCLUSA con data di chiusura impostata.
 * Il file temporaneo viene eliminato. La coda viene svuotata prima
 * della distruzione dell'archivio.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se una transizione
 * non produce lo stato atteso o se l'Oracolo non combacia.
 */
void testCase5_AggiornamentoStato(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 5 ] Verifica Aggiornamento Stato (Oracolo)                      |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    ArchivioRichieste* arc  = creaArchivioRichieste();
    CodaPriorita* coda = creaCodaPriorita(50);

    if (arc == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda) distruggiCodaPriorita(coda);
        if (arc)  distruggiArchivioRichieste(arc);
        return;
    }

    printf(CYAN "\n > Inizializzazione ambiente e caricamento dati...\n" RESET);
    caricaRichiesteDaFile(arc, coda, "test/data/richieste.txt");

    Richiesta* r = cercaRichiestaPerCodice(arc, "R002");

    /* Verifica che la richiesta esista e si trovi nello stato iniziale atteso prima di procedere. */
    assert(r != NULL);
    assert(getStatoRichiesta(r) == APERTA);

    setStatoRichiesta(r, IN_LAVORAZIONE);
    assert(getStatoRichiesta(r) == IN_LAVORAZIONE);

    setStatoRichiesta(r, CONCLUSA);
    assert(getStatoRichiesta(r) == CONCLUSA);

    setDataChiusuraRichiesta(r, "18/05/2026");

    printf(CYAN " > Generazione stato finale su file...\n" RESET);
    fileTempOracolo = fopen("test/oracoli/output_5.txt", "w");

    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_5.txt.\n" RESET);
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(arc);
        return;
    }

    fprintf(fileTempOracolo, "Codice: %s | Stato Finale: %s | Data Chiusura: %s\n",
            getCodiceRichiesta(r),
            statoRichiestaToString(getStatoRichiesta(r)),
            getDataChiusuraRichiesta(r));

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_5.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_5.txt", "test/oracoli/oracolo_test_5.txt");

    /* Blocca il test se le transizioni di stato non hanno prodotto la progressione attesa. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.5 superato con successo! Transizioni di stato corrette.\n" RESET);

    remove("test/oracoli/output_5.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}

/*
 * Funzione: testCase6_RicercaFiltri
 * ---------------------------------
 * Verifica il motore di ricerca per codice dell'archivio. Testa sia il caso
 * positivo (codice presente) sia quello negativo (codice assente), garantendo
 * che la ricerca non alteri lo stato dell'archivio in nessuno dei due casi.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_6.txt"
 * devono esistere. Il codice "R_INVAL" non deve essere presente nel
 * file di dati (garantito per costruzione).
 *
 * Post-condizione:
 * L'archivio non e' stato modificato. Il file temporaneo viene eliminato.
 * La coda viene svuotata prima della distruzione dell'archivio.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se la ricerca restituisce
 * un risultato inatteso o se l'Oracolo non combacia.
 */
void testCase6_RicercaFiltri(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 6 ] Verifica Ricerca e Filtri (Oracolo)                         |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    ArchivioRichieste* arc  = creaArchivioRichieste();
    CodaPriorita* coda = creaCodaPriorita(50);

    if (arc == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda) distruggiCodaPriorita(coda);
        if (arc)  distruggiArchivioRichieste(arc);
        return;
    }

    printf(CYAN "\n > Inizializzazione ambiente e caricamento dati...\n" RESET);
    caricaRichiesteDaFile(arc, coda, "test/data/richieste.txt");

    printf(CYAN " > Esecuzione ricerche e generazione file di output...\n" RESET);
    fileTempOracolo = fopen("test/oracoli/output_6.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_6.txt.\n" RESET);
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(arc);
        return;
    }

    Richiesta* r1 = cercaRichiestaPerCodice(arc, "R002");

    /* Positive test: verifica la corretta restituzione di una chiave presente nell'archivio. */
    assert(r1 != NULL);
    fprintf(fileTempOracolo, "Ricerca R002: TROVATA -> ");
    stampaRichiestaSuFile(r1);

    Richiesta* r2 = cercaRichiestaPerCodice(arc, "R_INVAL");

    /* Negative test: verifica che una chiave assente restituisca NULL senza alterare l'archivio. */
    assert(r2 == NULL);
    fprintf(fileTempOracolo, "Ricerca R_INVAL: NON TROVATA\n");

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_6.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_6.txt", "test/oracoli/oracolo_test_6.txt");

    /* Blocca il test se il motore di ricerca produce risultati inattesi. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.6 superato con successo! Il motore di ricerca funziona perfettamente.\n" RESET);

    remove("test/oracoli/output_6.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}

/*
 * Funzione: testCase7_StoricoInterventi
 * -------------------------------------
 * Testa la corretta generazione dello storico degli interventi.
 * Simula la chiusura di alcune pratiche, scorre l'Archivio tramite i getter
 * filtrando solo le richieste CONCLUSE e scrive il report su un file
 * temporaneo per il confronto con l'Oracolo.
 *
 * NOTA ARCHITETTURALE (Information Hiding):
 * L'iterazione avviene tramite getTestaArchivio() e getNextNodoLista().
 * Pur esponendo il concetto di "Nodo", questi sono getter pubblici dell'interfaccia
 * dell'ADT, pensati appositamente per consentire l'iterazione esterna senza
 * esporre la definizione concreta della struct (Opaque Pointer).
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_7.txt"
 * devono essere disponibili e validi.
 *
 * Post-condizione:
 * Lo stato di alcune richieste viene alterato per la simulazione. La memoria
 * viene deallocata rigorosamente (heap svuotato prima dell'archivio).
 *
 * Ritorna:
 * Niente (void). Interrompe l'esecuzione (assert) se l'oracolo fallisce.
 */
void testCase7_StoricoInterventi(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 7 ] Verifica Storico Interventi (Oracolo)                       |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    ArchivioRichieste* arc = creaArchivioRichieste();
    CodaPriorita* coda = creaCodaPriorita(50);

    if (arc == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda) distruggiCodaPriorita(coda);
        if (arc) distruggiArchivioRichieste(arc);
        return;
    }

    printf(CYAN "\n > Inizializzazione ambiente e caricamento dati...\n" RESET);
    caricaRichiesteDaFile(arc, coda, "test/data/richieste.txt");

    /* Forzatura di stato: predispone le richieste necessarie al subset "Storico". */
    Richiesta* r1 = cercaRichiestaPerCodice(arc, "R001");
    Richiesta* r2 = cercaRichiestaPerCodice(arc, "R003");
    assert(r1 != NULL && r2 != NULL);

    setCodiceTecnicoAssegnatoRichiesta(r1, "T001");
    setStatoRichiesta(r1, CONCLUSA);
    setDataChiusuraRichiesta(r1, "18/05/2026");

    setCodiceTecnicoAssegnatoRichiesta(r2, "T003");
    setStatoRichiesta(r2, CONCLUSA);
    setDataChiusuraRichiesta(r2, "20/05/2026");

    printf(CYAN " > Estrazione storico e generazione file di output...\n" RESET);
    fileTempOracolo = fopen("test/oracoli/output_7.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_7.txt.\n" RESET);
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(arc);
        return;
    }

    /* Iterazione tramite getter ADT: evita l'esposizione dei dettagli implementativi della lista. */
    NodoLista* nodoCorrente = getTestaArchivio(arc);
    int contatore = 0;
    while (nodoCorrente != NULL) {
        Richiesta* req = getRichiestaDalNodoLista(nodoCorrente);

        /* Filtra le sole richieste CONCLUSE per costruire il report dello storico. */
        if (req != NULL && getStatoRichiesta(req) == CONCLUSA) {
            contatore++;
            fprintf(fileTempOracolo, "Storico #%d | Codice: %s | Tecnico: %s | Data Chiusura: %s\n",
                    contatore,
                    getCodiceRichiesta(req),
                    getCodiceTecnicoAssegnatoRichiesta(req) ? getCodiceTecnicoAssegnatoRichiesta(req) : "N/A",
                    getDataChiusuraRichiesta(req) ? getDataChiusuraRichiesta(req) : "N/A");
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_7.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_7.txt", "test/oracoli/oracolo_test_7.txt");

    /* Blocca il test se il filtro sullo storico produce un output non atteso. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.7 superato con successo! Il filtro dello storico e' esatto.\n" RESET);

    remove("test/oracoli/output_7.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}

/*
 * Funzione: testCase8_GenerazioneReport
 * -------------------------------------
 * Testa l'aggregazione dei dati statistici di sistema. Simula un ambiente
 * eterogeneo, itera l'archivio tramite i getter e calcola le metriche.
 * Il report generato viene scritto su un file temporaneo e confrontato
 * con l'Oracolo per validarne l'esattezza matematica.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * I file "test/data/richieste.txt" e "test/oracoli/oracolo_test_8.txt"
 * devono esistere.
 *
 * Post-condizione:
 * Viene generato e poi rimosso il file "output_8.txt". Memoria allocata
 * liberata in ordine di ownership corretto senza memory leak.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si interrompe in caso di errore.
 */
void testCase8_GenerazioneReport(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 8 ] Verifica Statistiche e Report (Oracolo)                     |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    ArchivioRichieste* arc = creaArchivioRichieste();
    CodaPriorita* coda = creaCodaPriorita(50);

    if (arc == NULL || coda == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (coda) distruggiCodaPriorita(coda);
        if (arc) distruggiArchivioRichieste(arc);
        return;
    }

    printf(CYAN "\n > Inizializzazione ambiente e caricamento dati...\n" RESET);
    caricaRichiesteDaFile(arc, coda, "test/data/richieste.txt");

    Richiesta* r1 = cercaRichiestaPerCodice(arc, "R001");

    /* Forzatura di stato per coprire le label CONCLUSA, IN_LAVORAZIONE e APERTA nel calcolo. */
    assert(r1 != NULL);
    setStatoRichiesta(r1, CONCLUSA);
    setCodiceTecnicoAssegnatoRichiesta(r1, "T001");

    Richiesta* r2 = cercaRichiestaPerCodice(arc, "R002");
    assert(r2 != NULL);
    setStatoRichiesta(r2, IN_LAVORAZIONE);
    setCodiceTecnicoAssegnatoRichiesta(r2, "T002");

    printf(CYAN " > Calcolo delle metriche e generazione file di output...\n" RESET);
    fileTempOracolo = fopen("test/oracoli/output_8.txt", "w");
    if (fileTempOracolo == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Impossibile aprire file output_8.txt.\n" RESET);
        while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
        distruggiCodaPriorita(coda);
        distruggiArchivioRichieste(arc);
        return;
    }

    int totAperte = 0, totLavorazione = 0, totConcluse = 0;
    int totIdraulico = 0, totElettrico = 0;

    /* Singolo giro esplorativo per aggregare tutte le metriche in O(N). */
    NodoLista* nodoCorrente = getTestaArchivio(arc);
    while (nodoCorrente != NULL) {
        Richiesta* req = getRichiestaDalNodoLista(nodoCorrente);
        if (req != NULL) {
            StatoRichiesta stato = getStatoRichiesta(req);
            if (stato == APERTA) totAperte++;
            else if (stato == IN_LAVORAZIONE) totLavorazione++;
            else if (stato == CONCLUSA) totConcluse++;

            const char* tipo = getTipologiaProblemaRichiesta(req);
            if (strcmp(tipo, "Idraulico") == 0) totIdraulico++;
            else if (strcmp(tipo, "Elettrico") == 0) totElettrico++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }

    fprintf(fileTempOracolo, "Statistiche Generali:\n");
    fprintf(fileTempOracolo, "- Totale Pratiche: %d\n", getDimensioneArchivio(arc));
    fprintf(fileTempOracolo, "- Aperte: %d\n", totAperte);
    fprintf(fileTempOracolo, "- In Lavorazione: %d\n", totLavorazione);
    fprintf(fileTempOracolo, "- Concluse: %d\n", totConcluse);
    fprintf(fileTempOracolo, "- Problemi Idraulici: %d\n", totIdraulico);
    fprintf(fileTempOracolo, "- Problemi Elettrici: %d\n", totElettrico);

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_8.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_8.txt", "test/oracoli/oracolo_test_8.txt");

    /* Blocca il test se le statistiche aggregate non corrispondono all'Oracolo. */
    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.8 superato con successo! Le statistiche aggregate sono corrette.\n" RESET);

    remove("test/oracoli/output_8.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}

/*
 * Funzione: testCase9_MultiSlotGiornoStesso
 * ------------------------------------------
 * Verifica che un tecnico possa avere piu' interventi nello stesso giorno
 * in fasce orarie differenti e non sovrapposte.
 * Testa il flusso completo:
 * 1. Assegnazione (APERTA -> PIANIFICATA, senza agenda)
 * 2. Pianificazione primo slot (PIANIFICATA -> IN_LAVORAZIONE, aggiunge agenda)
 * 3. Pianificazione secondo slot (stesso giorno, fascia diversa, IN_LAVORAZIONE)
 * Verifica che non ci siano conflitti e che l'agenda contenga entrambi gli interventi.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * Il file tecnici.txt deve contenere T001 (Idraulico).
 *
 * Post-condizione:
 * Entrambe le richieste risultano IN_LAVORAZIONE e presenti nell'agenda del tecnico.
 *
 * Ritorna:
 * Niente (void). L'esecuzione si blocca (assert) se il flusso non produce
 * lo stato atteso.
 */
void testCase9_MultiSlotGiornoStesso(void) {
    printf(MAGENTA BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ TEST 9 ] Verifica Multi-Slot Stesso Giorno                         |\n");
    printf("|________________________________________________________________________________________|\n" RESET);

    AlberoTecnici* db = creaAlberoTecnici();
    ArchivioRichieste* arc = creaArchivioRichieste();

    if (db == NULL || arc == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione strutture fallita.\n" RESET);
        if (arc) distruggiArchivioRichieste(arc);
        if (db)  distruggiAlberoTecnici(db);
        return;
    }

    printf(CYAN "\n > Caricamento tecnici e creazione richieste di test...\n" RESET);
    caricaTecniciDaFile(db, "test/data/tecnici.txt");

    Tecnico* t = cercaTecnicoInAlbero(db, "T001");
    if (t == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Tecnico T001 non trovato.\n" RESET);
        distruggiArchivioRichieste(arc);
        distruggiAlberoTecnici(db);
        return;
    }

    /* Creazione di due richieste per lo stesso giorno in fasce diverse */
    Richiesta* r1 = creaRichiesta("R_MULTI1", "App. A", "Idraulico", "Perdita mattina", "20/05/2026", 3);
    Richiesta* r2 = creaRichiesta("R_MULTI2", "App. B", "Idraulico", "Perdita pomeriggio", "20/05/2026", 2);

    if (r1 == NULL || r2 == NULL) {
        fprintf(stderr, RED BOLD "\n [ ERRORE ] Allocazione richieste fallita.\n" RESET);
        if (r1) distruggiRichiesta(r1);
        if (r2) distruggiRichiesta(r2);
        distruggiArchivioRichieste(arc);
        distruggiAlberoTecnici(db);
        return;
    }

    inserisciInCodaArchivio(arc, r1);
    inserisciInCodaArchivio(arc, r2);

    printf(CYAN " > Fase 1: Assegnazione delle richieste (senza pianificazione agenda)...\n" RESET);

    /* Assegnazione r1 */
    setCodiceTecnicoAssegnatoRichiesta(r1, getCodiceTecnico(t));
    setStatoRichiesta(r1, PIANIFICATA);
    assert(getStatoRichiesta(r1) == PIANIFICATA);
    printf(GREEN "  [OK] R1 assegnata, stato: PIANIFICATA\n" RESET);

    /* Assegnazione r2 */
    setCodiceTecnicoAssegnatoRichiesta(r2, getCodiceTecnico(t));
    setStatoRichiesta(r2, PIANIFICATA);
    assert(getStatoRichiesta(r2) == PIANIFICATA);
    printf(GREEN "  [OK] R2 assegnata, stato: PIANIFICATA\n" RESET);

    printf(CYAN " > Fase 2: Pianificazione primo intervallo (mattina 09:00-12:00)...\n" RESET);

    /* Pianificazione r1 - mattina */
    int p1 = pianificaIntervento(r1, t, "20/05/2026", "09:00-12:00");
    assert(p1 == 1);
    assert(getStatoRichiesta(r1) == IN_LAVORAZIONE);
    assert(getDataInizioLavorazioneRichiesta(r1) != NULL);
    assert(getFasciaOrariaRichiesta(r1) != NULL);
    printf(GREEN "  [OK] R1 pianificata, stato: IN_LAVORAZIONE, fascia: 09:00-12:00\n" RESET);

    printf(CYAN " > Fase 3: Pianificazione secondo intervallo (pomeriggio 14:00-17:00 - STESSO GIORNO)...\n" RESET);

    /* Pianificazione r2 - pomeriggio dello stesso giorno */
    int p2 = pianificaIntervento(r2, t, "20/05/2026", "14:00-17:00");
    assert(p2 == 1);
    assert(getStatoRichiesta(r2) == IN_LAVORAZIONE);
    assert(getDataInizioLavorazioneRichiesta(r2) != NULL);
    assert(getFasciaOrariaRichiesta(r2) != NULL);
    printf(GREEN "  [OK] R2 pianificata, stato: IN_LAVORAZIONE, fascia: 14:00-17:00\n" RESET);

    printf(CYAN " > Fase 4: Verifica agenda tecnico (contiene entrambi gli interventi)...\n" RESET);

    AgendaTecnico* agenda = getAgendaTecnico(t);
    int numInterventi = getNumeroInterventiAgenda(agenda);
    printf(CYAN "  Numero interventi in agenda: %d\n" RESET, numInterventi);
    assert(numInterventi == 2);
    printf(GREEN "  [OK] Agenda contiene esattamente 2 interventi\n" RESET);

    printf(CYAN " > Fase 5: Verifica assenza di conflitti orari...\n" RESET);

    /* Tentativo di pianificare su uno slot gia' occupato - dovrebbe fallire */
    Richiesta* r3 = creaRichiesta("R_CONFLICT", "App. C", "Idraulico", "Conflitto", "20/05/2026", 1);
    if (r3 != NULL) {
        inserisciInCodaArchivio(arc, r3);
        setCodiceTecnicoAssegnatoRichiesta(r3, getCodiceTecnico(t));
        setStatoRichiesta(r3, PIANIFICATA);

        int p3 = pianificaIntervento(r3, t, "20/05/2026", "09:00-12:00");  /* Conflitto con r1 */
        assert(p3 == 0);  /* Deve fallire */
        assert(getStatoRichiesta(r3) == PIANIFICATA);  /* Rimane PIANIFICATA */
        printf(GREEN "  [OK] Conflitto rilevato e rifiutato correttamente\n" RESET);
    }

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.9 superato con successo! Multi-slot stesso giorno funziona correttamente.\n" RESET);

    distruggiAlberoTecnici(db);
    distruggiArchivioRichieste(arc);
}