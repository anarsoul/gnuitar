/*
 * $Id$
 *
 * $Log$
 * Revision 1.4  2001/04/27 14:29:14  fonin
 * <sys/soundcard.h> for better compatibility with FreeBSD.
 *
 * Revision 1.3  2001/03/25 17:42:55  fonin
 * Switching back to real user identifier immediately after setting realtime priority.
 *
 * Revision 1.2  2001/03/25 12:10:06  fonin
 * Text messages begin from newline rather than end with it.
 *
 * Revision 1.1.1.1  2001/01/11 13:21:53  fonin
 * Version 0.1.0 Release 1 beta
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <sched.h>
#include <pthread.h>
#include <sys/soundcard.h>

#include "pump.h"
#include "tracker.h"
#include "gui.h"

pthread_t       audio_thread;


static int      stop = 0;
int             fd;

char            version[32] = "GNUitar $Name$";

void           *
audio_thread_start(void *V)
{
    int             count,
                    i;

    short           rdbuf[BUFFER_SIZE / 2];
    int             procbuf[BUFFER_SIZE / 2];

    while (!stop) {
	count = read(fd, rdbuf, BUFFER_SIZE);
	if (count != BUFFER_SIZE) {
	    fprintf(stderr, "Cannot read samples!\n");
	    close(fd);
	    exit(0);
	}

	for (i = 0; i < count / 2; i++)
	    procbuf[i] = rdbuf[i];

	pump_sample(procbuf, count / 2);

	for (i = 0; i < count / 2; i++) {
	    int             W = procbuf[i];
	    if (W < -32767)
		W = -32767;
	    if (W > 32767)
		W = 32767;
	    rdbuf[i] = W;
	}

	count = write(fd, rdbuf, BUFFER_SIZE);
	if (count != BUFFER_SIZE) {
	    fprintf(stderr, "Cannot write samples!\n");
	    close(fd);
	    exit(0);
	}
    }
}

void
signal_ctrlc(int sig)
{
    stop = 1;
}

int
main(int argc, char **argv)
{
    int             max_priority,
                    i;
    struct sched_param p;


    if ((fd = open("/dev/dsp", O_RDWR)) == -1) {
	fprintf(stderr, "\nCannot open audio device!");
	return -1;
    }

    i = 0x7fff0008;
    if (ioctl(fd, SNDCTL_DSP_SETFRAGMENT, &i) < 0) {
	fprintf(stderr, "\nCannot setup fragments!");
	close(fd);
	return -1;
    }

    if (ioctl(fd, SNDCTL_DSP_SETDUPLEX, 0) == -1) {
	fprintf(stderr, "\nCannot setup fullduplex audio!");
	close(fd);
	return -1;
    }

    if (ioctl(fd, SNDCTL_DSP_GETCAPS, &i) == -1) {
	fprintf(stderr, "\nCannot get soundcard capabilities!");
	close(fd);
	return -1;
    }

    if (!(i & DSP_CAP_DUPLEX)) {
	fprintf(stderr,
		"\nSorry but your soundcard isn't full duplex capable!");
	close(fd);
	return -1;
    }

    i = AFMT_S16_LE;
    if (ioctl(fd, SNDCTL_DSP_SETFMT, &i) == -1) {
	fprintf(stderr, "\nCannot setup 16 bit audio!");
	close(fd);
	return -1;
    }

    i = 0;
    if (ioctl(fd, SNDCTL_DSP_STEREO, &i) == -1) {
	fprintf(stderr, "\nCannot setup mono audio!");
	close(fd);
	return -1;
    }

    i = SAMPLE_RATE;
    if (ioctl(fd, SNDCTL_DSP_SPEED, &i) == -1) {
	fprintf(stderr, "\nCannot setup sampling frequency %dHz!", i);
	close(fd);
	return -1;
    }

    gtk_init(&argc, &argv);
    init_gui();


    pump_start(argc, argv);

    max_priority = sched_get_priority_max(SCHED_FIFO);
    p.sched_priority = max_priority;

    if (sched_setscheduler(0, SCHED_FIFO, &p)) {
	printf
	    ("\nFailed to set scheduler priority. (Are you running as root?)");
	printf("\nContinuing with default priority");
    }

    if (pthread_create(&audio_thread, NULL, audio_thread_start, NULL)) {
	fprintf(stderr, "\nAudio thread creation failed!");
	exit(1);
    }

    /*
     * We were running to this point as setuid root program.
     * Switching to our native user id
     */
    setuid(getuid());

    gtk_main();

    pump_stop();

    tracker_done();

    close(fd);

    return 0;
}
