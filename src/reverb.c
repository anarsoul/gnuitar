#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "reverb.h"
#include "gui.h"

int            *
newChunk()
{
    return (int *) malloc(BUFFER_SIZE * sizeof(int));
}

void
freeChunk(int *p)
{
    free(p);
}

void
reverbBuffer_init(struct reverbBuffer *r, int chunks)
{
    r->data = (int *) malloc(chunks * BUFFER_SIZE * sizeof(int));
    r->nChunks = chunks;
    r->nCursor = 0;
}

void
reverbBuffer_done(struct reverbBuffer *r)
{
    free(r->data);
    r->data = NULL;
}

void
reverbBuffer_addChunk(struct reverbBuffer *r, int *chunk)
{
    int            *addTo;
    addTo = r->data + r->nCursor * BUFFER_SIZE;
    memcpy(addTo, chunk, BUFFER_SIZE * sizeof(int));
    r->nCursor++;
    r->nCursor %= r->nChunks;
}

int            *
reverbBuffer_getChunk(struct reverbBuffer *r, int delay)
{
    int             nFrom;
    int            *getFrom;
    int            *giveTo;

    assert((delay >= 0) && (delay < r->nChunks));
    nFrom = r->nCursor - delay;
    while (nFrom < 0)
	nFrom += r->nChunks;
    getFrom = r->data + nFrom * BUFFER_SIZE;
    giveTo = newChunk();
    memcpy(giveTo, getFrom, BUFFER_SIZE * sizeof(int));
    return giveTo;
}

void            reverb_filter(struct effect *p, struct data_block *db);

void
update_reverb_wet(GtkAdjustment * adj, struct reverb_params *params)
{
    params->wet = (int) adj->value;
}

void
update_reverb_dry(GtkAdjustment * adj, struct reverb_params *params)
{
    params->dry = (int) adj->value;
}

void
update_reverb_delay(GtkAdjustment * adj, struct reverb_params *params)
{
    params->delay = (int) adj->value;
}

void
update_reverb_regen(GtkAdjustment * adj, struct reverb_params *params)
{
    params->regen = (int) adj->value;
}

void
toggle_reverb(void *bullshit, struct effect *p)
{
    if (p->toggle == 1) {
	p->proc_filter = passthru;
	p->toggle = 0;
    } else {
	p->proc_filter = reverb_filter;
	p->toggle = 1;
    }
}


void
reverb_init(struct effect *p)
{
    struct reverb_params *preverb;

    GtkWidget      *wet;
    GtkWidget      *wet_label;
    GtkObject      *adj_wet;

    GtkWidget      *dry;
    GtkWidget      *dry_label;
    GtkObject      *adj_dry;

    GtkWidget      *delay;
    GtkWidget      *delay_label;
    GtkObject      *adj_delay;

    GtkWidget      *regen;
    GtkWidget      *regen_label;
    GtkObject      *adj_regen;

    GtkWidget      *button;

    GtkWidget      *parmTable;

    preverb = (struct reverb_params *) p->params;

    /*
     * GUI Init
     */
    p->control = gtk_window_new(GTK_WINDOW_DIALOG);
    rnd_window_pos(GTK_WINDOW(p->control));
    parmTable = gtk_table_new(2, 8, FALSE);



    adj_delay = gtk_adjustment_new(preverb->delay,
				   1.0, 256, 1.0, 1.0, 1.0);
    delay_label = gtk_label_new("delay");
    gtk_table_attach(GTK_TABLE(parmTable), delay_label, 0, 1, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_delay), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_delay), preverb);

    delay = gtk_vscale_new(GTK_ADJUSTMENT(adj_delay));

    gtk_table_attach(GTK_TABLE(parmTable), delay, 0, 1, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_wet = gtk_adjustment_new(preverb->wet, 1.0, 255.0, 1.0, 1.0, 1.0);
    wet_label = gtk_label_new("wet");
    gtk_table_attach(GTK_TABLE(parmTable), wet_label, 3, 4, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_wet), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_wet), preverb);

    wet = gtk_vscale_new(GTK_ADJUSTMENT(adj_wet));

    gtk_table_attach(GTK_TABLE(parmTable), wet, 3, 4, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_dry = gtk_adjustment_new(preverb->dry, 1.0, 255.0, 1.0, 1.0, 1.0);
    dry_label = gtk_label_new("dry");
    gtk_table_attach(GTK_TABLE(parmTable), dry_label, 5, 6, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_dry), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_dry), preverb);

    dry = gtk_vscale_new(GTK_ADJUSTMENT(adj_dry));

    gtk_table_attach(GTK_TABLE(parmTable), dry, 5, 6, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    adj_regen = gtk_adjustment_new(preverb->regen,
				   0.0, 256, 1.0, 1.0, 1.0);
    regen_label = gtk_label_new("regen");
    gtk_table_attach(GTK_TABLE(parmTable), regen_label, 7, 8, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);


    gtk_signal_connect(GTK_OBJECT(adj_regen), "value_changed",
		       GTK_SIGNAL_FUNC(update_reverb_regen), preverb);

    regen = gtk_vscale_new(GTK_ADJUSTMENT(adj_regen));

    gtk_table_attach(GTK_TABLE(parmTable), regen, 7, 8, 1, 2,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);

    button = gtk_check_button_new_with_label("On");
    gtk_signal_connect(GTK_OBJECT(button), "toggled",
		       GTK_SIGNAL_FUNC(toggle_reverb), p);

    gtk_table_attach(GTK_TABLE(parmTable), button, 9, 10, 0, 1,
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK),
		     __GTKATTACHOPTIONS
		     (GTK_FILL | GTK_EXPAND | GTK_SHRINK), 0, 0);
    if (p->toggle == 1) {
	p->toggle = 0;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    }


    gtk_window_set_title(GTK_WINDOW(p->control),
			 (gchar *) ("Reverberator"));
    gtk_container_add(GTK_CONTAINER(p->control), parmTable);

    gtk_widget_show_all(p->control);

}

void
reverb_filter(struct effect *p, struct data_block *db)
{
    struct reverb_params *dr;
    int            *s,
                    count;
    int             Dry,
                    Wet,
                    Rgn,
                    dd;
    int            *Old,
                   *Old2,
                    Out;
    float           delay;
    int             tmp,
                    tot;
    dr = (struct reverb_params *) p->params;

    s = db->data;
    count = db->len;

    /*
     * get delay 
     */
    delay = dr->delay;
    delay /= 256.0;
    delay *= (float) dr->history->nChunks;
    dd = (int) delay;
    dd =
	(dd < 1) ? 1 : (dd >=
			dr->history->nChunks) ? dr->history->nChunks -
	1 : dd;

    /*
     * get parms 
     */
    Dry = (int) dr->dry;
    Wet = (int) dr->wet;
    Rgn = (int) dr->regen;

    Old = (int *) reverbBuffer_getChunk(dr->history, dd);
    Old2 = Old;
    while (count) {
	/*
	 * mix Old and In into Out, based upon Wet/Dry * then mix Out and In
	 * back into In, based upon Rgn/1 
	 */
	tmp = *s;
	tmp *= Dry;
	tmp /= 256;
	tot = *Old;
	tot *= Wet;
	tot /= 256;
	tot += tmp;
	tot = (tot < -32767) ? -32767 : (tot > 32767) ? 32767 : tot;
	Out = tot;

	tot *= Rgn;
	tot /= 256;
	tot += *s;
	tot = (tot < -32767) ? -32767 : (tot > 32767) ? 32767 : tot;
	*s = tot;
	s++;
	Old++;
	count--;
    }

    reverbBuffer_addChunk(dr->history, db->data);
    freeChunk(Old2);
}

void
reverb_done(struct effect *p)
{
    struct reverb_params *dr;

    dr = (struct reverb_params *) p->params;

    if (dr->history != NULL)
	reverbBuffer_done(dr->history);
    free(dr->history);
    free(p->params);
    gtk_widget_destroy(p->control);
    free(p);
    p = NULL;
}

void
reverb_save(struct effect *p, int fd)
{
    struct reverb_params *rp;

    rp = (struct reverb_params *) p->params;

    write(fd, &rp->dry, sizeof(int));
    write(fd, &rp->wet, sizeof(int));
    write(fd, &rp->regen, sizeof(int));
    write(fd, &rp->delay, sizeof(int));
}

void
reverb_load(struct effect *p, int fd)
{
    struct reverb_params *rp;

    rp = (struct reverb_params *) p->params;

    read(fd, &rp->dry, sizeof(int));
    read(fd, &rp->wet, sizeof(int));
    read(fd, &rp->regen, sizeof(int));
    read(fd, &rp->delay, sizeof(int));
    if (p->toggle == 0) {
	p->proc_filter = passthru;
    } else {
	p->proc_filter = reverb_filter;
    }
}


void
reverb_create(struct effect *p)
{
    struct reverb_params *dr;
    p->params =
	(struct reverb_params *) malloc(sizeof(struct reverb_params));

    p->proc_init = reverb_init;
    p->proc_filter = passthru;
    p->toggle = 0;
    p->id = REVERB;
    p->proc_done = reverb_done;
    p->proc_load = reverb_load;
    p->proc_save = reverb_save;
    dr = (struct reverb_params *) p->params;
    dr->history =
	(struct reverbBuffer *) malloc(sizeof(struct reverbBuffer));
    reverbBuffer_init(dr->history,
		      (int) ((SAMPLE_RATE * sizeof(int) / BUFFER_SIZE) /
			     4 + 1));

    dr->delay = 15.0f;
    dr->wet = 128.0f;
    dr->dry = 200.0f;
    dr->regen = 0.0f;
}
