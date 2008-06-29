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
#include "helpers.h"
#include <dbus/dbus-glib.h>



gint input_space_pollfd_length1;
GPollFD* input_space_pollfd = NULL;
struct _InputPrivate {
	char* device;
	char* dev_node;
	gint auxbutton_keycode;
	gint powerbutton_keycode;
	gint touchscreen_keycode;
};

#define INPUT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_INPUT, InputPrivate))
enum  {
	INPUT_DUMMY_PROPERTY
};
static gboolean input_prepare (GSource* source, gint* timeout);
static gboolean input_check (GSource* source);
static gboolean input_dispatch (GSource* source, GSourceFunc cback, void* cback_target);
static GObject * input_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer input_parent_class = NULL;
static void input_dispose (GObject * obj);


static void g_cclosure_user_marshal_VOID__STRING_STRING_INT (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

static gboolean input_prepare (GSource* source, gint* timeout) {
	g_return_val_if_fail (source != NULL, FALSE);
	return FALSE;
}


static gboolean input_check (GSource* source) {
	g_return_val_if_fail (source != NULL, FALSE);
	{
		GPollFD* _fd_collection;
		int _fd_collection_length1;
		int _fd_it;
		_fd_collection = input_space_pollfd;
		_fd_collection_length1 = input_space_pollfd_length1;
		g_assert (input_space_pollfd_length1 != -1);
		for (_fd_it = 0; _fd_it < input_space_pollfd_length1; _fd_it = _fd_it + 1) {
			GPollFD _fd;
			_fd = _fd_collection[_fd_it];
			{
				if ((_fd.revents & G_IO_IN) != 0) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


static gboolean input_dispatch (GSource* source, GSourceFunc cback, void* cback_target) {
	g_return_val_if_fail (source != NULL, FALSE);
	return TRUE;
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
		GSource* watcher;
		_conf = g_key_file_new ();
		dir = g_dir_open (self->priv->dev_node, ((guint) (0)), &inner_error);
		if (inner_error != NULL) {
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
			g_clear_error (&inner_error);
		}

		/* This is crappy, Must look for a way to do this in vala */
		
		static GSourceFuncs funcs = {
			input_prepare,
			input_check,
			input_dispatch,
			NULL,
		};
		
		watcher = g_source_new (&funcs, ((guint) (sizeof (GSource))));
		
		
		{
			char* _tmp0;
			char* _tmp2;
			const char* _tmp1;
			gint i;
			g_key_file_load_from_file (_conf, "/usr/share/odeviced/plugins/input.plugin", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch8_g_error;
			}
			_tmp0 = NULL;
			self->priv->device = (_tmp0 = odeviced_get_device (), (self->priv->device = (g_free (self->priv->device), NULL)), _tmp0);
			self->priv->auxbutton_keycode = g_key_file_get_integer (_conf, self->priv->device, "auxbutton", &inner_error);
			if (inner_error != NULL) {
				goto __catch8_g_error;
			}
			self->priv->powerbutton_keycode = g_key_file_get_integer (_conf, self->priv->device, "powerbutton", &inner_error);
			if (inner_error != NULL) {
				goto __catch8_g_error;
			}
			self->priv->touchscreen_keycode = g_key_file_get_integer (_conf, self->priv->device, "touchscreen", &inner_error);
			if (inner_error != NULL) {
				goto __catch8_g_error;
			}
			_tmp2 = NULL;
			_tmp1 = NULL;
			self->priv->dev_node = (_tmp2 = (_tmp1 = g_dir_read_name (dir), (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->dev_node = (g_free (self->priv->dev_node), NULL)), _tmp2);
			i = 0;
			while (self->priv->dev_node != NULL) {
				char* _tmp4;
				const char* _tmp3;
				if (g_str_has_prefix (self->priv->dev_node, "event")) {
					GPollFD _fd;
					input_space_pollfd[i].fd = open(g_strdup_printf("/dev/input/event%d", i), O_RDONLY);
					input_space_pollfd[i].revents = 0;
					input_space_pollfd[i].events = G_IO_IN | G_IO_HUP | G_IO_ERR;
					_fd = input_space_pollfd[i];
					g_source_add_poll (watcher, &_fd);
					i++;
				}
				_tmp4 = NULL;
				_tmp3 = NULL;
				self->priv->dev_node = (_tmp4 = (_tmp3 = g_dir_read_name (dir), (_tmp3 == NULL ? NULL : g_strdup (_tmp3))), (self->priv->dev_node = (g_free (self->priv->dev_node), NULL)), _tmp4);
			}
		}
		goto __finally8;
		__catch8_g_error:
		{
			GError * error;
			error = inner_error;
			inner_error = NULL;
			{
				g_message (error->message);
				(error == NULL ? NULL : (error = (g_error_free (error), NULL)));
			}
		}
		__finally8:
		;
		(_conf == NULL ? NULL : (_conf = (g_key_file_free (_conf), NULL)));
		(dir == NULL ? NULL : (dir = (g_dir_close (dir), NULL)));
		(watcher == NULL ? NULL : (watcher = (g_source_unref (watcher), NULL)));
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
}


static void input_dispose (GObject * obj) {
	Input * self;
	self = INPUT (obj);
	self->priv->device = (g_free (self->priv->device), NULL);
	self->priv->dev_node = (g_free (self->priv->dev_node), NULL);
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


G_MODULE_EXPORT gboolean input_init (ODevicedPlugin *plugin) {

	Input* inputobj;
	inputobj = input_new ();
	if(inputobj) 
		odeviced_register_dbus_object (plugin, G_OBJECT(inputobj));
	else
		return FALSE;
	
	return TRUE;
  
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



