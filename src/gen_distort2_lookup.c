/*
 * $Log$
 * Revision 1.1  2003/04/11 18:33:00  fonin
 * Lookup table generator for distortion2.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "pump.h"

#define MAX_TUBE MAX_SAMPLE
int tube[MAX_SAMPLE];

void init_distort2_lookup(char* c_r1, char* c_r2, char* sr) {
    int sample_rate;
    int             r1 = 1;
    int             r2 = 510;
    int		    res;
    float           c = 0.041;
    int             is = 10;
    long            mut = 2000000;	// 500000
    float           a1,
                    a2;
    float           z = 0.0000239;
    float           s2;
    int             i=0,
                    j;
    int             x;
    float           x2,
                    y,
                    rz,
                    rz2;
    char	    filename[255]="";

    r1=atoi(c_r1);
    r2=atoi(c_r2);
    sample_rate=atoi(sr);

    printf("\nInitializing distortion2 lookup table: r1=%i, r2=%i, sampling rate=%i\n",r1,r2,sample_rate);
    memset(tube,0,sizeof(tube));

    /* create lookup table */
    a1 = (r1 + r2) * c;
    a2 = r1 * c;

    s2 = 2 * sample_rate * (z - 1) / (z + 1);
    for(i=1;i<=MAX_TUBE;i++) {
	x=i;
	x2 = x * (s2 * a1 + 1) / (s2 * a2 + 1);
	y = 1;
	rz = ((x2 - y) / r2) + is * (exp((x - y) / mut) -
				     exp((y - x) / mut)) * 1000;
	tube[i] = 1;
	for (j = 2; j <= MAX_TUBE; j++) {
	    y = j;
	    rz2 =
		((x2 - y) / r2) + is * (exp((x - y) / mut) -
					exp((y - x) / mut)) * 1000;
	    if (rz2 > 0 && rz2 < rz) {
		rz = rz2;
		tube[i] = j;
	    }
	}
	if (tube[i] > MAX_SAMPLE)
	    tube[i] = MAX_SAMPLE;
	if(i % 30 == 0) {
	    printf(".");
	}
//	tube[i] = tube[i] * 0.4;	// 40 - гpомкость
    }
    for(i=MAX_TUBE+1;i<MAX_SAMPLE;i++)
	tube[i]=i;

    /* write the table to the file */
    strcpy(filename,"distort2lookup_");
    strncat(filename,sr,255);
    strcat(filename,"_");
    strncat(filename,c_r1,255);
    strcat(filename,"_");
    strncat(filename,c_r2,255);
    res=open(filename,O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
    if(res==-1) {
	perror("open");
	exit(1);
    }
    write(res,tube,sizeof(tube));
    close(res);
}

int main(int argc, char** argv) {
    time_t t1,t2;
    if(argc!=4) {
	printf("\nLookup table generator for distortion 2 effect." \
				"\nUsage: %s r1 r2 sample_rate\n",argv[0]);
	exit(0);
    }

    t1=time(NULL);

    /* unbuffered output */
    setvbuf(stdout,NULL,_IONBF,0);

    memset(tube,0,sizeof(tube));
    init_distort2_lookup(argv[1],argv[2],argv[3]);

    t2=time(NULL);
    printf("\nComplete in %i seconds.\n",t2-t1);
}
