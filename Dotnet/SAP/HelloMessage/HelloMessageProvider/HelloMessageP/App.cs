/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
using System;
using System.Diagnostics;
using Xamarin.Forms;
using Samsung.Sap;

namespace HelloMessageP
{
    public class App : Application
    {
        private bool _isInitialStartUp;

        public App()
        {
            _isInitialStartUp = true;
            MainPage = new MainPage();
        }

        protected override async void OnStart()
        {
            if (_isInitialStartUp)
            {
                _isInitialStartUp = false;
                if (!Service.IsSupported)
                {
                    Debug.WriteLine("[DEBUG] SAP is not supported on this device.");
                    await this.MainPage.DisplayAlert("Alert", "SAP is not supported on this device.", "OK");
                    Quit();
                }
                else
                {
                    MainPage mainPage = MainPage as MainPage; 
                    try
                    {
                        await mainPage?.InitializeSap();
                    }
                    catch (Exception ex)
                    {
                        Debug.WriteLine("[DEBUG] Failed to acquire agent, " + ex.Message);
                        await this.MainPage.DisplayAlert("Alert", ex.Message, "OK");
                        Quit();
                    }
                }
            }
        }
    }
}
