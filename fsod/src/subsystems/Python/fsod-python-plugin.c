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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

static GObjectClass *parent_class;

/* Just initialize the interpreter, nothing much for now */
gboolean fsod_init_python() {	
	if (Py_IsInitialized()) {
		g_log ("Python", G_LOG_LEVEL_INFO, "Interpreter already initialized");
		return TRUE;
	}
	
	g_log ("Python", G_LOG_LEVEL_INFO, "Trying to initialize the python plugin system");
	Py_Initialize();
	return TRUE;
}

void fsod_finalize_python() {
	while (PyGC_Collect ())
			;	
	g_log ("Python", G_LOG_LEVEL_INFO, "Finalizing Python interpreter");
	Py_Finalize ();
}

/* PythonPlugin Class GObject code follow */
#define FSOD_PYTHON_PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), FSOD_TYPE_PYTHON_PLUGIN, FSODPythonPluginPrivate))
enum {
	FSOD_PYTHON_PLUGIN_DUMMY,
	FSOD_PYTHON_PLUGIN_MODULE_NAME
};

/* Private members of PythonPlugin class */
struct _FSODPythonPluginPrivate {
	gchar *_module_name;
	PyObject *module;
};


/* Call this to initialize a new PythonPlugin object */
FSODPythonPlugin* fsod_python_plugin_new (const char *module_name) {
	GParameter *param;
	FSODPythonPlugin *self;

	/* Return NULL if modules_path is NULL */
	g_return_val_if_fail (module_name!=NULL, NULL);
	param = g_new0 (GParameter, 1);
	      
	param->name = "module_name";
	g_value_init (&param->value, G_TYPE_STRING);
	g_value_set_string (&param->value, module_name);
	
	self = g_object_newv (FSOD_TYPE_PYTHON_PLUGIN, 1, param);

	g_value_unset(&param->value);
	g_free(param);
	
	return self;

}



static void fsod_python_plugin_init (FSODPythonPlugin *object) {
	FSODPythonPluginClass *class;
	class = (FSODPythonPluginClass*) (((GTypeInstance*) object)->g_class);
}


static void fsod_python_plugin_finalize (GObject *object) {
	if(((FSODPythonPlugin *)object)->priv->module)
		Py_DECREF(((FSODPythonPlugin *)object)->priv->module);
	G_OBJECT_CLASS (parent_class)->finalize (object);
}



static void fsod_python_plugin_instance_init (FSODPythonPlugin *self) {
	self->priv = FSOD_PYTHON_PLUGIN_GET_PRIVATE(self);
}


/* Load the modules here, the constructor parameter is the path to the python module */	
static GObject * fsod_python_plugin_constructor (GType type,
						 guint n_construct_properties,
						 GObjectConstructParam * construct_properties)
{
	GObject *object;
	FSODPythonPluginClass *klass;
	GObjectClass *parentclass;
	FSODPythonPlugin *self;

	PyObject *path = NULL, *module_path = NULL;
	PyObject *dict = NULL, *func = NULL;
			
	klass = g_type_class_peek (FSOD_TYPE_PYTHON_PLUGIN);
	parentclass = G_OBJECT_CLASS (g_type_class_peek_parent(klass));
	object = parentclass->constructor(type, n_construct_properties, construct_properties);
	self = FSOD_PYTHON_PLUGIN (object);
	
	/* Try to load the python module priv->_module_name */
	path = PySys_GetObject ("path");

	/* Add $libdir/fsod/subsystems/Python to python path variable */
	module_path = PyString_FromString (FSOD_PYTHON_PLUGINS_PATH);
	PyList_Insert (path, 0, module_path);
	Py_DECREF (module_path);
	
	g_message ("\tTrying to import %s", self->priv->_module_name);
	self->priv->module = PyImport_ImportModule(self->priv->_module_name);
	if (self->priv->module == NULL) {
		g_log ("PythonManager", G_LOG_LEVEL_WARNING, "Couldn't import %s", self->priv->_module_name);
		goto pyerr_occurred;
	}
		
	dict = PyModule_GetDict (self->priv->module);
	func = PyDict_GetItemString (dict, "factory");
			
	if (func==NULL || !(PyCallable_Check(func))) {
		g_log ("PythonManager", G_LOG_LEVEL_WARNING, "Factory function not callable. Possible name conflict in %s",
		       self->priv->_module_name);
		goto pyerr_occurred;
	}

	PyObject_CallObject (func, NULL);

 pyerr_occurred:
	if(PyErr_Occurred()) {
		PyErr_Print();
		PyErr_Clear();
	}

 	return object;
}


/* Properties follow */
static char* fsod_python_plugin_get_module_name (FSODPythonPlugin *self) {
	g_return_val_if_fail (FSOD_IS_PYTHON_PLUGIN(self), NULL);
	return self->priv->_module_name;
}

static void fsod_python_plugin_set_module_name (FSODPythonPlugin *self,
						const gchar *module_name) {
	g_return_if_fail (FSOD_IS_PYTHON_PLUGIN(self));
	self->priv->_module_name = g_strdup(module_name);
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
		g_value_set_string (value, fsod_python_plugin_get_module_name(self));
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
