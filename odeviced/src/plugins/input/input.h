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

#ifndef __INPUT_H__
#define __INPUT_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <src/daemon/plugin.h>

G_BEGIN_DECLS


#define TYPE_INPUT (input_get_type ())
#define INPUT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_INPUT, Input))
#define INPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_INPUT, InputClass))
#define IS_INPUT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_INPUT))
#define IS_INPUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_INPUT))
#define INPUT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_INPUT, InputClass))

typedef struct _Input Input;
typedef struct _InputClass InputClass;
typedef struct _InputPrivate InputPrivate;

struct _Input {
	GObject parent_instance;
	InputPrivate * priv;
};

struct _InputClass {
	GObjectClass parent_class;
};


Input* input_new (void);
GType input_get_type (void);
extern Input* input_obj;
gboolean input_init (ODevicedPlugin* plugin);


G_END_DECLS

#endif
