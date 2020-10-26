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
using Samsung.Sap;
using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace FileTransferS
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// The main page of the app
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
            try
            {
                switch (e.Item as string)
                {
                    case "Find Peer":
                        FindPeer();
                        break;

                    case "Send File":
                        SendFile();
                        break;
                }
            }
            catch (Exception ex)
            {
                ShowMessage(ex.Message);
            }
        }

        private void ShowMessage(string message)
        {
            DisplayAlert("", message, "ok");
            Debug.WriteLine("[DEBUG] " + message);
        }

        private void SendFile()
        {
            if (peer == null)
            {
                ShowMessage("Peer doesn't exist.");
                return;
            }

            var path = Path.Combine(Tizen.Applications.Application.Current.DirectoryInfo.Resource, "text.txt");
            OutgoingFileTransfer outgoingFileTransfer = new OutgoingFileTransfer(peer, path);
            (Application.Current.MainPage as NavigationPage).PushAsync(new FTPage(outgoingFileTransfer));
        }

        private async void FindPeer()
        {
            string serviceProfileId = Service.Profiles.FirstOrDefault();
            agent = await Agent.GetAgent(serviceProfileId);
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
    }
}

