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

#include <time.h>

#include <glib.h>

#define RTC_DEV "/dev/rtc"


static int read_alarm (struct rtc_wkalrm *alarm) {
	
	int res;
	int fd = open (RTC_DEV, O_RDONLY);
	if (fd < 0) {
		perror ("Couldn't open rtc device");
		close (fd);
		return 0;
	}

	res = ioctl(fd, RTC_WKALM_RD, alarm);
	if (res < 0) {
		perror("ioctl(RTC_WKALM_RD) error");
		g_message ("ioctl call unsuccessful");
		close (fd);
		return 0;
	}
	
	close (fd);
	return 1;
}


static int write_alarm (struct rtc_wkalrm *alarm) {

	int res;
	int fd = open (RTC_DEV, O_RDONLY);
	if (fd < 0) {
		perror ("Couldn't open rtc device");
		close (fd);
		return 0;
	}

	res = ioctl (fd, RTC_WKALM_SET, alarm);
	if (res < 0) {
		perror ("ioctl(RTC_WKALM_SET) error");
		close (fd);
		return 0;
	}
	
	close (fd);
	return 1;
}



char* rtc_get_wakeup () {

	struct rtc_wkalrm alarm;
	struct tm time;
	gchar ret[20];

	/* FIXME: is this even right? */
	if(read_alarm (&alarm))	{
		if (!alarm.enabled)
			g_strdup ("0");
			
		time.tm_year = alarm.time.tm_year - 1900;
		time.tm_mon = alarm.time.tm_mon - 1;
		time.tm_sec = alarm.time.tm_sec;
		time.tm_min = alarm.time.tm_min;
		time.tm_hour = alarm.time.tm_hour;
		time.tm_mday = alarm.time.tm_mday;
		time.tm_wday = alarm.time.tm_wday;
		time.tm_yday = alarm.time.tm_yday;
		time.tm_isdst = alarm.time.tm_isdst;
		strftime (ret, sizeof (ret), "%s", &time);
	}

	g_print ("%s\n", ret);
	return g_strdup(ret);
}


/* Not yet fully implemented..*/
void rtc_set_currtime (const char* seconds) {

	time_t time;
	unsigned long epoch;
 
	epoch = atol (seconds);
	time = (time_t) epoch;
	g_print ("%ld\n", epoch);
	stime (&time);
		  	  
}



void rtc_disable_alarm () {
	
	struct rtc_wkalrm alarm;
	
	if (read_alarm (&alarm))
		alarm.enabled = 0;

	write_alarm (&alarm);

}
	
