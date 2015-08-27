#pragma once

#include <queue>
#include <vector>
#include "CConfigSettingData.hpp"
#include "../common/TxDuiMessageNotifyDlg.hpp"
#include "../include/CSqliteExpandDll.h"
#include "../common/TxAutoWaitDuiDialog.hpp"
#include "../common/TxDuiListMenuExt.hpp"
#include "../common/TxDuiMessageBox.hpp"
#include "../common/TxSystemDependent.hpp"
#include "../include/CSqliteExpandDll.h"
#include <UIlib.h>

class CMainFrameDlgBasic: public DuiLib::WindowImplBase
{
private:
	struct tagOnTimerFunctionInfo
	{
		long long llLstTime;
		long long llTagTime;
		struct tagFuncBase
		{
			virtual ~tagFuncBase() { }
			virtual bool run()=0;
		} *pFunc;
		inline bool operator<(const tagOnTimerFunctionInfo &_tem) const { return this->llLstTime<_tem.llLstTime; }
	};
private:
	tagConfigSettingData mConfigSettingData;
	TxDuiMessageNotifyDlg m_DuiMessageNotifyDlg;
	TxDuiListMenuExt m_DuiListMenuExt;
	TxCppPlatform::weak_ptr<TxAutoWaitDuiDialog> wpAutoDuiDlg;
private://postmessage
	TxSystemDependent::TxMutexWrap mVectorTaskMutex;
	std::vector<TxCppPlatform::shared_ptr<TxCppPlatform::function<void()>>> mVectorTask;
	enum { e_post_task_deal_id=WM_USER+WM_APP+186 };
private://ontimer
	TxSystemDependent::TxTimeSpan<true> mOnTimerSpan;
	TxSystemDependent::TxMutexWrap pq_timer_mutex;
	std::priority_queue<tagOnTimerFunctionInfo> pq_timer;
	UINT uTimerId;
public :
	virtual void vfSelectTabWidget(int _iIndex)=0;
public :
	CMainFrameDlgBasic():m_DuiMessageNotifyDlg(this,3,true)
	{
		this->uTimerId=0;
		this->mVectorTask.reserve(64);
		this->mOnTimerSpan.reset(0);
	}
	virtual ~CMainFrameDlgBasic()
	{
		this->dealTimerTask_(true);
	}
	virtual void InitWindow()
	{
		this->m_DuiMessageNotifyDlg.initialize();
		this->m_DuiListMenuExt.initialize(this->GetHWND());
		this->uTimerId=::SetTimer(this->GetHWND(),1658,150,NULL);
		return __super::InitWindow();
	}
	virtual LRESULT HandleMessage(UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		switch(_uMsg)
		{
		case WM_TIMER:
			this->dealTimerTask_(false);
			break;
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
			if(this->uTimerId!=0)
			{
				::KillTimer(this->GetHWND(),this->uTimerId);
				this->uTimerId=0;
			}
			this->dealTimerTask_(true);
			break;
		case e_post_task_deal_id:
			{
				std::vector<TxCppPlatform::shared_ptr<TxCppPlatform::function<void()>>> mLcVectorTaskMid;
				this->mVectorTaskMutex.lock();
				if(this->mVectorTask.size()>0)
				{
					mLcVectorTaskMid=this->mVectorTask;
					this->mVectorTask.clear();
				}
				this->mVectorTaskMutex.unlock();
				for(int i=(int)mLcVectorTaskMid.size();i--;)
					(*mLcVectorTaskMid[i])();
			}
			break;
		default:
			break;
		}
		return __super::HandleMessage(_uMsg,_wParam,_lParam);
	}
public:
	template<class T>
	void postTaskMessage(T &_func)
	{
		this->mVectorTaskMutex.lock();
		this->mVectorTask.push_back(TxCppPlatform::shared_ptr<TxCppPlatform::function<void()>>(new TxCppPlatform::function<void()>(_func)));
		this->mVectorTaskMutex.unlock();
		this->PostMessage(e_post_task_deal_id);
	}
	template<class T>
	static void buildSaftWidgetPtr(DuiLib::CPaintManagerUI &_mPaintManager,T **_obj,LPCTSTR _str)
	{
		DuiLib::CControlUI* p_ui=_mPaintManager.FindControl(_str);
		*_obj=NULL;
		if(p_ui!=NULL)
			*_obj=dynamic_cast<T*>(p_ui);
		if((*_obj)==NULL)
			assert(0);
	}
	TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> getAutoWaitDuiDialog(LPCTSTR _text)
	{
		this->clearAutoWaitDuiDialog();
		TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> ret(new TxAutoWaitDuiDialog(_text));
		this->wpAutoDuiDlg=ret;
		return ret;
	}
	void clearAutoWaitDuiDialog()
	{
		TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> sp(this->wpAutoDuiDlg.lock());
		if(sp)
		{
			sp->quitDuiDialog();
		}
		this->wpAutoDuiDlg.reset();
	}
	void updateAutoWaitDlgText(LPCTSTR _text)
	{
		TxCppPlatform::shared_ptr<TxAutoWaitDuiDialog> sp(this->wpAutoDuiDlg.lock());
		if(sp&&_text)
			sp->updateText(_text);
	}
	tagConfigSettingData* getConfigSettingDataPtr()
	{
		return &this->mConfigSettingData;
	}
	TxDuiMessageNotifyDlg* getDuiMessageNotifyDlgPtr()
	{
		return &this->m_DuiMessageNotifyDlg;
	}
	TxDuiListMenuExt *getDuiListMenuExt() const
	{
		return &(const_cast<CMainFrameDlgBasic*>(this))->m_DuiListMenuExt;
	}
	DuiLib::CPaintManagerUI *getPaintManagerUI() const
	{
		return &(const_cast<CMainFrameDlgBasic*>(this))->m_PaintManager;
	}
	static bool gTransTcpIpPort(TxBlobString *_retIp,int *_retPort,LPCTSTR _s_ip,LPCTSTR _s_port)
	{
		TxBlobString str_ip(_s_ip);
		_retIp->assign(str_ip.trimLR("	\n\r\t"));
		TxBlobString(_s_port).trimLR("	\n\r\t").toDatum(*_retPort);
		return (_retIp->length()>0&&(*_retPort)>0&&(*_retPort)<=65536);
	}
	template<class TFUNC>
	void installTimerTask(long long _llTagTime,const TFUNC _mFunc)
	{
		struct T_R: public tagOnTimerFunctionInfo::tagFuncBase
		{
			TFUNC mFunc;
			T_R(const TFUNC &_mFunc):mFunc(_mFunc) { }
			virtual bool run() { return this->mFunc(); }
		};
		tagOnTimerFunctionInfo tem;
		tem.llTagTime=_llTagTime;
		tem.llLstTime=this->mOnTimerSpan.getCurrentMillisecond()+_llTagTime;
		tem.pFunc=new T_R(_mFunc);
		_mFunc();
		this->pq_timer_mutex.lock();
		this->pq_timer.push(tem);
		this->pq_timer_mutex.unlock();
	}
	void dealTimerTask_(bool _destroy=false)
	{
		long long tt=this->mOnTimerSpan.getCurrentMillisecond();
		this->pq_timer_mutex.lock();
		while(this->pq_timer.size()>0&&(this->pq_timer.top().llLstTime<=tt||_destroy))
		{
			tagOnTimerFunctionInfo mLcElem=this->pq_timer.top();
			if(!mLcElem.pFunc->run()||_destroy)
			{
				delete mLcElem.pFunc;
				this->pq_timer.pop();
			}
			else
			{
				this->pq_timer.pop();
				mLcElem.llLstTime+=mLcElem.llTagTime;
				this->pq_timer.push(mLcElem);
			}
		}
		this->pq_timer_mutex.unlock();
	}
	static void getDropFileListName(std::list<std::string> *_list_string,HDROP hDrop)
	{
		const int lc_count=(int)::DragQueryFile(hDrop,-1,NULL,0);
		char __lc_path_tem[FILENAME_MAX+MAX_PATH+4];
		char *pLcPath=__lc_path_tem;
		int iLcPathMax=sizeof(__lc_path_tem)-1;
		for(int nn=0;nn<lc_count+1;nn++)
		{
			int iLcPathnameSize =::DragQueryFile(hDrop,nn,NULL,0);
			if(iLcPathnameSize>0)
			{
				iLcPathnameSize++;
				if(iLcPathnameSize>iLcPathMax)
				{
					if(pLcPath!=__lc_path_tem)
						delete pLcPath;
					pLcPath=new char[iLcPathnameSize];
					iLcPathMax=iLcPathnameSize;
				}
				if((int)::DragQueryFile(hDrop,nn,pLcPath,iLcPathMax)>0)
				{
					_list_string->push_back(pLcPath);
				}
			}
		}
		if (__lc_path_tem!=pLcPath)
			delete pLcPath;
	}

};
