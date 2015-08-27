#include "StdAfx.h"

#include "CDuiFfmpegPlayWndBasic.h"


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
	::MessageBox(NULL,_err,_T("ÑÏÖØ´íÎó"),MB_OK);
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
	this->llVideoPtsStartTime=(long long)((((unsigned long long)-1)>>2)-1);
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
	if(this->i_audio_stream_idx>=0)
	{
		::avcodec_close(this->fmt_ctx->streams[this->i_audio_stream_idx]->codec);
		this->i_audio_stream_idx=-1;
	}
	if(this->i_video_stream_idx>=0)
	{
		::avcodec_close(this->fmt_ctx->streams[this->i_video_stream_idx]->codec);
		this->i_video_stream_idx=-1;
	}
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

CDuiFfmpegPlayWndBasic::EnumResultStatus CDuiFfmpegPlayWndBasic::readFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,long long *_ll_time,float *_fPlayRatio)
{
	while(this->mapVideoFrame.size()<=e_pictrue_frame_count)
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
					long long lc_pts_time=(long long)(this->frame->pkt_pts*1000*::av_q2d(lc_codec->pkt_timebase));
					if(lc_pts_time<this->llVideoPtsStartTime)
					{
						assert(this->llVideoPtsStartTime>=(long long)((unsigned long long)0x7fffffffff));
						this->llVideoPtsStartTime=lc_pts_time;
					}
					lc_pts_time-=this->llVideoPtsStartTime;
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
					this->mapVideoFrame[lc_pts_time]=tagUnitInfo(lc_sp_frame);
				}
			}
			else if(mLcDataPacket.stream_index==this->i_audio_stream_idx)
			{
				int lc_res=::avcodec_decode_audio4(lc_codec, frame, &i_lc_got_frame, &mLcDataPacket);
				if(lc_res<0)
				{
					//assert(0);
					break;
				}
				lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
				if(i_lc_got_frame&&this->frame->pkt_pts>=0)
				{
					size_t lc_unpadded_linesize=this->frame->nb_samples * (int)::av_get_bytes_per_sample((AVSampleFormat)this->frame->format);
				}
			}
			mLcDataPacket.data += lc_decoded;
			mLcDataPacket.size -= lc_decoded;
		}
		if(mLcOrgPacket.size>0)
			::av_free_packet(&mLcOrgPacket);
	}

	_spDdFrame->reset();
	if(this->mapVideoFrame.size()>0)
	{
		std::map<long long,tagUnitInfo>::iterator iter=this->mapVideoFrame.begin();
		*_spDdFrame=iter->second.spDdFrame;
		*_ll_time=iter->first;
		this->mapVideoFrame.erase(iter);
	}
	{
		*_fPlayRatio=0;
		if(*_spDdFrame)
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

	if(*_spDdFrame)
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
	this->mapVideoFrame.clear();
	this->mStorePacketOp.reset();
}

AVFormatContext* CDuiFfmpegPlayWndBasic::getAVFormatContext(int *_video_idx,int *_audio_idx)
{
	if(_audio_idx!=NULL)
	*_audio_idx=this->i_audio_stream_idx;
	if(_video_idx!=0)
	*_video_idx=this->i_video_stream_idx;
	return this->fmt_ctx;
}

