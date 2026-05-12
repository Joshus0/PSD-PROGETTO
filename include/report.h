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

/* Genera e stampa un report completo con tutte le statistiche */
void generaReportCompleto(const ArchivioRichieste* archivio, const AlberoTecnici* albero);

/* Genera e stampa report interventi per tipologia */
void generaReportPerTipologia(const ArchivioRichieste* archivio);

/* Genera e stampa report interventi aperti e chiusi */
void generaReportStatoInterventi(const ArchivioRichieste* archivio);

/* Calcola e stampa il tempo medio di completamento degli interventi */
void generaReportTempoMedio(const ArchivioRichieste* archivio);

/* Trova e stampa il tecnico più attivo */
void generaReportTecnicoPiuAttivo(const ArchivioRichieste* archivio);

/* Genera e stampa report aree con più problemi */
void generaReportAreeProblematiche(const ArchivioRichieste* archivio);

#endif
//Chiusura di #ifndef REPORT_H
