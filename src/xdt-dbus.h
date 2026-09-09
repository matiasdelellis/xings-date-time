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
xdt_get_local_rtc (gboolean *local_rtc, GError **error);

gboolean
xdt_set_local_rtc (gboolean local_rtc, GError **error);

gboolean
xdt_get_can_ntp (gboolean *can_ntp, GError **error);

gboolean
xdt_get_timezone (gchar **timezone, GError **error);

gboolean
xdt_set_timezone (const gchar *timezone, GError **error);

gboolean
xdt_set_time (GDateTime *date_time, GError **error);

gboolean
xdt_list_timezones (GVariant **timezones, GError **error);

/* Asynchronous variants.
 *
 * The synchronous helpers above are executed in a worker thread, so slow
 * system bus calls (or polkit authentication dialogs) never block the UI.
 * The callbacks run on the thread-default main context.
 */

void
xdt_get_ntp_async (GCancellable        *cancellable,
                   GAsyncReadyCallback  callback,
                   gpointer             user_data);

gboolean
xdt_get_ntp_finish (GAsyncResult  *result,
                    gboolean      *ntp,
                    GError       **error);

void
xdt_set_ntp_async (gboolean             use_ntp,
                   GCancellable        *cancellable,
                   GAsyncReadyCallback  callback,
                   gpointer             user_data);

gboolean
xdt_set_ntp_finish (GAsyncResult  *result,
                    GError       **error);

void
xdt_get_local_rtc_async (GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data);

gboolean
xdt_get_local_rtc_finish (GAsyncResult  *result,
                          gboolean      *local_rtc,
                          GError       **error);

void
xdt_set_local_rtc_async (gboolean             local_rtc,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data);

gboolean
xdt_set_local_rtc_finish (GAsyncResult  *result,
                          GError       **error);

void
xdt_get_can_ntp_async (GCancellable        *cancellable,
                       GAsyncReadyCallback  callback,
                       gpointer             user_data);

gboolean
xdt_get_can_ntp_finish (GAsyncResult  *result,
                        gboolean      *can_ntp,
                        GError       **error);

void
xdt_get_timezone_async (GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data);

gboolean
xdt_get_timezone_finish (GAsyncResult  *result,
                         gchar        **timezone,
                         GError       **error);

void
xdt_set_timezone_async (const gchar         *timezone,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data);

gboolean
xdt_set_timezone_finish (GAsyncResult  *result,
                         GError       **error);

void
xdt_set_time_async (GDateTime           *date_time,
                    GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data);

gboolean
xdt_set_time_finish (GAsyncResult  *result,
                     GError       **error);

void
xdt_list_timezones_async (GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data);

gboolean
xdt_list_timezones_finish (GAsyncResult  *result,
                           GVariant     **timezones,
                           GError       **error);

#endif /* __XDT_DBUS_H__ */
