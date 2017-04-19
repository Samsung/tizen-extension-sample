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

var CLIENT_ID = "985249717635-m2g710rhvtpj8qeefmivv7c5j9eojkrp.apps.googleusercontent.com";
var CLIENT_SECRET = "xwypl235JB7dljd7EjnrVWRN";

var userCodeResponse = function(data) {
	Object.defineProperties(this, {
		device_code : { value: String(data.device_code), writeable: false, enumerable: true},
		user_code : { value: String(data.user_code), writeable: false, enumerable: true},
		verification_url : { value: String(data.verification_url), writeable: false, enumerable: true},
		expires_in : {value: data.expires_in, writeable: false, enumerable: true},
		interval : {value: data.interval, writeable: false, enumerable: true}
	});	
}

function toastPopup(contentString)
{
	var content = document.getElementById("toast-content");
	content.textContent = contentString;
	tau.openPopup("#toast");
}

function requestProfile(access_token, expires_in)
{
	try {
		var reqURI = "https://www.googleapis.com/plus/v1/people/me?access_token=" + access_token;
		var xmlRequest = new XMLHttpRequest();
		if (xmlRequest) {
			xmlRequest.open("GET", reqURI);
	
			xmlRequest.onreadystatechange = function() {
			    if (xmlRequest.readyState == 4) {
			    	console.log(xmlRequest.responseText);
			        if(xmlRequest.status == 200) {
			        	toastPopup("OAuth Complete.");
			        	var profile = JSON.parse(xmlRequest.responseText);
			        	var txt_content = document.getElementById("text-content");
			        	txt_content.innerHTML = profile.displayName;
			        } else {
			    	   var errorMsg = JSON.parse(xmlRequest.responseText);
			    	   if ( errorMsg && errorMsg.error ) {
			    		   toastPopup(errorMsg.error_description);
			    	   }
			        }
			    }
			};		
		}

		xmlRequest.send();
	} catch(err) {
		console.log("exception [" + err.name + "] msg[" + err.message + "]");
	}	
}

function requestAccessToken()
{
	var postfields = "client_id=" + CLIENT_ID + "&client_secret=" + CLIENT_SECRET + "&code=" + deviceCode + "&grant_type=http://oauth.net/grant_type/device/1.0";

	try {
		var xmlRequest = new XMLHttpRequest();
		if (xmlRequest) {
			
			xmlRequest.open("POST", "https://www.googleapis.com/oauth2/v4/token", true);
			xmlRequest.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=UTF-8");
			xmlRequest.timeout = intervalTime;

			xmlRequest.onreadystatechange = function() {
			    if (xmlRequest.readyState == 4) {
			    	console.log(xmlRequest.responseText);
			        if(xmlRequest.status == 200) {
			           var token = JSON.parse(xmlRequest.responseText);
			           if (token && token.access_token) {
			        	   tokenObj = token;
				       }
			        } else {
			        	var errorMsg = JSON.parse(xmlRequest.responseText);
						if ( errorMsg && errorMsg.error ) {
							console.log(errorMsg.error_description);
						} else {
							toastPopup("Request Access Token fail..." );
						}
			        }
			    }
			};

			xmlRequest.ontimeout = function(e) {
				console.log("timeout:" + e);
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
	if (tokenObj && intervalObj) {
		clearInterval(intervalObj);
		requestProfile(tokenObj.access_token, tokenObj.expires_in);
	}
}

function consentLogin(userCode) {
	var txt_content = document.getElementById("text-content");
	txt_content.innerHTML = userCode.user_code; 
	
	deviceCode = userCode.device_code;
	intervalTime = userCode.interval*1000;

	var remoteAppCtrl = new webapis.RemoteApplicationControl(
			"http://samsung.com/appcontrol/operation/remote/view",
			userCode.verification_url, null, null, null
		);

	try {
		webapis.remoteappcontrol.launchRemoteAppControl( remoteAppCtrl,
				function () {
					console.log("success");
					toastPopup("enter user code to mobile");
					intervalObj = setInterval(pollAccessToken, intervalTime);
				}, function(e) {toastPopup("failed : " + e.message);});
	} catch(e) {
		toastPopup("Error Exception, error name : " + e.name + ", error message : " + e.message);
	}
}

function requestUserCode() {
	
	var postfields = "client_id=" + CLIENT_ID + "&scope=email profile";

	try {
		var xmlRequest = new XMLHttpRequest();
		if (xmlRequest) {
			xmlRequest.open("POST", "https://accounts.google.com/o/oauth2/device/code", true);
			xmlRequest.setRequestHeader("Content-Type","application/x-www-form-urlencoded;charset=UTF-8");
	
			xmlRequest.onreadystatechange = function() {
			    if (xmlRequest.readyState == 4) {
			    	console.log(xmlRequest.responseText);
			        if(xmlRequest.status == 200) {
			           consentLogin(new userCodeResponse(JSON.parse(xmlRequest.responseText)));   
			        } else {
			        	console.log("status:" + xmlRequest.status);
			        	var errorMsg = JSON.parse(xmlRequest.responseText);
						if ( errorMsg && errorMsg.error ) {
							toastPopup(errorMsg.error_description);
						} else {
							toastPopup("Request Access Token fail..." );
						}
			        }
			        
			    }
			};

			xmlRequest.send(postfields);
		}
	} catch(err) {
		console.log("exception [" + err.name + "] msg[" + err.message + "]");
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
