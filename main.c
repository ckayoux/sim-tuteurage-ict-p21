#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structuresDonnees.h"
#include "loisProba.h"

//FONCTIONS D'INTERFACE
void nl();void nl2();
void dhl();
void ddl();
void blank();
void notifier(const char* msg,lot_plantes* P);
void notifier_light(const char* msg);
void ask(const char* question);
int yesno(const char* question);
int get_int(int minimum);
double get_double(double minimum);
void menu_parametres();
void liste_parametres_actuels();
void afficher_tableau_files(queue** tab,int deb,int fin);
#define TRUE 1
#define FALSE 0
#define PARAM_AUTO 0
#define PARAM_CUSTOM 1

//EVENEMENTS
typedef struct{
    int etat;
    lot_plantes* P;
}serveur;
serveur creer_serveur();

#define NB_EVENEMENTS 6
typedef struct{
    double date[NB_EVENEMENTS];
    unsigned char type_evenement[NB_EVENEMENTS];
    lot_plantes* lot[NB_EVENEMENTS];
}t_echeancier;
t_echeancier echeancier;
void initialiser_echeancier();
void ajouter_evenement(double instant,unsigned char type,lot_plantes* lot_concerne,serveur* S);
void supprimer_evenement();

serveur Mise_sur_tapis, Soufflerie, Nettoyage, Tuteurage_attachage, Decoupe, Dechargement ;
queue *Stock_initial,*Stock_soufflerie, *Stock_nettoyage, *Stock_tuteurage_attachage, *Stock_decoupe, *Stock_dechargement, *Stock_final ;
#define FIN_MISE_SUR_TAPIS 1
void fin_mise_sur_tapis();
#define FIN_SOUFFLERIE 2
void fin_soufflerie();
#define FIN_NETTOYAGE 3
void fin_nettoyage();
#define FIN_TUTEURAGE_ATTACHAGE 4
 void fin_tuteurage_attachage();
#define FIN_DECOUPE 5
 void fin_decoupe();
 #define FIN_DECHARGEMENT 6
 void fin_dechargement();

//VARIABLES GLOBALES
double t=0; //horloge
unsigned char type_evenement;
lot_plantes* P;

    //parametres
char mode_parametres=PARAM_CUSTOM;
char mode_affichage=0;
unsigned int TOTAL_PLANTES= 50;
double ESPERANCE_MISE_SUR_TAPIS = 12, ECART_MISE_SUR_TAPIS = 3;
double DUREE_SOUFFLERIE = 8;
double ESPERANCE_NETTOYAGE = 10,ECART_NETTOYAGE=2;  //ajouter ici le retard dû a des pots qui tombent, le temps de trajet neglige ...
double DUREE_TUTEURAGE_ATTACHAGE = 15;
double DUREE_DECOUPE = 5;
double ESPERANCE_DECHARGEMENT = 10, ECART_DECHARGEMENT = 2;

int main(int argc, char* argv[]){
    srand(time(NULL));

    puts("**********************************************");
    printf("\t\t\t\t  SIMULATION\n\t\tSystème de tuteurage automatisé\n\a");
    puts("**********************************************");
    nl();

    menu_parametres();

    dhl();
    printf("\nDEBUT DE LA SIMULATION\n\a");
    dhl();
    mode_affichage=yesno("Voulez-vous activer l'affichage des événements");

    Mise_sur_tapis=Soufflerie=Nettoyage=Tuteurage_attachage=Decoupe=Dechargement=creer_serveur();
    Stock_initial = creer_queue(TOTAL_PLANTES,"Initiale    ");
    Stock_soufflerie=creer_queue(0,"Soufflerie  ");
    Stock_nettoyage=creer_queue(0,"Nettoyage   ");
    Stock_tuteurage_attachage=creer_queue(0,"Tuteurage   ");
    Stock_decoupe=creer_queue(0,"Découpe     ");
    Stock_dechargement=creer_queue(0,"Dechargement");
    Stock_final=creer_queue(0,"Finale      ");
    queue* tab_queues[NB_EVENEMENTS+1];
    tab_queues[0]=Stock_initial; tab_queues[1]=Stock_soufflerie; tab_queues[2] = Stock_nettoyage; tab_queues[3]=Stock_tuteurage_attachage; tab_queues[4]=Stock_decoupe;
    tab_queues[5]=Stock_dechargement; tab_queues[6]=Stock_final;

    initialiser_echeancier();
    ajouter_evenement(t+generer_normale(ESPERANCE_MISE_SUR_TAPIS,ECART_MISE_SUR_TAPIS),FIN_MISE_SUR_TAPIS,defiler(Stock_initial,t),&Mise_sur_tapis);

    while(Stock_final->nb<TOTAL_PLANTES){
        t=echeancier.date[0];
        type_evenement=echeancier.type_evenement[0];
        P=echeancier.lot[0];
        supprimer_evenement();

        switch(type_evenement){
            case FIN_MISE_SUR_TAPIS:
                fin_mise_sur_tapis();
                break;

            case FIN_SOUFFLERIE:
                fin_soufflerie();
                break;

            case FIN_NETTOYAGE:
                fin_nettoyage();
                break;

            case FIN_TUTEURAGE_ATTACHAGE:
                fin_tuteurage_attachage();
                break;

            case FIN_DECOUPE:
                fin_decoupe();
                break;

            case FIN_DECHARGEMENT:
                fin_dechargement();
                break;

            default:
                printf("??\n");
        }

    }
    nl();
    dhl();nl();
    puts("PERFORMANCES :\a");
    dhl();
    nl();
    printf("\tCadence de production :\n");
    ddl();
    printf("\t%d lots de 8 plantes (soit %d pots) traités en %d secondes (soit %.1lf minutes).\n",Stock_final->nb,(Stock_final->nb)*8,(int)ceil(t),t/60);
    printf("\tDébit : %.2lf pots par heure.\n",(Stock_final->nb)*8/(t/3600));
    nl();
    nl();
    afficher_tableau_files(tab_queues,1,5);

    for(int i=0; i<NB_EVENEMENTS; i++){
        queue* Q = tab_queues[i];
        suppr_queue(Q);
    }
    nl();
	puts("Appuyez sur Entree pour quitter.");
    while(getchar()!='\n');
	return 0;
}


//EVENEMENTS
void initialiser_echeancier(){
    for(int i =0 ; i<NB_EVENEMENTS;i++){
        echeancier.date[i] = -1;
    }
}
void ajouter_evenement(double instant,unsigned char type,lot_plantes* lot_concerne,serveur* S){
    int i =0;
    if(echeancier.date[0]==-1) blank();
    else{
        while(echeancier.date[i]<instant && echeancier.date[i+1]!=-1){
            i++;
        }
        if(echeancier.date[i]<instant&&echeancier.date[i+1]==-1)i++;
    }

    for(int j=NB_EVENEMENTS-1; j>i; j--){
        echeancier.date[j]=echeancier.date[j-1];
        echeancier.type_evenement[j]=echeancier.type_evenement[j-1];
        echeancier.lot[j]=echeancier.lot[j-1];
    }
    echeancier.date[i]= instant;
    echeancier.type_evenement[i]=type;
    echeancier.lot[i]=lot_concerne;
    S->etat=1;
    S->P=lot_concerne;
}
void supprimer_evenement(){
    for(int i = 0; i<NB_EVENEMENTS-1; i++){
        echeancier.date[i]=echeancier.date[i+1];
        echeancier.type_evenement[i]=echeancier.type_evenement[i+1];
        echeancier.lot[i]=echeancier.lot[i+1];
    }
    echeancier.date[NB_EVENEMENTS-1]=-1;
}

serveur creer_serveur(){
    serveur S;
    S.etat=0;
    S.P=NULL;
    return S;
}

void fin_mise_sur_tapis(){
    lot_plantes* P = Mise_sur_tapis.P;
    notifier("Lot %d chargé sur le tapis.",P);

    if(Soufflerie.etat ==0){
        notifier_light("envoyé a la soufflerie.");
        Mise_sur_tapis.P=NULL;
        ajouter_evenement(t+DUREE_SOUFFLERIE,FIN_SOUFFLERIE,P,&Soufflerie);
    }
    else{
        enfiler(Stock_soufflerie,P,t);
        notifier_light("envoyé dans le stock avant la soufflerie.");
    }

    if((P=defiler(Stock_initial,t))!=NULL){
        ajouter_evenement(t+generer_normale(ESPERANCE_MISE_SUR_TAPIS,ECART_MISE_SUR_TAPIS),FIN_MISE_SUR_TAPIS,P,&Mise_sur_tapis);

    }
    else{
        (mode_affichage)?("Il n'y a plus aucun lot a charger sur le tapis."):blank();
        Mise_sur_tapis.etat=0;
    }

    nl2();

}

void fin_soufflerie(){
    lot_plantes* P = Soufflerie.P;
    notifier("Lot %d sorti de la soufflerie.",P);

    if(Nettoyage.etat ==0){
        Soufflerie.P=NULL;
        notifier_light("envoyé au nettoyage.");
        ajouter_evenement(t+generer_normale(ESPERANCE_NETTOYAGE,ECART_NETTOYAGE),FIN_NETTOYAGE,P,&Nettoyage);
    }
    else{
        enfiler(Stock_nettoyage,P,t);
        notifier_light("envoyé dans le stock avant le nettoyage manuel.");
    }

    if((P=defiler(Stock_soufflerie,t))!=NULL){
        ajouter_evenement(t+DUREE_SOUFFLERIE,FIN_SOUFFLERIE,P,&Soufflerie);
    }
    else{
        notifier_light("Soufflerie disponible.");
        Soufflerie.etat=0;
    }
    nl2();

}

void fin_nettoyage(){
    lot_plantes* P = Nettoyage.P;
    notifier("Lot %d sorti du poste de nettoyage manuel.",P);

    if(Tuteurage_attachage.etat ==0){
        Nettoyage.P=NULL;
        notifier_light("envoyé au tuteurage et attachage.");
        ajouter_evenement(t+DUREE_TUTEURAGE_ATTACHAGE,FIN_TUTEURAGE_ATTACHAGE,P,&Tuteurage_attachage);
    }
    else{
        enfiler(Stock_tuteurage_attachage,P,t);
        notifier_light("envoyé dans le stock avant tuteurage et attachage.");
    }

    if((P=defiler(Stock_nettoyage,t))!=NULL){
        ajouter_evenement(t+generer_normale(ESPERANCE_NETTOYAGE,ECART_NETTOYAGE),FIN_NETTOYAGE,P,&Nettoyage);
    }
    else{
        notifier_light("Poste de nettoyage disponible.");
        Nettoyage.etat=0;
    }
    nl2();
}

void fin_tuteurage_attachage(){
    lot_plantes* P = Tuteurage_attachage.P;
    notifier("Lot %d sorti du poste de tuteurage et d'attachage.",P);

    if(Decoupe.etat ==0){
        Tuteurage_attachage.P=NULL;
        notifier_light("envoyé a la taille.");
        ajouter_evenement(t+DUREE_DECOUPE,FIN_DECOUPE,P,&Decoupe);
    }
    else{
        enfiler(Stock_decoupe,P,t);
        notifier_light("envoyé dans le stock avant la taille.");
    }

    if((P=defiler(Stock_tuteurage_attachage,t))!=NULL){
        ajouter_evenement(t+DUREE_TUTEURAGE_ATTACHAGE,FIN_TUTEURAGE_ATTACHAGE,P,&Tuteurage_attachage);
    }
    else{
        notifier_light("Poste de tuteurage et d'attachage disponible.");
        Tuteurage_attachage.etat=0;
    }
    nl2();
}

void fin_decoupe(){
    lot_plantes* P = Decoupe.P;
    notifier("Lot %d sorti du poste de taille.",P);

    if(Dechargement.etat ==0){
        Decoupe.P=NULL;
        ajouter_evenement(t+generer_normale(ESPERANCE_DECHARGEMENT,ECART_DECHARGEMENT),FIN_DECHARGEMENT,P,&Dechargement);
    }
    else{
        enfiler(Stock_dechargement,P,t);
        notifier_light("pret a etre decharge");
    }

    if((P=defiler(Stock_decoupe,t))!=NULL){
        ajouter_evenement(t+DUREE_DECOUPE,FIN_DECOUPE,P,&Decoupe);
    }
    else{
        notifier_light("Poste de decoupe disponible.");
        Decoupe.etat=0;
    }
    nl2();
}

void fin_dechargement(){
    lot_plantes* P = Dechargement.P;
    notifier("Lot %d decharge.",P);
    enfiler(Stock_final,P,t);

    if((P=defiler(Stock_dechargement,t))!=NULL){
        ajouter_evenement(t+generer_normale(ESPERANCE_DECHARGEMENT,ECART_DECHARGEMENT),FIN_DECHARGEMENT,P,&Dechargement);
    }
    else{
        notifier_light("Poste de dechargement disponible.");
        Dechargement.etat=0;
    }
    nl2();
}

//FONCTIONS D'INTERFACE
void nl() {
    putchar('\n');
}
void nl2() {
    (mode_affichage)?putchar('\n'):blank();
}
void dhl(){
    puts("_____________________________________________________________________");
}
void ddl(){
    puts("\t------------------------------------------------");
}
void blank(){
    printf("");
}
void ask(const char* question) {
    printf("%s\n\t>>",question);
    return;
}
int yesno(const char* question) {
    char reponse,c;
    printf("%s ? (o/n) :\n\t>>",question);
    do{
        scanf("%c",&reponse);while ((c = getchar()) != '\n' && c != EOF); 
        nl();
        if(reponse=='O' ||reponse =='o') return TRUE;
        else if(reponse=='N' ||reponse=='n') return FALSE;
        else ask("! -- Saisie incorrecte : veuillez entrer 'o' (oui) ou 'n' (non) -- !");
    }while(1);
}
int get_int(int minimum){
    int reponse; char c;
    scanf("%d",&reponse);;while ((c = getchar()) != '\n' && c != EOF); 
    while(reponse<minimum){
        printf("! -- Saisie incorrecte : l'entier saisi doit etre superieur ou egal a %d -- !",minimum);
        ask("Veuillez reessayer :");
    }
    return reponse;
}
double get_double(double minimum){
    double reponse;char c;
    scanf("%lf",&reponse);;while ((c = getchar()) != '\n' && c != EOF); 
    while(reponse<minimum){
        printf("! -- Saisie incorrecte : le reel saisi doit etre superieur ou egal a %lf -- !",minimum);
        ask("Veuillez reessayer :");
    }
    return reponse;
}
void notifier(const char* msg,lot_plantes* P){
    char msg2[80];
    if(P->id)sprintf(msg2,msg,P->id);
    else strcpy(msg2,msg);
    (mode_affichage)?printf("%.1lf : %s\n",t,msg2):blank();
}
void notifier_light(const char* msg){
    (mode_affichage)?printf("\t-> %s\n",msg):blank();
}

void afficher_tableau_files(queue** tab,int deb,int fin){
    printf("\tTemps d'attente (en secondes) dans les files :\n");
    ddl();
    nl();

    printf("\tFILE         :\t\t\t  TOTAL :\t\t\t MOYEN :\t\tEFFECTIF MAX:\n");
    puts("\t_______________________________________________________________________________");
    for(int i=deb; i<=fin; i++){
        if(tab[i]->attente==0) printf("\t%s : \t\t\t        -\t\t\t     -     \t\t     %d\n",tab[i]->nom,0);
        else {
            printf("\t%s : \t\t\t%10.1lf\t\t\t%7.1lf\t\t\t    %2d\n",tab[i]->nom,tab[i]->attente,tab[i]->attente/tab[i]->nb_total,tab[i]->nb_max);
        }
    }
    nl();
    return;
}

void menu_parametres(){
    puts("PARAMETRES\a");
    dhl();
    mode_parametres=!yesno("Souhaitez-vous utiliser les paramètres par défaut");
    nl();
    if(mode_parametres==PARAM_CUSTOM){
            printf("\tMODE MANUEL\n");
            ddl();
			int requete=-1;
			do{
				nl();
				liste_parametres_actuels();
				nl();
				ask("Entrez le numéro correspondant à votre requête :");
				requete=get_int(0);
				switch(requete){
					case 1:
						ask("Stock initial (nombre total de lots de 8 plantes à tuteurer) :");
						TOTAL_PLANTES = (unsigned int)get_int(1);
						break;

					case 2:
						ask("Espérance et écart-type pour le chargement des lots (en secondes) :");
               			ESPERANCE_MISE_SUR_TAPIS = get_double(1);ask("");
              			ECART_MISE_SUR_TAPIS = get_double(0);
						break;

					case 3:
						ask("Durée de la soufflerie (en secondes) :");
						DUREE_SOUFFLERIE = get_double(1);
						break;

					case 4:
						ask("Espérance et écart-type pour le nettoyage manuel (en secondes) :");
                		ESPERANCE_NETTOYAGE = get_double(1);ask("");
               			ECART_NETTOYAGE = get_double(0);
						break;

					case 5:
						ask("Durée tuteurage + attachage (en secondes) :");
                		DUREE_TUTEURAGE_ATTACHAGE= get_double(1);
						break;

					case 6:
						ask("Durée découpe (en secondes) :");
               			DUREE_DECOUPE = get_double(1);
						break;

					case 7:
						ask("Espérance et écart-type pour le déchargement (en secondes) :");
                		ESPERANCE_DECHARGEMENT = get_double(1);ask("");
               			ECART_DECHARGEMENT = get_double(0);
						break;
					case 0:
						printf("Paramètres validés.\n");
						break;

					default:
						printf("! -- Entrée invalide. Veuillez reessayer. -- !");
				}
				nl();
			}while(requete!=0);
    }
    else{
        printf("\tMODE PAR DEFAUT\n");
        ddl();

        printf("\tStock initial (lots de 8 pots) : %d\n",TOTAL_PLANTES);
        nl();
        printf("\tEspérance et écart-type pour chargement des lots : %.2lf s, %.2lf s\n",ESPERANCE_MISE_SUR_TAPIS,ECART_MISE_SUR_TAPIS);
        printf("\tDurée (fixe) de la soufflerie : %.2lf s\n",DUREE_SOUFFLERIE);
        printf("\tEspérance et écart-type pour nettoyage : %.2lf s, %.2lf s\n",ESPERANCE_NETTOYAGE,ECART_NETTOYAGE);
        printf("\tDurée (fixe) tuteurage + attachage : %.2lf s\n",DUREE_TUTEURAGE_ATTACHAGE);
        printf("\tDurée (fixe) de la découpe : %.2lf s\n",DUREE_DECOUPE);
        printf("\tEspérance et écart-type pour déchargement des lots : %.2lf s, %.2lf s\n",ESPERANCE_DECHARGEMENT,ECART_DECHARGEMENT);
    }
    nl();
    nl();
    return;
}

void liste_parametres_actuels(){
	printf("\tPARAMETRES ACTUELS\n");
    ddl();

    printf("\t1 - Stock initial (lots de 8 pots) : %d\n",TOTAL_PLANTES);
     printf("\t2 - Espérance et écart-type pour chargement des lots : %.2lf s, %.2lf s\n",ESPERANCE_MISE_SUR_TAPIS,ECART_MISE_SUR_TAPIS);
    printf("\t3 - Durée (fixe) de la soufflerie : %.2lf s\n",DUREE_SOUFFLERIE);
    printf("\t4 - Espérance et écart-type pour nettoyage : %.2lf s, %.2lf s\n",ESPERANCE_NETTOYAGE,ECART_NETTOYAGE);
    printf("\t5 - Durée (fixe) tuteurage + attachage : %.2lf s\n",DUREE_TUTEURAGE_ATTACHAGE);
    printf("\t6 - Durée (fixe) de la découpe : %.2lf s\n",DUREE_DECOUPE);
    printf("\t7 - Espérance et écart-type pour déchargement des lots : %.2lf s, %.2lf s\n",ESPERANCE_DECHARGEMENT,ECART_DECHARGEMENT);
	printf("\t0 - Valider\n");

}
