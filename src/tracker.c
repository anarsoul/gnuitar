/*
 * $Id$
 *
 * $Log$
 * Revision 1.3  2001/03/25 17:42:32  fonin
 * open() can overwrite existing files from now, because program switches back to real user priorities after start.
 *
 * Revision 1.2  2001/01/14 21:28:42  fonin
 * Fix: track write could overwrite existing files if executing in suid root mode.
 *
 * Revision 1.1.1.1  2001/01/11 13:22:22  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include "tracker.h"
#include "pump.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>

static int     *buffer;
/*
 * static FILE *fout = NULL;
 * 
 * static FILE *fin = NULL; 
 */
static int      fout = -1;
static int      fin = -1;
static int      fin_ix = 0;
static int      fin_size = 0;

void
tracker_out(const char *outfile)
{
    fout = open(outfile, O_NONBLOCK | O_WRONLY | O_CREAT, 0644);
    if (ioctl(fout, O_NONBLOCK, 0) == -1)
	perror("ioctl");

}

void
tracker_in(const char *infile)
{
    struct stat     fs;
    fin = open(infile, O_RDONLY, 0);
    if (fin == -1) {
	perror(infile);
    }
    fstat(fin, &fs);
    fin_size = fs.st_size >> 2;
    fin_size -= 39000;
    printf("Reading %d bytes.\n", fin_size * 4);
    buffer = (int *) malloc(fin_size * 4);
    read(fin, buffer, 4 * fin_size);
    close(fin);
}

void
tracker_done()
{
    if (fout > 0)
	close(fout);

    if (fin != -1)
	close(fin);

    if (buffer)
	free(buffer);
}

void
track_read(int *s, int count)
{
    static int      r[512];
    int             i;
    for (i = 0; i < count; i++)
	r[i] = 0;
    i = count;
    if (fin_ix + count >= fin_size)
	i = fin_size - fin_ix;
    memcpy(r, buffer + fin_ix, i * 4);
    for (i = 0; i < count; i++)
	s[i] += r[i];
    fin_ix += i;
    if (fin_ix >= fin_size)
	fin_ix = 0;
}


void
track_write(int *s, int count)
{
    __int16_t       tmp[BUFFER_SIZE];
    int             i;

    /*
     * Convert to 16bit raw data
     */
    for (i = 0; i < count; i++)
	tmp[i] = s[i];
    write(fout, tmp, sizeof(__int16_t) * count);
}
