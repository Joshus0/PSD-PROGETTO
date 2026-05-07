#ifndef TECNICO_H //Guardia di inclusione per evitare inclusioni multiple
#define TECNICO_H

#include "agendaTecnico.h"
typedef struct Tecnico Tecnico; //Forward declaration della struttura Tecnico presente in tecnico.c, utilizzata per rappresentare un tecnico con le sue informazioni e disponibilità

//Dichiarazione delle funzioni per la gestione dei tecnici
Tecnico* creaTecnico(const char* codice, const char* nome, const char* specializzazione);
void distruggiTecnico(Tecnico* tecnicoTarget);

//Funzioni getter per accedere ai campi di un tecnico
const char* getCodiceTecnico(const Tecnico* tecnicoTarget);
const char* getNomeTecnico(const Tecnico* tecnicoTarget);
const char* getSpecializzazioneTecnico(const Tecnico* tecnicoTarget);
int isDisponibileTecnico(const Tecnico* tecnicoTarget);
AgendaTecnico* getAgendaTecnico(const Tecnico* tecnicoTarget);

//Funzioni setter per modificare i campi di un tecnico
void setDisponibilitaTecnico(Tecnico* tecnicoTarget, int flagDisponibilita);
void setSpecializzazioneTecnico(Tecnico* tecnicoTarget, const char* nuovaSpecializzazione);

#endif
//Chiusura di #ifndef TECNICO_H