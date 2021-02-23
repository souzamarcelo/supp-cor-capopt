#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <time.h>
#include <assert.h>

#define CHECK      1

/* macros */
#define incl(x,y,z) ((x<=y) ? (y<=z) : 0)
#define rest1(c1,c2,c3,c4) (incl(c3,c2,c4)?(c2-c1):(c4-c1))
#define rest2(c1,c2,c3,c4) (incl(c3,c2,c4)?(c2-c3):(((c3>c2)||(c1>c4))?0:(c4-c3)))
#define intsz(c1,c2,c3,c4) (incl(c3,c1,c4)?rest1(c1,c2,c3,c4):rest2(c1,c2,c3,c4))

/* TABU SEARCH parameters */
//#define Kmax   3              /* maximum size of the parametric neighborhood     */
//#define Dmax   2              /* soft diversification parameter                  */
//#define Kten   20             /* tabu tenure (equal for each of the Kmax lists)  */
#define CCmax  50              /* max number of calls with no improvement allowed */
#define nITmax 5000            /* max number of calls of the search inner loop    */
//#define ALPHA  3.5            /* filling function parameter (active if whichF=0) */

/* user's specified selections */ 
#define whichH 0                /* select the HEURISTIC algorithm      */
//#define whichP 0              /* select the PENALTY function         */
#define whichF 0                /* select the FILLING function         */
//#define whichL 1              /* select the LOWER BOUNDING procedure */

/* constants */
#define INFINITY 1000000000.  /* an infinite value, machine dependent */
#define EPS      0.00001      /* a very small value                   */

/* general TS structures */
double **tl;                  /* tabu lists                       */
int    *kt;                   /* tabu tenure for each tabu list   */
int    *lm;                   /* last move executed for each list */

/* TEMPLATES: */

/* a) core procedures: */
int TSpack(int d, int n, int **w, int *W, int lb, double TL, int *ub0, int **x, int *b);
int search(int t, int *K, int *dv, 
           int nb, int d, int n, int **w, int *W, int **x, int *b, double *ff, double timeLeft);

/* b) general selection procedures: */
int    heur(int d, int n, int **w, int *W, int **x, int *b);
int    lower(int d, int n, int **w, int *W);
void   fllf(int  d, int n, int nc, int **w, int *b, int nb, int *W, double *ff);
double getPen(int snb, double *sff, int tnb, double *tff, int flag);

/* c) inner heuristic procedures: */
int HnextFit(int n, int **w, int *W, int **x, int *b, int maxb);
int HHnextFit(int n, int **w, int *W, int **x, int *b);

/* d) specific utility procedures: */
int  nextKT(int *kb, int K, int t, int nb);
int  target(int DP, double *ff, int n);
int  checkfs(int d, int n,int **w, int *W, int **x, int *b);

/* e) general utility procedures: */
void sort(int n, int *x, int *y);
void second(double *time);
