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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace HelloAccessoryP
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// The main page of the app
    /// </summary>
    public partial class MainPage : ContentPage
    {
        private Agent agent;
        private Connection connection;

        public MainPage()
        {
            InitializeComponent();
            connectionStatus.Text = ConnectionStatus.ConnectionClosed.ToString();
        }

        public async Task InitializeSAP()
        {
            try
            {
                string serviceProfileId = Service.Profiles.FirstOrDefault();
                agent = await Agent.GetAgent(serviceProfileId, onConnect: OnConnect);
            }
            catch (Exception e)
            {
                ShowMessage(e.Message);
            }
        }

        private bool OnConnect(Connection incomingConnection)
        {
            connection = incomingConnection;
            // This ensures that connection has at most one method of
            // Connection_DataReceived() and Connection_StatusChanged()
            // in the DataReceived and StatusChanged delegates respectively
            connection.StatusChanged -= Connection_StatusChanged;
            connection.StatusChanged += Connection_StatusChanged;
            connection.DataReceived -= Connection_DataReceived;
            connection.DataReceived += Connection_DataReceived;
            return true;
        }

        private void Connection_StatusChanged(object sender, ConnectionStatusEventArgs e)
        {
            connectionStatus.Text = e.Reason.ToString();
            Debug.WriteLine("[DEBUG] " + e.Reason.ToString());

            if (e.Reason == ConnectionStatus.ConnectionClosed ||
                e.Reason == ConnectionStatus.ConnectionLost)
            {
                connection.DataReceived -= Connection_DataReceived;
                connection.StatusChanged -= Connection_StatusChanged;
                connection.Close();
                connection = null;
                agent = null;
            }
        }

        private void Connection_DataReceived(object sender, Samsung.Sap.DataReceivedEventArgs arg)
        {
            string message = Encoding.UTF8.GetString(arg.Data) + " Time: " + DateTime.Now.ToShortTimeString();
            ShowMessage(message);
            try
            {
                connection.Send(arg.Channel, Encoding.UTF8.GetBytes(message));
            }
            catch (Exception e)
            {
                ShowMessage(e.Message);
            }
        }

        private void ShowMessage(string message)
        {
            DisplayAlert("", message, "ok");
            Debug.WriteLine("[DEBUG] " + message);
        }
    }
}

