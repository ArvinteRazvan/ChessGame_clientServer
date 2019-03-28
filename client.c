/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.

   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>


/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
    int sd1;			// descriptorul de socket
    struct sockaddr_in server1;	// structura folosita pentru conectare
    // mesajul trimis
    int nr=0;
    char buf[10];

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi (argv[2]);

    /* cream socketul */
    if ((sd1 = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server1.sin_family = AF_INET;
    /* adresa IP a serverului */
    server1.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server1.sin_port = htons (port);

    /* ne conectam la server */
    if (connect (sd1, (struct sockaddr *) &server1,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }



    /* trimiterea mesajului la server
    if (write (sd1,&nr,sizeof(int)) <= 0)
    {
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
    }
    */
    /* citirea raspunsului dat de server
       (apel blocant pina cind serverul joc_sah) */
    char server1_answer[100];
    char mutare[100];
    int game_end=0;
    int raspuns=0;
    memset(server1_answer,0,100);
    raspuns=read(sd1, server1_answer,100);
    if (raspuns < 0) {
        perror("[client]Eroare la read() de la server1.\n");
        return errno;
    }
    else {
        printf("|%s|\n",server1_answer);
        if (strstr(server1_answer, "W")!=NULL) {

            printf("Stunteti culoarea alb.\n");
            printf("Mutati\n");
            printf("Formatul mutarii este BH1245 - culoare, piesa, (i,j) pozitia initiala, (i,j) pozitie finala\n");
            memset(mutare,0,100);
            scanf("%s", mutare);
            if (write(sd1, mutare, 100) <= 0) {
                perror("[client]Eroare la write() spre server1.\n");
                return errno;
            }
        }
        else{
            if (strstr(server1_answer, "B")!=NULL) {
                printf("Sunteti culoarea negru.\n");
                printf("asteptati sa muta albul.\n");
            }
            else
            {
                printf("Mesaj invalid |%s|",server1_answer);
            }
        }
    }
    memset(server1_answer,0,100);
    while (game_end==0) {
        memset(server1_answer,0,100);
        if (read(sd1, server1_answer, 100) < 0) {
            perror("[client]Eroare la read() de la server1.\n");
            return errno;
        }
        /* afisam mesajul primit */
        printf("[client]Mesajul primit este: %s\n", server1_answer);
        if (strstr(server1_answer,"You")!=NULL)
        {
            game_end=1;
            printf("%s \n",server1_answer);
            break;
        }
        if (strcmp(server1_answer,"ok")==0)
        {
            printf("mutare acceptata\n");
            continue;
        }
        if(server1_answer[0]=='B' || server1_answer[0]=='W') {
                //facem mutarea trimisa de server1
                printf("Mutati\n");
                printf("Formatul mutarii este BH1245 - culoare, piesa, (i,j) pozitia initiala, (i,j) pozitie finala\n");
                memset(mutare,0,100);
                scanf("%s", mutare);
                if (write(sd1, mutare, 100) <= 0) {
                    perror("[client]Eroare la write() spre server1.\n");
                    return errno;
                }
            game_end=0;
            }
        else {
            printf("Introduceti o mutare valida\n");
            memset(mutare,0,100);
            scanf("%s", mutare);
            if (write(sd1, mutare, 100) <= 0) {
                perror("[client]Eroare la write() spre server1.\n");
                return errno;
            }
            game_end=0;
        }

        }
    /* inchidem conexiunea, am terminat */
    close (sd1);
}
