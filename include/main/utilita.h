#ifndef UTILITA_H
#define UTILITA_H

#include <stdio.h>
#include "entita/richiesta.h"
#include "entita/tecnico.h"
#include "alberoTecnici.h"
#define BOLD "\033[1m"
#define RESET "\033[0m"

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

#endif