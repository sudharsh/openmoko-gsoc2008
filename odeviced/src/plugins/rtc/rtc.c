/* 
 * rtc.vala
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

#include "rtc.h"
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include "helpers.h"
#include "odeviced.h"
#include <dbus/dbus-glib.h>




struct _RealTimeClockPrivate {
	char* name;
	char* dev;
	GKeyFile* conf;
	char* _node;
	char* _dbus_path;
};

#define REAL_TIME_CLOCK_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_REAL_TIME_CLOCK, RealTimeClockPrivate))
enum  {
	REAL_TIME_CLOCK_DUMMY_PROPERTY,
	REAL_TIME_CLOCK_NODE,
	REAL_TIME_CLOCK_DBUS_PATH
};
static RealTimeClock* real_time_clock_new (const char* node, const char* dbus_path);
static void real_time_clock_set_node (RealTimeClock* self, const char* value);
static void real_time_clock_set_dbus_path (RealTimeClock* self, const char* value);
static GObject * real_time_clock_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties);
static gpointer real_time_clock_parent_class = NULL;
static gboolean _dbus_real_time_clock_GetName (RealTimeClock* self, char** result, GError** error);
static gboolean _dbus_real_time_clock_GetCurrentTime (RealTimeClock* self, char** result, GError** error);
static gboolean _dbus_real_time_clock_GetWakeupTime (RealTimeClock* self, char** result, GError** error);
static gboolean _dbus_real_time_clock_SetCurrentTime (RealTimeClock* self, const char* seconds, GError** error);
static void real_time_clock_dispose (GObject * obj);
static void register_dbus (RealTimeClock* obj);


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);
static void g_cclosure_user_marshal_BOOLEAN__STRING_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

static RealTimeClock* real_time_clock_new (const char* node, const char* dbus_path) {
	GParameter * __params;
	GParameter * __params_it;
	RealTimeClock * self;
	g_return_val_if_fail (node != NULL, NULL);
	g_return_val_if_fail (dbus_path != NULL, NULL);
	__params = g_new0 (GParameter, 2);
	__params_it = __params;
	__params_it->name = "node";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, node);
	__params_it++;
	__params_it->name = "dbus-path";
	g_value_init (&__params_it->value, G_TYPE_STRING);
	g_value_set_string (&__params_it->value, dbus_path);
	__params_it++;
	self = g_object_newv (TYPE_REAL_TIME_CLOCK, __params_it - __params, __params);
	while (__params_it > __params) {
		--__params_it;
		g_value_unset (&__params_it->value);
	}
	g_free (__params);
	return self;
}


char* real_time_clock_GetName (RealTimeClock* self) {
	const char* _tmp0;
	g_return_val_if_fail (IS_REAL_TIME_CLOCK (self), NULL);
	_tmp0 = NULL;
	return (_tmp0 = self->priv->name, (_tmp0 == NULL ? NULL : g_strdup (_tmp0)));
}


char* real_time_clock_GetCurrentTime (RealTimeClock* self) {
	char* _tmp0;
	char* _tmp1;
	g_return_val_if_fail (IS_REAL_TIME_CLOCK (self), NULL);
	_tmp0 = NULL;
	_tmp1 = NULL;
	return (_tmp1 = odeviced_read_string ((_tmp0 = g_strconcat (self->priv->_node, "/since_epoch", NULL))), (_tmp0 = (g_free (_tmp0), NULL)), _tmp1);
}


char* real_time_clock_GetWakeupTime (RealTimeClock* self) {
	char* ret;
	int fd = 0;
	struct rtc_wkalrm alarm;
	
	g_return_val_if_fail (IS_REAL_TIME_CLOCK (self), NULL);
	ret = NULL;

	fd = open("/dev/rtc", O_RDONLY);
	if (fd == -1) {
		g_message("Couldn't open rtc device");
		return "0";
	}
	
	if(ioctl(fd, RTC_WKALM_RD, &alarm) == 0) 
		ret = g_strdup_printf("%d", (alarm.time.tm_sec + (60 * alarm.time.tm_min) + (60 * 60 * alarm.time.tm_hour)));
	
	close (fd);
	return ret;
}


void real_time_clock_SetCurrentTime (RealTimeClock* self, const char* seconds) {
	gint fd;
	g_return_if_fail (IS_REAL_TIME_CLOCK (self));
	g_return_if_fail (seconds != NULL);
	fd = 0;
	struct rtc_time time;
	fd = open("/dev/rtc", O_RDONLY);
	if (fd == -1) 
		g_message ("Couldn't open rtc device");
	
	time.tm_sec = g_printf("%s", seconds);
	
	ioctl(fd, RTC_SET_TIME, &time);
	
	close (fd);
}


const char* real_time_clock_get_node (RealTimeClock* self) {
	g_return_val_if_fail (IS_REAL_TIME_CLOCK (self), NULL);
	return self->priv->_node;
}


static void real_time_clock_set_node (RealTimeClock* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_REAL_TIME_CLOCK (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_node = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_node = (g_free (self->priv->_node), NULL)), _tmp2);
}


const char* real_time_clock_get_dbus_path (RealTimeClock* self) {
	g_return_val_if_fail (IS_REAL_TIME_CLOCK (self), NULL);
	return self->priv->_dbus_path;
}


static void real_time_clock_set_dbus_path (RealTimeClock* self, const char* value) {
	char* _tmp2;
	const char* _tmp1;
	g_return_if_fail (IS_REAL_TIME_CLOCK (self));
	_tmp2 = NULL;
	_tmp1 = NULL;
	self->priv->_dbus_path = (_tmp2 = (_tmp1 = value, (_tmp1 == NULL ? NULL : g_strdup (_tmp1))), (self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL)), _tmp2);
}


static GObject * real_time_clock_constructor (GType type, guint n_construct_properties, GObjectConstructParam * construct_properties) {
	GObject * obj;
	RealTimeClockClass * klass;
	GObjectClass * parent_class;
	RealTimeClock * self;
	klass = REAL_TIME_CLOCK_CLASS (g_type_class_peek (TYPE_REAL_TIME_CLOCK));
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));
	obj = parent_class->constructor (type, n_construct_properties, construct_properties);
	self = REAL_TIME_CLOCK (obj);
	{
		char* _tmp0;
		_tmp0 = NULL;
		self->priv->name = (_tmp0 = odeviced_compute_name (self->priv->_dbus_path), (self->priv->name = (g_free (self->priv->name), NULL)), _tmp0);
	}
	return obj;
}


static void real_time_clock_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	RealTimeClock * self;
	self = REAL_TIME_CLOCK (object);
	switch (property_id) {
		case REAL_TIME_CLOCK_NODE:
		g_value_set_string (value, real_time_clock_get_node (self));
		break;
		case REAL_TIME_CLOCK_DBUS_PATH:
		g_value_set_string (value, real_time_clock_get_dbus_path (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void real_time_clock_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	RealTimeClock * self;
	self = REAL_TIME_CLOCK (object);
	switch (property_id) {
		case REAL_TIME_CLOCK_NODE:
		real_time_clock_set_node (self, g_value_get_string (value));
		break;
		case REAL_TIME_CLOCK_DBUS_PATH:
		real_time_clock_set_dbus_path (self, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static gboolean _dbus_real_time_clock_GetName (RealTimeClock* self, char** result, GError** error) {
	*result = real_time_clock_GetName (self);
	return !error || !*error;
}


static gboolean _dbus_real_time_clock_GetCurrentTime (RealTimeClock* self, char** result, GError** error) {
	*result = real_time_clock_GetCurrentTime (self);
	return !error || !*error;
}


static gboolean _dbus_real_time_clock_GetWakeupTime (RealTimeClock* self, char** result, GError** error) {
	*result = real_time_clock_GetWakeupTime (self);
	return !error || !*error;
}


static gboolean _dbus_real_time_clock_SetCurrentTime (RealTimeClock* self, const char* seconds, GError** error) {
	real_time_clock_SetCurrentTime (self, seconds);
	return !error || !*error;
}


static void real_time_clock_class_init (RealTimeClockClass * klass) {
	real_time_clock_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RealTimeClockPrivate));
	G_OBJECT_CLASS (klass)->get_property = real_time_clock_get_property;
	G_OBJECT_CLASS (klass)->set_property = real_time_clock_set_property;
	G_OBJECT_CLASS (klass)->constructor = real_time_clock_constructor;
	G_OBJECT_CLASS (klass)->dispose = real_time_clock_dispose;
	g_object_class_install_property (G_OBJECT_CLASS (klass), REAL_TIME_CLOCK_NODE, g_param_spec_string ("node", "node", "node", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (G_OBJECT_CLASS (klass), REAL_TIME_CLOCK_DBUS_PATH, g_param_spec_string ("dbus-path", "dbus-path", "dbus-path", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	static const DBusGMethodInfo real_time_clock_dbus_methods[] = {
{ (GCallback) _dbus_real_time_clock_GetName, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 0 },
{ (GCallback) _dbus_real_time_clock_GetCurrentTime, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 66 },
{ (GCallback) _dbus_real_time_clock_GetWakeupTime, g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER, 139 },
{ (GCallback) _dbus_real_time_clock_SetCurrentTime, g_cclosure_user_marshal_BOOLEAN__STRING_POINTER, 211 },
}
;
	static const DBusGObjectInfo real_time_clock_dbus_object_info = { 0, real_time_clock_dbus_methods, 4, "org.freesmartphone.Device.RealTimeClock\0GetName\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.RealTimeClock\0GetCurrentTime\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.RealTimeClock\0GetWakeupTime\0S\0result\0O\0F\0N\0s\0\0org.freesmartphone.Device.RealTimeClock\0SetCurrentTime\0S\0seconds\0I\0s\0\0", "", "" };
	dbus_g_object_type_install_info (TYPE_REAL_TIME_CLOCK, &real_time_clock_dbus_object_info);
}


static void real_time_clock_instance_init (RealTimeClock * self) {
	self->priv = REAL_TIME_CLOCK_GET_PRIVATE (self);
	self->priv->name = g_new0 (char, 1);
	self->priv->dev = g_new0 (char, 1);
	self->priv->conf = g_key_file_new ();
}


static void real_time_clock_dispose (GObject * obj) {
	RealTimeClock * self;
	self = REAL_TIME_CLOCK (obj);
	self->priv->name = (g_free (self->priv->name), NULL);
	self->priv->dev = (g_free (self->priv->dev), NULL);
	(self->priv->conf == NULL ? NULL : (self->priv->conf = (g_key_file_free (self->priv->conf), NULL)));
	self->priv->_node = (g_free (self->priv->_node), NULL);
	self->priv->_dbus_path = (g_free (self->priv->_dbus_path), NULL);
	G_OBJECT_CLASS (real_time_clock_parent_class)->dispose (obj);
}


GType real_time_clock_get_type (void) {
	static GType real_time_clock_type_id = 0;
	if (G_UNLIKELY (real_time_clock_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (RealTimeClockClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) real_time_clock_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RealTimeClock), 0, (GInstanceInitFunc) real_time_clock_instance_init };
		real_time_clock_type_id = g_type_register_static (G_TYPE_OBJECT, "RealTimeClock", &g_define_type_info, 0);
	}
	return real_time_clock_type_id;
}


static void register_dbus (RealTimeClock* obj) {
	g_return_if_fail (IS_REAL_TIME_CLOCK (obj));
	g_message ("rtc.vala:134: Registering DBus object at %s", real_time_clock_get_dbus_path (obj));
	dbus_g_connection_register_g_object (odeviced_connection, real_time_clock_get_dbus_path (obj), G_OBJECT (obj));
}


G_MODULE_EXPORT gboolean rtc_init (ODevicedPlugin *plugin) {
	GType type;
	GList *list = NULL;
	RealTimeClock *obj;
	type = real_time_clock_get_type();
	list = odeviced_compute_objects (plugin, type);
	if(!list)
		return FALSE;
	g_list_foreach(list, (GFunc)register_dbus, NULL);
	
	return TRUE;
}


static void g_cclosure_user_marshal_BOOLEAN__POINTER_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__POINTER_POINTER) (gpointer data1, gpointer arg_1, gpointer arg_2, gpointer data2);
	register GMarshalFunc_BOOLEAN__POINTER_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) (closure));
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 3);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__POINTER_POINTER) ((marshal_data ? marshal_data : cc->callback)));
	v_return = callback (data1, g_value_get_pointer (param_values + 1), g_value_get_pointer (param_values + 2), data2);
	g_value_set_boolean (return_value, v_return);
}


static void g_cclosure_user_marshal_BOOLEAN__STRING_POINTER (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef gboolean (*GMarshalFunc_BOOLEAN__STRING_POINTER) (gpointer data1, const char* arg_1, gpointer arg_2, gpointer data2);
	register GMarshalFunc_BOOLEAN__STRING_POINTER callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	gboolean v_return;
	cc = ((GCClosure *) (closure));
	g_return_if_fail (return_value != NULL);
	g_return_if_fail (n_param_values == 3);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = ((GMarshalFunc_BOOLEAN__STRING_POINTER) ((marshal_data ? marshal_data : cc->callback)));
	v_return = callback (data1, g_value_get_string (param_values + 1), g_value_get_pointer (param_values + 2), data2);
	g_value_set_boolean (return_value, v_return);
}



