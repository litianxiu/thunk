#include "StdAfx.h"

#include "CDuiFfmpegPlayWndBasic.h"


static AVFrame *__gg_ffmpeg_wnd_mgr_alloc_audio_frame(enum AVSampleFormat _sample_fmt,
								  uint64_t _channel_layout,int _sample_rate, int _nb_samples)
{
	AVFrame *lc_frame = ::av_frame_alloc();
	lc_frame->format = _sample_fmt;
	lc_frame->channel_layout = _channel_layout;
	lc_frame->sample_rate = _sample_rate;
	lc_frame->nb_samples = _nb_samples;
	if (_nb_samples>0)
	{
		int nnn = av_frame_get_buffer(lc_frame, 0);
		if (nnn < 0)
		{
			::MessageBox(NULL,_T("av_frame_get_buffer() failure"),_T("av_frame_get_buffer() failure"),MB_OK);
		}
	}
	return lc_frame;
}


CDuiFfmpegPlayWndBasic::tagSwrContext::tagSwrContext()
{
	this->swr_ctx=NULL;
	this->pAudioMidFrame=NULL;
	//this->pp_dst_data=NULL;
}

CDuiFfmpegPlayWndBasic::tagSwrContext::~tagSwrContext()
{
	if(this->pAudioMidFrame!=NULL)
	{
		::av_frame_free(&this->pAudioMidFrame);
		this->pAudioMidFrame=NULL;
	}
	if(this->swr_ctx!=NULL)
	{
		::swr_free(&this->swr_ctx);
		this->swr_ctx=NULL;
	}










	//if(this->pp_dst_data!=NULL)
	//{
	//	av_freep(&this->pp_dst_data[0]);
	//	this->pp_dst_data=NULL;
	//}

	//int lc_dst_nb_samples =
	//	::av_rescale_rnd(_audio_frame->nb_samples, this->m_dst_rate, this->m_sample_rate, AV_ROUND_UP);

	///* buffer is going to be directly written to a rawaudio file, no alignment */
	//int lc_dst_nb_channels = ::av_get_channel_layout_nb_channels(this->m_dst_ch_layout);
	//ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, lc_dst_nb_channels,
	//	lc_dst_nb_samples,this->m_dst_sample_fmt, 0);
}

void CDuiFfmpegPlayWndBasic::tagSwrContext::build(TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spPcmFrame,AVFrame *_audio_frame,AVCodecContext *_codec)
{
	//int sdffds=AV_CH_LAYOUT_STEREO;
	_spPcmFrame->reset();
	if(this->swr_ctx==NULL
		||(uint64_t)this->m_ch_layout!=(uint64_t)_audio_frame->channel_layout
		||this->m_sample_fmt!=(AVSampleFormat)_audio_frame->format
		||this->m_sample_rate!=_audio_frame->sample_rate
		||this->m_channels!=_audio_frame->channels)
	{
		for(int n_lc_iii=0;;n_lc_iii++)
		{
			if(this->swr_ctx!=NULL)
			{
				::swr_free(&this->swr_ctx);
				this->swr_ctx=NULL;
			}
			if(this->pAudioMidFrame!=NULL)
			{
				::av_frame_free(&this->pAudioMidFrame);
				this->pAudioMidFrame=NULL;
			}
			if(n_lc_iii!=0)
				break;
			this->m_ch_layout=(uint64_t)_audio_frame->channel_layout;
			this->m_sample_fmt=(AVSampleFormat)_audio_frame->format;
			this->m_sample_rate=_audio_frame->sample_rate;
			this->m_channels=_audio_frame->channels;

			int lc_dst_rate=this->m_sample_rate;
			uint64_t lc_dst_ch_layout=AV_CH_LAYOUT_STEREO;
			AVSampleFormat lc_dst_sample_fmt=AV_SAMPLE_FMT_S16;

			//		AV_CH_LAYOUT_STEREO,              //输出通道布局
			//		AVSampleFormat::AV_SAMPLE_FMT_S16,//输出格式
			//		44100,                            //输出采样率

			this->swr_ctx=::swr_alloc();
			::av_opt_set_int	   (this->swr_ctx, "in_channel_layout",    _audio_frame->channel_layout, 0);
			::av_opt_set_int       (this->swr_ctx, "in_channel_count",   _audio_frame->channels,       0);
			::av_opt_set_int       (this->swr_ctx, "in_sample_rate",     _audio_frame->sample_rate,    0);
			::av_opt_set_sample_fmt(this->swr_ctx, "in_sample_fmt",      _codec->sample_fmt, 0);
			::av_opt_set_int       (this->swr_ctx, "out_channel_layout",    lc_dst_ch_layout, 0);
			::av_opt_set_int       (this->swr_ctx, "out_channel_count",  this->m_channels,       0);
			::av_opt_set_int       (this->swr_ctx, "out_sample_rate",    lc_dst_rate,    0);
			::av_opt_set_sample_fmt(this->swr_ctx, "out_sample_fmt",     lc_dst_sample_fmt,     0);

			int dsffds=::swr_init(swr_ctx);
			if(dsffds>=0)
			{
				int lc_nb_samples=0;
				if (_codec->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)
				{
					lc_nb_samples = 10000;
				}
				else
				{
					lc_nb_samples = _codec->frame_size;
				}
				this->pAudioMidFrame=::__gg_ffmpeg_wnd_mgr_alloc_audio_frame(lc_dst_sample_fmt,lc_dst_ch_layout,lc_dst_rate,lc_nb_samples);
				break;
			}
		}
	}

	if(this->swr_ctx!=NULL&&this->pAudioMidFrame!=NULL)
	{
		int lc_dst_nb_samples = (int)::av_rescale_rnd(swr_get_delay(this->swr_ctx,this->m_sample_rate) + _audio_frame->nb_samples,
			_audio_frame->sample_rate,_audio_frame->sample_rate, AV_ROUND_UP);
		(void)lc_dst_nb_samples;
		assert(lc_dst_nb_samples == this->pAudioMidFrame->nb_samples&&lc_dst_nb_samples == _audio_frame->nb_samples);
		int lc_ret = ::swr_convert(this->swr_ctx,
			(uint8_t **)this->pAudioMidFrame->data,this->pAudioMidFrame->nb_samples,
			(const uint8_t **)_audio_frame->data,_audio_frame->nb_samples);
		int lc_dst_bufsize = av_samples_get_buffer_size(
			this->pAudioMidFrame->linesize,
			this->pAudioMidFrame->channels,
			this->pAudioMidFrame->nb_samples,
			(AVSampleFormat)this->pAudioMidFrame->format,
			1);
		//fwrite(dst_data[0], 1, dst_bufsize, dst_file);
		CDirectSoundOutput::tagAudioFrame *pRes=new CDirectSoundOutput::tagAudioFrame();
		_spPcmFrame->reset(pRes);
		//int nLcLen=_audio_frame->linesize[0]*_audio_frame->channels<< 1;//[]//
		//pRes->data.assign((const char*)this->pAudioMidFrame->data[0],lc_dst_bufsize);
		pRes->data.assign((const char*)this->pAudioMidFrame->data[0],lc_dst_bufsize);
		pRes->nSamplesPerSec=_audio_frame->sample_rate;
		pRes->wBitsPerSample=16;
		pRes->nChannels=_audio_frame->channels;

		//CDirectSoundOutput::tagAudioFrame *pRes=new CDirectSoundOutput::tagAudioFrame();
		//_spPcmFrame->reset(pRes);
		//int nLcLen=_audio_frame->linesize[0]*_audio_frame->channels<< 1;//[]//
		//pRes->data.resize(nLcLen,false);
		//pRes->nSamplesPerSec=_audio_frame->sample_rate;
		//pRes->wBitsPerSample=16;
		//pRes->nChannels=_audio_frame->channels;
		//int nLcRet=::swr_convert(this->pSC,
		//	(uint8_t**)const_cast<char*>(pRes->data.data()),//输出BUFFER
		//	nLcLen/_audio_frame->channels/av_get_bytes_per_sample(AV_SAMPLE_FMT_S16),//buffer长度能容纳每个通道多少个采样，输出缓冲器中每个通道的采样数
		//	(const uint8_t**)_audio_frame->data,
		//	_audio_frame->nb_samples//每个通道的采样数
		//	);
		//if(nLcRet>0)
		//{
		//	nLcLen=nLcRet *av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) *_audio_frame->channels;
		//	pRes->data.resize(nLcLen,true);
		//}
		//else
		//{
		//	_spPcmFrame->reset();
		//}
	}
	//size_t lc_unpadded_linesize=this->frame->nb_samples * (int)::av_get_bytes_per_sample((AVSampleFormat)this->frame->format);
}

CDuiFfmpegPlayWndBasic::tagStorePacketOp::~tagStorePacketOp()
{
	this->reset();
}

void CDuiFfmpegPlayWndBasic::tagStorePacketOp::reset()
{
	this->mPacketMutex.lock();
	std::list<AVPacket>::iterator iter=this->mListPacket.begin();
	std::list<AVPacket>::iterator iter_end=this->mListPacket.end();
	for(;iter!=iter_end;iter++)
		::av_free_packet(&(*iter));
	this->mListPacket.clear();
	this->iPacketTotalSize=0;
	this->mPacketMutex.unlock();
}

void CDuiFfmpegPlayWndBasic::tagStorePacketOp::pushPacket(AVPacket &_pkg)
{
	if(_pkg.size>0)
	{
		this->mPacketMutex.lock();
		this->mListPacket.push_back(_pkg);
		this->iPacketTotalSize+=_pkg.size;
		this->mPacketMutex.unlock();
	}
	else
	{
		::av_free_packet(&_pkg);
	}
}

bool CDuiFfmpegPlayWndBasic::tagStorePacketOp::popPacket(AVPacket *_pkg)
{
	bool res=false;
	this->mPacketMutex.lock();
	if(this->mListPacket.size()>0)
	{
		*_pkg=this->mListPacket.front();
		this->mListPacket.pop_front();
		this->iPacketTotalSize-=_pkg->size;
		res=true;
	}
	this->mPacketMutex.unlock();
	return res;
}

int CDuiFfmpegPlayWndBasic::tagStorePacketOp::getPacketBufferSize()
{
	int res=0;
	this->mPacketMutex.lock();
	res=this->iPacketTotalSize;
	this->mPacketMutex.unlock();
	return res;
}

void CDuiFfmpegPlayWndBasic::initialize()
{
	struct T_R { T_R() { ::av_register_all(); } };
	static T_R tem; (void)(&tem);
}

void CDuiFfmpegPlayWndBasic::_show_error(LPCTSTR _err)
{
	::MessageBox(NULL,_err,_T("严重错误"),MB_OK);
}

int CDuiFfmpegPlayWndBasic::_static_io_read_packet_func(void *_opaque,uint8_t *_buf, int _buf_size)
{
	CDuiFfmpegPlayWndBasic *pThis=(CDuiFfmpegPlayWndBasic*)_opaque;
	//int len=pThis->read_file_data_(pThis->llReadPos,_buf,_buf_size);
	if(!pThis->bThreadRunStatus) return -1;
	int len=pThis->spIReadFile->vfReadStream(_buf,_buf_size,pThis->llReadPos);
	pThis->bLastReadFileSuccess=TRUE;
	if(len>=0)
		pThis->llReadPos+=len;
	return len;
}

int64_t CDuiFfmpegPlayWndBasic::_static_io_seek_packet_func(void *_opaque, int64_t _offset, int _whence)
{
	CDuiFfmpegPlayWndBasic *pThis=(CDuiFfmpegPlayWndBasic*)_opaque;
	switch(_whence)
	{
	case SEEK_SET:
		pThis->llReadPos=_offset;
		break;
	case SEEK_CUR:
		pThis->llReadPos+=_offset;
		break;
	case SEEK_END:
		assert(0);
		//pThis->llReadPos=offset;
		break;
	default:
		break;
	}
	return 0;
}

CDuiFfmpegPlayWndBasic::CDuiFfmpegPlayWndBasic(IThreadCallBack *_pIThreadCallBack,TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> _spIReadFile)
	:pIThreadCallBack(_pIThreadCallBack)
{
	initialize();
	this->bLastReadFileSuccess=TRUE;
	this->llAvPtsStartTime=(long long)((((unsigned long long)-1)>>2)-1);
	this->spIReadFile=_spIReadFile;
	::memset(this->video_dst_data,0,sizeof(video_dst_data));
	::memset(this->video_dst_linesize,0,sizeof(video_dst_linesize));
	this->i_video_dst_bufsize=0;
	this->i_audio_stream_idx=-1;
	this->i_video_stream_idx=-1;
	this->llReadPos=0;
	this->llAvTotalTime=-1;
	this->mAvSeekRatioOp.fUserSeekFramePos=-1;

	this->fmt_ctx=avformat_alloc_context();
	if(this->fmt_ctx==NULL)
	{
		this->_show_error(_T("avformat_alloc_context() fail"));
	}
	this->avio_ctx_buffer = (uint8_t*)::av_mallocz((e_avio_ctx_buffer_size*3+64)*sizeof(uint8_t));
	if(this->avio_ctx_buffer==NULL)
	{
		this->_show_error(_T("av_malloc() fail"));
	}
	this->avio_ctx=avio_alloc_context(this->avio_ctx_buffer+e_avio_ctx_buffer_size,e_avio_ctx_buffer_size,0,this,&_static_io_read_packet_func,NULL,&_static_io_seek_packet_func);
	if (this->avio_ctx==NULL)
	{
		this->_show_error(_T("avio_alloc_context() fail"));
	}
	this->fmt_ctx->pb = avio_ctx;
	this->frame=av_frame_alloc();
	if (this->frame==NULL)
	{
		this->_show_error(_T("av_frame_alloc() fail"));
	}
	{
		this->mAvInfo.iMapAudioSumCount=0;
		this->mAvInfo.iMapVideoSumCount=0;
		this->mAvInfo.iTotalBufferSize=0;
		this->bThreadRunStatus=TRUE;
		this->mStorePacketOp.reset();
		this->mReadFileThread.create(_static_thread_call_back_,this,NULL);
	}
}

CDuiFfmpegPlayWndBasic::~CDuiFfmpegPlayWndBasic()
{
	this->bThreadRunStatus=FALSE;
	this->mReadPackEvent.setEvent();
	this->mReadFileThread.join();

	this->mStorePacketOp.reset();

	if(this->avio_ctx_buffer!=NULL)
	{
		::av_free(this->avio_ctx_buffer);
		this->avio_ctx_buffer=NULL;
	}
	if(this->frame!=NULL)
	{
		::av_frame_free(&this->frame);
		this->frame=NULL;
	}
	if(this->fmt_ctx!=NULL)
	{
		for(int i=0;i<(int)this->fmt_ctx->nb_streams;i++)
		{
			::avcodec_close(this->fmt_ctx->streams[i]->codec);
		}
	}
	this->i_audio_stream_idx=-1;
	this->i_video_stream_idx=-1;
	if(this->avio_ctx!=NULL)
	{
		::av_free(this->avio_ctx);
		this->avio_ctx=NULL;
	}
	if(this->video_dst_data[0]!=NULL)
	{
		av_freep(&this->video_dst_data[0]);
		this->video_dst_data[0]=NULL;
	}
	if(this->fmt_ctx!=NULL)
	{
		::avformat_free_context(this->fmt_ctx);
		this->fmt_ctx=NULL;
	}
}

bool CDuiFfmpegPlayWndBasic::_open_codec_context__(int *stream_idx,enum AVMediaType type)
{
	bool ret=false;
	do {
		int lc_ret, stream_index;
		AVCodecContext *dec_ctx = NULL;
		AVCodec *dec = NULL;
		AVDictionary *opts = NULL;

		lc_ret = av_find_best_stream(this->fmt_ctx, type, -1, -1, NULL, 0);
		if(lc_ret < 0)
		{
			break;
		}
		else
		{
			stream_index = lc_ret;
			AVStream *st = this->fmt_ctx->streams[stream_index];
			dec_ctx = st->codec;
			dec = avcodec_find_decoder(dec_ctx->codec_id);
			if (dec==NULL)
			{
				break;
			}
			if ((lc_ret = avcodec_open2(dec_ctx, dec, &opts)) < 0)
			{
				break;
			}
			*stream_idx = stream_index;
		}
		ret=true;
	} while(0);
	return ret;
}

void CDuiFfmpegPlayWndBasic::_static_thread_call_back_(void *_arg1,void *_arg2)
{
	(void)_arg1;
	(void)_arg2;
	((CDuiFfmpegPlayWndBasic*)_arg1)->_thread_call_back_();
}

void CDuiFfmpegPlayWndBasic::_thread_call_back_()
{
	int iLcOptionStep=0;
	BOOL bLcInitAvSign=FALSE;
	while(this->bThreadRunStatus)
	{
		if(bLcInitAvSign)
			this->mReadPackEvent.waitEvent();
		if(!this->bThreadRunStatus)
			break;
		
		switch(iLcOptionStep)
		{
		case 0:
			if(0)
			{
				if(::avformat_open_input(&this->fmt_ctx,NULL,NULL,NULL)>=0)
					iLcOptionStep++;
				else this->bThreadRunStatus=FALSE;
			}
			else
			{
				if(::avformat_open_input(&this->fmt_ctx,NULL,NULL,NULL)>=0)
					iLcOptionStep++;
				else this->bThreadRunStatus=FALSE;
			}
			break;
		case 1:
			if(::avformat_find_stream_info(this->fmt_ctx,NULL)>=0)
				iLcOptionStep++;
			else this->bThreadRunStatus=FALSE;
			break;
		case 2:
			if(this->_open_codec_context__(&this->i_video_stream_idx,AVMEDIA_TYPE_VIDEO))
			{
				int lc_ret=::av_image_alloc(this->video_dst_data,this->video_dst_linesize,
					this->fmt_ctx->streams[this->i_video_stream_idx]->codec->width,
					this->fmt_ctx->streams[this->i_video_stream_idx]->codec->height,
					this->fmt_ctx->streams[this->i_video_stream_idx]->codec->pix_fmt,
					1);
				if(lc_ret>=0)
				{
					this->i_video_dst_bufsize = lc_ret;
				}
			}
			else
			{
				this->i_video_stream_idx=-1;
			}
			iLcOptionStep++;
			break;
		case 3:
			if(this->_open_codec_context__(&this->i_audio_stream_idx,AVMEDIA_TYPE_AUDIO))
			{
			}
			else
			{
				this->i_audio_stream_idx=-1;
			}
			iLcOptionStep++;
			break;
		case 4:
			::av_dump_format(this->fmt_ctx,0,NULL,0);
			if(this->i_video_stream_idx>=0||this->i_audio_stream_idx>=0)
			{
				if(this->fmt_ctx->duration>=0)
					this->llAvTotalTime=1000*(this->fmt_ctx->duration)/AV_TIME_BASE;
				else this->llAvTotalTime=0;
				iLcOptionStep++;
			}
			else
			{
				this->bThreadRunStatus=FALSE;
			}
			break;
		default:
			{
				this->mAvSeekRatioOp.mSeekFramePosMutex.lock();
				float fLcSeekPos=this->mAvSeekRatioOp.fUserSeekFramePos;
				this->mAvSeekRatioOp.fUserSeekFramePos=-1.0f;
				this->mAvSeekRatioOp.mSeekFramePosMutex.unlock();
				if(fLcSeekPos>=-0.0001f&&this->llAvTotalTime>0)
				{
					long long llLcFramePos=(long long)((float)fLcSeekPos*(float)this->llAvTotalTime);
					if(llLcFramePos>this->llAvTotalTime)
						llLcFramePos=this->llAvTotalTime;
					else if(llLcFramePos<0) llLcFramePos=0;
					int lc_res=::av_seek_frame(this->fmt_ctx,-1,llLcFramePos*(AV_TIME_BASE/1000),AVSEEK_FLAG_ANY);(void)lc_res;
					if(lc_res>=0)
					{
						this->mStorePacketOp.reset();
					}
				}

				while(this->mStorePacketOp.getPacketBufferSize()<=2*e_avio_ctx_buffer_size)
				{
					AVPacket mLcOrgPacket={0};
					::av_init_packet(&mLcOrgPacket);
					if(::av_read_frame(this->fmt_ctx,&mLcOrgPacket)<0)
					{
						break;
					}
					this->mStorePacketOp.pushPacket(mLcOrgPacket);
				}
				bLcInitAvSign=TRUE;
				this->pIThreadCallBack->vfPlayAvInitialize(true);
			}
			break;
		}
	}
	this->bThreadRunStatus=FALSE;
	if(!bLcInitAvSign)
	{
		this->pIThreadCallBack->vfPlayAvInitialize(false);
	}
}

void CDuiFfmpegPlayWndBasic::_add_av_in_map_(long long _ll_time,TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spAudioFrame)
{
	assert((_spDdFrame!=NULL)^(_spAudioFrame!=NULL));
	tagUnitInfo tem;
	long long lc_time;
	if(_spAudioFrame!=NULL)
	{
		tem.spAudioFrame=(*_spAudioFrame);
		lc_time=(_ll_time<<1)|0;
		this->mAvInfo.iTotalBufferSize+=(*_spAudioFrame)->data.size();
	}
	else
	{
		tem.spVideoDdFrame=(*_spDdFrame);
		lc_time=(_ll_time<<1)|1;
		this->mAvInfo.iTotalBufferSize+=(*_spDdFrame)->getBufferSize();
	}
	std::map<long long,tagUnitInfo>::iterator iter=this->mAvInfo.mapAudioVideoFrame.find(lc_time);
	if(iter!=this->mAvInfo.mapAudioVideoFrame.end())
	{
		if(iter->second.spAudioFrame)
		{
			this->mAvInfo.iTotalBufferSize-=iter->second.spAudioFrame->data.size();
			this->mAvInfo.iMapAudioSumCount--;
		}
		else if(iter->second.spVideoDdFrame)
		{
			this->mAvInfo.iTotalBufferSize-=iter->second.spVideoDdFrame->getBufferSize();
			this->mAvInfo.iMapVideoSumCount--;
		}
	}
	this->mAvInfo.mapAudioVideoFrame[lc_time]=tem;
}



//
//int fdsfsddfsfsdAVFrameToAudio(AVFrame*frame,char**pBuf,int&len)
//{
//	int nRet= 0;
//
//	struct SwrContext *pSC =NULL;
//	//准备格式转换
//	pSC=swr_alloc_set_opts(NULL,
//		AV_CH_LAYOUT_STEREO,              //输出通道布局
//		AVSampleFormat::AV_SAMPLE_FMT_S16,//输出格式
//		44100,                            //输出采样率
//		AV_CH_LAYOUT_STEREO,              //输入通道布局
//		(AVSampleFormat)frame->format,    //输入格式
//		frame->sample_rate,               //输入采样率
//		NULL,NULL);
//	if(NULL == pSC)
//		return-1;
//	nRet=swr_init(pSC);
//	if(nRet)
//	{
//		TRACE(_T("swr init fail\n"));
//		return nRet;
//	}
//	//转换音频
//	nRet=swr_convert(pSC,
//		(uint8_t**)pBuf,//输出BUFFER
//		len/frame->channels/av_get_bytes_per_sample(AV_SAMPLE_FMT_S16),//buffer长度能容纳每个通道多少个采样，输出缓冲器中每个通道的采样数
//		(const uint8_t**)frame->data,
//		frame->nb_samples//每个通道的采样数
//		);
//	if(nRet> 0)
//	{
//		len=nRet *av_get_bytes_per_sample(AV_SAMPLE_FMT_S16) *frame->channels;
//		nRet= 0;
//	}
//	swr_free(&pSC);
//	return nRet;
//
//}

long long CDuiFfmpegPlayWndBasic::_buildGetAvTime(AVFrame *_av_frame,AVCodecContext *_av_codec)
{
	long long lc_pts_time=(long long)(_av_frame->pkt_pts*1000*::av_q2d(_av_codec->pkt_timebase));
	if(lc_pts_time<this->llAvPtsStartTime)
	{
		assert(this->llAvPtsStartTime>=(long long)((unsigned long long)0x7fffffffff));
		this->llAvPtsStartTime=lc_pts_time;
	}
	return lc_pts_time-this->llAvPtsStartTime;
}

CDuiFfmpegPlayWndBasic::EnumResultStatus CDuiFfmpegPlayWndBasic::readFrame(float *_fPlayRatio,long long *_ll_time,
																		   TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,
																		   TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spAudioFrame)
{
	if(_spDdFrame!=NULL)
		_spDdFrame->reset();
	if(_spAudioFrame!=NULL)
	_spAudioFrame->reset();
	while(this->mAvInfo.iTotalBufferSize<e_av_frame_buffer_max_size
		&&((this->i_video_stream_idx>=0&&this->mAvInfo.iMapVideoSumCount<e_video_frame_count)||(this->i_audio_stream_idx>=0&&this->mAvInfo.iMapAudioSumCount<this->e_audio_frame_count)))
	{
		AVPacket mLcOrgPacket;
		if(!this->mStorePacketOp.popPacket(&mLcOrgPacket)||mLcOrgPacket.size<=0)
			break;
		int i_lc_got_frame = 0;
		AVPacket mLcDataPacket=mLcOrgPacket;
		AVCodecContext *lc_codec=this->fmt_ctx->streams[mLcDataPacket.stream_index]->codec;
		while(mLcDataPacket.size>0)
		{
			int lc_decoded = mLcDataPacket.size;
			if(mLcDataPacket.stream_index==this->i_video_stream_idx)
			{
				int lc_res=::avcodec_decode_video2(lc_codec,this->frame,&i_lc_got_frame,&mLcDataPacket);
				if(lc_res<0)
				{
					assert(0);
					break;
				}
				lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
				if(i_lc_got_frame!=0&&this->frame->pkt_pts>=0)
				{
					::av_image_copy(this->video_dst_data,this->video_dst_linesize,
						(const uint8_t **)(this->frame->data),this->frame->linesize,
						lc_codec->pix_fmt,lc_codec->width,lc_codec->height);
					long long lc_pts_time=_buildGetAvTime(this->frame,lc_codec);
					TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> lc_sp_frame(new CDirectDrawFrameFormat());
					lc_sp_frame->load_data(video_dst_data[0],(int)this->i_video_dst_bufsize,lc_codec->width,lc_codec->height);
					switch(lc_codec->pix_fmt)
					{
					case AV_PIX_FMT_YUV420P:
						//lc_sp_frame->init_YUYV(NULL);
						lc_sp_frame->init_YV12(NULL);
						break;
					case AV_PIX_FMT_RGB555LE:
						lc_sp_frame->init_RGB555(NULL);
						break;
					default:
						assert(0);
						break;
					}
					this->_add_av_in_map_(lc_pts_time,&lc_sp_frame,NULL);
				}
			}
			else if(mLcDataPacket.stream_index==this->i_audio_stream_idx)
			{
				int lc_res=::avcodec_decode_audio4(lc_codec,this->frame, &i_lc_got_frame, &mLcDataPacket);
				if(lc_res<0)
				{
					//assert(0);
					break;
				}
				lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
				if(i_lc_got_frame&&this->frame->pkt_pts>=0)
				{
					//size_t lc_unpadded_linesize=this->frame->nb_samples * (int)::av_get_bytes_per_sample((AVSampleFormat)this->frame->format);
					//TxCppPlatform::shared_ptr<TxBlobString> lc_aud_frame(new TxBlobString());
					TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> spLcPcmFrame;
					this->m_SwrContext.build(&spLcPcmFrame,this->frame,lc_codec);
					if(spLcPcmFrame)
						this->_add_av_in_map_(this->_buildGetAvTime(this->frame,lc_codec),NULL,&spLcPcmFrame);
				}
			}
			mLcDataPacket.data += lc_decoded;
			mLcDataPacket.size -= lc_decoded;
		}
		if(mLcOrgPacket.size>0)
			::av_free_packet(&mLcOrgPacket);
	}

	for(BOOL bLcLoopSign=TRUE;bLcLoopSign&&this->mAvInfo.mapAudioVideoFrame.size()>0;)
	{
		std::map<long long,tagUnitInfo>::iterator iter=this->mAvInfo.mapAudioVideoFrame.begin();
		if(iter->second.spAudioFrame)
		{
			if(_spAudioFrame!=NULL)
			{
				*_spAudioFrame=iter->second.spAudioFrame;
				this->mAvInfo.iMapAudioSumCount--;
				this->mAvInfo.iTotalBufferSize-=iter->second.spAudioFrame->data.size();
				bLcLoopSign=FALSE;
			}
		}
		else
		{
			if(_spDdFrame!=NULL)
			{
				*_spDdFrame=iter->second.spVideoDdFrame;
				this->mAvInfo.iMapVideoSumCount--;
				this->mAvInfo.iTotalBufferSize-=iter->second.spVideoDdFrame->getBufferSize();
				bLcLoopSign=FALSE;
			}
		}
		*_ll_time=((iter->first)>>1);
		this->mAvInfo.mapAudioVideoFrame.erase(iter);
	}
	{
		*_fPlayRatio=0;
		if((_spDdFrame!=NULL&&(*_spDdFrame))||(_spAudioFrame!=NULL&&(*_spAudioFrame)))
		{
			if(this->llAvTotalTime>0&&(*_ll_time)>0)
			{
				*_fPlayRatio=((float)(*_ll_time))/((float)this->llAvTotalTime);
				if(*_fPlayRatio<0) *_fPlayRatio=0;
				else if(*_fPlayRatio>1) *_fPlayRatio=1;
			}
			else *_fPlayRatio=0;
		}
	}

	if(this->mStorePacketOp.getPacketBufferSize()<e_avio_ctx_buffer_size*2)
		this->mReadPackEvent.setEvent();

	if((_spDdFrame!=NULL&&(*_spDdFrame))||(_spAudioFrame!=NULL&&(*_spAudioFrame)))
		return eResultSuccess;
	else if(!this->bLastReadFileSuccess)
		return eResultOffline;
	else if(!this->bThreadRunStatus)
		return eResultErrorFormat;
	return eResultEof;
}

void CDuiFfmpegPlayWndBasic::setPlayProgress(float _fPlayProgress)
{
	if(_fPlayProgress<0) _fPlayProgress=0;
	else if(_fPlayProgress>1) _fPlayProgress=1;
	this->mAvSeekRatioOp.mSeekFramePosMutex.lock();
	this->mAvSeekRatioOp.fUserSeekFramePos=_fPlayProgress;
	this->mAvSeekRatioOp.mSeekFramePosMutex.unlock();
	this->mAvInfo.mapAudioVideoFrame.clear();
	this->mAvInfo.iMapVideoSumCount=0;
	this->mAvInfo.iMapAudioSumCount=0;
	this->mAvInfo.iTotalBufferSize=0;
	this->mStorePacketOp.reset();
}

AVFormatContext* CDuiFfmpegPlayWndBasic::getAVFormatContext()
{
	return this->fmt_ctx;
}

