#ifndef __helloremoteconsumer_H__
#define __helloremoteconsumer_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <glib.h>
#include <tizen.h>
#include <sys/types.h>
#include <remote_app_control.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "helloremoteconsumer"

#if !defined(PACKAGE)
#define PACKAGE "com.samsung.helloremoteconsumer"
#endif

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *nf;
	Evas_Object *datetime;
	Evas_Object *popup;
	Evas_Object *button;
	Eext_Circle_Surface *circle_surface;
	struct tm saved_time;
} appdata_s;

void request_and_reply_cb(void *data, Evas_Object *obj, void *event_info);
void request_uri_cb(void *data, Evas_Object *obj, void *event_info);

#endif /* __helloremoteconsumer_H__ */
