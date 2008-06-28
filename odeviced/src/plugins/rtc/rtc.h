/* 
 * rtc.vala
 * Written by Sudharshan "Sup3rkiddo" S <sudharsh@gmail.com>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __RTC_H__
#define __RTC_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
G_BEGIN_DECLS


#define TYPE_REAL_TIME_CLOCK (real_time_clock_get_type ())
#define REAL_TIME_CLOCK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_REAL_TIME_CLOCK, RealTimeClock))
#define REAL_TIME_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_REAL_TIME_CLOCK, RealTimeClockClass))
#define IS_REAL_TIME_CLOCK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_REAL_TIME_CLOCK))
#define IS_REAL_TIME_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_REAL_TIME_CLOCK))
#define REAL_TIME_CLOCK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_REAL_TIME_CLOCK, RealTimeClockClass))

typedef struct _RealTimeClock RealTimeClock;
typedef struct _RealTimeClockClass RealTimeClockClass;
typedef struct _RealTimeClockPrivate RealTimeClockPrivate;

/* RTC plugin for odeviced 

*/
struct _RealTimeClock {
	GObject parent_instance;
	RealTimeClockPrivate * priv;
};

struct _RealTimeClockClass {
	GObjectClass parent_class;
};


char* real_time_clock_GetName (RealTimeClock* self);
char* real_time_clock_GetCurrentTime (RealTimeClock* self);
char* real_time_clock_GetWakeupTime (RealTimeClock* self);
const char* real_time_clock_get_node (RealTimeClock* self);
const char* real_time_clock_get_dbus_path (RealTimeClock* self);
GType real_time_clock_get_type (void);


G_END_DECLS

#endif
