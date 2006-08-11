/* GTK12 compatibility header (c) Antti S. Lankila 2005, 2006  <alankila@bel.fi>
 * All Rights reserved. See gtk12-compat.c for more info. Licensed under the GPL. */

#ifndef _GTK12_COMPAT_H_
#define _GTK12_COMPAT_H_

#ifdef HAVE_GTK2
#define GTK_WINDOW_DIALOG GTK_WINDOW_TOPLEVEL
#endif

#include <glib.h>

/* custom functions */
GtkWidget *gnuitar_gtk_text_view_new(GtkContainer *container);
void       gnuitar_gtk_text_view_append(GtkWidget *tw, gchar *text);

#ifndef HAVE_GTK
#include <glib/gstdio.h>
#include <glib/gthread.h>
#else

/* define some GTK 2.x functions away */
#define gtk_widget_set_size_request(a, b, c)
#define gtk_range_set_inverted(a, b)

/* glib/gfileutils.h */
#define G_FILE_ERROR g_file_error_quark ()

#ifndef _WIN32
typedef enum
{
  G_FILE_ERROR_EXIST,
  G_FILE_ERROR_ISDIR,
  G_FILE_ERROR_ACCES,
  G_FILE_ERROR_NAMETOOLONG,
  G_FILE_ERROR_NOENT,
  G_FILE_ERROR_NOTDIR,
  G_FILE_ERROR_NXIO,
  G_FILE_ERROR_NODEV,
  G_FILE_ERROR_ROFS,
  G_FILE_ERROR_TXTBSY,
  G_FILE_ERROR_FAULT,
  G_FILE_ERROR_LOOP,
  G_FILE_ERROR_NOSPC,
  G_FILE_ERROR_NOMEM,
  G_FILE_ERROR_MFILE,
  G_FILE_ERROR_NFILE,
  G_FILE_ERROR_BADF,
  G_FILE_ERROR_INVAL,
  G_FILE_ERROR_PIPE,
  G_FILE_ERROR_AGAIN,
  G_FILE_ERROR_INTR,
  G_FILE_ERROR_IO,
  G_FILE_ERROR_PERM,
  G_FILE_ERROR_NOSYS,
  G_FILE_ERROR_FAILED
} GFileError;
#endif

#define g_open open


/* some macro magic for declaring malloc type from glib-2.0/gmacros.h */
#if    __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
#define G_GNUC_PURE                            \
  __attribute__((__pure__))
#define G_GNUC_MALLOC                           \
  __attribute__((__malloc__))
#else
#define G_GNUC_PURE
#define G_GNUC_MALLOC
#endif

#ifndef _WIN32
/* glib/gstrfuncs.c */
/* Functions like the ones in <ctype.h> that are not affected by locale. */
typedef enum {
  G_ASCII_ALNUM  = 1 << 0,
  G_ASCII_ALPHA  = 1 << 1,
  G_ASCII_CNTRL  = 1 << 2,
  G_ASCII_DIGIT  = 1 << 3,
  G_ASCII_GRAPH  = 1 << 4,
  G_ASCII_LOWER  = 1 << 5,
  G_ASCII_PRINT  = 1 << 6,
  G_ASCII_PUNCT  = 1 << 7,
  G_ASCII_SPACE  = 1 << 8,
  G_ASCII_UPPER  = 1 << 9,
  G_ASCII_XDIGIT = 1 << 10
} GAsciiType;

/* glib/gerror.h */
typedef struct _GError GError;

struct _GError
{
  GQuark       domain;
  gint         code;
  gchar       *message;
};
#endif

GError*  g_error_new           (GQuark         domain,
                                gint           code,
                                const gchar   *format,
                                ...) G_GNUC_PRINTF (3, 4);

GError*  g_error_new_literal   (GQuark         domain,
                                gint           code,
                                const gchar   *message);

void     g_error_free          (GError        *error);
GError*  g_error_copy          (const GError  *error);

gboolean g_error_matches       (const GError  *error,
                                GQuark         domain,
                                gint           code);

/* if (err) *err = g_error_new(domain, code, format, ...), also has
 * some sanity checks.
 */
void     g_set_error           (GError       **err,
                                GQuark         domain,
                                gint           code,
                                const gchar   *format,
                                ...) G_GNUC_PRINTF (4, 5);

/* if (dest) *dest = src; also has some sanity checks.
 */
void     g_propagate_error     (GError       **dest,
				GError        *src);

/* if (err && *err) { g_error_free(*err); *err = NULL; } */
void     g_clear_error         (GError       **err);

typedef enum
{
  G_KEY_FILE_ERROR_UNKNOWN_ENCODING,
  G_KEY_FILE_ERROR_PARSE,
  G_KEY_FILE_ERROR_NOT_FOUND,
  G_KEY_FILE_ERROR_KEY_NOT_FOUND,
  G_KEY_FILE_ERROR_GROUP_NOT_FOUND,
  G_KEY_FILE_ERROR_INVALID_VALUE
} GKeyFileError;

#define G_KEY_FILE_ERROR g_key_file_error_quark()

GQuark g_key_file_error_quark (void);

typedef struct _GKeyFile GKeyFile;

typedef enum
{
  G_KEY_FILE_NONE              = 0,
  G_KEY_FILE_KEEP_COMMENTS     = 1 << 0,
  G_KEY_FILE_KEEP_TRANSLATIONS = 1 << 1
} GKeyFileFlags;

GKeyFile *g_key_file_new                    (void);
void      g_key_file_free                   (GKeyFile             *key_file);
void      g_key_file_set_list_separator     (GKeyFile             *key_file,
					     gchar                 separator);
gboolean  g_key_file_load_from_file         (GKeyFile             *key_file,
					     const gchar          *file,
					     GKeyFileFlags         flags,
					     GError              **error);
gboolean  g_key_file_load_from_data         (GKeyFile             *key_file,
					     const gchar          *data,
					     gsize                 length,
					     GKeyFileFlags         flags,
					     GError              **error);
gchar    *g_key_file_to_data                (GKeyFile             *key_file,
					     gsize                *length,
					     GError              **error) G_GNUC_MALLOC;
gchar    *g_key_file_get_start_group        (GKeyFile             *key_file) G_GNUC_MALLOC;
gchar   **g_key_file_get_groups             (GKeyFile             *key_file,
					     gsize                *length) G_GNUC_MALLOC;
gchar   **g_key_file_get_keys               (GKeyFile             *key_file,
					     const gchar          *group_name,
					     gsize                *length,
					     GError              **error) G_GNUC_MALLOC;
gboolean  g_key_file_has_group              (GKeyFile             *key_file,
					     const gchar          *group_name);
gboolean  g_key_file_has_key                (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     GError              **error);
gchar    *g_key_file_get_value              (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     GError              **error) G_GNUC_MALLOC;
void      g_key_file_set_value              (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     const gchar          *value);
gchar    *g_key_file_get_string             (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     GError              **error) G_GNUC_MALLOC;
void      g_key_file_set_string             (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     const gchar          *string);
gboolean  g_key_file_get_boolean            (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     GError              **error);
void      g_key_file_set_boolean            (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gboolean              value);
gint      g_key_file_get_integer            (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     GError              **error);
void      g_key_file_set_integer            (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gint                  value);
gchar   **g_key_file_get_string_list        (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gsize                *length,
					     GError              **error) G_GNUC_MALLOC;
void      g_key_file_set_string_list        (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     const gchar * const   list[],
					     gsize                 length);
gboolean *g_key_file_get_boolean_list       (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gsize                *length,
					     GError              **error) G_GNUC_MALLOC;
void      g_key_file_set_boolean_list       (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gboolean              list[],
					     gsize                 length);
gint     *g_key_file_get_integer_list       (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gsize                *length,
					     GError              **error) G_GNUC_MALLOC;
void      g_key_file_set_integer_list       (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     gint                  list[],
					     gsize                 length);
void      g_key_file_set_comment            (GKeyFile             *key_file,
                                             const gchar          *group_name,
                                             const gchar          *key,
                                             const gchar          *comment,
                                             GError              **error);
gchar    *g_key_file_get_comment            (GKeyFile             *key_file,
                                             const gchar          *group_name,
                                             const gchar          *key,
                                             GError              **error) G_GNUC_MALLOC;

void      g_key_file_remove_comment         (GKeyFile             *key_file,
                                             const gchar          *group_name,
                                             const gchar          *key,
					     GError              **error);
void      g_key_file_remove_key             (GKeyFile             *key_file,
					     const gchar          *group_name,
					     const gchar          *key,
					     GError              **error);
void      g_key_file_remove_group           (GKeyFile             *key_file,
					     const gchar          *group_name,
					     GError              **error);

#endif /* HAVE_GTK */

#if defined(HAVE_GTK) || (GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION < 10)
gdouble   g_key_file_get_double             (GKeyFile             *,
					     const gchar          *,
					     const gchar          *,
					     GError              **);
void      g_key_file_set_double             (GKeyFile             *,
					     const gchar          *,
					     const gchar          *,
					     gdouble		   );
#endif

#endif

