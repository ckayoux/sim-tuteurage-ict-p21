#include <string.h>
#include "structuresDonnees.h"


lot_plantes* creer_lot_plantes(unsigned int ID){
    lot_plantes* P = calloc(1,sizeof(lot_plantes));
    P->id=ID;
	return P;
}
void enfiler(queue* Q,lot_plantes* P,double t){
  P->instant_arrivee_file=t;
	P->suiv = NULL;
	if(Q->first==NULL) {
		P->prec = NULL;
		Q->first = P;
        Q->last=P;
	}
	else{
        P->prec=Q->last;
        Q->last->suiv=P;
		Q->last=P;
	}
	Q->nb++;
	Q->nb_total++;
	if(Q->nb>Q->nb_max) Q->nb_max=Q->nb;
}
lot_plantes* defiler(queue* Q,double t){
	lot_plantes* P = Q->first;
    if(P) Q->attente += t- P->instant_arrivee_file;
    if(Q->first==Q->last){
        Q->first = NULL;
        Q->last = NULL;
        Q->nb=0;
	}
	if(Q->first){
        if(Q->first->suiv) Q->first->suiv->prec=NULL;
        Q->first=Q->first->suiv;

        (Q->nb)--;
	}


	return P;
}


queue* creer_queue(unsigned int NB,const char* NOM){
	queue* Q = calloc(1,sizeof(queue));
	Q->nb=NB;
	strcpy(Q->nom,NOM);
	for(int i=0; i<NB; i++) {
            lot_plantes* P=creer_lot_plantes(i+1);
            enfiler(Q,P,0);
    };
	return Q;
}

void suppr_queue(queue* Q){
	if(Q){
        lot_plantes* elem = Q->first;
        if(Q->first){
            while(elem->suiv!=NULL){
                elem=elem->suiv;
                free(elem->prec);
            }
            free(elem);
        }
        free(Q);
	}
	return;
}
