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
using FileTransferS.Utils;
using System;

namespace FileTransferS.Constants
{
    public static class UIConstants
    {
        // ListView in Tizen Platform uses a window that displays at most three items (including the header) 
        // on the screen at a time. The height of the header can be manually set so that all items in the list fit evenly
        // in a given window.
        public static double HeaderHeight { get; } = DeviceSystemInfo.Height / 3.0;
        // Shrink the width of the header to prevent long texts being off screen due to
        // the arcs of the circlular watch device.
        public static double HeaderWidth { get; } = DeviceSystemInfo.Width * Math.Cos(Math.PI / 4.0);
    }
}

