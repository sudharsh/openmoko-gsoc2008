/* 
 * time-helpers.c
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

/* Helpers for Time subsystem */
#include "Time.h" /* The local Time subsystem */
#include <time.h>

void emit_signal(TimeDBusIface *iface) {
	struct tm *_time;
	int sec, min, hour, mday, mon, year, wday, yday, isdst;
	time_t _tt;
	_tt = time(NULL);
	_time = localtime(&_tt);
	g_signal_emit_by_name (iface, "minute", _time->tm_sec, _time->tm_min,
			       _time->tm_hour, _time->tm_mday, _time->tm_mon,
			       _time->tm_year, _time->tm_wday, _time->tm_yday,
			       _time->tm_isdst);
}
	
	
	
