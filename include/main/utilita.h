/*
 * File: utilita.h
 * ---------------
 * Interfaccia pubblica del modulo di utilita' del sistema di gestione
 * degli interventi di manutenzione condominiale.
 *
 * Dichiara funzioni ausiliarie condivise tra tutti i moduli:
 * validazione di date, I/O da terminale, stampa formattata di entita',
 * logica di pianificazione degli interventi, caricamento dati da file
 * e confronto di output per i casi di test automatici.
 *
 * Sabato Pio
 * 7 maggio
 */
#ifndef UTILITA_H
#define UTILITA_H

#include <stdio.h>
#include "entita/richiesta.h"
#include "entita/tecnico.h"
#include "alberoTecnici.h"
#include "agendaTecnico.h"
#include "codaPriorita.h"
#include "archivioRichieste.h"

/* --- MACRO PER LA FORMATTAZIONE ANSI DEL TERMINALE --- */
#define BOLD "\033[1m"
#define RESET "\033[0m"

/* Forward declaration dei tipi opachi usati nelle firme delle funzioni */
typedef struct ArchivioRichieste ArchivioRichieste;
typedef struct AlberoTecnici AlberoTecnici;
typedef struct CodaPriorita CodaPriorita;

/* 
 * GESTIONE STRINGHE
 */
/*
 * Funzione: duplicaStringa
 * ------------------------
 * Alloca in heap una copia della stringa sorgente.
 * Alternativa portabile a strdup, compatibile con C89/C99.
 *
 * Parametri:
 *   src - Stringa da duplicare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso src == NULL.
 *
 * Post-condizione:
 *   Se src != NULL e la malloc ha successo, viene restituito
 *   un nuovo buffer heap contenente una copia di src.
 *   Il chiamante e' responsabile di liberarlo con free().
 *
 * Ritorna:
 *   Puntatore alla copia allocata, oppure NULL se src e' NULL
 *   o se la malloc fallisce.
 */
char* duplicaStringa(const char* src);

int validaData(const char* data);

/* Valida la data di chiusura verificando coerenza logica con le altre date della richiesta */
int validaDataChiusuraRichiesta(const char* dataChiusura, const Richiesta* richiesta);

/*
 * INPUT / OUTPUT DA TERMINALE
 */
/*
 * Funzione: pulisciBuffer
 * -----------------------
 * Svuota il buffer di input stdin consumando tutti i caratteri
 * fino al newline o all'EOF.
 *
 * Parametri:
 *   Nessuno.
 *
 * Pre-condizione:
 *   Nessuna.
 *
 * Post-condizione:
 *   Il buffer stdin e' vuoto; la successiva lettura con fgets
 *   o getchar non trovera' caratteri residui.
 *
 * Ritorna:
 *   Niente (void).
 */
void pulisciBuffer(void);

/*
 * Funzione: pulisciSchermo
 * ------------------------
 * Cancella il contenuto del terminale usando il comando di sistema
 * appropriato alla piattaforma ("cls" su Windows, "clear" altrove).
 *
 * Parametri:
 *   Nessuno.
 *
 * Pre-condizione:
 *   Nessuna.
 *
 * Post-condizione:
 *   Il terminale viene ripulito visivamente.
 *
 * Ritorna:
 *   Niente (void).
 */
void pulisciSchermo(void);

/*
 * Funzione: pausaSchermo
 * ----------------------
 * Sospende l'esecuzione stampando un prompt e attendendo la pressione
 * del tasto INVIO da parte dell'utente.
 *
 * Parametri:
 *   Nessuno.
 *
 * Pre-condizione:
 *   Nessuna.
 *
 * Post-condizione:
 *   L'esecuzione riprende solo dopo che l'utente ha premuto INVIO.
 *
 * Ritorna:
 *   Niente (void).
 */
void pausaSchermo(void);

/*
 * Funzione: acquisisciStringa
 * ---------------------------
 * Stampa un prompt e legge una riga di testo da stdin nel buffer
 * fornito, rimuovendo il carattere newline finale.
 *
 * Parametri:
 *   prompt - Testo da mostrare prima del cursore di input (non NULL)
 *   buffer - Buffer di destinazione per la stringa letta (non NULL)
 *   dim    - Dimensione massima del buffer, incluso il terminatore '\0'
 *
 * Pre-condizione:
 *   prompt != NULL, buffer != NULL, dim > 0.
 *
 * Post-condizione:
 *   buffer contiene la stringa inserita dall'utente, terminata da '\0'
 *   e senza il carattere newline. Se la lettura fallisce, buffer[0] = '\0'.
 *
 * Ritorna:
 *   Niente (void).
 */
void acquisisciStringa(const char* prompt, char* buffer, int dim);

/*
 * CONVERSIONE E STAMPA FORMATTATA
 */
/*
 * Funzione: statoRichiestaToString
 * --------------------------------
 * Converte un valore dell'enumerazione StatoRichiesta nella
 * corrispondente stringa leggibile in italiano maiuscolo.
 *
 * Parametri:
 *   stato - Valore dell'enumerazione StatoRichiesta da convertire
 *
 * Pre-condizione:
 *   stato deve essere uno dei valori validi dell'enumerazione
 *   StatoRichiesta (APERTA, PIANIFICATA, IN_LAVORAZIONE,
 *   CONCLUSA, ANNULLATA).
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   Puntatore a stringa costante con il nome dello stato
 *   (es. "APERTA"), oppure "SCONOSCIUTO" se il valore non
 *   corrisponde ad alcun caso dell'enumerazione.
 */
const char* statoRichiestaToString(StatoRichiesta stato);

/*
 * Funzione: stampaTecnico
 * -----------------------
 * Stampa una riga formattata a tabella con i dati di un tecnico:
 * codice, nome, specializzazione e stato di disponibilita'.
 *
 * Parametri:
 *   tecnico - Puntatore al Tecnico da stampare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso tecnico == NULL.
 *
 * Post-condizione:
 *   Una riga viene scritta su stdout. Lo stato del Tecnico
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaTecnico(Tecnico* tecnico);

/*
 * Funzione: stampaRichiesta
 * -------------------------
 * Stampa una riga formattata a tabella con i dati di una richiesta:
 * codice, appartamento, tipologia, urgenza, stato e tecnico assegnato.
 * Se nessun tecnico e' assegnato, mostra "N/A".
 *
 * Parametri:
 *   richiesta - Puntatore alla Richiesta da stampare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso richiesta == NULL.
 *
 * Post-condizione:
 *   Una riga viene scritta su stdout. Lo stato della Richiesta
 *   non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaRichiesta(const Richiesta* richiesta);

/*
 * Funzione: trovaTecnicoDisponibilePerSpecializzazione
 * ----------------------------------------------------
 * Cerca nel BST dei tecnici il primo tecnico disponibile la cui
 * specializzazione corrisponde a quella richiesta. La visita avviene
 * in-order, quindi a parita' di specializzazione viene preferito il
 * tecnico con codice lessicograficamente minore.
 *
 * Parametri:
 *   albero           - BST dei tecnici in cui effettuare la ricerca
 *                      (puo' essere NULL)
 *   specializzazione - Stringa con la specializzazione da cercare
 *                      (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce
 *   internamente il caso albero == NULL o specializzazione == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica al BST ne' ai tecnici contenuti.
 *
 * Ritorna:
 *   Puntatore al primo Tecnico disponibile con la specializzazione
 *   cercata, oppure NULL se nessun tecnico soddisfa i criteri.
 */
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