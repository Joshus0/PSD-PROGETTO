#include "main/utilita.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    
    /* Controllo lunghezza e presenza degli slash */
    if (data == NULL || strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    /* Estrazione dei numeri */
    if (sscanf(data, "%2d/%2d/%4d", &giorno, &mese, &anno) != 3) {
        return 0; /* Non sono numeri validi */
    }

    /* Controlli base su anno e mese */
    if (anno < 1900 || anno > 2100) return 0;
    if (mese < 1 || mese > 12) return 0;
    if (giorno < 1 || giorno > 31) return 0;

    /* Controllo giorni massimi in base al mese */
    if ((mese == 4 || mese == 6 || mese == 9 || mese == 11) && giorno > 30) {
        return 0;
    }
    
    /* Controllo specifico per Febbraio e anni bisestili */
    if (mese == 2) {
        bisestile = (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0));
        if (giorno > 28 + bisestile) {
            return 0;
        }
    }

    return 1; /* La data e' perfetta */
}
/* Funzione per pulire il buffer (Solo ciclo while) */
void pulisciBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Funzione per mantenere il terminale pulito cross-platform */
void pulisciSchermo() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/* Funzione helper per mettere in pausa lo schermo */
void pausaSchermo() {
    printf(BOLD "\nPremi INVIO per tornare al menu..." RESET);
    getchar();
}

void acquisisciStringa(const char* prompt, char* buffer, int dim) {
    printf(BOLD "%s" RESET, prompt);
    
    if (fgets(buffer, dim, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

const char* statoRichiestaToString(StatoRichiesta stato) {
    switch (stato) {
        case APERTA: 
            return "APERTA";
        case PIANIFICATA: 
            return "PIANIFICATA";
        case IN_LAVORAZIONE: 
            return "IN_LAVORAZIONE";
        case CONCLUSA: 
            return "CONCLUSA";
        case ANNULLATA: 
            return "ANNULLATA";
        default: 
            return "SCONOSCIUTO";
    }
}

void stampaTecnico(Tecnico* tecnico) {
    if (tecnico == NULL) {
        return;
    }
    
    printf("Codice: %s | Nome: %s | Specializzazione: %s | Stato: %s\n",
           getCodiceTecnico(tecnico),
           getNomeTecnico(tecnico),
           getSpecializzazioneTecnico(tecnico),
           isDisponibileTecnico(tecnico) ? "Disponibile" : "Occupato");
}

void stampaRichiesta(const Richiesta* richiesta) {
    if (richiesta == NULL) {
        return;
    }
    
    printf("Codice: %s | Appartamento: %s | Tipologia: %s | Urgenza: %d | Stato: %s",
           getCodiceRichiesta(richiesta),
           getAppartamentoRichiesta(richiesta),
           getTipologiaProblemaRichiesta(richiesta),
           getLivelloUrgenzaRichiesta(richiesta),
           statoRichiestaToString(getStatoRichiesta(richiesta)));
           
    if (getCodiceTecnicoAssegnatoRichiesta(richiesta) != NULL) {
        printf(" | Tecnico: %s", getCodiceTecnicoAssegnatoRichiesta(richiesta));
    }
    
    printf("\n");
}

static Tecnico* trovaTecnicoDisponibilePerSpecializzazioneRic(const NodoAlberoTecnici* nodo, const char* specializzazione) {
    if (nodo == NULL) {
        return NULL;
    }

    Tecnico* trovato = NULL;
    NodoAlberoTecnici* sinistro = getFiglioSinistroTecnici(nodo);
    
    if (sinistro != NULL) {
        trovato = trovaTecnicoDisponibilePerSpecializzazioneRic(sinistro, specializzazione);
        if (trovato != NULL) {
            return trovato;
        }
    }

    Tecnico* tecnico = getTecnicoDalNodoAlbero(nodo);
    
    if (tecnico != NULL && isDisponibileTecnico(tecnico) && strcmp(getSpecializzazioneTecnico(tecnico), specializzazione) == 0) {
        return tecnico;
    }

    NodoAlberoTecnici* destro = getFiglioDestroTecnici(nodo);
    
    if (destro != NULL) {
        trovato = trovaTecnicoDisponibilePerSpecializzazioneRic(destro, specializzazione);
        if (trovato != NULL) {
            return trovato;
        }
    }

    return NULL;
}

Tecnico* trovaTecnicoDisponibilePerSpecializzazione(const AlberoTecnici* albero, const char* specializzazione) {
    if (albero == NULL || specializzazione == NULL) {
        return NULL;
    }
    
    return trovaTecnicoDisponibilePerSpecializzazioneRic(getRadiceAlberoTecnici(albero), specializzazione);
}

/* Pianificazione intervento con verifica conflitti orari */
int pianificaIntervento(Richiesta* richiesta, Tecnico* tecnico, const char* data, const char* fasciaOraria) {
    if (richiesta == NULL || tecnico == NULL || data == NULL || fasciaOraria == NULL) {
        return 0;
    }
    
    AgendaTecnico* agendaTecnico = getAgendaTecnico(tecnico);
    if (agendaTecnico == NULL) {
        return 0;
    }
    
    /* Verifica se la fascia oraria è disponibile (nessun conflitto) */
    if (inserisciInterventoInAgenda(agendaTecnico, data, fasciaOraria, getCodiceRichiesta(richiesta)) == 0) {
        return 0; /* Conflitto di orario */
    }
    
    /* Aggiornamento della richiesta */
    setCodiceTecnicoAssegnatoRichiesta(richiesta, getCodiceTecnico(tecnico));
    setStatoRichiesta(richiesta, PIANIFICATA);
    setDataInizioLavorazioneRichiesta(richiesta, data);
    setFasciaOrariaRichiesta(richiesta, fasciaOraria);
    
    return 1; /* Pianificazione riuscita */
}

/* Visualizza storico interventi conclusi */
void stampaStoricoInterventi(const ArchivioRichieste* archivio) {
    if (archivio == NULL) {
        printf("Archivio non inizializzato.\n");
        return;
    }
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    int interventiConclusI = 0;
    
    if (nodoCorrente == NULL) {
        printf("Archivio vuoto.\n");
        return;
    }
    
    printf("\n=== STORICO INTERVENTI CONCLUSI ===\n\n");
    
    while (nodoCorrente != NULL) {
        Richiesta* richiesta = getRichiestaDalNodoLista(nodoCorrente);
        if (richiesta != NULL && getStatoRichiesta(richiesta) == CONCLUSA) {
            printf("[%d] ", interventiConclusI + 1);
            stampaRichiesta(richiesta);
            if (getDataChiusuraRichiesta(richiesta) != NULL) {
                printf("    Data Chiusura: %s\n", getDataChiusuraRichiesta(richiesta));
            }
            interventiConclusI++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    if (interventiConclusI == 0) {
        printf("Nessun intervento concluso nel sistema.\n");
    } else {
        printf("\nTotale interventi conclusi: %d\n", interventiConclusI);
    }
}

/* Genera statistiche sui interventi */
void stampaReportStatistiche(const ArchivioRichieste* archivio, const AlberoTecnici* albero) {
    if (archivio == NULL || albero == NULL) {
        printf("Archivio o Albero non inizializzato.\n");
        return;
    }
    
    NodoLista* nodoCorrente = getTestaArchivio(archivio);
    int totaleRichieste = getDimensioneArchivio(archivio);
    int aperte = 0, pianificate = 0, inLavorazione = 0, concluse = 0, annullate = 0;
    int idrauliciCount = 0, elettriciCount = 0, altrCount = 0;
    
    printf("\n========== REPORT STATISTICHE ==========\n\n");
    
    /* Conteggio per stato */
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
            
            /* Conteggio per tipologia */
            const char* tipologia = getTipologiaProblemaRichiesta(richiesta);
            if (strcmp(tipologia, "Idraulico") == 0) idrauliciCount++;
            else if (strcmp(tipologia, "Elettrico") == 0) elettriciCount++;
            else altrCount++;
        }
        nodoCorrente = getNextNodoLista(nodoCorrente);
    }
    
    printf("--- STATISTICHE GENERALI ---\n");
    printf("Totale richieste: %d\n", totaleRichieste);
    printf("Richieste APERTE: %d\n", aperte);
    printf("Richieste PIANIFICATE: %d\n", pianificate);
    printf("Richieste IN LAVORAZIONE: %d\n", inLavorazione);
    printf("Richieste CONCLUSE: %d\n", concluse);
    printf("Richieste ANNULLATE: %d\n", annullate);
    
    printf("\n--- INTERVENTI PER TIPOLOGIA ---\n");
    printf("Idraulici: %d\n", idrauliciCount);
    printf("Elettrici: %d\n", elettriciCount);
    printf("Altre tipologie: %d\n", altrCount);
    
    printf("\n--- PERCENTUALI ---\n");
    if (totaleRichieste > 0) {
        printf("Percentuale completamento: %.1f%%\n", (concluse * 100.0) / totaleRichieste);
        printf("Percentuale aperte: %.1f%%\n", (aperte * 100.0) / totaleRichieste);
    }
    
    printf("\n=========================================\n\n");
}

/* Helper ricorsiva per visita in-order dell'agenda */
static void visitaAgendaInOrder(NodoAgenda* nodo, int* numero) {
    if (nodo != NULL) {
        visitaAgendaInOrder(getFiglioSinistroAgenda(nodo), numero);
        printf("%-3d %-12s %-15s %-15s\n", 
               *numero, 
               getDataIntervento(nodo),
               getFasciaOrariaIntervento(nodo),
               getCodiceRichiestaInAgenda(nodo));
        (*numero)++;
        visitaAgendaInOrder(getFiglioDestroAgenda(nodo), numero);
    }
}

/* Stampa l'agenda di un tecnico */
void stampaAgendaTecnico(Tecnico* tecnico) {
    if (tecnico == NULL) {
        printf("Tecnico non valido.\n");
        return;
    }
    
    printf("\n=== AGENDA TECNICO: %s (%s) ===\n\n", getNomeTecnico(tecnico), getCodiceTecnico(tecnico));
    
    AgendaTecnico* agenda = getAgendaTecnico(tecnico);
    if (agenda == NULL) {
        printf("Agenda non inizializzata.\n");
        return;
    }
    
    NodoAgenda* nodo = getRadiceAgenda(agenda);
    if (nodo == NULL) {
        printf("Nessun intervento pianificato.\n");
        return;
    }
    
    /* Visita in-order dell'albero dell'agenda */
    int numero = 1;
    printf("Interventi pianificati:\n");
    printf("%-3s %-12s %-15s %-15s\n", "#", "Data", "Fascia Oraria", "Codice Richiesta");
    printf("%-3s %-12s %-15s %-15s\n", "-", "----", "-------------", "----------------");
    
    visitaAgendaInOrder(nodo, &numero);
    printf("\n");
}