 /* 
 * fsod-python-plugin.c
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


#include "fsod-python-plugin.h"
#include <src/fsod.h>
#include <dbus/dbus-glib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

static GObjectClass *parent_class;

#define LOG_DOMAIN "PythonManager"

/* Just initialize the interpreter, nothing much for now */
gboolean fsod_init_python() {

	if (Py_IsInitialized()) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_INFO, "Interpreter already initialized");
		return TRUE;
	}
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_INFO, "Trying to initialize the python plugin system");
	Py_InitializeEx(0);

	if (PyErr_Occurred) {
		PyErr_Print();
		PyErr_Clear();
		//return FALSE;
	}
	
	return TRUE;
}

void fsod_finalize_python() {

	while (PyGC_Collect ())
			;	

	g_log (LOG_DOMAIN, G_LOG_LEVEL_INFO, "Finalizing Python interpreter");
	Py_Finalize ();
}



/*---------------------------------------------------------------*
 * PythonPlugin Class GObject code follow                        *
 *---------------------------------------------------------------*/
#define FSOD_PYTHON_PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), FSOD_TYPE_PYTHON_PLUGIN, FSODPythonPluginPrivate))
enum {
	FSOD_PYTHON_PLUGIN_DUMMY,
	FSOD_PYTHON_PLUGIN_MODULE_NAME,
	FSOD_PYTHON_PLUGIN_SERVICE
};

/* Private members of PythonPlugin class */
struct _FSODPythonPluginPrivate {
	gchar *module_name;
	FSODService *service;
	PyObject *module;  /* Reference to the python module that has been loaded */
	PyObject *objects; /* The list of initialized DBus objects */
};


/* Private methods */
static void fsod_python_plugin_extract_ifaces (FSODPythonPlugin *self);


/* Call this to initialize a new PythonPlugin object */
FSODPythonPlugin* fsod_python_plugin_new (const char *module_name, FSODService *service) {

	GParameter * __params;
	GParameter * __params_it;
	FSODPythonPlugin *self;
	
	g_return_val_if_fail (module_name!=NULL, NULL);
	g_return_val_if_fail (service != NULL, NULL);
	__params = g_new0 (GParameter, 2);
	__params_it = __params;
	__params_it->name = "module_name";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, module_name);
	__params_it++;
	__params_it->name = "service";
	g_value_init (&__params_it->value, G_TYPE_POINTER);
	g_value_set_pointer (&__params_it->value, service);
	__params_it++;
	self = g_object_newv (FSOD_TYPE_PYTHON_PLUGIN, __params_it - __params, __params);
	while (__params_it > __params) {
		--__params_it;
		g_value_unset (&__params_it->value);
	}
	g_free (__params);
	return self;
}



static void fsod_python_plugin_init (FSODPythonPlugin *object) {
	FSODPythonPluginClass *class;
	class = (FSODPythonPluginClass*) (((GTypeInstance*) object)->g_class);
}


static void fsod_python_plugin_finalize (GObject *object) {
	if(((FSODPythonPlugin *)object)->priv->module)
		Py_DECREF(((FSODPythonPlugin *)object)->priv->module);

	if (((FSODPythonPlugin *)object)->priv->objects)
		Py_DECREF(((FSODPythonPlugin *)object)->priv->objects);

	g_object_unref (((FSODPythonPlugin *)object)->priv->service);
	G_OBJECT_CLASS (parent_class)->finalize (object);
}



static void fsod_python_plugin_instance_init (FSODPythonPlugin *self) {
	self->priv = FSOD_PYTHON_PLUGIN_GET_PRIVATE(self);
}


/* The constructor parameter is the path to the python module. Import the python module here */	
static GObject * fsod_python_plugin_constructor (GType type,
						 guint n_construct_properties,
						 GObjectConstructParam * construct_properties)
{
	GObject *object;
	FSODPythonPluginClass *klass;
	FSODPythonPlugin *self;
	GObjectClass *parentclass;
		
	klass = g_type_class_peek (FSOD_TYPE_PYTHON_PLUGIN);
	parentclass = G_OBJECT_CLASS (g_type_class_peek_parent(klass));
	object = parentclass->constructor(type, n_construct_properties, construct_properties);
	self = FSOD_PYTHON_PLUGIN(object);
	
	PyObject *path = NULL, *module_path = NULL;
			
	path = PySys_GetObject ("path");

	/* Add $libdir/fsod/subsystems/Python to python path variable */
	module_path = PyString_FromString (FSOD_PYTHON_PLUGINS_PATH);
	PyList_Insert (path, 0, module_path);
	Py_DECREF (module_path);
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Trying to import %s", self->priv->module_name);
	self->priv->module = PyImport_ImportModule(self->priv->module_name);
	if (self->priv->module == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "Couldn't import %s", self->priv->module_name);
		CHECK_PYERR;
	}
	
	return object;

}	



/* Public Method: Call the factory function in the loaded module */
gboolean fsod_python_plugin_call_factory (FSODPythonPlugin *self) {
	g_return_val_if_fail (FSOD_IS_PYTHON_PLUGIN(self), FALSE);
	
	PyObject *attr_dict;    /* Attribute dictionary of the module */
	PyObject *factory_func; /* PyObject corresponding to the factory function */

	g_print ("%s\n", self->priv->module_name);

	/* Get the attributes of the imported module and get the 'factory' attribute
	   And then check if its callable, failing which log the error and getout */
	attr_dict = PyModule_GetDict (self->priv->module);
	factory_func = PyDict_GetItemString (attr_dict, "factory");
				
	if (factory_func == NULL || !(PyCallable_Check(factory_func))) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG|G_LOG_LEVEL_WARNING,
		       "Factory function not callable. Possible name conflict in %s",
		       self->priv->module_name);
		return FALSE;
	}
	
	self->priv->objects = PyObject_CallObject (factory_func, NULL);
	if (self->priv->objects == NULL || !(PyList_Check(self->priv->objects))) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_WARNING,
		       "Factory returned succesfully, but the return value was not a list",
		       self->priv->module_name);
		return FALSE;
	}
	
	fsod_python_plugin_extract_ifaces (self);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_INFO,
	       "module %s loaded successfully", self->priv->module_name);
	return TRUE;
 	
}


/* Private Method: extract interface string from the object PyList and the corresponding object paths */
static void fsod_python_plugin_extract_ifaces (FSODPythonPlugin *self) {
	g_return_if_fail (FSOD_IS_PYTHON_PLUGIN(self));

	PyObject *attr_dict = NULL;
	PyObject *_iface_str = NULL;
	PyObject *_dbus_path = NULL; 
	PyObject *_dbus_obj = NULL;

	gchar *iface = NULL;
	gchar *dbus_path = NULL;

       	GList *_paths = NULL;

	Py_ssize_t list_len = PyList_Size (self->priv->objects);
	Py_ssize_t i;

	for (i=0; i<list_len; i++) {
		_dbus_obj = PyList_GetItem(self->priv->objects, i);
		_iface_str = PyObject_GetAttrString (_dbus_obj, "interface");

		if (!_iface_str) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG|G_LOG_LEVEL_WARNING,
			       "No 'interface' attribute found. Not fatal but ListObjectsByInterface may not work properly");
			continue; /* Move on with the next object in the list */
		}
		
		/* This is really redundant, iface isnt gonna change. But we will be screwed
		 if the same module exports multiple objects at multiple interfaces
		 See kernel26.py under the odeviced subsystem of frameworkd */
		iface = PyString_AsString(_iface_str);
	

		_dbus_path = PyObject_GetAttrString (_dbus_obj, "path");
		if (!_dbus_path) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG|G_LOG_LEVEL_WARNING,
			       "No 'path' attribute. Not fatal but ListObjectsByInterface may not work properly");
			continue;
		}
		dbus_path = PyString_AsString(_dbus_path);
		
		_paths = g_list_append (_paths, dbus_path);		

		/* FIXME: Is there any chance of a leak here? _paths gets overwritten */
		g_hash_table_insert(python_manager_ifaces, iface, _paths);
		
	}
	
}
		
	

/* Properties follow */
static char* fsod_python_plugin_getmodule_name (FSODPythonPlugin *self) {
	g_return_val_if_fail (FSOD_IS_PYTHON_PLUGIN(self), NULL);
	return self->priv->module_name;
}

static void fsod_python_plugin_set_module_name (FSODPythonPlugin *self,
						const gchar *module_name) {
	g_return_if_fail (FSOD_IS_PYTHON_PLUGIN(self));
	self->priv->module_name = g_strdup(module_name);
}
 
static FSODService* fsod_python_plugin_getservice (FSODPythonPlugin *self) {
	g_return_val_if_fail (FSOD_IS_PYTHON_PLUGIN(self), NULL);
	return self->priv->service; 
}

static void fsod_python_plugin_set_service (FSODPythonPlugin *self, FSODService *service) {
	g_return_if_fail (FSOD_IS_PYTHON_PLUGIN(self));
	self->priv->service = g_object_ref(service);
	g_object_notify ((GObject *)self, "service");
}


static void 
fsod_python_plugin_get_property (GObject * object,
				 guint property_id,
				 GValue * value,
				 GParamSpec * pspec)
{
	FSODPythonPlugin *self;
	self = FSOD_PYTHON_PLUGIN(object);
	switch (property_id) {
	case FSOD_PYTHON_PLUGIN_MODULE_NAME:
		g_value_set_string (value, fsod_python_plugin_getmodule_name(self));
		break;
	case FSOD_PYTHON_PLUGIN_SERVICE:
		g_value_set_pointer (value, fsod_python_plugin_getservice(self));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}

}


static void 
fsod_python_plugin_set_property (GObject * object,
				 guint property_id,
				 const GValue * value,
				 GParamSpec * pspec)
{
	FSODPythonPlugin *self;
	self = FSOD_PYTHON_PLUGIN(object);
	switch (property_id) {
	case FSOD_PYTHON_PLUGIN_MODULE_NAME:
		fsod_python_plugin_set_module_name(self, g_value_get_string(value));
		break;
	case FSOD_PYTHON_PLUGIN_SERVICE:
		fsod_python_plugin_set_service(self, g_value_get_pointer(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}

}

static void fsod_python_plugin_class_init (FSODPythonPluginClass *klass,
					   gpointer class_data)
{
	parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof(FSODPythonPluginPrivate));

	G_OBJECT_CLASS (klass)->get_property = fsod_python_plugin_get_property;
	G_OBJECT_CLASS (klass)->set_property = fsod_python_plugin_set_property;
	G_OBJECT_CLASS (klass)->constructor = fsod_python_plugin_constructor;
	G_OBJECT_CLASS (klass)->finalize = fsod_python_plugin_finalize;

	g_object_class_install_property (G_OBJECT_CLASS(klass), FSOD_PYTHON_PLUGIN_MODULE_NAME,
					 g_param_spec_string ("module-name", "module-name", "module-name", NULL,
							      G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
							      G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
							      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY)
					 );
	g_object_class_install_property (G_OBJECT_CLASS (klass), FSOD_PYTHON_PLUGIN_SERVICE,
					 g_param_spec_pointer ("service", "service", "service",
							       G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
							       G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
							       G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}


GType fsod_python_plugin_get_type (void) {
	static GType gtype = 0;
	if (gtype == 0) {
		GTypeInfo info = {
			sizeof (FSODPythonPluginClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) fsod_python_plugin_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (FSODPythonPlugin),
			0,              /* n_preallocs */
			(GInstanceInitFunc) fsod_python_plugin_instance_init,
		NULL
		};
		gtype = g_type_register_static (G_TYPE_OBJECT, "FSODPythonPlugin",
						&info, 0);
	}
	return gtype;
}
