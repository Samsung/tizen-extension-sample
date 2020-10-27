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
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace FileTransferS
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    /// <summary>
    /// File transferring page
    /// </summary>
    public partial class FTPage : ContentPage
    {
        private OutgoingFileTransfer outgoingFileTransfer;

        public FTPage(OutgoingFileTransfer outgoingFileTransfer)
        {
            InitializeComponent();
            this.outgoingFileTransfer = outgoingFileTransfer;
            outgoingFileTransfer.Finished += OutgoingFileTransfer_Finished;
            outgoingFileTransfer.Progress += OutgoingFileTransfer_Progress;
            outgoingFileTransfer.Send();
        }

        private void OutgoingFileTransfer_Progress(object sender, FileTransferProgressEventArgs e)
        {
            progressBar.Progress = e.Progress / 100f;
            System.Diagnostics.Debug.WriteLine(e.Progress);
        }

        private async void OutgoingFileTransfer_Finished(object sender, FileTransferFinishedEventArgs e)
        {
            string message;
            string displayHeader;
            if (e.Result == FileTransferResult.Success)
            {
                displayHeader = "Success";
                message = "File successfully sent";
            }
            else
            {
                displayHeader = "Failed";
                message = e.Result.ToString();
            }
            await DisplayAlert(displayHeader, message, "OK");

            NavigationPage navigationPage = Application.Current.MainPage as NavigationPage;
            await navigationPage?.PopAsync();
        }

        private void OnCancelButtonClicked(object sender, EventArgs e)
        {
            if (!outgoingFileTransfer.IsFinished)
            {
                outgoingFileTransfer.Cancel();
            }
        }

        protected override void OnDisappearing()
        {
            base.OnDisappearing();
            outgoingFileTransfer.Finished -= OutgoingFileTransfer_Finished;
            outgoingFileTransfer.Progress -= OutgoingFileTransfer_Progress;
            this.outgoingFileTransfer = null;
        }

        protected override bool OnBackButtonPressed()
        {
            if (!outgoingFileTransfer.IsFinished)
            {
                outgoingFileTransfer.Cancel();
            }
            (Application.Current.MainPage as NavigationPage).PopAsync();
            return base.OnBackButtonPressed();
        }
    }
}

