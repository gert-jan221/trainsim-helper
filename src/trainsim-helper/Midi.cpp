#include "stdafx.h"

#include <stdio.h>
#include "Config.h"
#include "Midi.h"

#define MAX_CONTROLLER_VALUE 127
#define MIDI_CHANNELS	16
#define MIDI_CONTROLLERS 128

#define CONTROL_CHANGE 0xb
#define NOTE_ON 0x8
#define NOTE_OFF 0x9

#define HNIBBLE(c) ((c & 0xf0) >> 4)
#define LNIBBLE(c) (c & 0xf)

HMIDIIN gMidiDevice;
UINT16 gEnabledChannels;
char gMidiState[MIDI_CHANNELS][MIDI_CONTROLLERS];
bool gMidiStateChanged;

inline float Normalize(char nValue)
{
	return (nValue / 127.0f);
}

void OnMidiData(char d0, char d1, char d2)
{	
	switch (HNIBBLE(d0))
	{
	case CONTROL_CHANGE:
		gMidiState[LNIBBLE(d0)][d1] = d2;
		gMidiStateChanged = true;
		break;
	case NOTE_ON:
		gMidiState[LNIBBLE(d0)][d1] = MAX_CONTROLLER_VALUE;
		gMidiStateChanged = true;
		break;
	case NOTE_OFF:
		gMidiState[LNIBBLE(d0)][d1] = 0;
		gMidiStateChanged = true;
		break;
	}
}

// Frame update
void UpdateMidi()
{
	if (!gMidiStateChanged)
		return;

	FILE *f = fopen(SETMIDITXT, "w");
	if (f == NULL)
		return;

	int nLine = 1;
	// Second loop for writing file to minimize the write time
	for (int nChannel = 0; nChannel < MIDI_CHANNELS; ++nChannel)
		if((gEnabledChannels >> nChannel) & 1)
			for (int nController = 0; nController < MIDI_CONTROLLERS; ++nController)
				fprintf(f, "%f     #%d Midi %d: %d %d\n", Normalize(gMidiState[nChannel][nController]), nLine++, 0, nChannel, nController);

	fclose(f);

	gMidiStateChanged = false;
}


//-----------------------------------------------------------------------------
// Name: InitMidi()
// Desc: Initialize the midi device variables.
//-----------------------------------------------------------------------------
HRESULT InitMidi(int nController, UINT16 uChannels, HWND hWnd)
{
	gEnabledChannels = uChannels;
	//Clear the status array
	memset(gMidiState, 0, sizeof(char) * MIDI_CHANNELS * MIDI_CONTROLLERS);
	
	unsigned long result;

	result = midiInOpen(&gMidiDevice, nController, (DWORD)hWnd, 0, CALLBACK_WINDOW);
	if (result)
	{
		MessageBox(NULL, TEXT("Cannot open MIDI In device."),
			TEXT("TrainSim Helper"), MB_ICONERROR | MB_OK);
	}

	midiInStart(gMidiDevice);

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FreeMidi()
// Desc: Free the midi handle
//-----------------------------------------------------------------------------
VOID FreeMidi()
{
	if (gMidiDevice != NULL)
	{
		midiInStop(gMidiDevice);
		midiInClose(gMidiDevice);
		gMidiDevice = NULL;
	}
}
