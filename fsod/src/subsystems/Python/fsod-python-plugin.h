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

G_BEGIN_DECLS


#define FSOD_TYPE_PYTHON_PLUGIN (fsod_python_object_get_type ())
#define FSOD_PYTHON_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), FSOD_TYPE_PYTHON_PLUGIN, FsodPythonPlugin))
#define FSOD_PYTHON_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), FSOD_PYTHON_PLUGIN, FsodPythonPluginClass))
#define FSOD_IS_PYTHON_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FSOD_TYPE_PYTHON_PLUGIN))
#define FSOD_IS_PYTHON_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FSOD_TYPE_PYTHON_PLUGIN))
#define FSOD_PYTHON_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), FSOD_TYPE_PYTHON_PLUGIN, FsodPythonPluginClass))


typedef struct _FsodPythonPlugin      FsodPythonPlugin;
typedef struct _FsodPythonPluginClass FsodPythonPluginClass;


struct _FsodPythonPlugin {
	GObject parent;
	PyObject *instance;
};

struct _FsodPythonPluginClass {
	GObjectClass parent_class;
	PyObject *type;
};


GType fsod_python_plugin_get_type (GTypeModule *module, PyObject *type);

G_END_DECLS
#endif

