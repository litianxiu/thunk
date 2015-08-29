#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <Mmreg.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <dsound.h>
#include <MMSystem.h>
#include "../common/TxBlobString.hpp"
#include "../common/TxSystemDependent.hpp"
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dsound.lib")


class CDirectSoundOutput
{
public :
	struct tagAudioFrame
	{
		TxBlobString data;
		int nSamplesPerSec;
		int wBitsPerSample;
		int nChannels;
	};
private:
	enum { e_MAX_AUDIO_BUF=4, };
	IDirectSound8 *m_pDS;
	IDirectSoundBuffer8 *m_pDSBuffer8;
	IDirectSoundBuffer *m_pDSBuffer;
	HANDLE m_notify_event[e_MAX_AUDIO_BUF];
	LONG bThreadRunning;
	std::list<TxCppPlatform::shared_ptr<tagAudioFrame>> mListAudioData;
	TxSystemDependent::TxMutexWrap mAudioMutex;
	TxSystemDependent::TxEventWrap mPushDataEvent;
	TxSystemDependent::TxThreadWrap mThread;
	float fVolumeRatio;

	struct {
		int iDataSize;
		int nSamplesPerSec;
		int wBitsPerSample;
		int nChannels;
	} mRecordInfo;
private:
	static void _g_thread_cb_(void *_arg1,void *_arg2) { (void)_arg1; (void)_arg2; ((CDirectSoundOutput*)_arg1)->_thread_cb_(); }
	void _thread_cb_()
	{
		DWORD lc_offset=0;
		int lc_event_index=0;
		float lc_fVolumeRatio=-1;
		while(this->bThreadRunning)
		{
			while(this->bThreadRunning)
			{
				this->mPushDataEvent.waitEvent(500);
				if(!this->bThreadRunning)
					break;
				this->mAudioMutex.lock();
				BOOL bLcBreak=this->mListAudioData.size()>0;
				this->mAudioMutex.unlock();
				if(bLcBreak)
					break;
			}
			while(this->bThreadRunning)
			{
				TxCppPlatform::shared_ptr<tagAudioFrame> spLcFrame;
				this->mAudioMutex.lock();
				if(this->mListAudioData.size()>0)
				{
					spLcFrame=this->mListAudioData.front();
					this->mListAudioData.pop_front();
				}
				if(this->fVolumeRatio>=0)
				{
					lc_fVolumeRatio=this->fVolumeRatio;
					this->fVolumeRatio=-1;
				}
				this->mAudioMutex.unlock();
				if(lc_fVolumeRatio>=0&&this->m_pDSBuffer!=NULL&&this->m_pDSBuffer8!=NULL)
				{
					//DSBVOLUME_MIN-DSBVOLUME_MAX 
					const LONG ll_tag=DSBVOLUME_MAX-DSBVOLUME_MIN;
					LONG lLcVolumeRatio=(LONG)(lc_fVolumeRatio*(ll_tag)+0.5f);
					if(lLcVolumeRatio<0) lLcVolumeRatio=0;
					else if(lLcVolumeRatio>ll_tag) lLcVolumeRatio=ll_tag;
					lLcVolumeRatio+=DSBVOLUME_MIN;
					HRESULT sdfffds=this->m_pDSBuffer->SetVolume(lLcVolumeRatio);
					HRESULT sdffds=this->m_pDSBuffer8->SetVolume(lLcVolumeRatio);
					lc_fVolumeRatio=-1;
				}
				if(!spLcFrame)
					break;
				__thread_init_(spLcFrame);
				if(m_pDSBuffer8!=NULL&&m_pDSBuffer!=NULL)
				{
					LPVOID lc_buf=NULL;
					DWORD  lc_buf_len=0;
					m_pDSBuffer8->Lock(lc_offset,spLcFrame->data.size(),&lc_buf,&lc_buf_len,NULL,NULL,0);
					assert(lc_buf_len==spLcFrame->data.size());
					if((int)lc_buf_len>(int)spLcFrame->data.size())
						lc_buf_len=spLcFrame->data.size();
					::memcpy(lc_buf,spLcFrame->data.data(),spLcFrame->data.size());
					lc_offset+=lc_buf_len;
					lc_offset %= (spLcFrame->data.size() * e_MAX_AUDIO_BUF);
					lc_offset=lc_event_index*spLcFrame->data.size();
					m_pDSBuffer8->Unlock(lc_buf,lc_buf_len,NULL,0);
					while(this->bThreadRunning)
					{
						lc_event_index=::WaitForMultipleObjects (e_MAX_AUDIO_BUF, this->m_notify_event, FALSE, INFINITE);
						if((lc_event_index >=WAIT_OBJECT_0)&&(lc_event_index <WAIT_OBJECT_0+e_MAX_AUDIO_BUF))
							break;
					}
				}
			}
		}
		__thread_uninit_();
	}
	void __thread_init_(TxCppPlatform::shared_ptr<tagAudioFrame> &_spAudioFrame)
	{
		tagAudioFrame *lc_pAudioFrame=_spAudioFrame.get();

		if(this->m_pDSBuffer==NULL
			||this->m_pDSBuffer8==NULL
			||this->mRecordInfo.iDataSize!=lc_pAudioFrame->data.size()
			||this->mRecordInfo.nChannels!=lc_pAudioFrame->nChannels
			||this->mRecordInfo.nSamplesPerSec!=lc_pAudioFrame->nSamplesPerSec
			||this->mRecordInfo.wBitsPerSample!=lc_pAudioFrame->wBitsPerSample)
		{
			assert((this->m_pDSBuffer==NULL&&this->m_pDSBuffer8==NULL)||lc_pAudioFrame->data.size()==0);
			this->__thread_uninit_();
			if(lc_pAudioFrame->data.size()>0)
			{
				this->mRecordInfo.iDataSize=lc_pAudioFrame->data.size();
				this->mRecordInfo.nChannels=lc_pAudioFrame->nChannels;
				this->mRecordInfo.nSamplesPerSec=lc_pAudioFrame->nSamplesPerSec;
				this->mRecordInfo.wBitsPerSample=lc_pAudioFrame->wBitsPerSample;

				DSBUFFERDESC lc_dsbd={0};
				WAVEFORMATEX lc_WAVEFORMATEX={0};
				lc_dsbd.dwSize=sizeof(lc_dsbd);
				lc_dsbd.dwFlags=DSBCAPS_CTRLVOLUME;
				lc_dsbd.dwFlags=DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY |DSBCAPS_GETCURRENTPOSITION2;
				lc_dsbd.dwBufferBytes=e_MAX_AUDIO_BUF*lc_pAudioFrame->data.size(); 
				lc_dsbd.lpwfxFormat=&lc_WAVEFORMATEX;
				lc_WAVEFORMATEX.wFormatTag=WAVE_FORMAT_PCM;
				lc_WAVEFORMATEX.cbSize=sizeof(lc_WAVEFORMATEX);
				lc_WAVEFORMATEX.nChannels=lc_pAudioFrame->nChannels;
				lc_WAVEFORMATEX.nSamplesPerSec=lc_pAudioFrame->nSamplesPerSec;
				lc_WAVEFORMATEX.wBitsPerSample=lc_pAudioFrame->wBitsPerSample;
				lc_WAVEFORMATEX.nAvgBytesPerSec=lc_WAVEFORMATEX.nSamplesPerSec*(lc_WAVEFORMATEX.wBitsPerSample/8)*lc_WAVEFORMATEX.nChannels;
				lc_WAVEFORMATEX.nBlockAlign=(lc_WAVEFORMATEX.wBitsPerSample/8)*lc_WAVEFORMATEX.nChannels;

				HRESULT lc_res;
				lc_res=this->m_pDS->CreateSoundBuffer(&lc_dsbd,&this->m_pDSBuffer,NULL);
				if(lc_res!=DS_OK)
				{
					assert(0);
					return ;
				}
				lc_res=this->m_pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8,(LPVOID*)&this->m_pDSBuffer8);
				if(lc_res!=DS_OK)
				{
					assert(0);
					return  ;
				}
				IDirectSoundNotify8 *lc_pDSNotify;
				lc_res=this->m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify,(LPVOID*)&lc_pDSNotify);
				if(lc_res!=DS_OK)
				{
					assert(0);
					return  ;
				}
				DSBPOSITIONNOTIFY lc_pDSPosNotify[e_MAX_AUDIO_BUF]={0};
				for(int i =0;i<e_MAX_AUDIO_BUF;i++){
					lc_pDSPosNotify[i].dwOffset =i*lc_pAudioFrame->data.size();
					lc_pDSPosNotify[i].hEventNotify=this->m_notify_event[i];
				}
				lc_pDSNotify->SetNotificationPositions(e_MAX_AUDIO_BUF,lc_pDSPosNotify);
				lc_pDSNotify->Release();
				this->m_pDSBuffer8->SetCurrentPosition(0);
				this->m_pDSBuffer8->Play(0,0,DSBPLAY_LOOPING);
			}
		}

	}
	void __thread_uninit_()
	{
		if(this->m_pDSBuffer8!=NULL)
		{
			this->m_pDSBuffer8->Release();
			this->m_pDSBuffer8=NULL;
		}
		if(this->m_pDSBuffer!=NULL)
		{
			this->m_pDSBuffer->Release();
			this->m_pDSBuffer=NULL;
		}
	}
public :
	CDirectSoundOutput(HWND _hWnd)
	{
		this->m_pDS=0;					
		this->m_pDSBuffer8=NULL;	//used to manage sound buffers.
		this->m_pDSBuffer=NULL;		
		//this->m_pDSPosNotify[MAX_AUDIO_BUF];
		//m_event[MAX_AUDIO_BUF];
		HRESULT lc_res=DS_OK;
		this->m_pDS=NULL;
		lc_res=::DirectSoundCreate8(NULL,&this->m_pDS,NULL);
		if(lc_res!=DS_OK)
		{
			::MessageBox(NULL,_T("请安装DirectX库"),_T(""),MB_OK);
		}
		lc_res=this->m_pDS->SetCooperativeLevel(_hWnd,DSSCL_NORMAL);
		if(lc_res!=DS_OK)
		{
			::MessageBox(NULL,_T("请安装DirectX库"),_T(""),MB_OK);
		}
		for(int i =0;i<e_MAX_AUDIO_BUF;i++)
		{
			this->m_notify_event[i]=::CreateEvent(NULL,false,false,NULL);
		}
		this->bThreadRunning=TRUE;
		mThread.create(_g_thread_cb_,this,NULL);
	}
	~CDirectSoundOutput()
	{
		this->bThreadRunning=FALSE;
		if(this->m_notify_event[0]!=NULL)
			::SetEvent(this->m_notify_event[0]);
		this->mPushDataEvent.setEvent();
		this->mThread.join();
		assert(this->m_pDSBuffer==NULL);
		assert(this->m_pDSBuffer8==NULL);
		if(this->m_pDS!=NULL)
		{
			this->m_pDS->Release();
			this->m_pDS=NULL;
		}
		for(int i =0;i<e_MAX_AUDIO_BUF;i++)
		{
			if(this->m_notify_event[i]!=NULL)
			{
				::CloseHandle(this->m_notify_event[i]);
				this->m_notify_event[i]=NULL;
			}
		}
	}
	void push(TxCppPlatform::shared_ptr<tagAudioFrame> &_sp_elem_tem)
	{
		this->mAudioMutex.lock();
		while((int)this->mListAudioData.size()>1)
			this->mListAudioData.pop_front();
		this->mListAudioData.push_back(_sp_elem_tem);
		this->mAudioMutex.unlock();
		this->mPushDataEvent.setEvent();
	}
	void setVolumeRatio(float _fVolumeRatio)
	{
		if(_fVolumeRatio<0)
			_fVolumeRatio=0;
		else if(_fVolumeRatio>1)
			_fVolumeRatio=1;
		this->mAudioMutex.lock();
		this->fVolumeRatio=_fVolumeRatio;
		this->mAudioMutex.unlock();
	}
};
