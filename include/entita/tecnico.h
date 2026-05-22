#ifndef TECNICO_H //Guardia di inclusione per evitare inclusioni multiple
#define TECNICO_H

#include "agendaTecnico.h"
typedef struct Tecnico Tecnico; //Forward declaration della struttura Tecnico presente in tecnico.c, utilizzata per rappresentare un tecnico con le sue informazioni e disponibilità

//Dichiarazione delle funzioni per la gestione dei tecnici

/*
 * Funzione: creaTecnico
 * ---------------------
 * Alloca e inizializza un nuovo tecnico nel sistema di gestione
 * degli interventi di manutenzione condominiale.
 *
 * Parametri:
 *   codice           - Identificatore univoco del tecnico (non NULL)
 *   nome             - Nome completo del tecnico (non NULL)
 *   specializzazione - Ambito di competenza del tecnico (non NULL)
 *
 * Pre-condizione:
 *   codice, nome e specializzazione devono essere stringhe non NULL
 *   e valide. Il codice deve essere univoco nel sistema.
 *
 * Post-condizione:
 *   Un nuovo Tecnico viene allocato in heap con stato disponibile (1) e
 *   un'agenda vuota associata.
 *
 * Ritorna:
 *   Puntatore al nuovo Tecnico allocato, oppure NULL in caso di
 *   fallimento della malloc.
 */
Tecnico* creaTecnico(const char* codice, const char* nome, const char* specializzazione);

/*
 * Funzione: distruggiTecnico
 * ---------------------------
 * Libera tutta la memoria associata a un Tecnico, inclusa la sua agenda
 * e tutti i campi stringa allocati dinamicamente.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico da deallocare (ignorato se NULL)
 *
 * Pre-condizione:
 *   tecnicoTarget deve puntare a memoria allocata precedentemente tramite
 *   creaTecnico, o essere NULL.
 *
 * Post-condizione:
 *   La memoria del Tecnico viene liberata. Il puntatore tecnicoTarget
 *   diventa invalido dopo questa chiamata.
 *
 * Ritorna:
 *   Niente (void).
 */
void distruggiTecnico(Tecnico* tecnicoTarget);

//Funzioni getter per accedere ai campi di un tecnico

/*
 * Funzione: getCodiceTecnico
 * ---------------------------
 * Restituisce il codice identificativo univoco del tecnico nel sistema.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso tecnicoTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con il codice identificativo, oppure NULL se tecnicoTarget e' NULL.
 */
const char* getCodiceTecnico(const Tecnico* tecnicoTarget);

/*
 * Funzione: getNomeTecnico
 * -------------------------
 * Restituisce il nome completo del tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso tecnicoTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con il nome, oppure NULL se tecnicoTarget e' NULL.
 */
const char* getNomeTecnico(const Tecnico* tecnicoTarget);

/*
 * Funzione: getSpecializzazioneTecnico
 * ------------------------------------
 * Restituisce la specializzazione o competenza principale del tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso tecnicoTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Stringa con la specializzazione, oppure NULL se tecnicoTarget e' NULL.
 */
const char* getSpecializzazioneTecnico(const Tecnico* tecnicoTarget);

/*
 * Funzione: isDisponibileTecnico
 * --------------------------------
 * Verifica se il tecnico e' attualmente disponibile per ricevere nuovi incarichi.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso tecnicoTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   1 se il tecnico e' disponibile, 0 altrimenti (anche se tecnicoTarget e' NULL).
 */
int isDisponibileTecnico(const Tecnico* tecnicoTarget);

/*
 * Funzione: getAgendaTecnico
 * ---------------------------
 * Restituisce il puntatore all'agenda degli interventi pianificati per il tecnico.
 *
 * Parametri:
 *   tecnicoTarget - Puntatore al tecnico (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso tecnicoTarget == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore alla struttura AgendaTecnico, oppure NULL se tecnicoTarget e' NULL.
 */
AgendaTecnico* getAgendaTecnico(const Tecnico* tecnicoTarget);

//Funzioni setter per modificare i campi di un tecnico

/*
 * Funzione: setDisponibilitaTecnico
 * ---------------------------------
 * Aggiorna il flag di disponibilita' del tecnico nel sistema.
 *
 * Parametri:
 *   tecnicoTarget     - Puntatore al tecnico da aggiornare (ignorato se NULL)
 *   flagDisponibilita - 1 per marcare come disponibile, 0 per non disponibile
 *
 * Pre-condizione:
 *   tecnicoTarget deve puntare a un Tecnico valido allocato precedentemente,
 *   o essere NULL (nel qual caso la funzione non fa nulla).
 *
 * Post-condizione:
 *   Se tecnicoTarget != NULL, il flag di disponibilita' viene aggiornato.
 *
 * Ritorna:
 *   Niente (void).
 */
void setDisponibilitaTecnico(Tecnico* tecnicoTarget, int flagDisponibilita);

/*
 * Funzione: setSpecializzazioneTecnico
 * ------------------------------------
 * Modifica la specializzazione del tecnico nel sistema.
 *
 * Parametri:
 *   tecnicoTarget         - Puntatore al tecnico da aggiornare (ignorato se NULL)
 *   nuovaSpecializzazione - Stringa con la nuova specializzazione
 *
 * Pre-condizione:
 *   tecnicoTarget deve puntare a un Tecnico valido allocato precedentemente,
 *   o essere NULL.
 *
 * Post-condizione:
 *   Se tecnicoTarget != NULL, la specializzazione viene liberata e sostituita
 *   con la nuova stringa duplicata in heap.
 *
 * Ritorna:
 *   Niente (void).
 */
void setSpecializzazioneTecnico(Tecnico* tecnicoTarget, const char* nuovaSpecializzazione);

#endif
//Chiusura di #ifndef TECNICO_H