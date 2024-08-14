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

#include "xdt-datetime-dialog.h"


GDateTime *
xdt_date_time_new_local_from_dialog (GtkBuilder *builder)
{
	GtkWidget *widget;
	guint year, month, day, hour, min, sec;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "calendar"));
 	gtk_calendar_get_date(GTK_CALENDAR(widget), &year, &month, &day);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "hour_spin"));
	hour = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "minutes_spin"));
	min  = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "seconds_spin"));
	sec = atoi(gtk_entry_get_text(GTK_ENTRY(widget)));

	return g_date_time_new_local (year, month + 1, day, hour, min, sec);
}

static void
xdt_date_time_dialog_update_label (GtkBuilder *builder)
{
	GtkWidget *widget;
	GDateTime *date_time;
	gchar *label = NULL;

	date_time = xdt_date_time_new_local_from_dialog (builder);
	label = xdt_get_frienly_date_time(date_time);
	g_date_time_unref (date_time);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_current_time"));
	gtk_label_set_text (GTK_LABEL (widget), label);
	g_free(label);
}

static void
xdt_date_time_dialog_value_changed_cb (GtkWidget *widget, GtkBuilder *builder)
{
	xdt_date_time_dialog_update_label (builder);
}


static void
xdt_date_time_dialog_cancel_activated_cb (GtkButton *button,
                                          GtkWindow *dialog)
{
	GtkWidget *parent;
	parent = gtk_widget_get_toplevel (GTK_WIDGET(button));
	gtk_widget_destroy(GTK_WIDGET(parent));
}

static void
xdt_date_time_dialog_apply_activated_cb (GtkButton  *button,
                                         GtkBuilder *builder)
{
	GtkWidget *parent;
	GDateTime *date_time;
	GError *error = NULL;

	date_time = xdt_date_time_new_local_from_dialog (builder);
	if (!xdt_set_time (date_time, &error)) {
		g_critical (_("Failed to set time: %s"), error->message);
		g_error_free(error);
		return;
	}

	parent = gtk_widget_get_toplevel (GTK_WIDGET(button));
	gtk_widget_destroy(GTK_WIDGET(parent));
}


gboolean
xdt_date_time_dialog (GDateTime *date_time, GtkWindow *parent)
{
	GtkWidget *widget;
	GtkBuilder *builder;
	GtkTextBuffer *buffer = NULL;
	gchar *label = NULL;
	guint retval;
	GError *error = NULL;

	builder = gtk_builder_new ();
	retval = gtk_builder_add_from_file (builder, PKGDATADIR "/xdt-datetime-dialog.ui", &error);
	if (retval == 0) {
		g_warning ("Failed to load ui: %s", error->message);
		g_error_free (error);
		goto out_build;
	}

	/* Set initial date & time */

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "calendar"));
	gtk_calendar_select_month (GTK_CALENDAR (widget),
	                           g_date_time_get_month (date_time) - 1,
	                           g_date_time_get_year (date_time));
	gtk_calendar_select_day (GTK_CALENDAR (widget),
	                         g_date_time_get_day_of_month (date_time));
	g_signal_connect (widget, "month-changed",
	                  G_CALLBACK (xdt_date_time_dialog_value_changed_cb), builder);
	g_signal_connect (widget, "day-selected",
	                  G_CALLBACK (xdt_date_time_dialog_value_changed_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "hour_spin"));
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), g_date_time_get_hour(date_time));
	g_signal_connect (widget, "value-changed",
	                  G_CALLBACK (xdt_date_time_dialog_value_changed_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "minutes_spin"));
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), g_date_time_get_minute(date_time));
	g_signal_connect (widget, "value_changed",
	                  G_CALLBACK (xdt_date_time_dialog_value_changed_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "seconds_spin"));
	gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), g_date_time_get_second(date_time));
	g_signal_connect (widget, "value_changed",
	                  G_CALLBACK (xdt_date_time_dialog_value_changed_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_current_time"));
	label = xdt_get_frienly_date_time(date_time);
	gtk_label_set_text (GTK_LABEL (widget), label);
	g_free(label);

	/* Main buttons */

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_cancel"));
	g_signal_connect (widget, "clicked",
	                  G_CALLBACK (xdt_date_time_dialog_cancel_activated_cb), widget);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_apply"));
	g_signal_connect (widget, "clicked",
	                  G_CALLBACK (xdt_date_time_dialog_apply_activated_cb), builder);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "date_time_dialog"));
	gtk_window_set_transient_for (GTK_WINDOW (widget), parent);
	gtk_window_set_modal (GTK_WINDOW (widget), TRUE);
	gtk_window_set_icon_name (GTK_WINDOW (widget), "time-admin");

	gtk_widget_show_all (widget);

out_build:
	//g_object_unref (builder);

	return TRUE;
}
