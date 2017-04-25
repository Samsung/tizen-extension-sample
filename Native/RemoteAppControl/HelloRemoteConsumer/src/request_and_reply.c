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

#include "helloremoteconsumer.h"

static void
remote_request_and_reply_cb(remote_app_control_h request, remote_app_control_h reply, remote_app_control_result_e result, void * user_data)
{
	char *receive_data = NULL;

	dlog_print(DLOG_INFO, LOG_TAG, "reply_cb: result(%d)", result);
	Evas_Object *label = (Evas_Object *) user_data;
	if (result == REMOTE_APP_CONTROL_RESULT_SUCCEEDED)
	{
		remote_app_control_get_data(reply, &receive_data);
		if (receive_data)
		{
			char buf[1024];
			snprintf(buf, 1023,"<br><br><br><align=center>%s</align><br>", receive_data);
			dlog_print(DLOG_INFO, LOG_TAG, "remote_app_control_and_reply_cb: %s", receive_data);
			elm_object_text_set(label, buf);
		}
	}
	else if (result == REMOTE_APP_CONTROL_RESULT_FAILED)
	{
		elm_object_text_set(label,
				"<br><br><br><align=center>FAILED!</align><br>");
	}
	else if (result == REMOTE_APP_CONTROL_RESULT_APP_NOT_FOUND)
	{
		elm_object_text_set(label,
				"<br><br><br><align=center>APP NOT FOUND!</align><br>");
	}
	else
	{
		elm_object_text_set(label,
				"<br><br><br><align=center>ERROR!</align><br>");
	}

	// Show the label
	evas_object_show(label);

	free(receive_data);
}

static void
btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "remote_app_control request");
	Evas_Object *label = (Evas_Object *)data;
	remote_app_control_h remote;
	remote_app_control_create(&remote);
	remote_app_control_set_application(remote, "com.samsung.android.sdk.accessory.example.helloremoteprovider.RequestAndReplyActivity");
	remote_app_control_set_package(remote, "com.samsung.android.sdk.accessory.example.helloremoteprovider");
	remote_app_control_set_data(remote, "This data from gear application");

	if(remote_app_control_send_launch_request(remote, remote_request_and_reply_cb, data) == REMOTE_APP_CONTROL_ERROR_NOT_CONNECTED){
		elm_object_text_set(label, "<br><br><br><align=center>NOT CONNECTED!</align><br>");
	}
	remote_app_control_destroy(remote);
}

void
request_and_reply_cb(void *data, Evas_Object * obj, void *event_info)
{
	appdata_s *ad = (appdata_s *)data;
	Evas_Object *nf = ad->nf;
	Evas_Object *ly, *btn, *label;
	Elm_Object_Item *nf_it;

	ly = elm_button_add(nf);
	elm_layout_theme_set(ly, "layout", "bottom_button", "default");
	evas_object_show(ly);

	label = elm_label_add(nf);
	elm_object_style_set(label, "default");
	elm_object_text_set(label,"<br><br><br><align=center>Hello Remote!</align><br>");
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, 0.5);
	elm_object_part_content_set(ly, "elm.swallow.content", label);

	evas_object_show(label);

	btn = elm_button_add(nf);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "REQUEST");
	elm_object_part_content_set(ly, "elm.swallow.button", btn);
	evas_object_smart_callback_add(btn, "clicked", btn_clicked_cb, label);
	evas_object_show(btn);

	nf_it = elm_naviframe_item_push(nf, "Button", NULL, NULL, ly, "empty");
}

