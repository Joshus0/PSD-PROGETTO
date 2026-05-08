#ifndef UTILITA_H
#define UTILITA_H

/* Ritorna una copia allocata dinamicamente di src, 
   oppure NULL se src è NULL o se malloc fallisce */
char* duplicaStringa(const char* src);

int validaData(const char* data);

#endif