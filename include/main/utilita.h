#ifndef UTILITA_H
#define UTILITA_H

#include <stdio.h>
#include "entita/richiesta.h"
#include "entita/tecnico.h"
#include "alberoTecnici.h"
#include "agendaTecnico.h"
#include "codaPriorita.h"
#include "archivioRichieste.h"
#define BOLD "\033[1m"
#define RESET "\033[0m"

typedef struct ArchivioRichieste ArchivioRichieste;
typedef struct AlberoTecnici AlberoTecnici;
typedef struct CodaPriorita CodaPriorita;
/* Ritorna una copia allocata dinamicamente di src, 
   oppure NULL se src è NULL o se malloc fallisce */
char* duplicaStringa(const char* src);

int validaData(const char* data);

void pulisciBuffer(void);
void pulisciSchermo(void);
void pausaSchermo(void);
void acquisisciStringa(const char* prompt, char* buffer, int dim);

const char* statoRichiestaToString(StatoRichiesta stato);

void stampaTecnico(Tecnico* tecnico);
void stampaRichiesta(const Richiesta* richiesta);
Tecnico* trovaTecnicoDisponibilePerSpecializzazione(const AlberoTecnici* albero, const char* specializzazione);

/* Nuove funzioni per pianificazione, ricerca e reportistica */
int pianificaIntervento(Richiesta* richiesta, Tecnico* tecnico, const char* data, const char* fasciaOraria);
void stampaStoricoInterventi(const ArchivioRichieste* archivio);
void stampaReportStatistiche(const ArchivioRichieste* archivio, const AlberoTecnici* albero);
void stampaAgendaTecnico(Tecnico* tecnico);
void stampaCaricoLavoroTecnico(Tecnico* t);

/* Funzioni per il caricamento dati da file */
int caricaTecniciDaFile(AlberoTecnici* albero, const char* percorsoFile);
int caricaRichiesteDaFile(ArchivioRichieste* archivio, CodaPriorita* coda, const char* percorsoFile);
/*Funzione per il confrontro oracoli*/
int confrontaFileOracolo(const char* fileOutput, const char* fileOracolo);


#endif