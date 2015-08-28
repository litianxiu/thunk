#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ddraw.h>
#include <Mmreg.h>
#include <MMSystem.h>
#include <dsound.h>
#include <ddraw.h>
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"ddraw.lib")
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")

#include "../common/TxBlobString.hpp"
#include "../common/TxSystemDependent.hpp"

class CAudioWaveOutput
{
private:
	TxSystemDependent::TxEventWrap m_event;
	HWAVEOUT m_hWaveOut;
	BOOL b_hWaveOut;
	WAVEFORMATEX mWaveFormat;
	BOOL bPlaySoundDoing;
	struct tagUnitInfo
	{
		TxBlobString data;
		int nSamplesPerSec;
		int wBitsPerSample;
		int nChannels;
	};
	std::list<TxCppPlatform::shared_ptr<tagUnitInfo>> mListAudioData;
	TxSystemDependent::TxMutexWrap mAudioDataMutex;
	TxSystemDependent::TxThreadWrap mOutputThread;
	BOOL bThreadRunning;
private:
	static void CALLBACK _g_waveOutProc(HWAVEOUT _hWaveOut,UINT _uMsg,DWORD _dwInstance,DWORD _dwParam1,DWORD _dwParam2)
	{
		CAudioWaveOutput* pThis=(CAudioWaveOutput*)_dwInstance;
		switch(_uMsg)
		{
		case WOM_DONE:
			pThis->bPlaySoundDoing=FALSE;
			pThis->m_event.setEvent();
			break;
		case WOM_CLOSE:
			pThis->bPlaySoundDoing=FALSE;
			pThis->bThreadRunning=FALSE;
			pThis->mAudioDataMutex.lock();
			pThis->mListAudioData.clear();
			pThis->mAudioDataMutex.unlock();
		case WOM_OPEN:
			pThis->m_event.setEvent();
			break;
		case WIM_OPEN:
			break;
		case WIM_CLOSE:
			break;
		case WIM_DATA:
			break;
		default:
			assert(0);
			break;
		}
	}
	void _uninit_wave()
	{
		if(this->b_hWaveOut)
		{
			::waveOutClose(m_hWaveOut);
			this->b_hWaveOut=FALSE;
		}
	}
	void _init_wave(int _nSamplesPerSec,int _wBitsPerSample,int _nChannels)
	{
		if(_nSamplesPerSec!=(int)this->mWaveFormat.nSamplesPerSec
			||_wBitsPerSample!=(int)this->mWaveFormat.wBitsPerSample
			||_nChannels!=(int)this->mWaveFormat.nChannels
			||!this->b_hWaveOut)
		{
			this->_uninit_wave();
			::memset(&this->mWaveFormat,0,sizeof(this->mWaveFormat));
			this->mWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			this->mWaveFormat.cbSize = sizeof(this->mWaveFormat);
			this->mWaveFormat.nSamplesPerSec = _nSamplesPerSec;
			this->mWaveFormat.wBitsPerSample = _wBitsPerSample;
			this->mWaveFormat.nChannels = _nChannels;
			this->mWaveFormat.nBlockAlign = (this->mWaveFormat.wBitsPerSample >> 3) * this->mWaveFormat.nChannels;
			this->mWaveFormat.nAvgBytesPerSec = this->mWaveFormat.nBlockAlign * this->mWaveFormat.nSamplesPerSec;
			MMRESULT lc_res=::waveOutOpen(&this->m_hWaveOut,WAVE_MAPPER,&this->mWaveFormat,
				(DWORD_PTR)_g_waveOutProc,(DWORD_PTR)this,CALLBACK_FUNCTION);
			if(lc_res==MMSYSERR_NOERROR)
			{
				this->b_hWaveOut=TRUE;
			}
			else
			{
				assert(0);
			}
		}
	}
	static void _g_output_audio_thread_cb_(void *_arg1,void *_arg2)
	{
		(void)_arg1;
		(void)_arg2;
		CAudioWaveOutput* pThis=(CAudioWaveOutput*)_arg1;
		pThis->_output_audio_thread_cb();
	}
	void _output_audio_thread_cb()
	{
		while(this->bThreadRunning)
		{
			this->m_event.waitEvent();
			if(!this->bThreadRunning)
				break;
			if(this->bPlaySoundDoing)
				continue;

			while(this->bThreadRunning)
			{
				TxCppPlatform::shared_ptr<tagUnitInfo> spAudioElem;
				this->mAudioDataMutex.lock();
				if(this->mListAudioData.size()>0)
				{
					spAudioElem=this->mListAudioData.front();
					this->mListAudioData.pop_front();
				}
				this->mAudioDataMutex.unlock();
				if(!spAudioElem)
					break;
				tagUnitInfo *lc_pAudioElem=spAudioElem.get();
				this->_init_wave(lc_pAudioElem->nSamplesPerSec,lc_pAudioElem->wBitsPerSample,lc_pAudioElem->nChannels);
				WAVEHDR lc_header={0};
				lc_header.dwBufferLength = (int)lc_pAudioElem->data.size();
				lc_header.lpData = (LPSTR)lc_pAudioElem->data.data();
				lc_header.dwFlags=WHDR_DONE;
				this->bPlaySoundDoing=TRUE;
				::waveOutPrepareHeader(m_hWaveOut, &lc_header, sizeof(WAVEHDR));
				::waveOutWrite(m_hWaveOut, &lc_header, sizeof(WAVEHDR));
				while(::waveOutUnprepareHeader(m_hWaveOut,&lc_header,sizeof(WAVEHDR))==WAVERR_STILLPLAYING)
				{
					this->m_event.waitEvent();
					if(!this->bThreadRunning)
						break;
				}
			}
		}
		this->_uninit_wave();
	}
public :
	CAudioWaveOutput()
	{
		::memset(&this->mWaveFormat,0,sizeof(this->mWaveFormat));
		this->b_hWaveOut=FALSE;
		this->bPlaySoundDoing=FALSE;
		//WAVEFORMATEX lc_waveFormat={0};//数据格式结构
		//lc_waveFormat.cbSize = sizeof(lc_waveFormat);
		//lc_waveFormat.nSamplesPerSec = 44100;
		//lc_waveFormat.wBitsPerSample = 16;
		//lc_waveFormat.nChannels = 2;
		//lc_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		//lc_waveFormat.nBlockAlign = (lc_waveFormat.wBitsPerSample >> 3) * lc_waveFormat.nChannels;
		//lc_waveFormat.nAvgBytesPerSec = lc_waveFormat.nBlockAlign * lc_waveFormat.nSamplesPerSec;
		this->bThreadRunning=TRUE;
		this->mOutputThread.create(_g_output_audio_thread_cb_,this,NULL);
	}
	~CAudioWaveOutput()
	{
		this->bThreadRunning=FALSE;
		this->m_event.setEvent();
		this->mOutputThread.join();
	}
	void push(int _iListMaxCount,const void *_data,int _size,int _nSamplesPerSec,int _wBitsPerSample,int _nChannels)
	{
		TxCppPlatform::shared_ptr<tagUnitInfo> sp_elem_tem(new tagUnitInfo());
		sp_elem_tem->data.assign((const char*)_data,_size);
		sp_elem_tem->nSamplesPerSec=_nSamplesPerSec;
		sp_elem_tem->wBitsPerSample=_wBitsPerSample;
		sp_elem_tem->nChannels=_nChannels;
		this->mAudioDataMutex.lock();
		while((int)this->mListAudioData.size()>_iListMaxCount)
			this->mListAudioData.pop_front();
		this->mListAudioData.push_back(sp_elem_tem);
		this->mAudioDataMutex.unlock();
		this->m_event.setEvent();
	}
	static void run()
	{
		static const int BUFFERNOTIFYSIZE=192000;
		static const int sample_rate=44100;  //PCM sample rate  
		static const int channels=2;         //PCM channel number  
		static const int bits_per_sample=16; //bits per sample
		static unsigned char buf[BUFFERNOTIFYSIZE];
		const int buf_len=BUFFERNOTIFYSIZE;
		FILE * fp=fopen("NocturneNo2inEflat_44.1k_s16le.pcm","rb"); 

		CAudioWaveOutput m_AudioWaveOutput;
		for(int i=0;;i++)
		{
			if(i%10==0)
			gets((char*)buf);
			if(fread(buf,1,buf_len,fp)!=buf_len)
			{
				fseek(fp, 0, SEEK_SET);  
				fread(buf,1,buf_len,fp);
			}
			m_AudioWaveOutput.push(10,buf,buf_len,sample_rate,bits_per_sample,channels);
		}

		fclose(fp);
	}
};

class CAudioWaveOutput1
{
private:
	TxSystemDependent::TxEventWrap m_event;
	HWAVEOUT m_hWaveOutHandle;
	BOOL b_hWaveOutHandle;
	struct tagUnitInfo
	{
		TxBlobString data;
	};
	std::list<TxCppPlatform::shared_ptr<tagUnitInfo>> mListAudioData;
private:
	static void CALLBACK _g_waveOutProc(HWAVEOUT _hWaveOut,UINT _uMsg,DWORD _dwInstance,DWORD _dwParam1,DWORD _dwParam2)
	{
	}
	void _waveOutProc(HWAVEOUT _hWaveOut,UINT _uMsg,DWORD _dwInstance,DWORD _dwParam1,DWORD _dwParam2)
	{
		CAudioWaveOutput* pThis=(CAudioWaveOutput*)_dwInstance;
		switch(_uMsg)
		{
		case WOM_DONE:
			{
				TxCppPlatform::shared_ptr<tagUnitInfo> spAudioElem;
				pThis->mAudioDataMutex.lock();
				if(this->mListAudioData.size()>0)
				{
					spAudioElem=this->mListAudioData.front();
					this->mListAudioData.pop_front();
				}
				pThis->mAudioDataMutex.unlock();

				WAVEHDR lc_header={0};
				lc_header.dwBufferLength = (int)lc_pAudioElem->data.size();
				lc_header.lpData = (LPSTR)lc_pAudioElem->data.data();
				lc_header.dwFlags=WHDR_DONE;
				this->bPlaySoundDoing=TRUE;
				::waveOutPrepareHeader(m_hWaveOut, &lc_header, sizeof(WAVEHDR));
				::waveOutWrite(m_hWaveOut, &lc_header, sizeof(WAVEHDR));
				while(::waveOutUnprepareHeader(m_hWaveOut,&lc_header,sizeof(WAVEHDR))==WAVERR_STILLPLAYING)
				{
					this->m_event.waitEvent();
					if(!this->bThreadRunning)
						break;
				}
			}
			break;
		case WOM_CLOSE:
			pThis->mAudioDataMutex.lock();
			pThis->mListAudioData.clear();
			pThis->mAudioDataMutex.unlock();
		case WOM_OPEN:
			pThis->m_event.setEvent();
			break;
		case WIM_OPEN:
			break;
		case WIM_CLOSE:
			break;
		case WIM_DATA:
			break;
		default:
			assert(0);
			break;
		}
	}
	static void _g_output_audio_thread_cb_(void *_arg1,void *_arg2)
	{
		(void)_arg1;
		(void)_arg2;
		CAudioWaveOutput* pThis=(CAudioWaveOutput*)_arg1;
		pThis->_output_audio_thread_cb();
	}
	void _output_audio_thread_cb()
	{
		while(this->bThreadRunning)
		{
			this->m_event.waitEvent();
			if(!this->bThreadRunning)
				break;
			if(this->bPlaySoundDoing)
				continue;

			while(this->bThreadRunning)
			{
				if(!spAudioElem)
					break;
				tagUnitInfo *lc_pAudioElem=spAudioElem.get();
				this->_init_wave(lc_pAudioElem->nSamplesPerSec,lc_pAudioElem->wBitsPerSample,lc_pAudioElem->nChannels);
			}
		}
		this->_uninit_wave();
	}
public :
	CAudioWaveOutput1()
	{
		::memset(&this->mWaveFormat,0,sizeof(this->mWaveFormat));
		this->b_hWaveOut=FALSE;
		this->bPlaySoundDoing=FALSE;
		//WAVEFORMATEX lc_waveFormat={0};//数据格式结构
		//lc_waveFormat.cbSize = sizeof(lc_waveFormat);
		//lc_waveFormat.nSamplesPerSec = 44100;
		//lc_waveFormat.wBitsPerSample = 16;
		//lc_waveFormat.nChannels = 2;
		//lc_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		//lc_waveFormat.nBlockAlign = (lc_waveFormat.wBitsPerSample >> 3) * lc_waveFormat.nChannels;
		//lc_waveFormat.nAvgBytesPerSec = lc_waveFormat.nBlockAlign * lc_waveFormat.nSamplesPerSec;
		this->bThreadRunning=TRUE;
		this->b_hWaveOutHandle=FALSE;
	}
	~CAudioWaveOutput1()
	{
		if(this->b_hWaveOutHandle)
		{
			::waveOutClose(m_hWaveOutHandle);
			this->b_hWaveOutHandle=FALSE;
		}
	}
	bool init(int _nSamplesPerSec,int _wBitsPerSample,int _nChannels)
	{
		assert(this->b_hWaveOutHandle==FALSE);
		WAVEFORMATEX lc_WaveFormat;
		::memset(&lc_WaveFormat,0,sizeof(lc_WaveFormat));
		lc_WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
		lc_WaveFormat.cbSize = sizeof(lc_WaveFormat);
		lc_WaveFormat.nSamplesPerSec = _nSamplesPerSec;
		lc_WaveFormat.wBitsPerSample = _wBitsPerSample;
		lc_WaveFormat.nChannels = _nChannels;
		lc_WaveFormat.nBlockAlign = (lc_WaveFormat.wBitsPerSample >> 3) * lc_WaveFormat.nChannels;
		lc_WaveFormat.nAvgBytesPerSec = lc_WaveFormat.nBlockAlign * lc_WaveFormat.nSamplesPerSec;
		MMRESULT lc_res=::waveOutOpen(&this->m_hWaveOutHandle,WAVE_MAPPER,&lc_WaveFormat,
			(DWORD_PTR)_g_waveOutProc,(DWORD_PTR)this,CALLBACK_FUNCTION);
		return lc_res==MMSYSERR_NOERROR;
	}
	void push(int _iListMaxCount,const void *_data,int _size)
	{
		TxCppPlatform::shared_ptr<tagUnitInfo> sp_elem_tem(new tagUnitInfo());
		sp_elem_tem->data.assign((const char*)_data,_size);
		sp_elem_tem->nSamplesPerSec=_nSamplesPerSec;
		sp_elem_tem->wBitsPerSample=_wBitsPerSample;
		sp_elem_tem->nChannels=_nChannels;
		this->mAudioDataMutex.lock();
		while((int)this->mListAudioData.size()>_iListMaxCount)
			this->mListAudioData.pop_front();
		this->mListAudioData.push_back(sp_elem_tem);
		this->mAudioDataMutex.unlock();
		this->m_event.setEvent();
	}
	static void run()
	{
		static const int BUFFERNOTIFYSIZE=192000;
		static const int sample_rate=44100;  //PCM sample rate  
		static const int channels=2;         //PCM channel number  
		static const int bits_per_sample=16; //bits per sample
		static unsigned char buf[BUFFERNOTIFYSIZE];
		const int buf_len=BUFFERNOTIFYSIZE;
		FILE * fp=fopen("NocturneNo2inEflat_44.1k_s16le.pcm","rb"); 

		CAudioWaveOutput m_AudioWaveOutput;
		for(int i=0;;i++)
		{
			if(i%10==0)
				gets((char*)buf);
			if(fread(buf,1,buf_len,fp)!=buf_len)
			{
				fseek(fp, 0, SEEK_SET);  
				fread(buf,1,buf_len,fp);
			}
			m_AudioWaveOutput.push(10,buf,buf_len,sample_rate,bits_per_sample,channels);
		}

		fclose(fp);
	}
};
