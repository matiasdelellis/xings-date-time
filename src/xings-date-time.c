/*************************************************************************/
/* Copyright (C) 2024 matias <mati86dl@gmail.com>                        */
/*                                                                       */
/* This program is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*************************************************************************/


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <locale.h>

#include "xdt-debug.h"

#include "xdt-panel.h"


static void
xings_date_time_application_activate (GtkApplication *application,
                                        gpointer        user_data)
{
	XdtPanel *panel;
	GtkWidget *window;

	g_assert (GTK_IS_APPLICATION (application));

	window = GTK_WIDGET (gtk_application_get_active_window (GTK_APPLICATION (application)));
	if (window == NULL) {
		window = gtk_application_window_new (application);
		gtk_window_set_title (GTK_WINDOW (window), _("Date & Time"));
		gtk_window_set_icon_name (GTK_WINDOW (window), "time-admin");
		gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);

		panel = xdt_panel_new ();
		gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (panel));

		gtk_widget_show_all (GTK_WIDGET (window));
	}

	gtk_window_present (GTK_WINDOW (window));
}

static gint
xings_date_time_handle_local_options (GApplication *application,
                                      GVariantDict *options,
                                      gpointer      user_data)
{
	gboolean verbose = FALSE;

	g_variant_dict_lookup (options, "verbose", "b", &verbose);
	xdt_debug_set_verbose (verbose);

	/* -1 lets the application continue with activate */
	return -1;
}

int
main (int    argc,
      char **argv)
{
	GtkApplication *app;
	int status;
	const GOptionEntry option_entries[] = {
		{ "verbose", 'v', 0, G_OPTION_ARG_NONE, NULL,
		  N_("Show debugging information for all files"), NULL },
		{ NULL }
	};

	/* Translation */

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, XINGS_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* GtkApplication */

	app = gtk_application_new ("org.xings.DateTime",
#if GLIB_CHECK_VERSION (2, 74, 0)
	                             G_APPLICATION_DEFAULT_FLAGS);
#else
	                             G_APPLICATION_FLAGS_NONE);
#endif
	g_application_add_main_option_entries (G_APPLICATION (app), option_entries);
	g_signal_connect (app, "handle-local-options",
	                  G_CALLBACK (xings_date_time_handle_local_options), NULL);
	g_signal_connect (app, "activate",
	                  G_CALLBACK (xings_date_time_application_activate), NULL);

	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
