/* 
 *  plugins.vala
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

#include "plugin.h"
#include "service.h"

typedef gboolean (*ODevicedPluginPluginFunc) (ODevicedPlugin* plugin, void* user_data);



struct _ODevicedPluginPrivate {
	GModule* _library;
	gint _handle;
	char** depends;
	gint depends_length1;
	char* _path;
	char* _name;
};

#define ODEVICED_PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), ODEVICED_TYPE_PLUGIN, ODevicedPluginPrivate))
enum  {
	ODEVICED_PLUGIN_DUMMY_PROPERTY,
	ODEVICED_PLUGIN_LIBRARY,
	ODEVICED_PLUGIN_HANDLE,
	ODEVICED_PLUGIN_PATH,
	ODEVICED_PLUGIN_NAME
};
static void odeviced_plugin_set_path (ODevicedPlugin* self, const char* value);
static void odeviced_plugin_set_name (ODevicedPlugin* self, const char* value);
static GObject * odeviced_plugin_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer odeviced_plugin_parent_class = NULL;
static void odeviced_plugin_dispose (GObject * obj);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);



GQuark odeviced_plugin_error_quark (void) {
	return g_quark_from_static_string ("odeviced_plugin_error-quark");
}


ODevicedPlugin* odeviced_plugin_new (const char* name, const char* path) {
	GParameter * __params;
	GParameter * __params_it;
	ODevicedPlugin * self;
	g_return_val_if_fail (name != NULL, NULL);
	g_return_val_if_fail (path != NULL, NULL);
	__params = g_new0 (GParameter, 2);
	__params_it = __params;
	__params_it->name = "name";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, name);
	__params_it++;
	__params_it->name = "path";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, path);
	__params_it++;
	self = g_object_newv (ODEVICED_TYPE_PLUGIN, __params_it - __params, __params);
	while (__params_it > __params) {
		--__params_it;
		g_value_unset (&__params_it->value);
	}
	g_free (__params);
	return self;
}


gboolean odeviced_plugin_register (ODevicedPlugin* self, GError** error) {
	GError * inner_error;
	GModule* _tmp0;
	gpointer _symbol;
	char* _tmp1;
	gboolean _tmp2;
	ODevicedPluginPluginFunc _tmp3;
	void* func_target;
	ODevicedPluginPluginFunc func;
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (self), FALSE);
	inner_error = NULL;
	_tmp0 = NULL;
	self->priv->_library = (_tmp0 = g_module_open (self->priv->_path, G_MODULE_BIND_MASK), (self->priv->_library == NULL ? NULL : (self->priv->_library = (g_module_close (self->priv->_library), NULL))), _tmp0);
	if (self->priv->_library == NULL) {
		inner_error = g_error_new (ODEVICED_PLUGIN_ERROR, ODEVICED_PLUGIN_ERROR_LOAD_ERROR, "_library is null");
		if (inner_error != NULL) {
			g_propagate_error (error, inner_error);
			return FALSE;
		}
	}
	g_module_make_resident (self->priv->_library);
	_symbol = NULL;
	_tmp1 = NULL;
	if ((_tmp2 = !g_module_symbol (self->priv->_library, (_tmp1 = g_strconcat (self->priv->_name, "_init", NULL)), &_symbol), (_tmp1 = (g_free (_tmp1), NULL)), _tmp2)) {
		inner_error = g_error_new (ODEVICED_PLUGIN_ERROR, ODEVICED_PLUGIN_ERROR_LOAD_ERROR, "Malformed odeviced plugin");
		if (inner_error != NULL) {
			g_propagate_error (error, inner_error);
			return FALSE;
		}
	}
	func = (_tmp3 = ((ODevicedPluginPluginFunc) (_symbol)), func_target = NULL, _tmp3);
	return func (self, func_target);
}


GModule* odeviced_plugin_get_library (ODevicedPlugin* self) {
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (self), NULL);
	return self->priv->_library;
}


gint odeviced_plugin_get_handle (ODevicedPlugin* self) {
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (self), 0);
	return self->priv->_handle;
}


const char* odeviced_plugin_get_path (ODevicedPlugin* self) {
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (self), NULL);
	return self->priv->_path;
}


static void odeviced_plugin_set_path (ODevicedPlugin* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (ODEVICED_IS_PLUGIN (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_path = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_path = (g_free (self->priv->_path), NULL)), _tmp2);
}


const char* odeviced_plugin_get_name (ODevicedPlugin* self) {
	g_return_val_if_fail (ODEVICED_IS_PLUGIN (self), NULL);
	return self->priv->_name;
}


static void odeviced_plugin_set_name (ODevicedPlugin* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (ODEVICED_IS_PLUGIN (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_name = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_name = (g_free (self->priv->_name), NULL)), _tmp2);
}


static GObject * odeviced_plugin_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	ODevicedPluginClass * klass;
	GObjectClass * parent_class;
	ODevicedPlugin * self;
	GError * inner_error;
	klass = ODEVICED_PLUGIN_CLASS (g_type_class_peek (ODEVICED_TYPE_PLUGIN));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = ODEVICED_PLUGIN (obj);
	inner_error = NULL;
	{
		GRand* rand;
		char* _tmp1;
		char* _tmp0;
		char* _tmp2;
		char* _conf;
		rand = g_rand_new ();
		self->priv->_handle = ((gint) (g_rand_int_range (rand, ((gint32) (10)), ((gint32) (99)))));
		_tmp1 = NULL;
		_tmp0 = NULL;
		_tmp2 = NULL;
		_conf = (_tmp2 = g_strconcat ((_tmp1 = g_strconcat ((_tmp0 = g_strconcat (odeviced_service_conf_dir_plugins, "/", NULL)), self->priv->_name, NULL)), ".plugin", NULL), (_tmp1 = (g_free (_tmp1), NULL)), (_tmp0 = (g_free (_tmp0), NULL)), _tmp2);
		{
			g_key_file_load_from_file (self->conf, _conf, G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch7_g_error;
			}
			g_key_file_set_list_separator (self->conf, ',');
		}
		goto __finally7;
		__catch7_g_error:
		{
			GError * error;
			error = inner_error;
			inner_error = NULL;
			{
				g_critical ("plugin.vala:73: Plugin configuration file for %s malformed/not found : %s", self->priv->_name, error->message);
			}
		}
		__finally7:
		;
		(rand == NULL ? NULL : (rand = (g_rand_free (rand), NULL)));
		_conf = (g_free (_conf), NULL);
	}
	return obj;
}


static void odeviced_plugin_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	ODevicedPlugin * self;
	self = ODEVICED_PLUGIN (object);
	switch (property_id) {
		case ODEVICED_PLUGIN_LIBRARY:
		g_value_set_pointer (value, odeviced_plugin_get_library (self));
		break;
		case ODEVICED_PLUGIN_HANDLE:
		g_value_set_int (value, odeviced_plugin_get_handle (self));
		break;
		case ODEVICED_PLUGIN_PATH:
		g_value_set_string (value, odeviced_plugin_get_path (self));
		break;
		case ODEVICED_PLUGIN_NAME:
		g_value_set_string (value, odeviced_plugin_get_name (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void odeviced_plugin_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	ODevicedPlugin * self;
	self = ODEVICED_PLUGIN (object);
	switch (property_id) {
		case ODEVICED_PLUGIN_PATH:
		odeviced_plugin_set_path (self, g_value_get_string (value));
		break;
		case ODEVICED_PLUGIN_NAME:
		odeviced_plugin_set_name (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void odeviced_plugin_class_init (ODevicedPluginClass * klass) {
	odeviced_plugin_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ODevicedPluginPrivate));
	G_OBJECT_CLASS (klass)->get_property = odeviced_plugin_get_property;
	G_OBJECT_CLASS (klass)->set_property = odeviced_plugin_set_property;
	G_OBJECT_CLASS (klass)->constructor = odeviced_plugin_constructor;
	G_OBJECT_CLASS (klass)->dispose = odeviced_plugin_dispose;
	g_object_class_install_property (G_OBJECT_CLASS (klass), ODEVICED_PLUGIN_LIBRARY, g_param_spec_pointer ("library", "library", "library", G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), ODEVICED_PLUGIN_HANDLE, g_param_spec_int ("handle", "handle", "handle", G_MININT, G_MAXINT, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), ODEVICED_PLUGIN_PATH, g_param_spec_string ("path", "path", "path", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (G_OBJECT_CLASS (klass), ODEVICED_PLUGIN_NAME, g_param_spec_string ("name", "name", "name", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}


static void odeviced_plugin_instance_init (ODevicedPlugin * self) {
	self->priv = ODEVICED_PLUGIN_GET_PRIVATE (self);
	self->conf = g_key_file_new ();
}


static void odeviced_plugin_dispose (GObject * obj) {
	ODevicedPlugin * self;
	self = ODEVICED_PLUGIN (obj);
	(self->priv->_library == NULL ? NULL : (self->priv->_library = (g_module_close (self->priv->_library), NULL)));
	self->priv->depends = (_vala_array_free (self->priv->depends, self->priv->depends_length1, ((GDestroyNotify) (g_free))), NULL);
	(self->conf == NULL ? NULL : (self->conf = (g_key_file_free (self->conf), NULL)));
	self->priv->_path = (g_free (self->priv->_path), NULL);
	self->priv->_name = (g_free (self->priv->_name), NULL);
	G_OBJECT_CLASS (odeviced_plugin_parent_class)->dispose (obj);
}


GType odeviced_plugin_get_type (void) {
	static GType odeviced_plugin_type_id = 0;
	if (G_UNLIKELY (odeviced_plugin_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (ODevicedPluginClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) odeviced_plugin_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ODevicedPlugin), 0, (GInstanceInitFunc) odeviced_plugin_instance_init };
		odeviced_plugin_type_id = g_type_register_static (G_TYPE_OBJECT, "ODevicedPlugin", &g_define_type_info, 0);
	}
	return odeviced_plugin_type_id;
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




