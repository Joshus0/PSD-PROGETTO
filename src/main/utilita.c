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