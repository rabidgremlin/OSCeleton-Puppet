/*
 *      Interface to the waveIn waveForm API
 *
 *     Copyright (c) 2010 THEARTOFWEB Software
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author contact: theartofweb@hotmail.com
 *
 */

/* $Id: waveCapture.h, v0.3 15/01/2001 17:52:00 */

/* (v0.2) Thanks to user "shodan" from forum.html.it for fixing some bugs */

#ifndef WAVECAPTURE_H
#define WAVECAPTURE_H

#include <windows.h>
#pragma comment (lib, "winmm.lib")
#include <mmsystem.h>
#include <fstream>

enum WAVECAPTURE_MODE {
	WAVE_MONO   = 1,
	WAVE_STEREO = 2
};

enum WAVECAPTURE_ERROR {
	ERR_WAVEINOPEN          = -1,
	ERR_WAVEINSTART         = -2,
	ERR_WAVEINPREPAREHEADER = -3,
	ERR_WAVEINADDBUFFER     = -4,
	ERR_NOERROR             = 0,
	ERR_ALREADYSTARTED      = -5,
	ERR_BUFFERALLOC         = -6,
	ERR_EVENTNULL           = -10
};

////
  // Declare waveCapture class
////

class waveCapture
{
public:
	waveCapture();
	~waveCapture();
	waveCapture(const DWORD dwSamplePerSec, const WORD wBitsPerSample, const WORD nChannels);

	WORD getWaveInNumDevices(void);
	LPSTR getWaveInDevName(const WORD uDev);
	WORD getWaveInDevProductId(const WORD uDev);
	WORD getWaveInDevManufacturerId(const WORD uDev);
	WORD getWaveInDevNumChannels(const WORD uDev);

	DWORD start(void);
	DWORD start(const WORD uDevice);
	DWORD start(const WORD uDevice, const DWORD dwBufferLength, const DWORD dwNumBuffers);

	DWORD stop(void);

	DWORD readBuffer(char* pWAVBuffer);

	bool createWAVEFile(const char * szFilePath);
	bool saveWAVEChunk(const char * pWAVBuffer, const DWORD dwBuffLen);
	bool closeWAVEFile(void);

	DWORD dwSamplePerSec() const { return _dwSamplePerSec; }
	WORD wBitsPerSample() const { return _wBitsPerSample; }
	WORD nChannels() const { return _nChannels; }
	DWORD getTotalBytesRecorded() const { return dwTotalBufferLength; }
	DWORD getSuggestedBufferSize(void);

	const static char szVersion[32];

private:
	DWORD _start(const WORD, const DWORD, const DWORD);

	DWORD _dwSamplePerSec;
	WORD _wBitsPerSample;
	WORD _nChannels;
	WAVEINCAPS wcaps;
	WAVEFORMATEX wf;
	WAVEHDR **buff;
	HANDLE hevent;
	HWAVEIN hwi;
	DWORD dwTotalBufferLength;
	DWORD __dwNumBuffers;
	DWORD _dwBufferCount;
	DWORD dwCNumBuffers;
	std::ofstream hFile;
	const char* _szFilePath;
	bool _recording;
};

#endif
