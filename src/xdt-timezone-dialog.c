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

#include <glib/gi18n.h>

#include "xdt-dbus.h"
#include "xdt-common.h"

#include "xdt-timezone-dialog.h"


static void
xdt_timezone_dialog_cancel_activated_cb (GtkButton  *button,
                                         GtkBuilder *builder)
{
	GtkWidget *parent;
	parent = gtk_widget_get_toplevel (GTK_WIDGET(button));
	gtk_window_close(GTK_WINDOW(parent));

	g_object_unref (builder);
}

static void
xdt_timezone_dialog_apply_activated_cb (GtkButton  *button,
                                        GtkBuilder *builder)
{
	GtkWidget *parent, *widget;
	const gchar *timezone = NULL;
	GError *error = NULL;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	timezone = gtk_label_get_text(GTK_LABEL(widget));

	if (!xdt_set_timezone (timezone, &error)) {
		g_critical (_("Failed to set timezone: %s"), error->message);
		g_error_free(error);
		return;
	}


	parent = gtk_widget_get_toplevel (GTK_WIDGET(button));
	gtk_window_close(GTK_WINDOW(parent));

	g_object_unref (builder);
}

static void
xdt_timezone_row_selected (GtkListBox    *listbox,
                           GtkListBoxRow *row,
                           GtkBuilder *builder)
{
	GtkWidget *widget = NULL;
	const gchar *timezone = g_object_get_data (G_OBJECT (row), "TIMEZONE");
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	gtk_label_set_text(GTK_LABEL(widget), timezone);
}

static gboolean
xdt_timezone_list_filter (GtkListBoxRow *row,
                          GtkBuilder *builder)
{
	GtkWidget *widget;
	gchar *search_text_lower, *timezone_lower;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_filter"));

	timezone_lower = g_ascii_strdown(g_object_get_data (G_OBJECT (row), "TIMEZONE"), -1);
	search_text_lower = g_ascii_strdown(gtk_entry_get_text (GTK_ENTRY (widget)), -1);

	if (g_strrstr(timezone_lower, search_text_lower) != NULL) {
		return TRUE;
	}

	g_free (search_text_lower);
	g_free (timezone_lower);

	return FALSE;
}
void
xdt_timezone_list_filter_changed (GtkSearchEntry *self, GtkBuilder *builder)
{
	GtkWidget *widget;
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_list"));
	gtk_list_box_invalidate_filter (GTK_LIST_BOX(widget));
}

static GtkWidget *
xdt_timezone_row_new (const gchar *timezone)
{
	GtkWidget *row = gtk_list_box_row_new();
	g_object_set_data_full (G_OBJECT (row), "TIMEZONE", g_strdup (timezone), g_free);
	GtkWidget *label = gtk_label_new(timezone);
	gtk_container_add(GTK_CONTAINER(row), label);
	return row;
}

GtkWidget *
xdt_timezone_dialog_new (const gchar *timezone, GtkWindow *parent)
{
	GtkWidget *widget, *row;
	GtkBuilder *builder;
	GtkTextBuffer *buffer = NULL;
	gchar *label = NULL;
	guint retval;
	GError *error = NULL;
	GVariant *timezones;
	GVariantIter *iter;
	const gchar **array = NULL;

	builder = gtk_builder_new ();
	retval = gtk_builder_add_from_file (builder, PKGDATADIR "/xdt-timezone-dialog.ui", &error);
	if (retval == 0) {
		g_warning ("Failed to load ui: %s", error->message);
		g_error_free (error);
		return FALSE;
	}

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	gtk_label_set_text(GTK_LABEL(widget), timezone);

	if (!xdt_list_timezones (&timezones, &error)) {
		g_warning ("Failed to ListTimezones: %s", error->message);
		g_error_free (error);
		return FALSE;
	}

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_list"));

	g_variant_get (timezones, "(as)", &iter);
	while (g_variant_iter_loop (iter, "s", &label)) {
		row = xdt_timezone_row_new (label);
		gtk_list_box_insert(GTK_LIST_BOX(widget), row, -1);

		if (g_strcmp0(label, timezone) == 0)
			gtk_list_box_select_row(GTK_LIST_BOX(widget), GTK_LIST_BOX_ROW(row));
	}
 	g_variant_iter_free (iter);
	g_variant_unref (timezones);

	g_signal_connect (widget, "row-selected",
	                  G_CALLBACK (xdt_timezone_row_selected),
	                  builder);
	gtk_list_box_set_filter_func (GTK_LIST_BOX(widget),
	                              (GtkListBoxFilterFunc) xdt_timezone_list_filter,
	                              builder, NULL);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_filter"));
	g_signal_connect (widget, "search-changed",
	                  G_CALLBACK (xdt_timezone_list_filter_changed), builder);

	/* Main buttons */

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_cancel"));
	g_signal_connect (widget, "clicked",
	                  G_CALLBACK (xdt_timezone_dialog_cancel_activated_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_apply"));
	g_signal_connect (widget, "clicked",
	                  G_CALLBACK (xdt_timezone_dialog_apply_activated_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_dialog"));
	gtk_window_set_transient_for (GTK_WINDOW (widget), parent);
	gtk_window_set_modal (GTK_WINDOW (widget), TRUE);
	gtk_window_set_icon_name (GTK_WINDOW (widget), "time-admin");

	return widget;
}
