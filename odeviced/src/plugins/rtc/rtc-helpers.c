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
	gchar ret[20];

	/* FIXME: is this even right? */
	if(read_alarm (&alarm))		
		strftime (ret, sizeof (ret), "%s", (struct tm *)&alarm.time);

	g_print ("%s\n", ret);
	return g_strdup(ret);
}


/* Not yet fully implemented..*/
void rtc_set_currtime (const char* seconds) {

	int fd, res;
	unsigned long epoch;
 
	fd = open(RTC_DEV, O_RDONLY);
	if (fd < 0) { 
		perror ("Couldn't open rtc device");
		close (fd);
		return;
	}
	
	epoch = g_printf("%s", seconds);
	g_print ("%d\n", epoch);
	res = ioctl(fd, RTC_EPOCH_SET, &epoch);
	if (res < 1)
		perror ("ioctl(RTC_EPOCH_SET) unsuccessful");
		  	  
	close (fd);
	
}



void rtc_disable_alarm () {
	
	struct rtc_wkalrm alarm;
	
	if (read_alarm (&alarm))
		alarm.enabled = 0;

	write_alarm (&alarm);

}
	
