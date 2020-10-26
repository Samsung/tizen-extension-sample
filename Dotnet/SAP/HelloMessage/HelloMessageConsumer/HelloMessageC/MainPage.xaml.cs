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
using System.Linq;
using System.Text;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Samsung.Sap;

namespace HelloMessageC
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// The main page of the app.
    /// </summary>
    public partial class MainPage : ContentPage
    {
        private Agent agent;
        private Peer peer;

        public MainPage()
        {
            InitializeComponent();
        }

        private void ListView_ItemTapped(object sender, ItemTappedEventArgs e)
        {
            switch (e.Item as string)
            {
                case "Find Peer":
                    FindPeer();
                    break;

                case "Fetch":
                    Fetch();
                    break;
            }
        }

        private async void Fetch()
        {
            await peer.SendMessage(Encoding.UTF8.GetBytes("Hello Message"));
        }

        private async void FindPeer()
        {
            try
            {
                string serviceProfileId = Service.Profiles.FirstOrDefault();
                agent = await Agent.GetAgent(serviceProfileId, onMessage: OnMessage);
                var peers = await agent.FindPeers();
                if (peers.Count() > 0)
                {
                    peer = peers.First();
                    ShowMessage("Peer found, now you can send data.");
                }
                else
                {
                    ShowMessage("Any peer not found");
                }
            }
            catch (Exception e)
            {
                ShowMessage(e.Message);
            }
        }

        private void OnMessage(Peer peer, byte[] content)
        {
            ShowMessage("Received data: " + Encoding.UTF8.GetString(content));
        }

        private void ShowMessage(string message)
        {
            DisplayAlert("", message, "ok");
            Debug.WriteLine("[DEBUG] " + message);
        }
    }
}

