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
#include <gtk/gtk.h>

#include "xdt-common.h"

gchar *
xdt_get_friendly_date_time (GDateTime *date_time)
{
	g_return_val_if_fail (date_time != NULL, NULL);
	return g_date_time_format (date_time, _("%k:%M:%S, %A, %e of %B of %Y"));
}

void
xdt_show_error_dialog (GtkWindow   *parent,
                       const gchar *message)
{
	GtkWidget *dialog;

	g_return_if_fail (message != NULL);

	dialog = gtk_message_dialog_new (parent,
	                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                 GTK_MESSAGE_ERROR,
	                                 GTK_BUTTONS_CLOSE,
	                                 "%s", message);
	gtk_window_set_icon_name (GTK_WINDOW (dialog), "time-admin");
	g_signal_connect_swapped (dialog, "response",
	                          G_CALLBACK (gtk_widget_destroy), dialog);
	gtk_widget_show_all (dialog);
}

struct _XdtWeakOp
{
	GWeakRef ref;
};

XdtWeakOp *
xdt_weak_op_new (gpointer object)
{
	XdtWeakOp *op;

	g_return_val_if_fail (G_IS_OBJECT (object), NULL);

	op = g_new0 (XdtWeakOp, 1);
	g_weak_ref_init (&op->ref, object);

	return op;
}

gpointer
xdt_weak_op_take (XdtWeakOp *op)
{
	gpointer object;

	g_return_val_if_fail (op != NULL, NULL);

	object = g_weak_ref_get (&op->ref);
	g_weak_ref_clear (&op->ref);
	g_free (op);

	return object;
}
