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

package com.samsung.android.sdk.accessory.example.helloremoteprovider;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class RequestAndReplyActivity extends AppCompatActivity {

    public static final String LOG_TAG = "HELLO_REMOTE";
    EditText etReply;
    Button btnReply;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_requet_and_reply);

        etReply = (EditText) findViewById(R.id.etReply);
        btnReply = (Button) findViewById(R.id.btnReply);
        btnReply.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String data = etReply.getText().toString();
                Intent newintent = new Intent();
                newintent.putExtra("remote_extra_data", data);
                setResult(RESULT_OK, newintent);
                Log.d(LOG_TAG, "reply data: " + data);
                finish();

            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
        Intent intent = getIntent();
        String extraData = intent.getStringExtra("remote_extra_data");
        if(extraData != null) {
            Log.d(LOG_TAG, "receive data: " + extraData);
        }
    }
}
