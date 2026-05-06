#include "../../include/entita/tecnico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Tecnico {
    char* codice;
    char* nome;
    char* specializzazione;
    int disponibile;
};

static char* duplicaStringa(const char* src) {
    char* copia;
    if (src == NULL) return NULL;
    copia = (char*)malloc(strlen(src) + 1);
    if (copia != NULL) {
        strcpy(copia, src);
    }
    return copia;
}

Tecnico* creaTecnico(const char* codice, const char* nome, const char* specializzazione) {
    Tecnico* nuovo = (Tecnico*)malloc(sizeof(Tecnico));
    if (nuovo == NULL) return NULL;

    nuovo->codice = duplicaStringa(codice);
    nuovo->nome = duplicaStringa(nome);
    nuovo->specializzazione = duplicaStringa(specializzazione);
    nuovo->disponibile = 1;

    return nuovo;
}

void distruggiTecnico(Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return;

    free(tecnicoTarget->codice);
    free(tecnicoTarget->nome);
    free(tecnicoTarget->specializzazione);
    
    free(tecnicoTarget);
}

const char* getCodiceTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->codice;
}

const char* getNomeTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->nome;
}

const char* getSpecializzazioneTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->specializzazione;
}

int isDisponibileTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return 0;
    return tecnicoTarget->disponibile;
}

void setDisponibilitaTecnico(Tecnico* tecnicoTarget, int flagDisponibilita) {
    if (tecnicoTarget == NULL) return;
    tecnicoTarget->disponibile = flagDisponibilita;
}

void setSpecializzazioneTecnico(Tecnico* tecnicoTarget, const char* nuovaSpecializzazione) {
    if (tecnicoTarget == NULL) return;
    free(tecnicoTarget->specializzazione);
    tecnicoTarget->specializzazione = duplicaStringa(nuovaSpecializzazione);
}