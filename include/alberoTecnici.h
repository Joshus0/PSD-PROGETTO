#ifndef ALBERO_TECNICI_H    //Guardia di inclusione per evitare inclusioni multiple
#define ALBERO_TECNICI_H

#include "entita/tecnico.h" //Inclusione dell'header per la definizione della struttura Tecnico e delle funzioni correlate, necessaria per gestire i tecnici nell'albero binario di ricerca

typedef struct AlberoTecnici AlberoTecnici; //Forward declaration della struttura AlberoTecnici presente in alberoTecnici.c, utilizzata per implementare l'albero binario di ricerca dei tecnici
typedef struct NodoAlberoTecnici NodoAlberoTecnici; //Forward declaration della struttura NodoAlberoTecnici presente in alberoTecnici.c, utilizzata per implementare i nodi dell'albero binario di ricerca dei tecnici

//Dichiarazione delle funzioni per la gestione dell'albero dei tecnici
AlberoTecnici* creaAlberoTecnici();
void distruggiAlberoTecnici(AlberoTecnici* alberoTarget);

//Funzioni per la manipolazione dell'albero dei tecnici
void inserisciInAlberoTecnici(AlberoTecnici* alberoTarget, Tecnico* nuovoTecnico);
Tecnico* cercaTecnicoInAlbero(const AlberoTecnici* alberoTarget, const char* codiceDaCercare);

//Funzioni per accedere ai nodi e ai tecnici nell'albero
NodoAlberoTecnici* getRadiceAlberoTecnici(const AlberoTecnici* alberoTarget);
NodoAlberoTecnici* getFiglioSinistroTecnici(const NodoAlberoTecnici* nodoCorrente);
NodoAlberoTecnici* getFiglioDestroTecnici(const NodoAlberoTecnici* nodoCorrente);
Tecnico* getTecnicoDalNodoAlbero(const NodoAlberoTecnici* nodoCorrente);

/* Visita tutti i tecnici in ordine applicando la funzione visitatore a ciascuno.*/

void visitaAlberoTecnici(const AlberoTecnici* alberoTarget, void (*visitatore)(Tecnico*));
#endif
//Chiusura di #ifndef ALBERO_TECNICI_H