#ifndef AGENDA_TECNICO_H    //Guardia di inclusione per evitare inclusioni multiple
#define AGENDA_TECNICO_H

typedef struct AgendaTecnico AgendaTecnico; //Forward declaration della struttura AgendaTecnico presente in agendaTecnico.c, utilizzata per rappresentare l'agenda di un tecnico con i suoi interventi pianificati
typedef struct NodoAgenda NodoAgenda;       //Forward declaration della struttura NodoAgenda presente in agendaTecnico.c, utilizzata per implementare i nodi dell'agenda del tecnico, contenenti le informazioni sugli interventi pianificati


//Dichiarazione delle funzioni per la gestione dell'agenda del tecnico
AgendaTecnico* creaAgendaTecnico();
void distruggiAgendaTecnico(AgendaTecnico* agendaTarget);


//Funzioni per la manipolazione dell'agenda del tecnico
int inserisciInterventoInAgenda(AgendaTecnico* agendaTarget, const char* data, const char* fasciaOraria, const char* codiceRichiesta);
int getNumeroInterventiAgenda(AgendaTecnico* agenda);

//Funzioni per accedere ai nodi e alle informazioni sugli interventi nell'agenda del tecnico
NodoAgenda* getRadiceAgenda(const AgendaTecnico* agendaTarget);
NodoAgenda* getFiglioSinistroAgenda(const NodoAgenda* nodoCorrente);
NodoAgenda* getFiglioDestroAgenda(const NodoAgenda* nodoCorrente);

//Funzioni getter per accedere alle informazioni sugli interventi pianificati nell'agenda del tecnico
const char* getDataIntervento(const NodoAgenda* nodoCorrente);
const char* getFasciaOrariaIntervento(const NodoAgenda* nodoCorrente);
const char* getCodiceRichiestaInAgenda(const NodoAgenda* nodoCorrente);

/*
 * Funzione: rimuoviInterventoDaAgenda
 * ------------------------------------
 * Rimuove dall'agenda il nodo corrispondente all'appuntamento
 * identificato dalla coppia data/fascia oraria, ripristinando
 * la proprieta' di BST dopo la cancellazione.
 * Se il nodo da eliminare ha due figli, viene sostituito con il
 * successore in-order (minimo del sottoalbero destro) per mantenere
 * l'ordinamento cronologico. Le stringhe del nodo rimosso vengono
 * deallocate.
 *
 * Parametri:
 *   agenda - Puntatore all'agenda da cui rimuovere l'intervento
 *            (puo' essere NULL)
 *   data   - Data dell'appuntamento da rimuovere nel formato
 *            "GG/MM/AAAA" (puo' essere NULL)
 *   fascia - Fascia oraria dell'appuntamento da rimuovere,
 *            es. "09:00-11:00" (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso agenda == NULL.
 *   La coppia data/fascia deve identificare univocamente un nodo
 *   nell'agenda per produrre un effetto.
 *
 * Post-condizione:
 *   Se il nodo e' trovato, viene rimosso e la sua memoria deallocata.
 *   La proprieta' di BST dell'agenda viene ripristinata.
 *   Se la coppia data/fascia non corrisponde ad alcun nodo,
 *   l'agenda rimane invariata.
 *
 * Ritorna:
 *   Niente (void).
 */
void rimuoviInterventoDaAgenda(AgendaTecnico* agenda, const char* data, const char* fascia);

#endif
//Chiusura di #ifndef AGENDA_TECNICO_H