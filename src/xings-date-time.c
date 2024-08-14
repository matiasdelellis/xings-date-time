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

int
main (int    argc,
      char **argv)
{
	GtkApplication *app;
	GOptionContext *context;
	int status;

	/* Translation */

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, XINGS_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Debug options */

	context = g_option_context_new (NULL);
	g_option_context_set_summary (context, _("Xings Date & Time"));
	g_option_context_add_group (context, xdt_debug_get_option_group ());
	g_option_context_parse (context, &argc, &argv, NULL);
	g_option_context_free (context);

	/* GtkApplication */

	app = gtk_application_new ("org.xings.DateTime", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate",
	                  G_CALLBACK (xings_date_time_application_activate), NULL);

	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
