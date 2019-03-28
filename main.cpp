

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>

/* portul folosit */
#define PORT1 2910
#define PORT2 2920

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl[2]; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int vefiricare_daca_piesa_este_acolo(char *mutare,char tabla_sah[9][9][2])
{
    //printf("ts:%c m:%c \n| ts:%c m:%c\n",tabla_sah[mutare[2]-'0'][mutare[3]-'0'][0],mutare[0], tabla_sah[mutare[2]-'0'][mutare[3]-'0'][1],mutare[1]);
    if (tabla_sah[mutare[2]-'0'][mutare[3]-'0'][0]==mutare[0] && tabla_sah[mutare[2]-'0'][mutare[3]-'0'][1]==mutare[1])
    {
        return 1;
    }
    return 0;
}

int verificare_mutare_piesa(char * mutare, char tabla_sah[9][9][2])
{
    int x,y,i,j;
    if (mutare[1]=='R')
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if ((x==0 && y!=0) || (x!=0 && y==0))
            return 1;
        if (((mutare[2]-'0')-(mutare[4]-'0')==0) &&((mutare[3]-'0')-(mutare[5]-'0'))<0)
        {
            for(j=mutare[3]-'0'; j<mutare[5]-'0';j++)
            {
                if(tabla_sah[mutare[2]-'0'][j][0]!='0')
                    return 2;
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')==0) &&((mutare[3]-'0')-(mutare[5]-'0'))>0)
        {
            for(j=mutare[3]-'0'; j>mutare[5]-'0';j--)
            {
                if(tabla_sah[mutare[2]-'0'][j][0]!='0')
                    return 2;
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')<0) &&((mutare[3]-'0')-(mutare[5]-'0'))==0)
        {
            for(i=mutare[2]-'0'; i<mutare[5]-'0';i++)
            {
                if(tabla_sah[i][mutare[3]-'0'][0]!='0')
                    return 2;
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')>0) &&((mutare[3]-'0')-(mutare[5]-'0'))==0)
        {
            for(i=mutare[2]-'0'; i>mutare[5]-'0';i--)
            {
                if(tabla_sah[i][mutare[3]-'0'][0]!='0')
                    return 2;
            }
        }
        return 0;
    }

    if (mutare[1]=='B')
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if (x==y || x==8-y || y==8-x)
            return 1;

        if (((mutare[2]-'0')-(mutare[4]-'0')<0) &&((mutare[3]-'0')-(mutare[5]-'0'))<0)
        {
            for (i=mutare[2]-'0';i>mutare[4]-'0';i--)
            {
                if (tabla_sah[i][i][0]!='0')
                {
                    return 2;
                }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')>0) &&((mutare[3]-'0')-(mutare[5]-'0'))>0)
        {
            for (i=mutare[2]-'0';i<mutare[4]-'0';i++)
            {
                if (tabla_sah[i][i][0]!='0')
                {
                    return 2;
                }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')>0) &&((mutare[3]-'0')-(mutare[5]-'0'))<0)
        {
            for (i=mutare[2]-'0';i<mutare[4]-'0';i++)
            {
                for(j=mutare[3]-'0';j>mutare[5]-'0';j--)
                    if (tabla_sah[i][j][0]!='0')
                    {
                        return 2;
                    }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')<0) &&((mutare[3]-'0')-(mutare[5]-'0'))>0)
        {
            for (i=mutare[2]-'0';i>mutare[4]-'0';i--)
            {
                for(j=mutare[3]-'0';j<mutare[5]-'0';j++)
                    if (tabla_sah[i][j][0]!='0')
                    {
                        return 2;
                    }
            }
        }
        return 0;
    }

    if (mutare[1]=='H')
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if (((mutare[2]-'0')-(mutare[4]-'0')==-2) &&((mutare[3]-'0')-(mutare[5]-'0'))==-1)
        {
            return 0;
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')==-1) &&((mutare[3]-'0')-(mutare[5]-'0'))==-2)
        {
            return 0;
        }
        if (((mutare[2]-'0')-(mutare[4]-'0')==-2) &&((mutare[3]-'0')-(mutare[5]-'0'))==1)
        {
            return 0;
        }
        if (((mutare[2]-'0')-(mutare[4]-'0')==-1) &&((mutare[3]-'0')-(mutare[5]-'0'))==2)
        {
            return 0;
        }
        if (((mutare[2]-'0')-(mutare[4]-'0')==1) &&((mutare[3]-'0')-(mutare[5]-'0'))==-2)
        {
            return 0;
        }
        if (((mutare[2]-'0')-(mutare[4]-'0')==2) &&((mutare[3]-'0')-(mutare[5]-'0'))==-1)
        {
            return 0;
        }
        if (((mutare[2]-'0')-(mutare[4]-'0')==1) &&((mutare[3]-'0')-(mutare[5]-'0'))==2)
        {
            return 0;
        }
        if (((mutare[2]-'0')-(mutare[4]-'0')==2) &&((mutare[3]-'0')-(mutare[5]-'0'))==1)
        {
            return 0;
        }
        return 1;
    }

    if(mutare[1]=='P') // White
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if(((mutare[2]-'0')-(mutare[4]-'0')==-1) && ((mutare[3]-'0')-(mutare[5]-'0')==0))
        {
            if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]!='0')
            {
                return 2;
            }
            return 0;
        }

        if(((mutare[2]-'0')-(mutare[4]-'0')==-1) && ((mutare[3]-'0')-(mutare[5]-'0')==-1))
        {
            if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]!='0')
            {
                return 0;
            }
            return 1;
        }

        if(((mutare[2]-'0')-(mutare[4]-'0')==-1) && ((mutare[3]-'0')-(mutare[5]-'0')==1))
        {
            if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]!='0')
            {
                return 0;
            }
            return 1;
        }


    }

    if(mutare[1]=='P') // black
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if(((mutare[2]-'0')-(mutare[4]-'0')==1) && ((mutare[3]-'0')-(mutare[5]-'0')==0))
        {
            if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]!='0')
            {
                return 2;
            }
            return 0;
        }

        if(((mutare[2]-'0')-(mutare[4]-'0')==1) && ((mutare[3]-'0')-(mutare[5]-'0')==-1))
        {
            if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]!='0')
            {
                return 0;
            }
            return 1;
        }

        if(((mutare[2]-'0')-(mutare[4]-'0')==1) && ((mutare[3]-'0')-(mutare[5]-'0')==1))
        {
            if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]!='0')
            {
                return 0;
            }
            return 1;
        }


    }

    if(mutare[1]=='K')
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if(x<=1 && y<=1)
        {
            return 0;
        }
        return 1;
    }
    if (mutare[1]=='Q')
    {
        x=abs((mutare[2]-'0')-(mutare[4]-'0'));
        y=abs((mutare[3]-'0')-(mutare[5]-'0'));
        if (x==y || x==8-y || y==8-x || (x==0 && y!=0) || (x!=0 && y==0))
            return 1;

        if (((mutare[2]-'0')-(mutare[4]-'0')<0) &&((mutare[3]-'0')-(mutare[5]-'0'))<0)
        {
            for (i=mutare[2]-'0';i>mutare[4]-'0';i--)
            {
                if (tabla_sah[i][i][0]!='0')
                {
                    return 2;
                }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')>0) &&((mutare[3]-'0')-(mutare[5]-'0'))>0)
        {
            for (i=mutare[2]-'0';i<mutare[4]-'0';i++)
            {
                if (tabla_sah[i][i][0]!='0')
                {
                    return 2;
                }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')>0) &&((mutare[3]-'0')-(mutare[5]-'0'))<0)
        {
            for (i=mutare[2]-'0';i<mutare[4]-'0';i++)
            {
                for(j=mutare[3]-'0';j>mutare[5]-'0';j--)
                    if (tabla_sah[i][j][0]!='0')
                    {
                        return 2;
                    }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')<0) &&((mutare[3]-'0')-(mutare[5]-'0'))>0)
        {
            for (i=mutare[2]-'0';i>mutare[4]-'0';i--)
            {
                for(j=mutare[3]-'0';j<mutare[5]-'0';j++)
                    if (tabla_sah[i][j][0]!='0')
                    {
                        return 2;
                    }
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')==0) &&((mutare[3]-'0')-(mutare[5]-'0'))<0)
        {
            for(j=mutare[3]-'0'; j<mutare[5]-'0';j++)
            {
                if(tabla_sah[mutare[2]-'0'][j][0]!='0')
                    return 2;
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')==0) &&((mutare[3]-'0')-(mutare[5]-'0'))>0)
        {
            for(j=mutare[3]-'0'; j>mutare[5]-'0';j--)
            {
                if(tabla_sah[mutare[2]-'0'][j][0]!='0')
                    return 2;
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')<0) &&((mutare[3]-'0')-(mutare[5]-'0'))==0)
        {
            for(i=mutare[2]-'0'; i<mutare[5]-'0';i++)
            {
                if(tabla_sah[i][mutare[3]-'0'][0]!='0')
                    return 2;
            }
        }

        if (((mutare[2]-'0')-(mutare[4]-'0')>0) &&((mutare[3]-'0')-(mutare[5]-'0'))==0)
        {
            for(i=mutare[2]-'0'; i>mutare[5]-'0';i--)
            {
                if(tabla_sah[i][mutare[3]-'0'][0]!='0')
                    return 2;
            }
        }
        return 0;
    }
}

int verificare_mutare_valida(char *mutare,char tabla_sah[9][9][2])
{
    if (((mutare[4]-'0')<=8 && (mutare[4]-'0')>=1) && ((mutare[5]-'0')<=8 && (mutare[5]-'0')>=1))
    {
        int ok=verificare_mutare_piesa(mutare,tabla_sah);
        return ok;
    }
    else
    {
        return 1;
    }
}
int verificare_mutare(char *mutare, char tabla_sah[9][9][2]) {
    int ok = vefiricare_daca_piesa_este_acolo(mutare, tabla_sah);
    if (ok == 0) {
        return 3;
    } else
    {
        ok=verificare_mutare_valida(mutare,tabla_sah);
        if (ok!=0) // in caz de vreo eroare trimitem codul de eroare
        {
            return ok;
        }
        // verificam daca acolo este o piesa de culoarea noastra, daca da , coliziune
        if (tabla_sah[mutare[4]-'0'][mutare[5]-'0'][0]==mutare[0]) {
            return 4;
        }
    }
    return 0;
}

int main ()
{
    struct sockaddr_in server1,server2;	// structura folosita de server1
    struct sockaddr_in from1,from2;
    int nr;		//mesajul primit de trimis la client
    int sd1,sd2;		//descriptorul de socket
    int pid;
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
    int i=0;


    /* crearea unui socket */
    if ((sd1 = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server1]Eroare la socket().\n");
        return errno;
    }
    if ((sd2 = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server1]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on=1;
    setsockopt(sd1,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    setsockopt(sd2,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    /* pregatirea structurilor de date */
    bzero (&server1, sizeof (server1));
    bzero (&server1, sizeof (server2));
    bzero (&from1, sizeof (from1));
    bzero (&from2, sizeof (from2));

    /* umplem structura folosita de server1 */
    /* stabilirea familiei de socket-uri */
    server1.sin_family = AF_INET;
    /* acceptam orice adresa */
    server1.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server1.sin_port = htons (PORT1);

    /* umplem structura folosita de server1 */
    /* stabilirea familiei de socket-uri */
    server2.sin_family = AF_INET;
    /* acceptam orice adresa */
    server2.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server2.sin_port = htons (PORT2);
    /* atasam socketul */
    if (bind (sd1, (struct sockaddr *) &server1, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server1]Eroare la bind().\n");
        return errno;
    }
    if (bind (sd2, (struct sockaddr *) &server2, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server1]Eroare la bind().\n");
        return errno;
    }
    /* punem server1ul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd1, 2) == -1)
    {
        perror ("[server1]Eroare la listen().\n");
        return errno;
    }
    if (listen (sd2, 2) == -1)
    {
        perror ("[server1]Eroare la listen().\n");
        return errno;
    }

    thData * td;
    td = (struct thData*)malloc(sizeof(struct thData)); //parametru functia executata de thread
    td->idThread=0;
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1) {
        int client1;
        int length1 = sizeof (from1);

        printf ("[server1]Asteptam la portul %d...\n",PORT1);
        fflush (stdout);

        //client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client1 = accept (sd1, (struct sockaddr *) &from1, (socklen_t *) &length1)) < 0)
        {
            perror ("[server1]Eroare la accept().\n");
            continue;
        }

        td->cl[0]=client1;
        int client2;
        int length2 = sizeof (from2);

        printf ("[server1]Asteptam la portul %d...\n",PORT2);
        fflush (stdout);

        //client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client2 = accept (sd2, (struct sockaddr *) &from2, (socklen_t *) &length2)) < 0)
        {
            perror ("[server1]Eroare la accept().\n");
            continue;
        }
        /* s-a realizat conexiunea, se astepta mesajul */

        int idThread; //id-ul threadului

        td->cl[1]=client2;
        td->idThread=i++;
        printf("[server1]A venit si clientul 2\n");
        pthread_create(&th[td->idThread], NULL, &treat, td);
    }//while
};
static void *treat(void * arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
    fflush (stdout);
    pthread_detach(pthread_self());
    raspunde((struct thData*)arg);//aici se intampla tot sahul
    /* am terminat cu acest client, inchidem conexiunea */
    close ((intptr_t)arg);
    return(NULL);

};


void raspunde(void *arg) {
    int id_fork;
    id_fork=fork();
    if (id_fork==0) {
        char tabla_sah[9][9][2];
        memset(tabla_sah,0,100);

        //initializare tabla sah
        tabla_sah[1][1][0]='W';
        tabla_sah[1][1][1]='R';
        tabla_sah[1][2][0]='W';
        tabla_sah[1][2][1]='H';
        tabla_sah[1][3][0]='W';
        tabla_sah[1][3][1]='B';
        tabla_sah[1][4][0]='W';
        tabla_sah[1][4][1]='Q';
        tabla_sah[1][5][0]='W';
        tabla_sah[1][5][1]='K';
        tabla_sah[1][6][0]='W';
        tabla_sah[1][6][1]='B';
        tabla_sah[1][7][0]='W';
        tabla_sah[1][7][1]='H';
        tabla_sah[1][8][0]='W';
        tabla_sah[1][8][1]='R';
        int i,j;
        for (i=1;i<=8;i++)
        {
            tabla_sah[2][i][0]='W';
            tabla_sah[2][i][1]='P';
        }

        tabla_sah[8][1][0]='B';
        tabla_sah[8][1][1]='R';
        tabla_sah[8][2][0]='B';
        tabla_sah[8][2][1]='H';
        tabla_sah[8][3][0]='B';
        tabla_sah[8][3][1]='B';
        tabla_sah[8][4][0]='B';
        tabla_sah[8][4][1]='Q';
        tabla_sah[8][5][0]='B';
        tabla_sah[8][5][1]='K';
        tabla_sah[8][6][0]='B';
        tabla_sah[8][6][1]='B';
        tabla_sah[8][7][0]='B';
        tabla_sah[8][7][1]='H';
        tabla_sah[8][8][0]='B';
        tabla_sah[8][8][1]='R';
        for (i=1;i<=8;i++)
        {
            tabla_sah[7][i][0]='B';
            tabla_sah[7][i][1]='P';
        }
        for (i=3;i<=6;i++)
        {
            for (j=1;j<=8;j++)
                tabla_sah[i][j][0]='0';
                tabla_sah[i][j][1]='0';
        }
        // initializam culorile pentru jucatori
        int x=rand()%2;
        int col_1=x;

        //trimitem la fiecare jucator ce culoare are si daca este la mutare
        //negru = B , alb = W
        //la mutare = 1, nu esti la mutare=0
        //format string W0/W1/B0/B1,   culoare.mutare
        thData tdL;
        tdL= *((struct thData*)arg);
        char alb='W';
        char black='B';
        int la_mutare=0;
        if (col_1==0) // 0 =white
        {
            if (write (tdL.cl[0], "W", sizeof(char)) <= 0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write() catre client. trimitere culoare\n");
            }
            else {
                printf("[Thread %d]Culoarea player 1 a fost trasmis cu success \n", tdL.idThread);
            }
            if (write (tdL.cl[1], "B", sizeof(char)) <= 0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write() catre client. trimitere culoare\n");
            }
            else {
                printf("[Thread %d]Culoarea player 2 a fost trasmis cu success \n", tdL.idThread);
            }
            la_mutare=0;
        }
        else
        {
            if (write (tdL.cl[0], "B", sizeof(char)) <= 0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write() catre client. trimitere culoare\n");
            }
            else {
                printf("[Thread %d]Culoarea player 1 a fost trasmis cu success \n", tdL.idThread);
            }
            if (write (tdL.cl[1], "W", sizeof(char)) <= 0)
            {
                printf("[Thread %d] ",tdL.idThread);
                perror ("[Thread]Eroare la write() catre client. trimitere culoare\n");
            }
            else {
                printf("[Thread %d]Culoarea player 2 a fost trasmis cu success \n", tdL.idThread);
            }
            la_mutare=1;
        }
        int game_end=0;// game_end= false
        char mutare[100];
        int result=0;
        while(game_end==0) {
            game_end=1;
            memset(mutare,0,100);
            if (read (tdL.cl[la_mutare], mutare,100) <= 0)
            {
                printf("[Thread %d]Eroare la citire mutare de la player %d.\n",tdL.idThread,la_mutare);
                printf("one of the clients disconected");
                break;
            }
            else {
                //printf("mutare:%s\n",mutare);
                //printf("%c | %c\n",tabla_sah[2][1][0],tabla_sah[2][1][1]);
                result = verificare_mutare(mutare, tabla_sah);
                //printf("|result:%d|\n",result);
                switch (result) {
                    case 0: {
                        //correct
                        //printf("la_mutare abs:%d\n",abs(la_mutare-1));
                        if (write(tdL.cl[abs(la_mutare - 1)], mutare, 100) <= 0) {
                            printf("[Thread %d]Eroare la scriere mutare de la player %d.\n", tdL.idThread,
                                   abs(la_mutare - 1));
                        } else {
                            printf("[Thread %d]Mutarea la player %d a fost trasmis cu success \n", tdL.idThread,
                                   abs(la_mutare - 1));
                        }
                        char aux[5] = "ok";
                        //printf("la_mutare abs:%d\n",la_mutare);
                        if (write(tdL.cl[la_mutare], aux, 100) <= 0) {
                            printf("[Thread %d]Eroare la scriere ok pentru playerul care a mutat %d.\n", tdL.idThread,
                                   abs(la_mutare - 1));
                        } else {
                            printf("[Thread %d]Playerul %d a primit ok \n", tdL.idThread, abs(la_mutare - 1));
                        }
                        tabla_sah[mutare[2] - '0'][mutare[3] - '0'][0] = '0';
                        tabla_sah[mutare[2] - '0'][mutare[3] - '0'][1] = '0';
                        tabla_sah[mutare[4] - '0'][mutare[5] - '0'][0] = mutare[0];
                        tabla_sah[mutare[4] - '0'][mutare[5] - '0'][1] = mutare[1];

                        char col;
                        if (la_mutare==0)
                        {
                            col='B';
                        } else{
                            col='W';
                        }
                        for (i=1;i<=8;i++)
                            for (j=1;j<=8;j++)
                            {
                                if (tabla_sah[i][j][0]==col && tabla_sah[i][j][1]=='K')
                                {
                                    game_end=0;
                                }
                            }
                        if (game_end==1)
                        {

                            if (write (tdL.cl[la_mutare], "You won", 100) <= 0)
                            {
                                printf("[Thread %d]Eroare la scriere mutare de la player %d.\n",tdL.idThread,abs(la_mutare));
                            }
                            else {
                                printf("[Thread %d]Culoarea player %d a fost trasmis cu success \n", tdL.idThread,abs(la_mutare));
                            }
                            if (write (tdL.cl[la_mutare-1], "You lost", 100) <= 0)
                            {
                                printf("[Thread %d]Eroare la scriere mutare de la player %d.\n",tdL.idThread,abs(la_mutare-1));
                            }
                            else {
                                printf("[Thread %d]Culoarea player %d a fost trasmis cu success \n", tdL.idThread,abs(la_mutare-1));
                            }
                            break;

                        }
                        la_mutare=abs(la_mutare-1);
                        break;
                    }
                    case 1: {
                        //write piesa iese inafara tablei - miscare invalida
                        char mes[] = "piesa iese inafara tablei - miscare invalida";
                        printf(" piesa iese inafara tablei - miscare invalida\n");
                        if (write(tdL.cl[la_mutare], mes, 100) <= 0) {
                            printf("[Thread %d]Eroare la scriere mesaj %s pentru player player %d.\n", tdL.idThread,
                                   mes, la_mutare);
                        } else {
                            printf("[Thread %d]Mesajul de eroare %s pentru player %d a fost trasmis cu success \n",
                                   tdL.idThread, mes, la_mutare);
                        }
                        game_end=0;
                        break;
                    }
                    case 2: {
                        //coliziune
                        char mes[] = "Coliziune, cel putin o piesa blocheaza miscarea";
                        printf("Coliziune, o piesa blocheaza miscarea\n");
                        if (write(tdL.cl[la_mutare], mes, 100) <= 0) {
                            printf("[Thread %d]Eroare la scriere mesaj %s pentru player player %d.\n", tdL.idThread,
                                   mes, la_mutare);
                        } else {
                            printf("[Thread %d]Mesajul de eroare %s pentru player %d a fost trasmis cu success \n",
                                   tdL.idThread, mes, la_mutare);
                        }
                        game_end=0;
                        break;
                    }
                    case 3: {
                        //piesa nu a plecat de pe un punct valabil ,comanda invalida de la client
                        char mes[] = "Punctul de plecare a piersei nu este valabil";
                        printf("Punctul de plecare a piesei nu este valabil\n");
                        if (write(tdL.cl[la_mutare], mes, 100) <= 0) {
                            printf("[Thread %d]Eroare la scriere mesaj %s pentru player player %d.\n", tdL.idThread,
                                   mes, la_mutare);
                        } else {
                            printf("[Thread %d]Mesajul de eroare %s pentru player %d a fost trasmis cu success \n",
                                   tdL.idThread, mes, la_mutare);
                        }
                        game_end=0;
                        break;
                    }
                    case 4: {
                        //isi mananca propria piesa
                        char mes[] = "Nu iti poti manca propriile piese";
                        printf(" Nu iti poti manca propriile piese\n");
                        if (write(tdL.cl[la_mutare], mes, 100) <= 0) {
                            printf("[Thread %d]Eroare la scriere mesaj %s pentru player player %d.\n", tdL.idThread,
                                   mes, la_mutare);
                        } else {
                            printf("[Thread %d]Mesajul de eroare %s pentru player %d a fost trasmis cu success \n",
                                   tdL.idThread, mes, la_mutare);
                        }
                        game_end=0;
                        break;
                    }
                    default: {
                        printf("HACKS");
                        // nu ar trebui sa se ajunga aici niciuodata
                    }
                }

            }

        }
    }
}
