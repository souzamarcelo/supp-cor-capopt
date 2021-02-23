#include "TSpack.h"

int main(
	int argc, char **argv
    )
{                                           /* driver program for the TSPACK code */
	int    i, j, value, correct;
	int    d, n, lb, ub0, ub;
	double  timeLimit = INFINITY;
	char   fname[400];
	FILE   *file;

	int    **w, **x;
	int    *W, *b;

	for(int i = 0; i < argc; i++)
		if (strcmp(argv[i], "--instance") == 0)
			sscanf(argv[i + 1], "%s", fname);
	
	setParameters(argc, argv);

	file = fopen(fname, "r");
	fscanf(file, "%d", &d);
        fscanf(file, "%d", &n);

	/* memory allocation */
	W = (int*)calloc(d, sizeof(int));
	b = (int*)calloc(n, sizeof(int));
	
	w = (int**)calloc(d, sizeof(int*));
	x = (int**)calloc(d, sizeof(int*));
	for (i = 0; i < d; i++) {
	    w[i] = (int*)calloc(n, sizeof(int));
	    x[i] = (int*)calloc(n, sizeof(int));
	}

	/* get the current instance */
	for (i = 0; i < d; i++) {
	    fscanf(file, "%d", &value);
	    W[i] = value;
	}
	for (i = 0; i < n; i++) {
	    for (j = 0; j < d; j++) {
	        fscanf(file, "%d", &value);
	        w[j][i] = value;
	    }
	}
	fclose(file);

	/* compute an initial lower bound for the instance */
	lb = lower(d, n, w, W);

	/* compute the TS solution */
	ub = TSpack(d, n, w, W, lb, timeLimit, &ub0, x, b);
	if (ub <= 0) {
	   printf("\n an error occurred in procedure TSpack!\n");
	   exit(0);
	}

	/* check the correctnes of the solution */
	correct = checkfs(d, n, w, W, x, b);
	if (!correct) {
	   printf("\n the final solution is not feasible!\n");
	   exit(0);
	}

	/* print the solution value */
	//printf(" LB = %4d iUB = %4d fUB = %4d\n", lb, ub0, ub);

	/* memory de-allocation */
	for (i = 0; i < d; i++) {
	    free(w[i]); free(x[i]);
	}
	free(w); free(x);
	free(W); free(b);

	return(0);
}
