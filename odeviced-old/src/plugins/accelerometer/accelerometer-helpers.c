/* 
 * accelerometer-helpers.c
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

#include <glib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>

#include "accelerometer.h"

int retrieve_xyz (int fd) {

	struct input_event event;
	int len;

	len = read (fd, &event, sizeof(event));
	if (len < 0) {
		perror ("read");
		return -1;
	}
	
	if (event.type != EV_SYN) {
		g_print ("\tAccel: INFO value %d\n", event.value);
		return event.value;
	}
		
	return -1; /* EV_SYN */
}	
	
