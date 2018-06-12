#include "hyperquicksort.c"
#include <time.h>
int main(int argc, char *argv[]){
	srand(time(NULL));

	/*
	*For running the hyperquicksort algorithm, uncomment the next line and run the
	*programm with mpirun:
	*mpirun -np P main ARG
	*with P the number of proc (must be a power of 2) and ARG the path to a file or an integer
	*/
	//return hyperquicksort(argc, argv);
}
