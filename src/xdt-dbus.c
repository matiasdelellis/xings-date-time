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

	g_object_unref (proxy);

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

	g_object_unref (proxy);

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

/* ---------------------------------------------------------------------------
 * Asynchronous variants
 *
 * Each wrapper runs the synchronous helper in a worker thread via GTask.
 * The sync helpers only use thread-safe calls and per-call state, so this
 * is safe. Callbacks are invoked on the thread-default main context.
 * ------------------------------------------------------------------------- */

static void
xdt_get_ntp_thread (GTask        *task,
                    gpointer      source_object,
                    gpointer      task_data,
                    GCancellable *cancellable)
{
	gboolean ntp = FALSE;
	GError *error = NULL;

	if (g_cancellable_set_error_if_cancelled (cancellable, &error)) {
		g_task_return_error (task, error);
		return;
	}

	if (!xdt_get_ntp (&ntp, &error)) {
		g_task_return_error (task, error);
		return;
	}

	g_task_return_int (task, ntp);
}

void
xdt_get_ntp_async (GCancellable        *cancellable,
                   GAsyncReadyCallback  callback,
                   gpointer             user_data)
{
	GTask *task;

	task = g_task_new (NULL, cancellable, callback, user_data);
	g_task_set_source_tag (task, xdt_get_ntp_async);
	g_task_run_in_thread (task, xdt_get_ntp_thread);
	g_object_unref (task);
}

gboolean
xdt_get_ntp_finish (GAsyncResult  *result,
                    gboolean      *ntp,
                    GError       **error)
{
	gssize value;

	g_return_val_if_fail (g_task_is_valid (result, NULL), FALSE);

	/* Only 0/1 are ever returned, so -1 unambiguously means error */
	value = g_task_propagate_int (G_TASK (result), error);
	if (value == -1)
		return FALSE;

	if (ntp != NULL)
		*ntp = (value != 0);

	return TRUE;
}

static void
xdt_set_ntp_thread (GTask        *task,
                    gpointer      source_object,
                    gpointer      task_data,
                    GCancellable *cancellable)
{
	gboolean use_ntp = GPOINTER_TO_INT (task_data);
	GError *error = NULL;

	if (g_cancellable_set_error_if_cancelled (cancellable, &error)) {
		g_task_return_error (task, error);
		return;
	}

	if (!xdt_set_ntp (use_ntp, &error)) {
		g_task_return_error (task, error);
		return;
	}

	g_task_return_boolean (task, TRUE);
}

void
xdt_set_ntp_async (gboolean             use_ntp,
                   GCancellable        *cancellable,
                   GAsyncReadyCallback  callback,
                   gpointer             user_data)
{
	GTask *task;

	task = g_task_new (NULL, cancellable, callback, user_data);
	g_task_set_source_tag (task, xdt_set_ntp_async);
	g_task_set_task_data (task, GINT_TO_POINTER (use_ntp), NULL);
	g_task_run_in_thread (task, xdt_set_ntp_thread);
	g_object_unref (task);
}

gboolean
xdt_set_ntp_finish (GAsyncResult  *result,
                    GError       **error)
{
	g_return_val_if_fail (g_task_is_valid (result, NULL), FALSE);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
xdt_get_timezone_thread (GTask        *task,
                         gpointer      source_object,
                         gpointer      task_data,
                         GCancellable *cancellable)
{
	gchar *timezone = NULL;
	GError *error = NULL;

	if (g_cancellable_set_error_if_cancelled (cancellable, &error)) {
		g_task_return_error (task, error);
		return;
	}

	if (!xdt_get_timezone (&timezone, &error)) {
		g_task_return_error (task, error);
		return;
	}

	g_task_return_pointer (task, timezone, g_free);
}

void
xdt_get_timezone_async (GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
	GTask *task;

	task = g_task_new (NULL, cancellable, callback, user_data);
	g_task_set_source_tag (task, xdt_get_timezone_async);
	g_task_run_in_thread (task, xdt_get_timezone_thread);
	g_object_unref (task);
}

gboolean
xdt_get_timezone_finish (GAsyncResult  *result,
                         gchar        **timezone,
                         GError       **error)
{
	gchar *value;

	g_return_val_if_fail (g_task_is_valid (result, NULL), FALSE);

	value = g_task_propagate_pointer (G_TASK (result), error);
	if (value == NULL)
		return FALSE;

	if (timezone != NULL)
		*timezone = value;
	else
		g_free (value);

	return TRUE;
}

static void
xdt_set_timezone_thread (GTask        *task,
                         gpointer      source_object,
                         gpointer      task_data,
                         GCancellable *cancellable)
{
	const gchar *timezone = task_data;
	GError *error = NULL;

	if (g_cancellable_set_error_if_cancelled (cancellable, &error)) {
		g_task_return_error (task, error);
		return;
	}

	if (!xdt_set_timezone (timezone, &error)) {
		g_task_return_error (task, error);
		return;
	}

	g_task_return_boolean (task, TRUE);
}

void
xdt_set_timezone_async (const gchar         *timezone,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
	GTask *task;

	task = g_task_new (NULL, cancellable, callback, user_data);
	g_task_set_source_tag (task, xdt_set_timezone_async);
	g_task_set_task_data (task, g_strdup (timezone), g_free);
	g_task_run_in_thread (task, xdt_set_timezone_thread);
	g_object_unref (task);
}

gboolean
xdt_set_timezone_finish (GAsyncResult  *result,
                         GError       **error)
{
	g_return_val_if_fail (g_task_is_valid (result, NULL), FALSE);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
xdt_set_time_thread (GTask        *task,
                     gpointer      source_object,
                     gpointer      task_data,
                     GCancellable *cancellable)
{
	GDateTime *date_time = task_data;
	GError *error = NULL;

	if (g_cancellable_set_error_if_cancelled (cancellable, &error)) {
		g_task_return_error (task, error);
		return;
	}

	if (!xdt_set_time (date_time, &error)) {
		g_task_return_error (task, error);
		return;
	}

	g_task_return_boolean (task, TRUE);
}

void
xdt_set_time_async (GDateTime           *date_time,
                    GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data)
{
	GTask *task;

	g_return_if_fail (date_time != NULL);

	task = g_task_new (NULL, cancellable, callback, user_data);
	g_task_set_source_tag (task, xdt_set_time_async);
	g_task_set_task_data (task,
	                      g_date_time_ref (date_time),
	                      (GDestroyNotify) g_date_time_unref);
	g_task_run_in_thread (task, xdt_set_time_thread);
	g_object_unref (task);
}

gboolean
xdt_set_time_finish (GAsyncResult  *result,
                     GError       **error)
{
	g_return_val_if_fail (g_task_is_valid (result, NULL), FALSE);

	return g_task_propagate_boolean (G_TASK (result), error);
}

static void
xdt_list_timezones_thread (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
	GVariant *timezones = NULL;
	GError *error = NULL;

	if (g_cancellable_set_error_if_cancelled (cancellable, &error)) {
		g_task_return_error (task, error);
		return;
	}

	if (!xdt_list_timezones (&timezones, &error)) {
		g_task_return_error (task, error);
		return;
	}

	g_task_return_pointer (task, timezones, (GDestroyNotify) g_variant_unref);
}

void
xdt_list_timezones_async (GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
	GTask *task;

	task = g_task_new (NULL, cancellable, callback, user_data);
	g_task_set_source_tag (task, xdt_list_timezones_async);
	g_task_run_in_thread (task, xdt_list_timezones_thread);
	g_object_unref (task);
}

gboolean
xdt_list_timezones_finish (GAsyncResult  *result,
                           GVariant     **timezones,
                           GError       **error)
{
	GVariant *value;

	g_return_val_if_fail (g_task_is_valid (result, NULL), FALSE);

	value = g_task_propagate_pointer (G_TASK (result), error);
	if (value == NULL)
		return FALSE;

	if (timezones != NULL)
		*timezones = value;
	else
		g_variant_unref (value);

	return TRUE;
}

