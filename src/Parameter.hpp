#ifndef _PARAMETER_H_
#define _PARAMETER_H_

/*global variables*/

#define LEARNTIMES	30	/*times of online learning*/
#define BETA		0.2	/*beta for mutation*/
#define DETA		0.0000001	/*quantity changed by mutation*/

/*Status of Word-agent*/
#define ACTIVE		1	/*active*/
#define MATCH		2	/*match*/
#define MUTATE		3	/*mutate*/
#define MATURE		4	/*mature*/
#define DIE		5	/*die*/
#define REGULATE        6       /*regulated*/

/*Behavior of Word-agent*/
#define MOVING		1	/*move*/
#define INTERACTING	2	/*interact*/
#define MUTATING	3	/*mutate*/
#define SELECTING	4	/*select*/
#define CLONING		5	/*clone*/
#define	REGULATING	6	/*regulate*/
#define DYING		7	/*die*/

#define AFFINITY	0.0	/*affinity threshold for adaptive recognition*/
#define STIMULUS	0.0	/*stimulus threshold*/
#define SUPPRESS	0.0	/*suppress threshold*/

/*Category of Word-agent*/
#define ANTIGEN		0	/*Antigen word-agent*/
#define BCELL		1	/*B cell word-agent*/
#define MEMORYBCELL     2       /*Memory B cell word-agents*/
#define ANTIBODY        3       /*Antibody word-agents*/


#define MUTATEPRO   	0.2 	/*Mutated probability*/

#define PRECISION	10000000	/*3 points after 0: 0.000*/

// Enviroment scale
#define ROWS 2
#define COLS 2

#define AGQUANTITY	5	/*quantity of antigens*/
#define MAXNUMAGENT     1000    /*local maximum number of agents*/

#define ROUND           10      /*threshold for running*/

#define AGSURVIORRATE   0.7     /*rate of survivor of antigens*/

#define ACCURACYTHRESHOLD       0.6     /*threshold for mutated accuracy*/

#define LAMDA           5       /*clone number*/

#define AFFINITYMATURE  0.8     /*affinity mature threshold*/

#endif
