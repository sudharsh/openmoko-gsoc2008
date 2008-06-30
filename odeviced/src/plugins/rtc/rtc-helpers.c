/* 
 * rtc-helpers.c
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

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>


char* rtc_get_wakeup () {

	int fd, res;
	struct rtc_wkalrm alarm;			
	gchar* ret;

	fd = open("/dev/rtc", O_RDONLY);
	if (fd < 0) {
		g_message("Couldn't open rtc device");
		close (fd);
		return "0";
	}
	
	res = ioctl(fd, RTC_WKALM_RD, &alarm);
	if (res < 0) {
		perror("ioctl(RTC_WKALM_RD) error");
		g_message ("ioctl call unsuccessful");
		close (fd);
		return "0";
	}
	
	ret = g_strdup_printf("%d", (alarm.time.tm_sec + (60 * alarm.time.tm_min) 
			     + (60 * 60 * alarm.time.tm_hour)));
	close (fd);
	return ret;
}


/* Not yet fully implemented..just exists for the sake of it */
void rtc_set_currtime (const char* seconds) {

	int fd, res;
	struct rtc_time time;
	fd = open("/dev/rtc", O_RDONLY);
	if (fd < 0) { 
		g_message ("Couldn't open rtc device");
		close (fd);
		return;
	}
	
	time.tm_sec = g_printf("%s", seconds);
	
	res = ioctl(fd, RTC_SET_TIME, &time);
	if (res < 1)
		perror ("ioctl(RTC_SET_TIME) unsuccessful");
		  	  
	close (fd);
	
}
