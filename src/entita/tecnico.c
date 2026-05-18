/*
 * Implementazione dell'entita' Tecnico.
 *
 * Gestisce il ciclo di vita di un tecnico all'interno del sistema: dalla 
 * creazione, all'aggiornamento della sua specializzazione e disponibilita', 
 * fino alla gestione della sua agenda di interventi. Ogni tecnico e' 
 * identificato da un codice univoco.
 * 
 * 6 maggio
 * Joshua Sarnelli
 */
#include "entita/tecnico.h"
#include "main/utilita.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Struttura interna di un Tecnico.
 *
 * I campi stringa (codice, nome, specializzazione) vengono allocati
 * dinamicamente nello heap. Il flag 'disponibile' indica se il tecnico 
 * puo' ricevere nuovi incarichi. Il campo 'agenda' e' un puntatore 
 * alla struttura dati che gestisce gli appuntamenti pianificati.
 */
struct Tecnico {
    char* codice;
    char* nome;
    char* specializzazione;
    int disponibile;
    AgendaTecnico* agenda;
};

/*
 * creaTecnico - Alloca e inizializza un nuovo tecnico.
 *
 * Tutti i campi stringa vengono duplicati in memoria heap, rendendo
 * la struttura indipendente dal ciclo di vita dei buffer passati dal chiamante.
 * Di default, il tecnico viene impostato come disponibile (1) e viene
 * creata un'agenda vuota ad esso associata.
 *
 * Parametri:
 *   codice           - Identificatore univoco del tecnico (non NULL)
 *   nome             - Nome completo del tecnico (non NULL)
 *   specializzazione - Ambito di competenza del tecnico (non NULL)
 *
 * Ritorna:
 *   Puntatore al nuovo Tecnico allocato, oppure NULL in caso di
 *   fallimento della malloc.
 */
Tecnico* creaTecnico(const char* codice, const char* nome, const char* specializzazione) {
    Tecnico* nuovo = (Tecnico*)malloc(sizeof(Tecnico));
    if (nuovo == NULL) return NULL;

    nuovo->codice = duplicaStringa(codice);
    nuovo->nome = duplicaStringa(nome);
    nuovo->specializzazione = duplicaStringa(specializzazione);
    nuovo->disponibile = 1;
    nuovo->agenda = creaAgendaTecnico();

    return nuovo;
}

/*
 * distruggiTecnico - Libera tutta la memoria associata a un Tecnico.
 *
 * Distrugge l'agenda associata e dealloca ogni campo stringa 
 * prima di liberare la struttura stessa.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico da deallocare (ignorato se NULL)
 */
void distruggiTecnico(Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return;

    if (tecnicoTarget->agenda != NULL) distruggiAgendaTecnico(tecnicoTarget->agenda);

    free(tecnicoTarget->codice);
    free(tecnicoTarget->nome);
    free(tecnicoTarget->specializzazione);
    
    free(tecnicoTarget);
}

/*
 * GETTER — lettura dei campi del tecnico
 *
 * Tutte le funzioni getter restituiscono NULL (o 0 per i tipi interi)
 * se il puntatore ricevuto e' NULL, cosi' da evitare dereferenziazioni
 * non sicure nel codice chiamante.
 */

/*
 * getCodiceTecnico - Restituisce il codice univoco del tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con il codice, oppure NULL se tecnicoTarget e' NULL.
 */
const char* getCodiceTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->codice;
}

/*
 * getNomeTecnico - Restituisce il nome del tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con il nome, oppure NULL se tecnicoTarget e' NULL.
 */
const char* getNomeTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->nome;
}

/*
 * getSpecializzazioneTecnico - Restituisce la specializzazione del tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Ritorna:
 *   Stringa con la specializzazione, oppure NULL se tecnicoTarget e' NULL.
 */
const char* getSpecializzazioneTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->specializzazione;
}

/*
 * isDisponibileTecnico - Indica se il tecnico e' attualmente disponibile.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Ritorna:
 *   1 se il tecnico e' disponibile, 0 altrimenti (anche se NULL).
 */
int isDisponibileTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return 0;
    return tecnicoTarget->disponibile;
}

/*
 * SETTER — modifica dei campi del tecnico
 *
 * I setter che aggiornano campi stringa liberano prima la memoria del
 * valore precedente per evitare memory leak, poi duplicano il nuovo valore.
 */

/*
 * setDisponibilitaTecnico - Aggiorna il flag di disponibilita' del tecnico.
 *
 * Parametri:
 *   tecnicoTarget     - Puntatore al tecnico da aggiornare (ignorato se NULL)
 *   flagDisponibilita - 1 per disponibile, 0 per non disponibile
 */
void setDisponibilitaTecnico(Tecnico* tecnicoTarget, int flagDisponibilita) {
    if (tecnicoTarget == NULL) return;
    tecnicoTarget->disponibile = flagDisponibilita;
}

/*
 * setSpecializzazioneTecnico - Modifica la specializzazione del tecnico.
 *
 * Libera l'eventuale specializzazione precedente prima di duplicare
 * la nuova stringa, per evitare memory leak.
 *
 * Parametri:
 *   tecnicoTarget         - Puntatore al tecnico da aggiornare (ignorato se NULL)
 *   nuovaSpecializzazione - Stringa con la nuova specializzazione
 */
void setSpecializzazioneTecnico(Tecnico* tecnicoTarget, const char* nuovaSpecializzazione) {
    if (tecnicoTarget == NULL) return;
    free(tecnicoTarget->specializzazione);
    tecnicoTarget->specializzazione = duplicaStringa(nuovaSpecializzazione);
}

/*
 * getAgendaTecnico - Restituisce l'agenda degli interventi del tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore alla struttura AgendaTecnico, oppure NULL se tecnicoTarget e' NULL.
 */
AgendaTecnico* getAgendaTecnico(const Tecnico* tecnicoTarget) {
    if (tecnicoTarget == NULL) return NULL;
    return tecnicoTarget->agenda;
}