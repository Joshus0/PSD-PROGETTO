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

#endif