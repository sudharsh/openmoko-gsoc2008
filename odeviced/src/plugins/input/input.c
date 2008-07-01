/* 
 * input.vala
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

#include "input.h"
#include <src/daemon/helpers.h>
#include <dbus/dbus-glib.h>




struct _InputPrivate {
	char* device;
	char* dev_node;
	gint auxbutton_keycode;
	gint powerbutton_keycode;
	gint touchscreen_keycode;
	char** ignore_list;
	gint ignore_list_length1;
	GList* channels;
};

#define INPUT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_INPUT, InputPrivate))
enum  {
	INPUT_DUMMY_PROPERTY
};
static void _g_list_free_g_io_channel_unref (GList* self);
static void input_onActivity (GIOChannel* source, GIOCondition condition, const char* name);
static GObject * input_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer input_parent_class = NULL;
static void input_dispose (GObject * obj);
Input* input_obj = NULL;
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);


static void g_cclosure_user_marshal_VOID__STRING_STRING_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

static void _g_list_free_g_io_channel_unref (GList* self) {
	g_list_foreach (self, ((GFunc) (g_io_channel_unref)), NULL);
	g_list_free (self);
}


static void input_onActivity (GIOChannel* source, GIOCondition condition, const char* name) {
	g_return_if_fail (source != NULL);
	g_return_if_fail (name != NULL);
	g_message ("input.vala:77: Activity in %s", name);
}


Input* input_new (void) {
	Input * self;
	self = g_object_newv (TYPE_INPUT, 0, NULL);
	return self;
}


static GObject * input_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	InputClass * klass;
	GObjectClass * parent_class;
	Input * self;
	GError * inner_error;
	klass = INPUT_CLASS (g_type_class_peek (TYPE_INPUT));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = INPUT (obj);
	inner_error = NULL;
	{
		GKeyFile* _conf;
		GDir* dir;
		_conf = g_key_file_new ();
		dir = g_dir_open (self->priv->dev_node, ((guint) (0)), &inner_error);
		if (inner_error != NULL) {
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
			g_clear_error (&inner_error);
		}
		{
			char* _tmp0;
			char** _tmp2;
			gint _tmp1;
			const char* _tmp3;
			char* name;
			g_key_file_load_from_file (_conf, "/usr/share/odeviced/plugins/input.plugin", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch0_g_error;
			}
			g_key_file_set_list_separator (_conf, ',');
			_tmp0 = NULL;
			self->priv->device = (_tmp0 = odeviced_get_device (), (self->priv->device = (g_free (self->priv->device), NULL)), _tmp0);
			self->priv->auxbutton_keycode = g_key_file_get_integer (_conf, self->priv->device, "auxbutton", &inner_error);
			if (inner_error != NULL) {
				goto __catch0_g_error;
			}
			self->priv->powerbutton_keycode = g_key_file_get_integer (_conf, self->priv->device, "powerbutton", &inner_error);
			if (inner_error != NULL) {
				goto __catch0_g_error;
			}
			self->priv->touchscreen_keycode = g_key_file_get_integer (_conf, self->priv->device, "touchscreen", &inner_error);
			if (inner_error != NULL) {
				goto __catch0_g_error;
			}
			_tmp2 = NULL;
			self->priv->ignore_list = (_tmp2 = g_key_file_get_string_list (_conf, self->priv->device, "ignore_input", &_tmp1, &inner_error), (self->priv->ignore_list = (_vala_array_free (self->priv->ignore_list, self->priv->ignore_list_length1, ((GDestroyNotify) (g_free))), NULL)), self->priv->ignore_list_length1 = _tmp1, _tmp2);
			if (inner_error != NULL) {
				goto __catch0_g_error;
			}
			_tmp3 = NULL;
			name = (_tmp3 = g_dir_read_name (dir), (_tmp3 == NULL ? NULL : g_strdup (_tmp3)));
			while (name != NULL) {
				char* _tmp9;
				const char* _tmp8;
				/* Wait till vala has support for "in" operator in if clauses*/
				if (g_str_has_prefix (name, "event") && !(g_utf8_get_char (g_utf8_offset_to_pointer (name, 5)) == '2' || g_utf8_get_char (g_utf8_offset_to_pointer (name, 5)) == '3')) {
					char* _tmp5;
					char* _tmp4;
					GIOChannel* _tmp6;
					GIOChannel* _temp;
					GIOChannel* _tmp7;
					_tmp5 = NULL;
					_tmp4 = NULL;
					_tmp6 = NULL;
					_temp = (_tmp6 = g_io_channel_new_file ((_tmp5 = g_strconcat ((_tmp4 = g_strconcat (self->priv->dev_node, "/", NULL)), name, NULL)), "r", &inner_error), (_tmp5 = (g_free (_tmp5), NULL)), (_tmp4 = (g_free (_tmp4), NULL)), _tmp6);
					if (inner_error != NULL) {
						goto __catch0_g_error;
					}
					_tmp7 = NULL;
					self->priv->channels = g_list_append (self->priv->channels, (_tmp7 = _temp, (_tmp7 == NULL ? NULL : g_io_channel_ref (_tmp7))));
					g_io_add_watch (_temp, G_IO_IN, ((GIOFunc) (input_onActivity)), NULL);
					(_temp == NULL ? NULL : (_temp = (g_io_channel_unref (_temp), NULL)));
				}
				_tmp9 = NULL;
				_tmp8 = NULL;
				name = (_tmp9 = (_tmp8 = g_dir_read_name (dir), (_tmp8 == NULL ? NULL : g_strdup (_tmp8))), (name = (g_free (name), NULL)), _tmp9);
			}
			name = (g_free (name), NULL);
		}
		goto __finally0;
		__catch0_g_error:
		{
			GError * error;
			error = inner_error;
			inner_error = NULL;
			{
				g_message (error->message);
				(error == NULL ? NULL : (error = (g_error_free (error), NULL)));
			}
		}
		__finally0:
		;
		(_conf == NULL ? NULL : (_conf = (g_key_file_free (_conf), NULL)));
		(dir == NULL ? NULL : (dir = (g_dir_close (dir), NULL)));
	}
	return obj;
}


static void input_class_init (InputClass * klass) {
	input_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (InputPrivate));
	G_OBJECT_CLASS (klass)->constructor = input_constructor;
	G_OBJECT_CLASS (klass)->dispose = input_dispose;
	g_signal_new ("event", TYPE_INPUT, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_user_marshal_VOID__STRING_STRING_INT, G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
	static const DBusGMethodInfo input_dbus_methods[] = {
}
;
	static const DBusGObjectInfo input_dbus_object_info = { 0, input_dbus_methods, 0, "", "org.freesmartphone.Device.Input\0Event\0", "" };
	dbus_g_object_type_install_info (TYPE_INPUT, &input_dbus_object_info);
}


static void input_instance_init (Input * self) {
	self->priv = INPUT_GET_PRIVATE (self);
	self->priv->device = g_new0 (char, 1);
	self->priv->dev_node = g_strdup ("/dev/input");
	self->priv->channels = NULL;
}


static void input_dispose (GObject * obj) {
	Input * self;
	self = INPUT (obj);
	self->priv->device = (g_free (self->priv->device), NULL);
	self->priv->dev_node = (g_free (self->priv->dev_node), NULL);
	self->priv->ignore_list = (_vala_array_free (self->priv->ignore_list, self->priv->ignore_list_length1, ((GDestroyNotify) (g_free))), NULL);
	(self->priv->channels == NULL ? NULL : (self->priv->channels = (_g_list_free_g_io_channel_unref (self->priv->channels), NULL)));
	G_OBJECT_CLASS (input_parent_class)->dispose (obj);
}


GType input_get_type (void) {
	static GType input_type_id = 0;
	if (G_UNLIKELY (input_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (InputClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) input_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Input), 0, (GInstanceInitFunc) input_instance_init };
		input_type_id = g_type_register_static (G_TYPE_OBJECT, "Input", &g_define_type_info, 0);
	}
	return input_type_id;
}


gboolean input_init (ODevicedPlugin* plugin) {
	Input* _tmp0;
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (plugin), FALSE);
	_tmp0 = NULL;
	input_obj = (_tmp0 = input_new (), (input_obj == NULL ? NULL : (input_obj = (g_object_unref (input_obj), NULL))), _tmp0);
	if (input_obj == NULL) {
		return FALSE;
	}
	odeviced_register_dbus_object (plugin, G_OBJECT (input_obj));
	return TRUE;
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



static void g_cclosure_user_marshal_VOID__STRING_STRING_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef void (*GMarshalFunc_VOID__STRING_STRING_INT) (gpointer data1, const char* arg_1, const char* arg_2, gint arg_3, gpointer data2);
	register GMarshalFunc_VOID__STRING_STRING_INT callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	cc = ((GCClosure *) (closure));
	g_return_if_fail (n_param_values == 4);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_VOID__STRING_STRING_INT) ((marshal_data ? marshal_data : cc->callback)));
	callback (data1, g_value_get_string (param_values + 1), g_value_get_string (param_values + 2), g_value_get_int (param_values + 3), data2);
}



