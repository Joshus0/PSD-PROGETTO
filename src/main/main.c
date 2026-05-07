#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Inclusioni corrette per il nuovo progetto */
#include "entita/tecnico.h"
#include "entita/richiesta.h"
#include "alberoTecnici.h"
#include "codaPriorita.h"
#include "archivioRichieste.h"
#include "agendaTecnico.h"
#include "main/utilita.h"

/* --- DEFINIZIONE COLORI ANSI --- */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

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

int main() {
    /* 1. Inizializzazione delle nuove strutture ADT */
    AlberoTecnici* databaseTecnici = creaAlberoTecnici();
    
    /* FIX: Aggiunta la capacità iniziale (es. 100) richiesta dalla tua firma */
    CodaPriorita* codaAttesa = creaCodaPriorita(100); 
    
    ArchivioRichieste* archivioStorico = creaArchivioRichieste();
    
    int scelta = -1;
    char bufferCodice[50], bufferNome[50], bufferSpec[50];

    pulisciSchermo();
    printf(CYAN BOLD "=========================================\n");
    printf(" SISTEMA GESTIONE MANUTENZIONE CONDOMINIO \n");
    printf("=========================================\n" RESET);
    
    printf(BOLD "\nPremi INVIO per iniziare..." RESET);
    getchar();

    /* Ciclo principale (rigorosamente while) */
    while (scelta != 0) {
        pulisciSchermo();

        printf(CYAN BOLD "\n--- MENU PRINCIPALE ---\n" RESET);
        printf("1. Registra Nuovo Tecnico\n");
        printf("2. Inserisci Nuova Richiesta\n");
        printf("3. Assegna Richiesta Prioritaria (Logica Automatica)\n");
        printf("4. Visualizza Elenco Tecnici\n");
        printf("5. Visualizza Coda di Attesa (Heap)\n");
        printf("0. Esci dal Programma\n");
        printf(BOLD "Seleziona un'opzione: " RESET);

        if (scanf("%d", &scelta) != 1) {
            printf(RED BOLD "\n[ERRORE]" RESET RED " Inserisci un numero valido.\n" RESET);
            pulisciBuffer();
            pausaSchermo();
            continue;
        }
        pulisciBuffer();

        switch (scelta) {
            case 1:
                pulisciSchermo();
                printf(CYAN BOLD "\n-- REGISTRAZIONE NUOVO TECNICO --\n" RESET);
                
                printf(BOLD "Codice (es. T01): " RESET);
                fgets(bufferCodice, 50, stdin);
                bufferCodice[strcspn(bufferCodice, "\n")] = 0;

                printf(BOLD "Nome: " RESET);
                fgets(bufferNome, 50, stdin);
                bufferNome[strcspn(bufferNome, "\n")] = 0;

                printf(BOLD "Specializzazione (es. Idraulico): " RESET);
                fgets(bufferSpec, 50, stdin);
                bufferSpec[strcspn(bufferSpec, "\n")] = 0;

                /* Logica di inserimento nell'Albero (BST) */
                Tecnico* nuovoTecnico = creaTecnico(bufferCodice, bufferNome, bufferSpec);
                
                /* FIX TEMPORANEO: Distruggiamo subito il tecnico per non avere il warning "unused variable" */
                /* Finché non implementiamo l'inserimento vero e proprio nell'albero */
                distruggiTecnico(nuovoTecnico); 
                
                printf(GREEN BOLD "\n[OK]" RESET GREEN " Tecnico registrato con successo (Da collegare ad AlberoTecnici)!\n" RESET);
                pausaSchermo();
                break;

            case 2:
                pulisciSchermo();
                printf(CYAN BOLD "\n-- INSERIMENTO RICHIESTA --\n" RESET);
                printf(YELLOW "Funzionalita' in sviluppo: richiede integrazione con CodaPriorita e Archivio.\n" RESET);
                pausaSchermo();
                break;

            case 3:
                pulisciSchermo();
                printf(CYAN BOLD "\n-- ASSEGNAZIONE AUTOMATICA --\n" RESET);
                printf(YELLOW "Funzionalita' in sviluppo: estrazione da Heap e ricerca in BST.\n" RESET);
                pausaSchermo();
                break;

            case 4:
                pulisciSchermo();
                printf(CYAN BOLD "\n-- ELENCO TECNICI (IN ORDINE ALFABETICO) --\n" RESET);
                printf(YELLOW "Stampa albero in sviluppo...\n" RESET);
                pausaSchermo();
                break;

            case 5:
                pulisciSchermo();
                printf(CYAN BOLD "\n-- CODA DI ATTESA (PRIORITA' MAX) --\n" RESET);
                printf(YELLOW "Stampa heap in sviluppo...\n" RESET);
                pausaSchermo();
                break;

            case 0:
                pulisciSchermo();
                printf(YELLOW BOLD "\nPulizia memoria e uscita in corso... Arrivederci!\n\n" RESET);
                break;

            default:
                printf(RED BOLD "\n[ERRORE]" RESET RED " Opzione non valida. Riprova.\n" RESET);
                pausaSchermo();
        }
    }

    /* 3. Pulizia Memoria finale (Zero Memory Leak) - ORA DECOMMENTATE */
    distruggiAlberoTecnici(databaseTecnici);
    distruggiCodaPriorita(codaAttesa);
    distruggiArchivioRichieste(archivioStorico);

    return 0;
}