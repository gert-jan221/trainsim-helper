#pragma once

#include "stdafx.h"

HRESULT InitMidi(int nController, UINT16 uChannels, HWND hWnd);
void OnMidiData(char d0, char d1, char d2);
VOID FreeMidi();
void UpdateMidi();
