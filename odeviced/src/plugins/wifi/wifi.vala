/* 
 * wifi.vala
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

/* This file exists to generate wifi.[c/h], being the lazy bum I am
   $ valac wifi.vala --pkg dbus-glib-1 -C */

using GLib;


[DBus (name = "org.freesmartphone.Device.Wifi") ]
public class WifiPlugin: Object {
	public bool GetStatus(string iface) {
		return true;
	}
	
	public bool SetControl(string iface, bool enable) {
		return true;
	}
}

/*
gboolean wifi_plugin_GetStatus (WifiPlugin* self, const char* iface) {
	g_return_val_if_fail (IS_WIFI_PLUGIN (self), FALSE);
	g_return_val_if_fail (iface != NULL, FALSE);
	struct iwreq wrq;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (!sock)
	{
		perror( "Unable to open socket" );
		return 0;
	}

	memset (&wrq, 0, sizeof (struct iwreq));
	strncpy ((char *)&wrq.ifr_name, iface, IFNAMSIZ);

	if (ioctl (sock, SIOCGIWTXPOW, &wrq) != 0)
	{
		perror( "Error performing ioctl" );
		close (sock);
		return 0;
	}
	
	close (sock);
	
	return !wrq.u.txpower.disabled;

}


gboolean wifi_plugin_SetControl (WifiPlugin* self, const char* iface, gboolean enable) {
	g_return_val_if_fail (IS_WIFI_PLUGIN (self), FALSE);
	g_return_val_if_fail (iface != NULL, FALSE);
	struct iwreq wrq;
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (!sock)
	{
		perror( "Unable to open socket" );
		return 0;
	}

	memset (&wrq, 0, sizeof (struct iwreq));
	strncpy ((char *)&wrq.ifr_name, iface, IFNAMSIZ);

	if (ioctl (sock, SIOCGIWTXPOW, &wrq) != 0)
	{
		perror( "Error performing ioctl" );
		close (sock);
		return 0;
	}

	if ( wrq.u.txpower.disabled != !enable )
	{
		wrq.u.txpower.disabled = !enable;

		if (ioctl (sock, SIOCSIWTXPOW, &wrq) != 0)
		{
			perror( "Error performing ioctl" );
			close (sock);
			return 0;
		}
	}

	close (sock);
	return 1;

}


G_MODULE_EXPORT gboolean wifi_init (ODevicedPlugin *plugin) {

          GError * inner_error;
          {
                  WifiPlugin* wifiobj;
                  if (inner_error != NULL) {
                          goto __catch0_g_error;
                  }
                  wifiobj = wifi_plugin_new ();
                  if(wifiobj) {
			  odeviced_register_dbus_object (plugin, G_OBJECT(wifiobj));
                  }
          }
          goto __finally0;
          __catch0_g_error:
          {
                  GError * error;
                  error = inner_error;
		  inner_error = NULL;
                  { 
                          fprintf (stderr, "%s\n", error->message);
                  }
          }
         __finally0:
          ;
  	  return TRUE;
	  
 }

*/

public int init() {
	try {
		DBus.Connection conn = DBus.Bus.get(DBus.BusType.SYSTEM);
		WifiPlugin wifiobj = new WifiPlugin();
		conn.register_object ("/org/freesmartphone/Device/Plugins/Wifi", wifiobj);
		
		
	}
	catch (GLib.Error error) {
		stderr.printf("%s\n", error.message);
	}
	return 0;
	
}


		