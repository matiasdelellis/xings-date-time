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
	gtk_window_close (GTK_WINDOW (parent));
}

static void
xdt_timezone_set_cb (GObject      *source_object,
                     GAsyncResult *res,
                     gpointer      user_data)
{
	GtkBuilder *builder;
	GtkWidget *widget, *parent;
	GError *error = NULL;

	builder = (GtkBuilder *) xdt_weak_op_take (user_data);
	if (builder == NULL)
		return;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_apply"));
	parent = gtk_widget_get_toplevel (widget);

	if (!xdt_set_timezone_finish (res, &error)) {
		gchar *message;

		widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_cancel"));
		gtk_widget_set_sensitive (widget, TRUE);
		widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_apply"));
		gtk_widget_set_sensitive (widget, TRUE);

		message = g_strdup_printf (_("Failed to set timezone: %s"), error->message);
		g_critical ("%s", message);
		xdt_show_error_dialog (GTK_IS_WINDOW (parent) ? GTK_WINDOW (parent) : NULL,
		                       message);
		g_free (message);
		g_error_free (error);
		g_object_unref (builder);
		return;
	}

	gtk_window_close (GTK_WINDOW (parent));
	g_object_unref (builder);
}

static void
xdt_timezone_dialog_apply_activated_cb (GtkButton  *button,
                                        GtkBuilder *builder)
{
	GtkWidget *widget;
	const gchar *timezone = NULL;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	timezone = gtk_label_get_text (GTK_LABEL (widget));

	/* Block the buttons until the async call completes */
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_cancel"));
	gtk_widget_set_sensitive (widget, FALSE);
	widget = GTK_WIDGET (gtk_builder_get_object (builder, "button_apply"));
	gtk_widget_set_sensitive (widget, FALSE);

	xdt_set_timezone_async (timezone, NULL,
	                        xdt_timezone_set_cb,
	                        xdt_weak_op_new (builder));
}

static void
xdt_timezone_row_selected (GtkListBox    *listbox,
                           GtkListBoxRow *row,
                           GtkBuilder *builder)
{
	GtkWidget *widget = NULL;
	const gchar *timezone;

	/* row is NULL when the selection is cleared */
	if (row == NULL)
		return;

	timezone = g_object_get_data (G_OBJECT (row), "TIMEZONE");
	if (timezone == NULL)
		return;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	gtk_label_set_text (GTK_LABEL (widget), timezone);
}

/* Normalize a string for loose matching: casefolded with _, - and /
 * treated as spaces, so "Buenos Aires" matches "America/Argentina/Buenos_Aires". */
static gchar *
xdt_timezone_normalize (const gchar *str)
{
	gchar *normalized;

	normalized = g_utf8_casefold (str, -1);
	g_strdelimit (normalized, "_-/", ' ');

	return normalized;
}

static gboolean
xdt_timezone_list_filter (GtkListBoxRow *row,
                          GtkBuilder *builder)
{
	GtkWidget *widget;
	const gchar *timezone;
	const gchar *search_text;
	gchar *search_text_lower, *timezone_lower;
	gboolean match;

	timezone = g_object_get_data (G_OBJECT (row), "TIMEZONE");
	if (timezone == NULL)
		return FALSE;

	/* The loading indicator has empty data and always stays visible */
	if (*timezone == '\0')
		return TRUE;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_filter"));
	search_text = gtk_entry_get_text (GTK_ENTRY (widget));
	if (search_text == NULL || *search_text == '\0')
		return TRUE;

	timezone_lower = xdt_timezone_normalize (timezone);
	search_text_lower = xdt_timezone_normalize (search_text);

	match = (g_strstr_len (timezone_lower, -1, search_text_lower) != NULL);

	g_free (search_text_lower);
	g_free (timezone_lower);

	return match;
}
void
xdt_timezone_list_filter_changed (GtkSearchEntry *self, GtkBuilder *builder)
{
	GtkWidget *widget, *ancestor;
	GtkAdjustment *adjustment;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_list"));
	gtk_list_box_invalidate_filter (GTK_LIST_BOX (widget));

	/* Back to the top so the first matches are visible */
	ancestor = gtk_widget_get_ancestor (widget, GTK_TYPE_SCROLLED_WINDOW);
	if (ancestor != NULL) {
		adjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (ancestor));
		gtk_adjustment_set_value (adjustment,
		                          gtk_adjustment_get_lower (adjustment));
	}
}

static GtkWidget *
xdt_timezone_row_new (const gchar *timezone)
{
	GtkWidget *row = gtk_list_box_row_new();
	g_object_set_data_full (G_OBJECT (row), "TIMEZONE", g_strdup (timezone), g_free);
	GtkWidget *label = gtk_label_new(timezone);
	gtk_container_add(GTK_CONTAINER(row), label);
	/* Rows may be inserted long after the dialog was shown */
	gtk_widget_show_all (row);
	return row;
}

static GtkWidget *
xdt_timezone_loading_row_new (void)
{
	GtkWidget *row, *box, *spinner, *label;

	row = gtk_list_box_row_new ();
	gtk_list_box_row_set_selectable (GTK_LIST_BOX_ROW (row), FALSE);
	gtk_list_box_row_set_activatable (GTK_LIST_BOX_ROW (row), FALSE);
	/* Empty data keeps it visible while the filter is active */
	g_object_set_data (G_OBJECT (row), "TIMEZONE", "");

	box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
	spinner = gtk_spinner_new ();
	gtk_spinner_start (GTK_SPINNER (spinner));
	label = gtk_label_new (_("Loading time zones…"));
	gtk_container_add (GTK_CONTAINER (box), spinner);
	gtk_container_add (GTK_CONTAINER (box), label);
	gtk_container_add (GTK_CONTAINER (row), box);
	gtk_widget_show_all (row);

	return row;
}

/* Load operation context: a weak reference to the builder (NULL-safe when
 * the dialog is closed mid-load) plus the loading row, which is alive as
 * long as the dialog is. */
typedef struct {
	GWeakRef   builder_ref;
	GtkWidget *loading_row;
} XdtTimezoneLoadOp;

static void
xdt_timezone_list_loaded_cb (GObject      *source_object,
                             GAsyncResult *res,
                             gpointer      user_data)
{
	XdtTimezoneLoadOp *op = user_data;
	GtkBuilder *builder;
	GtkWidget *widget, *row;
	GVariant *timezones;
	GVariantIter *iter;
	gchar *label = NULL;
	const gchar *current_timezone;
	GError *error = NULL;

	builder = (GtkBuilder *) g_weak_ref_get (&op->builder_ref);
	row = op->loading_row;
	g_weak_ref_clear (&op->builder_ref);
	g_free (op);

	if (builder == NULL)
		return;

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_list"));
	gtk_widget_destroy (row);

	if (!xdt_list_timezones_finish (res, &timezones, &error)) {
		GtkWidget *parent;
		gchar *message;

		message = g_strdup_printf (_("Failed to list time zones: %s"), error->message);
		g_warning ("%s", message);
		parent = gtk_widget_get_toplevel (widget);
		xdt_show_error_dialog (GTK_IS_WINDOW (parent) ? GTK_WINDOW (parent) : NULL,
		                       message);
		g_free (message);
		g_error_free (error);
		g_object_unref (builder);
		return;
	}

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	current_timezone = gtk_label_get_text (GTK_LABEL (widget));

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_list"));

	g_variant_get (timezones, "(as)", &iter);
	while (g_variant_iter_loop (iter, "s", &label)) {
		row = xdt_timezone_row_new (label);
		gtk_list_box_insert (GTK_LIST_BOX (widget), row, -1);

		if (g_strcmp0 (label, current_timezone) == 0)
			gtk_list_box_select_row (GTK_LIST_BOX (widget), GTK_LIST_BOX_ROW (row));
	}
	g_variant_iter_free (iter);
	g_variant_unref (timezones);

	g_object_unref (builder);
}

GtkWidget *
xdt_timezone_dialog_new (const gchar *timezone, GtkWindow *parent)
{
	GtkWidget *widget, *row;
	GtkBuilder *builder;
	guint retval;
	GError *error = NULL;
	XdtTimezoneLoadOp *op;

	builder = gtk_builder_new ();
	retval = gtk_builder_add_from_file (builder, PKGDATADIR "/xdt-timezone-dialog.ui", &error);
	if (retval == 0) {
		g_warning ("Failed to load ui: %s", error->message);
		g_error_free (error);
		g_object_unref (builder);
		return NULL;
	}

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "label_timezone"));
	gtk_label_set_text (GTK_LABEL (widget), timezone);

	widget = GTK_WIDGET (gtk_builder_get_object (builder, "timezone_list"));

	g_signal_connect (widget, "row-selected",
	                  G_CALLBACK (xdt_timezone_row_selected),
	                  builder);
	gtk_list_box_set_filter_func (GTK_LIST_BOX(widget),
	                              (GtkListBoxFilterFunc) xdt_timezone_list_filter,
	                              builder, NULL);

	/* Feedback shown when no row matches the search */
	{
		GtkWidget *placeholder = gtk_label_new (_("No matching time zones"));
		gtk_widget_set_margin_top (placeholder, 12);
		gtk_widget_set_margin_bottom (placeholder, 12);
		gtk_widget_show (placeholder);
		gtk_list_box_set_placeholder (GTK_LIST_BOX (widget), placeholder);
	}

	/* Placeholder shown while the list loads asynchronously */
	row = xdt_timezone_loading_row_new ();
	gtk_list_box_insert (GTK_LIST_BOX (widget), row, -1);

	op = g_new0 (XdtTimezoneLoadOp, 1);
	g_weak_ref_init (&op->builder_ref, builder);
	op->loading_row = row;
	xdt_list_timezones_async (NULL,
	                          xdt_timezone_list_loaded_cb,
	                          op);

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

	/* The dialog owns the builder: this keeps it alive for the
	 * signal handlers above and releases it when the dialog
	 * is destroyed, however it is closed. */
	g_object_set_data_full (G_OBJECT (widget), "xdt-builder",
	                        builder, g_object_unref);

	return widget;
}
