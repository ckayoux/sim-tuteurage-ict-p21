#include <stdio.h>
#include <stdlib.h>



typedef struct element{
	struct element* suiv;
	struct element* prec;
	unsigned int id;
	double instant_arrivee_file;
}lot_plantes;

typedef struct {
	lot_plantes* first;
	lot_plantes* last;
	double attente;
	unsigned int nb;
	unsigned int nb_max;
	unsigned int nb_total;
    const char nom[25];
}queue;


lot_plantes* creer_lot_plantes(unsigned int ID);
void enfiler(queue* Q,lot_plantes* P,double t);
lot_plantes* defiler(queue* Q,double t);


queue* creer_queue(unsigned int NB,const char* NOM);
void suppr_queue(queue* Q);
