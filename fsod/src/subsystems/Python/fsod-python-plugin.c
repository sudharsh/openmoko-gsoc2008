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

/* Python plugin loaders, Inspired by gedit-python-plugin.c/h from Gedit project */

#include "fsod-python-plugin.h"
#include <config.h>

static GObjectClass *parent_class;

static void fsod_python_plugin_init (FsodPythonPlugin *object) {
	FsodPythonPluginClass *class;
	class = (FsodPythonPluginClass*) (((GTypeInstance*) object)->g_class);

	object->instance = PyObject_CallObject (class->type, NULL);
	if (object->instance == NULL)
		PyErr_Print();
}

static void fsod_python_plugin_finalize (GObject *object) {
	Py_DECREF (((FsodPythonPlugin *) object)->instance);
	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void fsod_python_plugin_class_init (FsodPythonPluginClass *klass,
					   gpointer class_data)
{
	FsodPythonPluginClass *plugin_class = FSOD_PYTHON_PLUGIN_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);
	klass->type = (PyObject*) class_data;
	G_OBJECT_CLASS (klass)->finalize = fsod_python_plugin_finalize;

}


GType fsod_python_plugin_get_type (GTypeModule *module, PyObject *type) {
	GType gtype;
	gchar *type_name;

	GTypeInfo info = {
		sizeof (FsodPythonPluginClass),
		NULL,           /* base_init */
		NULL,           /* base_finalize */
		(GClassInitFunc) fsod_python_plugin_class_init,
		NULL,           /* class_finalize */
		type,           /* class_data */
		sizeof (FsodPythonPlugin),
		0,              /* n_preallocs */
		(GInstanceInitFunc) fsod_python_plugin_init,
	};
	Py_INCREF (type);
	type_name = g_strdup_printf ("%s+FsodPythonPlugin",
				     PyString_AsString (PyObject_GetAttrString (type, "__name__")));

	gtype = g_type_module_register_type (module, 
					     TYPE_FSOD_PYTHON_PLUGIN,
					     type_name,
					     &info, 0);
	g_free (type_name);

	return gtype;
}