//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2016  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <circle/debug.h>
#include <circle/gpiopin.h>
#include <circle/string.h>
#include <circle/timer.h>

#include "kernel.h"
static const char FromKernel[] = "kernel";
CKernel::CKernel(void) : m_Screen(m_Options.GetWidth(), m_Options.GetHeight()), m_Logger(m_Options.GetLogLevel()) {
    m_ActLED.Blink(5); // show we are alive
}

CKernel::~CKernel(void) {}

boolean CKernel::Initialize(void) {
    boolean bOK = TRUE;

    if (bOK) {
        bOK = m_Screen.Initialize();
    }

    if (bOK) {
        bOK = m_Serial.Initialize(115200);
    }

    if (bOK) {
        CDevice *pTarget = m_DeviceNameService.GetDevice(m_Options.GetLogDevice(), FALSE);
        if (pTarget == 0) {
            pTarget = &m_Screen;
        }

        bOK = m_Logger.Initialize(pTarget);
    }

    return TRUE;
}

void CKernel::Print(const char *pFormat, ...) {
    CString Msg;

    va_list var;
    va_start(var, pFormat);

    Msg.FormatV(pFormat, var);

    va_end(var);

    m_Serial.Write((const char *)Msg, Msg.GetLength());
}

TShutdownMode CKernel::Run(void) {
    CGPIOPin AudioLeft(GPIOPinAudioLeft, GPIOModeOutput);
    CGPIOPin AudioRight(GPIOPinAudioRight, GPIOModeOutput);

    // flash the Act LED 10 times and click on audio (3.5mm headphone jack)
    for (unsigned i = 1; i <= 1000000; i++) {
        m_ActLED.On();
        AudioLeft.Invert();
        AudioRight.Invert();
        CTimer::SimpleMsDelay(200);
        m_ActLED.Off();
        CTimer::SimpleMsDelay(500);
        Print("Hello\n");
        Print("World i:%d\n", i);
    }

    return ShutdownReboot;
}
