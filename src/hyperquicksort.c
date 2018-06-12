#ifndef HYPERQUICKSORT_C
#define HYPERQUICKSORT_C

#include "mpi.h"
#include "../inc/header.h"

//Implementation of a comparison function for the standard qsort C function
int compare(const void *x, const void *y){
	double a = *(double*)x;
	double b = *(double*)y;
	if(a == b)	return 0;
	else if(a < b)	return -1;
	else		return 1;
}

/*
* Compute a quicksort algorithm in parallel using a hypercube
* topology for the communicators.
* Args:
*	argv[1]: An integer 'n' OR a path to a file containing the array to sort (duoble values)
*		 The file MUST begin with the size (n) of the following array.
*		 If the file doesn't exists, a random array of size n will be generated instead;
* Return:
*	-1 if an error occurred else 0
*	The resulting array will be printed to the file "results"
*/

int hyperquicksort(int argc, char *argv[]){
	//MPI and communicator initialization
	int myRank, nProc;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProc);
	MPI_Request request;
	MPI_Status status;
	MPI_Comm bcastComm;

	//Variable initialization
	FILE *file;
	double *arr, *buf, *bufr, pivot;
	int i, j, k, l, t, n, size, d, bcastRank, bcastProc;

	if(argc != 2){
		fprintf(stderr, "[ERROR:] Hyperquicksort needs 1 arguments\n");
		MPI_Finalize();
		return -1;
	}
	else{
		i = 0; j = 0;
		file = fopen(argv[1], "r");
		//If argv[1] is an existing file
		if(file){
			l = fscanf(file, "%d", &n);		//first element of the file must be 'n'
			size = n/nProc;				//nElements in the proc
			arr = (double*)malloc(n*sizeof(double));
			k = myRank*size;
			while(i < k){	//skip the elements concerning other proc
				l = fscanf(file, "%*f");
				i++;
			}
			while(i++ < k+size) l = fscanf(file, "%lf ", &arr[j++]);
			//size--;
			fclose(file);
		}
		//Else, argv[1] must be 'n'
		else{
			n = atoi(argv[1]);
			size = n/nProc;
			arr = (double*)malloc(n*sizeof(double));
			while(i < size) arr[i++] = randd();
		}
		//We need 2 buffer for send/recv operations of size 'n' (worst case), it's the
		buf =	(double*)malloc(n*sizeof(double));
		bufr =	(double*)malloc(n*sizeof(double));
	}

	//Computing the hyperquicksort
	MPI_Comm_split(MPI_COMM_WORLD, 1, myRank, &bcastComm);	//split the communicator in dimension 1
	d = (int)logX(nProc, 2);		//Computing the number of dimensions, nProc must be a power of 2
	k = 0;
	//For timing the parallel execution, uncomment the next line
	//double begin = MPI_Wtime();
	for(i = d-1; i >= 0; i--){
		//Ask for the actual rank and nProc in the actual dimension of the hypercube
		MPI_Comm_rank(bcastComm, &bcastRank);
		MPI_Comm_size(bcastComm, &bcastProc);
		//each proc quicksort sequentially his subarray
		qsort(&arr[0], size, sizeof(double), compare);
		//each proc of id '0' in the dimension compute his median pivot
		if(bcastRank == 0)
			pivot = (size%2 == 0) ? (arr[size/2-1]+arr[size/2])/2 : arr[size/2];
		//all proc have now the global median pivot	
		MPI_Bcast(&pivot, 1, MPI_DOUBLE, 0, bcastComm);
		k = 0; l = 0;
		//all proc with a '0' in their rank's d-th position send all values > pivot
		if( ((myRank >> i) & 1) == 0 ){
			for(j = 0; j < size; j++){
				if(arr[j] > pivot) buf[k++] = arr[j];
				else arr[l++] = arr[j];	
			}
		}
		//all proc with a '1' in their rank's d-th position send all values <= pivot
		else{
			for(j = 0; j < size; j++){
				if(arr[j] > pivot) arr[l++] = arr[j];
				else buf[k++] = arr[j];
			}
		}
		//Exchange the buffers between neighbours
		MPI_Isend(&buf[0], k, MPI_DOUBLE, (myRank^(1<<i)), 0, MPI_COMM_WORLD, &request);
		MPI_Recv(&bufr[0], n, MPI_DOUBLE, (myRank^(1<<i)), 0, MPI_COMM_WORLD, &status);
		//Count the new number of datas and put the new ones in the main array
		MPI_Get_count(&status, MPI_DOUBLE, &k);
		size = l+k;
		t = 0;
		for(j = l; j < size; j++)
			arr[j] = bufr[t++];
		//Split the hypercube in 2 to the next dimensions
		MPI_Comm_split(bcastComm, bcastRank < bcastProc/2, bcastRank, &bcastComm);
	}
	//final local quicksort
	qsort(&arr[0], size, sizeof(double), compare);
	//For timing the execution, uncomment the next line
	//double end = MPI_Wtime();

	//Global proc 0 receive all the datas and write them
	if(myRank == 0){
		FILE *out = fopen("results", "w");
		for(i = 0; i < size; i++)
			fprintf(out, "%lf ", arr[i]);
		for(i = 1; i < nProc; i++){
			MPI_Recv(&bufr[0], n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			MPI_Get_count(&status, MPI_DOUBLE, &k);
			if(k > 0){
				for(j = 0; j < k; j++)
					fprintf(out, "%lf ", bufr[j]);
			}
		}
		fclose(out);
	}
	else
		MPI_Send(&arr[0], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	MPI_Finalize();
	free(arr); free(buf); free(bufr);
	//For timing, uncomment the next line
	//printf("Proc. %d: %lf\n", myRank, (double)(end-begin));
	return 0;
}

#endif
