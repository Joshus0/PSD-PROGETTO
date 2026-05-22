#ifndef REPORT_H
#define REPORT_H

#include "archivioRichieste.h"
#include "alberoTecnici.h"

/* Strutture di supporto per i report */

typedef struct {
    char* tipologia;
    int numeroInterventi;
} RiportoTipologia;

typedef struct {
    char* appartamento;
    int numeroProblemi;
} RiportoArea;

typedef struct {
    char* codiceTecnico;
    char* nomeTecnico;
    int numeroInterventi;
} RiportoTecnico;

typedef struct {
    int interventiAperti;
    int interventiChiusi;
    int interventiTotali;
    float percentualeChiusura;
} RiportoStato;

typedef struct {
    float tempoMedio;
    int interventiCompletati;
} RiportoTempoMedio;

/* Dichiarazione delle funzioni di report */

/*
 * Funzione: generaReportCompleto
 * -------------------------------
 * Genera e stampa su stdout un report completo e strutturato con tutte
 * le statistiche del sistema di gestione degli interventi, includendo
 * metriche sulle richieste, i tecnici, le aree problematiche e i tempi
 * medi di risoluzione. Utilizza formattazione ANSI per la leggibilita'.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *   albero   - Albero dei tecnici da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso in cui archivio o albero siano NULL.
 *
 * Post-condizione:
 *   Un report completo viene scritto su stdout. L'archivio e l'albero
 *   non vengono modificati.
 *
 * Ritorna:
 *   Niente (void).
 */
void generaReportCompleto(const ArchivioRichieste* archivio, const AlberoTecnici* albero);

/*
 * Funzione: generaReportPerTipologia
 * -----------------------------------
 * Scorre l'archivio e genera un report che raggruppa e conta gli interventi
 * per tipologia di problema (es. "Idraulico", "Elettrico"). Stampa su stdout
 * una tabella riepilogativa con il numero di interventi per ogni categoria.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL.
 *
 * Post-condizione:
 *   Un report per tipologia viene scritto su stdout. L'archivio
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void generaReportPerTipologia(const ArchivioRichieste* archivio);

/*
 * Funzione: generaReportStatoInterventi
 * ------------------------------------
 * Calcola e stampa le statistiche aggregate sullo stato degli interventi:
 * numero di interventi aperti, chiusi, e il totale, insieme alla
 * percentuale di completamento. Fornisce una visione d'insieme sulla
 * situazione complessiva del carico di lavoro.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL.
 *
 * Post-condizione:
 *   Un report sugli stati viene scritto su stdout. L'archivio
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void generaReportStatoInterventi(const ArchivioRichieste* archivio);

/*
 * Funzione: generaReportTempoMedio
 * --------------------------------
 * Calcola il tempo medio di completamento degli interventi conclusi,
 * considerando la differenza tra la data di chiusura e la data di
 * apertura della richiesta. Stampa il risultato in giorni con il numero
 * di interventi su cui il calcolo si basa.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL o mancanza di interventi conclusi.
 *
 * Post-condizione:
 *   Un report sul tempo medio viene scritto su stdout. L'archivio
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void generaReportTempoMedio(const ArchivioRichieste* archivio);

/*
 * Funzione: generaReportTecnicoPiuAttivo
 * --------------------------------
 * Analizza l'archivio per identificare il tecnico con il maggior numero
 * di interventi completati, sia in termini assoluti che di specialita'.
 * Stampa un report con nome, codice e numero di interventi assegnati.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL o mancanza di interventi assegnati.
 *
 * Post-condizione:
 *   Un report sul tecnico più attivo viene scritto su stdout. L'archivio
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void generaReportTecnicoPiuAttivo(const ArchivioRichieste* archivio);

/*
 * Funzione: generaReportAreeProblematiche
 * ----------------------------------------
 * Analizza l'archivio e raggruppa gli interventi per appartamento/area
 * di provenienza, ordinando le aree per numero di problemi riscontrati
 * in ordine decrescente. Stampa un elenco delle aree più critiche.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL.
 *
 * Post-condizione:
 *   Un report sulle aree problematiche viene scritto su stdout. L'archivio
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void generaReportAreeProblematiche(const ArchivioRichieste* archivio);

#endif
//Chiusura di #ifndef REPORT_H
