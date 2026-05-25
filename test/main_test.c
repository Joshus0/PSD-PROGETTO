/*
 * File: main_test.c
 * Scopo: Suite di testing automatizzata per il sistema di gestione del condominio.
 * Fornisce un'interfaccia a riga di comando per eseguire singoli casi di test
 * o l'intera suite, validando il comportamento delle strutture dati tramite assert
 * e pattern "Golden Master" (confronto con file Oracolo).
 * L'information hiding e' rigorosamente mantenuto: ogni accesso alle strutture
 * avviene esclusivamente tramite le funzioni getter pubbliche degli ADT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "entita/richiesta.h"
#include "entita/tecnico.h"
#include "archivioRichieste.h"
#include "codaPriorita.h"
#include "alberoTecnici.h"
#include "main/utilita.h"
#include "Funzioni/Test.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"




/*
 * Funzione: main
 * --------------
 * Punto di ingresso della suite di testing. Presenta un menu interattivo
 * e delega l'esecuzione ai singoli test case. L'opzione 10 esegue tutti
 * i test in sequenza, bloccandosi al primo fallimento grazie agli assert.
 *
 * Parametri:
 * Nessuno.
 *
 * Pre-condizione:
 * L'ambiente deve supportare sequenze escape ANSI per la colorazione.
 *
 * Post-condizione:
 * Il programma termina normalmente con la selezione dello "0".
 * Se un assert fallisce internamente, il programma termina con abort().
 *
 * Ritorna:
 * 0 al completamento corretto.
 */
int main(void) {
    int scelta = -1;
    setvbuf(stdout, NULL, _IONBF, 0);

    while (scelta != 0) {
        pulisciSchermo();
        printf(CYAN BOLD " ________________________________________________________________________________________ \n");
        printf("|                                                                                        |\n");
        printf("|                  " RESET BOLD "SUITE DI TESTING AUTOMATIZZATA (CON ASSERT)" CYAN BOLD "                           |\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [1] Reg. Richieste (da file)              " CYAN BOLD "|" RESET " [5] Aggiornamento Stato                    " CYAN BOLD "|\n");
        printf("|" RESET " [2] Reg. Tecnici (da file)                " CYAN BOLD "|" RESET " [6] Ricerca e Filtri                       " CYAN BOLD "|\n");
        printf("|" RESET " [3] Assegnazione Corretta                 " CYAN BOLD "|" RESET " [7] Storico Interventi                     " CYAN BOLD "|\n");
        printf("|" RESET " [4] Pianificazione e Conflitti            " CYAN BOLD "|" RESET " [8] Generazione Report                     " CYAN BOLD "|\n");
        printf("|" RESET " [9] Multi-slot Stesso Giorno              " CYAN BOLD "|" RESET " [0] Esci dalla Suite                       " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n");
        printf("|" RESET " [10] Esegui TUTTI i Test in Sequenza      " CYAN BOLD "|" YELLOW BOLD " " CYAN BOLD "                                           " CYAN BOLD "|\n");
        printf("|___________________________________________|____________________________________________|\n" RESET);
        printf(BOLD YELLOW "\n>> Seleziona il caso di test da avviare: " RESET);

        if (scanf("%d", &scelta) != 1) {
            pulisciBuffer();
            scelta = -1;
            continue;
        }
        pulisciBuffer();

        switch (scelta) {
            case 1: pulisciSchermo(); testCase1_RegistrazioneRichieste(); pausaSchermo(); break;
            case 2: pulisciSchermo(); testCase2_RegistrazioneTecnici();   pausaSchermo(); break;
            case 3: pulisciSchermo(); testCase3_AssegnazioneCorretta();   pausaSchermo(); break;
            case 4: pulisciSchermo(); testCase4_PianificazioneConflitti(); pausaSchermo(); break;
            case 5: pulisciSchermo(); testCase5_AggiornamentoStato();     pausaSchermo(); break;
            case 6: pulisciSchermo(); testCase6_RicercaFiltri();          pausaSchermo(); break;
            case 7: pulisciSchermo(); testCase7_StoricoInterventi();      pausaSchermo(); break;
            case 8: pulisciSchermo(); testCase8_GenerazioneReport();      pausaSchermo(); break;
            case 9: pulisciSchermo(); testCase9_MultiSlotGiornoStesso();  pausaSchermo(); break;
            case 10:
                pulisciSchermo(); testCase1_RegistrazioneRichieste(); pausaSchermo();
                pulisciSchermo(); testCase2_RegistrazioneTecnici();   pausaSchermo();
                pulisciSchermo(); testCase3_AssegnazioneCorretta();   pausaSchermo();
                pulisciSchermo(); testCase4_PianificazioneConflitti(); pausaSchermo();
                pulisciSchermo(); testCase5_AggiornamentoStato();     pausaSchermo();
                pulisciSchermo(); testCase6_RicercaFiltri();          pausaSchermo();
                pulisciSchermo(); testCase7_StoricoInterventi();      pausaSchermo();
                pulisciSchermo(); testCase8_GenerazioneReport();      pausaSchermo();
                pulisciSchermo(); testCase9_MultiSlotGiornoStesso();  pausaSchermo();
                break;
            case 0: break;
            default:
                printf(RED BOLD "\n [ ERRORE ] Opzione non valida.\n" RESET);
                pausaSchermo();
                break;
        }
    }
    return 0;
}