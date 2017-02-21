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

( function () {
	window.addEventListener( 'tizenhwkey', function( ev ) {
		if( ev.keyName === "back" ) {
			var page = document.getElementsByClassName( 'ui-page-active' )[0],
				pageid = page ? page.id : "";
			if( pageid === "main" ) {
				try {
					tizen.application.getCurrentApplication().exit();
				} catch (ignore) {
				}
			} else {
				window.history.back();
			}
		}
	} );
	var mainPage = document.querySelector('#main');
} () );

var itemIds = [];

function getItem() {
	console.log("getItem Init()");

	function onsuccessCB(list) {

		var htmlStr = "";

		if(list._items.length == 0) {
			console.log("No Item");
			htmlStr = "<p3> No Item </p3>"
			document.getElemetById('gettingItems').innerHTML = htmlStr;
			tau.changePage("#main");
		} else {

			for(var i = 0; i < list._items.length; i++) {
				itemIds[i] = list._items[i].mItemId;
				htmlStr += "<li onclick=\"startPayment('" + list._items[i].mItemId + "');\">" + list._items[i].mItemName + "</li>";
			}
			htmlStr += "<li onclick=\"startPayment('Invalid item');\">Invalid item</li>";

			document.getElementById('gettingItems').innerHTML = htmlStr;
			tau.changePage("#main");
		}

	}

	function onerrorCB(e) {
		   console.log(e.name + " : " + e.message);
		   var htmlStr = "";
		   htmlStr += "error name : " + e.name + "<br>";
		   htmlStr += "error msg : " + e.message;

		   document.getElementById('fail').innerHTML = htmlStr;
		   tau.changePage("#error");
		}

	try {
		webapis.inapppurchase.getItemList(1, 15, "ALL", "IAP_SUCCESS_TEST_MODE", onsuccessCB, onerrorCB);
	} catch(e) {
		console.log(e.name + " : " + e.message);
	}

}

var toggle = 0;
function button() {
	console.log("button() init");
	if(++toggle % 2 == 0) {
		getPurchasedItemList();
	} else {
		getPurchasedItemListByIds();
	}
}

function mainpage() {
	tau.changePage("#main");
}

function startPayment(itemId) {
	console.log("startPayment() init");

	function onsuccessCB(item) {
		var htmlStr = "";

	    htmlStr += "<p>Item ID : " + item.mItemId + "\n";
	    htmlStr += "Price String : " + item.mItemPriceString + "\n";
	    htmlStr += "Description : " + item.mItemDesc + "\n";
	    htmlStr += "</p>";
	    document.getElementById('success').innerHTML = htmlStr;
	    tau.changePage("#successcb");
	}

	function onerrorCB(e) {
	   console.log(e.name + " : " + e.message);

	   var htmlStr = "";
	   htmlStr += "error name : " + e.name + "<br>";
	   htmlStr += "error msg : " + e.message;

	   document.getElementById('fail').innerHTML = htmlStr;
	   tau.changePage("#error");

	}

	try {
		webapis.inapppurchase.startPayment(itemId, "IAP_SUCCESS_TEST_MODE", onsuccessCB, onerrorCB);
	} catch(e) {
		console.log(e.name + " : " + e.message);
	}
}

function getItemList() {
	console.log("getItemList() init");

	function onsuccessCB(list) {
		var htmlStr = "";

		if(list._items.length == 0) {
			console.log("No Item");
			htmlStr = "<h3> No Item </h3>";
			document.getElementById('success').innerHTML = htmlStr;
		} else {
			for(var i = 0; i < list._items.length; i++) {
				htmlStr += "<p><h3> Item [" + i + "]</h3>";
				htmlStr += "<h4> Item Name : " + list._items[i].mItemName + "</h4>";
				htmlStr += "<h4> Price String : " + list._items[i].mItemPriceString + "</h4>";
				htmlStr += "</p><hr>";
			}
			document.getElementById('success').innerHTML = htmlStr;
		}

		tau.changePage("#successcb");
	}

	function onerrorCB(e) {
		   console.log(e.name + " : " + e.message);

		   var htmlStr = "";
		   htmlStr += "error name : " + e.name + "<br>";
		   htmlStr += "error msg : " + e.message;

		   document.getElementById('fail').innerHTML = htmlStr;
		   tau.changePage("#error");

		}

	try {
		webapis.inapppurchase.getItemList(1, 15, "ALL", "IAP_SUCCESS_TEST_MODE", onsuccessCB, onerrorCB);
	} catch(e) {
		console.log(e.name + " : " + e.message);
	}
}

function getPurchasedItemList() {
	console.log("getPurchasedItemList() init");

	function onsuccessCB(list) {
		var htmlStr = "";

		if(list._items.length == 0) {
			console.log("No Item");
			htmlStr = "<h3> No Item </h3>";
			document.getElementById('success').innerHTML = htmlStr;
		} else {
			for(var i = 0; i < list._items.length; i++) {
				htmlStr += "<p>Item [" + i + "]\n";
				htmlStr += "Item ID : " + list._items[i].mItemId + "\n";
				htmlStr += "Price String : " + list._items[i].mItemPriceString + "\n";
				htmlStr += "</p><hr>";
			}
			document.getElementById('success').innerHTML = htmlStr;
		}

		tau.changePage("#successcb");
	}

	function onerrorCB(e) {
		   console.log(e.name + " : " + e.message);

		   var htmlStr = "";
		   htmlStr += "error name : " + e.name + "<br>";
		   htmlStr += "error msg : " + e.message;

		   document.getElementById('fail').innerHTML = htmlStr;
		   tau.changePage("#error");

    	}
    try {
		webapis.inapppurchase.getPurchasedItemList(1, 15, new tizen.TZDate(2016, 01, 01),
					new tizen.TZDate(2016, 12, 31), onsuccessCB, onerrorCB);
	} catch(e) {
		console.log(e.name + " : " + e.message);
	}

}

function getPurchasedItemListByIds() {
	console.log("getPurchasedItemListByIds() init");

	function onsuccessCB(list) {
		var htmlStr = "";

		if(list._items.length == 0) {
			console.log("No Item");
			htmlStr = "<h3> No Item </h3>";
			document.getElementById('success').innerHTML = htmlStr;
		} else {
			for(var i = 0; i < list._items.length; i++) {
				htmlStr += "<p>Item [" + i + "]\n";
				htmlStr += "Item ID : " + list._items[i].mItemId + "\n";
				htmlStr += "Price String : " + list._items[i].mItemPriceString + "\n";
				htmlStr += "</p><hr>";
			}
			document.getElementById('success').innerHTML = htmlStr;
		}

		tau.changePage("#successcb");
	}

	function onerrorCB(e) {
		   console.log(e.name + " : " + e.message);

		   var htmlStr = "";
		   htmlStr += "error name : " + e.name + "<br>";
		   htmlStr += "error msg : " + e.message;

		   document.getElementById('fail').innerHTML = htmlStr;
		   tau.changePage("#error");

	}

	try {
		webapis.inapppurchase.getPurchasedItemListByIds(itemIds, onsuccessCB, onerrorCB);
	} catch(e) {
		console.log(e.name + " : " + e.message);
	}
}
