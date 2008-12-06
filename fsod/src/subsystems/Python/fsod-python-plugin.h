/* 
 * fsod-python-plugin.h
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


#ifndef FSOD_PYTHON_OBJECT_H
#define FSOD_PYTHON_OBJECT_H

#include <Python.h>
#include <glib-object.h>
#include <src/fsod.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS


typedef struct _FSODPythonPlugin      FSODPythonPlugin;
typedef struct _FSODPythonPluginClass FSODPythonPluginClass;

/* Private fields, defined in fsod-python-plugin.c */
typedef struct _FSODPythonPluginPrivate FSODPythonPluginPrivate;


/* Some Boiler-plate code */
#define FSOD_TYPE_PYTHON_PLUGIN (fsod_python_plugin_get_type ())
#define FSOD_PYTHON_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), FSOD_TYPE_PYTHON_PLUGIN, FSODPythonPlugin))
#define FSOD_PYTHON_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), FSOD_PYTHON_PLUGIN, FSODPythonPluginClass))
#define FSOD_IS_PYTHON_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FSOD_TYPE_PYTHON_PLUGIN))
#define FSOD_IS_PYTHON_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FSOD_TYPE_PYTHON_PLUGIN))
#define FSOD_PYTHON_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), FSOD_TYPE_PYTHON_PLUGIN, FSODPythonPluginClass))

#define CHECK_PYERR if(PyErrOccured()) { \
		PyErr_Print();		 \
		PyErr_Clear();		 \
	}



/* FSOD.PythonPlugin class declarations */
struct _FSODPythonPlugin {
	GObject parent;
	FSODPythonPluginPrivate *priv;
};

struct _FSODPythonPluginClass {
	GObjectClass parent_class;
};


/* Public methods */
FSODPythonPlugin* fsod_python_plugin_new (const gchar *module_name, FSODService *service);
GType fsod_python_plugin_get_type ();
gboolean fsod_python_plugin_call_factory (FSODPythonPlugin *self);

/* Static functions not related to any objects
   Merely initializes and finalizes the python interpreter for us. Calling once should do */
gboolean fsod_init_python();
void fsod_finalize_python();

G_END_DECLS
#endif
