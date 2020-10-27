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
using System.Text;
using System.Linq;
using System.Diagnostics;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Samsung.Sap;

namespace HelloMessageP
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// The main page of the app
    /// </summary>
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();
        }

        public async Task InitializeSap()
        {
            string accessoryServiceId = Service.Profiles.First();
            Agent agent = await Agent.GetAgent(accessoryServiceId, onMessage: OnMessage);
            Debug.WriteLine("[DEBUG] Agent acquired for " + accessoryServiceId);
        }
        private async void OnMessage(Peer peer, byte[] content)
        {
            string query = Encoding.UTF8.GetString(content);
            ShowMessage("Data received: " + query);
            string response = query + " Time: " + DateTime.Now.ToShortTimeString();
            await peer.SendMessage(Encoding.UTF8.GetBytes(response));
        }

        private void ShowMessage(string message)
        {
            DisplayAlert("", message, "ok");
            Debug.WriteLine("[DEBUG] " + message);
        }
    }
}

