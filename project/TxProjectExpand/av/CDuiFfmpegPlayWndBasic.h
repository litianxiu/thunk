#pragma once

#include "CDuiPlayVideoWndBasic.h"
#include "CDirectSoundOutput.hpp"
#include <map>
#include <list>

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/avutil.h>
#include <libavfilter/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/mathematics.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/file.h>
#include <libavutil/error.h>
#include <libavutil/mem.h>
}

#pragma comment(lib,"../lib/avcodec.lib")
#pragma comment(lib,"../lib/avdevice.lib")
#pragma comment(lib,"../lib/avfilter.lib")
#pragma comment(lib,"../lib/avformat.lib")
#pragma comment(lib,"../lib/avutil.lib")
#pragma comment(lib,"../lib/postproc.lib")
#pragma comment(lib,"../lib/swresample.lib")
#pragma comment(lib,"../lib/swscale.lib")

class CDuiFfmpegPlayWndBasic
{
public :
	class IThreadReadFile
	{
	public :
		virtual ~IThreadReadFile() { }
		virtual int vfReadStream(void *_buff,int _size,long long _llFilePos)=0;
	};
	class IThreadCallBack
	{
	public:
		virtual void vfPlayAvInitialize(bool _bSuccess)=NULL;
	};
	enum EnumResultStatus
	{
		eResultSuccess=0,//成功
		eResultEof,//视频尽头
		eResultOffline,//掉线
		eResultErrorFormat,//视频格式错误
	};
	enum {
		e_avio_ctx_buffer_size=128*1024,
		e_av_frame_buffer_max_size=10*1024*1024,
		e_video_frame_count=32,
		e_audio_frame_count=8,
	};
	struct tagUnitInfo
	{
		TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> spVideoDdFrame;
		TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> spAudioFrame;
	};
	class tagStorePacketOp
	{
		std::list<AVPacket> mListPacket;
		int iPacketTotalSize;
		TxSystemDependent::TxMutexWrap mPacketMutex;
	public :
		~tagStorePacketOp();
		void reset();
		void pushPacket(AVPacket &_pkg);
		bool popPacket(AVPacket *_pkg);
		inline int getPacketBufferSize();
	};
	class tagSwrContext
	{
		int64_t  m_ch_layout;
		enum AVSampleFormat  m_sample_fmt;
		int  m_sample_rate;
		int m_channels;
		struct SwrContext *swr_ctx;
		AVFrame *pAudioMidFrame;
		//uint8_t **pp_dst_data;
		//int m_dst_rate;
		//long m_dst_ch_layout;
		//AVSampleFormat m_dst_sample_fmt;
	public:
		tagSwrContext();
		~tagSwrContext();
		void build(TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spPcmFrame,AVFrame *_audio_frame,AVCodecContext *_codec);
	};
private:
	typedef unsigned char uint8_t;
	AVFormatContext *fmt_ctx;
	AVIOContext *avio_ctx;
	uint8_t *avio_ctx_buffer;
	int i_video_stream_idx;
	int i_audio_stream_idx;
	uint8_t *video_dst_data[4];
	int      video_dst_linesize[4];
	int i_video_dst_bufsize;
	AVFrame *frame;
private:
	long long llReadPos;
	size_t bThreadRunStatus;
	TxCppPlatform::shared_ptr<IThreadReadFile> spIReadFile;
	IThreadCallBack *const pIThreadCallBack;
	long long llAvPtsStartTime;
	struct {
		std::map<long long,tagUnitInfo> mapAudioVideoFrame;
		int iMapVideoSumCount;
		int iMapAudioSumCount;
		int iTotalBufferSize;
	} mAvInfo;
	TxSystemDependent::TxEventWrap mReadPackEvent;
	TxSystemDependent::TxThreadWrap mReadFileThread;
	BOOL bLastReadFileSuccess;
	//long long llPlayCurrentFrameTime;
	long long llAvTotalTime;

	tagSwrContext m_SwrContext;

	tagStorePacketOp mStorePacketOp;

	struct {
		float fUserSeekFramePos;
		TxSystemDependent::TxMutexWrap mSeekFramePosMutex;
	} mAvSeekRatioOp;
public:
	static void initialize();
	static void _static_thread_call_back_(void *_arg1,void *_arg2);
	void _thread_call_back_();
	bool _read_frame_(std::list<std::pair<long long,tagUnitInfo>> *_listSpFrame);
	void _show_error(LPCTSTR _err);
	static int _static_io_read_packet_func(void *_opaque,uint8_t *_buf, int _buf_size);
	static int64_t _static_io_seek_packet_func(void *_opaque, int64_t _offset, int _whence);
	bool _open_codec_context__(int *stream_idx,enum AVMediaType type);
	void _add_av_in_map_(long long _ll_time,TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spAudioFrame);
	//void _trans_AVFrameToAudio_(TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spPcmFrame,AVFrame *_frame);
	long long _buildGetAvTime(AVFrame *_av_frame,AVCodecContext *_av_codec);
public :
	CDuiFfmpegPlayWndBasic(IThreadCallBack *_pIThreadCallBack,TxCppPlatform::shared_ptr<CDuiFfmpegPlayWndBasic::IThreadReadFile> _spIReadFile);
	~CDuiFfmpegPlayWndBasic();
	//bool _read_frame_(std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>> *_listSpFrame,int _iListSize);
	//virtual void vfThreadReadFrame(std::list<std::pair<long long,CDuiPlayVideoWndBasic::tagUnitInfo>> *_listSpFrame,int _iListSize);
	EnumResultStatus readFrame(float *_fPlayRatio,long long *_ll_time,TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,TxCppPlatform::shared_ptr<CDirectSoundOutput::tagAudioFrame> *_spAudioFrame);
	//EnumResultStatus readAudioFrame(TxCppPlatform::shared_ptr<TxBlobString> *_spDdFrame,long long *_ll_time);

	void setPlayProgress(float _fPlayProgress);
	AVFormatContext* getAVFormatContext();
	//CDuiFfmpegPlayWndBasic::EnumResultStatus  _decode_frame_(TxCppPlatform::shared_ptr<CDirectDrawFrameFormat> *_spDdFrame,long long *_ll_time);
};

