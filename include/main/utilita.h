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

/*
 * Funzione: pianificaIntervento
 * ----------------------------
 * Pianifica un intervento assegnandolo a un tecnico e inserendolo nell'agenda
 * del tecnico alla data e fascia oraria indicate. Aggiorna lo stato della
 * richiesta a PIANIFICATA e registra la fascia oraria e la data di inizio.
 *
 * Parametri:
 *   richiesta   - Richiesta da pianificare (puo' essere NULL)
 *   tecnico     - Tecnico a cui assegnare la richiesta (puo' essere NULL)
 *   data        - Data dell'intervento nel formato "GG/MM/AAAA" (puo' essere NULL)
 *   fasciaOraria - Fascia oraria dell'intervento nel formato "HH:MM-HH:MM" (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   i casi NULL, ma richiede un tecnico e una richiesta validi per avere successo.
 *
 * Post-condizione:
 *   Se la pianificazione ha successo, la richiesta viene assegnata al tecnico,
 *   la sua agenda viene aggiornata e lo stato viene impostato a PIANIFICATA.
 *
 * Ritorna:
 *   1 se l'intervento e' stato pianificato con successo, 0 in caso di errore.
 */
int pianificaIntervento(Richiesta* richiesta, Tecnico* tecnico, const char* data, const char* fasciaOraria);

/*
 * Funzione: stampaStoricoInterventi
 * ---------------------------------
 * Scorre l'archivio delle richieste e stampa su stdout l'elenco delle
 * richieste concluse, mostrando codice, appartamento, tipologia, stato,
 * tecnico assegnato e data di chiusura.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'archivio non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaStoricoInterventi(const ArchivioRichieste* archivio);

/*
 * Funzione: stampaReportStatistiche
 * ---------------------------------
 * Genera un report sintetico con statistiche generali di sistema basate
 * sull'archivio delle richieste e sull'albero dei tecnici.
 *
 * Parametri:
 *   archivio - Archivio delle richieste da analizzare (puo' essere NULL)
 *   albero   - Albero dei tecnici da analizzare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso archivio == NULL o albero == NULL.
 *
 * Post-condizione:
 *   Il report viene scritto su stdout. Archivio e albero non vengono modificati.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaReportStatistiche(const ArchivioRichieste* archivio, const AlberoTecnici* albero);

/*
 * Funzione: stampaAgendaTecnico
 * ----------------------------
 * Stampa su stdout l'agenda degli interventi pianificati di un tecnico,
 * con data, fascia oraria e codice della richiesta.
 *
 * Parametri:
 *   tecnico - Tecnico di cui stampare l'agenda (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso tecnico == NULL o agenda non inizializzata.
 *
 * Post-condizione:
 *   L'output viene scritto su stdout. L'agenda del tecnico non viene modificata.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaAgendaTecnico(Tecnico* tecnico);

/*
 * Funzione: stampaCaricoLavoroTecnico
 * ----------------------------------
 * Stampa su stdout una riga contenente il codice, il nome, la specializzazione
 * e lo stato di carico del tecnico in base al numero di interventi pianificati.
 * Lo stato viene evidenziato con colori diversi a seconda della saturazione.
 *
 * Parametri:
 *   t - Puntatore al Tecnico da valutare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso t == NULL.
 *
 * Post-condizione:
 *   Una riga viene scritta su stdout. Il Tecnico non viene modificato.
 *
 * Ritorna:
 *   Niente (void).
 */
void stampaCaricoLavoroTecnico(Tecnico* t);

/*
 * Funzione: caricaTecniciDaFile
 * ----------------------------
 * Legge un file di testo contenente la lista dei tecnici e li inserisce
 * nell'albero dei tecnici. Ogni riga del file deve contenere i campi
 * separati da ";" nel formato: codice;nome;specializzazione.
 *
 * Parametri:
 *   albero       - Albero dei tecnici in cui inserire i dati (non NULL)
 *   percorsoFile - Percorso del file da leggere (puo' essere NULL)
 *
 * Pre-condizione:
 *   albero deve essere un AlberoTecnici valido. Il percorso del file deve
 *   essere leggibile dal filesystem.
 *
 * Post-condizione:
 *   I tecnici validi vengono inseriti nell'albero. L'albero viene modificato.
 *
 * Ritorna:
 *   Numero di tecnici caricati con successo, oppure 0 se il file non viene
 *   aperto o non contiene righe valide.
 */
int caricaTecniciDaFile(AlberoTecnici* albero, const char* percorsoFile);

/*
 * Funzione: caricaRichiesteDaFile
 * ------------------------------
 * Legge un file di testo contenente la lista delle richieste e le inserisce
 * nell'archivio e nella coda di priorita'. Ogni riga del file deve contenere
 * i campi separati da ";" nel formato: codice;appartamento;tipologia;descrizione;data;urgenza.
 *
 * Parametri:
 *   archivio     - Archivio delle richieste in cui inserire i dati (non NULL)
 *   coda         - Coda di priorita' in cui inserire le richieste (non NULL)
 *   percorsoFile - Percorso del file da leggere (puo' essere NULL)
 *
 * Pre-condizione:
 *   archivio e coda devono essere strutture validamente inizializzate.
 *   Il file deve essere leggibile dal filesystem.
 *
 * Post-condizione:
 *   Le richieste valide vengono aggiunte all'archivio e alla coda.
 *
 * Ritorna:
 *   Numero di richieste caricate con successo, oppure 0 se il file non
 *   viene aperto o non contiene righe valide.
 */
int caricaRichiesteDaFile(ArchivioRichieste* archivio, CodaPriorita* coda, const char* percorsoFile);

/*
 * Funzione: confrontaFileOracolo
 * ---------------------------
 * Confronta due file di testo riga per riga per verificare che l'output
 * prodotto corrisponda esattamente al file oracolo di riferimento.
 *
 * Parametri:
 *   fileOutput  - Percorso del file prodotto dal sistema (puo' essere NULL)
 *   fileOracolo - Percorso del file oracolo di riferimento (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso in cui uno dei file non possa essere aperto.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   1 se i file sono identici, 0 se differiscono, -1 se uno dei file non
 *   puo' essere aperto.
 */
int confrontaFileOracolo(const char* fileOutput, const char* fileOracolo);

/*
 * Funzione: validaFasciaOraria
 * ----------------------------------------------------
 * Verifica la validita' sintattica e logica di una stringa che
 * rappresenta una fascia oraria. La funzione accerta che la stringa
 * rispetti rigidamente il formato standardizzato "HH:MM-HH:MM" (lunghezza
 * esatta di 11 caratteri), che i separatori siano posizionati correttamente,
 * che ore e minuti rientrino nei range reali dell'orologio (0-23 e 0-59)
 * e che l'orario di inizio sia strettamente precedente a quello di fine.
 *
 * Parametri:
 * fascia - Stringa costante contenente la fascia oraria da convalidare
 * (puo' essere NULL)
 *
 * Pre-condizione:
 * Nessuna condizione particolare: la funzione gestisce internamente
 * il caso fascia == NULL in modo sicuro senza causare crash.
 *
 * Post-condizione:
 * Nessuna modifica alla stringa passata come parametro, trattandosi
 * di un'operazione di sola lettura (funzione pura).
 *
 * Ritorna:
 * 1 se la fascia oraria e' sintatticamente e logicamente valida;
 * 0 se la stringa e' NULL, malformata, fuori range, oppure se
 * l'orario di inizio non precede quello di fine.
 */
int validaFasciaOraria(const char* fascia);

#endif