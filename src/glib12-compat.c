/* GTK12 compatiblity code (c) Antti S. Lankila 2005  <alankila@bel.fi>
 * Licensed under the LGPL. Mostly lifted off from glib-2.0 and massaged
 * a bit to compile */

#include <gtk/gtk.h>
#include "glib12-compat.h"

#ifdef HAVE_GTK2

GtkWidget *
gnuitar_gtk_text_view_new(GtkContainer *container) {
    GtkWidget *tw = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(tw), FALSE);
    gtk_container_add(GTK_CONTAINER(container), tw);
    return tw;
}

void
gnuitar_gtk_text_view_append(GtkWidget *tw, gchar *text) {
    GtkTextIter iter;
    GtkTextBuffer  *textbuf;
    textbuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tw));
    gtk_text_buffer_get_end_iter(textbuf, &iter);\
    gtk_text_buffer_insert(textbuf, &iter, text, -1);
}

#endif

/* substitute locale versions for glib1.2 */
#ifdef HAVE_GTK
#define g_ascii_strtod		strtod
#define g_ascii_dtostr(a, b, c)	sprintf(a, "%lf", c)

GtkWidget *
gnuitar_gtk_text_view_new(GtkContainer *container) {
    GtkWidget *tw = gtk_text_new(
        gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(container)),
        gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(container)));
    gtk_container_add(GTK_CONTAINER(container), tw);
    return tw;
}

void
gnuitar_gtk_text_view_append(GtkWidget *tw, gchar *text) {
    gtk_text_insert(GTK_TEXT(tw), NULL, NULL, NULL, text, -1);
}

#endif

#include <stdio.h>
#include <stdlib.h>

#if defined(HAVE_GTK) || (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 10)
void
g_key_file_set_double(GKeyFile *kf, const gchar *grp, const gchar *name, gdouble val)
{
    gchar   tmp[80];
    /* XXX this output should be locale independent, how to do it? */
    g_ascii_dtostr(tmp, 80, val);
    g_key_file_set_string(kf, grp, name, tmp);
}

gdouble
g_key_file_get_double(GKeyFile *kf, const gchar *grp, const gchar *name, GError **error)
{
    gchar   *tmp;
    double   value;

    tmp = g_key_file_get_string(kf, grp, name, error);
    if (*error != NULL)
        return 0;
    
    value = g_ascii_strtod(tmp, NULL);
    g_free(tmp);
    
    return value;
}
#endif

#ifdef HAVE_GTK

#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#include <io.h>
#ifndef S_ISREG
#define S_ISREG(mode) ((mode)&_S_IFREG)
#endif
#endif

/* no translations yet */
#define _(a) a

#ifndef _WIN32
/* glib/gprintf.c */
gint
g_vasprintf (gchar      **string,
             gchar const *format,
             va_list      args)
{
  gint len;
  g_return_val_if_fail (string != NULL, -1);

  len = vasprintf (string, format, args);
  if (len < 0)
    *string = NULL;

  return len;
}
#endif

/* glib/gstring.c */
static void
g_string_append_printf_internal (GString     *string,
                                 const gchar *fmt,
                                 va_list      args)
{
  gchar *buffer;
#ifndef _WIN32
  gint length;

  length = g_vasprintf (&buffer, fmt, args);
#else
#define GLIB_TMPBUFSIZE 4096
  if((buffer = (gchar*)malloc(GLIB_TMPBUFSIZE))!=NULL)
    _vsnprintf(buffer,GLIB_TMPBUFSIZE,fmt,args);
  else return;
#endif
  g_string_append (string, buffer);
  g_free (buffer);
}


void
g_string_printf (GString *string,
                 const gchar *fmt,
                 ...)
{
  va_list args;

  g_string_truncate (string, 0);

  va_start (args, fmt);
  g_string_append_printf_internal (string, fmt, args);
  va_end (args);
}

void
g_string_append_printf (GString *string,
                        const gchar *fmt,
                        ...)
{
  va_list args;

  va_start (args, fmt);
  g_string_append_printf_internal (string, fmt, args);
  va_end (args);
}

/* glib/gstrfuncs.c */
#define ISSPACE(c)              ((c) == ' ' || (c) == '\f' || (c) == '\n' || \
                                 (c) == '\r' || (c) == '\t' || (c) == '\v')
#define ISUPPER(c)              ((c) >= 'A' && (c) <= 'Z')
#define ISLOWER(c)              ((c) >= 'a' && (c) <= 'z')
#define ISALPHA(c)              (ISUPPER (c) || ISLOWER (c))
#define TOUPPER(c)              (ISLOWER (c) ? (c) - 'a' + 'A' : (c))
#define TOLOWER(c)              (ISUPPER (c) ? (c) - 'A' + 'a' : (c))

#define g_ascii_isspace(c) \
  ((g_ascii_table[(guchar) (c)] & G_ASCII_SPACE) != 0)

static const guint16 ascii_table_data[256] = {
  0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004,
  0x004, 0x104, 0x104, 0x004, 0x104, 0x104, 0x004, 0x004,
  0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004,
  0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004, 0x004,
  0x140, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
  0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
  0x459, 0x459, 0x459, 0x459, 0x459, 0x459, 0x459, 0x459,
  0x459, 0x459, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
  0x0d0, 0x653, 0x653, 0x653, 0x653, 0x653, 0x653, 0x253,
  0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253,
  0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253, 0x253,
  0x253, 0x253, 0x253, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x0d0,
  0x0d0, 0x473, 0x473, 0x473, 0x473, 0x473, 0x473, 0x073,
  0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073,
  0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073, 0x073,
  0x073, 0x073, 0x073, 0x0d0, 0x0d0, 0x0d0, 0x0d0, 0x004
  /* the upper 128 are all zeroes */
};

const guint16 * const g_ascii_table = ascii_table_data;

gint
g_ascii_strcasecmp (const gchar *s1,
                    const gchar *s2)
{
  gint c1, c2;

  g_return_val_if_fail (s1 != NULL, 0);
  g_return_val_if_fail (s2 != NULL, 0);

  while (*s1 && *s2)
    {
      c1 = (gint)(guchar) TOLOWER (*s1);
      c2 = (gint)(guchar) TOLOWER (*s2);
      if (c1 != c2)
        return (c1 - c2);
      s1++; s2++;
    }

  return (((gint)(guchar) *s1) - ((gint)(guchar) *s2));
}

/* glib/gfileutils.c */
GQuark
g_file_error_quark (void)
{
  static GQuark q = 0;
  if (q == 0)
    q = g_quark_from_static_string ("g-file-error-quark");

  return q;
}

GFileError
g_file_error_from_errno (gint err_no)
{
  switch (err_no)
    {
#ifdef EEXIST
    case EEXIST:
      return G_FILE_ERROR_EXIST;
      break;
#endif

#ifdef EISDIR
    case EISDIR:
      return G_FILE_ERROR_ISDIR;
      break;
#endif

#ifdef EACCES
    case EACCES:
      return G_FILE_ERROR_ACCES;
      break;
#endif

#ifdef ENAMETOOLONG
    case ENAMETOOLONG:
      return G_FILE_ERROR_NAMETOOLONG;
      break;
#endif

#ifdef ENOENT
    case ENOENT:
      return G_FILE_ERROR_NOENT;
      break;
#endif

#ifdef ENOTDIR
    case ENOTDIR:
      return G_FILE_ERROR_NOTDIR;
      break;
#endif

#ifdef ENXIO
    case ENXIO:
      return G_FILE_ERROR_NXIO;
      break;
#endif

#ifdef ENODEV
    case ENODEV:
      return G_FILE_ERROR_NODEV;
      break;
#endif

#ifdef EROFS
    case EROFS:
      return G_FILE_ERROR_ROFS;
      break;
#endif

#ifdef ETXTBSY
    case ETXTBSY:
      return G_FILE_ERROR_TXTBSY;
      break;
#endif

#ifdef EFAULT
    case EFAULT:
      return G_FILE_ERROR_FAULT;
      break;
#endif

#ifdef ELOOP
    case ELOOP:
      return G_FILE_ERROR_LOOP;
      break;
#endif

#ifdef ENOSPC
    case ENOSPC:
      return G_FILE_ERROR_NOSPC;
      break;
#endif

#ifdef ENOMEM
    case ENOMEM:
      return G_FILE_ERROR_NOMEM;
      break;
#endif

#ifdef EMFILE
    case EMFILE:
      return G_FILE_ERROR_MFILE;
      break;
#endif

#ifdef ENFILE
    case ENFILE:
      return G_FILE_ERROR_NFILE;
      break;
#endif

#ifdef EBADF
    case EBADF:
      return G_FILE_ERROR_BADF;
      break;
#endif

#ifdef EINVAL
    case EINVAL:
      return G_FILE_ERROR_INVAL;
      break;
#endif

#ifdef EPIPE
    case EPIPE:
      return G_FILE_ERROR_PIPE;
      break;
#endif

#ifdef EAGAIN
    case EAGAIN:
      return G_FILE_ERROR_AGAIN;
      break;
#endif

#ifdef EINTR
    case EINTR:
      return G_FILE_ERROR_INTR;
      break;
#endif

#ifdef EIO
    case EIO:
      return G_FILE_ERROR_IO;
      break;
#endif

#ifdef EPERM
    case EPERM:
      return G_FILE_ERROR_PERM;
      break;
#endif

/* there is no G_FILE_ERROR_NOSYS in glib on Win32 */
#if defined(ENOSYS) && !defined(_WIN32)
    case ENOSYS:
      return G_FILE_ERROR_NOSYS;
      break;
#endif

    default:
      return G_FILE_ERROR_FAILED;
      break;
    }
}

/* glib/gerror.c */
static GError*
g_error_new_valist(GQuark         domain,
                   gint           code,
                   const gchar   *format,
                   va_list        args)
{
  GError *error;

  error = g_new (GError, 1);

  error->domain = domain;
  error->code = code;
  error->message = g_strdup_vprintf (format, args);

  return error;
}

GError*
g_error_new (GQuark       domain,
             gint         code,
             const gchar *format,
             ...)
{
  GError* error;
  va_list args;

  g_return_val_if_fail (format != NULL, NULL);
  g_return_val_if_fail (domain != 0, NULL);

  va_start (args, format);
  error = g_error_new_valist (domain, code, format, args);
  va_end (args);

  return error;
}

GError*
g_error_new_literal (GQuark         domain,
                     gint           code,
                     const gchar   *message)
{
  GError* err;

  g_return_val_if_fail (message != NULL, NULL);
  g_return_val_if_fail (domain != 0, NULL);

  err = g_new (GError, 1);

  err->domain = domain;
  err->code = code;
  err->message = g_strdup (message);

  return err;
}

void
g_error_free (GError *error)
{
  g_return_if_fail (error != NULL);

  g_free (error->message);

  g_free (error);
}

GError*
g_error_copy (const GError *error)
{
  GError *copy;

  g_return_val_if_fail (error != NULL, NULL);

  copy = g_new (GError, 1);

  *copy = *error;

  copy->message = g_strdup (error->message);

  return copy;
}

gboolean
g_error_matches (const GError *error,
                 GQuark        domain,
                 gint          code)
{
  return error &&
    error->domain == domain &&
    error->code == code;
}

#define ERROR_OVERWRITTEN_WARNING "GError set over the top of a previous GError or uninitialized memory.\n" \
               "This indicates a bug in someone's code. You must ensure an error is NULL before it's set.\n" \
               "The overwriting error message was: %s"

void
g_set_error (GError      **err,
             GQuark        domain,
             gint          code,
             const gchar  *format,
             ...)
{
  GError *new;

  va_list args;

  if (err == NULL)
    return;

  va_start (args, format);
  new = g_error_new_valist (domain, code, format, args);
  va_end (args);

  if (*err == NULL)
    *err = new;
  else
    g_warning (ERROR_OVERWRITTEN_WARNING, new->message);
}

void
g_propagate_error (GError       **dest,
		   GError        *src)
{
  g_return_if_fail (src != NULL);

  if (dest == NULL)
    {
      if (src)
        g_error_free (src);
      return;
    }
  else
    {
      if (*dest != NULL)
        g_warning (ERROR_OVERWRITTEN_WARNING, src->message);
      else
        *dest = src;
    }
}

void
g_clear_error (GError **err)
{
  if (err && *err)
    {
      g_error_free (*err);
      *err = NULL;
    }
}

/* glib/gkeyfile.c */
typedef struct _GKeyFileGroup GKeyFileGroup;

struct _GKeyFile
{
  GList *groups;

  GKeyFileGroup *start_group;
  GKeyFileGroup *current_group;

  GString *parse_buffer; /* Holds up to one line of not-yet-parsed data */

  /* Used for sizing the output buffer during serialization
   */
  gsize approximate_size;

  gchar list_separator;

  GKeyFileFlags flags;
};

typedef struct _GKeyFileKeyValuePair GKeyFileKeyValuePair;

struct _GKeyFileGroup
{
  const gchar *name;  /* NULL for above first group (which will be comments) */

  GKeyFileKeyValuePair *comment; /* Special comment that is stuck to the top of a group */

  GList *key_value_pairs;

  /* Used in parallel with key_value_pairs for
   * increased lookup performance
   */
  GHashTable *lookup_map;
};

struct _GKeyFileKeyValuePair
{
  gchar *key;  /* NULL for comments */
  gchar *value;
};

static gboolean              g_key_file_load_from_fd           (GKeyFile               *key_file,
								gint                    fd,
								GKeyFileFlags           flags,
								GError                **error);
static GList                *g_key_file_lookup_group_node      (GKeyFile               *key_file,
			                                        const gchar            *group_name);
static GKeyFileGroup        *g_key_file_lookup_group           (GKeyFile               *key_file,
								const gchar            *group_name);

static GList                *g_key_file_lookup_key_value_pair_node  (GKeyFile       *key_file,
			                                             GKeyFileGroup  *group,
                                                                     const gchar    *key);
static GKeyFileKeyValuePair *g_key_file_lookup_key_value_pair       (GKeyFile       *key_file,
                                                                     GKeyFileGroup  *group,
                                                                     const gchar    *key);

static void                  g_key_file_remove_group_node          (GKeyFile      *key_file,
							  	    GList         *group_node);
static void                  g_key_file_remove_key_value_pair_node (GKeyFile      *key_file,
                                                                    GKeyFileGroup *group,
                                                                    GList         *pair_node);

static void                  g_key_file_add_key                (GKeyFile               *key_file,
								GKeyFileGroup          *group,
								const gchar            *key,
								const gchar            *value);
static void                  g_key_file_add_group              (GKeyFile               *key_file,
								const gchar            *group_name);
static void                  g_key_file_key_value_pair_free    (GKeyFileKeyValuePair   *pair);
static gboolean              g_key_file_line_is_comment        (const gchar            *line);
static gboolean              g_key_file_line_is_group          (const gchar            *line);
static gboolean              g_key_file_line_is_key_value_pair (const gchar            *line);
static gchar                *g_key_file_parse_value_as_string  (GKeyFile               *key_file,
								const gchar            *value,
								GSList                **separators,
								GError                **error);
static gchar                *g_key_file_parse_string_as_value  (GKeyFile               *key_file,
								const gchar            *string,
								gboolean                escape_separator);
static gint                  g_key_file_parse_value_as_integer (GKeyFile               *key_file,
								const gchar            *value,
								GError                **error);
static gchar                *g_key_file_parse_integer_as_value (GKeyFile               *key_file,
								gint                    value);
static gboolean              g_key_file_parse_value_as_boolean (GKeyFile               *key_file,
								const gchar            *value,
								GError                **error);
static gchar                *g_key_file_parse_boolean_as_value (GKeyFile               *key_file,
								gboolean                value);
static gchar                *g_key_file_parse_value_as_comment (GKeyFile               *key_file,
                                                                const gchar            *value);
static gchar                *g_key_file_parse_comment_as_value (GKeyFile               *key_file,
                                                                const gchar            *comment);
static void                  g_key_file_parse_key_value_pair   (GKeyFile               *key_file,
								const gchar            *line,
								gsize                   length,
								GError                **error);
static void                  g_key_file_parse_comment          (GKeyFile               *key_file,
								const gchar            *line,
								gsize                   length,
								GError                **error);
static void                  g_key_file_parse_group            (GKeyFile               *key_file,
								const gchar            *line,
								gsize                   length,
								GError                **error);
static void                  g_key_file_parse_data             (GKeyFile               *key_file,
								const gchar            *data,
								gsize                   length,
								GError                **error);
static void                  g_key_file_flush_parse_buffer     (GKeyFile               *key_file,
								GError                **error);


GQuark
g_key_file_error_quark (void)
{
  static GQuark error_quark = 0;

  if (error_quark == 0)
    error_quark = g_quark_from_static_string ("g-key-file-error-quark");

  return error_quark;
}

static void
g_key_file_init (GKeyFile *key_file)
{
  key_file->current_group = g_new0 (GKeyFileGroup, 1);
  key_file->groups = g_list_prepend (NULL, key_file->current_group);
  key_file->start_group = NULL;
  key_file->parse_buffer = g_string_sized_new (128);
  key_file->approximate_size = 0;
  key_file->list_separator = ';';
  key_file->flags = 0;
}

static void
g_key_file_clear (GKeyFile *key_file)
{
  GList *tmp, *group_node;

  if (key_file->parse_buffer)
    g_string_free (key_file->parse_buffer, TRUE);

  tmp = key_file->groups;
  while (tmp != NULL)
    {
      group_node = tmp;
      tmp = tmp->next;
      g_key_file_remove_group_node (key_file, group_node);
    }

  g_assert (key_file->groups == NULL);
}


/**
 * g_key_file_new:
 *
 * Creates a new empty #GKeyFile object. Use g_key_file_load_from_file(),
 * g_key_file_load_from_data() or g_key_file_load_from_data_dirs() to
 * read an existing key file.
 *
 * Return value: an empty #GKeyFile.
 *
 * Since: 2.6
 **/
GKeyFile *
g_key_file_new (void)
{
  GKeyFile *key_file;

  key_file = g_new0 (GKeyFile, 1);
  g_key_file_init (key_file);

  return key_file;
}

/**
 * g_key_file_set_list_separator:
 * @key_file: a #GKeyFile
 * @separator: the separator
 *
 * Sets the character which is used to separate
 * values in lists. Typically ';' or ',' are used
 * as separators. The default list separator is ';'.
 *
 * Since: 2.6
 */
void
g_key_file_set_list_separator (GKeyFile *key_file,
			       gchar     separator)
{
  key_file->list_separator = separator;
}


static gboolean
g_key_file_load_from_fd (GKeyFile       *key_file,
			 gint            fd,
			 GKeyFileFlags   flags,
			 GError        **error)
{
  GError *key_file_error = NULL;
  gsize bytes_read;
  struct stat stat_buf;
  gchar read_buf[4096];

  if (fstat (fd, &stat_buf) < 0)
    {
      g_set_error (error, G_FILE_ERROR,
                   g_file_error_from_errno (errno),
                   "%s", g_strerror (errno));
      return FALSE;
    }

  if (!S_ISREG (stat_buf.st_mode))
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_PARSE,
                   _("Not a regular file"));
      return FALSE;
    }

  if (stat_buf.st_size == 0)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_PARSE,
                   _("File is empty"));
      return FALSE;
    }

  if (key_file->approximate_size > 0)
    {
      g_key_file_clear (key_file);
      g_key_file_init (key_file);
    }
  key_file->flags = flags;

  bytes_read = 0;
  do
    {
      bytes_read = read (fd, read_buf, 4096);

      if (bytes_read == 0)  /* End of File */
        break;

      if (bytes_read < 0)
        {
          if (errno == EINTR || errno == EAGAIN)
            continue;

          g_set_error (error, G_FILE_ERROR,
                       g_file_error_from_errno (errno),
                       "%s", g_strerror (errno));
          return FALSE;
        }

      g_key_file_parse_data (key_file,
			     read_buf, bytes_read,
			     &key_file_error);
    }
  while (!key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return FALSE;
    }

  g_key_file_flush_parse_buffer (key_file, &key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return FALSE;
    }

  return TRUE;
}

/**
 * g_key_file_load_from_file:
 * @key_file: an empty #GKeyFile struct
 * @file: the path of a filename to load, in the GLib file name encoding
 * @flags: flags from #GKeyFileFlags
 * @error: return location for a #GError, or %NULL
 *
 * Loads a key file into an empty #GKeyFile structure.
 * If the file could not be loaded then %error is set to
 * either a #GFileError or #GKeyFileError.
 *
 * Return value: %TRUE if a key file could be loaded, %FALSE othewise
 * Since: 2.6
 **/
gboolean
g_key_file_load_from_file (GKeyFile       *key_file,
			   const gchar    *file,
			   GKeyFileFlags   flags,
			   GError        **error)
{
  GError *key_file_error = NULL;
  gint fd;

  g_return_val_if_fail (key_file != NULL, FALSE);
  g_return_val_if_fail (file != NULL, FALSE);

  fd = g_open (file, O_RDONLY, 0);

  if (fd < 0)
    {
      g_set_error (error, G_FILE_ERROR,
                   g_file_error_from_errno (errno),
                   "%s", g_strerror (errno));
      return FALSE;
    }

  g_key_file_load_from_fd (key_file, fd, flags, &key_file_error);
  close (fd);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return FALSE;
    }

  return TRUE;
}

/**
 * g_key_file_load_from_data:
 * @key_file: an empty #GKeyFile struct
 * @data: key file loaded in memory.
 * @length: the length of @data in bytes
 * @flags: flags from #GKeyFileFlags
 * @error: return location for a #GError, or %NULL
 *
 * Loads a key file from memory into an empty #GKeyFile structure.  If
 * the object cannot be created then %error is set to a
 * #GKeyFileError.
 *
 * Return value: %TRUE if a key file could be loaded, %FALSE othewise
 * Since: 2.6
 **/
gboolean
g_key_file_load_from_data (GKeyFile       *key_file,
			   const gchar    *data,
			   gsize           length,
			   GKeyFileFlags   flags,
			   GError        **error)
{
  GError *key_file_error = NULL;

  g_return_val_if_fail (key_file != NULL, FALSE);
  g_return_val_if_fail (data != NULL, FALSE);
  g_return_val_if_fail (length != 0, FALSE);

  if (length == (gsize)-1)
    length = strlen (data);

  if (key_file->approximate_size > 0)
    {
      g_key_file_clear (key_file);
      g_key_file_init (key_file);
    }
  key_file->flags = flags;

  g_key_file_parse_data (key_file, data, length, &key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return FALSE;
    }

  g_key_file_flush_parse_buffer (key_file, &key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return FALSE;
    }

  return TRUE;
}

/**
 * g_key_file_free:
 * @key_file: a #GKeyFile
 *
 * Frees a #GKeyFile.
 *
 * Since: 2.6
 **/
void
g_key_file_free (GKeyFile *key_file)
{
  g_return_if_fail (key_file != NULL);

  g_key_file_clear (key_file);
  g_free (key_file);
}

static void
g_key_file_parse_line (GKeyFile     *key_file,
		       const gchar  *line,
		       gsize         length,
		       GError      **error)
{
  GError *parse_error = NULL;
  gchar *line_start;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (line != NULL);

  line_start = (gchar *) line;
  while (g_ascii_isspace (*line_start))
    line_start++;

  if (g_key_file_line_is_comment (line_start))
    g_key_file_parse_comment (key_file, line, length, &parse_error);
  else if (g_key_file_line_is_group (line_start))
    g_key_file_parse_group (key_file, line_start,
			    length - (line_start - line),
			    &parse_error);
  else if (g_key_file_line_is_key_value_pair (line_start))
    g_key_file_parse_key_value_pair (key_file, line_start,
				     length - (line_start - line),
				     &parse_error);
  else
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_PARSE,
                   _("Key file contains line '%s' which is not "
                     "a key-value pair, group, or comment"), line);
      return;
    }

  if (parse_error)
    g_propagate_error (error, parse_error);
}

static void
g_key_file_parse_comment (GKeyFile     *key_file,
			  const gchar  *line,
			  gsize         length,
			  GError      **error)
{
  GKeyFileKeyValuePair *pair;

  if (!(key_file->flags & G_KEY_FILE_KEEP_COMMENTS))
    return;

  g_assert (key_file->current_group != NULL);

  pair = g_new0 (GKeyFileKeyValuePair, 1);

  pair->key = NULL;
  pair->value = g_strndup (line, length);

  key_file->current_group->key_value_pairs =
    g_list_prepend (key_file->current_group->key_value_pairs, pair);
}

static void
g_key_file_parse_group (GKeyFile     *key_file,
			const gchar  *line,
			gsize         length,
			GError      **error)
{
  gchar *group_name;
  const gchar *group_name_start, *group_name_end;

  /* advance past opening '['
   */
  group_name_start = line + 1;
  group_name_end = line + length - 1;

  while (*group_name_end != ']')
    group_name_end--;

  group_name = g_strndup (group_name_start,
                          group_name_end - group_name_start);

  g_key_file_add_group (key_file, group_name);
  g_free (group_name);
}

static void
g_key_file_parse_key_value_pair (GKeyFile     *key_file,
				 const gchar  *line,
				 gsize         length,
				 GError      **error)
{
  gchar *key, *value, *key_end, *value_start;
  gsize key_len, value_len;

  if (key_file->current_group == NULL || key_file->current_group->name == NULL)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
		   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
		   _("Key file does not start with a group"));
      return;
    }

  key_end = value_start = strchr (line, '=');

  g_assert (key_end != NULL);

  key_end--;
  value_start++;

  /* Pull the key name from the line (chomping trailing whitespace)
   */
  while (g_ascii_isspace (*key_end))
    key_end--;

  key_len = key_end - line + 2;

  g_assert (key_len <= length);

  key = g_strndup (line, key_len - 1);

  /* Pull the value from the line (chugging leading whitespace)
   */
  while (g_ascii_isspace (*value_start))
    value_start++;

  value_len = line + length - value_start + 1;

  value = g_strndup (value_start, value_len);

  g_assert (key_file->start_group != NULL);

  if (key_file->current_group
      && key_file->current_group->name
      && strcmp (key_file->start_group->name,
                 key_file->current_group->name) == 0
      && strcmp (key, "Encoding") == 0)
    {
      if (g_ascii_strcasecmp (value, "UTF-8") != 0)
        {
          g_set_error (error, G_KEY_FILE_ERROR,
                       G_KEY_FILE_ERROR_UNKNOWN_ENCODING,
                       _("Key file contains unsupported encoding '%s'"), value);

          g_free (key);
          g_free (value);
          return;
        }
    }

  g_key_file_add_key (key_file, key_file->current_group, key, value);

  g_free (key);
  g_free (value);
}

static void
g_key_file_parse_data (GKeyFile     *key_file,
		       const gchar  *data,
		       gsize         length,
		       GError      **error)
{
  GError *parse_error;
  gsize i;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (data != NULL);

  parse_error = NULL;

  for (i = 0; i < length; i++)
    {
      if (data[i] == '\n')
        {
	  if (i > 0 && data[i - 1] == '\r')
	    g_string_erase (key_file->parse_buffer,
			    key_file->parse_buffer->len - 1,
			    1);

          /* When a newline is encountered flush the parse buffer so that the
           * line can be parsed.  Note that completely blank lines won't show
           * up in the parse buffer, so they get parsed directly.
           */
          if (key_file->parse_buffer->len > 0)
            g_key_file_flush_parse_buffer (key_file, &parse_error);
          else
            g_key_file_parse_comment (key_file, "", 1, &parse_error);

          if (parse_error)
            {
              g_propagate_error (error, parse_error);
              return;
            }
        }
      else
        g_string_append_c (key_file->parse_buffer, data[i]);
    }

  key_file->approximate_size += length;
}

static void
g_key_file_flush_parse_buffer (GKeyFile  *key_file,
			       GError   **error)
{
  GError *file_error = NULL;

  g_return_if_fail (key_file != NULL);

  file_error = NULL;

  if (key_file->parse_buffer->len > 0)
    {
      g_key_file_parse_line (key_file, key_file->parse_buffer->str,
			     key_file->parse_buffer->len,
			     &file_error);
      g_string_erase (key_file->parse_buffer, 0, key_file->parse_buffer->len);

      if (file_error)
        {
          g_propagate_error (error, file_error);
          return;
        }
    }
}

/**
 * g_key_file_to_data:
 * @key_file: a #GKeyFile
 * @length: return location for the length of the
 *   returned string, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * This function outputs @key_file as a string.
 *
 * Return value: a newly allocated string holding
 *   the contents of the #GKeyFile
 *
 * Since: 2.6
 **/
gchar *
g_key_file_to_data (GKeyFile  *key_file,
		    gsize     *length,
		    GError   **error)
{
  GString *data_string;
  gchar *data;
  GList *group_node, *key_file_node;

  g_return_val_if_fail (key_file != NULL, NULL);

  data_string = g_string_sized_new (2 * key_file->approximate_size);

  for (group_node = g_list_last (key_file->groups);
       group_node != NULL;
       group_node = group_node->prev)
    {
      GKeyFileGroup *group;

      group = (GKeyFileGroup *) group_node->data;

      if (group->comment != NULL)
        g_string_append_printf (data_string, "%s\n", group->comment->value);
      if (group->name != NULL)
        g_string_append_printf (data_string, "[%s]\n", group->name);

      for (key_file_node = g_list_last (group->key_value_pairs);
           key_file_node != NULL;
           key_file_node = key_file_node->prev)
        {
          GKeyFileKeyValuePair *pair;

          pair = (GKeyFileKeyValuePair *) key_file_node->data;

          if (pair->key != NULL)
            g_string_append_printf (data_string, "%s=%s\n", pair->key, pair->value);
          else
            g_string_append_printf (data_string, "%s\n", pair->value);
        }
    }

  if (length)
    *length = data_string->len;

  data = data_string->str;

  g_string_free (data_string, FALSE);

  return data;
}

/**
 * g_key_file_get_keys:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @length: return location for the number of keys returned, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * Returns all keys for the group name @group_name.  The array of
 * returned keys will be %NULL-terminated, so @length may
 * optionally be %NULL. In the event that the @group_name cannot
 * be found, %NULL is returned and @error is set to
 * #G_KEY_FILE_ERROR_GROUP_NOT_FOUND.
 *
 * Return value: a newly-allocated %NULL-terminated array of
 * strings. Use g_strfreev() to free it.
 *
 * Since: 2.6
 **/
gchar **
g_key_file_get_keys (GKeyFile     *key_file,
		     const gchar  *group_name,
		     gsize        *length,
		     GError      **error)
{
  GKeyFileGroup *group;
  GList *tmp;
  gchar **keys;
  gsize i, num_keys;

  g_return_val_if_fail (key_file != NULL, NULL);
  g_return_val_if_fail (group_name != NULL, NULL);

  group = g_key_file_lookup_group (key_file, group_name);

  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");
      return NULL;
    }

  num_keys = g_list_length (group->key_value_pairs);

  keys = (gchar **) g_new0 (gchar **, num_keys + 1);

  tmp = group->key_value_pairs;
  for (i = 1; i <= num_keys; i++)
    {
      GKeyFileKeyValuePair *pair;

      pair = (GKeyFileKeyValuePair *) tmp->data;
      keys[num_keys - i] = g_strdup (pair->key);

      tmp = tmp->next;
    }
  keys[num_keys] = NULL;

  if (length)
    *length = num_keys;

  return keys;
}

/**
 * g_key_file_get_start_group:
 * @key_file: a #GKeyFile
 *
 * Returns the name of the start group of the file.
 *
 * Return value: The start group of the key file.
 *
 * Since: 2.6
 **/
gchar *
g_key_file_get_start_group (GKeyFile *key_file)
{
  g_return_val_if_fail (key_file != NULL, NULL);

  if (key_file->start_group)
    return g_strdup (key_file->start_group->name);

  return NULL;
}

/**
 * g_key_file_get_groups:
 * @key_file: a #GKeyFile
 * @length: return location for the number of returned groups, or %NULL
 *
 * Returns all groups in the key file loaded with @key_file.  The
 * array of returned groups will be %NULL-terminated, so @length may
 * optionally be %NULL.
 *
 * Return value: a newly-allocated %NULL-terminated array of strings.
 *   Use g_strfreev() to free it.
 * Since: 2.6
 **/
gchar **
g_key_file_get_groups (GKeyFile *key_file,
		       gsize    *length)
{
  GList *group_node;
  gchar **groups;
  gsize i, num_groups;

  g_return_val_if_fail (key_file != NULL, NULL);

  num_groups = g_list_length (key_file->groups);

  g_assert (num_groups > 0);

  /* Only need num_groups instead of num_groups + 1
   * because the first group of the file (last in the
   * list) is always the comment group at the top,
   * which we skip
   */
  groups = (gchar **) g_new0 (gchar **, num_groups);

  group_node = g_list_last (key_file->groups);

  g_assert (((GKeyFileGroup *) group_node->data)->name == NULL);

  i = 0;
  for (group_node = group_node->prev;
       group_node != NULL;
       group_node = group_node->prev)
    {
      GKeyFileGroup *group;

      group = (GKeyFileGroup *) group_node->data;

      g_assert (group->name != NULL);

      groups[i++] = g_strdup (group->name);
    }
  groups[i] = NULL;

  if (length)
    *length = i;

  return groups;
}

/**
 * g_key_file_get_value:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @error: return location for a #GError, or %NULL
 *
 * Returns the value associated with @key under @group_name.
 *
 * In the event the key cannot be found, %NULL is returned and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND.  In the
 * event that the @group_name cannot be found, %NULL is returned
 * and @error is set to #G_KEY_FILE_ERROR_GROUP_NOT_FOUND.
 *
 * Return value: a newly allocated string or %NULL if the specified
 * key cannot be found.
 *
 * Since: 2.6
 **/
gchar *
g_key_file_get_value (GKeyFile     *key_file,
		      const gchar  *group_name,
		      const gchar  *key,
		      GError      **error)
{
  GKeyFileGroup *group;
  GKeyFileKeyValuePair *pair;
  gchar *value = NULL;

  g_return_val_if_fail (key_file != NULL, NULL);
  g_return_val_if_fail (group_name != NULL, NULL);
  g_return_val_if_fail (key != NULL, NULL);

  group = g_key_file_lookup_group (key_file, group_name);

  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");
      return NULL;
    }

  pair = g_key_file_lookup_key_value_pair (key_file, group, key);

  if (pair)
    value = g_strdup (pair->value);
  else
    g_set_error (error, G_KEY_FILE_ERROR,
                 G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                 _("Key file does not have key '%s'"), key);

  return value;
}

/**
 * g_key_file_set_value:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @value: a string
 *
 * Associates a new value with @key under @group_name.  If @key
 * cannot be found then it is created. If @group_name cannot be
 * found then it is created.
 *
 * Since: 2.6
 **/
void
g_key_file_set_value (GKeyFile    *key_file,
		      const gchar *group_name,
		      const gchar *key,
		      const gchar *value)
{
  GKeyFileGroup *group;
  GKeyFileKeyValuePair *pair;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);
  g_return_if_fail (value != NULL);

  group = g_key_file_lookup_group (key_file, group_name);

  if (!group)
    {
      g_key_file_add_group (key_file, group_name);
      group = (GKeyFileGroup *) key_file->groups->data;

      g_key_file_add_key (key_file, group, key, value);
    }
  else
    {
      pair = g_key_file_lookup_key_value_pair (key_file, group, key);

      if (!pair)
        g_key_file_add_key (key_file, group, key, value);
      else
        {
          g_free (pair->value);
          pair->value = g_strdup (value);
        }
    }
}

/**
 * g_key_file_get_string:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @error: return location for a #GError, or %NULL
 *
 * Returns the value associated with @key under @group_name.
 *
 * In the event the key cannot be found, %NULL is returned and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND.  In the
 * event that the @group_name cannot be found, %NULL is returned
 * and @error is set to #G_KEY_FILE_ERROR_GROUP_NOT_FOUND.
 *
 * Return value: a newly allocated string or %NULL if the specified
 * key cannot be found.
 *
 * Since: 2.6
 **/
gchar *
g_key_file_get_string (GKeyFile     *key_file,
		       const gchar  *group_name,
		       const gchar  *key,
		       GError      **error)
{
  gchar *value, *string_value;
  GError *key_file_error;

  g_return_val_if_fail (key_file != NULL, NULL);
  g_return_val_if_fail (group_name != NULL, NULL);
  g_return_val_if_fail (key != NULL, NULL);

  key_file_error = NULL;

  value = g_key_file_get_value (key_file, group_name, key, &key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return NULL;
    }

  string_value = g_key_file_parse_value_as_string (key_file, value, NULL,
						   &key_file_error);
  g_free (value);

  if (key_file_error)
    {
      if (g_error_matches (key_file_error,
                           G_KEY_FILE_ERROR,
                           G_KEY_FILE_ERROR_INVALID_VALUE))
        {
          g_set_error (error, G_KEY_FILE_ERROR,
                       G_KEY_FILE_ERROR_INVALID_VALUE,
                       _("Key file contains key '%s' "
                         "which has value that cannot be interpreted."),
                       key);
          g_error_free (key_file_error);
        }
      else
        g_propagate_error (error, key_file_error);
    }

  return string_value;
}

/**
 * g_key_file_set_string:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @string: a string
 *
 * Associates a new string value with @key under @group_name.  If
 * @key cannot be found then it is created.  If @group_name
 * cannot be found then it is created.
 *
 * Since: 2.6
 **/
void
g_key_file_set_string (GKeyFile    *key_file,
		       const gchar *group_name,
		       const gchar *key,
		       const gchar *string)
{
  gchar *value;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);
  g_return_if_fail (string != NULL);

  value = g_key_file_parse_string_as_value (key_file, string, FALSE);
  g_key_file_set_value (key_file, group_name, key, value);
  g_free (value);
}

/**
 * g_key_file_get_string_list:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @length: return location for the number of returned strings, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * Returns the values associated with @key under @group_name.
 *
 * In the event the key cannot be found, %NULL is returned and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND.  In the
 * event that the @group_name cannot be found, %NULL is returned
 * and @error is set to #G_KEY_FILE_ERROR_GROUP_NOT_FOUND.
 *
 * Return value: a %NULL-terminated string array or %NULL if the specified
 *   key cannot be found. The array should be freed with g_strfreev().
 *
 * Since: 2.6
 **/
gchar **
g_key_file_get_string_list (GKeyFile     *key_file,
			    const gchar  *group_name,
			    const gchar  *key,
			    gsize        *length,
			    GError      **error)
{
  GError *key_file_error = NULL;
  gchar *value, *string_value, **values;
  gint i, len;
  GSList *p, *pieces = NULL;

  g_return_val_if_fail (key_file != NULL, NULL);
  g_return_val_if_fail (group_name != NULL, NULL);
  g_return_val_if_fail (key != NULL, NULL);

  value = g_key_file_get_value (key_file, group_name, key, &key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return NULL;
    }

  string_value = g_key_file_parse_value_as_string (key_file, value, &pieces, &key_file_error);
  g_free (value);
  g_free (string_value);

  if (key_file_error)
    {
      if (g_error_matches (key_file_error,
                           G_KEY_FILE_ERROR,
                           G_KEY_FILE_ERROR_INVALID_VALUE))
        {
          g_set_error (error, G_KEY_FILE_ERROR,
                       G_KEY_FILE_ERROR_INVALID_VALUE,
                       _("Key file contains key '%s' "
                         "which has value that cannot be interpreted."),
                       key);
          g_error_free (key_file_error);
        }
      else
        g_propagate_error (error, key_file_error);
    }

  len = g_slist_length (pieces);
  values = g_new0 (gchar *, len + 1);
  for (p = pieces, i = 0; p; p = p->next)
    values[i++] = p->data;
  values[len] = NULL;

  g_slist_free (pieces);

  if (length)
    *length = len;

  return values;
}

/**
 * g_key_file_set_string_list:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @list: an array of locale string values
 * @length: number of locale string values in @list
 *
 * Associates a list of string values for @key under @group_name.
 * If @key cannot be found then it is created.  If @group_name
 * cannot be found then it is created.
 *
 * Since: 2.6
 **/
void
g_key_file_set_string_list (GKeyFile            *key_file,
			    const gchar         *group_name,
			    const gchar         *key,
			    const gchar * const  list[],
			    gsize                length)
{
  GString *value_list;
  gsize i;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);
  g_return_if_fail (list != NULL);

  value_list = g_string_sized_new (length * 128);
  for (i = 0; list[i] != NULL && i < length; i++)
    {
      gchar *value;

      value = g_key_file_parse_string_as_value (key_file, list[i], TRUE);
      g_string_append (value_list, value);
      g_string_append_c (value_list, key_file->list_separator);

      g_free (value);
    }

  g_key_file_set_value (key_file, group_name, key, value_list->str);
  g_string_free (value_list, TRUE);
}

/**
 * g_key_file_get_boolean:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @error: return location for a #GError
 *
 * Returns the value associated with @key under @group_name as a
 * boolean. If @group_name is %NULL, the start group is used.
 *
 * If @key cannot be found then the return value is undefined and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND. Likewise, if
 * the value associated with @key cannot be interpreted as a boolean
 * then the return value is also undefined and @error is set to
 * #G_KEY_FILE_ERROR_INVALID_VALUE.
 *
 * Return value: the value associated with the key as a boolean
 * Since: 2.6
 **/
gboolean
g_key_file_get_boolean (GKeyFile     *key_file,
			const gchar  *group_name,
			const gchar  *key,
			GError      **error)
{
  GError *key_file_error = NULL;
  gchar *value;
  gboolean bool_value;

  g_return_val_if_fail (key_file != NULL, FALSE);
  g_return_val_if_fail (group_name != NULL, FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  value = g_key_file_get_value (key_file, group_name, key, &key_file_error);

  if (!value)
    {
      g_propagate_error (error, key_file_error);
      return FALSE;
    }

  bool_value = g_key_file_parse_value_as_boolean (key_file, value,
						  &key_file_error);
  g_free (value);

  if (key_file_error)
    {
      if (g_error_matches (key_file_error,
                           G_KEY_FILE_ERROR,
                           G_KEY_FILE_ERROR_INVALID_VALUE))
        {
          g_set_error (error, G_KEY_FILE_ERROR,
                       G_KEY_FILE_ERROR_INVALID_VALUE,
                       _("Key file contains key '%s' "
                         "which has value that cannot be interpreted."),
                       key);
          g_error_free (key_file_error);
        }
      else
        g_propagate_error (error, key_file_error);
    }

  return bool_value;
}

/**
 * g_key_file_set_boolean:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @value: %TRUE or %FALSE
 *
 * Associates a new boolean value with @key under @group_name.
 * If @key cannot be found then it is created. If @group_name
 * is %NULL, the start group is used.
 *
 * Since: 2.6
 **/
void
g_key_file_set_boolean (GKeyFile    *key_file,
			const gchar *group_name,
			const gchar *key,
			gboolean     value)
{
  gchar *result;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);

  result = g_key_file_parse_boolean_as_value (key_file, value);
  g_key_file_set_value (key_file, group_name, key, result);
  g_free (result);
}

/**
 * g_key_file_get_boolean_list:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @length: the number of booleans returned
 * @error: return location for a #GError
 *
 * Returns the values associated with @key under @group_name as
 * booleans. If @group_name is %NULL, the start_group is used.
 *
 * If @key cannot be found then the return value is undefined and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND. Likewise, if
 * the values associated with @key cannot be interpreted as booleans
 * then the return value is also undefined and @error is set to
 * #G_KEY_FILE_ERROR_INVALID_VALUE.
 *
 * Return value: the values associated with the key as a boolean
 *
 * Since: 2.6
 **/
gboolean *
g_key_file_get_boolean_list (GKeyFile     *key_file,
			     const gchar  *group_name,
			     const gchar  *key,
			     gsize        *length,
			     GError      **error)
{
  GError *key_file_error;
  gchar **values;
  gboolean *bool_values;
  gsize i, num_bools;

  g_return_val_if_fail (key_file != NULL, NULL);
  g_return_val_if_fail (group_name != NULL, NULL);
  g_return_val_if_fail (key != NULL, NULL);

  key_file_error = NULL;

  values = g_key_file_get_string_list (key_file, group_name, key,
				       &num_bools, &key_file_error);

  if (key_file_error)
    g_propagate_error (error, key_file_error);

  if (!values)
    return NULL;

  bool_values = g_new0 (gboolean, num_bools);

  for (i = 0; i < num_bools; i++)
    {
      bool_values[i] = g_key_file_parse_value_as_boolean (key_file,
							  values[i],
							  &key_file_error);

      if (key_file_error)
        {
          g_propagate_error (error, key_file_error);
          g_strfreev (values);
          g_free (bool_values);

          return NULL;
        }
    }
  g_strfreev (values);

  if (length)
    *length = num_bools;

  return bool_values;
}

/**
 * g_key_file_set_boolean_list:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @list: an array of boolean values
 * @length: length of @list
 *
 * Associates a list of boolean values with @key under
 * @group_name.  If @key cannot be found then it is created.
 * If @group_name is %NULL, the start_group is used.
 *
 * Since: 2.6
 **/
void
g_key_file_set_boolean_list (GKeyFile    *key_file,
			     const gchar *group_name,
			     const gchar *key,
			     gboolean     list[],
			     gsize        length)
{
  GString *value_list;
  gsize i;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);
  g_return_if_fail (list != NULL);

  value_list = g_string_sized_new (length * 8);
  for (i = 0; i < length; i++)
    {
      gchar *value;

      value = g_key_file_parse_boolean_as_value (key_file, list[i]);

      g_string_append (value_list, value);
      g_string_append_c (value_list, key_file->list_separator);

      g_free (value);
    }

  g_key_file_set_value (key_file, group_name, key, value_list->str);
  g_string_free (value_list, TRUE);
}

/**
 * g_key_file_get_integer:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @error: return location for a #GError
 *
 * Returns the value associated with @key under @group_name as an
 * integer. If @group_name is %NULL, the start_group is used.
 *
 * If @key cannot be found then the return value is undefined and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND. Likewise, if
 * the value associated with @key cannot be interpreted as an integer
 * then the return value is also undefined and @error is set to
 * #G_KEY_FILE_ERROR_INVALID_VALUE.
 *
 * Return value: the value associated with the key as an integer.
 *
 * Since: 2.6
 **/
gint
g_key_file_get_integer (GKeyFile     *key_file,
			const gchar  *group_name,
			const gchar  *key,
			GError      **error)
{
  GError *key_file_error;
  gchar *value;
  gint int_value;

  g_return_val_if_fail (key_file != NULL, -1);
  g_return_val_if_fail (group_name != NULL, -1);
  g_return_val_if_fail (key != NULL, -1);

  key_file_error = NULL;

  value = g_key_file_get_value (key_file, group_name, key, &key_file_error);

  if (key_file_error)
    {
      g_propagate_error (error, key_file_error);
      return 0;
    }

  int_value = g_key_file_parse_value_as_integer (key_file, value,
						 &key_file_error);
  g_free (value);

  if (key_file_error)
    {
      if (g_error_matches (key_file_error,
                           G_KEY_FILE_ERROR,
                           G_KEY_FILE_ERROR_INVALID_VALUE))
        {
          g_set_error (error, G_KEY_FILE_ERROR,
                       G_KEY_FILE_ERROR_INVALID_VALUE,
                       _("Key file contains key '%s' in group '%s' "
                         "which has value that cannot be interpreted."), key,
                       group_name);
          g_error_free (key_file_error);
        }
      else
        g_propagate_error (error, key_file_error);
    }

  return int_value;
}

/**
 * g_key_file_set_integer:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @value: an integer value
 *
 * Associates a new integer value with @key under @group_name.
 * If @key cannot be found then it is created. If @group_name
 * is %NULL, the start group is used.
 *
 * Since: 2.6
 **/
void
g_key_file_set_integer (GKeyFile    *key_file,
			const gchar *group_name,
			const gchar *key,
			gint         value)
{
  gchar *result;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);

  result = g_key_file_parse_integer_as_value (key_file, value);
  g_key_file_set_value (key_file, group_name, key, result);
  g_free (result);
}

/**
 * g_key_file_get_integer_list:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @length: the number of integers returned
 * @error: return location for a #GError
 *
 * Returns the values associated with @key under @group_name as
 * integers. If @group_name is %NULL, the start group is used.
 *
 * If @key cannot be found then the return value is undefined and
 * @error is set to #G_KEY_FILE_ERROR_KEY_NOT_FOUND. Likewise, if
 * the values associated with @key cannot be interpreted as integers
 * then the return value is also undefined and @error is set to
 * #G_KEY_FILE_ERROR_INVALID_VALUE.
 *
 * Return value: the values associated with the key as a integer
 *
 * Since: 2.6
 **/
gint *
g_key_file_get_integer_list (GKeyFile     *key_file,
			     const gchar  *group_name,
			     const gchar  *key,
			     gsize        *length,
			     GError      **error)
{
  GError *key_file_error = NULL;
  gchar **values;
  gint *int_values;
  gsize i, num_ints;

  g_return_val_if_fail (key_file != NULL, NULL);
  g_return_val_if_fail (group_name != NULL, NULL);
  g_return_val_if_fail (key != NULL, NULL);

  values = g_key_file_get_string_list (key_file, group_name, key,
				       &num_ints, &key_file_error);

  if (key_file_error)
    g_propagate_error (error, key_file_error);

  if (!values)
    return NULL;

  int_values = g_new0 (gint, num_ints);

  for (i = 0; i < num_ints; i++)
    {
      int_values[i] = g_key_file_parse_value_as_integer (key_file,
							 values[i],
							 &key_file_error);

      if (key_file_error)
        {
          g_propagate_error (error, key_file_error);
          g_strfreev (values);
          g_free (int_values);

          return NULL;
        }
    }
  g_strfreev (values);

  if (length)
    *length = num_ints;

  return int_values;
}

/**
 * g_key_file_set_integer_list:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key
 * @list: an array of integer values
 * @length: number of integer values in @list
 *
 * Associates a list of integer values with @key under
 * @group_name.  If @key cannot be found then it is created.
 * If @group_name is %NULL the start group is used.
 *
 * Since: 2.6
 **/
void
g_key_file_set_integer_list (GKeyFile     *key_file,
			     const gchar  *group_name,
			     const gchar  *key,
			     gint          list[],
			     gsize         length)
{
  GString *values;
  gsize i;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);
  g_return_if_fail (list != NULL);

  values = g_string_sized_new (length * 16);
  for (i = 0; i < length; i++)
    {
      gchar *value;

      value = g_key_file_parse_integer_as_value (key_file, list[i]);

      g_string_append (values, value);
      g_string_append_c (values, ';');

      g_free (value);
    }

  g_key_file_set_value (key_file, group_name, key, values->str);
  g_string_free (values, TRUE);
}

static void
g_key_file_set_key_comment (GKeyFile             *key_file,
                            const gchar          *group_name,
                            const gchar          *key,
                            const gchar          *comment,
                            GError              **error)
{
  GKeyFileGroup *group;
  GKeyFileKeyValuePair *pair;
  GList *key_node, *comment_node, *tmp;

  group = g_key_file_lookup_group (key_file, group_name);
  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");

      return;
    }

  /* First find the key the comments are supposed to be
   * associated with
   */
  key_node = g_key_file_lookup_key_value_pair_node (key_file, group, key);

  if (key_node == NULL)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                   _("Key file does not have key '%s' in group '%s'"),
                   key, group->name);
      return;
    }

  /* Then find all the comments already associated with the
   * key and free them
   */
  tmp = key_node->next;
  while (tmp != NULL)
    {
      GKeyFileKeyValuePair *pair;

      pair = (GKeyFileKeyValuePair *) tmp->data;

      if (pair->key != NULL)
        break;

      comment_node = tmp;
      tmp = tmp->next;
      g_key_file_remove_key_value_pair_node (key_file, group,
                                             comment_node);
    }

  if (comment == NULL)
    return;

  /* Now we can add our new comment
   */
  pair = g_new0 (GKeyFileKeyValuePair, 1);

  pair->key = NULL;
  pair->value = g_key_file_parse_comment_as_value (key_file, comment);

  key_node = g_list_insert (key_node, pair, 1);
}

static void
g_key_file_set_group_comment (GKeyFile             *key_file,
                              const gchar          *group_name,
                              const gchar          *comment,
                              GError              **error)
{
  GKeyFileGroup *group;

  group = g_key_file_lookup_group (key_file, group_name);
  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");

      return;
    }

  /* First remove any existing comment
   */
  if (group->comment)
    {
      g_key_file_key_value_pair_free (group->comment);
      group->comment = NULL;
    }

  if (comment == NULL)
    return;

  /* Now we can add our new comment
   */
  group->comment = g_new0 (GKeyFileKeyValuePair, 1);

  group->comment->key = NULL;
  group->comment->value = g_key_file_parse_comment_as_value (key_file, comment);
}

static void
g_key_file_set_top_comment (GKeyFile             *key_file,
                            const gchar          *comment,
                            GError              **error)
{
  GList *group_node;
  GKeyFileGroup *group;
  GKeyFileKeyValuePair *pair;

  /* The last group in the list should be the top (comments only)
   * group in the file
   */
  g_assert (key_file->groups != NULL);
  group_node = g_list_last (key_file->groups);
  group = (GKeyFileGroup *) group_node->data;
  g_assert (group->name == NULL);

  /* Note all keys must be comments at the top of
   * the file, so we can just free it all.
   */
  if (group->key_value_pairs != NULL)
    {
      g_list_foreach (group->key_value_pairs,
                      (GFunc) g_key_file_key_value_pair_free,
                      NULL);
      g_list_free (group->key_value_pairs);
      group->key_value_pairs = NULL;
    }

  if (comment == NULL)
     return;

  pair = g_new0 (GKeyFileKeyValuePair, 1);

  pair->key = NULL;
  pair->value = g_key_file_parse_comment_as_value (key_file, comment);

  group->key_value_pairs =
    g_list_prepend (group->key_value_pairs, pair);
}

/**
 * g_key_file_set_comment:
 * @key_file: a #GKeyFile
 * @group_name: a group name, or %NULL
 * @key: a key
 * @comment: a comment
 * @error: return location for a #GError
 *
 * Places a comment above @key from @group_name.
 * @group_name. If @key is %NULL then @comment will
 * be written above @group_name.  If both @key
 * and @group_name are NULL, then @comment will
 * be written above the first group in the file.
 *
 * Since: 2.6
 **/
void
g_key_file_set_comment (GKeyFile             *key_file,
                        const gchar          *group_name,
                        const gchar          *key,
                        const gchar          *comment,
                        GError              **error)
{
  g_return_if_fail (key_file != NULL);

  if (group_name != NULL && key != NULL)
    g_key_file_set_key_comment (key_file, group_name, key, comment, error);
  else if (group_name != NULL)
    g_key_file_set_group_comment (key_file, group_name, comment, error);
  else
    g_key_file_set_top_comment (key_file, comment, error);

  if (comment != NULL)
    key_file->approximate_size += strlen (comment);
}

static gchar *
g_key_file_get_key_comment (GKeyFile             *key_file,
                            const gchar          *group_name,
                            const gchar          *key,
                            GError              **error)
{
  GKeyFileGroup *group;
  GList *key_node, *tmp;
  GString *string;
  gchar *comment;

  group = g_key_file_lookup_group (key_file, group_name);
  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");

      return NULL;
    }

  /* First find the key the comments are supposed to be
   * associated with
   */
  key_node = g_key_file_lookup_key_value_pair_node (key_file, group, key);

  if (key_node == NULL)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                   _("Key file does not have key '%s' in group '%s'"),
                   key, group->name);
      return NULL;
    }

  string = NULL;

  /* Then find all the comments already associated with the
   * key and concatentate them.
   */
  tmp = key_node->next;
  while (tmp != NULL)
    {
      GKeyFileKeyValuePair *pair;

      pair = (GKeyFileKeyValuePair *) tmp->data;

      if (pair->key != NULL)
        break;

      if (string == NULL)
        string = g_string_sized_new (512);

      comment = g_key_file_parse_value_as_comment (key_file, pair->value);
      g_string_append (string, comment);
      g_free (comment);

      tmp = tmp->next;
    }

  if (string != NULL)
    {
      comment = string->str;
      g_string_free (string, FALSE);
    }
  else
    comment = NULL;

  return comment;
}

static gchar *
g_key_file_get_group_comment (GKeyFile             *key_file,
                              const gchar          *group_name,
                              GError              **error)
{
  GKeyFileGroup *group;

  group = g_key_file_lookup_group (key_file, group_name);
  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");

      return NULL;
    }

  if (group->comment)
    return g_strdup (group->comment->value);

  return NULL;
}

static gchar *
g_key_file_get_top_comment (GKeyFile             *key_file,
                            GError              **error)
{
  GList *group_node, *tmp;
  GKeyFileGroup *group;
  GString *string;
  gchar *comment;

  /* The last group in the list should be the top (comments only)
   * group in the file
   */
  g_assert (key_file->groups != NULL);
  group_node = g_list_last (key_file->groups);
  group = (GKeyFileGroup *) group_node->data;
  g_assert (group->name == NULL);

  string = NULL;

  /* Then find all the comments already associated with the
   * key and concatentate them.
   */
  tmp = group->key_value_pairs;
  while (tmp != NULL)
    {
      GKeyFileKeyValuePair *pair;

      pair = (GKeyFileKeyValuePair *) tmp->data;

      if (pair->key != NULL)
        break;

      if (string == NULL)
        string = g_string_sized_new (512);

      comment = g_key_file_parse_value_as_comment (key_file, pair->value);
      g_string_append (string, comment);
      g_free (comment);

      tmp = tmp->next;
    }

  if (string != NULL)
    {
      comment = string->str;
      g_string_free (string, FALSE);
    }
  else
    comment = NULL;

  return comment;
}

/**
 * g_key_file_get_comment:
 * @key_file: a #GKeyFile
 * @group_name: a group name, or %NULL
 * @key: a key
 * @error: return location for a #GError
 *
 * Retreives a comment above @key from @group_name.
 * @group_name. If @key is %NULL then @comment will
 * be read from above @group_name.  If both @key
 * and @group_name are NULL, then @comment will
 * be read from above the first group in the file.
 *
 * Since: 2.6
 * Returns: a comment that should be freed with g_free()
 **/
gchar *
g_key_file_get_comment (GKeyFile             *key_file,
                        const gchar          *group_name,
                        const gchar          *key,
                        GError              **error)
{
  g_return_val_if_fail (key_file != NULL, NULL);

  if (group_name != NULL && key != NULL)
    return g_key_file_get_key_comment (key_file, group_name, key, error);
  else if (group_name != NULL)
    return g_key_file_get_group_comment (key_file, group_name, error);
  else
    return g_key_file_get_top_comment (key_file, error);
}

/**
 * g_key_file_remove_comment:
 * @key_file: a #GKeyFile
 * @group_name: a group name, or %NULL
 * @key: a key
 * @error: return location for a #GError
 *
 * Removes a comment above @key from @group_name.
 * @group_name. If @key is %NULL then @comment will
 * be written above @group_name.  If both @key
 * and @group_name are NULL, then @comment will
 * be written above the first group in the file.
 *
 * Since: 2.6
 **/

void
g_key_file_remove_comment (GKeyFile             *key_file,
                           const gchar          *group_name,
                           const gchar          *key,
                           GError              **error)
{
  g_return_if_fail (key_file != NULL);

  if (group_name != NULL && key != NULL)
    g_key_file_set_key_comment (key_file, group_name, key, NULL, error);
  else if (group_name != NULL)
    g_key_file_set_group_comment (key_file, group_name, NULL, error);
  else
    g_key_file_set_top_comment (key_file, NULL, error);
}

/**
 * g_key_file_has_group:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 *
 * Looks whether the key file has the group @group_name.
 *
 * Return value: %TRUE if @group_name is a part of @key_file, %FALSE
 * otherwise.
 * Since: 2.6
 **/
gboolean
g_key_file_has_group (GKeyFile    *key_file,
		      const gchar *group_name)
{
  g_return_val_if_fail (key_file != NULL, FALSE);
  g_return_val_if_fail (group_name != NULL, FALSE);

  return g_key_file_lookup_group_node (key_file, group_name) != NULL;
}

/**
 * g_key_file_has_key:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key name
 * @error: return location for a #GError
 *
 * Looks whether the key file has the key @key in the group
 * @group_name. If @group_name is %NULL, the start group is
 * used.
 *
 * Return value: %TRUE if @key is a part of @group_name, %FALSE
 * otherwise.
 *
 * Since: 2.6
 **/
gboolean
g_key_file_has_key (GKeyFile     *key_file,
		    const gchar  *group_name,
		    const gchar  *key,
		    GError      **error)
{
  GKeyFileKeyValuePair *pair;
  GKeyFileGroup *group;

  g_return_val_if_fail (key_file != NULL, FALSE);
  g_return_val_if_fail (group_name != NULL, FALSE);
  g_return_val_if_fail (key != NULL, FALSE);

  group = g_key_file_lookup_group (key_file, group_name);

  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");

      return FALSE;
    }

  pair = g_key_file_lookup_key_value_pair (key_file, group, key);

  return pair != NULL;
}

static void
g_key_file_add_group (GKeyFile    *key_file,
		      const gchar *group_name)
{
  GKeyFileGroup *group;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (g_key_file_lookup_group_node (key_file, group_name) == NULL);

  group = g_new0 (GKeyFileGroup, 1);
  group->name = g_strdup (group_name);
  group->lookup_map = g_hash_table_new (g_str_hash, g_str_equal);
  key_file->groups = g_list_prepend (key_file->groups, group);
  key_file->approximate_size += strlen (group_name) + 3;
  key_file->current_group = group;

  if (key_file->start_group == NULL)
    key_file->start_group = group;
}

static void
g_key_file_key_value_pair_free (GKeyFileKeyValuePair *pair)
{
  if (pair != NULL)
    {
      g_free (pair->key);
      g_free (pair->value);
      g_free (pair);
    }
}

/* Be careful not to call this function on a node with data in the
 * lookup map without removing it from the lookup map, first.
 *
 * Some current cases where this warning is not a concern are
 * when:
 *   - the node being removed is a comment node
 *   - the entire lookup map is getting destroyed soon after
 *     anyway.
 */
static void
g_key_file_remove_key_value_pair_node (GKeyFile      *key_file,
                                       GKeyFileGroup *group,
			               GList         *pair_node)
{

  GKeyFileKeyValuePair *pair;

  pair = (GKeyFileKeyValuePair *) pair_node->data;

  group->key_value_pairs = g_list_remove_link (group->key_value_pairs, pair_node);

  if (pair->key != NULL)
    key_file->approximate_size -= strlen (pair->key) + 1;

  g_assert (pair->value != NULL);
  key_file->approximate_size -= strlen (pair->value);

  g_key_file_key_value_pair_free (pair);

  g_list_free_1 (pair_node);
}

static void
g_key_file_remove_group_node (GKeyFile *key_file,
			      GList    *group_node)
{
  GKeyFileGroup *group;
  GList *tmp;

  group = (GKeyFileGroup *) group_node->data;

  /* If the current group gets deleted make the current group the last
   * added group.
   */
  if (key_file->current_group == group)
    {
      /* groups should always contain at least the top comment group,
       * unless g_key_file_clear has been called
       */
      if (key_file->groups)
        key_file->current_group = (GKeyFileGroup *) key_file->groups->data;
      else
        key_file->current_group = NULL;
    }

  /* If the start group gets deleted make the start group the first
   * added group.
   */
  if (key_file->start_group == group)
    {
      tmp = g_list_last (key_file->groups);
      while (tmp != NULL)
	{
	  if (tmp != group_node &&
	      ((GKeyFileGroup *) tmp->data)->name != NULL)
	    break;

	  tmp = tmp->prev;
	}

      if (tmp)
        key_file->start_group = (GKeyFileGroup *) tmp->data;
      else
        key_file->start_group = NULL;
    }

  key_file->groups = g_list_remove_link (key_file->groups, group_node);

  if (group->name != NULL)
    key_file->approximate_size -= strlen (group->name) + 3;

  tmp = group->key_value_pairs;
  while (tmp != NULL)
    {
      GList *pair_node;

      pair_node = tmp;
      tmp = tmp->next;
      g_key_file_remove_key_value_pair_node (key_file, group, pair_node);
    }

  g_assert (group->key_value_pairs == NULL);

  if (group->lookup_map)
    {
      g_hash_table_destroy (group->lookup_map);
      group->lookup_map = NULL;
    }

  g_free ((gchar *) group->name);
  g_free (group);
  g_list_free_1 (group_node);
}

/**
 * g_key_file_remove_group:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @error: return location for a #GError or %NULL
 *
 * Removes the specified group, @group_name,
 * from the key file.
 *
 * Since: 2.6
 **/
void
g_key_file_remove_group (GKeyFile     *key_file,
			 const gchar  *group_name,
			 GError      **error)
{
  GList *group_node;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);

  group_node = g_key_file_lookup_group_node (key_file, group_name);

  if (!group_node)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
		   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
		   _("Key file does not have group '%s'"),
		   group_name);
      return;
    }

    g_key_file_remove_group_node (key_file, group_node);
}

static void
g_key_file_add_key (GKeyFile      *key_file,
		    GKeyFileGroup *group,
		    const gchar   *key,
		    const gchar   *value)
{
  GKeyFileKeyValuePair *pair;

  pair = g_new0 (GKeyFileKeyValuePair, 1);

  pair->key = g_strdup (key);
  pair->value = g_strdup (value);

  g_hash_table_remove(group->lookup_map, pair->key);
  g_hash_table_insert(group->lookup_map, pair->key, pair);
  group->key_value_pairs = g_list_prepend (group->key_value_pairs, pair);
  key_file->approximate_size += strlen (key) + strlen (value) + 2;
}

/**
 * g_key_file_remove_key:
 * @key_file: a #GKeyFile
 * @group_name: a group name
 * @key: a key name to remove
 * @error: return location for a #GError or %NULL
 *
 * Removes @key in @group_name from the key file.
 *
 * Since: 2.6
 **/
void
g_key_file_remove_key (GKeyFile     *key_file,
		       const gchar  *group_name,
		       const gchar  *key,
		       GError      **error)
{
  GKeyFileGroup *group;
  GKeyFileKeyValuePair *pair;

  g_return_if_fail (key_file != NULL);
  g_return_if_fail (group_name != NULL);
  g_return_if_fail (key != NULL);

  pair = NULL;

  group = g_key_file_lookup_group (key_file, group_name);
  if (!group)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
                   _("Key file does not have group '%s'"),
                   group_name ? group_name : "(null)");
      return;
    }

  pair = g_key_file_lookup_key_value_pair (key_file, group, key);

  if (!pair)
    {
      g_set_error (error, G_KEY_FILE_ERROR,
                   G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                   _("Key file does not have key '%s' in group '%s'"),
		   key, group->name);
      return;
    }

  key_file->approximate_size -= strlen (pair->key) + strlen (pair->value) + 2;

  group->key_value_pairs = g_list_remove (group->key_value_pairs, pair);
  g_hash_table_remove (group->lookup_map, pair->key);
  g_key_file_key_value_pair_free (pair);
}

static GList *
g_key_file_lookup_group_node (GKeyFile    *key_file,
			      const gchar *group_name)
{
  GKeyFileGroup *group;
  GList *tmp;

  group = NULL;
  for (tmp = key_file->groups; tmp != NULL; tmp = tmp->next)
    {
      group = (GKeyFileGroup *) tmp->data;

      if (group && group->name && strcmp (group->name, group_name) == 0)
        break;

      group = NULL;
    }

  return tmp;
}

static GKeyFileGroup *
g_key_file_lookup_group (GKeyFile    *key_file,
			 const gchar *group_name)
{
  GList *group_node;

  group_node = g_key_file_lookup_group_node (key_file, group_name);

  if (group_node != NULL)
    return (GKeyFileGroup *) group_node->data;

  return NULL;
}

static GList *
g_key_file_lookup_key_value_pair_node (GKeyFile       *key_file,
			               GKeyFileGroup  *group,
                                       const gchar    *key)
{
  GList *key_node;

  for (key_node = group->key_value_pairs;
       key_node != NULL;
       key_node = key_node->next)
    {
      GKeyFileKeyValuePair *pair;

      pair = (GKeyFileKeyValuePair *) key_node->data;

      if (pair->key && strcmp (pair->key, key) == 0)
        break;
    }

  return key_node;
}

static GKeyFileKeyValuePair *
g_key_file_lookup_key_value_pair (GKeyFile      *key_file,
				  GKeyFileGroup *group,
				  const gchar   *key)
{
  return (GKeyFileKeyValuePair *) g_hash_table_lookup (group->lookup_map, key);
}

/* Lines starting with # or consisting entirely of whitespace are merely
 * recorded, not parsed. This function assumes all leading whitespace
 * has been stripped.
 */
static gboolean
g_key_file_line_is_comment (const gchar *line)
{
  return (*line == '#' || *line == '\0' || *line == '\n');
}

/* A group in a key file is made up of a starting '[' followed by one
 * or more letters making up the group name followed by ']'.
 */
static gboolean
g_key_file_line_is_group (const gchar *line)
{
  gchar *p;

  p = (gchar *) line;
  if (*p != '[')
    return FALSE;

  p++;

  if (!*p)
    return FALSE;

  p++;

  /* Group name must be non-empty
   */
  if (*p == ']')
    return FALSE;

  while (*p && *p != ']')
    p++;

  if (!*p)
    return FALSE;

  return TRUE;
}

static gboolean
g_key_file_line_is_key_value_pair (const gchar *line)
{
  gchar *p;

  p = (gchar *) strchr (line, '=');

  if (!p)
    return FALSE;

  /* Key must be non-empty
   */
  if (*p == line[0])
    return FALSE;

  return TRUE;
}

static gchar *
g_key_file_parse_value_as_string (GKeyFile     *key_file,
				  const gchar  *value,
				  GSList      **pieces,
				  GError      **error)
{
  gchar *string_value, *p, *q0, *q;

  string_value = g_new0 (gchar, strlen (value) + 1);

  p = (gchar *) value;
  q0 = q = string_value;
  while (*p)
    {
      if (*p == '\\')
        {
          p++;

          switch (*p)
            {
            case 's':
              *q = ' ';
              break;

            case 'n':
              *q = '\n';
              break;

            case 't':
              *q = '\t';
              break;

            case 'r':
              *q = '\r';
              break;

            case '\\':
              *q = '\\';
              break;

	    case '\0':
	      g_set_error (error, G_KEY_FILE_ERROR,
			   G_KEY_FILE_ERROR_INVALID_VALUE,
			   _("Key file contains escape character "
			     "at end of line"));
	      break;

            default:
	      if (pieces && *p == key_file->list_separator)
		*q = key_file->list_separator;
	      else
		{
		  *q++ = '\\';
		  *q = *p;

		  if (*error == NULL)
		    {
		      gchar sequence[3];

		      sequence[0] = '\\';
		      sequence[1] = *p;
		      sequence[2] = '\0';

		      g_set_error (error, G_KEY_FILE_ERROR,
				   G_KEY_FILE_ERROR_INVALID_VALUE,
				   _("Key file contains invalid escape "
				     "sequence '%s'"), sequence);
		    }
		}
              break;
            }
        }
      else
	{
	  *q = *p;
	  if (pieces && (*p == key_file->list_separator))
	    {
	      *pieces = g_slist_prepend (*pieces, g_strndup (q0, q - q0));
	      q0 = q + 1;
	    }
	}

      if (*p == '\0')
	break;

      q++;
      p++;
    }

  *q = '\0';
  if (pieces)
  {
    if (q0 < q)
      *pieces = g_slist_prepend (*pieces, g_strndup (q0, q - q0));
    *pieces = g_slist_reverse (*pieces);
  }

  return string_value;
}

static gchar *
g_key_file_parse_string_as_value (GKeyFile    *key_file,
				  const gchar *string,
				  gboolean     escape_separator)
{
  gchar *value, *p, *q;
  gsize length;
  gboolean parsing_leading_space;

  length = strlen (string) + 1;

  /* Worst case would be that every character needs to be escaped.
   * In other words every character turns to two characters
   */
  value = g_new0 (gchar, 2 * length);

  p = (gchar *) string;
  q = value;
  parsing_leading_space = TRUE;
  while (p < (string + length - 1))
    {
      gchar escaped_character[3] = { '\\', 0, 0 };

      switch (*p)
        {
        case ' ':
          if (parsing_leading_space)
            {
              escaped_character[1] = 's';
              strcpy (q, escaped_character);
              q += 2;
            }
          else
            {
	      *q = *p;
	      q++;
            }
          break;
        case '\t':
          if (parsing_leading_space)
            {
              escaped_character[1] = 't';
              strcpy (q, escaped_character);
              q += 2;
            }
          else
            {
	      *q = *p;
	      q++;
            }
          break;
        case '\n':
          escaped_character[1] = 'n';
          strcpy (q, escaped_character);
          q += 2;
          break;
        case '\r':
          escaped_character[1] = 'r';
          strcpy (q, escaped_character);
          q += 2;
          break;
        case '\\':
          escaped_character[1] = '\\';
          strcpy (q, escaped_character);
          q += 2;
          parsing_leading_space = FALSE;
          break;
        default:
	  if (escape_separator && *p == key_file->list_separator)
	    {
	      escaped_character[1] = key_file->list_separator;
	      strcpy (q, escaped_character);
	      q += 2;
              parsing_leading_space = TRUE;
	    }
	  else
	    {
	      *q = *p;
	      q++;
              parsing_leading_space = FALSE;
	    }
          break;
        }
      p++;
    }
  *q = '\0';

  return value;
}

static gint
g_key_file_parse_value_as_integer (GKeyFile     *key_file,
				   const gchar  *value,
				   GError      **error)
{
  gchar *end_of_valid_int;
  gint int_value = 0;

  int_value = strtol (value, &end_of_valid_int, 10);

  if (*end_of_valid_int != '\0')
    g_set_error (error, G_KEY_FILE_ERROR,
		 G_KEY_FILE_ERROR_INVALID_VALUE,
		 _("Value '%s' cannot be interpreted as a number."), value);

  return int_value;
}

static gchar *
g_key_file_parse_integer_as_value (GKeyFile *key_file,
				   gint      value)

{
  return g_strdup_printf ("%d", value);
}

static gboolean
g_key_file_parse_value_as_boolean (GKeyFile     *key_file,
				   const gchar  *value,
				   GError      **error)
{
  if (value)
    {
      if (strcmp (value, "true") == 0 || strcmp (value, "1") == 0)
        return TRUE;
      else if (strcmp (value, "false") == 0 || strcmp (value, "0") == 0)
        return FALSE;
    }

  g_set_error (error, G_KEY_FILE_ERROR,
               G_KEY_FILE_ERROR_INVALID_VALUE,
               _("Value '%s' cannot be interpreted as a boolean."), value);

  return FALSE;
}

static gchar *
g_key_file_parse_boolean_as_value (GKeyFile *key_file,
				   gboolean  value)
{
  if (value)
    return g_strdup ("true");
  else
    return g_strdup ("false");
}

static gchar *
g_key_file_parse_value_as_comment (GKeyFile    *key_file,
                                   const gchar *value)
{
  GString *string;
  gchar **lines, *comment;
  gsize i;

  string = g_string_sized_new (512);

  lines = g_strsplit (value, "\n", 0);

  for (i = 0; lines[i] != NULL; i++)
    {
        if (lines[i][0] != '#')
           g_string_append_printf (string, "%s\n", lines[i]);
        else
           g_string_append_printf (string, "%s\n", lines[i] + 1);
    }
  g_strfreev (lines);

  comment = string->str;

  g_string_free (string, FALSE);

  return comment;
}

static gchar *
g_key_file_parse_comment_as_value (GKeyFile      *key_file,
                                   const gchar   *comment)
{
  GString *string;
  gchar **lines, *value;
  gsize i;

  string = g_string_sized_new (512);

  lines = g_strsplit (comment, "\n", 0);

  for (i = 0; lines[i] != NULL; i++)
    g_string_append_printf (string, "#%s%s", lines[i],
                            lines[i + 1] == NULL? "" : "\n");
  g_strfreev (lines);

  value = string->str;

  g_string_free (string, FALSE);

  return value;
}

// GKeyFile end

#endif

