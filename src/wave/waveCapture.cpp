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

/* $Id: waveCapture.cpp, v0.3 15/01/2001 17:52:00 */

/* (v0.2) Thanks to user "shodan" from forum.html.it for fixing some bugs */

#include "waveCapture.h"
#include <fstream>

// Define class version:
const char waveCapture::szVersion[32] = "waveCapture Class v0.1";

// Define constructors:
waveCapture::waveCapture()
:
_dwSamplePerSec(44100),
_wBitsPerSample(16),
_nChannels(2),
buff(NULL),
hwi(NULL),
hevent(NULL),
dwCNumBuffers(3),
_recording(false)
{ }

waveCapture::waveCapture(const DWORD dwSamplePerSec, const WORD wBitsPerSample, const WORD nChannels)
:
_dwSamplePerSec(dwSamplePerSec),
_wBitsPerSample(wBitsPerSample),
_nChannels(nChannels),
buff(NULL),
hwi(NULL),
hevent(NULL),
dwCNumBuffers(3),
_recording(false)
{ }

// Define destructor:
waveCapture::~waveCapture()
{
	for (int u = 0; u<(int)__dwNumBuffers; u++)
	{
		free( buff[u]->lpData );
	}
	delete[] buff;
}

// Define: getSuggestedBufferSize()
DWORD waveCapture::getSuggestedBufferSize()
{
	return (DWORD)(_dwSamplePerSec * ((_nChannels * _wBitsPerSample) / 8));
}

// Define: getWaveInNumDevices()
WORD waveCapture::getWaveInNumDevices()
{
	return (WORD)waveInGetNumDevs();
}

// Define: getWaveInDevName(DWORD deviceID)
LPSTR waveCapture::getWaveInDevName(const WORD uDev)
{
	if ( waveInGetDevCaps(uDev, &wcaps, sizeof(wcaps)) == MMSYSERR_NOERROR )
	{
		return wcaps.szPname;
	} else {
		return NULL;
	}
}

// Define: getWaveInDevProductId(DWORD deviceID)
WORD waveCapture::getWaveInDevProductId(const WORD uDev)
{
	if ( waveInGetDevCaps(uDev, &wcaps, sizeof(wcaps)) == MMSYSERR_NOERROR )
	{
		return wcaps.wPid;
	} else {
		return NULL;
	}
}

// Define: getWaveInDevManufacturerId(DWORD deviceID)
WORD waveCapture::getWaveInDevManufacturerId(const WORD uDev)
{
	if ( waveInGetDevCaps(uDev, &wcaps, sizeof(wcaps)) == MMSYSERR_NOERROR )
	{
		return wcaps.wMid;
	} else {
		return NULL;
	}
}

// Define: getWaveInDevNumChannels(DWORD deviceID)
WORD waveCapture::getWaveInDevNumChannels(const WORD uDev)
{
	if ( waveInGetDevCaps(uDev, &wcaps, sizeof(wcaps)) == MMSYSERR_NOERROR )
	{
		return wcaps.wChannels;
	} else {
		return NULL;
	}
}

// Define: start() (fake)
DWORD waveCapture::start()
{
	DWORD dwCBufferLength = getSuggestedBufferSize();
	return _start(0, dwCBufferLength, dwCNumBuffers);
}

// Define: start(WORD) (fake)
DWORD waveCapture::start(const WORD uDevice)
{
	DWORD dwCBufferLength = getSuggestedBufferSize();
	return _start(uDevice, dwCBufferLength, dwCNumBuffers);
}

// Define: start(WORD, DWORD, DWORD) (fake)
DWORD waveCapture::start(const WORD uDevice, const DWORD dwBufferLength, const DWORD dwNumBuffers)
{
	return _start(uDevice, dwBufferLength, dwNumBuffers);
}

// Define: _start(WORD, DWORD, DWORD) (real)
DWORD waveCapture::_start(const WORD uDevice, const DWORD dwBufferLength, const DWORD dwNumBuffers)
{
	// only one start per session is permitted
	if(_recording)
		return ERR_ALREADYSTARTED;

	// I need dwNumBuffers in stop() as well so I'll set __dwNumBuffers under the counter
	__dwNumBuffers = dwNumBuffers;

	// Define WAVEFORMATEX Structure (WAVEFORMATEX wf):
	wf.wFormatTag      = WAVE_FORMAT_PCM;
	wf.wBitsPerSample  = _wBitsPerSample;
	wf.nChannels       = _nChannels;
	wf.nSamplesPerSec  = _dwSamplePerSec;
	wf.nBlockAlign     = (wf.nChannels * wf.wBitsPerSample) / 8;
	wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nBlockAlign);
	wf.cbSize          = 0;

	// Create event:
	hevent = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(hevent == NULL)
		return ERR_EVENTNULL;

	// WaveInOpen
	if(waveInOpen(&hwi,uDevice,(LPWAVEFORMATEX)&wf,(DWORD)hevent,0,CALLBACK_EVENT) != MMSYSERR_NOERROR)
		return ERR_WAVEINOPEN;

	// Define WAVEHDR Structure:
	buff = new WAVEHDR*[dwNumBuffers];
	for (int i = 0; i<(int)dwNumBuffers; i++)
	{
		buff[i] = new WAVEHDR;
		ZeroMemory(buff[i],sizeof(WAVEHDR));
		buff[i]->lpData          = (char*) malloc(dwBufferLength);
		buff[i]->dwBufferLength  = dwBufferLength;
		buff[i]->dwBytesRecorded = 0;
		buff[i]->dwUser          = 0;
		buff[i]->dwFlags         = 0;
		buff[i]->dwLoops         = 0;

		if(waveInPrepareHeader(hwi, buff[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			return ERR_WAVEINPREPAREHEADER;

		if(waveInAddBuffer(hwi, buff[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			return ERR_WAVEINADDBUFFER;
	}

	// Start capturing...
	if(waveInStart(hwi) != MMSYSERR_NOERROR)
		return ERR_WAVEINSTART;

	_dwBufferCount      = 0;
	dwTotalBufferLength = 0;
	_recording          = true;
	return ERR_NOERROR;
}

// Define: stop()
DWORD waveCapture::stop()
{
	MMRESULT h;  
	for (int u = 0; u<(int)__dwNumBuffers; u++)
	{
		do {
			h = waveInUnprepareHeader(hwi, buff[u], sizeof(WAVEHDR));
		} while (h != MMSYSERR_NOERROR); 
	}
	waveInClose(hwi);
	CloseHandle(hevent);
	_recording = false;
	return ERR_NOERROR;
}

// Define: readBuffer(char*)
DWORD waveCapture::readBuffer(char* pWAVBuffer)
{
	DWORD _dwBytesRecorded = 0;
	while(1)
	{
		WaitForSingleObject(hevent, INFINITE);
		if(buff[_dwBufferCount]->dwFlags & WHDR_DONE)
		{
			_dwBytesRecorded = buff[_dwBufferCount]->dwBytesRecorded;
			memcpy(pWAVBuffer, buff[_dwBufferCount]->lpData, buff[_dwBufferCount]->dwBytesRecorded);
			waveInAddBuffer(hwi, buff[_dwBufferCount], sizeof(WAVEHDR));
		}

		if(_dwBufferCount == __dwNumBuffers -1)
		{
			_dwBufferCount = 0;
		} else {
			_dwBufferCount++;
		}

		if (_dwBytesRecorded > 0)
			break;
	}
	dwTotalBufferLength += _dwBytesRecorded;
	return _dwBytesRecorded;
}

// Define: createWAVEFile(const char*);
bool waveCapture::createWAVEFile(const char * szFilePath)
{
	_szFilePath = szFilePath;
	char nullbuff[44];
	ZeroMemory(&nullbuff, sizeof(nullbuff));

	hFile.open( szFilePath, std::ios::out|std::ios::binary );
	hFile.write(nullbuff, 44);

	return true;
}

// Define: saveWAVEChunk(const char*)
bool waveCapture::saveWAVEChunk(const char * pWAVBuffer, const DWORD dwBuffLen)
{
	hFile.write(pWAVBuffer, dwBuffLen);
	return true;
}

// Define: closeWAVEFile();
bool waveCapture::closeWAVEFile()
{
	// close file and re-open it to add the header at the beginning:
	hFile.close();

	DWORD total_buff = getTotalBytesRecorded();
	MMCKINFO mmckinfo;
	MMCKINFO mmckinfoSubchunk;
	MMCKINFO mmckinfoData;

	memset(&mmckinfo, 0, sizeof(mmckinfo));
	memset(&mmckinfoSubchunk, 0, sizeof(mmckinfoSubchunk));
	memset(&mmckinfoData, 0, sizeof(mmckinfoData));

	// Open the WAVE file using mmio (no create!)
	// creating a file using mmio
	HMMIO hfile;
	hfile = mmioOpenA((LPSTR)_szFilePath, NULL, MMIO_WRITE);
	mmioSeek(hfile, 0, SEEK_SET);
	//step 1 create riff chunk
	mmckinfo.fccType = mmioFOURCC('W','A','V','E');
	mmckinfo.cksize = (36 + total_buff);
	mmioCreateChunk(hfile, &mmckinfo, MMIO_CREATERIFF);
	
	//step 2 create fmt chunk
	//creating fmt chunk also includes writing formatex to this chunk
	mmckinfoSubchunk.ckid   = mmioFOURCC('f','m','t',' ');
	mmckinfoSubchunk.cksize = sizeof(WAVEFORMATEX);

	mmioCreateChunk(hfile, &mmckinfoSubchunk, 0);
	mmioWrite(hfile, (char*)&wf, sizeof(wf));
	mmioAscend(hfile, &mmckinfoSubchunk, 0);

	//step 3 creating data chunk
	//creating this chunk includes writing actual voice data
	mmckinfoData.ckid=mmioFOURCC('d','a','t','a');
	mmckinfoData.cksize = total_buff;
	mmioCreateChunk(hfile, &mmckinfoData, 0);

	mmioClose( MMIO_READ, MMIO_FHOPEN );

	return true;
}
