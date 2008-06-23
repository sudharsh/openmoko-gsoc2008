/* 
 *  helpers.vala
 *  Author: Sudharshan "Sup3rkiddo" sudharsh@gmail.com>
 *  All Rights Reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "helpers.h"
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <gio/gio.h>
#include "odeviced.h"
#include "service.h"




static void _g_list_free_g_object_unref (GList* self);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);



void odeviced_register_dbus_object (ODevicedPlugin* plugin, GObject* interface_obj) {
	GError * inner_error;
	g_return_if_fail (ODEVICED_IS_PLUGIN (plugin));
	g_return_if_fail (G_IS_OBJECT (interface_obj));
	inner_error = NULL;
	{
		if (g_key_file_has_group (plugin->conf, odeviced_plugin_get_name (plugin))) {
			char* at_path;
			at_path = g_key_file_get_string (plugin->conf, odeviced_plugin_get_name (plugin), "dbus_object_path", &inner_error);
			if (inner_error != NULL) {
				goto __catch4_g_error;
			}
			dbus_g_connection_register_g_object (odeviced_connection, at_path, interface_obj);
			at_path = (g_free (at_path), NULL);
		} else {
			g_critical ("helpers.vala:33: Malformed plugin configuration file");
		}
	}
	goto __finally4;
	__catch4_g_error:
	{
		GError * error;
		error = inner_error;
		inner_error = NULL;
		{
			g_critical (error->message);
		}
	}
	__finally4:
	;
}


char* odeviced_get_device (void) {
	const char* _tmp0;
	_tmp0 = NULL;
	return (_tmp0 = odeviced_service_dev_name, (_tmp0 == NULL ? NULL : g_strdup (_tmp0)));
}


static void _g_list_free_g_object_unref (GList* self) {
	g_list_foreach (self, ((GFunc) (g_object_unref)), NULL);
	g_list_free (self);
}


GList* odeviced_compute_objects (ODevicedPlugin* plugin, GType klass) {
	GError * inner_error;
	GList* objList;
	char* dev_class;
	char* dev_node;
	GList* _tmp13;
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (plugin), NULL);
	inner_error = NULL;
	objList = NULL;
	dev_class = NULL;
	dev_node = NULL;
	if (g_key_file_has_group (plugin->conf, odeviced_plugin_get_name (plugin))) {
		{
			char* _tmp0;
			char* _tmp1;
			char* dbus_path;
			GDir* dir;
			const char* _tmp2;
			char* node;
			gint i;
			_tmp0 = NULL;
			dev_class = (_tmp0 = g_key_file_get_string (plugin->conf, odeviced_plugin_get_name (plugin), "device_class", &inner_error), (dev_class = (g_free (dev_class), NULL)), _tmp0);
			if (inner_error != NULL) {
				if (inner_error->domain == G_FILE_ERROR) {
					goto __catch5_g_file_error;
				}
				if (inner_error->domain == G_KEY_FILE_ERROR) {
					goto __catch5_g_key_file_error;
				}
				g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
				g_clear_error (&inner_error);
			}
			_tmp1 = NULL;
			dev_node = (_tmp1 = g_strconcat ("/sys/class/", dev_class, NULL), (dev_node = (g_free (dev_node), NULL)), _tmp1);
			dbus_path = g_key_file_get_string (plugin->conf, odeviced_plugin_get_name (plugin), "dbus_object_path", &inner_error);
			if (inner_error != NULL) {
				if (inner_error->domain == G_FILE_ERROR) {
					goto __catch5_g_file_error;
				}
				if (inner_error->domain == G_KEY_FILE_ERROR) {
					goto __catch5_g_key_file_error;
				}
				g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
				g_clear_error (&inner_error);
			}
			dir = g_dir_open (dev_node, ((guint) (0)), &inner_error);
			if (inner_error != NULL) {
				if (inner_error->domain == G_FILE_ERROR) {
					goto __catch5_g_file_error;
				}
				if (inner_error->domain == G_KEY_FILE_ERROR) {
					goto __catch5_g_key_file_error;
				}
				g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
				g_clear_error (&inner_error);
			}
			_tmp2 = NULL;
			node = (_tmp2 = g_dir_read_name (dir), (_tmp2 == NULL ? NULL : g_strdup (_tmp2)));
			i = 0;
			while (node != NULL) {
				char* _tmp6;
				char* _tmp5;
				char* _tmp4;
				char* _tmp3;
				GObject* _tmp7;
				GObject* obj;
				char* _tmp9;
				char* _tmp8;
				GObject* _tmp10;
				char* _tmp12;
				const char* _tmp11;
				_tmp6 = NULL;
				_tmp5 = NULL;
				_tmp4 = NULL;
				_tmp3 = NULL;
				_tmp7 = NULL;
				obj = (_tmp7 = g_object_new (klass, "node", (_tmp4 = g_strconcat ((_tmp3 = g_strconcat (dev_node, "/", NULL)), node, NULL)), "dbus_path", (_tmp6 = g_strconcat ((_tmp5 = g_strconcat (dbus_path, "/", NULL)), node, NULL)), NULL), (_tmp6 = (g_free (_tmp6), NULL)), (_tmp5 = (g_free (_tmp5), NULL)), (_tmp4 = (g_free (_tmp4), NULL)), (_tmp3 = (g_free (_tmp3), NULL)), _tmp7);
				_tmp9 = NULL;
				_tmp8 = NULL;
				g_message ("helpers.vala:65: Created object for sysfs node, %s", (_tmp9 = g_strconcat ((_tmp8 = g_strconcat (dev_node, "/", NULL)), node, NULL)));
				_tmp9 = (g_free (_tmp9), NULL);
				_tmp8 = (g_free (_tmp8), NULL);
				_tmp10 = NULL;
				objList = g_list_append (objList, (_tmp10 = obj, (_tmp10 == NULL ? NULL : g_object_ref (_tmp10))));
				_tmp12 = NULL;
				_tmp11 = NULL;
				node = (_tmp12 = (_tmp11 = g_dir_read_name (dir), (_tmp11 == NULL ? NULL : g_strdup (_tmp11))), (node = (g_free (node), NULL)), _tmp12);
				(obj == NULL ? NULL : (obj = (g_object_unref (obj), NULL)));
			}
			dbus_path = (g_free (dbus_path), NULL);
			(dir == NULL ? NULL : (dir = (g_dir_close (dir), NULL)));
			node = (g_free (node), NULL);
		}
		goto __finally5;
		__catch5_g_file_error:
		{
			GError * e;
			e = inner_error;
			inner_error = NULL;
			{
				g_message ("helpers.vala:72: Device class: %s doesn't exist", dev_class);
			}
		}
		goto __finally5;
		__catch5_g_key_file_error:
		{
			GError * e;
			e = inner_error;
			inner_error = NULL;
			{
				g_message ("helpers.vala:75: %s %s", odeviced_plugin_get_name (plugin), e->message);
			}
		}
		__finally5:
		;
	}
	_tmp13 = NULL;
	return (_tmp13 = objList, (dev_class = (g_free (dev_class), NULL)), (dev_node = (g_free (dev_node), NULL)), _tmp13);
}


char* odeviced_compute_name (const char* path) {
	char** _tmp0;
	gint split_list_length1;
	char** split_list;
	guint _length;
	const char* _tmp1;
	char* _tmp2;
	g_return_val_if_fail (path != NULL, NULL);
	_tmp0 = NULL;
	split_list = (_tmp0 = g_strsplit (path, "/", 0), split_list_length1 = -1, _tmp0);
	_length = g_strv_length (split_list);
	_tmp1 = NULL;
	_tmp2 = NULL;
	return (_tmp2 = (_tmp1 = split_list[_length - 1], (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (split_list = (_vala_array_free (split_list, split_list_length1, ((GDestroyNotify) (g_free))), NULL)), _tmp2);
}


/* I know =(, Ideally these should have been a single function */
gint odeviced_read_integer (const char* node) {
	gint val;
	FILE* node_file;
	gint _tmp1;
	g_return_val_if_fail (node != NULL, 0);
	val = 0;
	node_file = fopen (node, "r");
	if (node_file == NULL) {
		gint _tmp0;
		return (_tmp0 = -1, (node_file == NULL ? NULL : (node_file = (fclose (node_file), NULL))), _tmp0);
	}
	fscanf (node_file, "%d", &val, NULL);
	return (_tmp1 = val, (node_file == NULL ? NULL : (node_file = (fclose (node_file), NULL))), _tmp1);
}


char* odeviced_read_string (const char* node) {
	GError * inner_error;
	GFile* node_file;
	GDataInputStream* stream;
	char* line;
	char* _tmp4;
	g_return_val_if_fail (node != NULL, NULL);
	inner_error = NULL;
	node_file = g_file_new_for_path (node);
	stream = NULL;
	line = NULL;
	{
		GDataInputStream* _tmp0;
		char* _tmp2;
		const char* _tmp1;
		_tmp0 = NULL;
		stream = (_tmp0 = g_data_input_stream_new (G_INPUT_STREAM (g_file_read (node_file, NULL, &inner_error))), (stream == NULL ? NULL : (stream = (g_object_unref (stream), NULL))), _tmp0);
		_tmp2 = NULL;
		_tmp1 = NULL;
		line = (_tmp2 = (_tmp1 = g_data_input_stream_read_line (stream, NULL, NULL, &inner_error), (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (line = (g_free (line), NULL)), _tmp2);
		if (inner_error != NULL) {
			goto __catch6_g_error;
		}
	}
	goto __finally6;
	__catch6_g_error:
	{
		GError * e;
		e = inner_error;
		inner_error = NULL;
		{
			char* _tmp3;
			g_message (e->message);
			_tmp3 = NULL;
			return (_tmp3 = NULL, (node_file == NULL ? NULL : (node_file = (g_object_unref (node_file), NULL))), (stream == NULL ? NULL : (stream = (g_object_unref (stream), NULL))), (line = (g_free (line), NULL)), _tmp3);
		}
	}
	__finally6:
	;
	_tmp4 = NULL;
	return (_tmp4 = line, (node_file == NULL ? NULL : (node_file = (g_object_unref (node_file), NULL))), (stream == NULL ? NULL : (stream = (g_object_unref (stream), NULL))), _tmp4);
}


gboolean odeviced_write_integer (const char* node, gint val) {
	FILE* node_file;
	gboolean _tmp1;
	g_return_val_if_fail (node != NULL, FALSE);
	node_file = fopen (node, "w");
	if (node_file == NULL) {
		gboolean _tmp0;
		return (_tmp0 = FALSE, (node_file == NULL ? NULL : (node_file = (fclose (node_file), NULL))), _tmp0);
	}
	fprintf (node_file, "%d", val);
	return (_tmp1 = TRUE, (node_file == NULL ? NULL : (node_file = (fclose (node_file), NULL))), _tmp1);
}


static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func) {
	if (array != NULL && destroy_func != NULL) {
		int i;
		if (array_length >= 0)
		for (i = 0; i < array_length; i = i + 1) {
			if (((gpointer*) (array))[i] != NULL)
			destroy_func (((gpointer*) (array))[i]);
		}
		else
		for (i = 0; ((gpointer*) (array))[i] != NULL; i = i + 1) {
			destroy_func (((gpointer*) (array))[i]);
		}
	}
	g_free (array);
}




