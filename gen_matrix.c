/*
 *  gen_matrix.c
 *  
 *
 *  Created by Amilcar Meneses on 7/27/10.
 *  Copyright 2010 CINVESTAV-IPN. All rights reserved.
 *
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define MIN 10.0
#define MAX -10.0

#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)

float ran3(long *idum)
{
	static int inext,inextp;
	static long ma[56];
	static int iff=0;
	long mj,mk;
	int i,ii,k;
	
	if (*idum < 0 || iff == 0) {
		iff=1;
		mj=MSEED-(*idum < 0 ? -*idum : *idum);
		mj %= MBIG;
		ma[55]=mj;
		mk=1;
		for (i=1;i<=54;i++) {
			ii=(21*i) % 55;
			ma[ii]=mk;
			mk=mj-mk;
			if (mk < MZ) mk += MBIG;
			mj=ma[ii];
		}
		for (k=1;k<=4;k++)
			for (i=1;i<=55;i++) {
				ma[i] -= ma[1+(i+30) % 55];
				if (ma[i] < MZ) ma[i] += MBIG;
			}
		inext=0;
		inextp=31;
		*idum=1;
	}
	if (++inext == 56) inext=1;
	if (++inextp == 56) inextp=1;
	mj=ma[inext]-ma[inextp];
	if (mj < MZ) mj += MBIG;
	ma[inext]=mj;
	return mj*FAC;
}
#undef MBIG
#undef MSEED
#undef MZ
#undef FAC

int main2(int argc, char **argv) {
	struct timeval tv;
   int m, n, i, j;
	float value, d, md; 
	long idum;


   gettimeofday(&tv, NULL);
	idum = tv.tv_usec;
	d = MAX-MIN; 
   md = d*0.5; 
   if (argc < 2) {
	   perror("Error in number of arguments\n");
      exit(-1); 
	}
   
//	idum = 87877232300127; 
   n = atoi(argv[1]);
//   printf("%d\n", n);
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			value = ran3(&idum)*d - md; 
			printf("%f  ",value); 
		}
		printf("\n");
	}
	printf("\n");
/*	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			value = ran3(&idum)*d - md; 
			printf("%f  ",value); 
		}
		printf("\n");
	}

  */
}
