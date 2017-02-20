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
} () );


function createHTML(log_string)
{
	var content = document.getElementById("toast-content");
	content.textContent = log_string;
	console.log(log_string);
	tau.openPopup("#toast");
}


function implicit()
{
	var remoteAppCtrl = new webapis.RemoteApplicationControl(
			"http://samsung.com/appcontrol/operation/remote/view",
			"https://developer.tizen.org/"
			);	

	try {
		webapis.remoteappcontrol.launchRemoteAppControl( remoteAppCtrl,
				function () {createHTML("launch success");}, function(e) {createHTML("failed : " + e.name);});
	} catch(e) {
		console.log("Error Exception, error name : " + e.name + ", error message : " + e.message);
	}
}

function explicit()
{
	 var remoteAppControlReplyCallback = {
          onsuccess: function(data) {
        	  createHTML('Recevied data is :' + data);
          },
          // callee returned failure
          onfailure: function() {
        	  createHTML('The launch remote application control failed');
          }
      }
	
		var remoteAppCtrl = new webapis.RemoteApplicationControl(
				"http://samsung.com/appcontrol/operation/remote/default", null,
				"com.samsung.android.sdk.accessory.example.helloremoteprovider", 
				"com.samsung.android.sdk.accessory.example.helloremoteprovider.RequestAndReplyActivity", 
				"this data from host device"
				);

		try {
			webapis.remoteappcontrol.launchRemoteAppControl( remoteAppCtrl,
					function () {createHTML("launch success");}, function(e) {createHTML("failed : " + e.name);}, remoteAppControlReplyCallback);
		} catch(e) {
			console.log("Error Exception, error name : " + e.name + ", error message : " + e.message);
		}	 
}

(function(tau) {
	var toastPopup = document.getElementById('toast');
	toastPopup.addEventListener('popupshow', function(ev){
		setTimeout(function(){tau.closePopup();}, 3000);
	}, false);
})(window.tau);
