/*
 * $id$
 */

#ifndef _TRACKER_H_
#define _TRACKER_H_ 1

extern void     tracker_out(const char *outfile);
extern void     tracker_in(const char *infile);
extern void     tracker_done();
extern void     track_read(int *s, int count);
extern void     track_write(int *s, int count);

#endif
