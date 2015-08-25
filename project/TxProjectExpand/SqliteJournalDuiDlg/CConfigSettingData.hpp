#pragma once

#include <vector>
#include <string>
#include <string.h>
#include "../common/TxBlobString.hpp"
#include "../include/CSqliteWrapDll.h"
#include "../common/TxSystemDependent.hpp"


//服务器表字段名
#define Def_SrvTb_DbFileName "filename"
#define Def_SrvTb_DbPort "port"
#define Def_SrvTb_DbJournal "journal"

struct tagConfigSettingSqliteServerPort
{
	TxBlobString filename;
	int port;
	bool bJournal;
	tagConfigSettingSqliteServerPort()
	{
		this->filename="";
		this->port=-1;
		this->bJournal=false;
	}
	bool isValid() const
	{
		return this->port>0
			&&this->filename.length()>0;
	}
};

//公共部分
#define Def_M_iNetWork_Mtu "M_iNetWork_Mtu"
#define Def_M_iSvcThreadCount "M_iSvcThreadCount"
#define Def_M_bKillOtherSameNameProcess "M_bKillOtherSameNameProcess"
//服务器
#define Def_S_bEnableRemotFileServer "S_bServerInfo_bEnableRemotFileServer"
#define Def_S_iServerResponseLogMaxCount "S_iServerResponseLogMaxCount"
#define Def_S_iServerResponseLogMaxMemory "S_iServerResponseLogMaxMemory"
//客户端
#define Def_C_iViewLogMaxCount "C_iViewLogMaxCount"
#define Def_C_iViewLogWaitMaxTime "C_iViewLogWaitMaxTime"
#define Def_C_iExecSqlWaitMaxTime "C_iExecSqlWaitMaxTime"


struct tagConfigSettingData
{
	int iNetWork_Mtu;//网络最大传输单元
	int iSvcThreadCount;//服务线程数
	bool bKillOtherSameNameProcess;//杀死同名进程
	struct {
		std::vector<tagConfigSettingSqliteServerPort> listSqliteServer;
		bool bEnableRemotFileServer;//激活远程文件服务器
		int iServerResponseLogMaxCount;//服务器响应最大数目
		int iServerResponseLogMaxMemory;//服务器响应最大内存
	} mServerInfo;
	struct {
		int iViewLogMaxCount;//查询返回日志的条数
		int iViewLogWaitMaxTime;//查询日志等待的最大时间
		int iExecSqlWaitMaxTime;//执行SQL语言时等待的最大时间
	} mClientInfo;
	TxBlobString sDbFileName;
private:
	template<class T>
	inline static void gBoundTailor(T& _val,T _min1, T _min2)
	{
		assert(_min1<_min2);
		if(_val<_min1) _val=_min1;
		else if(_val>_min2) _val=_min2;
	}
	static int __stdcall g_SqliteWrapExecCb_svr(void *para,int n_column,char **column_value,char **column_name)
	{
		tagConfigSettingData* pThis=(tagConfigSettingData*)para;
		tagConfigSettingSqliteServerPort tem;
		for(int i=n_column;i--;)
		{
			TxBlobString tsName(column_name[i]);
			TxBlobString tsValue(column_value[i]);
			if(tsName.compare(Def_SrvTb_DbFileName)==0)
				tsValue.toDatum(tem.filename);
			else if(tsName.compare(Def_SrvTb_DbPort)==0)
				tsValue.toDatum(tem.port);
			else if(tsName.compare(Def_SrvTb_DbJournal)==0)
				tsValue.toDatum(tem.bJournal);
		}
		if(tem.isValid())
			pThis->mServerInfo.listSqliteServer.push_back(tem);
		return 0;
	}
	static int __stdcall g_SqliteWrapExecCb_mix(void *para,int n_column,char **column_value,char **column_name)
	{
		tagConfigSettingData* pThis=(tagConfigSettingData*)para;
		if(n_column==2&&strcmp(column_name[0],"name")==0&&strcmp(column_name[1],"value")==0)
		{
			TxBlobString tsName(column_value[0]);
			TxBlobString tsValue(column_value[1]);
			if(tsName.compare(Def_M_iNetWork_Mtu)==0)
			{
				tsValue.toDatum(pThis->iNetWork_Mtu);
				gBoundTailor(pThis->iNetWork_Mtu,1,64*1024);
			}
			else if(tsName.compare(Def_M_iSvcThreadCount)==0)
			{
				tsValue.toDatum(pThis->iSvcThreadCount);
				gBoundTailor(pThis->iSvcThreadCount,1,128);
			}
			else if(tsName.compare(Def_M_bKillOtherSameNameProcess)==0)
			{
				tsValue.toDatum(pThis->bKillOtherSameNameProcess);
			}
			//服务器
			else if(tsName.compare(Def_S_bEnableRemotFileServer)==0)
			{
				tsValue.toDatum(pThis->mServerInfo.bEnableRemotFileServer);
			}
			else if(tsName.compare(Def_S_iServerResponseLogMaxCount)==0)
			{
				tsValue.toDatum(pThis->mServerInfo.iServerResponseLogMaxCount);
				gBoundTailor(pThis->mServerInfo.iServerResponseLogMaxCount,1,100000);
			}
			else if(tsName.compare(Def_S_iServerResponseLogMaxMemory)==0)
			{
				tsValue.toDatum(pThis->mServerInfo.iServerResponseLogMaxMemory);
				gBoundTailor(pThis->mServerInfo.iServerResponseLogMaxMemory,1024,50*1024*1024);
			}
			//客户端
			else if(tsName.compare(Def_C_iViewLogMaxCount)==0)
			{
				tsValue.toDatum(pThis->mClientInfo.iViewLogMaxCount);
				gBoundTailor(pThis->mClientInfo.iViewLogMaxCount,1,100000);
			}
			else if(tsName.compare(Def_C_iViewLogWaitMaxTime)==0)
			{
				tsValue.toDatum(pThis->mClientInfo.iViewLogWaitMaxTime);
				gBoundTailor(pThis->mClientInfo.iViewLogWaitMaxTime,1,10*60);
			}
			else if(tsName.compare(Def_C_iExecSqlWaitMaxTime)==0)
			{
				tsValue.toDatum(pThis->mClientInfo.iExecSqlWaitMaxTime);
				gBoundTailor(pThis->mClientInfo.iExecSqlWaitMaxTime,1,10*60);
			}
		}
		return 0;
	}
public:
	tagConfigSettingData()
	{
		this->sDbFileName=TxSystemDependent::TxRuntimeWrap::getWorkingDirectory();
		this->sDbFileName.append("config.db");
		this->reset();
	}
	~tagConfigSettingData()
	{
	}
	void reset()
	{
		this->iNetWork_Mtu=1440;
		this->iSvcThreadCount=1;
		this->bKillOtherSameNameProcess=false;
		this->mServerInfo.bEnableRemotFileServer=false;
		this->mServerInfo.iServerResponseLogMaxCount=10000;
		this->mServerInfo.iServerResponseLogMaxMemory=10*1024*1024;
		this->mClientInfo.iViewLogMaxCount=1000;
		this->mClientInfo.iViewLogWaitMaxTime=60;
		this->mClientInfo.iExecSqlWaitMaxTime=60;
		this->mServerInfo.listSqliteServer.clear();
	}
	const char* loadConfig()
	{
		CSqliteWrapDll lc_db;
		char *ret=NULL;
		if(lc_db.open(this->sDbFileName.c_str()))
		{
			bool b1=lc_db.execute("select * from config_mix limit 10000; ",g_SqliteWrapExecCb_mix,this,&ret);
			bool b2=lc_db.execute("select * from config_svr limit 10000; ",g_SqliteWrapExecCb_svr,this,ret==NULL?&ret:NULL);
			if(b1&&b2)
			{
				return NULL;
			}
			else
			{
				if(ret[0]!=0)
					ret="执行数据库失败";
			}
		}
		else
		{
			ret="无法打开数据库";
		}
		return ret;
	}
	bool doSaveAffairConfigDb(CSqliteWrapDll *_p_db,char **_error)
	{
		TxBlobString sql;
		sql+="begin;\n";

		sql+="DELETE FROM config_mix;\n";
		sql+="DELETE FROM config_svr;\n";
		//公共部分
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_M_iNetWork_Mtu"\',\'%d\');\n",(int)this->iNetWork_Mtu);
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_M_iSvcThreadCount"\',\'%d\');\n",(int)this->iSvcThreadCount);
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_M_bKillOtherSameNameProcess"\',\'%d\');\n",(int)this->bKillOtherSameNameProcess);
		//服务器
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_S_bEnableRemotFileServer"\',\'%d\');\n",(int)this->mServerInfo.bEnableRemotFileServer);
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_S_iServerResponseLogMaxCount"\',\'%d\');\n",(int)this->mServerInfo.iServerResponseLogMaxCount);
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_S_iServerResponseLogMaxMemory"\',\'%d\');\n",(int)this->mServerInfo.iServerResponseLogMaxMemory);
		//客户端
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_C_iViewLogMaxCount"\',\'%d\');\n",(int)this->mClientInfo.iViewLogMaxCount);
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_C_iViewLogWaitMaxTime"\',\'%d\');\n",(int)this->mClientInfo.iViewLogWaitMaxTime);
		sql.appendFormat("INSERT INTO config_mix (name,value) VALUES (\'"Def_C_iExecSqlWaitMaxTime"\',\'%d\');\n",(int)this->mClientInfo.iExecSqlWaitMaxTime);

		//list
		for(int i=(int)this->mServerInfo.listSqliteServer.size();i--;)
		{
			sql.appendFormat("INSERT INTO config_svr ("Def_SrvTb_DbPort","Def_SrvTb_DbFileName","Def_SrvTb_DbJournal") VALUES ("
				"\'%d\',\'%s\',\'%d\');\n",
				(int)this->mServerInfo.listSqliteServer[i].port,
				(const char*)this->mServerInfo.listSqliteServer[i].filename.c_str(),
				(int)!!this->mServerInfo.listSqliteServer[i].bJournal);
		}

		sql+="commit;\n";

		return _p_db->execute(sql.c_str(),NULL,NULL,_error);
	}
	const char* saveConfig()
	{
		CSqliteWrapDll lc_db;
		char *ret=NULL;
		if(lc_db.open(this->sDbFileName.c_str()))
		{
			bool b1=lc_db.execute("SELECT * FROM config_mix LIMIT 1;",NULL,NULL,NULL);
			if(!b1)
			{
				b1=lc_db.execute("CREATE TABLE config_mix("
					"name			TEXT,"
					"value			TEXT"
					");",
					NULL,NULL,ret==NULL?&ret:NULL);
			}
			bool b2=lc_db.execute("SELECT * FROM config_svr LIMIT 1;",NULL,NULL,NULL);
			if(!b2)
			{
				b2=lc_db.execute("CREATE TABLE config_svr("
					Def_SrvTb_DbPort"			TEXT,"
					Def_SrvTb_DbFileName"		TEXT,"
					Def_SrvTb_DbJournal"		TEXT"
					");",
					NULL,NULL,ret==NULL?&ret:NULL);
			}
			if(b1&&b2)
			{
				if(this->doSaveAffairConfigDb(&lc_db,ret==NULL?&ret:NULL))
				{
				}
				else
				{
					if(ret==0)
						ret="执行数据库失败";
				}
			}
			else
			{
				if(ret==0)
					ret="初始化数据库失败";
			}
		}
		else
		{
			if(ret==0)
				ret="无法打开数据库";
		}
		return ret;
	}
};

