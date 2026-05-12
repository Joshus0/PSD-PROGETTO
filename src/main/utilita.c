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

char* duplicaStringa(const char* src) {
    char* copia;
    if (src == NULL) return NULL;
    
    copia = (char*)malloc(strlen(src) + 1);
    if (copia != NULL) {
        strcpy(copia, src);
    }
    return copia;
}

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

static int confrontaDate(const char* data1, const char* data2) {
    int g1, m1, a1;
    int g2, m2, a2;
    
    if (data1 == NULL || data2 == NULL) return 0;
    
    sscanf(data1, "%d/%d/%d", &g1, &m1, &a1);
    sscanf(data2, "%d/%d/%d", &g2, &m2, &a2);
    
    if (a1 != a2) return a1 < a2 ? 1 : -1;
    if (m1 != m2) return m1 < m2 ? 1 : -1;
    if (g1 != g2) return g1 < g2 ? 1 : -1;
    
    return 0; 
}

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

void pulisciBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pulisciSchermo() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausaSchermo() {
    printf(BOLD YELLOW "\n>> Premi INVIO per tornare al menu..." RESET);
    getchar();
}

void acquisisciStringa(const char* prompt, char* buffer, int dim) {
    printf(BOLD "\n>> %s" RESET, prompt);
    
    if (fgets(buffer, dim, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

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

void stampaTecnico(Tecnico* tecnico) {
    if (tecnico == NULL) return;
    
    printf(CYAN "|" RESET " %-10s " CYAN "|" RESET " %-25s " CYAN "|" RESET " %-20s " CYAN "|" RESET " %-22s " CYAN "|\n" RESET,
           getCodiceTecnico(tecnico),
           getNomeTecnico(tecnico),
           getSpecializzazioneTecnico(tecnico),
           isDisponibileTecnico(tecnico) ? "Disponibile" : "Occupato");
}

void stampaRichiesta(const Richiesta* richiesta) {
    if (richiesta == NULL) return;
    
    const char* tecnicoAss = getCodiceTecnicoAssegnatoRichiesta(richiesta);
    
    printf(CYAN "|" RESET " %-8s " CYAN "|" RESET " %-14s " CYAN "|" RESET " %-15s " CYAN "|" RESET " %-7d " CYAN "|" RESET " %-14s " CYAN "|" RESET " %-13s " CYAN "|\n" RESET,
           getCodiceRichiesta(richiesta),
           getAppartamentoRichiesta(richiesta),
           getTipologiaProblemaRichiesta(richiesta),
           getLivelloUrgenzaRichiesta(richiesta),
           statoRichiestaToString(getStatoRichiesta(richiesta)),
           tecnicoAss != NULL ? tecnicoAss : "N/A");
}

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

Tecnico* trovaTecnicoDisponibilePerSpecializzazione(const AlberoTecnici* albero, const char* specializzazione) {
    if (albero == NULL || specializzazione == NULL) return NULL;
    return trovaTecnicoDisponibilePerSpecializzazioneRic(getRadiceAlberoTecnici(albero), specializzazione);
}

int pianificaIntervento(Richiesta* richiesta, Tecnico* tecnico, const char* data, const char* fasciaOraria) {
    if (richiesta == NULL || tecnico == NULL || data == NULL || fasciaOraria == NULL) return 0;
    
    AgendaTecnico* agendaTecnico = getAgendaTecnico(tecnico);
    if (agendaTecnico == NULL) return 0;
    
    if (inserisciInterventoInAgenda(agendaTecnico, data, fasciaOraria, getCodiceRichiesta(richiesta)) == 0) return 0; 
    
    setCodiceTecnicoAssegnatoRichiesta(richiesta, getCodiceTecnico(tecnico));
    setStatoRichiesta(richiesta, PIANIFICATA);
    setDataInizioLavorazioneRichiesta(richiesta, data);
    setFasciaOrariaRichiesta(richiesta, fasciaOraria);
    
    return 1; 
}

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
            
            printf(CYAN "|" RESET " %-3d " CYAN "|" RESET " %-8s " CYAN "|" RESET " %-12s " CYAN "|" RESET " %-13s " CYAN "|" RESET " %-11s " CYAN "|" RESET " %-9s " CYAN "|" RESET " %-12s " CYAN " |\n" RESET, 
                   interventiConclusI,
                   getCodiceRichiesta(richiesta),
                   getAppartamentoRichiesta(richiesta),
                   getTipologiaProblemaRichiesta(richiesta),
                   statoRichiestaToString(getStatoRichiesta(richiesta)),
                   tecnicoAss != NULL ? tecnicoAss : "N/A",
                   dataChiusura != NULL ? dataChiusura : "N/A");
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    if (interventiConclusI == 0) {
        printf(CYAN "|" RESET YELLOW " Nessun intervento concluso registrato al momento.                                     " CYAN "|\n" RESET);
    }
    printf(CYAN "|________________________________________________________________________________________|\n" RESET);
    printf(BOLD "\n Totale interventi conclusi: %d\n" RESET, interventiConclusI);
}

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
            Tecnico* t = creaTecnico(codice, nome, spec);
            if (t != NULL) {
                inserisciInAlberoTecnici(albero, t);
                caricati++;
            }
        }
    }
    fclose(file);
    return caricati;
}

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