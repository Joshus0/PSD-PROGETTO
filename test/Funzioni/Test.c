/*
 * Implementazione della suite di Test.
 *
 * Raggruppa l'implementazione dettagliata di tutti i casi di test
 * automatizzati per il sistema di gestione del condominio. Le funzioni
 * verificano la corretta interazione tra le strutture dati (Archivio, Coda, BST)
 * e la logica di business, utilizzando pattern "Golden Master" e assert.
 *
 * 22 maggio
 * Raffaele Pio Villani
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

/* Costanti colore (rimosso YELLOW in quanto superfluo in questo modulo) */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
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

    assert(getDimensioneArchivio(archivio) == 0);

    printf(CYAN "\n > Caricamento dati da 'richieste.txt'...\n" RESET);
    int caricate = caricaRichiesteDaFile(archivio, coda, "test/data/richieste.txt");

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

    while (!isCodaPrioritaVuota(coda)) {
        Richiesta* r = estraiMaxDaCodaPriorita(coda);
        if (r != NULL) stampaRichiestaSuFile(r);
    }

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_1.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_1.txt", "test/oracoli/oracolo_test_1.txt");

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.1 superato con successo! L'ordine dell'Heap combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_1.txt");

    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(archivio);
}


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

    printf(CYAN " > Generazione dell'output tramite visita in-order dell'albero...\n" RESET);

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

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.2 superato con successo! L'output del BST combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_2.txt");
    distruggiAlberoTecnici(db);
}


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

    Tecnico* t = (r != NULL)
                   ? trovaTecnicoDisponibilePerSpecializzazione(db, getTipologiaProblemaRichiesta(r))
                   : NULL;

    assert(r != NULL);
    assert(t != NULL);

    setCodiceTecnicoAssegnatoRichiesta(r, getCodiceTecnico(t));
    setStatoRichiesta(r, PIANIFICATA);
    setDisponibilitaTecnico(t, 0);

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

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.3 superato con successo! L'assegnazione combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_3.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(archivio);
    distruggiAlberoTecnici(db);
}


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

    assert(p1 == 1);
    assert(getStatoRichiesta(r1) == PIANIFICATA);

    fprintf(fileTempOracolo,
            "Tentativo 1 | Richiesta: %s | Tecnico: %s | Fascia: 10:00-12:00 | Esito: SUCCESSO\n",
            getCodiceRichiesta(r1), getCodiceTecnico(t));

    int p2 = pianificaIntervento(r2, t, "16/05/2026", "10:00-12:00");

    assert(p2 == 0);
    assert(getStatoRichiesta(r2) == APERTA);

    fprintf(fileTempOracolo,
            "Tentativo 2 | Richiesta: %s | Tecnico: %s | Fascia: 10:00-12:00 | Esito: CONFLITTO\n",
            getCodiceRichiesta(r2), getCodiceTecnico(t));

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_4.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_4.txt", "test/oracoli/oracolo_test_4.txt");

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.4 superato con successo! La gestione conflitti combacia con l'Oracolo.\n" RESET);

    remove("test/oracoli/output_4.txt");

    distruggiAlberoTecnici(db);
    distruggiArchivioRichieste(arcTemp);
}


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

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.5 superato con successo! Transizioni di stato corrette.\n" RESET);

    remove("test/oracoli/output_5.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}


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

    assert(r1 != NULL);
    fprintf(fileTempOracolo, "Ricerca R002: TROVATA -> ");
    stampaRichiestaSuFile(r1);

    Richiesta* r2 = cercaRichiestaPerCodice(arc, "R_INVAL");

    assert(r2 == NULL);
    fprintf(fileTempOracolo, "Ricerca R_INVAL: NON TROVATA\n");

    fclose(fileTempOracolo);
    fileTempOracolo = NULL;

    printf(CYAN " > Confronto con il file 'oracolo_test_6.txt'...\n" RESET);
    int match = confrontaFileOracolo("test/oracoli/output_6.txt", "test/oracoli/oracolo_test_6.txt");

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.6 superato con successo! Il motore di ricerca funziona perfettamente.\n" RESET);

    remove("test/oracoli/output_6.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}


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

    NodoLista* nodoCorrente = getTestaArchivio(arc);
    int contatore = 0;
    while (nodoCorrente != NULL) {
        Richiesta* req = getRichiestaDalNodoLista(nodoCorrente);

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

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.7 superato con successo! Il filtro dello storico e' esatto.\n" RESET);

    remove("test/oracoli/output_7.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}


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

    assert(match == 1);

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.8 superato con successo! Le statistiche aggregate sono corrette.\n" RESET);

    remove("test/oracoli/output_8.txt");

    while (!isCodaPrioritaVuota(coda)) estraiMaxDaCodaPriorita(coda);
    distruggiCodaPriorita(coda);
    distruggiArchivioRichieste(arc);
}


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

    setCodiceTecnicoAssegnatoRichiesta(r1, getCodiceTecnico(t));
    setStatoRichiesta(r1, PIANIFICATA);
    assert(getStatoRichiesta(r1) == PIANIFICATA);
    printf(GREEN "  [OK] R1 assegnata, stato: PIANIFICATA\n" RESET);

    setCodiceTecnicoAssegnatoRichiesta(r2, getCodiceTecnico(t));
    setStatoRichiesta(r2, PIANIFICATA);
    assert(getStatoRichiesta(r2) == PIANIFICATA);
    printf(GREEN "  [OK] R2 assegnata, stato: PIANIFICATA\n" RESET);

    printf(CYAN " > Fase 2: Pianificazione primo intervallo (mattina 09:00-12:00)...\n" RESET);

    int p1 = pianificaIntervento(r1, t, "20/05/2026", "09:00-12:00");
    assert(p1 == 1);
    assert(getStatoRichiesta(r1) == PIANIFICATA);
    assert(getDataInizioLavorazioneRichiesta(r1) != NULL);
    assert(getFasciaOrariaRichiesta(r1) != NULL);
    printf(GREEN "  [OK] R1 pianificata, stato: PIANIFICATA, fascia: 09:00-12:00\n" RESET);

    printf(CYAN " > Fase 3: Pianificazione secondo intervallo (pomeriggio 14:00-17:00 - STESSO GIORNO)...\n" RESET);

    int p2 = pianificaIntervento(r2, t, "20/05/2026", "14:00-17:00");
    assert(p2 == 1);
    assert(getStatoRichiesta(r2) == PIANIFICATA);
    assert(getDataInizioLavorazioneRichiesta(r2) != NULL);
    assert(getFasciaOrariaRichiesta(r2) != NULL);
    printf(GREEN "  [OK] R2 pianificata, stato: PIANIFICATA, fascia: 14:00-17:00\n" RESET);

    printf(CYAN " > Fase 4: Verifica agenda tecnico (contiene entrambi gli interventi)...\n" RESET);

    AgendaTecnico* agenda = getAgendaTecnico(t);
    int numInterventi = getNumeroInterventiAgenda(agenda);
    printf(CYAN "  Numero interventi in agenda: %d\n" RESET, numInterventi);
    assert(numInterventi == 2);
    printf(GREEN "  [OK] Agenda contiene esattamente 2 interventi\n" RESET);

    printf(CYAN " > Fase 5: Verifica assenza di conflitti orari...\n" RESET);

    Richiesta* r3 = creaRichiesta("R_CONFLICT", "App. C", "Idraulico", "Conflitto", "20/05/2026", 1);
    if (r3 != NULL) {
        inserisciInCodaArchivio(arc, r3);
        setCodiceTecnicoAssegnatoRichiesta(r3, getCodiceTecnico(t));
        setStatoRichiesta(r3, PIANIFICATA);

        int p3 = pianificaIntervento(r3, t, "20/05/2026", "09:00-12:00"); 
        assert(p3 == 0); 
        assert(getStatoRichiesta(r3) == PIANIFICATA); 
        printf(GREEN "  [OK] Conflitto rilevato e rifiutato correttamente\n" RESET);
    }

    printf(GREEN BOLD "\n [ SUCCESS ] Test N.9 superato con successo! Multi-slot stesso giorno funziona correttamente.\n" RESET);

    distruggiAlberoTecnici(db);
    distruggiArchivioRichieste(arc);
}