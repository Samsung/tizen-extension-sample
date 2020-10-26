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
using System.IO;
using System.Linq;
using System.Diagnostics;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Samsung.Sap;

namespace FileTransferR
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// The main page of the app
    /// </summary>
    public partial class MainPage : ContentPage
    {
        private IncomingFileTransfer currentFileTransfer;

        public MainPage()
        {
            InitializeComponent();
        }

        public async Task InitializeSap()
        {
            string accessoryServiceId = Service.Profiles.First();
            Agent agent = await Agent.GetAgent(accessoryServiceId, onFileTransfer: OnFileTransfer);
            agent.PeerStatusChanged += OnPeerStatusChanged;
        }

        private void OnPeerStatusChanged(object sender, PeerStatusEventArgs arg)
        {
            ShowMessage(arg.GetType().ToString());
        }

        private void OnFileTransfer(IncomingFileTransfer incomingFileTransfer)
        {
            Debug.WriteLine("[DEBUG] File sent from companion app.");
            Device.BeginInvokeOnMainThread(async () =>
            {
                bool accept = await DisplayAlert("File sent from companion app", $"Do you want to receive file: {incomingFileTransfer.FileName}?", "Yes", "No");
                if (accept)
                {
                    currentFileTransfer = incomingFileTransfer;
                    fileTransferDialog.IsVisible = true;
                    var path = Path.Combine(Tizen.Applications.Application.Current.DirectoryInfo.Data, incomingFileTransfer.FileName);
                    if (File.Exists(path))
                    {
                        File.Delete(path);
                    }
                    incomingFileTransfer.Finished += FileTransfer_Finished;
                    incomingFileTransfer.Progress += FileTransfer_Progress;
                    incomingFileTransfer.Receive(path);
                }
                else
                {
                    // Notifies the peer that the app has rejected the transfer
                    incomingFileTransfer.Cancel();
                }
            });
        }

        private void OnCancelButtonClicked(object sender, EventArgs e)
        {
            Debug.WriteLine("[DEBUG] Cancel button clicked.");
            if (currentFileTransfer != null)
            {
                currentFileTransfer.Cancel();
                currentFileTransfer = null;
            }
        }

        private void FileTransfer_Progress(object sender, FileTransferProgressEventArgs e)
        {
            progressBar.Progress = e.Progress / 100f;
            Debug.WriteLine("[DEBUG] " + e.Progress / 100f);
        }

        private void FileTransfer_Finished(object sender, FileTransferFinishedEventArgs e)
        {
            fileTransferDialog.IsVisible = false;
            currentFileTransfer = null;
            ShowMessage(e.Result.ToString());
        }

        private void ShowMessage(string message)
        {
            DisplayAlert("", message, "ok");
            Debug.WriteLine("[DEBUG] " + message);
        }
    }
}

