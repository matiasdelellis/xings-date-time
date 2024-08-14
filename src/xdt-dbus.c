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

#include <gio/gio.h>

#include <xdt-dbus.h>

gboolean
xdt_get_ntp (gboolean *ntp, GError **error)
{
	GDBusProxy *proxy = NULL;
	GVariant *retvar, *ntpvar;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.freedesktop.timedate1",
	                                       "/org/freedesktop/timedate1",
	                                       "org.freedesktop.DBus.Properties",
	                                       NULL,
	                                       error);

	if (proxy == NULL)
		return FALSE;

	retvar = g_dbus_proxy_call_sync (proxy,
	                                 "Get",
	                                  g_variant_new ("(ss)",
	                                 "org.freedesktop.timedate1",
	                                 "NTP"),
	                                  G_DBUS_CALL_FLAGS_NONE,
	                                 -1,
	                                 NULL,
	                                 error);

	g_object_unref (proxy);

	if (retvar == NULL)
		return FALSE;

	g_variant_get (retvar, "(v)", &ntpvar);
	g_variant_unref (retvar);

	*ntp = g_variant_get_boolean (ntpvar);
	g_variant_unref (ntpvar);

	return TRUE;
}

gboolean
xdt_set_ntp (gboolean use_ntp, GError **error)
{
	GDBusProxy *proxy = NULL;
	GVariant *retvar;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.freedesktop.timedate1",
	                                       "/org/freedesktop/timedate1",
	                                       "org.freedesktop.timedate1",
	                                       NULL,
	                                       error);

	if (proxy == NULL)
		return FALSE;

	retvar = g_dbus_proxy_call_sync (proxy,
	                                 "SetNTP",
	                                 g_variant_new ("(bb)", use_ntp, TRUE),
	                                 G_DBUS_CALL_FLAGS_NONE,
	                                 -1,
	                                 NULL,
	                                 error);

	g_object_unref (proxy);

	if (retvar == NULL)
		return FALSE;

	g_variant_unref (retvar);

	return TRUE;
}

gboolean
xdt_get_timezone (gchar **timezone, GError **error)
{
	GDBusProxy *proxy = NULL;
	GVariant *retvar, *timezonevar;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.freedesktop.timedate1",
	                                       "/org/freedesktop/timedate1",
	                                       "org.freedesktop.DBus.Properties",
	                                       NULL,
	                                       error);

	if (proxy == NULL)
		return FALSE;

	retvar = g_dbus_proxy_call_sync (proxy,
	                                 "Get",
	                                 g_variant_new ("(ss)",
	                                 "org.freedesktop.timedate1",
	                                 "Timezone"),
	                                 G_DBUS_CALL_FLAGS_NONE,
	                                 -1,
	                                 NULL,
	                                 error);

	if (retvar == NULL)
		return FALSE;

	g_variant_get (retvar, "(v)", &timezonevar);
	g_variant_unref (retvar);

	*timezone = g_strdup(g_variant_get_string (timezonevar, 0));
	g_variant_unref (timezonevar);

	return TRUE;
}

gboolean
xdt_set_timezone (const gchar *timezone, GError **error)
{
	GDBusProxy *proxy = NULL;
	GVariant *retvar;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.freedesktop.timedate1",
	                                       "/org/freedesktop/timedate1",
	                                       "org.freedesktop.timedate1",
	                                       NULL,
	                                       error);

	if (proxy == NULL)
		return FALSE;

	retvar = g_dbus_proxy_call_sync (proxy,
	                                 "SetTimezone",
	                                 g_variant_new ("(sb)", timezone, TRUE),
	                                 G_DBUS_CALL_FLAGS_NONE,
	                                 -1,
	                                 NULL,
	                                 error);

	g_object_unref (proxy);

	if (retvar == NULL)
		return FALSE;

	g_variant_unref (retvar);

	return TRUE;
}

gboolean
xdt_list_timezones (GVariant **timezones, GError **error)
{
	GDBusProxy *proxy = NULL;
	GVariant *retvar;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.freedesktop.timedate1",
	                                       "/org/freedesktop/timedate1",
	                                       "org.freedesktop.timedate1",
	                                       NULL,
	                                       error);

	if (proxy == NULL)
		return FALSE;

	retvar = g_dbus_proxy_call_sync (proxy,
	                                 "ListTimezones",
	                                 NULL,
	                                 G_DBUS_CALL_FLAGS_NONE,
	                                 -1,
	                                 NULL,
	                                 error);


	if (retvar == NULL)
		return FALSE;

	*timezones = retvar;

	return TRUE;
}

gboolean
xdt_set_time (GDateTime *date_time, GError **error)
{
	GDBusProxy *proxy = NULL;
	GVariant *retvar;
	gint64 newtime;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
	                                       G_DBUS_PROXY_FLAGS_NONE,
	                                       NULL,
	                                       "org.freedesktop.timedate1",
	                                       "/org/freedesktop/timedate1",
	                                       "org.freedesktop.timedate1",
	                                       NULL,
	                                       error);

	if (proxy == NULL)
		return FALSE;

	newtime = g_date_time_to_unix(date_time);

	retvar = g_dbus_proxy_call_sync (proxy,
	                                 "SetTime",
	                                 g_variant_new ("(xbb)", (newtime * G_TIME_SPAN_SECOND), FALSE, TRUE),
	                                 G_DBUS_CALL_FLAGS_NONE,
	                                 -1,
	                                 NULL,
	                                 error);

	g_object_unref (proxy);

	if (retvar == NULL)
		return FALSE;

	g_variant_unref (retvar);

	return TRUE;
}

