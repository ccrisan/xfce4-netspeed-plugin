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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef _
# include <libintl.h>
# define _(String) gettext (String)
#endif

#include "utils.h"


gchar* bytes_to_string(double bytes, gboolean per_sec, gboolean bits) {
    const char *format;
    const char *unit;
    guint kilo; /* no really a kilo : a kilo or kibi */

    if (bits) {
        bytes *= 8;
        kilo = 1000;
    }
    else {
        kilo = 1024;
    }

    if (bytes < kilo) {
        format = "%.0f %s";

        if (per_sec)
            unit = bits ? _("b/s")   : _("B/s");
        
        else
            unit = bits ? _("bits")  : _("bytes");

    }
    else if (bytes < (kilo * kilo)) {
        format = (bytes < (100 * kilo)) ? "%.1f %s" : "%.0f %s";
        bytes /= kilo;

        if (per_sec)
            unit = bits ? _("kb/s") : _("KiB/s");
        
        else
            unit = bits ? _("kb")   : _("KiB");

    } 
    else {
        format = "%.1f %s";
        bytes /= kilo * kilo;

        if (per_sec)
            unit = bits ? _("Mb/s") : _("MiB/s");
        
        else
            unit = bits ? _("Mb")   : _("MiB");
    }

    return g_strdup_printf(format, bytes, unit);
}
