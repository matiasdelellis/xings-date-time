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

#ifndef __XDT_COMMON_H
#define __XDT_COMMON_H

#include <gio/gio.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

gchar *
xdt_get_friendly_date_time (GDateTime *date_time);

void
xdt_show_error_dialog (GtkWindow   *parent,
                       const gchar *message);

/* Opaque async user data holding a weak reference.
 *
 * Async D-Bus callbacks may fire after their UI is gone (e.g. the user
 * closed a dialog mid-operation). Take the object with xdt_weak_op_take():
 * it returns NULL (transfer nothing) when the object is already destroyed,
 * so callbacks can bail out without touching dead widgets.
 */
typedef struct _XdtWeakOp XdtWeakOp;

XdtWeakOp *
xdt_weak_op_new (gpointer object);

gpointer
xdt_weak_op_take (XdtWeakOp *op);

G_END_DECLS

#endif /* __XDT_COMMON_H */
