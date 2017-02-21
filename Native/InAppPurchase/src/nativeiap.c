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

#include "nativeiap.h"
#include <iap_galaxyapps.h>
#include <device/display.h>

#define ICON_DIR "/opt/usr/apps/org.tizen.nativeiaptest/res"

#define UI_STR_LEN	512
char foreach_str[UI_STR_LEN*2] = {0,};

static bool successfully_get_item_list = false;
int total_item_num = 0;
int end_item_num = 0;
static int selected_index = 0;

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *naviframe;
	Evas_Object *rect[10];
	Eext_Circle_Surface *circle_surface;
	Evas_Object *circle_genlist;
} appdata_s;

typedef struct _item_data {
	int index;
	Elm_Object_Item *item;
} item_data;

static appdata_s *object;
static Evas_Object *getting_item_list_popup;
char *main_menu_names[20] = {0,};

static void create_list_view(appdata_s *ad);
void update_ui(char *data);
static void _show_popup_text(appdata_s *ad, char *string);
void get_purchased_item_list_cb(iap_galaxyapps_h reply, iap_galaxyapps_error_e result, void *user_data);


static void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "win_delete_request_cb");
	ui_app_exit();
}

static void _popup_hide_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "_popup_hide_cb");
	if(!obj) return;
	if (successfully_get_item_list)
		elm_popup_dismiss(obj);
	else
		ui_app_exit();
}

static void _popup_hide_finished_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "_popup_hide_finished_cb");
	if (obj) evas_object_del(obj);
}

static void _response_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "_response_cb");
	if(!data) return;
	if (successfully_get_item_list)
		elm_popup_dismiss(data);
	else
		ui_app_exit();
}

///////////// GET ITEM LIST /////////////
bool
foreach_item(iap_galaxyapps_h handle, void *user_data)
{
	static int count = 0;
	output_data value = {0,};

	iap_galaxyapps_get_value(handle, "mItemId", &value.mItemId);
	main_menu_names[count] = strdup(value.mItemId);
	dlog_print(DLOG_INFO, LOG_TAG, "[%d/%d] %s", ++count, total_item_num, value.mItemId);

	if (count == total_item_num) {	// create List
		total_item_num += 1;	// Add an invalid item for negative test
		main_menu_names[count] = strdup("Invalid Item");

		create_list_view(object);
		count = 0;
	}
	return true;
}

void
get_item_list_cb(iap_galaxyapps_h reply, iap_galaxyapps_error_e result, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "get_item_list_cb");
	char ui_statement[UI_STR_LEN] = {0,};

	if (result != IAP_GALAXYAPPS_ERROR_NONE) {
		char *mErrorString = NULL;
		iap_galaxyapps_get_value(reply, "mErrorString", &mErrorString);
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get item list. ret=%d, %s", result, mErrorString);
		snprintf(ui_statement, UI_STR_LEN, "[FAIL] GetItemList : %s",mErrorString);
		update_ui(ui_statement);
		return;
	}

	// Get TotalCount
	char *mTotalCount = NULL;
	iap_galaxyapps_get_value(reply, "mTotalCount", &mTotalCount);
	total_item_num = atoi(mTotalCount);

	int ret = iap_galaxyapps_foreach_item_info(reply, foreach_item, NULL);
	if (ret != IAP_GALAXYAPPS_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to foreach item list. ret=%d", ret);
		snprintf(ui_statement, UI_STR_LEN, "[FAIL] GetItemList : Foreach");
		update_ui(ui_statement);
		return;
	}
	successfully_get_item_list = true;
	return;
}

static void _get_items_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "START getting item list");

	// 1. show "getting item list..." popup
	if (data) elm_popup_dismiss(data);
	_show_popup_text(object, "Getting item list...");

	// 2. request to get avaialble item list
#if 1
	int ret = iap_galaxyapps_get_item_list(1, 10, "10", IAP_GALAXYAPPS_COMMERCIAL_MODE, get_item_list_cb, NULL);
#else
	int ret = iap_galaxyapps_get_item_list(1, 10, "10", IAP_GALAXYAPPS_SUCCESS_TEST_MODE, get_item_list_cb, NULL);
#endif

	if (ret != IAP_GALAXYAPPS_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get item list. ret=%d", ret);
		update_ui("[FAIL] GetItemList");
	}
}

///////////// PURCHASE /////////////
void
purchase_cb(iap_galaxyapps_h reply, iap_galaxyapps_error_e result, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "purchase_cb");
	output_data value = {0,};
	char ui_statement[UI_STR_LEN] = {0,};

	if (result != IAP_GALAXYAPPS_ERROR_NONE) {
		iap_galaxyapps_get_value(reply, "mErrorString", &value.mErrorString);
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to purchase item. ret=%d, %s", result, value.mErrorString);
		snprintf(ui_statement, UI_STR_LEN, "[FAIL] Purchase : %s", value.mErrorString);
		update_ui(ui_statement);
		return;
	}

	iap_galaxyapps_get_value(reply, "mItemId", &value.mItemId);
	iap_galaxyapps_get_value(reply, "mItemPriceString", &value.mItemPriceString);
	iap_galaxyapps_get_value(reply, "mItemDesc", &value.mItemDesc);
	dlog_print(DLOG_INFO, LOG_TAG, "Successfully purchase an item. %s | %s | %s", value.mItemId, value.mItemPriceString, value.mItemDesc);
	snprintf(ui_statement, UI_STR_LEN, "[Purchase]<br>ID: %s<br>Price: %s<br>Desc: %s", value.mItemId, value.mItemPriceString, value.mItemDesc);
	update_ui(ui_statement);

	return;
}

static void _purchase_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "Cancel purchase");
	if(!data) return;
	elm_popup_dismiss(data);
}

static void _purchase_start_cb(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "Start purchase");
	if(data) elm_popup_dismiss(data);
#if 1
	int ret = iap_galaxyapps_start_payment(main_menu_names[selected_index], IAP_GALAXYAPPS_COMMERCIAL_MODE, purchase_cb, NULL);
#else
	int ret = iap_galaxyapps_start_payment(main_menu_names[selected_index], IAP_GALAXYAPPS_SUCCESS_TEST_MODE, purchase_cb, NULL);
#endif
	if (ret != IAP_GALAXYAPPS_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to purchase item. ret=%d", ret);
		update_ui("[FAIL] Purchase");
	}
}

///////////// GET PURCHASED ITEM LIST /////////////
bool
foreach_purchased_item(iap_galaxyapps_h handle, void *user_data) {
	static int count = 1;
	output_data value = {0,};
	char tmp[UI_STR_LEN] = {0,};

	iap_galaxyapps_get_value(handle, "mTotalCount", &value.mTotalCount);
	iap_galaxyapps_get_value(handle, "mItemId", &value.mItemId);
	iap_galaxyapps_get_value(handle, "mItemPriceString", &value.mItemPriceString);

	snprintf(tmp, UI_STR_LEN, "[%d/%s]%s-%s<br>", count, value.mTotalCount, value.mItemId, value.mItemPriceString);
	dlog_print(DLOG_INFO, LOG_TAG, ">> %s", tmp);
	strncat(foreach_str, tmp, strlen(tmp));

	if (count == atoi(value.mTotalCount)) {
		update_ui(foreach_str);
		memset(foreach_str, '\0', UI_STR_LEN*2);
		count = 1;
	} else {
		++count;
	}
	return true;
}

void
get_purchased_item_list_cb(iap_galaxyapps_h reply, iap_galaxyapps_error_e result, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "get_purchased_item_list_cb");
	char ui_statement[UI_STR_LEN] = {0,};

	if (result != IAP_GALAXYAPPS_ERROR_NONE) {
		char *mErrorString = NULL;
		iap_galaxyapps_get_value(reply, "mErrorString", &mErrorString);
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get purchased item list. ret=%d, %s", result, mErrorString);
		snprintf(ui_statement, UI_STR_LEN, "[FAIL] GetPurchasedItemList : %s",mErrorString);
		update_ui(ui_statement);
		return;
	}

	char *mTotalCount = NULL;
	iap_galaxyapps_get_value(reply, "mTotalCount", &mTotalCount);
	if (atoi(mTotalCount) == 0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "You did not purchase any item");
		snprintf(ui_statement, UI_STR_LEN, "No item");
		update_ui(ui_statement);
		return;
	}

	int ret = iap_galaxyapps_foreach_item_info(reply, foreach_purchased_item, NULL);
	if (ret != IAP_GALAXYAPPS_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to foreach item list. ret=%d", ret);
		snprintf(ui_statement, UI_STR_LEN, "[FAIL] GetPurchasedItemList : Foreach");
		update_ui(ui_statement);
		return;
	}
	return;
}

static void my_items_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = event_info;
	elm_genlist_item_selected_set(it, EINA_FALSE);

	static int toggle = 0;
	int ret = 0;

	if (++toggle % 2 == 0) {
		// #1. iap_galaxyapps_get_purchased_item_list
		dlog_print(DLOG_INFO, LOG_TAG, "GET PUCHASED ITEMS");
		ret = iap_galaxyapps_get_purchased_item_list(1, 10, "20160101", "20161231", get_purchased_item_list_cb, NULL);
	} else {
		// #2. iap_galaxyapps_get_item_list_by_item_ids
		char ids[512] = {0,};
		int i = 0;
		for (i = 0; i < total_item_num-1; ++i) {	// except 'invalid item'
			strncat(ids, main_menu_names[i], strlen(main_menu_names[i]));
			if (i != total_item_num-2) {	// except last item
				strncat(ids, ",", strlen(","));
			}
		}
		dlog_print(DLOG_INFO, LOG_TAG, "GET PUCHASED ITEMS by ITEM IDS");
		dlog_print(DLOG_INFO, LOG_TAG, "Item IDs: %s", ids);
		ret = iap_galaxyapps_get_purchased_item_list_by_item_ids(ids, get_purchased_item_list_cb, NULL);
	}

	if (ret != IAP_GALAXYAPPS_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get purchased item list. ret=%d", ret);
		update_ui("[FAIL] GetPurchasedItemList");
	}
	return;
}

// POPUP
static void _show_popup_text(appdata_s *ad, char *string)
{
	Evas_Object *layout;

	getting_item_list_popup = elm_popup_add(ad->win);
	elm_object_style_set(getting_item_list_popup, "circle");
	evas_object_size_hint_weight_set(getting_item_list_popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(getting_item_list_popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(getting_item_list_popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(getting_item_list_popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle");

	elm_object_part_text_set(layout, "elm.text", string);
	elm_object_content_set(getting_item_list_popup, layout);

	evas_object_show(getting_item_list_popup);
}

static void _show_popup_text_1button(char *string, char *button_string)
{
	Evas_Object *popup;
	Evas_Object *btn;
	Evas_Object *layout;
	appdata_s* ad = object;

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle");

	elm_object_part_text_set(layout, "elm.text", string);
	elm_object_content_set(popup, layout);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "popup/circle");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_text_set(btn, button_string);
	elm_object_part_content_set(popup, "button1", btn);
	if (button_string && strcmp(button_string, "OK") == 0) {
		evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);
	} else {
		evas_object_smart_callback_add(btn, "clicked", _get_items_btn_cb, popup);
	}
	evas_object_show(popup);
}

static void _show_purchase_confirm_popup(void)
{
	Evas_Object *popup;
	Evas_Object *btn;
	Evas_Object *icon;
	Evas_Object *layout;
	appdata_s* ad = object;
	char text_str[128] = {0,};

	popup = elm_popup_add(ad->win);
	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, _popup_hide_cb, NULL);
	evas_object_smart_callback_add(popup, "dismissed", _popup_hide_finished_cb, NULL);

	layout = elm_layout_add(popup);
	elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");
	elm_object_part_text_set(layout, "elm.text.title", "Purchase");

	snprintf(text_str, 128, "Are you sure you want to buy %s?", main_menu_names[selected_index]);
	elm_object_part_text_set(layout, "elm.text", text_str);
	elm_object_content_set(popup, layout);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "popup/circle/left");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", _purchase_cancel_cb, popup);

	icon = elm_image_add(btn);
	elm_image_file_set(icon, ICON_DIR"/tw_ic_popup_btn_delete.png", NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn, "elm.swallow.content", icon);
	evas_object_show(icon);

	btn = elm_button_add(popup);
	elm_object_style_set(btn, "popup/circle/right");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(popup, "button2", btn);
	evas_object_smart_callback_add(btn, "clicked", _purchase_start_cb, popup);

	icon = elm_image_add(btn);
	elm_image_file_set(icon, ICON_DIR"/tw_ic_popup_btn_check.png", NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(btn, "elm.swallow.content", icon);
	evas_object_show(icon);

	evas_object_show(popup);
}

void update_ui(char *str)
{
	device_display_change_state(DISPLAY_STATE_NORMAL);	// lcd on
	dlog_print(DLOG_INFO, LOG_TAG, "Updating UI with data %s", str);
	_show_popup_text_1button(str, "OK");
}

static char *_gl_title_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	snprintf(buf, 1023, "%s", "Native IAP");
	return strdup(buf);
}

static char *_gl_sub_title_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	snprintf(buf, 1023, "%s", "In App Purchase");
	return strdup(buf);
}

static char *_gl_main_text_get(void *data, Evas_Object *obj, const char *part)
{
	char buf[1024];
	item_data *id = data;
	int index = id->index;

	if (!strcmp(part, "elm.text"))
		snprintf(buf, 1023, "%s", main_menu_names[index - 1]);

	return strdup(buf);
}


static void _gl_del(void *data, Evas_Object *obj)
{
	// FIXME: Unrealized callback can be called after this.
	// Accessing Item_Data can be dangerous on unrealized callback.
	item_data *id = data;
	if (id) free(id);
}

static void _gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *it = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(it, EINA_FALSE);

	selected_index = (int)data - 1;
	dlog_print(DLOG_ERROR, LOG_TAG, "Selected Item: [%d]%s", selected_index, main_menu_names[selected_index]);
	_show_purchase_confirm_popup();

	return;
}

static Evas_Object *_gl_icon_get(void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *btn = NULL;
	Evas_Object *ic = NULL;

	if (strcmp(part, "elm.icon") != 0) return NULL;

	item_data *id = data;
	int index = id->index;

	btn = elm_button_add(obj);
	elm_object_style_set(btn, "popup/circle/right");
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);

	ic = elm_image_add(btn);
	elm_image_file_set(ic, ICON_DIR"/cart_icon.png", NULL);
	elm_object_content_set(btn, ic);
	evas_object_smart_callback_add(btn, "clicked", _gl_sel, (void*)index);
	evas_object_propagate_events_set(btn, EINA_FALSE);

	return btn;
}

static Eina_Bool _naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
	ui_app_exit();
	return EINA_FALSE;
}

static void create_list_view(appdata_s *ad)
{
	Evas_Object *layout = NULL;
	Evas_Object *genlist = NULL;
	Evas_Object *naviframe = ad->naviframe;
	Elm_Object_Item *nf_it = NULL;
	item_data *id = NULL;
	int index = 0;
	int i =0;

	dlog_print(DLOG_INFO, LOG_TAG, "Create list with items");

	// bottom button
	layout = elm_layout_add(naviframe);
	elm_layout_theme_set(layout, "layout", "bottom_button", "default");
	evas_object_show(layout);

	Evas_Object* btn = elm_button_add(naviframe);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "My Items");
	elm_object_part_content_set(layout, "elm.swallow.button", btn);
	evas_object_smart_callback_add(btn, "clicked", my_items_btn_cb, NULL);
	evas_object_show(btn);

	// genlist
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *titc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *pitc = elm_genlist_item_class_new();
	Elm_Genlist_Item_Class *gic = elm_genlist_item_class_new();

	/* Genlist Item Style */
	itc->item_style = "1text.1icon.1";
	itc->func.content_get = _gl_icon_get;
	itc->func.text_get = _gl_main_text_get;
	itc->func.del = _gl_del;

	/* Genlist Title Item Style */
	titc->item_style = "title";
	titc->func.text_get = _gl_title_text_get;
	titc->func.del = _gl_del;

	gic->item_style = "groupindex";
	gic->func.text_get = _gl_sub_title_text_get;
	gic->func.del = _gl_del;

	pitc->item_style = "padding";
	pitc->func.del = _gl_del;

	/* Create Genlist */
	genlist = elm_genlist_add(naviframe);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "selected", NULL, NULL);

	/* Create Circle Genlist */
	ad->circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);

	/* Set Scroller Policy */
	eext_circle_object_genlist_scroller_policy_set(ad->circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

	/* Activate Rotary Event */
	eext_rotary_object_event_activated_set(ad->circle_genlist, EINA_TRUE);

	/* Title Item Here */
	id = calloc(sizeof(item_data), 1);
	elm_genlist_item_append(genlist, titc, NULL, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);

	id = calloc(sizeof(item_data), 1);
	id->index = index++;
	id->item = elm_genlist_item_append(genlist, gic, id, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL);

	/* Main Menu Items Here*/
	for (i = 0; i < total_item_num; ++i) {
		id = calloc(sizeof(item_data), 1);
		id->index = index++;
		id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);
	}

	/* Padding Item Here */
	elm_genlist_item_append(genlist, pitc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);

	elm_object_part_content_set(layout, "elm.swallow.content", genlist);

	nf_it = elm_naviframe_item_push(naviframe, NULL, NULL, NULL, layout, "empty");
	elm_naviframe_item_pop_cb_set(nf_it, _naviframe_pop_cb, ad->win);

	// hide popup
	if (getting_item_list_popup) {
		elm_popup_dismiss(getting_item_list_popup);
		evas_object_del(getting_item_list_popup);
	}

	evas_object_show(ad->win);
}

static void create_base_gui(appdata_s *ad)
{
	Evas_Object *conform = NULL;

	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);

	/* Conformant */
	conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, conform);
	evas_object_show(conform);

	/* Naviframe */
	ad->naviframe = elm_naviframe_add(conform);
	elm_object_content_set(conform, ad->naviframe);

	/* Eext Circle Surface*/
	ad->circle_surface = eext_circle_surface_naviframe_add(ad->naviframe);

	/* popup text */
	_show_popup_text_1button("Press button to get available item list<br>", "Get Items");

	eext_object_event_callback_add(ad->naviframe, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ad->naviframe, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
	   Initialize UI resources and application's data
	   If this function returns true, the main loop of application starts
	   If this function returns false, the application is terminated */
	dlog_print(DLOG_INFO, LOG_TAG, "Create");
	object = data;

	create_base_gui(object);

	return TRUE;
}

static void app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
	dlog_print(DLOG_INFO, LOG_TAG, "AppControl");
}

static void app_pause(void *data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "Pause");
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume(void *data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "Resume");
	/* Take necessary actions when application becomes visible. */
}

static void app_terminate(void *data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "Terminate");
	int i = 0;
	for (i = 0; i < total_item_num; ++i) {
		if (main_menu_names[i])
			free(main_menu_names[i]);
	}
	/* Release all resources. */
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int main(int argc, char *argv[])
{
	appdata_s ad = { 0, };
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = { 0, };
	app_event_handler_h handlers[5] = { NULL, };

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
