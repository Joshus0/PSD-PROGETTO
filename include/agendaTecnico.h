#ifndef AGENDA_TECNICO_H    //Guardia di inclusione per evitare inclusioni multiple
#define AGENDA_TECNICO_H

typedef struct AgendaTecnico AgendaTecnico; //Forward declaration della struttura AgendaTecnico presente in agendaTecnico.c, utilizzata per rappresentare l'agenda di un tecnico con i suoi interventi pianificati
typedef struct NodoAgenda NodoAgenda;       //Forward declaration della struttura NodoAgenda presente in agendaTecnico.c, utilizzata per implementare i nodi dell'agenda del tecnico, contenenti le informazioni sugli interventi pianificati


//Dichiarazione delle funzioni per la gestione dell'agenda del tecnico
AgendaTecnico* creaAgendaTecnico();
void distruggiAgendaTecnico(AgendaTecnico* agendaTarget);

//Funzioni per la manipolazione dell'agenda del tecnico
int inserisciInterventoInAgenda(AgendaTecnico* agendaTarget, const char* data, const char* fasciaOraria, const char* codiceRichiesta);

//Funzioni per accedere ai nodi e alle informazioni sugli interventi nell'agenda del tecnico
NodoAgenda* getRadiceAgenda(const AgendaTecnico* agendaTarget);
NodoAgenda* getFiglioSinistroAgenda(const NodoAgenda* nodoCorrente);
NodoAgenda* getFiglioDestroAgenda(const NodoAgenda* nodoCorrente);

//Funzioni getter per accedere alle informazioni sugli interventi pianificati nell'agenda del tecnico
const char* getDataIntervento(const NodoAgenda* nodoCorrente);
const char* getFasciaOrariaIntervento(const NodoAgenda* nodoCorrente);
const char* getCodiceRichiestaInAgenda(const NodoAgenda* nodoCorrente);

void rimuoviInterventoDaAgenda(AgendaTecnico* agenda, const char* data, const char* fascia);

#endif
//Chiusura di #ifndef AGENDA_TECNICO_H