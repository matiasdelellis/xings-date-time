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


#ifndef __XDT_DBUS_H__
#define __XDT_DBUS_H__

#include <gio/gio.h>

gboolean
xdt_get_ntp (gboolean *ntp, GError **error);

gboolean
xdt_set_ntp (gboolean use_ntp, GError **error);

gboolean
xdt_get_timezone (gchar **timezone, GError **error);

gboolean
xdt_set_timezone (const gchar *timezone, GError **error);

gboolean
xdt_set_time (GDateTime *date_time, GError **error);

gboolean
xdt_list_timezones (GVariant **timezones, GError **error);

#endif /* __XDT_DBUS_H__ */
