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

namespace HelloAccessoryC
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// The main page of the app
    /// </summary>
    public partial class MainPage : ContentPage
    {
        private Agent agent;
        private Connection connection;
        private Peer peer;

        public MainPage()
        {
            InitializeComponent();
        }

        private async void Connect()
        {
            try
            {
                string serviceProfileId = Service.Profiles.FirstOrDefault();
                agent = await Agent.GetAgent(serviceProfileId);
                agent.PeerStatusChanged += PeerStatusChanged;
                var peers = await agent.FindPeers();
                if (peers.Count() > 0)
                {
                    peer = peers.First();
                    connection = peer.Connection;
                    // This ensures that connection has at most one method of
                    // Connection_DataReceived() and Connection_StatusChanged()
                    // in the DataReceived and StatusChanged delegates respectively
                    connection.DataReceived -= Connection_DataReceived;
                    connection.DataReceived += Connection_DataReceived;
                    connection.StatusChanged -= Connection_StatusChanged;
                    connection.StatusChanged += Connection_StatusChanged;
                    await connection.Open();
                }
                else
                {
                    ShowMessage("Any peer not found");
                }
            }
            catch (Exception ex)
            {
                ShowMessage(ex.Message);
            }
        }

        private void PeerStatusChanged(object sender, PeerStatusEventArgs e)
        {
            if (e.Peer == peer)
            {
                ShowMessage($"Peer Available: {e.Available}, Status: {e.Peer.Status}");
            }
        }

        private void Connection_DataReceived(object sender, Samsung.Sap.DataReceivedEventArgs e)
        {
            ShowMessage(Encoding.UTF8.GetString(e.Data));
        }

        private void Connection_StatusChanged(object sender, ConnectionStatusEventArgs e)
        {
            ShowMessage(e.Reason.ToString());

            if (e.Reason == ConnectionStatus.ConnectionClosed ||
                e.Reason == ConnectionStatus.ConnectionLost)
            {
                connection.DataReceived -= Connection_DataReceived;
                connection.StatusChanged -= Connection_StatusChanged;
                connection.Close();
                connection = null;
                peer = null;
                agent = null;
            }
        }

        private void Disconnect()
        {
            if (connection != null)
            {
                connection.Close();
            }
        }

        private void Fetch()
        {
            if (connection != null && agent != null && agent.Channels.Count > 0)
            {
                connection.Send(agent.Channels.First().Value, Encoding.UTF8.GetBytes("Hello Accessory!"));
            }
        }

        private void ListView_ItemTapped(object sender, ItemTappedEventArgs e)
        {
            try
            {
                switch (e.Item as string)
                {
                    case "Connect":
                        Connect();
                        break;

                    case "Fetch":
                        Fetch();
                        break;

                    case "Disconnect":
                        Disconnect();
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
    }
}

