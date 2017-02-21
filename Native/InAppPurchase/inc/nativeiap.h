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

#ifndef __NATIVE_IAP_SAMPLE_H__
#define __NATIVE_IAP_SAMPLE_H__

#include <app.h>
#include <glib.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "NativeIAPTest"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.nativeiaptest"
#endif

typedef struct {
	char* mErrorCode;
	char* mErrorString;
	char* mExtraString;

	char* mStartNumber;
	char* mEndNumber;
	char* mTotalCount;

	char* mItemId;
	char* mItemName;
	char* mItemPrice;
	char* mItemPriceString;
	char* mCurrencyUnit;
	char* mCurrencyCode;
	char* mItemDesc;
	char* mItemImageUrl;
	char* mItemDownloadUrl;
	char* mPaymentId;
	char* mPurchaseId;
	char* mPurchaseDate;
	char* mVerifyUrl;
	char* mType;
	char* mSubscriptionDurationUnit;
	char* mSubscriptionDurationMultiplier;
	char* mSubscriptionEndDate;
	char* mJsonString;
} output_data;
#endif
