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
using Xamarin.Forms;
using Samsung.Sap;
using System.Diagnostics;
using System.Threading.Tasks;

namespace FileTransferS
{
    public class App : Application
    {

        private bool _isInitialStartUp;

        public App()
        {
            _isInitialStartUp = true;
            MainPage = new NavigationPage(new MainPage());
        }

        protected override async void OnResume()
        {
            if (_isInitialStartUp)
            {
                _isInitialStartUp = false;
                await ExitIfNoSAPSupport();
            }
        }


        private async Task ExitIfNoSAPSupport()
        {
            if (!Service.IsSupported)
            {
                Debug.WriteLine("[DEBUG] SAP is not supported on this device.");
                await this.MainPage.DisplayAlert("Alert", "SAP is not supported on this device.", "OK");
                Quit();
            }
        }
    }
}
