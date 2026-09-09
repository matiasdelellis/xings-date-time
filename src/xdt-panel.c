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

#include "xdt-panel.h"
#include "xdt-dbus.h"
#include "xdt-common.h"
#include "xdt-datetime-dialog.h"
#include "xdt-timezone-dialog.h"


struct _XdtPanel
{
	GtkBox		_parent;

	GtkBuilder	*builder;

	guint            label_timeout_id;

	GDBusConnection	*connection;
	guint            properties_subscription_id;
};

G_DEFINE_TYPE (XdtPanel, xdt_panel, GTK_TYPE_BOX)

static gboolean
xdt_panel_time_auto_set (GtkSwitch *widget,
                         gboolean   state,
                         XdtPanel  *panel);

/* Async operation context: a weak reference to the panel plus the state
 * the user requested, so completion callbacks can bail out safely when
 * the panel is gone and revert the UI otherwise. */
typedef struct {
	GWeakRef  panel_ref;
	gboolean  requested_state;
} XdtPanelOp;

static XdtPanelOp *
xdt_panel_op_new (XdtPanel *panel,
                  gboolean  requested_state)
{
	XdtPanelOp *op;

	op = g_new0 (XdtPanelOp, 1);
	g_weak_ref_init (&op->panel_ref, panel);
	op->requested_state = requested_state;

	return op;
}

/* Returns the panel (transfer full) or NULL when it is already destroyed.
 * Either way the op is consumed. */
static XdtPanel *
xdt_panel_op_take_panel (XdtPanelOp *op,
                         gboolean   *requested_state)
{
	XdtPanel *panel;

	g_return_val_if_fail (op != NULL, NULL);

	if (requested_state != NULL)
		*requested_state = op->requested_state;

	panel = XDT_PANEL (g_weak_ref_get (&op->panel_ref));
	g_weak_ref_clear (&op->panel_ref);
	g_free (op);

	return panel;
}

static void
xdt_panel_set_manual_visible (XdtPanel *panel,
                              gboolean  visible)
{
	GtkWidget *widget;

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "revealer_manual_label"));
	gtk_revealer_set_reveal_child (GTK_REVEALER (widget), visible);

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "revealer_manual"));
	gtk_revealer_set_reveal_child (GTK_REVEALER (widget), visible);
}

/* Error infobar: only shown for real errors, hidden otherwise.
 * Widgets are looked up on use, like every other builder object. */
static GtkWidget *
xdt_panel_get_infobar (XdtPanel *panel)
{
	return GTK_WIDGET (gtk_builder_get_object (panel->builder, "infobar"));
}

static void
xdt_panel_show_error (XdtPanel    *panel,
                      const gchar *message)
{
	GtkWidget *infobar, *label;

	infobar = xdt_panel_get_infobar (panel);
	label = GTK_WIDGET (g_object_get_data (G_OBJECT (panel->builder), "xdt-error-label"));
	gtk_label_set_text (GTK_LABEL (label), message);
	gtk_widget_show (infobar);
}

static void
xdt_panel_hide_error (XdtPanel *panel)
{
	gtk_widget_hide (xdt_panel_get_infobar (panel));
}

static void
xdt_panel_ntp_loaded_cb (GObject      *source_object,
                         GAsyncResult *res,
                         gpointer      user_data)
{
	XdtPanel *panel;
	GtkWidget *widget;
	gboolean enabled = FALSE;
	GError *error = NULL;

	panel = xdt_panel_op_take_panel (user_data, NULL);
	if (panel == NULL)
		return;

	if (!xdt_get_ntp_finish (res, &enabled, &error)) {
		gchar *message;

		message = g_strdup_printf (_("Failed to get automatic time state: %s"), error->message);
		g_warning ("%s", message);
		xdt_panel_show_error (panel, message);
		g_free (message);
		g_error_free (error);
		enabled = FALSE;
	}

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "switch_time_auto"));
	g_signal_handlers_block_by_func (widget, xdt_panel_time_auto_set, panel);
	gtk_switch_set_active (GTK_SWITCH (widget), enabled);
	g_signal_handlers_unblock_by_func (widget, xdt_panel_time_auto_set, panel);

	xdt_panel_set_manual_visible (panel, !enabled);

	g_object_unref (panel);
}

static void
xdt_panel_timezone_loaded_cb (GObject      *source_object,
                              GAsyncResult *res,
                              gpointer      user_data)
{
	XdtPanel *panel;
	GtkWidget *widget;
	gchar *timezone = NULL;
	GError *error = NULL;

	panel = xdt_panel_op_take_panel (user_data, NULL);
	if (panel == NULL)
		return;

	if (!xdt_get_timezone_finish (res, &timezone, &error)) {
		gchar *message;

		message = g_strdup_printf (_("Failed to get timezone: %s"), error->message);
		g_warning ("%s", message);
		xdt_panel_show_error (panel, message);
		g_free (message);
		g_error_free (error);
		timezone = g_strdup (_("Unknown"));
	}

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "button_timezone"));
	gtk_button_set_label (GTK_BUTTON (widget), timezone);
	g_free (timezone);

	g_object_unref (panel);
}

static void
xdt_panel_ntp_set_cb (GObject      *source_object,
                      GAsyncResult *res,
                      gpointer      user_data)
{
	XdtPanel *panel;
	GtkWidget *widget;
	GError *error = NULL;
	gboolean requested_state;

	panel = xdt_panel_op_take_panel (user_data, &requested_state);
	if (panel == NULL)
		return;

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "switch_time_auto"));
	gtk_widget_set_sensitive (widget, TRUE);

	if (!xdt_set_ntp_finish (res, &error)) {
		gchar *message;

		/* Revert to the previous state */
		g_signal_handlers_block_by_func (widget, xdt_panel_time_auto_set, panel);
		gtk_switch_set_active (GTK_SWITCH (widget), !requested_state);
		g_signal_handlers_unblock_by_func (widget, xdt_panel_time_auto_set, panel);
		xdt_panel_set_manual_visible (panel, requested_state);

		message = g_strdup_printf (_("Failed to set automatic time: %s"), error->message);
		g_warning ("%s", message);
		xdt_panel_show_error (panel, message);
		g_free (message);
		g_error_free (error);
	} else {
		xdt_panel_hide_error (panel);
	}

	g_object_unref (panel);
}

static gboolean
xdt_panel_time_auto_set (GtkSwitch *widget,
                         gboolean   state,
                         XdtPanel  *panel)
{
	/* Show the requested state right away and wait for the async
	 * call. The switch is insensitive meanwhile to avoid re-entrancy. */
	gtk_switch_set_state (widget, state);
	xdt_panel_set_manual_visible (panel, !state);
	gtk_widget_set_sensitive (GTK_WIDGET (widget), FALSE);

	xdt_set_ntp_async (state, NULL,
	                   xdt_panel_ntp_set_cb,
	                   xdt_panel_op_new (panel, state));

	return TRUE;
}

static void
xdt_panel_manual_activated_cb (GtkButton *button,
                               XdtPanel  *panel)
{
	GtkWindow *parent = NULL;
	GDateTime *date_time = NULL;

	date_time = g_date_time_new_now_local();
	parent = GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET(button)));

	xdt_date_time_dialog (date_time, parent);
}

static gboolean
xdt_panel_timezone_closed_cb (GtkWidget *widget,
			      GdkEvent  *event,
			      XdtPanel  *panel)
{
	/* Refresh the label without blocking on the system bus */
	xdt_get_timezone_async (NULL,
	                        xdt_panel_timezone_loaded_cb,
	                        xdt_panel_op_new (panel, FALSE));

	return FALSE;
}

static void
xdt_panel_timezone_activated_cb (GtkButton *button,
                                 XdtPanel  *panel)
{
	GtkWindow *parent = NULL;
	GtkWidget *widget = NULL;
	const gchar *timezone = NULL;

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "button_timezone"));
	timezone = gtk_button_get_label (GTK_BUTTON (widget));

	parent = GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET(button)));

	widget = xdt_timezone_dialog_new (timezone, parent);
	if (widget == NULL) {
		g_warning (_("Failed to open time zone dialog"));
		xdt_panel_show_error (panel, _("Failed to open time zone dialog"));
		return;
	}
	g_signal_connect (G_OBJECT (widget), "delete_event",
	                  G_CALLBACK (xdt_panel_timezone_closed_cb), panel);
	gtk_widget_show_all (widget);
}

static gboolean
xdt_update_time_label (XdtPanel *panel)
{
	GtkWidget *widget = NULL;
	gchar *label = NULL;
	GDateTime *date_time = NULL;

	date_time = g_date_time_new_now_local();
	label = xdt_get_friendly_date_time(date_time);
	g_date_time_unref (date_time);

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "label_current_time"));
	gtk_label_set_text (GTK_LABEL (widget), label);
	g_free(label);

	return G_SOURCE_CONTINUE;
}


static void
xdt_panel_weak_free (GWeakRef *weak)
{
	g_weak_ref_clear (weak);
	g_free (weak);
}

/* Push updates from timedate1: the daemon settles NTP/NTPSynchronized
 * asynchronously after SetNTP, and settings may also change externally
 * (e.g. via timedatectl), so refresh whatever changed instead of
 * trusting a single read. */
static void
xdt_panel_properties_changed_cb (GDBusConnection *connection,
                                 const gchar     *sender_name,
                                 const gchar     *object_path,
                                 const gchar     *interface_name,
                                 const gchar     *signal_name,
                                 GVariant        *parameters,
                                 gpointer         user_data)
{
	GWeakRef *weak = user_data;
	XdtPanel *panel;
	const gchar *iface = NULL;
	GVariant *changed = NULL;
	GVariant *invalid = NULL;
	GVariant *value = NULL;
	gboolean refresh_ntp = FALSE;
	gboolean refresh_timezone = FALSE;

	panel = XDT_PANEL (g_weak_ref_get (weak));
	if (panel == NULL)
		return;

	g_variant_get (parameters, "(&s@a{sv}@as)", &iface, &changed, &invalid);
	(void) iface;

	value = g_variant_lookup_value (changed, "NTP", NULL);
	if (value != NULL) {
		g_variant_unref (value);
		refresh_ntp = TRUE;
	}
	value = g_variant_lookup_value (changed, "Timezone", NULL);
	if (value != NULL) {
		g_variant_unref (value);
		refresh_timezone = TRUE;
	}

	g_variant_unref (changed);
	g_variant_unref (invalid);

	if (refresh_ntp) {
		xdt_get_ntp_async (NULL,
		                   xdt_panel_ntp_loaded_cb,
		                   xdt_panel_op_new (panel, FALSE));
	}
	if (refresh_timezone) {
		xdt_get_timezone_async (NULL,
		                        xdt_panel_timezone_loaded_cb,
		                        xdt_panel_op_new (panel, FALSE));
	}

	g_object_unref (panel);
}

static void
xdt_panel_bus_ready_cb (GObject      *source_object,
                        GAsyncResult *res,
                        gpointer      user_data)
{
	XdtPanel *panel;
	GDBusConnection *connection;
	GError *error = NULL;
	GWeakRef *weak;

	panel = xdt_panel_op_take_panel (user_data, NULL);
	if (panel == NULL)
		return;

	connection = g_bus_get_finish (res, &error);
	if (connection == NULL) {
		gchar *message;

		message = g_strdup_printf (_("Failed to connect to the system bus: %s"), error->message);
		g_warning ("%s", message);
		xdt_panel_show_error (panel, message);
		g_free (message);
		g_error_free (error);
		g_object_unref (panel);
		return;
	}

	/* Weak user data: no reference cycle with the panel */
	weak = g_new0 (GWeakRef, 1);
	g_weak_ref_init (weak, panel);

	panel->connection = connection; /* transfer full */
	panel->properties_subscription_id =
		g_dbus_connection_signal_subscribe (connection,
		                                    "org.freedesktop.timedate1",
		                                    "org.freedesktop.DBus.Properties",
		                                    "PropertiesChanged",
		                                    "/org/freedesktop/timedate1",
		                                    NULL,
		                                    G_DBUS_SIGNAL_FLAGS_NONE,
		                                    xdt_panel_properties_changed_cb,
		                                    weak,
		                                    (GDestroyNotify) xdt_panel_weak_free);

	g_debug ("Subscribed to timedate1 property changes");
	g_object_unref (panel);
}

/**
 * xdt_panel_finalize:
 * @object: The object to finalize
 **/
static void
xdt_panel_finalize (GObject *object)
{
	XdtPanel *panel;

	g_return_if_fail (XDT_IS_PANEL (object));

	panel = XDT_PANEL (object);

	if (panel->label_timeout_id != 0) {
		g_source_remove (panel->label_timeout_id);
		panel->label_timeout_id = 0;
	}

	if (panel->properties_subscription_id != 0 && panel->connection != NULL) {
		g_dbus_connection_signal_unsubscribe (panel->connection,
		                                      panel->properties_subscription_id);
		panel->properties_subscription_id = 0;
	}
	g_clear_object (&panel->connection);

	g_clear_object (&panel->builder);

	G_OBJECT_CLASS (xdt_panel_parent_class)->finalize (object);
}

/**
 * xdt_panel_init:
 * @preferences_panel: This class instance
 **/
static void
xdt_panel_init (XdtPanel *panel)
{
	GtkWidget *widget, *label, *infobar;

	/* Get builder to construct panel */

	panel->builder = gtk_builder_new_from_file (PKGDATADIR "/xdt-panel.ui");

	gtk_orientable_set_orientation (GTK_ORIENTABLE (panel), GTK_ORIENTATION_VERTICAL);

	/* Main widget */

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "xdt-panel"));
	if (widget == NULL) {
		g_critical ("Failed to load panel UI");
		return;
	}

	/* Infobar first, at full width; content below */
	infobar = GTK_WIDGET (gtk_builder_get_object (panel->builder, "infobar"));
	g_object_ref (infobar);
	gtk_container_remove (GTK_CONTAINER (widget), infobar);
	gtk_box_pack_start (GTK_BOX (panel), infobar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (panel), GTK_WIDGET (widget), TRUE, TRUE, 0);
	g_object_unref (infobar);

	/* Automatic time switch. The actual state arrives asynchronously below. */

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "switch_time_auto"));
	g_signal_connect (widget, "state-set",
	                  G_CALLBACK (xdt_panel_time_auto_set), panel);

	/* Manual date time button */

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "button_manual"));
	g_signal_connect (widget, "clicked",
	                  G_CALLBACK (xdt_panel_manual_activated_cb), panel);

	/* Timezone button. The label arrives asynchronously below. */

	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "button_timezone"));
	g_signal_connect (widget, "clicked",
	                  G_CALLBACK (xdt_panel_timezone_activated_cb), panel);

	/* Error infobar, hidden until a real error happens.
	 * Content is built in code: GtkInfoBar internal children
	 * cannot be defined from GtkBuilder. */
	widget = GTK_WIDGET (gtk_builder_get_object (panel->builder, "infobar"));
	gtk_widget_set_no_show_all (widget, TRUE);
	gtk_widget_hide (widget);

	label = gtk_label_new ("");
	gtk_label_set_xalign (GTK_LABEL (label), 0.0);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER (gtk_info_bar_get_content_area (GTK_INFO_BAR (widget))),
	                   label);
	g_object_set_data (G_OBJECT (panel->builder), "xdt-error-label", label);

	gtk_info_bar_add_button (GTK_INFO_BAR (widget), _("_Close"), GTK_RESPONSE_CLOSE);
	g_signal_connect_swapped (widget, "response",
	                          G_CALLBACK (gtk_widget_hide), widget);

	/* Update current datetime */
	xdt_update_time_label (panel);
	panel->label_timeout_id = g_timeout_add_seconds (1, (GSourceFunc)xdt_update_time_label, panel);

	/* Load the system state without blocking the UI */
	xdt_get_ntp_async (NULL,
	                   xdt_panel_ntp_loaded_cb,
	                   xdt_panel_op_new (panel, FALSE));
	xdt_get_timezone_async (NULL,
	                        xdt_panel_timezone_loaded_cb,
	                        xdt_panel_op_new (panel, FALSE));

	/* Stay correct when settings change externally (e.g. via timedatectl) */
	g_bus_get (G_BUS_TYPE_SYSTEM, NULL,
	           xdt_panel_bus_ready_cb,
	           xdt_panel_op_new (panel, FALSE));
}

/**
 * xdt_panel_class_init:
 * @klass: The XdtPanelClass
 **/
static void
xdt_panel_class_init (XdtPanelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = xdt_panel_finalize;
}

/**
 * xdt_panel_new:
 *
 * Return value: a new XdtPanel object.
 **/
XdtPanel *
xdt_panel_new (void)
{
	XdtPanel *panel;
	panel = g_object_new (XDT_TYPE_PANEL, NULL);
	return XDT_PANEL (panel);
}

