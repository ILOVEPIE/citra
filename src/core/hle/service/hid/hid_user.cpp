// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core/hle/hle.h"
#include "core/hle/service/hid/hid.h"
#include "core/hle/service/hid/hid_user.h"

namespace Service {
namespace HID {

const Interface::FunctionInfo FunctionTable[] = {
    {0x000A0000, GetIPCHandles, "GetIPCHandles"},
    {0x00110000, nullptr,       "EnableAccelerometer"},
    {0x00120000, nullptr,       "DisableAccelerometer"},
    {0x00130000, nullptr,       "EnableGyroscopeLow"},
    {0x00140000, nullptr,       "DisableGyroscopeLow"},
    {0x00150000, nullptr,       "GetGyroscopeLowRawToDpsCoefficient"},
    {0x00160000, nullptr,       "GetGyroscopeLowCalibrateParam"},
    {0x00170000, nullptr,       "GetSoundVolume"},
};

HID_U_Interface::HID_U_Interface() {
    Register(FunctionTable);
}

} // namespace HID
} // namespace Service
