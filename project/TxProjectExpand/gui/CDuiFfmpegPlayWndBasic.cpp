#include "StdAfx.h"

#include "CDuiPlayVideoAttrb.h"
#include "CDuiFfmpegPlayWndBasic.h"


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

	this->fmt_ctx=avformat_alloc_context();
	if(this->fmt_ctx==NULL)
	{
		this->_show_error(_T("avformat_alloc_context() fail"));
	}
	this->avio_ctx_buffer = (uint8_t*)::av_malloc(e_avio_ctx_buffer_size);
	if(this->avio_ctx_buffer==NULL)
	{
		this->_show_error(_T("av_malloc() fail"));
	}
	this->avio_ctx=avio_alloc_context(this->avio_ctx_buffer,e_avio_ctx_buffer_size,0,this,&_static_io_read_packet_func,NULL,&_static_io_seek_packet_func);
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
		this->iPacketTotalSize=0;
		this->mReadFileThread.create(_static_thread_call_back_,this,NULL);
	}
}

CDuiFfmpegPlayWndBasic::~CDuiFfmpegPlayWndBasic()
{
	this->bThreadRunStatus=FALSE;
	this->mReadPackEvent.setEvent();
	this->mReadFileThread.join();

	if(this->frame!=NULL)
	{
		::av_frame_free(&this->frame);
		this->frame=NULL;
	}
	if(this->avio_ctx_buffer!=NULL)
	{
		::av_free(this->avio_ctx_buffer);
		this->avio_ctx_buffer=NULL;
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
			iLcOptionStep++;
			break;
		default:
			{
				while(this->iPacketTotalSize<=2*e_avio_ctx_buffer_size)
				{
					AVPacket mLcOrgPacket={0};
					::av_init_packet(&mLcOrgPacket);
					if(::av_read_frame(this->fmt_ctx,&mLcOrgPacket)<0)
					{
						break;
					}
					this->mPacketMutex.lock();
					this->mListPacket.push_back(mLcOrgPacket);
					this->iPacketTotalSize+=mLcOrgPacket.size;
					this->mPacketMutex.unlock();
				}
				bLcInitAvSign=TRUE;
				this->pIThreadCallBack->vfPlayAvInitialize(true);
			}
			//{
			//	std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>> mLcSpFrame;
			//	this->_read_frame_(&mLcSpFrame);
			//	std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>>::iterator iter=mLcSpFrame.begin();
			//	std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>>::iterator iter_end=mLcSpFrame.end();
			//	if(iter!=iter_end)
			//	{
			//		this->mVideoFrameMutex.lock();
			//		for(;iter!=iter_end;iter++)
			//			this->mapVideoFrame[iter->first]=iter->second;
			//		this->mVideoFrameMutex.unlock();
			//	}
			//}
			break;
		}
	}
	this->bThreadRunStatus=FALSE;
	if(!bLcInitAvSign)
	{
		this->pIThreadCallBack->vfPlayAvInitialize(false);
	}
}

CDuiFfmpegPlayWndBasic::EnumResultStatus CDuiFfmpegPlayWndBasic::readFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,long long *_ll_time)
{
	while(this->mapVideoFrame.size()<=e_pictrue_frame_count)
	{
		AVPacket mLcOrgPacket={0};
		this->mPacketMutex.lock();
		if(this->mListPacket.size()>0)
		{
			mLcOrgPacket=this->mListPacket.front();
			this->mListPacket.pop_front();
			this->iPacketTotalSize-=(int)mLcOrgPacket.size;
		}
		this->mPacketMutex.unlock();
		if(mLcOrgPacket.size<=0) break;
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
					//return false;
				}
				lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
				if(i_lc_got_frame!=0)
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
					assert(0);
					break;
				}
				lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
				if(i_lc_got_frame)
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
	this->mPacketMutex.lock();
	BOOL bLcSetReadPackEvent=FALSE;
	if(this->mListPacket.size()<e_avio_ctx_buffer_size*2)
		bLcSetReadPackEvent=TRUE;
	this->mPacketMutex.unlock();
	if(bLcSetReadPackEvent)
		this->mReadPackEvent.setEvent();

	if(*_spDdFrame)
		return eResultSuccess;
	else if(!this->bLastReadFileSuccess)
		return eResultOffline;
	else if(!this->bThreadRunStatus)
		return eResultErrorFormat;
	return eResultEof;
}

//bool CDuiFfmpegPlayWndBasic::_read_frame_(std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>> *_listSpFrame)
//{
//	AVPacket mLcOrgPacket={0};
//	::av_init_packet(&mLcOrgPacket);
//	if(::av_read_frame(this->fmt_ctx,&mLcOrgPacket)<0)
//	{
//		break;
//	}
//	AVPacket mLcDataPacket=mLcOrgPacket;
//	while((int)mLcDataPacket.size>0)
//	{
//		int i_lc_got_frame = 0;
//		int lc_decoded = mLcDataPacket.size;
//		AVCodecContext *lc_codec=this->fmt_ctx->streams[mLcDataPacket.stream_index]->codec;
//		if(mLcDataPacket.stream_index==this->i_video_stream_idx)
//		{
//			int lc_res=::avcodec_decode_video2(lc_codec,this->frame,&i_lc_got_frame,&mLcDataPacket);
//			if(lc_res<0)
//			{
//				return false;
//			}
//			lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
//			if(i_lc_got_frame!=0)
//			{
//				::av_image_copy(this->video_dst_data,this->video_dst_linesize,
//					(const uint8_t **)(this->frame->data),this->frame->linesize,
//					lc_codec->pix_fmt,lc_codec->width,lc_codec->height);
//				long long lc_pts_time=(long long)(this->frame->pkt_pts*1000*::av_q2d(lc_codec->pkt_timebase));
//				if(lc_pts_time<this->llVideoPtsStartTime)
//				{
//					assert(this->llVideoPtsStartTime>=(long long)((unsigned long long)0x7fffffffff));
//					this->llVideoPtsStartTime=lc_pts_time;
//				}
//				lc_pts_time-=this->llVideoPtsStartTime;
//				TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> lc_sp_frame=this->newDataCell();
//				lc_sp_frame->load_data(video_dst_data[0],(int)this->i_video_dst_bufsize,lc_codec->width,lc_codec->height);
//				switch(lc_codec->pix_fmt)
//				{
//				case AV_PIX_FMT_YUV420P:
//					//lc_sp_frame->init_YUYV(NULL);
//					lc_sp_frame->init_YV12(NULL);
//					break;
//				default:
//					assert(0);
//					break;
//				}
//				_listSpFrame->push_back(std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>(
//					lc_pts_time,CDuiPlayVideoWndBasic::tagUnitInfo(lc_sp_frame)));
//			}
//		}
//		else if(mLcDataPacket.stream_index==this->i_audio_stream_idx)
//		{
//			int lc_res=::avcodec_decode_audio4(lc_codec, frame, &i_lc_got_frame, &mLcDataPacket);
//			if(lc_res<0)
//			{
//				return false;
//			}
//			lc_decoded = FFMIN(lc_res,mLcDataPacket.size);
//			if(i_lc_got_frame)
//			{
//				size_t lc_unpadded_linesize=this->frame->nb_samples * (int)::av_get_bytes_per_sample((AVSampleFormat)this->frame->format);
//			}
//		}
//		mLcDataPacket.data += lc_decoded;
//		mLcDataPacket.size -= lc_decoded;
//	}
//	::av_free_packet(&mLcOrgPacket);
//	return true;
//}

//CDuiFfmpegPlayWndBasic::EnumResultStatus CDuiFfmpegPlayWndBasic::readFrame(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,long long *_ll_time)
//{
//	_spDdFrame->clear();
//	BOOL bLcSetReadEvent=FALSE;
//	this->mVideoFrameMutex.lock();
//	if(this->mapVideoFrame.size()>0)
//	{
//		std::map<long long,CDuiPlayVideoWndBasic::tagUnitInfo>::iterator iter=this->mapVideoFrame.begin();
//		*_spDdFrame=(*iter);
//		this->mapVideoFrame.erase(iter);
//		if(bLcSetReadEvent<e_pictrue_frame_count)
//			bLcSetReadEvent=TRUE;
//	}
//	this->mVideoFrameMutex.unlock();
//	if(bLcSetReadEvent)
//	{
//		this->mReadWaitEvent.setEvent();
//	}
//
//	if(*_spDdFrame)
//		return eResultSuccess;
//	else if(!this->bLastReadFileSuccess)
//		return eResultOffline;
//	else if(!this->bThreadRunStatus)
//		return eResultErrorFormat;
//	return eResultEof;
//}

//bool CDuiFfmpegPlayWndBasic::vfThreadReadFrame(std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>> *_listSpFrame,int _iListSize)
//{
//	_listSpFrame->clear();
//	BOOL bLcLoop=TRUE;
//	BOOL bLcDecodeSuccess=FALSE;
//	while(bLcLoop)
//	{
//		switch(this->iOptionStep)
//		{
//		case 0:
//			if(0)
//			{
//				if(::avformat_open_input(&this->fmt_ctx,NULL,NULL,NULL)>=0)
//					this->iOptionStep++;
//				else bLcLoop=FALSE;
//			}
//			else
//			{
//				if(::avformat_open_input(&this->fmt_ctx,NULL,NULL,NULL)>=0)
//					this->iOptionStep++;
//				else bLcLoop=FALSE;
//			}
//			break;
//		case 1:
//			if(::avformat_find_stream_info(this->fmt_ctx,NULL)>=0)
//				this->iOptionStep++;
//			else bLcLoop=FALSE;
//			break;
//		case 2:
//			if(this->_open_codec_context__(&this->i_video_stream_idx,AVMEDIA_TYPE_VIDEO))
//			{
//				int lc_ret=::av_image_alloc(this->video_dst_data,this->video_dst_linesize,
//					this->fmt_ctx->streams[this->i_video_stream_idx]->codec->width,
//					this->fmt_ctx->streams[this->i_video_stream_idx]->codec->height,
//					this->fmt_ctx->streams[this->i_video_stream_idx]->codec->pix_fmt,
//					1);
//				if(lc_ret>=0)
//				{
//					this->i_video_dst_bufsize = lc_ret;
//				}
//			}
//			else
//			{
//				this->i_video_stream_idx=-1;
//			}
//			this->iOptionStep++;
//			break;
//		case 3:
//			if(this->_open_codec_context__(&this->i_audio_stream_idx,AVMEDIA_TYPE_AUDIO))
//			{
//			}
//			else
//			{
//				this->i_audio_stream_idx=-1;
//			}
//			this->iOptionStep++;
//			break;
//		case 4:
//			::av_dump_format(this->fmt_ctx,0,NULL,0);
//			this->iOptionStep++;
//			break;
//		default:
//			bLcDecodeSuccess=TRUE;
//			bLcLoop=FALSE;
//			this->_read_frame_(_listSpFrame,_iListSize);
//			break;
//		}
//	}
//	return !!bLcDecodeSuccess;
//}


