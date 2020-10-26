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
using Tizen.System;

namespace FileTransferR.Utils
{
    public static class DeviceSystemInfo
    {
        public static double Width { get; } = 0;
        public static double Height { get; } = 0;

        static DeviceSystemInfo()
        {
            int width, height;
            bool getWidthSuccess = Information.TryGetValue<int>("http://tizen.org/feature/screen.width", out width);
            bool getHeightSeccess = Information.TryGetValue<int>("http://tizen.org/feature/screen.height", out height);
            if (getWidthSuccess && getHeightSeccess)
            {
                Width = (double)width;
                Height = (double)height;
            }
        }
    }
}

