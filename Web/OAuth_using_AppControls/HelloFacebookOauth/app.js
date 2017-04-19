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

var deviceCode = null;
var intervalTime = null;
var intervalObj = null;
var tokenObj = null;

var CLIENT_TOKEN = "ee152bba9f3aaf2aea38752996a8c7c5";
var APP_ID = "1418696321484314";

function toastPopup(contentString)
{
	var content = document.getElementById("toast-content");
	content.textContent = contentString;
	tau.openPopup("#toast");
}

function requestProfile(access_token, expires_in)
{
	var reqURI = "https://graph.facebook.com/v2.7/me?fields=name,email&access_token=" + access_token;
	console.log("reqURI:" + reqURI);
	var xmlRequest = new XMLHttpRequest();
	if (xmlRequest) {
		xmlRequest.open("GET", reqURI);

		xmlRequest.onreadystatechange = function() {
		    if (xmlRequest.readyState == 4) {
		    	console.log(xmlRequest.responseText);
		        if(xmlRequest.status == 200) {
		        	var profile = JSON.parse(xmlRequest.responseText);

		        	var txt_content = document.getElementById("text-content");
		        	txt_content.innerHTML = profile.name; 

		        } else {
		        	console.log(xmlRequest.responseText);
		        	toastPopup("Request Profile fail..." );
		        }
		    } else {
		    	console.log("readyState:" + xmlRequest.readyState);
		    }
		};
		
		try {
			xmlRequest.send();
		} catch(err) {
			console.log("exception [" + err.name + "] msg[" + err.message + "]");
		}
	}
}

function requestAccessToken()
{
	try {
		var postfields = "access_token=" + APP_ID + "|" + CLIENT_TOKEN + "&code=" + deviceCode;
		console.log(postfields);
		var xmlRequest = new XMLHttpRequest();
		if (xmlRequest) {

			xmlRequest.open("POST", "https://graph.facebook.com/v2.7/device/login_status", true);
			xmlRequest.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=UTF-8");
			xmlRequest.timeout = intervalTime;

			xmlRequest.onreadystatechange = function() {
			    if (xmlRequest.readyState == 4) {
			        console.log(xmlRequest.responseText);
			        if(xmlRequest.status == 200) {
			           var token = JSON.parse(xmlRequest.responseText);
			           if (token && token.access_token) {
			        	   tokenObj = token;
			           } else {
			        	   toastPopup("Request Access Token fail..." );
			           }
			        } else {
			        	var errorMsg = JSON.parse(xmlRequest.responseText);
						if ( errorMsg && errorMsg.error ) {
							console.log(errorMsg.error.message);
						} else {
							toastPopup("Request Access Token fail..." );
						}
			        }
			    }
			};

			xmlRequest.ontimeout = function(e) {
				console.log("timeout");
			}

			xmlRequest.send(postfields);
		}

	} catch(err) {
		console.log("exception [" + err.name + "] msg[" + err.message + "]");
	}
}

function pollAccessToken()
{
	console.log("poll...");
	requestAccessToken();
	if (tokenObj) {
		clearInterval(intervalObj);
		requestProfile(tokenObj.access_token, tokenObj.expires_in);
	}
}

function consentLogin(userCode) {
	var txt_content = document.getElementById("text-content");
	txt_content.innerHTML = userCode.user_code; 
	console.log("url...:" + userCode.verification_uri);

	deviceCode = userCode.code;
	intervalTime = userCode.interval*1000;

	var appid = "tizen.wearablemanager",
    extra_data = [new tizen.ApplicationControlData("type", ["launch-remote-browser"])];

    var appControl = new tizen.ApplicationControl(
        "http://tizen.org/appcontrol/operation/default",
        userCode.verification_uri,
        null,
        null,
        extra_data
   );
    
  tizen.application.launchAppControl(appControl, appid,
	function() {
	  	console.log("success");
	  	toastPopup("enter user code to mobile");
	  	intervalObj = setInterval(pollAccessToken, intervalTime);
  	}, 
    function(e) { toastPopup("failed : " + e.message); }
  );
}

function requestUserCode() {

	var postfields = "access_token=" + APP_ID + "|" + CLIENT_TOKEN  + "&scope=public_profile";

	var xmlRequest = new XMLHttpRequest();
	if (xmlRequest) {
		console.log("request");
		xmlRequest.open("POST", "https://graph.facebook.com/v2.7/device/login", true);
		xmlRequest.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=UTF-8");

		xmlRequest.onreadystatechange = function() {
		    if (xmlRequest.readyState == 4) {
		    	console.log(xmlRequest.responseText);
		        if(xmlRequest.status == 200) {
		           var userCode = JSON.parse(xmlRequest.responseText);
		           consentLogin(userCode);
		        } else {
		        	console.log("status:" + xmlRequest.status);
		        	var errorMsg = JSON.parse(xmlRequest.responseText);
					if ( errorMsg && errorMsg.error ) {
						toastPopup(errorMsg.error.message);
					} else {
						toastPopup("Request Access Token fail..." );
					}
		        }
		    }
		};

		try {
			xmlRequest.send(postfields);
		} catch(err) {
			console.log("exception [" + err.name + "] msg[" + err.message + "]");
		}
	}
}

function clickConfirmBtn()
{
	requestUserCode();
}

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

	function init() {
		var confirmBtn = document.querySelector("#auth-button");
		confirmBtn.addEventListener("click", clickConfirmBtn);
	}

	window.onload = init();
} () );

(function(tau) {
	var toastPopup = document.getElementById('toast');
	toastPopup.addEventListener('popupshow', function(ev){
		setTimeout(function(){tau.closePopup();}, 2000);
	}, false);
})(window.tau);
