/*****************************************************************************
   模块名      : Board Agent
   文件名      : criguard.h
   相关文件    : 
   文件实现功能: 单板守卫基类定义
   作者        : jianghy
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/08/25  1.0         jianghy       创建
   2004/11/11  3.5         李 博         新接口的修改
******************************************************************************/
#ifndef CRIGUARD_H
#define CRIGUARD_H

#include "osp.h"
#include "kdvsys.h"
#include "mcuagtstruct.h"
#include "criagent.h"

class CBoardGuard : public CInstance  
{
public:
	CBoardGuard();
	~CBoardGuard();

	
protected:
	//消息入口函数，必须重载
	void InstanceEntry( CMessage * const pcMsg );
	//GUARD模块启动消息
	void ProcGuardPowerOn(CMessage* const pcMsg);
	//单板状态定时扫描
	void ProcGuardStateScan(CMessage* const pcMsg);

private:
	u8   m_byCriAlarm;
#ifndef WIN32
	TBrdCRILedStateDesc m_tLedState;
#endif
};

typedef zTemplate< CBoardGuard, 1 > CBoardGuardApp;
extern CBoardGuardApp	g_cBrdGuardApp;	//单板的守卫应用实例

#endif    /* CRIGUARD_H */
