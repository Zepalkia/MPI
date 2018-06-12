#ifndef HEADER
#define HEADER

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

//Compute the n-log
inline float logX(int value, int base){
	return log(value)/log(base);
}

//Generate a random integer
inline int randi(){
	return (int)(rand()%INT_MAX);
}

//Generate a random integer between 0 and 'max'
inline int randint(int max){
	return (int)(rand()%max);
}

//Generate a random double value
inline double randd(){
	return (double)rand()/(double)RAND_MAX;
}

#endif
