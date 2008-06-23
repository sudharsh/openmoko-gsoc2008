/* 
 *  service.vala
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

#include "service.h"
#include "plugin.h"




struct _ODevicedServicePrivate {
	GMainLoop* loop;
	GHashTable* loadedTable;
	GKeyFile* conf_file;
	char* plugins_location;
};

#define ODEVICED_SERVICE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), ODEVICED_TYPE_SERVICE, ODevicedServicePrivate))
enum  {
	ODEVICED_SERVICE_DUMMY_PROPERTY
};
char* odeviced_service_dev_name = NULL;
char* odeviced_service_conf_dir_plugins = NULL;
static gboolean odeviced_service_load (ODevicedService* self, const char* plugin_name);
static void odeviced_service_load_multiple (ODevicedService* self, char** plugins, int plugins_length1);
static void odeviced_service_probe_plugins (ODevicedService* self, const char* plugins_path);
static gboolean odeviced_service_timeout (ODevicedService* self);
static gboolean odeviced_service_idle (ODevicedService* self);
static gboolean _odeviced_service_idle_gsource_func (gpointer self);
static gboolean _odeviced_service_timeout_gsource_func (gpointer self);
static GObject * odeviced_service_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer odeviced_service_parent_class = NULL;
static void odeviced_service_dispose (GObject * obj);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);



static gboolean odeviced_service_load (ODevicedService* self, const char* plugin_name) {
	GError * inner_error;
	char* _tmp1;
	char* _tmp0;
	char* _tmp2;
	char* plugin_path;
	gint _deps_length1;
	char** _deps;
	GKeyFile* _plugin_conf;
	gboolean _tmp12;
	g_return_val_if_fail (ODEVICED_IS_SERVICE (self), FALSE);
	g_return_val_if_fail (plugin_name != NULL, FALSE);
	inner_error = NULL;
	_tmp1 = NULL;
	_tmp0 = NULL;
	_tmp2 = NULL;
	plugin_path = (_tmp2 = g_strconcat ((_tmp1 = g_strconcat ((_tmp0 = g_strconcat (self->priv->plugins_location, "/", NULL)), plugin_name, NULL)), ".so", NULL), (_tmp1 = (g_free (_tmp1), NULL)), (_tmp0 = (g_free (_tmp0), NULL)), _tmp2);
	_deps = (_deps_length1 = 0, NULL);
	g_debug ("service.vala:69: No of plugins already loaded: %d", g_hash_table_size (self->priv->loadedTable));
	/* Check the hash table if the plugin exists */
	if (odeviced_service_is_loaded (self, plugin_name)) {
		gboolean _tmp3;
		g_message ("service.vala:73: Plugin already loaded");
		return (_tmp3 = TRUE, (plugin_path = (g_free (plugin_path), NULL)), (_deps = (_vala_array_free (_deps, _deps_length1, ((GDestroyNotify) (g_free))), NULL)), _tmp3);
	}
	_plugin_conf = g_key_file_new ();
	{
		char* _tmp6;
		char* _tmp5;
		char* _tmp4;
		ODevicedPlugin* plugin;
		_tmp6 = NULL;
		_tmp5 = NULL;
		_tmp4 = NULL;
		g_key_file_load_from_file (_plugin_conf, (_tmp6 = g_strconcat ((_tmp5 = g_strconcat ((_tmp4 = g_strconcat (odeviced_service_conf_dir_plugins, "/", NULL)), plugin_name, NULL)), ".plugin", NULL)), G_KEY_FILE_NONE, &inner_error);
		if (inner_error != NULL) {
			if (inner_error->domain == ODEVICED_PLUGIN_ERROR) {
				goto __catch0_odeviced_plugin_error;
			}
			goto __catch0_g_error;
		}
		_tmp6 = (g_free (_tmp6), NULL);
		_tmp5 = (g_free (_tmp5), NULL);
		_tmp4 = (g_free (_tmp4), NULL);
		/* Get dependencies of the plugin and try to load them */
		if (g_key_file_has_group (_plugin_conf, plugin_name) && g_key_file_has_key (_plugin_conf, plugin_name, "depends", &inner_error)) {
			char** _tmp8;
			gint _tmp7;
			_tmp8 = NULL;
			_deps = (_tmp8 = g_key_file_get_string_list (self->priv->conf_file, plugin_name, "depends", &_tmp7, &inner_error), (_deps = (_vala_array_free (_deps, _deps_length1, ((GDestroyNotify) (g_free))), NULL)), _deps_length1 = _tmp7, _tmp8);
			if (inner_error != NULL) {
				if (inner_error->domain == ODEVICED_PLUGIN_ERROR) {
					goto __catch0_odeviced_plugin_error;
				}
				goto __catch0_g_error;
			}
			g_print ("\t%s has dependencies\n", plugin_name);
			odeviced_service_load_multiple (self, _deps, _deps_length1);
			g_print ("\tDone handling dependencies\n", plugin_name);
		}
		plugin = odeviced_plugin_new (plugin_name, plugin_path);
		if (odeviced_plugin_register (plugin, &inner_error)) {
			ODevicedPlugin* _tmp10;
			const char* _tmp9;
			gboolean _tmp11;
			/* This throws up an error in valac atm 
			   plugin.depends = _deps; */
			_tmp10 = NULL;
			_tmp9 = NULL;
			g_hash_table_insert (self->priv->loadedTable, (_tmp9 = plugin_name, (_tmp9 == NULL ? NULL : g_strdup (_tmp9))), (_tmp10 = plugin, (_tmp10 == NULL ? NULL : g_object_ref (_tmp10))));
			g_message ("service.vala:95: Successfully loaded %s\n", plugin_name);
			return (_tmp11 = TRUE, (plugin == NULL ? NULL : (plugin = (g_object_unref (plugin), NULL))), (plugin_path = (g_free (plugin_path), NULL)), (_deps = (_vala_array_free (_deps, _deps_length1, ((GDestroyNotify) (g_free))), NULL)), (_plugin_conf == NULL ? NULL : (_plugin_conf = (g_key_file_free (_plugin_conf), NULL))), _tmp11);
		}
		(plugin == NULL ? NULL : (plugin = (g_object_unref (plugin), NULL)));
	}
	goto __finally0;
	__catch0_odeviced_plugin_error:
	{
		GError * error;
		error = inner_error;
		inner_error = NULL;
		{
			g_message (error->message);
		}
	}
	goto __finally0;
	__catch0_g_error:
	{
		GError * error;
		error = inner_error;
		inner_error = NULL;
		{
			g_warning ("service.vala:105: No configuration file for %s", plugin_name);
		}
	}
	__finally0:
	;
	return (_tmp12 = FALSE, (plugin_path = (g_free (plugin_path), NULL)), (_deps = (_vala_array_free (_deps, _deps_length1, ((GDestroyNotify) (g_free))), NULL)), (_plugin_conf == NULL ? NULL : (_plugin_conf = (g_key_file_free (_plugin_conf), NULL))), _tmp12);
}


static void odeviced_service_load_multiple (ODevicedService* self, char** plugins, int plugins_length1) {
	g_return_if_fail (ODEVICED_IS_SERVICE (self));
	{
		char** plugin_collection;
		int plugin_collection_length1;
		int plugin_it;
		plugin_collection = plugins;
		plugin_collection_length1 = plugins_length1;
		for (plugin_it = 0; (plugins_length1 != -1 && plugin_it < plugins_length1) || (plugins_length1 == -1 && plugin_collection[plugin_it] != NULL); plugin_it = plugin_it + 1) {
			const char* _tmp0;
			char* plugin;
			_tmp0 = NULL;
			plugin = (_tmp0 = plugin_collection[plugin_it], (_tmp0 == NULL ? NULL : g_strdup (_tmp0)));
			{
				if (odeviced_service_is_loaded (self, plugin)) {
					g_print ("\tDependency %s already loaded\n", plugin);
					plugin = (g_free (plugin), NULL);
					continue;
				} else {
					odeviced_service_load (self, plugin);
				}
				plugin = (g_free (plugin), NULL);
			}
		}
	}
}


gboolean odeviced_service_is_loaded (ODevicedService* self, const char* plugin_name) {
	g_return_val_if_fail (ODEVICED_IS_SERVICE (self), FALSE);
	g_return_val_if_fail (plugin_name != NULL, FALSE);
	if (((ODevicedPlugin*) (g_hash_table_lookup (self->priv->loadedTable, plugin_name))) == NULL) {
		return FALSE;
	}
	return TRUE;
}


static void odeviced_service_probe_plugins (ODevicedService* self, const char* plugins_path) {
	GError * inner_error;
	gint _temp_length1;
	char** _temp;
	g_return_if_fail (ODEVICED_IS_SERVICE (self));
	g_return_if_fail (plugins_path != NULL);
	inner_error = NULL;
	g_message ("service.vala:139: Probing plugins at %s\n", plugins_path);
	_temp = (_temp_length1 = 0, NULL);
	{
		GDir* dir;
		const char* _tmp0;
		char* plugin;
		dir = g_dir_open (plugins_path, ((guint) (0)), &inner_error);
		if (inner_error != NULL) {
			if (inner_error->domain == G_FILE_ERROR) {
				goto __catch1_g_file_error;
			}
			g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, inner_error->message);
			g_clear_error (&inner_error);
		}
		_tmp0 = NULL;
		plugin = (_tmp0 = g_dir_read_name (dir), (_tmp0 == NULL ? NULL : g_strdup (_tmp0)));
		while (plugin != NULL) {
			char* _tmp4;
			const char* _tmp3;
			if (g_str_has_suffix (plugin, ".so")) {
				char** _tmp1;
				char* _tmp2;
				_tmp1 = NULL;
				_temp = (_tmp1 = g_strsplit (plugin, ".", 0), (_temp = (_vala_array_free (_temp, _temp_length1, ((GDestroyNotify) (g_free))), NULL)), _temp_length1 = -1, _tmp1);
				_tmp2 = NULL;
				g_message ((_tmp2 = g_strconcat ("Probing ", _temp[0], NULL)));
				_tmp2 = (g_free (_tmp2), NULL);
				odeviced_service_load (self, _temp[0]);
			}
			_tmp4 = NULL;
			_tmp3 = NULL;
			plugin = (_tmp4 = (_tmp3 = g_dir_read_name (dir), (_tmp3 == NULL ? NULL : g_strdup (_tmp3))), (plugin = (g_free (plugin), NULL)), _tmp4);
		}
		(dir == NULL ? NULL : (dir = (g_dir_close (dir), NULL)));
		plugin = (g_free (plugin), NULL);
	}
	goto __finally1;
	__catch1_g_file_error:
	{
		GError * error;
		error = inner_error;
		inner_error = NULL;
		{
			g_warning (error->message);
		}
	}
	__finally1:
	;
	_temp = (_vala_array_free (_temp, _temp_length1, ((GDestroyNotify) (g_free))), NULL);
}


static gboolean odeviced_service_timeout (ODevicedService* self) {
	g_return_val_if_fail (ODEVICED_IS_SERVICE (self), FALSE);
	/*log("", LogLevelFLags.LEVEL_INFO, "Timeout");*/
	g_message ("service.vala:165: timeout");
	return TRUE;
}


static gboolean odeviced_service_idle (ODevicedService* self) {
	g_return_val_if_fail (ODEVICED_IS_SERVICE (self), FALSE);
	/*log("", LogLevelFlags.LEVEL_INFO, "idle");*/
	g_message ("service.vala:172: idle");
	return FALSE;
}


void odeviced_service_run (ODevicedService* self) {
	g_return_if_fail (ODEVICED_IS_SERVICE (self));
	g_main_loop_run (self->priv->loop);
}


ODevicedService* odeviced_service_new (void) {
	ODevicedService * self;
	self = g_object_newv (ODEVICED_TYPE_SERVICE, 0, NULL);
	return self;
}


static gboolean _odeviced_service_idle_gsource_func (gpointer self) {
	return odeviced_service_idle (self);
}


static gboolean _odeviced_service_timeout_gsource_func (gpointer self) {
	return odeviced_service_timeout (self);
}


static GObject * odeviced_service_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	ODevicedServiceClass * klass;
	GObjectClass * parent_class;
	ODevicedService * self;
	GError * inner_error;
	klass = ODEVICED_SERVICE_CLASS (g_type_class_peek (ODEVICED_TYPE_SERVICE));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = ODEVICED_SERVICE (obj);
	inner_error = NULL;
	{
		gint plugins_length1;
		char** plugins;
		g_idle_add (_odeviced_service_idle_gsource_func, self);
		plugins = (plugins_length1 = 0, NULL);
		g_timeout_add_seconds (((guint) (50)), _odeviced_service_timeout_gsource_func, self);
		g_key_file_set_list_separator (self->priv->conf_file, ',');
		{
			/* Hard coded, change later */
			g_key_file_load_from_file (self->priv->conf_file, "/etc/odeviced.conf", G_KEY_FILE_NONE, &inner_error);
			if (inner_error != NULL) {
				goto __catch2_g_error;
			}
			if (g_key_file_has_group (self->priv->conf_file, "odeviced")) {
				char* _tmp0;
				char* _tmp1;
				char* _tmp2;
				_tmp0 = NULL;
				odeviced_service_dev_name = (_tmp0 = g_key_file_get_string (self->priv->conf_file, "odeviced", "device_name", &inner_error), (odeviced_service_dev_name = (g_free (odeviced_service_dev_name), NULL)), _tmp0);
				if (inner_error != NULL) {
					goto __catch2_g_error;
				}
				_tmp1 = NULL;
				self->priv->plugins_location = (_tmp1 = g_key_file_get_string (self->priv->conf_file, "odeviced", "plugins_path", &inner_error), (self->priv->plugins_location = (g_free (self->priv->plugins_location), NULL)), _tmp1);
				if (inner_error != NULL) {
					goto __catch2_g_error;
				}
				_tmp2 = NULL;
				odeviced_service_conf_dir_plugins = (_tmp2 = g_key_file_get_string (self->priv->conf_file, "odeviced", "plugins_conf", &inner_error), (odeviced_service_conf_dir_plugins = (g_free (odeviced_service_conf_dir_plugins), NULL)), _tmp2);
				if (inner_error != NULL) {
					goto __catch2_g_error;
				}
			}
			if (g_key_file_has_key (self->priv->conf_file, "odeviced", "enable", &inner_error)) {
				char** _tmp4;
				gint plugins_length1;
				gint _tmp3;
				char** plugins;
				_tmp4 = NULL;
				plugins = (_tmp4 = g_key_file_get_string_list (self->priv->conf_file, "odeviced", "enable", &_tmp3, &inner_error), plugins_length1 = _tmp3, _tmp4);
				if (inner_error != NULL) {
					goto __catch2_g_error;
				}
				odeviced_service_load_multiple (self, plugins, plugins_length1);
				plugins = (_vala_array_free (plugins, plugins_length1, ((GDestroyNotify) (g_free))), NULL);
			} else {
				odeviced_service_probe_plugins (self, self->priv->plugins_location);
			}
		}
		goto __finally2;
		__catch2_g_error:
		{
			GError * error;
			error = inner_error;
			inner_error = NULL;
			{
				g_print ("Oops %s\n", error->message);
			}
		}
		__finally2:
		;
		plugins = (_vala_array_free (plugins, plugins_length1, ((GDestroyNotify) (g_free))), NULL);
	}
	return obj;
}


static void odeviced_service_class_init (ODevicedServiceClass * klass) {
	odeviced_service_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ODevicedServicePrivate));
	G_OBJECT_CLASS (klass)->constructor = odeviced_service_constructor;
	G_OBJECT_CLASS (klass)->dispose = odeviced_service_dispose;
	odeviced_service_dev_name = g_new0 (char, 1);
	odeviced_service_conf_dir_plugins = g_new0 (char, 1);
}


static void odeviced_service_instance_init (ODevicedService * self) {
	self->priv = ODEVICED_SERVICE_GET_PRIVATE (self);
	self->priv->loop = g_main_loop_new (NULL, FALSE);
	self->priv->loadedTable = g_hash_table_new (((GHashFunc) (g_str_hash)), ((GEqualFunc) (g_str_equal)));
	self->priv->conf_file = g_key_file_new ();
	self->priv->plugins_location = g_new0 (char, 1);
}


static void odeviced_service_dispose (GObject * obj) {
	ODevicedService * self;
	self = ODEVICED_SERVICE (obj);
	(self->priv->loop == NULL ? NULL : (self->priv->loop = (g_main_loop_unref (self->priv->loop), NULL)));
	(self->priv->loadedTable == NULL ? NULL : (self->priv->loadedTable = (g_hash_table_unref (self->priv->loadedTable), NULL)));
	(self->priv->conf_file == NULL ? NULL : (self->priv->conf_file = (g_key_file_free (self->priv->conf_file), NULL)));
	self->priv->plugins_location = (g_free (self->priv->plugins_location), NULL);
	G_OBJECT_CLASS (odeviced_service_parent_class)->dispose (obj);
}


GType odeviced_service_get_type (void) {
	static GType odeviced_service_type_id = 0;
	if (G_UNLIKELY (odeviced_service_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (ODevicedServiceClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) odeviced_service_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ODevicedService), 0, (GInstanceInitFunc) odeviced_service_instance_init };
		odeviced_service_type_id = g_type_register_static (G_TYPE_OBJECT, "ODevicedService", &g_define_type_info, 0);
	}
	return odeviced_service_type_id;
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




