/*
 * A small utility to strip the fill sequence from the distort2 lookup table.
 * Based on the fact that the most part of the lookup table is filled with
 * the 32767, and therefore it is possible to get rid of it.
 *
 * $Id$
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "pump.h"
#define FILL 32767

int main(int argc, char** argv) {
    int in, out;
    int i;
    SAMPLE tube[MAX_SAMPLE];

    if(argc!=3) {
	printf("\nUsage: %s <source> <target>\n" \
	    "where <source> is the source lookup table file,\n" \
	    "      <target> is the target (stripped) lookup table.\n\n");
	exit();
    }

    /* open input file */
    in=open(argv[1],O_RDONLY);
    if(in==-1) {
	perror("open");
	exit();
    }
    read(in,tube,sizeof(tube));
    close(in);

    for(i=MAX_SAMPLE-1;i>=0;i--) {
	if(tube[i]!=FILL)
	    break;
    }

    out=open(argv[2],O_CREAT|O_WRONLY|O_TRUNC,S_IREAD|S_IWRITE|S_IRGRP|S_IROTH);
    if(out==-1) {
	perror("open");
	exit(1);
    }
    write(out,tube,(i+1)*sizeof(SAMPLE));
    close(out);
}
