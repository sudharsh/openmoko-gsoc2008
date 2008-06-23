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

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>

G_BEGIN_DECLS


#define ODEVICED_TYPE_SERVICE (odeviced_service_get_type ())
#define ODEVICED_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ODEVICED_TYPE_SERVICE, ODevicedService))
#define ODEVICED_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ODEVICED_TYPE_SERVICE, ODevicedServiceClass))
#define ODEVICED_IS_SERVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ODEVICED_TYPE_SERVICE))
#define ODEVICED_IS_SERVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ODEVICED_TYPE_SERVICE))
#define ODEVICED_SERVICE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ODEVICED_TYPE_SERVICE, ODevicedServiceClass))

typedef struct _ODevicedService ODevicedService;
typedef struct _ODevicedServiceClass ODevicedServiceClass;
typedef struct _ODevicedServicePrivate ODevicedServicePrivate;

struct _ODevicedService {
	GObject parent_instance;
	ODevicedServicePrivate * priv;
};

struct _ODevicedServiceClass {
	GObjectClass parent_class;
};


extern char* odeviced_service_dev_name;
extern char* odeviced_service_conf_dir_plugins;
gboolean odeviced_service_is_loaded (ODevicedService* self, const char* plugin_name);
void odeviced_service_run (ODevicedService* self);
ODevicedService* odeviced_service_new (void);
GType odeviced_service_get_type (void);


G_END_DECLS

#endif
