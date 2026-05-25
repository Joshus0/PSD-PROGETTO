/*
 * Implementazione delle funzioni di utilita' del sistema.
 *
 * Raccoglie funzioni ausiliarie condivise tra i vari moduli:
 * validazione e confronto di date, I/O da terminale, stampa
 * formattata di entita', logica di pianificazione degli interventi,
 * caricamento dati da file e confronto di output per i casi di test.
 *
 * Dipende da tutte le entita' principali (Richiesta, Tecnico,
 * AlberoTecnici, AgendaTecnico, ArchivioRichieste) poiche' funge
 * da livello di presentazione e coordinamento del sistema.
 *
 * 7 maggio
 * Sabato Pio
 */
#include "main/utilita.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

/*
 * duplicaStringa - Alloca e restituisce una copia di una stringa.
 *
 * Alternativa portabile a strdup, compatibile con C89/C99.
 * Usata ovunque nel sistema per rendere le strutture dati indipendenti
 * dal ciclo di vita dei buffer passati dal chiamante.
 *
 * Parametri:
 *   src - Stringa da duplicare (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore alla copia allocata in heap, oppure NULL se src e' NULL
 *   o se la malloc fallisce.
 */
char* duplicaStringa(const char* src) {
    char* copia;
    if (src == NULL) return NULL;
    
    copia = (char*)malloc(strlen(src) + 1);
    if (copia != NULL) {
        strcpy(copia, src);
    }
    return copia;
}

/*
 * Funzione: validaData
 * ---------------------
 * Verifica che la stringa data rispetti il formato "GG/MM/AAAA" e che
 * rappresenti una data valida del calendario, inclusi i controlli sui mesi
 * e gli anni bisestili.
 *
 * Parametri:
 *   data - Stringa contenente la data da validare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso data == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   1 se la data e' valida, 0 se la stringa e' NULL, malformata o
 *   rappresenta una data non valida.
 */
int validaData(const char* data) {
    int giorno, mese, anno;
    int bisestile;
    
    if (data == NULL || strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    if (sscanf(data, "%2d/%2d/%4d", &giorno, &mese, &anno) != 3) {
        return 0; 
    }

    if (anno < 1900 || anno > 2100) return 0;
    if (mese < 1 || mese > 12) return 0;
    if (giorno < 1 || giorno > 31) return 0;

    if ((mese == 4 || mese == 6 || mese == 9 || mese == 11) && giorno > 30) {
        return 0;
    }
    
    if (mese == 2) {
        bisestile = (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0));
        if (giorno > 28 + bisestile) {
            return 0;
        }
    }

    return 1; 
}

/*
 * Funzione: confrontaDate
 * ------------------------
 * Confronta due date nel formato "GG/MM/AAAA" restituendo l'ordinamento
 * cronologico tra le due stringhe. Se una delle due date e' NULL, la funzione
 * restituisce 0.
 *
 * Parametri:
 *   data1 - Prima data da confrontare (puo' essere NULL)
 *   data2 - Seconda data da confrontare (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso data1 == NULL o data2 == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica allo stato del sistema.
 *
 * Ritorna:
 *   1 se data1 precede data2, -1 se data1 segue data2, 0 se le date sono uguali
 *   o se uno dei puntatori e' NULL.
 */
static int confrontaDate(const char* data1, const char* data2) {
    int g1, m1, a1;
    int g2, m2, a2;
    
    if (data1 == NULL || data2 == NULL) return 0;
    
    sscanf(data1, "%d/%d/%d", &g1, &m1, &a1);
    sscanf(data2, "%d/%d/%d", &g2, &m2, &a2);
    
    if (a1 != a2) {
        if (a1 < a2) return 1;
        else return -1;
    }
    if (m1 != m2) {
        if (m1 < m2) return 1;
        else return -1;
    }
    if (g1 != g2) {
        if (g1 < g2) return 1;
        else return -1;
    }
    
    return 0; 
}

/*
 * Funzione: validaDataChiusuraRichiesta
 * -------------------------------------
 * Verifica che la data di chiusura fornita sia valida e coerente con le
 * date della richiesta: non deve essere antecedente alla data di creazione
 * e non deve precedere la data di inizio lavorazione se quest'ultima e' presente.
 *
 * Parametri:
 *   dataChiusura - Data di chiusura nel formato "GG/MM/AAAA" (puo' essere NULL)
 *   richiesta    - Richiesta a cui la data di chiusura si riferisce (puo' essere NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   i casi dataChiusura == NULL e richiesta == NULL.
 *
 * Post-condizione:
 *   Nessuna modifica alla richiesta o allo stato del sistema.
 *
 * Ritorna:
 *   1 se la data di chiusura e' valida rispetto alla richiesta;
 *   0 altrimenti.
 */
int validaDataChiusuraRichiesta(const char* dataChiusura, const Richiesta* richiesta) {
    if (dataChiusura == NULL || richiesta == NULL) return 0;
    
    if (validaData(dataChiusura) == 0) {
        return 0;
    }
    
    const char* dataCreazione = getDataRichiesta(richiesta);
    const char* dataInizioLav = getDataInizioLavorazioneRichiesta(richiesta);
    
    if (dataCreazione != NULL && confrontaDate(dataChiusura, dataCreazione) > 0) {
        printf(RED BOLD "\n [ ERRORE ] Data chiusura non puo' essere prima della data di creazione.\n" RESET);
        return 0;
    }
    
    if (dataInizioLav != NULL && confrontaDate(dataChiusura, dataInizioLav) > 0) {
        printf(RED BOLD "\n [ ERRORE ] Data chiusura non puo' essere prima della data di inizio lavorazione.\n" RESET);
        return 0;
    }
    
    return 1;
}

/*
 * pulisciBuffer - Svuota il buffer di input stdin fino al newline o EOF.
 *
 * Da chiamare dopo ogni scanf per evitare che caratteri residui
 * (in particolare '\n') vengano letti dalle chiamate successive
 * a fgets o getchar.
 */
void pulisciBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * pulisciSchermo - Cancella il contenuto del terminale.
 *
 * Usa "cls" su Windows e "clear" su sistemi POSIX (Linux/macOS).
 * Chiamata prima di ogni schermata del menu per un'interfaccia pulita.
 */
void pulisciSchermo() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/*
 * pausaSchermo - Sospende l'esecuzione fino alla pressione di INVIO.
 *
 * Permette all'utente di leggere l'output di un'operazione prima
 * che il menu venga ridisegnato. Da chiamare a fine di ogni case
 * del menu principale.
 */
void pausaSchermo() {
    printf(BOLD YELLOW "\n>> Premi INVIO per tornare al menu..." RESET);
    getchar();
}

/*
 * acquisisciStringa - Legge una riga di testo da stdin nel buffer fornito.
 *
 * Stampa il prompt, acquisisce la riga con fgets e rimuove il
 * carattere newline finale. Se fgets fallisce, restituisce una
 * stringa vuota anziche' lasciare il buffer indeterminato.
 *
 * Parametri:
 *   prompt - Testo da mostrare prima dell'input (non NULL)
 *   buffer - Destinazione della stringa letta (non NULL)
 *   dim    - Dimensione massima del buffer, incluso il terminatore
 */
void acquisisciStringa(const char* prompt, char* buffer, int dim) {
    printf(BOLD "\n>> %s" RESET, prompt);
    
    if (fgets(buffer, dim, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

/*
 * statoRichiestaToString - Converte un valore StatoRichiesta in stringa leggibile.
 *
 * Usata dalla funzione stampaRichiesta e dalle stampe di dettaglio
 * per presentare lo stato in forma testuale anziche' numerica.
 *
 * Parametri:
 *   stato - Valore dell'enumerazione StatoRichiesta
 *
 * Ritorna:
 *   Stringa costante con il nome dello stato, oppure "SCONOSCIUTO"
 *   se il valore non corrisponde ad alcun caso dell'enumerazione.
 */
const char* statoRichiestaToString(StatoRichiesta stato) {
    switch (stato) {
        case APERTA: return "APERTA";
        case PIANIFICATA: return "PIANIFICATA";
        case IN_LAVORAZIONE: return "IN_LAVORAZIONE";
        case CONCLUSA: return "CONCLUSA";
        case ANNULLATA: return "ANNULLATA";
        default: return "SCONOSCIUTO";
    }
}


/*
 * stampaTecnico - Stampa una riga formattata con i dati di un tecnico.
 *
 * Produce una riga nel formato a tabella previsto dal menu principale
 * (opzione 4 - Elenco Tecnici). I campi sono allineati con %-Ns per
 * uniformita' visiva nell'elenco completo.
 *
 * Parametri:
 *   tecnico - Puntatore al Tecnico da stampare (ignorato se NULL)
 */
void stampaTecnico(Tecnico* tecnico) {
    if (tecnico == NULL) return;
    
    const char* statoDisp;
    if (isDisponibileTecnico(tecnico)) {
        statoDisp = "Disponibile";
    } else {
        statoDisp = "Occupato";
    }
    printf(CYAN "|" RESET " %-10s " CYAN "|" RESET " %-25s " CYAN "|" RESET " %-20s " CYAN "|" RESET " %-22s " CYAN "|\n" RESET,
           getCodiceTecnico(tecnico),
           getNomeTecnico(tecnico),
           getSpecializzazioneTecnico(tecnico),
           statoDisp);
}

/*
 * stampaRichiesta - Stampa una riga formattata con i dati di una richiesta.
 *
 * Produce una riga nel formato a tabella condiviso da piu' schermate
 * (ricerca, assegnazione, aggiornamento stato). Se nessun tecnico e'
 * ancora assegnato, mostra "N/A" al posto del codice.
 *
 * Parametri:
 *   richiesta - Puntatore alla Richiesta da stampare (ignorata se NULL)
 */
void stampaRichiesta(const Richiesta* richiesta) {
    if (richiesta == NULL) return;
    
    const char* tecnicoAss = getCodiceTecnicoAssegnatoRichiesta(richiesta);
    
    const char* tecnicoStampa;
    if (tecnicoAss != NULL) {
        tecnicoStampa = tecnicoAss;
    } else {
        tecnicoStampa = "N/A";
    }
    printf(CYAN "|" RESET " %-8s " CYAN "|" RESET " %-14s " CYAN "|" RESET " %-15s " CYAN "|" RESET " %-7d " CYAN "|" RESET " %-14s " CYAN "|" RESET " %-13s " CYAN "|\n" RESET,
           getCodiceRichiesta(richiesta),
           getAppartamentoRichiesta(richiesta),
           getTipologiaProblemaRichiesta(richiesta),
           getLivelloUrgenzaRichiesta(richiesta),
           statoRichiestaToString(getStatoRichiesta(richiesta)),
           tecnicoStampa);
}

/*
 * trovaTecnicoDisponibilePerSpecializzazioneRic - Visita ricorsiva del BST
 *   dei tecnici alla ricerca del primo disponibile con la specializzazione
 *   richiesta.
 *
 * Funzione statica di supporto a trovaTecnicoDisponibilePerSpecializzazione.
 * La visita e' in-order (sinistro -> radice -> destro) per garantire
 * determinismo nella selezione: a parita' di specializzazione viene
 * preferito il tecnico con codice lessicograficamente minore.
 *
 * Parametri:
 *   nodo             - Nodo corrente del BST (NULL indica sottoalbero vuoto)
 *   specializzazione - Specializzazione da cercare (non NULL)
 *
 * Ritorna:
 *   Puntatore al primo Tecnico disponibile trovato, oppure NULL se
 *   nessun tecnico soddisfa i criteri nel sottoalbero corrente.
 */
static Tecnico* trovaTecnicoDisponibilePerSpecializzazioneRic(const NodoAlberoTecnici* nodo, const char* specializzazione) {
    if (nodo == NULL) return NULL;

    Tecnico* trovato = NULL;
    NodoAlberoTecnici* sinistro = getFiglioSinistroTecnici(nodo);
    
    if (sinistro != NULL) {
        trovato = trovaTecnicoDisponibilePerSpecializzazioneRic(sinistro, specializzazione);
        if (trovato != NULL) return trovato;
    }

    Tecnico* tecnico = getTecnicoDalNodoAlbero(nodo);
    if (tecnico != NULL && isDisponibileTecnico(tecnico) && strcmp(getSpecializzazioneTecnico(tecnico), specializzazione) == 0) {
        return tecnico;
    }

    NodoAlberoTecnici* destro = getFiglioDestroTecnici(nodo);
    if (destro != NULL) {
        trovato = trovaTecnicoDisponibilePerSpecializzazioneRic(destro, specializzazione);
        if (trovato != NULL) return trovato;
    }

    return NULL;
}

/*
 * trovaTecnicoDisponibilePerSpecializzazione - Trova il primo tecnico
 *   disponibile con la specializzazione corrispondente alla tipologia
 *   del problema.
 *
 * Wrapper pubblico della funzione ricorsiva. Usato nell'assegnazione
 * automatica (case 3 del menu) per abbinare una richiesta al tecnico
 * piu' adatto tra quelli registrati nel BST.
 *
 * Parametri:
 *   albero           - BST dei tecnici in cui cercare (puo' essere NULL)
 *   specializzazione - Specializzazione richiesta (puo' essere NULL)
 *
 * Ritorna:
 *   Puntatore al Tecnico trovato, oppure NULL se nessun tecnico
 *   disponibile con quella specializzazione e' presente nell'albero.
 */
Tecnico* trovaTecnicoDisponibilePerSpecializzazione(const AlberoTecnici* albero, const char* specializzazione) {
    if (albero == NULL || specializzazione == NULL) return NULL;
    return trovaTecnicoDisponibilePerSpecializzazioneRic(getRadiceAlberoTecnici(albero), specializzazione);
}

/*
 * Funzione: pianificaIntervento
 * -----------------------------
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
int pianificaIntervento(Richiesta* richiesta, Tecnico* tecnico, const char* data, const char* fasciaOraria) {
    if (richiesta == NULL || tecnico == NULL || data == NULL || fasciaOraria == NULL) return 0;
    
    AgendaTecnico* agendaTecnico = getAgendaTecnico(tecnico);
    if (agendaTecnico == NULL) return 0;
    
    /* Inserisce nell'agenda e gestisce i conflitti standard */
    if (inserisciInterventoInAgenda(agendaTecnico, data, fasciaOraria, getCodiceRichiesta(richiesta)) == 0) return 0; 
    
    setCodiceTecnicoAssegnatoRichiesta(richiesta, getCodiceTecnico(tecnico));
    
    /* CORREZIONE: Lo stato iniziale di pianificazione deve essere PIANIFICATA */
    setStatoRichiesta(richiesta, PIANIFICATA); 
    
    setDataInizioLavorazioneRichiesta(richiesta, data);
    setFasciaOrariaRichiesta(richiesta, fasciaOraria);
    
    return 1; 
}

/*
 * Funzione: stampaStoricoInterventi
 * ----------------------------------
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
void stampaStoricoInterventi(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio non inizializzato.\n" RESET);
        return;
    }
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    int interventiConclusI = 0;
    
    if (nodoCorrente == NULL) {
        printf(YELLOW "\n [ AVVISO ] L'archivio e' vuoto.\n" RESET);
        return;
    }
    
    printf(CYAN BOLD " _________________________________________________________________________________________ \n");
    printf("| %-3s | %-8s | %-12s | %-13s | %-11s | %-9s | %-11s |\n", "#", "CODICE", "APPARTAMENTO", "TIPOLOGIA", "STATO", "TECNICO", "DATA CHIUSURA");
    printf("|-----|----------|--------------|---------------|-------------|-----------|--------------|\n" RESET);
    
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL && getStatoRichiesta(richiesta) == CONCLUSA) {
            interventiConclusI++;
            const char* dataChiusura = getDataChiusuraRichiesta(richiesta);
            const char* tecnicoAss = getCodiceTecnicoAssegnatoRichiesta(richiesta);
            
            const char* tecnicoStampa;
            if (tecnicoAss != NULL) {
                tecnicoStampa = tecnicoAss;
            } else {
                tecnicoStampa = "N/A";
            }
            const char* dataChiusuraStampa;
            if (dataChiusura != NULL) {
                dataChiusuraStampa = dataChiusura;
            } else {
                dataChiusuraStampa = "N/A";
            }
            printf(CYAN "|" RESET " %-3d " CYAN "|" RESET " %-8s " CYAN "|" RESET " %-12s " CYAN "|" RESET " %-13s " CYAN "|" RESET " %-11s " CYAN "|" RESET " %-9s " CYAN "|" RESET " %-12s " CYAN " |\n" RESET, 
                   interventiConclusI,
                   getCodiceRichiesta(richiesta),
                   getAppartamentoRichiesta(richiesta),
                   getTipologiaProblemaRichiesta(richiesta),
                   statoRichiestaToString(getStatoRichiesta(richiesta)),
                   tecnicoStampa,
                   dataChiusuraStampa);
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    if (interventiConclusI == 0) {
        printf(CYAN "|" RESET YELLOW " Nessun intervento concluso registrato al momento.                                     " CYAN "|\n" RESET);
    }
    printf(CYAN "|________________________________________________________________________________________|\n" RESET);
    printf(BOLD "\n Totale interventi conclusi: %d\n" RESET, interventiConclusI);
}

/*
 * Funzione: stampaReportStatistiche
 * ----------------------------------
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
void stampaReportStatistiche(const ArchivioRichieste* archivio, const AlberoTecnici* albero) {
    if (archivio == NULL || albero == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Archivio o Albero non inizializzato.\n" RESET);
        return;
    }
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    int totaleRichieste = getDimensioneArchivio(archivio);
    int aperte = 0, pianificate = 0, inLavorazione = 0, concluse = 0, annullate = 0;
    int idrauliciCount = 0, elettriciCount = 0, altrCount = 0;
    
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL) {
            switch (getStatoRichiesta(richiesta)) {
                case APERTA: aperte++; break;
                case PIANIFICATA: pianificate++; break;
                case IN_LAVORAZIONE: inLavorazione++; break;
                case CONCLUSA: concluse++; break;
                case ANNULLATA: annullate++; break;
            }
            
            const char* tipologia = getTipologiaProblemaRichiesta(richiesta);
            if (strcmp(tipologia, "Idraulico") == 0) idrauliciCount++;
            else if (strcmp(tipologia, "Elettrico") == 0) elettriciCount++;
            else altrCount++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    printf(CYAN BOLD "\n ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|                 " MAGENTA BOLD "[ REPORT ] Statistiche Generali di Sistema" CYAN BOLD "                             |\n");
    printf("|________________________________________________________________________________________|\n" RESET);
    printf(CYAN "| %-42s | %-41s |\n", "STATISTICHE STATO", "STATISTICHE TIPOLOGIA");
    printf("|--------------------------------------------|-------------------------------------------|\n" RESET);
    printf(CYAN "|" RESET " Totale richieste     : %-18d " CYAN "|" RESET " Idraulici            : %-18d " CYAN "|\n" RESET, totaleRichieste, idrauliciCount);
    printf(CYAN "|" RESET " Richieste APERTE     : %-18d " CYAN "|" RESET " Elettrici            : %-18d " CYAN "|\n" RESET, aperte, elettriciCount);
    printf(CYAN "|" RESET " Richieste PIANIF.    : %-18d " CYAN "|" RESET " Altre tipologie      : %-18d " CYAN "|\n" RESET, pianificate, altrCount);
    printf(CYAN "|" RESET " Richieste IN LAVOR.  : %-18d " CYAN "|" RESET "                                           " CYAN "|\n" RESET, inLavorazione);
    printf(CYAN "|" RESET " Richieste CONCLUSE   : %-18d " CYAN "|" RESET "                                           " CYAN "|\n" RESET, concluse);
    printf(CYAN "|" RESET " Richieste ANNULLATE  : %-18d " CYAN "|" RESET "                                           " CYAN "|\n" RESET, annullate);
    printf(CYAN "|--------------------------------------------|-------------------------------------------|\n" RESET);
    
    if (totaleRichieste > 0) {
        char buf1[50], buf2[50];
        sprintf(buf1, "%.1f%%", (concluse * 100.0) / totaleRichieste);
        sprintf(buf2, "%.1f%%", (aperte * 100.0) / totaleRichieste);
        printf(CYAN "|" RESET " Perc. completamento  : %-18s " CYAN "|" RESET " Perc. aperte         : %-18s " CYAN "|\n" RESET, buf1, buf2);
    } else {
        printf(CYAN "|" RESET YELLOW " Dati percentuali non disponibili per assenza di richieste.                              " CYAN "|\n" RESET);
    }
    printf(CYAN "|________________________________________________________________________________________|\n" RESET);
}

/*
 * Funzione: visitaAgendaInOrder
 * ------------------------------
 * Visita ricorsivamente in ordine simmetrico i nodi dell'agenda del tecnico
 * e stampa i dati di ogni intervento con un indice crescente.
 *
 * Parametri:
 *   nodo   - Nodo corrente dell'agenda (puo' essere NULL)
 *   numero - Puntatore al contatore numerico degli interventi riscontrati (non NULL)
 *
 * Pre-condizione:
 *   Nessuna condizione particolare: la funzione gestisce internamente
 *   il caso nodo == NULL.
 *
 * Post-condizione:
 *   Viene eseguita la stampa dei nodi dell'agenda in ordine cronologico.
 *   Il contatore numero viene incrementato per ogni intervento stampato.
 *
 * Ritorna:
 *   Niente (void).
 */
static void visitaAgendaInOrder(NodoAgenda* nodo, int* numero) {
    if (nodo != NULL) {
        visitaAgendaInOrder(getFiglioSinistroAgenda(nodo), numero);
        printf(CYAN "|" RESET " %-4d " CYAN "|" RESET " %-15s " CYAN "|" RESET " %-25s " CYAN "|" RESET " %-33s " CYAN "|\n" RESET, 
               *numero, 
               getDataIntervento(nodo),
               getFasciaOrariaIntervento(nodo),
               getCodiceRichiestaInAgenda(nodo));
        (*numero)++;
        visitaAgendaInOrder(getFiglioDestroAgenda(nodo), numero);
    }
}

/*
 * Funzione: stampaAgendaTecnico
 * -----------------------------
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
void stampaAgendaTecnico(Tecnico* tecnico) {
    if (tecnico == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Tecnico non valido.\n" RESET);
        return;
    }
    
    AgendaTecnico* agenda = getAgendaTecnico(tecnico);
    if (agenda == NULL) {
        printf(RED BOLD "\n [ ERRORE ] Agenda non inizializzata.\n" RESET);
        return;
    }
    
    printf(CYAN BOLD " ________________________________________________________________________________________ \n");
    printf("|                                                                                        |\n");
    printf("|  AGENDA TECNICO: %-70s|\n", getNomeTecnico(tecnico));
    printf("|________________________________________________________________________________________|\n" RESET);
    
    NodoAgenda* nodo = getRadiceAgenda(agenda);
    if (nodo == NULL) {
        printf(CYAN "|" RESET YELLOW " Nessun intervento pianificato al momento.                                             " CYAN "|\n" RESET);
        printf(CYAN "|________________________________________________________________________________________|\n" RESET);
        return;
    }
    
    int numero = 1;
    printf(CYAN "| %-4s | %-15s | %-25s | %-33s |\n", "#", "DATA", "FASCIA ORARIA", "CODICE RICHIESTA");
    printf("|------|-----------------|---------------------------|-----------------------------------|\n" RESET);
    
    visitaAgendaInOrder(nodo, &numero);
    printf(CYAN "|________________________________________________________________________________________|\n" RESET);
}

/*
 * Funzione: caricaTecniciDaFile
 * -----------------------------
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
int caricaTecniciDaFile(AlberoTecnici* albero, const char* percorsoFile) {
    FILE* file = fopen(percorsoFile, "r");
    if (file == NULL) return 0;

    char linea[256];
    int caricati = 0;

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\r\n")] = 0;

        char* codice = strtok(linea, ";");
        char* nome = strtok(NULL, ";");
        char* spec = strtok(NULL, ";");

        if (codice != NULL && nome != NULL && spec != NULL) {
            if (cercaTecnicoInAlbero(albero, codice) == NULL) {
                Tecnico* t = creaTecnico(codice, nome, spec);
                if (t != NULL) {
                    inserisciInAlberoTecnici(albero, t);
                    caricati++;
                }
            }
        }
    }
    fclose(file);
    return caricati;
}

/*
 * Funzione: caricaRichiesteDaFile
 * --------------------------------
 * Legge un file di testo contenente la lista delle richieste e le inserisce
 * nell'archivio e nella coda di priorita'. Ogni riga del file deve contenere
 * i campi separati da ";" nel formato: codice;appartamento;tipologia;descrizione;data;urgenza.
 *
 * Parametri:
 *   archivio      - Archivio delle richieste in cui inserire i dati (non NULL)
 *   coda          - Coda di priorita' in cui inserire le richieste (non NULL)
 *   percorsoFile  - Percorso del file da leggere (puo' essere NULL)
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
int caricaRichiesteDaFile(ArchivioRichieste* archivio, CodaPriorita* coda, const char* percorsoFile) {
    FILE* file = fopen(percorsoFile, "r");
    if (file == NULL) return 0;

    char linea[512];
    int caricati = 0;

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\r\n")] = 0;

        char* codice = strtok(linea, ";");
        char* app = strtok(NULL, ";");
        char* tipo = strtok(NULL, ";");
        char* desc = strtok(NULL, ";");
        char* data = strtok(NULL, ";");
        char* urgStr = strtok(NULL, ";");

        if (codice && app && tipo && desc && data && urgStr) {
            int urgenza = atoi(urgStr);
            Richiesta* r = creaRichiesta(codice, app, tipo, desc, data, urgenza);
            if (r != NULL) {
                inserisciInCodaArchivio(archivio, r);
                inserisciInCodaPriorita(coda, r);
                caricati++;
            }
        }
    }
    fclose(file);
    return caricati;
}

/*
 * Funzione: confrontaFileOracolo
 * -------------------------------
 * Confronta due file di testo riga per riga per verificare che l'output
 * prodotto corrisponda esattamente al file oracolo di riferimento.
 *
 * Parametri:
 *   fileOutput - Percorso del file prodotto dal sistema (puo' essere NULL)
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
int confrontaFileOracolo(const char* fileOutput, const char* fileOracolo) {
    FILE* fOut = fopen(fileOutput, "r");
    FILE* fOra = fopen(fileOracolo, "r");

    if (fOut == NULL || fOra == NULL) {
        if (fOut) fclose(fOut);
        if (fOra) fclose(fOra);
        return -1; 
    }

    char rigaOut[512];
    char rigaOra[512];
    int uguali = 1;

    while (1) {
        char* readOut = fgets(rigaOut, sizeof(rigaOut), fOut);
        char* readOra = fgets(rigaOra, sizeof(rigaOra), fOra);

        if (readOut == NULL && readOra == NULL) break; 
        
        if (readOut != NULL) rigaOut[strcspn(rigaOut, "\r\n")] = 0;
        if (readOra != NULL) rigaOra[strcspn(rigaOra, "\r\n")] = 0;

        if (readOut == NULL || readOra == NULL || strcmp(rigaOut, rigaOra) != 0) {
            uguali = 0;
            break;
        }
    }

    fclose(fOut);
    fclose(fOra);
    
    return uguali;
}

/*
 * Funzione: stampaCaricoLavoroTecnico
 * -----------------------------------
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
void stampaCaricoLavoroTecnico(Tecnico* t) {
    if (t == NULL) return;
    
    int carico = getNumeroInterventiAgenda(getAgendaTecnico(t));
    char statoStr[50]; 
    
    if (carico == 0) {
        sprintf(statoStr, "LIBERO (%d)", carico);
        printf(CYAN "|" RESET " %-10s " CYAN "|" RESET " %-22s " CYAN "|" RESET " %-18s " CYAN "|" GREEN " %-27s " CYAN "|\n" RESET, 
               getCodiceTecnico(t), getNomeTecnico(t), getSpecializzazioneTecnico(t), statoStr);
    } else if (carico < 3) {
        sprintf(statoStr, "OCCUPATO (%d)", carico);
        printf(CYAN "|" RESET " %-10s " CYAN "|" RESET " %-22s " CYAN "|" RESET " %-18s " CYAN "|" YELLOW " %-27s " CYAN "|\n" RESET, 
               getCodiceTecnico(t), getNomeTecnico(t), getSpecializzazioneTecnico(t), statoStr);
    } else {
        sprintf(statoStr, "PIENO (%d)", carico);
        printf(CYAN "|" RESET " %-10s " CYAN "|" RESET " %-22s " CYAN "|" RESET " %-18s " CYAN "|" RED BOLD " %-27s " CYAN "|\n" RESET, 
               getCodiceTecnico(t), getNomeTecnico(t), getSpecializzazioneTecnico(t), statoStr);
    }
}

/*
 * validaFasciaOraria - Controlla che la stringa rispetti il formato HH:MM-HH:MM.
 *
 * Verifica la lunghezza esatta (11 caratteri), la presenza dei separatori
 * nelle posizioni attese, il corretto range numerico di ore (0-23) e
 * minuti (0-59), e infine che l'orario di inizio preceda strettamente
 * quello di fine per garantire la coerenza logica dell'appuntamento.
 *
 * Parametri:
 * fascia - Stringa contenente la fascia oraria da validare (puo' essere NULL)
 *
 * Ritorna:
 * 1 se la fascia oraria e' sintatticamente e logicamente valida,
 * 0 in caso di formato errato, valori fuori limite, incongruenza
 * logica o se il puntatore passato e' nullo.
 */
int validaFasciaOraria(const char* fascia) {
    int h1, m1, h2, m2;

    /* 1. Controllo base e lunghezza esatta per standardizzare il formato */
    if (fascia == NULL || strlen(fascia) != 11) return 0;
    
    /* 2. Controllo posizione rigida dei separatori */
    if (fascia[2] != ':' || fascia[5] != '-' || fascia[8] != ':') return 0;

    /* 3. Estrazione dei numeri */
    if (sscanf(fascia, "%2d:%2d-%2d:%2d", &h1, &m1, &h2, &m2) != 4) return 0;

    /* 4. Validazione range orologio */
    if (h1 < 0 || h1 > 23 || m1 < 0 || m1 > 59) return 0;
    if (h2 < 0 || h2 > 23 || m2 < 0 || m2 > 59) return 0;

    /* 5. Validazione coerenza (l'inizio deve venire prima della fine) */
    if ((h1 * 60 + m1) >= (h2 * 60 + m2)) return 0;

    return 1;
}