/*  xfce4-netspeed-plugin
 *
 *  Copyright (c) 2011 Calin Crisan <ccrisan@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _DEVICE_INFO_H
#define _DEVICE_INFO_H

#include <glib.h>

#define DEVICE_TYPE_UNKNOWN     0
#define DEVICE_TYPE_LOOPBACK    1
#define DEVICE_TYPE_PLIP        2
#define DEVICE_TYPE_SLIP        3
#define DEVICE_TYPE_PPP         4
#define DEVICE_TYPE_ETHERNET    5
#define DEVICE_TYPE_WIRELESS    6


typedef struct {

    gchar *name;
    gint type;
    guint64 rx;
    guint64 tx;
    gboolean up;

} DeviceInfo;


DeviceInfo *device_info_new(char *device);
void        device_info_free(DeviceInfo *device_info);
void        device_info_fill(DeviceInfo *device_info);
GList*      device_info_list_get();
void        device_info_list_free(GList* list);


#endif /* _DEVICE_INFO_H */
