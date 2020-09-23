/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "foauth.h"
#include <stdio.h>
#include <curl/curl.h>
#include <net_connection.h>
#include <json-glib/json-glib.h>
#include <app_control.h>

#define CLIENT_TOKEN "<YOUR_CLIENT_TOKEN>"
#define APP_ID "<YOUR_APP_ID>"

typedef struct _user_code_response_s{
	char verification_uri[256];
	int expires_in;
	int interval;
	char device_code[512];
	char code[512];
	char user_code[128];
}user_code_response_s;

typedef struct _access_token_response_s{
	char access_token[1024];
	int expires_in;
}access_token_response_s;

typedef struct _profile_reponse{
	char name[128];
	char email[128];
}profile_response_s;

typedef struct appdata{
	Evas_Object* win;
	Evas_Object* layout;
	Evas_Object* conform;
	Evas_Object* button;
	Ecore_Timer* timer;
	user_code_response_s* user_code_response;
	access_token_response_s* access_token_response;
	profile_response_s* profile_response;
} appdata_s;

unsigned int write_callback(char *ptr, unsigned int size, unsigned int nmemb, void *userdata)
{
	int buffer_size = size * nmemb;
	char* response_json = userdata;

	int response_json_cur_size = strlen(response_json);

	memcpy(response_json + response_json_cur_size, ptr, buffer_size);
	response_json_cur_size += buffer_size;
	response_json[response_json_cur_size] = '\n';

	return buffer_size;
}

void request_user_code(appdata_s* ad)
{
	char request[1024] = {0};
	char response_json[4096] = {0};

	CURL *curl;
	CURLcode res;
	char *proxy_address;
	connection_h connection;
	res=connection_create(&connection);

	sprintf(request, "access_token=%s|%s&scope=public_profile", APP_ID, CLIENT_TOKEN);
	dlog_print(DLOG_INFO, LOG_TAG, "request params = %s", request);

	res = connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
	if( res != CONNECTION_ERROR_NONE)
	{
		dlog_print(DLOG_INFO, LOG_TAG, "connection_get_proxy failed with %d", res);
		return;
	}
	dlog_print(DLOG_INFO, LOG_TAG, "Proxy Addr = %s", proxy_address);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://graph.facebook.com/v2.6/device/login");
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy_address);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_json);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK){
			dlog_print(DLOG_ERROR, LOG_TAG, "curl_easy_perform() failed: %s\n",  curl_easy_strerror(res));
			return;
		}

		curl_easy_cleanup(curl);
	}

	dlog_print(DLOG_INFO, LOG_TAG, "response_json = %s", response_json);

	g_type_init();

	JsonParser* json_parser = json_parser_new();
	json_parser_load_from_data(json_parser, response_json, -1, NULL);

	JsonObject* json_object_root_node = json_node_get_object(json_parser_get_root(json_parser));

	const char* code = json_object_get_string_member(json_object_root_node, "code");
	const char* user_code = json_object_get_string_member(json_object_root_node, "user_code");
	const char* verification_uri = json_object_get_string_member(json_object_root_node, "verification_uri");
	const int expires_in = json_object_get_int_member(json_object_root_node, "expires_in");
	const int interval = json_object_get_int_member(json_object_root_node, "interval");

	dlog_print(DLOG_INFO, LOG_TAG, "code = %s", code);
	dlog_print(DLOG_INFO, LOG_TAG, "user_code = %s", user_code);
	dlog_print(DLOG_INFO, LOG_TAG, "verification_uri = %s", verification_uri);
	dlog_print(DLOG_INFO, LOG_TAG, "expires_in = %d", expires_in);
	dlog_print(DLOG_INFO, LOG_TAG, "interval = %d", interval);

	memset(ad->user_code_response, 0x00, sizeof(user_code_response_s));

	strncpy(ad->user_code_response->code, code, strlen(code));
	strncpy(ad->user_code_response->user_code, user_code, strlen(user_code));
	strncpy(ad->user_code_response->verification_uri, verification_uri, strlen(verification_uri));
	ad->user_code_response->expires_in = expires_in;
	ad->user_code_response->interval = interval;

	if(json_parser){
		g_object_unref(json_parser);
		json_parser = NULL;
	}

	app_control_h app_control;
	app_control_create(&app_control);
	app_control_set_app_id(app_control, "tizen.wearablemanager");
	app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
	app_control_add_extra_data(app_control, "type", "launch-remote-browser");
	app_control_set_uri(app_control, "https://www.facebook.com/device");
	app_control_send_launch_request(app_control, NULL, NULL);
	app_control_destroy(app_control);
}

int request_access_token(appdata_s* ad)
{
	int ret;

	char request[4096] = {0};
	char response_json[4096] = {0};

	CURL *curl;
	CURLcode res;
	char *proxy_address;
	connection_h connection;
	connection_create(&connection);

	sprintf(request, "access_token=%s|%s&code=%s", APP_ID, CLIENT_TOKEN, ad->user_code_response->code);
	dlog_print(DLOG_INFO, LOG_TAG, "request params = %s", request);

	res = connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://graph.facebook.com/v2.6/device/login_status");
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy_address);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_json);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK){
			dlog_print(DLOG_ERROR, LOG_TAG, "curl_easy_perform() failed: %s\n",  curl_easy_strerror(res));
			return 0;
		}

		curl_easy_cleanup(curl);
	}

	dlog_print(DLOG_INFO, LOG_TAG, "response_json = %s", response_json);

	g_type_init();

	JsonParser* json_parser = json_parser_new();
	json_parser_load_from_data(json_parser, response_json, -1, NULL);

	JsonObject* json_object_root_node = json_node_get_object(json_parser_get_root(json_parser));

	JsonNode* error_node = json_object_get_member(json_object_root_node, "error");
	if(error_node == NULL){
		ret = 1;

		const char* access_token = json_object_get_string_member(json_object_root_node, "access_token");
		int expires_in = json_object_get_int_member(json_object_root_node, "expires_in");

		dlog_print(DLOG_INFO, LOG_TAG, "access_token = %s", access_token);
		dlog_print(DLOG_INFO, LOG_TAG, "expires_in = %d", expires_in);

		memset(ad->access_token_response, 0x00, sizeof(access_token_response_s));

		strncpy(ad->access_token_response->access_token, access_token, strlen(access_token));
		ad->access_token_response->expires_in = expires_in;
	}else{
		ret = 0;
	}

	if(json_parser){
		g_object_unref(json_parser);
		json_parser = NULL;
	}

	return ret;
}

int request_profile(appdata_s* ad)
{
	char request[4096] = {0};
	char response_json[4096] = {0};

	CURL *curl;
	CURLcode res;
	char *proxy_address;
	connection_h connection;
	connection_create(&connection);

	sprintf(request, "https://graph.facebook.com/v2.3/me?fields=name,email&access_token=%s", ad->access_token_response->access_token);
	dlog_print(DLOG_INFO, LOG_TAG, "request params = %s", request);

	res = connection_get_proxy(connection, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, request);
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy_address);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_json);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK){
			dlog_print(DLOG_ERROR, LOG_TAG, "curl_easy_perform() failed: %s\n",  curl_easy_strerror(res));
			return 1;
		}

		curl_easy_cleanup(curl);
	}

	dlog_print(DLOG_INFO, LOG_TAG, "response_json = %s", response_json);

	g_type_init();

	JsonParser* json_parser = json_parser_new();
	json_parser_load_from_data(json_parser, response_json, -1, NULL);

	JsonObject* json_object_root_node = json_node_get_object(json_parser_get_root(json_parser));

	const char* name = json_object_get_string_member(json_object_root_node, "name");
	const char* email = json_object_get_string_member(json_object_root_node, "email");

	dlog_print(DLOG_INFO, LOG_TAG, "name = %s", name);
	dlog_print(DLOG_INFO, LOG_TAG, "email = %s", email);

	memset(ad->profile_response, 0x00, sizeof(profile_response_s));

	strncpy(ad->profile_response->name, name, strlen(name));
	strncpy(ad->profile_response->email, email, strlen(email));

	if(json_parser){
		g_object_unref(json_parser);
		json_parser = NULL;
	}

	return 0;
}

Eina_Bool request_access_token_cb(void* data)
{
	int ret;
	char buffer[1024] = {0};

	appdata_s *ad = data;

	dlog_print(DLOG_ERROR, LOG_TAG, "request_access_token_cb");

	ret = request_access_token(ad);
	if(ret == 1){

		request_profile(ad);

		sprintf(buffer, "email : %s", ad->profile_response->email);
		elm_object_part_text_set(ad->layout, "content.textblock", buffer);
		evas_object_show(ad->layout);

		ad->timer = NULL;

		return ECORE_CALLBACK_CANCEL;
	}else{
		return ECORE_CALLBACK_RENEW;
	}
}

static void _onButtonClickedCb(void *data, Evas_Object *obj, void *event_info)
{
	char buffer[1024] = {0};

	appdata_s *ad = data;

	ad->user_code_response = (user_code_response_s*)malloc(sizeof(user_code_response_s));
	ad->access_token_response = (access_token_response_s*)malloc(sizeof(access_token_response_s));
	ad->profile_response = (profile_response_s*)malloc(sizeof(profile_response_s));

	request_user_code(ad);

	sprintf(buffer, "user_code : %s", ad->user_code_response->user_code);

	elm_object_part_text_set(ad->layout, "content.textblock", buffer);
	evas_object_show(ad->layout);

	ad->timer =  ecore_timer_add(ad->user_code_response->interval, request_access_token_cb, ad);

}
static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
layout_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void
app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max)
{
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(edj_path_out, edj_path_max, "%s%s", res_path, edj_file_in);
		free(res_path);
	}
}

static void
create_base_gui(appdata_s *ad)
{
	char edj_path[PATH_MAX] = {0, };

	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_conformant_set(ad->win, EINA_TRUE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Base Layout */
	app_get_resource(EDJ_FILE, edj_path, (int)PATH_MAX);
	ad->layout = elm_layout_add(ad->win);
	elm_layout_file_set(ad->layout, edj_path, GRP_MAIN);
	elm_object_part_text_set(ad->layout, "content.textblock", "Click Confirm");
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(ad->layout, EEXT_CALLBACK_BACK, layout_back_cb, ad);
	elm_object_content_set(ad->conform, ad->layout);

	Evas_Object *btn = elm_button_add(ad->layout);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "Confirm");
	elm_object_part_content_set(ad->layout, "button", btn);
	evas_object_smart_callback_add(btn, "clicked", _onButtonClickedCb, ad);
	evas_object_show(btn);
	ad->button = btn;

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
	   Initialize UI resources and application's data
	   If this function returns true, the main loop of application starts
	   If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);
	ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
	}

	return ret;
}
