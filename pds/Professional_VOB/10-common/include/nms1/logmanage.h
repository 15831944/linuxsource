/*=============================================================================
模   块   名: 网管公用模块库
文   件   名: logmanage.h
相 关  文 件: logmanage.cpp
文件实现功能: 日志管理类
作        者: 刘瑞飞
版        本: V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
说        明: 需要MFC支持
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/04/07  3.6     刘瑞飞      创建
2005/12/08  4.0     王昊        分离出单独文件
=============================================================================*/

#ifndef _LOGMANAGE_20051208_H_
#define _LOGMANAGE_20051208_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "kdvtype.h"

//////////////////////////////////////////////////////////////////////////
//
//      日志文件的相关操作类
//

//日志记录信息接口, 实际使用结构由本基类派生而来。
//一条基本记录信息应该包含:流水号、时间、简单描述、详细信息
class CLogItem
{
public:
    CLogItem() {}
    virtual ~CLogItem() {}

    /*=============================================================================
    函 数 名:GetSummaryLen
    功    能:得到日志记录简单信息的长度
    参    数:无
    注    意:无
    返 回 值:简单信息长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    virtual s32 GetSummaryLen(void) const = 0;

    /*=============================================================================
    函 数 名:GetDetailLen
    功    能:得到日志记录详细信息的长度
    参    数:无
    注    意:无
    返 回 值:详细信息长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	virtual s32 GetDetailLen(void) const = 0;

    /*=============================================================================
    函 数 名:GetSummary
    功    能:得到日志记录的简单信息
    参    数:无
    注    意:无
    返 回 值:简单信息
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	virtual LPCTSTR GetSummary(void) const = 0;

    /*=============================================================================
    函 数 名:GetDetail
    功    能:得到日志记录的详细信息
    参    数:无
    注    意:无
    返 回 值:详细信息
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	virtual LPCTSTR GetDetail(void) const = 0;

    /*=============================================================================
    函 数 名:SetSummary
    功    能:设置日志记录的简单信息
    参    数:LPCTSTR lptrSummary                [in]    简单信息
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	virtual void SetSummary(LPCTSTR lptrSummary) = 0;

    /*=============================================================================
    函 数 名:SetDetail
    功    能:设置日志记录的详细信息
    参    数:LPCTSTR lptrDetail                 [in]    详细信息
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	virtual void SetDetail(LPCTSTR lptrDetail) = 0;

    /*=============================================================================
    函 数 名:IsAdd
    功    能:判断日志信息是不是要记录
    参    数:void *pcbData                      [in]    回调数据
    注    意:被CLogManage::AddLog调用
    返 回 值:是否需要记录
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	virtual BOOL32 IsAdd(void *pcbData = NULL) = 0;

    /*=============================================================================
    函 数 名:SetSquence
    功    能:设置日志记录的流水号
    参    数:u32 dwSn                           [in]    流水号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    void SetSquence(u32 dwSn) { m_dwSn = dwSn; }

    /*=============================================================================
    函 数 名:GetSquence
    功    能:获取日志记录的流水号
    参    数:无
    注    意:无
    返 回 值:流水号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    u32 GetSquence(void) const { return m_dwSn; }

    /*=============================================================================
    函 数 名:SetTimeDate
    功    能:设置日志记录发生的时间
    参    数:time_t tOccurTime                  [in]    记录时间
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    void SetTimeDate(time_t tOccurTime) { m_tOccurTime = tOccurTime; }

protected:
    u32     m_dwSn;             //流水号
    time_t  m_tOccurTime;       //日志发生的时间
};

//////////////////////////////////////////////////////////////////////////
//
//      日志管理类
//

#define MAX_LOG_BUF_SIZE        (u32) 64 * 1024 //最大缓存长度64K
#define MAX_LOG_NUM             (u32) 10000     //最大日志记录条数
#define DEFAULT_LOG_STEP        (u32) 10        //默认日志增长步长

class CLogManage
{
public:
    //加入多少的日志记录时候就写入文件
    enum{ WRITE_NUM = 100 };

    //获取日志记录的类型,
    enum{ TYPE_SUMMARY, TYPE_DETAIL, TYPE_ALL };

    /*=============================================================================
    函 数 名:CLogManage
    功    能:构造函数
    参    数:CLogItem *pcLog                    [in]    日志记录的信息
             LPCTSTR lptrFilePath               [in]    日志文件的全路径，包括最后的文件名
             u32 dwBufferSize                   [in]    读取日志文件时候的缓冲区的大小
             u32 dwLogMaxNum                    [in]    文件里面最大的日志数目
             u32 dwMallocStep                   [in]    动态增长的步长
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    CLogManage(CLogItem *pcLog, LPCTSTR lptrFilePath,
               u32 dwBufferSize = MAX_LOG_BUF_SIZE,
		       u32 dwLogMaxNum = MAX_LOG_NUM,
               u32 dwMallocStep = DEFAULT_LOG_STEP);

    /*=============================================================================
    函 数 名:~CLogManage
    功    能:析构函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    virtual ~CLogManage();

public:
    /*=============================================================================
    函 数 名:AddLog
    功    能:把新增加的日志写入内存，当内存中的日志达到一定的数目时候，写入日志文件里
    参    数:CLogItem *pcLog                    [in]    增加的日志记录
             void *pcbData                      [in]    调用IsAdd时的输入参数
    注    意:会调用CLogItem的IsAdd判断是否需要写入内存
    返 回 值:日志记录的流水号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    2005/05/23  3.6     王昊    写入m_pchSummaryData查位置应该用m_dwSn来定位
    =============================================================================*/
	u32 AddLog(CLogItem *pcLog, void *pcbData = NULL);

    /*=============================================================================
    函 数 名:GetLog
    功    能:得到一条日志记录,可以是详细,可以是简单,也可以是全部的信息
    参    数:u32 dwSn                           [in]    要查找的日志记录的流水号
             CLogItem *pcLog                    [out]   日志记录
             u32 dwType                         [in]    TYPE_SUMMARY 得到简单信息
                                                        TYPE_DETAIL  得到详细信息
                                                        TYPE_ALL	 得到日志的全部信息
    注    意:无
    返 回 值:成功返回 TRUE，失败返回 FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	BOOL32 GetLog(u32 dwSn, CLogItem *pcLog, u32 dwType = TYPE_SUMMARY);

    /*=============================================================================
    函 数 名:GetLog
    功    能:得到一批日志记录的简单信息, 信息只能是简单信息
    参    数:u32 dwSn                           [in]    要查找的日志记录的流水号
             CLogItem *pcLog                    [out]   日志记录
             u32 dwOffSet                       [in]    指针的偏移量，即派生类的大小 
             u32 &dwNum                         [in/out]    要查找的日志的数目/实际数目
    注    意:如果查找的日志的流水号小于日志文件的最小的流水号,则从最小的流水号开始查起
    返 回 值:查找到记录: 返回找到最后一条记录的下一条纪录的流水号;
             没有找到记录: 返回0;
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	u32 GetLog(u32 dwSn, CLogItem *pcLog, u32 dwOffset, u32 &dwNum) const;

    /*=============================================================================
    函 数 名:DeleteLog
    功    能:删除指定流水号的日志记录
    参    数:u32 dwSn                           [in]    要删除的日志记录的流水号
    注    意:无
    返 回 值:成功返回TRUE, 失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	BOOL32 DeleteLog(u32 dwSn);

    /*=============================================================================
    函 数 名:ClearLog
    功    能:清除日志记录
    参    数:无
    注    意:无
    返 回 值:成功删除返回TRUE, 失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	BOOL32 ClearLog(void);

    /*=============================================================================
    函 数 名:GetCount
    功    能:得到日志的数目
    参    数:无
    注    意:无
    返 回 值:日志数目
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    inline u32 GetCount(void) const { return m_dwCount; }

    /*=============================================================================
    函 数 名:GetUsedCount
    功    能:获取日志实际使用数目
    参    数:无
    注    意:无
    返 回 值:使用数目
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/24  3.6     王昊    创建
    =============================================================================*/
    inline u32 GetUsedCount(void) const { return m_dwUsedCount; }

    /*=============================================================================
    函 数 名:GetMinSn
    功    能:得到日志的最小可用的流水号
    参    数:无
    注    意:无
    返 回 值:最小可用的流水号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    2005/05/23  3.6     王昊    流水号必须是最小的, 且存在的
    =============================================================================*/
	u32 GetMinSn(void) const;

    /*=============================================================================
    函 数 名:GetMaxSn
    功    能:得到日志文件中流水号的最大值
    参    数:无
    注    意:无
    返 回 值:流水号的最大值
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
    inline u32 GetMaxSn(void) const { return m_dwSn; }

    /*=============================================================================
    函 数 名:GetLogFilePath
    功    能:得到日志文件的路径
    参    数:无
    注    意:路径是绝对路径, 包含文件名
    返 回 值:日志文件的路径
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	CString& GetLogFilePath(void);

    /*=============================================================================
    函 数 名:GetLogFileName
    功    能:得到日志文件的文件名
    参    数:无
    注    意:仅文件名
    返 回 值:文件名
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	CString GetLogFileName(void);

protected:
    /*=============================================================================
    函 数 名:SetLogFilePath
    功    能:设置日志文件的路径
    参    数:LPCTSTR lptrNewName                [in]    文件路径
    注    意:可以是相对的, 也可以是绝对的
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	inline void SetLogFilePath(LPCTSTR lptrNewName)
    {
        if ( lptrNewName != NULL )
        {
            m_csPathName = lptrNewName;
        }
    }

    /*=============================================================================
    函 数 名:GetDetailLen
    功    能:得到详细信息的长度
    参    数:CLogItem *pcLog                    [in]    日志记录
    注    意:无
    返 回 值:详细信息的长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	inline s32 GetDetailLen(CLogItem *pcLog) 
	{
		m_nDetailLen = pcLog->GetDetailLen();
		return m_nDetailLen;
	}

    /*=============================================================================
    函 数 名:GetSummaryLen
    功    能:得到简要信息的长度
    参    数:CLogItem *pcLog                    [in]    日志记录
    注    意:无
    返 回 值:简要信息的长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	inline s32 GetSummaryLen(CLogItem *pcLog) 
	{
		m_nSummaryLen = pcLog->GetSummaryLen();
		return m_nSummaryLen;
	}

    /*=============================================================================
    函 数 名:ReadLog
    功    能:把文件里面日志记录的简单信息读到内存里面
    参    数:无
    注    意:无
    返 回 值:成功: TRUE; 失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    2005/05/23  3.6     王昊    处理空值情况
    =============================================================================*/
	BOOL32 ReadLog(void);

    /*=============================================================================
    函 数 名:GetSummary
    功    能:得到指定流水号的日志记录的简单信息
    参    数:u32 dwSn                           [in]    要查找的日志记录的流水号
             CLogItem *pcLog                    [out]   日志记录
    注    意:无
    返 回 值:成功: TRUE; 失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	BOOL32 GetSummary(u32 dwSn, CLogItem *pcLog) const;

    /*=============================================================================
    函 数 名:GetDetail
    功    能:得到指定流水号的日志记录的详细信息
    参    数:u32 dwSn                           [in]    要查找的日志记录的流水号
             CLogItem *pcLog                    [out]   日志记录
    注    意:无
    返 回 值:成功: TRUE; 失败: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	BOOL32 GetDetail(u32 dwSn, CLogItem *pcLog);

    /*=============================================================================
    函 数 名:GetLogLen
    功    能:得到一条日志的长度
    参    数:无
    注    意:无
    返 回 值:一条日志记录的长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	inline u32 GetLogLen(void)
    {
        m_nLogLen = m_nDetailLen + m_nSummaryLen + sizeof(u32) + sizeof(time_t);	
	    return m_nLogLen;
    }

    /*=============================================================================
    函 数 名:MallocBuffer
    功    能:动态分配内存
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	void MallocBuffer(void);

    /*=============================================================================
    函 数 名:WriteLog
    功    能:把内存里面的日志记录写到日志文件里面去
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/07  3.6     刘瑞飞  创建
    =============================================================================*/
	void WriteLog(void);

protected:
    u32 m_dwLogMaxSize;					//日志文件里存放的最大日志数目
    u32 m_dwCount;						//记录日志记录的数目
    u32 m_dwMallocStep;				    //动态增长内存的步长
	u32 m_dwMallocNum;					//动态分配内存递减指示器
	u32 m_dwNewAddCount;				//记录新增加的日志文件的数目
    u32 m_dwUsedCount;                  //记录的有效使用数目

    u32 m_dwSn;							//流水号

	u32 m_dwBufferSize;					//读取文件的缓冲区的大小
    s32 m_nSummaryLen;					//简要信息的长度
    s32 m_nDetailLen;					//详细信息的长度

    s32 m_nLogLen;						//日志记录的长度

	s8* m_pchBuffer;					//读取文件的缓冲区
    s8* m_pchSummaryData;               //读入的简单信息的缓存
    s8* m_pchData;						//写入的日志记录的缓存区

    time_t m_tOccurTime;				//日志的发生时间
	CString m_csPathName;				//日志文件的名字
};

#endif  //  _LOGMANAGE_20051208_H_