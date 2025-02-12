/*=============================================================================
模   块   名: 会议控制台业务功能库
文   件   名: mcsstruct.h
相 关  文 件: mcsstruct.cpp
文件实现功能: mcslib中基本数据结构
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/04/27  4.0     王昊        创建
=============================================================================*/


#ifndef _MCSSTRUCT_20050422_H_
#define _MCSSTRUCT_20050422_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#include "mcustruct.h"
#include "vcsstruct.h"
#include "mcsconst.h"
#include "nmscommon.h"
#include <afxmt.h>
#include "Item.h"
#include "ummessagestruct.h"
#include <map>
#include <set>
#include <vector>
using namespace std;

// 重定义下名称，如果以后再升级版本，无需更改太多代码。
typedef TMcuAddrEntryV1 TMcuAddrEntryCur;
typedef TMcuAddrGroupV1 TMcuAddrGroupCur;
typedef TMtExtU			TMtExtCur;

/*************************************
************************************/
#define MAX_HDU_VMP_NUM  56
//struct TVmpParamInfo;
class CConf;

#define MCS_MAXLEN_VIDEOSOURCE_ALIAS   64
#define MCU_RE_GK_PASSWORD_LEN		   64 //MCU注册GK密码

#define EQP_TYPE_HDU2_S			       100

//画面合成参数信息 //added by twb.
typedef struct tagVmpParamInfo
{
private:
	typedef set<u32>	MtSet;
	TVMPParam_25Mem		tVmpParam;		
	MtSet			    setVmpMember;	//画面合成器中成员，方便查找
	BOOL32				bOldMcuFlag;		//是否是老的MCU
	s8					achAlias[MAXLEN_EQP_ALIAS];
	
public:
	tagVmpParamInfo(){ 
		ZeroMemory((void*)&tVmpParam, sizeof(TVMPParam_25Mem) );
		ZeroMemory((void*)achAlias, sizeof(achAlias) );
		bOldMcuFlag = FALSE;
	}
	~tagVmpParamInfo(){ }
	void SetVmpParam(CConf *pConf, TVMPParam_25Mem &tParam)
	{	
		memcpy(&tVmpParam, &tParam, sizeof(TVMPParam_25Mem));
		
		u8 byVmpMember = tParam.GetMaxMemberNum();
		
		for ( u8 byIndex = 0; byIndex < byVmpMember; byIndex++ )
		{
			TVMPMember* ptVmpMember = tParam.GetVmpMember( byIndex );
			if ( ptVmpMember && ptVmpMember->IsNull() == FALSE )
			{
				u32 dwMtIdx = MAKEDWORD( ptVmpMember->GetMcuIdx(), ptVmpMember->GetMtId() );
				setVmpMember.insert( dwMtIdx );
			}
		}
	}
	void SetOldMcuFlag(BOOL32 bOldFlag)
	{
		bOldMcuFlag = bOldFlag;
	}
	BOOL32 IsOldMcu()
	{
		return bOldMcuFlag;
	}
	BOOL32 IsVmpMember(TMt &tMt)
	{
		u32 dwMtIdx = MAKEDWORD( tMt.GetMcuIdx(), tMt.GetMtId() );
		
		if ( setVmpMember.find( dwMtIdx ) == setVmpMember.end() )
		{
			return FALSE;
		}
		
		return TRUE;
	}
	
	TVMPParam_25Mem& GetVmpParam(){ return tVmpParam; }
	MtSet&  GetVmpMemberSet(){ return setVmpMember; }
	
	s8* GetVmpAlias(){ return achAlias; }
	void SetVmpAlias(const s8* pchAlias)
	{
		ZeroMemory((void*)achAlias, sizeof(achAlias));
		
		strncpy((s8*)achAlias, pchAlias, strlen(pchAlias));
	}
}TVmpParamInfo;

typedef map<u8, TVmpParamInfo> MAPVMPPARAMINFO;

//视频源结构体
typedef struct tagMcsMtVideoSrcAlias    //added by twb 2011.4.14
{
    tagMcsMtVideoSrcAlias() { SetNull(); }
    
    inline void SetNull(void) { memset( this, 0, sizeof (tagMcsMtVideoSrcAlias) ); }
    
    s8          m_szMtVideoAlias[EX_VIDEOSOURCE_BASE][MCS_MAXLEN_VIDEOSOURCE_ALIAS];   //视频源别名
    
    u8             m_byVidIdx[EX_VIDEOSOURCE_BASE];                                 //视频源端口索引
	
    u8             m_byVideoSrcNum;                                //视频源个数
    
}TMcsMtVideoSrcAlias;

typedef struct tagMcsMtVideoSrcAliasParam
{
    TMt                     m_tCurMt;                                       //当前终端
	
    TMcsMtVideoSrcAlias     m_tMcsMtVideoSrcAlias;  
    
}TMcsMtVideoSrcAliasParam;


//录像机参数设置  由于MCU不能修改TRecStartPara所以增加此结构体方便传参
typedef struct tagRecParam
{
    TRecStartPara   m_tRecStartPara;
    TSimCapSet      m_tSimCapSet;
}TRecParam;

// 按钮的数据
typedef struct tagVcsBtnItemData
{
    tagVcsBtnItemData()
    {
		m_dwDataType = 0;
    }
	
    TVCSEntryInfo   m_tEntryInfo;
    TMt             m_tMt;
	
    u32 m_dwDataType;
	
}TItemData, *PTItemData;

//vcs 预案
typedef struct tagVcsSchema
{
	tagVcsSchema()
	{
		SetNull();
	}	
	s8 m_pbySchemaName[ MAX_VCSGROUPNAMELEN ];
	vector <TMtVCSPlanAlias> m_vctTMtVCSPlanAlias;
	vector <TItemData> m_vctTItemData;

	void SetNull( )
	{
		ZeroMemory( m_pbySchemaName, sizeof( m_pbySchemaName ) );
		m_vctTMtVCSPlanAlias.clear();
		m_vctTItemData.clear();
	}

	void PopUpItem( TItemData& tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); nIndex ++ )
		{
			// 			if ( tItemData.m_tMt == m_vctTItemData[ nIndex ].m_tMt)
			// 			{
			// 				m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
			// 				break;
			// 			}
			TItemData &tItem = m_vctTItemData[nIndex];
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					break;
				}
			}
			else
			{
				if ( memcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName, sizeof(tItem.m_tEntryInfo.achEntryName)) == 0 )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					break;
				}
			}
		}
	}
	
	void PopupItemAndAlias( TItemData& tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); ++nIndex )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					m_vctTMtVCSPlanAlias.erase( m_vctTMtVCSPlanAlias.begin() + nIndex );
					break;
				}
			}
			else
			{
				if ( memcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName, sizeof(tItem.m_tEntryInfo.achEntryName)) == 0 )
				{
					m_vctTItemData.erase( m_vctTItemData.begin( ) + nIndex );
					m_vctTMtVCSPlanAlias.erase( m_vctTMtVCSPlanAlias.begin() + nIndex );
					break;
				}
			}
		}
	}
	
    //modified by twb 2010.8.31  增加对PU终端的支持
	s32 Forward( TItemData & tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); nIndex ++ )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
			
            BOOL32  bSameMt = FALSE;
			
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					bSameMt = TRUE;
				}
			}
			else
			{
				if (tItemData.m_tEntryInfo.byAddrType == puAliasTypeIPPlusAlias)
				{
					if ( strcmp( tItemData.m_tEntryInfo.achUIName, tItem.m_tEntryInfo.achUIName) == 0 
						&& strcmp( tItemData.m_tEntryInfo.achUIName, "") != 0)
					{
						bSameMt = TRUE;
					}
				}
				else
				{
					if ( strcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName) == 0 )
					{
						bSameMt = TRUE;
					}
				}
			}
			
            if (bSameMt)
			{
				if ( nIndex == 0)
				{
					return nIndex;
				}
				TItemData tTemData = m_vctTItemData[ nIndex - 1 ];
				m_vctTItemData[ nIndex - 1 ] = tItemData;
				m_vctTItemData[ nIndex ] = tTemData;
				
				TMtVCSPlanAlias tAlias = m_vctTMtVCSPlanAlias[ nIndex - 1 ];
				m_vctTMtVCSPlanAlias[ nIndex - 1 ] = m_vctTMtVCSPlanAlias[ nIndex ];
				m_vctTMtVCSPlanAlias[ nIndex ] = tAlias;
				return nIndex - 1;
				
			}
		}
		return -1;
	}
	
    //modified by twb 2010.8.31  增加对PU终端的支持
	s32 Backward( TItemData & tItemData )
	{
		for ( u32 nIndex = 0; nIndex< m_vctTItemData.size(); nIndex ++ )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
            BOOL32  bSameMt = FALSE;
			
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					bSameMt = TRUE;
				}
			}
			else
			{
				if (tItemData.m_tEntryInfo.byAddrType == puAliasTypeIPPlusAlias)
				{
					if ( strcmp( tItemData.m_tEntryInfo.achUIName, tItem.m_tEntryInfo.achUIName) == 0 
						&& strcmp( tItemData.m_tEntryInfo.achUIName, "" ) != 0)
					{
						bSameMt = TRUE;
					}
				}
				else
				{
					if ( strcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName) == 0 )
					{
						bSameMt = TRUE;
					}
				}
			}
            
            if (bSameMt)
			{
				if ( nIndex == m_vctTItemData.size() - 1 )
				{
					return nIndex;
				}
				TItemData tTemData = m_vctTItemData[ nIndex + 1 ];
				m_vctTItemData[ nIndex + 1 ] = tItemData;
				m_vctTItemData[ nIndex ] = tTemData;
				
				TMtVCSPlanAlias tAlias = m_vctTMtVCSPlanAlias[ nIndex + 1 ];
				m_vctTMtVCSPlanAlias[ nIndex + 1 ] = m_vctTMtVCSPlanAlias[ nIndex ];
				m_vctTMtVCSPlanAlias[ nIndex ] = tAlias;
				return nIndex + 1;
			}
		}
		return -1;
	}
	
	BOOL32 IsItemExist( const TItemData & tItemData )
	{
		for ( u32 nIndex = 0; nIndex < m_vctTItemData.size(); nIndex ++ )
		{
			TItemData &tItem = m_vctTItemData[nIndex];
			//先判断TMt -2011.3.15 by xcr
			if ( !tItemData.m_tMt.IsNull() && !tItem.m_tMt.IsNull() )
			{
				if ( tItemData.m_tMt == tItem.m_tMt )
				{
					return TRUE;
				}
			}
			else
			{
				if ( memcmp( tItemData.m_tEntryInfo.achEntryName, tItem.m_tEntryInfo.achEntryName, sizeof(tItem.m_tEntryInfo.achEntryName)) == 0 )
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	
}TVCSSchema, *PTVCSSchema;

// 画面合成器增加的通道数
#define VMP_MEMBER_EX_NUM ( MAXNUM_VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER )

struct TVmpModuleEx : public TVmpModule
{
public:
	TVmpChnnlMember m_atVmpMemberEx[MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER];
public:
	void EmptyMember(u8 byMemberId)
	{
		if (byMemberId >= MAXNUM_MPU2VMP_MEMBER)
			return;
		if (byMemberId < MAXNUM_MPUSVMP_MEMBER)
		{
			TVmpModule::EmptyMember();
		}
		else
		{
			memset(this, 0, (MAXNUM_MPU2VMP_MEMBER - MAXNUM_MPUSVMP_MEMBER)*sizeof(TVmpChnnlMember));
		}
	}
	// 设置某个通道
    void SetVmpMember(u8 byChnlIdx, u8 byMtIdx, u8 byMemberType)
    {
		if (byChnlIdx >= MAXNUM_MPU2VMP_MEMBER)
			return;
        if (byChnlIdx < MAXNUM_VMP_MEMBER)
        {
            m_abyVmpMember[byChnlIdx]  = byMtIdx;
            m_abyMemberType[byChnlIdx] = byMemberType;
        }
		else
		{
			for(u8 byIdx = 0; byIdx < 5; byIdx++)
			{
				if (m_atVmpMemberEx[byIdx].m_byVmpMember == 0)
				{
					m_atVmpMemberEx[byIdx].m_byChnIdx = byChnlIdx;
					m_atVmpMemberEx[byIdx].m_byVmpMember = byMtIdx;
					m_atVmpMemberEx[byIdx].m_byMemberType = byMemberType;
					break;
				}
			}		
		}
    }
	void RemoveVmpMember(u8 byChnlIdx) 
    {
		if (byChnlIdx >= MAXNUM_MPU2VMP_MEMBER)
			return;
        if(byChnlIdx < MAXNUM_VMP_MEMBER)
        {
            m_abyVmpMember[byChnlIdx]  = 0;
            m_abyMemberType[byChnlIdx] = 0;
        }
		else
		{
			for(u8 byIdx = 0; byIdx < 5; byIdx++)
			{
				if (m_atVmpMemberEx[byIdx].m_byChnIdx == byChnlIdx)
				{
					m_atVmpMemberEx[byIdx].m_byChnIdx = 0;
					m_atVmpMemberEx[byIdx].m_byVmpMember = 0;
					m_atVmpMemberEx[byIdx].m_byMemberType = 0;
				}
			}		
		}
    }
	BOOL32 SetVmpMemberEx(TVmpChnnlMember* tVmpMemberEx, u8 byCapNum )
	{
		if ( tVmpMemberEx == NULL || byCapNum == 0 || byCapNum > 5 )
		{ 
			StaticLog("[SetVmpMemberEx] invalid param!\n");
			return FALSE;
		}
		else
		{
			for(u8 byIdx = 0; byIdx < 5; byIdx++)
			{
				m_atVmpMemberEx[byIdx].clear();
			}
			memcpy((u8*)&m_atVmpMemberEx[0],tVmpMemberEx,byCapNum*sizeof(TVmpChnnlMember));
			return TRUE;
		}
	}
	u8 GetVmpMemberEx(TVmpChnnlMember* tVmpMemberEx)const
	{
		u8 byNum = 0;
		if (tVmpMemberEx == NULL)
		{
			StaticLog("[GetVmpMemberEx] invalid param!\n");
			return 0;
		}
		for(u8 byIdx = 0; byIdx < 5; byIdx++)
		{
			if(m_atVmpMemberEx[byIdx].m_byVmpMember != 0)
			{
				tVmpMemberEx[byIdx] = m_atVmpMemberEx[byIdx];
				byNum++;
			}	
		}
		return byNum;
	}
}
PACKED
;


// 界面用的VmpModule数据结构，取代原来MCU的TVmpModule
typedef struct tagMcsVmpModule
{
	TVMPParam_25Mem m_tVMPParam;                             //会控指定的画面合成参数
	u8				m_abyVmpMember[MAXNUM_VMP_MEMBER];   //会控指定的画面合成成员索引（即创会时TMtAlias数组索引+1）
	u8				m_abyMemberType[MAXNUM_VMP_MEMBER];  //画面合成成员的跟随方式, VMP_MEMBERTYPE_MCSSPEC, VMP_MEMBERTYPE_SPEAKER...

	tagMcsVmpModule()
	{
		EmptyMember();
	}

	void EmptyMember()
	{
		memset( this, 0, sizeof(tagMcsVmpModule) );    
	}

	// 设置某个通道
	void SetVmpMember(u8 byChnlIdx, u8 byMtIdx, u8 byMemberType)
	{
		if ( byChnlIdx < MAXNUM_VMP_MEMBER )
		{
			m_abyVmpMember[byChnlIdx]  = byMtIdx;
			m_abyMemberType[byChnlIdx] = byMemberType;
		}
	}

	// 移除某个通道
	void RemoveVmpMember(u8 byChnlIdx) 
	{
		if( byChnlIdx < MAXNUM_VMP_MEMBER )
		{
			m_abyVmpMember[byChnlIdx]  = 0;
			m_abyMemberType[byChnlIdx] = 0;
		}
	}

	// 设置合成参数
	void SetVmpParam(const TVMPParam &tParam)
	{
		*static_cast<TVMPParam*>(&m_tVMPParam) = tParam;
	}

	// 取得合成参数
	TVMPParam GetVmpParam()
	{
		return m_tVMPParam;
	}

	void SetVmpModule( TVmpModule& vmpModule )
	{
		SetVmpParam( vmpModule.m_tVMPParam );
		memcpy( &m_abyMemberType, &vmpModule.m_abyMemberType, MAXNUM_MPUSVMP_MEMBER );
		memcpy( &m_abyVmpMember, &vmpModule.m_abyVmpMember, MAXNUM_MPUSVMP_MEMBER );
	}

	void GetVmpModule( TVmpModule& vmpModule )
	{
		vmpModule.m_tVMPParam = *static_cast<TVMPParam*>( &m_tVMPParam );
		memcpy( &vmpModule.m_abyMemberType, &m_abyMemberType, MAXNUM_MPUSVMP_MEMBER );
		memcpy( &vmpModule.m_abyVmpMember, &m_abyVmpMember, MAXNUM_MPUSVMP_MEMBER );
	}
}TMcsVmpModule;

struct THduChnnl
{
public:
	u8 m_byChnnlIdx;    //通道号
	u8 m_byEqpId; 
	u8 m_byMemberIdx; //会控指定的电视墙成员索引（即创会时TMtAlias数组索引+1)如果m_abyTvWallMember[x]的值是193， 那么它表示该通道是一个该随类型通道
	u8 m_byMemberType;  //电视墙成员跟随类型,TW_MEMBERTYPE_MCSSPEC,TW_MEMBERTYPE_SPEAKER...
	
public:
	THduChnnl( void )
	{
		Clear();
	}
	
	~THduChnnl()
	{
		Clear();
	}
	void SetHduChnnl(u8 byEqpId, u8 byChnnlIdx, u8 byMemberIdx, u8 byMemberType)
	{
		m_byEqpId = byEqpId;
		m_byChnnlIdx = byChnnlIdx;
		m_byMemberIdx = byMemberIdx;
		m_byMemberType = byMemberType;
	}
	void Clear( void ){memset(this,0,sizeof(THduChnnl));}
}
PACKED
;

struct THduVmpOne
{
public:
	THduVmpChnnl m_tHduVmpChnnl;
	THduVmpSubChnnl m_atHduVmpSubChnnl[4];
public:
	THduVmpOne()
	{
		Empty();
	}
	~THduVmpOne()
	{
		Empty();
	}
	void Empty()
	{
		m_tHduVmpChnnl.Clear();
		for(u8 byIndex = 0; byIndex < 4; byIndex++)
		{
			m_atHduVmpSubChnnl[byIndex].Clear();
		}
	}
	void SetHduVmpMember(u8 byHduEqpId, u8 byChnnlIdx, u8 byVmpStyle)
    {	
		m_tHduVmpChnnl.m_byHduEqpId = byHduEqpId;
		m_tHduVmpChnnl.m_byChnnlIdx = byChnnlIdx;
		m_tHduVmpChnnl.m_byVmpStyle = byVmpStyle;		
    }
	void SetHduVmpSubMember(u8 byHduSubEqpId, u8 byChnnlMember, u8 bySubVmpStyle)
    {
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			if (m_atHduVmpSubChnnl[byIdx].m_byMemberType == 0)
			{
				m_atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx = byHduSubEqpId;
				m_atHduVmpSubChnnl[byIdx].m_byMember = byChnnlMember;
				m_atHduVmpSubChnnl[byIdx].m_byMemberType = bySubVmpStyle;
				break;
			}
		}		
    }
	
	void GetHduVmpSubMember(u8 &byIndex , u8 &byChnnlMember, u8 &byMemberType)
    {	
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			if (m_atHduVmpSubChnnl[byIdx].m_byMemberType != 0)
			{ 
				if (m_atHduVmpSubChnnl[byIdx].m_bySubChnnlIdx == byIndex)
				{
					byChnnlMember = m_atHduVmpSubChnnl[byIdx].m_byMember;
					byMemberType = m_atHduVmpSubChnnl[byIdx].m_byMemberType;
					break;
				}
			}	
		}
    }

	BOOL32 SetHduSubVmpMemberEx(THduVmpSubChnnl* tHduVmpSubChnnl, u8 byCapNum )
	{
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			m_atHduVmpSubChnnl[byIdx].Clear();
		}
		memcpy((u8*)&m_atHduVmpSubChnnl[0],tHduVmpSubChnnl,byCapNum*sizeof(THduVmpSubChnnl));
		return TRUE;
	}
	u8 GetHduVmpMemberEx(THduVmpSubChnnl* tHduVmpSubChnnl)const
	{
		u8 byNum = 0;
		if (tHduVmpSubChnnl == NULL)
		{
			StaticLog("[GetVmpMemberEx] invalid param!\n");
			return 0;
		}
		for(u8 byIdx = 0; byIdx < 4; byIdx++)
		{
			if(m_atHduVmpSubChnnl[byIdx].m_byMemberType != 0)
			{
				tHduVmpSubChnnl[byIdx] = m_atHduVmpSubChnnl[byIdx];
				byNum++;
			}	
		}
		return byNum;
	}
	
}
PACKED
;

struct THduVmpTotalEx
{
public:
	THduVmpOne m_atHduVmpOne[MAX_HDU_VMP_NUM];
public:
	THduVmpTotalEx()
	{
		Empty();
	}
	~THduVmpTotalEx()
	{
		Empty();
	}
	void Empty()
	{
		for(u8 byIndex = 0; byIndex < MAX_HDU_VMP_NUM; byIndex++)
		{
			m_atHduVmpOne[byIndex].Empty();
		}
	}
	void SetHduVmp(THduVmpOne& tHduVmpOne)
    {
		for(u8 byIdx = 0; byIdx < MAX_HDU_VMP_NUM; byIdx++)
		{
			if (m_atHduVmpOne[byIdx].m_tHduVmpChnnl.m_byHduEqpId == 0)
			{
				m_atHduVmpOne[byIdx] = tHduVmpOne;
				break;
			}
		}		
    } 
	void SetHduVmpChnnl(THduVmpChnnl* tHduVmpChnnl)
	{
		for(u8 byNum = 0; byNum < MAX_HDU_VMP_NUM; byNum++)
		{
			if (m_atHduVmpOne[byNum].m_tHduVmpChnnl.m_byHduEqpId == 0)
			{
				m_atHduVmpOne[byNum].m_tHduVmpChnnl = *tHduVmpChnnl;
				break;
			}
		}
	}

	void SetHduVmpSubChnnl(THduVmpSubChnnl* tHduVmpSubChnnl, u8 byCapNum )
	{
		for(s32 nNum = MAX_HDU_VMP_NUM - 1; nNum >= 0; nNum--)
		{
			if (m_atHduVmpOne[nNum].m_tHduVmpChnnl.m_byHduEqpId != 0)
			{
				m_atHduVmpOne[nNum].SetHduSubVmpMemberEx(tHduVmpSubChnnl, byCapNum);
				break;
			}
		}
	}

	u8 GetHduVmp(THduVmpOne* tHduVmpOne)const
	{
		u8 byNum = 0;
		if (tHduVmpOne == NULL)
		{
			StaticLog("[GetVmpMemberEx] invalid param!\n");
			return 0;
		}
		for(u8 byIdx = 0; byIdx < MAX_HDU_VMP_NUM; byIdx++)
		{
			if(m_atHduVmpOne[byIdx].m_tHduVmpChnnl.m_byHduEqpId != 0)
			{
				tHduVmpOne[byIdx] = m_atHduVmpOne[byIdx];
				byNum++;
			}	
		}
		return byNum;
	}
}
PACKED
;

//added by spriner 20100518 下级mcu是否支持多回传
//发送给上层的消息结构，在消息的WPARAM是指向此结构的指针
typedef struct tagMcsParam
{
    tagMcsParam() : m_pbyMsgBody(NULL), m_nMsgBodySize(0), m_dwErrorCode(0),
		m_bSelfProduce(FALSE) {}
    u8      *m_pbyMsgBody;          //消息体
    s32     m_nMsgBodySize;         //消息体长度
    u32     m_dwErrorCode;          //错误码
    BOOL32  m_bSelfProduce;         //是否由此会控引起的消息(应该没什么用)
} TMcsParam;

//监控参数结构
typedef struct tagMonitorParam : public TMt
{
    tagMonitorParam()
    {
        ZeroMemory( this, sizeof (tagMonitorParam) );
    }
    u32             m_dwLocalIp;        //  本机IP
    TMt             m_tVideoMt;         //  监控视频源
    TMt             m_tAudioMt;         //  监控音频源
    u16             m_wLocalPort;       //  接收码流端口
    u8              m_byReal;           //  0-抽帧监控, 1-实时监控
    u8              m_byQuiet;          //  0-不静音, 1-静音
    u8              m_byMode;           //  MODE_BOTH/MODE_VIDEO/MODE_AUDIO
    u8              m_byReserved;       //  保留字段
    CConfId         m_cConfId;          //  会议号
    TMediaEncrypt   m_tVideoMedia;      //  视频加密
    TDoublePayload  m_tVideoPayload;    //  视频载荷
    TMediaEncrypt   m_tAudioMedia;      //  音频加密
    TDoublePayload  m_tAudioPayload;    //  音频载荷
    TAudAACCap      m_tAudAACCap;       //  AAC的参数
	TTransportAddr  m_tVideoTransportAddr;   //  视屏RCTP端口和IP
	TTransportAddr  m_tAudioTransportAddr;   //  音频RCTP端口和IP
} TMonitorParam;

//切换监控窗口信息
typedef struct tagSwitchMonitor : public TMt
{
    tagSwitchMonitor()
    {
        ZeroMemory( this, sizeof (tagSwitchMonitor) );
    }
	
    u8      m_byIndex;
    u8      m_byReal;
    u8      m_byQuiet;
    u8      m_byMode;
    CConfId m_cConfId;          //  会议号
} TSwitchMonitor;

//录像文件列表结构
typedef struct tagRecFileInfo
{
public:
/*=============================================================================
函 数 名:tagFileInfo
功    能:构造函数
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/30  4.0     王昊    创建
    =============================================================================*/
    tagRecFileInfo() { Reset(); }
	
    /*=============================================================================
    函 数 名:GetFileNum
    功    能:获取文件总数
    参    数:无
    注    意:无
    返 回 值:文件数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/30  4.0     王昊    创建
    =============================================================================*/
    s32 inline GetFileNum(void) const { return m_nFileNum; }
	
    /*=============================================================================
    函 数 名:IsPublic
    功    能:判断指定序号的文件是否发布
    参    数:s32 nIndex                     [in]    文件序号
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/30  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsPublic(s32 nIndex) const;
	
    /*=============================================================================
    函 数 名:Reset
    功    能:重置录像机
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/30  4.0     王昊    创建
    =============================================================================*/
    void Reset(void);
	
    /*=============================================================================
    函 数 名:AddFileName
    功    能:增加文件
    参    数:LPCTSTR pszFileName            [in]    文件名
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/30  4.0     王昊    创建
    =============================================================================*/
    BOOL32 AddFileName(const s8* pszFileName);
	
    /*=============================================================================
    函 数 名:DelFileName
    功    能:删除录像文件时更新文件列表
    参    数:LPCTSTR pszFileName            [in]    文件名
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    BOOL32 DelFileName(const s8* pszFileName);
	
    /*=============================================================================
    函 数 名:Public
    功    能:设置指定序号的文发布
    参    数:s32 nIndex                     [in]    文件序号
	BOOL32 bPublic                 [in]    是否发布
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    BOOL32 Public(s32 nIndex, BOOL32 bPublic);
	
public:
    TEqp    m_tRecEqp;                                              //  录像机
    s8   m_aaszFileArray[MAX_FILE_NUM][MAXLEN_REC_FILE_NAME];    //  文件名数组
	
private:
    s32     m_nFileNum;                                             //  文件数
    u8      m_abyStatus[MAX_FILE_NUM];                              //  文件发布方式数组
} TRecFileInfo;

//mcustatus   mcs侧保存的数据
#ifndef SETBITSTATUS
#define SETBITSTATUS(StatusValue, StatusMask, bStatus)  \
    if (bStatus)    StatusValue |= StatusMask;          \
else            StatusValue &= ~StatusMask;

#define GETBITSTATUS(StatusValue, StatusMask)  (0 != (StatusValue&StatusMask))   
#endif
class CMcsMcuStatus:public TMcu
{
public:
	enum eMcuRunningStatusMask
    {
        Mask_Mp				=  0x00000004,
        Mask_Mtadp			=  0x00000008,
		Mask_HDSC			=  0x00000020,
		Mask_MDSC			=  0x00000040,
		Mask_DSC			=  0x00000080,
		Mask_McuRunOk		=  0x80000000
    };
	
	u8		m_byPeriEqpNum;	                        //外设数目
	TEqp	m_atPeriEqp[MAXNUM_MCU_PERIEQP];	    //外设数组
	u8      m_byEqpOnline[MAXNUM_MCU_PERIEQP];      //外设是否在线
	u32     m_dwPeriEqpIpAddr[MAXNUM_MCU_PERIEQP];  //外设Ip地址
    
    u8      m_byPeriDcsNum;                         //DCS数目
    TEqp    m_atPeriDcs[MAXNUM_MCU_DCS];            //DCS数组
    u8      m_byDcsOnline[MAXNUM_MCU_DCS];          //DCS是否在线
    u32     m_dwPeriDcsIpAddr[MAXNUM_MCU_DCS];      //DCS ip地址

    // xsl [8/26/2005] 
    u8      m_byOngoingConfNum;                     //mcu上即时会议个数
    u8      m_byScheduleConfNum;                    //mcu上预约会议个数
    u16     m_wAllJoinedMtNum;                      //mcu上所有与会终端个数
    
    // 顾振华 [5/29/2006]
    u8      m_byRegedGk;                            //是否成功注册GK。如果未配置或者注册失败，则为0
	u16     m_wLicenseNum;							//当前License数
    TMSSynState m_tMSSynState;                      //当前主备情况(主备环境下有效)
    
    // guzh [9/4/2006] 
    u8      m_byRegedMpNum;                         //当前已注册的Mp数量
    u8      m_byRegedMtAdpNum;                      //当前已注册的H323 MtAdp数量

    // guzh  [12/15/2006]
    u8      m_byNPlusState;                         //备份服务器工作状态(mcuconst.h NPLUS_BAKSERVER_IDLE 等)
    u32     m_dwNPlusReplacedMcuIp;                 //如果备份服务器处于接替工作，则接替的服务器IP

	//zgc [12/21/2006]
	u8      m_byMcuIsConfiged;						//MCU是否被配置过的标识
	//zgc [07/26/2007]
	u8		m_byMcuCfgLevel;						// MCU配置信息的可靠级别
	
	//zgc [07/25/2007]
	//[31…24 23…16 15…8 7…0]
	//bit 2: no mp? ([0]no; [1]yes)
	//bit 3: no mtadp? ([0]no; [1]yes)
	//bit 4: no HDCS module?  ([0]no; [1]yes)
	//bit 5: no MDCS module?  ([0]no; [1]yes)
	//bit 7: no DSC module? ([0]no; [1]yes)
	//bit 31: MCU当前是否正常工作? ([0]no; [1]yes)
	u32		m_dwMcuRunningState;					// MCU运行状态
	

protected:
    u32     m_dwPersistantRunningTime;              // MCU运行时间（单位:s, linux上限:497day，vx上限:828day）
	u16     m_wVcsAccessNum;  //VCS授权数
public:
	// xliang [11/20/2008] 
	u16		m_wAllHdiAccessMtNum;					// HDI授权接入MT的总数量,对于8000E来说，该字段表接入高清能力。
//	u16		m_wStdCriAccessMtNum;					// 标清接入能力（暂不支持，预留）
	u16		m_wAccessPCMtNum;						// 上面预留转为PCMT接入能力 // [3/10/2010 xliang] 	
	
public:
	void SetVcsAccessNum(u16 wVcsAccessNum)
	{
		m_wVcsAccessNum = wVcsAccessNum;
	}
	u16 GetVcsAccessNum()
	{
		return m_wVcsAccessNum;
	}
	u8 GetPeriEqpNum()
	{
		return m_byPeriEqpNum;
	}

	void SetIsExistMp( BOOL32 IsExistMp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mp, IsExistMp) 
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mp);
	}
	void SetIsExistMtadp( BOOL32 IsExistMtadp ) 
	{ 
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_Mtadp, IsExistMtadp)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMtadp(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_Mtadp);
	}
	void SetIsExistDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_DSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_DSC); 
	}
	void SetIsExistMDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_MDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistMDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_MDSC); 
	}
	void SetIsExistHDSC( BOOL32 IsExistDSC ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_HDSC, IsExistDSC)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsExistHDSC(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_HDSC); 
	}
	void SetIsMcuRunOk( BOOL32 IsMcuRunOk ) 
	{
		m_dwMcuRunningState = ntohl(m_dwMcuRunningState);
		SETBITSTATUS(m_dwMcuRunningState, Mask_McuRunOk, IsMcuRunOk)
		m_dwMcuRunningState = htonl(m_dwMcuRunningState);
	}
	BOOL32 IsMcuRunOk(void) const 
	{ 
		u32 dwMcuRunningState = ntohl(m_dwMcuRunningState);
		return GETBITSTATUS( dwMcuRunningState, Mask_McuRunOk);
	}
    void SetPersistantRunningTime(u32 dwTime) { m_dwPersistantRunningTime = htonl(dwTime);    }
    u32  GetPersistantRunningTime(void) const { return ntohl(m_dwPersistantRunningTime);    }

    void Print(void) const
    {
        OspPrintf(TRUE, FALSE, "MCU Current Status: \n" );
        OspPrintf(TRUE, FALSE, "\tIs Run OK:%d\n", IsMcuRunOk() );
        OspPrintf(TRUE, FALSE, "\tExist mp: %d, Exist mtadp: %d, Exist dsc module<DSC.%d, MDSC.%d, HDSC.%d>\n",
				IsExistMp(), IsExistMtadp(), IsExistDSC(), IsExistMDSC(), IsExistHDSC());

        OspPrintf(TRUE, FALSE, "\tConfig file state: ");
        switch(m_byMcuCfgLevel) 
        {
        case MCUCFGINFO_LEVEL_NEWEST:
            OspPrintf(TRUE, FALSE, "Success\n");
            break;
        case MCUCFGINFO_LEVEL_PARTNEWEST:
            OspPrintf(TRUE, FALSE, "Partly success\n");
            break;
        case MCUCFGINFO_LEVEL_LAST:
            OspPrintf(TRUE, FALSE, "Read fail\n");
            break;
        case MCUCFGINFO_LEVEL_DEFAULT:
            OspPrintf(TRUE, FALSE, "No cfg file\n");
            break;
        default:
            OspPrintf(TRUE, FALSE, "level error!\n");
            break;
		}
        OspPrintf(TRUE, FALSE, "\tIs Mcu Configed: %d\n", m_byMcuIsConfiged);

        OspPrintf(TRUE, FALSE, "\tIsReggedGk:%d, Mp Num:%d, H323MtAdp Num:%d, PeirEqpNum:%d, DcsNum:%d\n", 
                  m_byRegedGk, m_byRegedMpNum, m_byRegedMtAdpNum, m_byPeriEqpNum, m_byPeriDcsNum);
        
        OspPrintf(TRUE, FALSE, "\tOngoingConf:%d, ScheduleConf:%d, AllJoinedMt:%d\n", 
                  m_byOngoingConfNum, m_byScheduleConfNum, ntohs(m_wAllJoinedMtNum));

        if ( m_byNPlusState != NPLUS_NONE )
        {
            OspPrintf(TRUE, FALSE, "\tNPlusState:%d, NPlus Replaced Mcu:0x%x\n", 
                m_byNPlusState, ntohl(m_dwNPlusReplacedMcuIp));
        }
        
        if (!m_tMSSynState.IsSynSucceed() )
        {
            OspPrintf(TRUE, FALSE, "\tMS conflict entity ID:%d, Type:%d\n", 
                      m_tMSSynState.GetEntityId(), m_tMSSynState.GetEntityType() );
        }
        {
            u32 dwPersistantTime = GetPersistantRunningTime();

            u32 dwCutOffTime = 0;
            u32 dwDay = dwPersistantTime/(3600*24);
            dwCutOffTime += (3600*24) * dwDay;
            u32 dwHour = (dwPersistantTime - dwCutOffTime)/3600;
            dwCutOffTime += 3600 * dwHour;
            u32 dwMinute = (dwPersistantTime - dwCutOffTime)/60;
            dwCutOffTime += 60 * dwMinute;
            u32 dwSecond = dwPersistantTime - dwCutOffTime;
            if ( 0 == dwDay )
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.h %d.m %d.s\n",
                                         dwHour, dwMinute, dwSecond );                
            }
            else
            {
                OspPrintf(TRUE, FALSE, "\tPersistant running time: %d.day %d.h %d.m %d.s\n",
                                         dwDay, dwHour, dwMinute, dwSecond );                
            }
        }
		// xliang [11/20/2008] HDI 接入终端数量
		OspPrintf(TRUE, FALSE, "\tHDI access Mt Num: %d\n", ntohs(m_wAllHdiAccessMtNum));
		OspPrintf(TRUE, FALSE, "\taccess PCMt Num: %d\n", ntohs(m_wAccessPCMtNum));
    }
};

//TMcuExt继承至TMcu, 记录MCU信息
typedef struct tagMcuExt : public TMcu
{
    tagMcuExt() : m_byCpuUsage(0) {}
	
    /*=============================================================================
    函 数 名:Print
    功    能:在telnet里打印调试信息
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/09/01  4.0     王昊    创建
    =============================================================================*/
    void Print(void);
	
    TMtAlias					   m_tMcuAlias;        //  MCU别名
//    TMcuStatusAfterV4R6B2          m_tMcuStatus;       //  MCU状态
	CMcsMcuStatus                  m_tMcuStatus;       //MCU 状态
    u8							   m_byCpuUsage;       //  CPU使用率(0-100)
} TMcuExt;

typedef map<u16, TTvWallPollParam >	mapTwPollParam;

//CGetName 获取终端厂商名
class CGetName
{
public:
	
    enum
    {
        TYPE_NULL = 0,
			MT_MANUFACTURE,             //  终端的生产厂家
			BAS_CHANNEL_STATUS,         //  适配器通道状态
			BAS_VIDEO_TYPE,             //  适配器通道视频类型
			BAS_AUDIO_TYPE,             //  适配器通道音频类型
    };
	
    /*=============================================================================
    函 数 名:GetName
    功    能:得到某一参数所对应的描述
    参    数:u8 byValue                         [in]    要取的参数值
	u8 byType                          [in]    参数的类型
    注    意:无
    返 回 值:描述字符串
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    static const s8* GetName(u8 byValue, u8 byType);
	
private:
/*=============================================================================
函 数 名:GetMtManu
功    能:得到终端的生产厂家名称
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/26  4.0     王昊    创建
    =============================================================================*/
	static const s8* GetMtManu( u8 byValue );
	
    /*=============================================================================
    函 数 名:GetBasChannelStatus
    功    能:得到适配器通道状态
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	static const s8* GetBasChannelStatus( u8 byValue );
	
    /*=============================================================================
    函 数 名:GetBasVideoType
    功    能:得到适配器通道视频类型
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	static const s8* GetBasVideoType( u8 byValue );
	
    /*=============================================================================
    函 数 名:GetBasAudioType
    功    能:得到适配器通道音频类型
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	static const s8* GetBasAudioType( u8 byValue );
};

//CMtUtility是终端基本操作类
class CMtUtility
{
public:
    // Mt标识字节排列为
    // High |   BYTE   |   BYTE  | Low
    //      |   McuID  |   MtID  |
	
    /*=============================================================================
    函 数 名:GetMcuId
    功    能:根据u16得到McuID
    参    数:u16 wID                            [in]    u16
    注    意:无
    返 回 值:McuID
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    static inline u16 GetMcuId(u32 wID);
	
    /*=============================================================================
    函 数 名:GetMtId
    功    能:根据u16得到MtID
    参    数:u16 wID                            [in]    u16
    注    意:无
    返 回 值:MtID
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    static inline u8 GetMtId(u32 wID);
	
    /*=============================================================================
    函 数 名:GetwID
    功    能:将TMt转化为u16
    参    数:const TMt &tMt                     [in]    TMt
    注    意:无
    返 回 值:终端标识号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    static inline u32 GetwID(const TMt &tMt);
	
    /*=============================================================================
    函 数 名:GetwID
    功    能:将McuID和MtID组合转化为u16
    参    数:u8 byMcuId                         [in]    McuID
	u8 byMtId                          [in]    MtID
    注    意:无
    返 回 值:终端标识号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	static inline u32 GetwID(u16 byMcuId, u8 byMtId);
	
    /*=============================================================================
    函 数 名:IsMcu
    功    能:判断终端是否是MCU
    参    数:const TMt &tMt                     [in]    终端
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    static inline BOOL32 IsMcu(const TMt &tMt);
	
    /*=============================================================================
    函 数 名:IsSMcu
    功    能:判断终端是否是下级MCU
    参    数:const TMt &tMt                     [in]    终端
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    static inline BOOL32 IsSMcu(const TMt &tMt);
	
    /*=============================================================================
    函 数 名:IsMMcu
    功    能:判断终端是否是上级MCU
    参    数:const TMt &tMt                     [in]    终端
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    static inline BOOL32 IsMMcu(const TMt &tMt);
	
    /*=============================================================================
    函 数 名:IsLocalMcu
    功    能:判断终端是否是本级MCU
    参    数:const TMt &tMt                     [in]    终端
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/13  4.0     王昊    创建
    =============================================================================*/
    static inline BOOL32 IsLocalMcu(const TMt &tMt);
	/*=============================================================================
    函 数 名:IsSMt
    功    能:判断终端是否是下级MCU上的终端
    参    数:const TMt &tMt                     [in]    终端
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/05/29  4.0     刘瑞飞   创建
    =============================================================================*/
	//	static inline BOOL32 IsSMt( const TMt &tMt, const TMt &tMMcu );
    /*=============================================================================
    函 数 名:IsMMt
    功    能:判断终端是否是上级MCU上的终端
    参    数:const TMt &tMt                     [in]    终端
			 const TMt &tMMcu					[in]	上级Mcu
			 注    意:无
			 返 回 值:BOOL
			 -------------------------------------------------------------------------------
			 修改纪录:
			 日      期  版本    修改人  修改内容
			 2006/05/29  4.0     刘瑞飞   创建
    =============================================================================*/
	//	static inline BOOL32 IsMMt( const TMt &tMt, const TMt &tMMcu );
	
	/*---------------------------------------------------------------------
	* 函 数 名：
	* 功    能：
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/16	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
};

#pragma pack(push, 1)

#define     MAC_ADDRlENTH   6  //记录MAC地址有6个U8组成
struct TCRIBrdExInfo
{
protected:
	u8				m_byIndex;					    // 索引号
	u16             m_wTotalAuthMtNum;				// 接入总授权数
    u16				m_wAuthHDMtNum;					// 高清接入授权数
    u8              m_byMAC[MAC_ADDRlENTH];						// mac 地址
	u16				m_wAudioMtAccessNum;			// 语音接入点数
public:
	u8 GetIndex(void) { return m_byIndex; }

	u16 GetTotalAuthMtNum(void) { return ntohs(m_wTotalAuthMtNum); }

	u16 GetAuthHDMtNum(void) { return ntohs(m_wAuthHDMtNum); }

	void GetMac(u8 byMac[MAC_ADDRlENTH]){ memcpy(byMac, m_byMAC, sizeof(m_byMAC)); }

	u16 GetAudioMtAccessNum(void) { return ntohs(m_wAudioMtAccessNum); }

	void Clear(void) { ZeroMemory(this, sizeof(TCRIBrdExInfo)); }

    TCRIBrdExInfo(void) 
	{
		memset(this, 0, sizeof(TCRIBrdExInfo));
	}
    BOOL32  IsNull(void)
    {
        TCRIBrdExInfo tInfo;
        return (0 == memcmp(this, &tInfo, sizeof(TCRIBrdExInfo)));
    }
};

#pragma pack(pop)

// 将宏声明改为静态常量声明，这样就可以取到地址了 by ZJL
static const u64 MIXER_MASK				= 0x01;
static const u64 TVWALL_MASK			= 0x02;
static const u64 ADAPTER_MASK			= 0x04;
static const u64 VMP_MASK				= 0x08;
static const u64 PRS_MASK				= 0x10;
static const u64 MULTITVWALL_MASK		= 0x20;
static const u64 REC_MASK				= 0x40;        //  界面专用
static const u64 MDSC_MP_MASK			= 0x80;        //  以下为8000B DSC模块
static const u64 MDSC_MTADP_MASK		= 0x100;
static const u64 MDSC_GK_MASK			= 0x200;
static const u64 MDSC_PROXY_MASK		= 0x400;
static const u64 MDSC_DCS_MASK			= 0x800;
static const u64 HD_BAS_MASK			= 0x1000;
static const u64 MPUSVMP_MASK			= 0x2000;       //  以下为MPU工作模块 xts20081215
static const u64 MPUDVMP_MASK			= 0x4000;
static const u64 MPUBAS_MASK			= 0x8000;
static const u64 MPUEVPU_MASK			= 0x10000;
static const u64 MPUEBAP_MASK			= 0x20000;
static const u64 TVSHDU_MASK			= 0x40000;
static const u64 TVSHDU2_MASK			= 0x80000;
static const u64 TVSHDU2_L_MASK			= 0x100000;
static const u64 TVSHDU2_S_MASK			= 0x200000;
static const u64 MIXER_MASK1			= 0x400000;
static const u64 MIXER_MASK2			= 0x800000;
static const u64 MIXER_MASK3			= 0x1000000;
static const u64 TVSHDU1_MASK			= 0x2000000;		//HDU-1
static const u64 TVSHDUL_MASK			= 0x4000000;		//HUD-L
static const u64 MPUBAS2_MASK			= 0x8000000;		//MPU（BAP-2）模式
	//以下为MPU2工作模式 wyh20111219
static const u64 MPU2BASICVMP_MASK		= 0x10000000;		//MPU2-VMP
static const u64 MPU2BASICBAP_MASK		= 0x20000000;		//MPU2-BAP
static const u64 MPU2ECARDVMP_MASK		= 0x40000000;		//MPU2(Ecard)-VMP(Enhanced)
static const u64 MPU2ECARDVMP2_MASK		= 0x80000000;		//MPU2(Ecard)-VMP(Basic)*2
static const u64 MPU2ECARDBAP_MASK		= 0x100000000;		//MPU2(Ecard)-BAP(Enhanced)
static const u64 MPU2ECARDBAP2_MASK		= 0x200000000;		//MPU2(Ecard)-BAP(Basic)*2
static const u64 MPU2ECARDDVMP1_MASK	= 0x400000000;
static const u64 MPU2ECARDDBAP1_MASK	= 0x800000000;
static const u64 APU2BAS_MASK			= 0x1000000000;		//APU2(bas)

//CBrdCfg继承自TBrdCfgInfo, 记录单板及其配置外设
class CBrdCfg : public TBrdCfgInfo
{
public:
    CBrdCfg() : m_byEqpExist(0) { ZeroMemory( this, sizeof(CBrdCfg) ); }
    ~CBrdCfg() {}
	
public:
/*=============================================================================
函 数 名:operator TBrdCfgInfo
功    能:类型强转
参    数:无
注    意:无
返 回 值:TBrdCfgInfo
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/09/01  4.0     王昊    创建
    =============================================================================*/
    inline operator TBrdCfgInfo(void) { return dynamic_cast<TBrdCfgInfo&>(*this); }
	
    /*=============================================================================
    函 数 名:operator=
    功    能:赋值操作符
    参    数:无
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/09/01  4.0     王昊    创建
    =============================================================================*/
    inline CBrdCfg& operator=(const TBrdCfgInfo &tBrdCfg);
	
    /*=============================================================================
    函 数 名:HasMixer
    功    能:该单板是否含有混音器
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    BOOL32 HasMixer(void) const { return ( ( m_byEqpExist & MIXER_MASK ) > 0); }
	
	BOOL32 HasMixer1(void) const { return ( ( m_byEqpExist & MIXER_MASK1 ) > 0); }
	
	BOOL32 HasMixer2(void) const { return ( ( m_byEqpExist & MIXER_MASK2 ) > 0); }
	
	BOOL32 HasMixer3(void) const { return ( ( m_byEqpExist & MIXER_MASK3 ) > 0); }

	// 该单板是否含有音频适配器
	BOOL32 HasMixBas(void) const { return ( ( m_byEqpExist & APU2BAS_MASK ) > 0); }
    /*=============================================================================
    函 数 名:HasTvWall
    功    能:该单板是否含有电视墙
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    BOOL32 HasTvWall(void) const { return ( ( m_byEqpExist & TVWALL_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasAdapter
    功    能:该单板是否含有码流适配器
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    BOOL32 HasAdapter(void) const { return ( ( m_byEqpExist & ADAPTER_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasVmp
    功    能:该单板是否含有画面合成器
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    BOOL32 HasVmp(void) const { return ( ( m_byEqpExist & VMP_MASK ) > 0); }
	//xts20081216
    /*=============================================================================
    函 数 名:HasMpuSvmp
    功    能:该单板是否含有单vmp模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    BOOL32 HasMpuSvmp(void) const { return ( ( m_byEqpExist & MPUSVMP_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasMpuDvmp
    功    能:该单板是否含有双vmp模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    BOOL32 HasMpuDvmp(void) const { return ( ( m_byEqpExist & MPUDVMP_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasMpuBas
    功    能:该单板是否含有BAS模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    BOOL32 HasMpuBas(void) const { return ( ( m_byEqpExist & MPUBAS_MASK ) > 0); }
	
    BOOL32 HasMpuBas2(void) const { return ( ( m_byEqpExist & MPUBAS2_MASK ) > 0); }
    /*=============================================================================
    函 数 名:HasMpuEvpu
    功    能:该单板是否含有EVPU模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    BOOL32 HasMpuEvpu(void) const { return ( ( m_byEqpExist & MPUEVPU_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasMpuEbap
    功    能:该单板是否含有EBAP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    BOOL32 HasMpuEbap(void) const { return ( ( m_byEqpExist & MPUEBAP_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasMpuHdu
    功    能:该单板是否含有HDU模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    BOOL32 HasMpuHdu(void) const { return ( ( m_byEqpExist & TVSHDU_MASK ) > 0); }
	
	BOOL32 HasMpuHdu1(void) const { return ( ( m_byEqpExist & TVSHDU1_MASK ) > 0); }
	
	BOOL32 HasMpuHduL(void) const { return ( ( m_byEqpExist & TVSHDUL_MASK ) > 0); }

	BOOL32 HasMpuHdu2(void) const { return ( ( m_byEqpExist & TVSHDU2_MASK ) > 0); }
	
	BOOL32 HasMpuHdu2L(void) const { return ( ( m_byEqpExist & TVSHDU2_L_MASK ) > 0); }

	BOOL32 HasMpuHdu2S(void) const { return ( ( m_byEqpExist & TVSHDU2_S_MASK ) > 0); }
	
	//wyh20111219
    /*=============================================================================
    函 数 名:HasMpu2Vmp
    功    能:该单板是否含有VMP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	BOOL32 HasMpu2Vmp(void) const { return ( ( m_byEqpExist & MPU2BASICVMP_MASK ) > 0); }
	
	/*=============================================================================
    函 数 名:HasMpu2Bap
    功    能:该单板是否含有BAP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	BOOL32 HasMpu2Bap(void) const { return ( ( m_byEqpExist & MPU2BASICBAP_MASK ) > 0); }
	
	/*=============================================================================
    函 数 名:HasMpu2EcardVmp
    功    能:该单板是否含有VMP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	BOOL32 HasMpu2EcardVmp(void) const { return ( ( m_byEqpExist & MPU2ECARDVMP_MASK ) > 0); }
	
	/*=============================================================================
    函 数 名:HasMpu2EcardDVmp
    功    能:该单板是否含有双VMP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	BOOL32 HasMpu2EcardDVmp(void) const { return ( ( m_byEqpExist & MPU2ECARDVMP2_MASK ) > 0); }
	BOOL32 HasMpu2EcardDVmp1(void) const { return ( ( m_byEqpExist & MPU2ECARDDVMP1_MASK ) > 0); }
	
	/*=============================================================================
    函 数 名:HasMpu2EcardBap
    功    能:该单板是否含有BAP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	BOOL32 HasMpu2EcardBap(void) const { return ( ( m_byEqpExist & MPU2ECARDBAP_MASK ) > 0); }
	
	/*=============================================================================
    函 数 名:HasMpu2EcardDVmp
    功    能:该单板是否含有双BAP模式
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	BOOL32 HasMpu2EcardDBap(void) const { return ( ( m_byEqpExist & MPU2ECARDBAP2_MASK ) > 0); }
	BOOL32 HasMpu2EcardDBap1(void) const { return ( ( m_byEqpExist & MPU2ECARDDBAP1_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasPrs
    功    能:该单板是否含有丢包重传器
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    BOOL32 HasPrs(void) const { return ( ( m_byEqpExist & PRS_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:HasMultiTvWall
    功    能:该单板是否含有多画面电视墙
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/10/19  4.0     王昊    创建
    =============================================================================*/
    BOOL32 HasMultiTvWall(void) const
    { return ( ( m_byEqpExist & MULTITVWALL_MASK ) > 0); }
	
    /*=============================================================================
    函 数 名:SetMixer
    功    能:设置混音器配置
    参    数:TEqpMixerCfgInfo &tMixerCfg        [in]    混音器配置
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    void SetMixer( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK; m_tMixerCfg = tMixerCfg; }
	
    void SetMixer1( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK1; m_tMixerCfg1 = tMixerCfg; }
	void SetMixer2( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK2; m_tMixerCfg2 = tMixerCfg; }
	void SetMixer3( TEqpMixerCfgInfo &tMixerCfg )
    { m_byEqpExist |= MIXER_MASK3; m_tMixerCfg3 = tMixerCfg; }
	// 设置音频适配器
	void SetMixBas( TEqpMpuBasCfgInfo &tMixerBasCfg )
	{ m_byEqpExist |= APU2BAS_MASK; m_tMixerBasCfg = tMixerBasCfg; }
	
    /*=============================================================================
    函 数 名:SetTvWall
    功    能:设置电视墙配置
    参    数:TEqpTvWallCfgInfo &tTvWallCfg      [in]    电视墙配置
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    void SetTvWall( TEqpTvWallCfgInfo &tTvWallCfg )
    { m_byEqpExist |= TVWALL_MASK; m_tTvWallCfg = tTvWallCfg; }
	
    /*=============================================================================
    函 数 名:SetAdapter
    功    能:设置码流适配器配置
    参    数:TEqpBasCfgInfo &tAdapterCfg        [in]    码流适配器配置
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    void SetAdapter( TEqpBasCfgInfo &tAdapterCfg )
    { m_byEqpExist |= ADAPTER_MASK; m_tAdapterCfg = tAdapterCfg; }
	
    /*=============================================================================
    函 数 名:SetVmp
    功    能:设置画面合成器配置
    参    数:TEqpVmpCfgInfo &tVmpCfg            [in]    画面合成器配置
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    void SetVmp( TEqpVmpCfgInfo &tVmpCfg )
    { m_byEqpExist |= VMP_MASK; m_tVmpCfg = tVmpCfg; }
	
	//xts20081216
    /*=============================================================================
    函 数 名:SetSvmp
    功    能:设置单vmp模式
    参    数:TEqpSvmpCfgInfo &tVmpCfg            [in]    单vmp模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    void SetSvmp( TEqpSvmpCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUSVMP_MASK; m_tSvmpCfg = tVmpCfg; }
    /*=============================================================================
    函 数 名:SetDvmp
    功    能:设置双vmp模式
    参    数:TEqpDvmpCfgInfo &tVmpCfg            [in]    双vmp模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    void SetDvmp( TEqpDvmpCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUDVMP_MASK; m_tDvmpCfg = tVmpCfg; }
    /*=============================================================================
    函 数 名:SetMpuBas
    功    能:设置MPUBAS模式
    参    数:TEqpMpuBasCfgInfo &tVmpCfg            [in]    MPUBAS模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    void SetMpuBas( TEqpMpuBasCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUBAS_MASK; m_tMpuBasCfg = tVmpCfg; }
    void SetMpuBas2( TEqpMpuBasCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUBAS2_MASK; m_tMpuBasCfg = tVmpCfg; }
	
	
    /*=============================================================================
    函 数 名:SetEvpu
    功    能:设置EVPU模式
    参    数:TEqpEvpuCfgInfo &tVmpCfg            [in]    EVPU模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    void SetEvpu( TEqpEvpuCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUEVPU_MASK; m_tEvpuCfg = tVmpCfg; }
    /*=============================================================================
    函 数 名:SetEbap
    功    能:设置EBAP模式
    参    数:TEqpEbapCfgInfo &tVmpCfg            [in]    EBAP模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    void SetEbap( TEqpEbapCfgInfo &tVmpCfg )
    { m_byEqpExist |= MPUEBAP_MASK; m_tEbapCfg = tVmpCfg; }
	
    /*=============================================================================
    函 数 名:SetEbap
    功    能:设置EBAP模式
    参    数:TEqpEbapCfgInfo &tVmpCfg            [in]    EBAP模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    void SetHdu( TEqpHduCfgInfo &tVmpCfg )
    { m_byEqpExist |= TVSHDU_MASK; m_tHduCfg = tVmpCfg; }
	
    void SetHdu1( TEqpHduCfgInfo &tVmpCfg )
    { m_byEqpExist |= TVSHDU1_MASK; m_tHduCfg = tVmpCfg; }
	
    void SetHduL( TEqpHduCfgInfo &tVmpCfg )
    { m_byEqpExist |= TVSHDUL_MASK; m_tHduCfg = tVmpCfg; }
	
	void SetHdu2( TEqpHduCfgInfo &tVmpCfg )
	{ m_byEqpExist |= TVSHDU2_MASK; m_tHduCfg = tVmpCfg; }
	
	void SetHdu2L( TEqpHduCfgInfo &tVmpCfg )
	{ m_byEqpExist |= TVSHDU2_L_MASK; m_tHduCfg = tVmpCfg; }

	void SetHdu2S( TEqpHduCfgInfo &tVmpCfg )
	{ m_byEqpExist |= TVSHDU2_S_MASK; m_tHduCfg = tVmpCfg; }

	//wyh20111219
	/*=============================================================================
    函 数 名:SetMpu2Vmp
    功    能:设置VMP-BASIC模式
    参    数:TEqpSvmpCfgInfo &tVmpCfg            [in]    VMP-BASIC模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	void SetMpu2Vmp( TEqpSvmpCfgInfo &tVmpCfg )
	{ 
		m_byEqpExist |= MPU2BASICVMP_MASK; 
		m_tSvmpCfg = tVmpCfg; 
	}
	
	/*=============================================================================
    函 数 名:SetMpu2Bap
    功    能:设置BAP-BASIC模式
    参    数:TEqpMpuBasCfgInfo &tVmpCfg            [in]    BAP-BASIC模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	void SetMpu2Bap( TEqpMpuBasCfgInfo &tVmpCfg )
	{
		m_byEqpExist |= MPU2BASICBAP_MASK;
		m_tMpuBasCfg = tVmpCfg;
	}
	
	/*=============================================================================
    函 数 名:SetMpu2EcardVmp
    功    能:设置VMP-ENHANCED模式
    参    数:TEqpSvmpCfgInfo &tVmpCfg            [in]    VMP-ENHANCED模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	void SetMpu2EcardVmp( TEqpSvmpCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDVMP_MASK;
		m_tSvmpCfg3 = tVmpCfg;
	}
	
	/*=============================================================================
    函 数 名:SetMpu2EcardDVmp
    功    能:设置VMP-BASIC*2模式
    参    数:TEqpSvmpCfgInfo &tVmpCfg            [in]    VMP-BASIC*2模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	void SetMpu2EcardDVmp( TEqpSvmpCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDVMP2_MASK;
		m_tSvmpCfg1 = tVmpCfg;
	}
	void SetMpu2EcardDVmp1( TEqpSvmpCfgInfo &tVmpCfg )
	{
		m_byEqpExist |= MPU2ECARDDVMP1_MASK;
		m_tSvmpCfg2 = tVmpCfg;
	}
	
	/*=============================================================================
    函 数 名:SetMpu2EcardBap
    功    能:设置BAP-ENHANCED模式
    参    数:TEqpMpuBasCfgInfo &tVmpCfg            [in]    BAP-ENHANCED模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	void SetMpu2EcardBap( TEqpMpuBasCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDBAP_MASK;
		m_tMpuBasCfg3 = tVmpCfg;
	}
	
	/*=============================================================================
    函 数 名:SetMpu2EcardDBap
    功    能:设置BAP-BASIC*2模式
    参    数:TEqpMpuBasCfgInfo &tVmpCfg            [in]    BAP-BASIC*2模式
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	void SetMpu2EcardDBap( TEqpMpuBasCfgInfo &tVmpCfg)
	{
		m_byEqpExist |= MPU2ECARDBAP2_MASK;
		m_tMpuBasCfg1 = tVmpCfg;
	}
	void SetMpu2EcardDBap1( TEqpMpuBasCfgInfo &tVmpCfg )
	{
		m_byEqpExist |= MPU2ECARDDBAP1_MASK;
		m_tMpuBasCfg2 = tVmpCfg;
	}
	
    /*=============================================================================
    函 数 名:SetPrs
    功    能:设置丢包重传器配置
    参    数:TPrsCfgInfo &tPrsCfg               [in]    丢包重传器配置
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    void SetPrs( TPrsCfgInfo &tPrsCfg )
    { m_byEqpExist |= PRS_MASK; m_tPrsCfg = tPrsCfg; }
	
    /*=============================================================================
    函 数 名:SetMultiTvWall
    功    能:设置多画面电视墙配置
    参    数:TEqpMTvwallCfgInfo &tMultiTvWallCfg    [in]    丢包重传器配置
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/10/19  4.0     王昊    创建
    =============================================================================*/
    void SetMultiTvWall(TEqpMTvwallCfgInfo &tMultiTvWallCfg)
    { m_byEqpExist |= MULTITVWALL_MASK; m_tMultiTvWallCfg = tMultiTvWallCfg; }
	
    /*=============================================================================
    函 数 名:GetMixer
    功    能:获取混音器配置
    参    数:无
    注    意:必须存在混音器
    返 回 值:混音器配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    TEqpMixerCfgInfo& GetMixer(void) { return m_tMixerCfg; }
	TEqpMixerCfgInfo& GetMixer1(void) { return m_tMixerCfg1; }
	TEqpMixerCfgInfo& GetMixer2(void) { return m_tMixerCfg2; }
	TEqpMixerCfgInfo& GetMixer3(void) { return m_tMixerCfg3; }

	// 获取音频适配器配置
	TEqpMpuBasCfgInfo& GetMixerBas(void) { return m_tMixerBasCfg; }
	
    /*=============================================================================
    函 数 名:GetTvWall
    功    能:获取电视墙配置
    参    数:无
    注    意:必须存在电视墙
    返 回 值:电视墙配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    TEqpTvWallCfgInfo& GetTvWall(void) { return m_tTvWallCfg; }
	
    /*=============================================================================
    函 数 名:GetAdapter
    功    能:获取码流适配器配置
    参    数:无
    注    意:必须存在码流适配器
    返 回 值:码流适配器配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    TEqpBasCfgInfo& GetAdapter(void) { return m_tAdapterCfg; }
	
    /*=============================================================================
    函 数 名:GetVmp
    功    能:获取画面合成器配置
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:画面合成器配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    TEqpVmpCfgInfo& GetVmp(void) { return m_tVmpCfg; }
	
	//xts20081216
    /*=============================================================================
    函 数 名:GetSvmp
    功    能:获取单vmp模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:单vmp模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    TEqpSvmpCfgInfo& GetSvmp(void) { return m_tSvmpCfg; }
	
    /*=============================================================================
    函 数 名:GetDvmp
    功    能:获取双vmp模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:双vmp模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    TEqpDvmpCfgInfo& GetDvmp(void) { return m_tDvmpCfg; }
	
    /*=============================================================================
    函 数 名:GetMpuBas
    功    能:获取MPUBAS模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:MPUBAS模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    TEqpMpuBasCfgInfo& GetMpuBas(void) { return m_tMpuBasCfg; }
	
    TEqpMpuBasCfgInfo& GetMpuBas2(void) { return m_tMpuBasCfg; }
    /*=============================================================================
    函 数 名:GetEvpu
    功    能:获取EVPU模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:EVPU模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    TEqpEvpuCfgInfo& GetEvpu(void) { return m_tEvpuCfg; }
	
    /*=============================================================================
    函 数 名:GetEbap
    功    能:获取EBAP模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:EBAP模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    TEqpEbapCfgInfo& GetEbap(void) { return m_tEbapCfg; }
	
    /*=============================================================================
    函 数 名:GetHdu
    功    能:获取HDU模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:HDU模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2008/12/16  4.0     徐太松    创建
    =============================================================================*/
    TEqpHduCfgInfo& GetHdu(void) { return m_tHduCfg; }
    TEqpHduCfgInfo& GetHdu1(void) { return m_tHduCfg; }
	TEqpHduCfgInfo& GetHduL(void) { return m_tHduCfg; }
	//wyh20111219
	/*=============================================================================
    函 数 名:GetMpu2Vmp
    功    能:获取VMP-BASIC模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:VMP-BASIC模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	TEqpSvmpCfgInfo& GetMpu2Vmp(void) { return m_tSvmpCfg; }
	
	/*=============================================================================
    函 数 名:GetMpu2EcardVmp
    功    能:获取VMP-ENHANCED模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:VMP-ENHANCED模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	TEqpSvmpCfgInfo& GetMpu2EcardVmp(void) { return m_tSvmpCfg3; }
	
	/*=============================================================================
    函 数 名:GetMpu2EcardVmp
    功    能:获取VMP-BASIC*2模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:VMP-BASIC*2模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	TEqpSvmpCfgInfo& GetMpu2EcardDVmp(void) { return m_tSvmpCfg1; }
	TEqpSvmpCfgInfo& GetMpu2EcardDVmp1(void) { return m_tSvmpCfg2; }
	
	/*=============================================================================
    函 数 名:GetMpu2Bap
    功    能:获取BAP-BASIC模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:BAP-BASIC模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	TEqpMpuBasCfgInfo& GetMpu2Bap(void) { return m_tMpuBasCfg; }
	
	/*=============================================================================
    函 数 名:GetMpu2EcardBap
    功    能:获取BAP-ENHANCED模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:BAP-ENHANCED模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	TEqpMpuBasCfgInfo& GetMpu2EcardBap(void) { return m_tMpuBasCfg3; }
	
	/*=============================================================================
    函 数 名:GetMpu2EcardDBap
    功    能:获取BAP-BASIC*2模式
    参    数:无
    注    意:必须存在画面合成器
    返 回 值:BAP-BASIC*2模式配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/12/19  4.7     王云慧    创建
    =============================================================================*/
	TEqpMpuBasCfgInfo& GetMpu2EcardDBap(void) { return m_tMpuBasCfg1; }
	TEqpMpuBasCfgInfo& GetMpu2EcardDBap1(void) { return m_tMpuBasCfg2; }
	
    /*=============================================================================
    函 数 名:GetPrs
    功    能:获取丢包重传器配置
    参    数:无
    注    意:必须存在丢包重传器
    返 回 值:丢包重传器配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/31  4.0     王昊    创建
    =============================================================================*/
    TPrsCfgInfo& GetPrs(void) { return m_tPrsCfg; }
	
    /*=============================================================================
    函 数 名:GetMultiTvWall
    功    能:获取多画面电视墙配置
    参    数:无
    注    意:必须存在多画面电视墙
    返 回 值:多画面电视墙配置
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/10/19  4.0     王昊    创建
    =============================================================================*/
    TEqpMTvwallCfgInfo& GetMultiTvWall(void) { return m_tMultiTvWallCfg; }
	
    /*=============================================================================
    函 数 名:ClearPeriCfg
    功    能:清空一种或多种外设配置
    参    数:u8 byPeriUnion                     [in]    MIXER_MASK | TVWALL_MASK
	| ADAPTER_MASK
	| VMP_MASK | PRS_MASK
	| MULTITVWALL_MASK
    注    意:参数由各种mask拼出来
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/09/20  4.0     王昊    创建
    =============================================================================*/
    void ClearPeriCfg( u64 byPeriUnion , u8 byBrdType = 0);
	
    /*=============================================================================
    函 数 名:Print
    功    能:在telnet里打印调试信息
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/09/01  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const;
	
private:
    TEqpMixerCfgInfo    m_tMixerCfg;        //  混音器配置
	TEqpMixerCfgInfo    m_tMixerCfg1;        //  混音器配置
	TEqpMixerCfgInfo    m_tMixerCfg2;        //  混音器配置
	TEqpMixerCfgInfo    m_tMixerCfg3;        //  混音器配置
	TEqpMpuBasCfgInfo	m_tMixerBasCfg;		//	音频适配器配置
    TEqpTvWallCfgInfo   m_tTvWallCfg;       //  电视墙配置
    TEqpBasCfgInfo      m_tAdapterCfg;      //  码流适配器配置
    TEqpVmpCfgInfo      m_tVmpCfg;          //  画面合成器配置
    TPrsCfgInfo         m_tPrsCfg;          //  丢包重传器配置
    TEqpMTvwallCfgInfo  m_tMultiTvWallCfg;  //  多画面电视墙
    //xts
    TEqpSvmpCfgInfo     m_tSvmpCfg;         //单vmp模式
    TEqpDvmpCfgInfo     m_tDvmpCfg;         //双vmp模式
    TEqpMpuBasCfgInfo   m_tMpuBasCfg;       //BAS模式
    TEqpEbapCfgInfo     m_tEbapCfg;         //EBAP模式
    TEqpEvpuCfgInfo     m_tEvpuCfg;         //EVPU模式
    TEqpHduCfgInfo      m_tHduCfg;         //HDU模式	
	//wyh20111226
	TEqpSvmpCfgInfo		m_tSvmpCfg1;		//VMP-BASIC*2模式
	TEqpSvmpCfgInfo		m_tSvmpCfg2;
	TEqpSvmpCfgInfo		m_tSvmpCfg3;		//VMP-ENHANCED
	TEqpMpuBasCfgInfo	m_tMpuBasCfg1;		//BAP-BASIC*2模式
	TEqpMpuBasCfgInfo   m_tMpuBasCfg2;
	TEqpMpuBasCfgInfo	m_tMpuBasCfg3;		//BAP-ENHANCED
	
    u64                  m_byEqpExist;       //  该单板上有哪些外设
};

//CMt继承至TMt，存储单个Mt的所有信息
class CMt : public TMt
{
public:
    CMt() { 
		SetNull();
	}
    CMt(const CMt &cMt);
    CMt(const TMtExtCur &tMtExt);
    CMt(const TMtStatus &tMtStatus);
	CMt(const TMt &tMt);
    ~CMt() { }
	
    /*=============================================================================
    函 数 名:SetNull
    功    能:将终端置空
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline void SetNull(void)
	{
		TMt::SetNull();
		m_tMtExt.SetNull();
		m_tMtStatus.SetNull();
		m_tMtAlias.SetNull();
		m_tMtVersion.SetNull();
		m_dwUseStatus = 0;
		m_dwOnlineStatus = 0;
		m_tRecEqp.SetNull();
		ZeroMemory( &m_tRecProg, sizeof(m_tRecProg) );
		m_dwLastRateReqTime = 0;
		m_byUsed = 0;
		m_tMediaSrcMt.SetNull();
		m_byLockStatus = 0;
		m_tMixParam.Clear();
		m_dwEqpStatus = 0;
		m_tMcsMtVideoSrcAlias.SetNull();
	}
	
public:
/*=============================================================================
函 数 名:operator=
功    能:全部赋值
参    数:const CMt& cMt                     [in]    全部终端信息
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline CMt& operator=(const CMt &cMt);
	
    /*=============================================================================
    函 数 名:operator=
    功    能:基本结构赋值
    参    数:const TMt& tMt                     [in]    基本终端信息
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline CMt& operator=(const TMt &tMt);
	
    /*=============================================================================
    函 数 名:operator=
    功    能:部分结构赋值
    参    数:const TMtExt &tMtExt               [in]    部分终端信息
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline CMt& operator=(const TMtExtCur &tMtExt);
	
    /*=============================================================================
    函 数 名:operator=
    功    能:部分结构赋值
    参    数:const TMtStatus &tMtStatus         [in]    部分终端信息
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/14  4.0     王昊    创建
    =============================================================================*/
    inline CMt& operator=(const TMtStatus &tMtStatus);
	
    /*=============================================================================
    函 数 名:operator TMt
    功    能:转化为TMt
    参    数:无
    注    意:无
    返 回 值:const TMt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline operator const TMt(void) const { return (*this); }
	
    /*=============================================================================
    函 数 名:operator TMt
    功    能:转化为TMt
    参    数:无
    注    意:无
    返 回 值:TMt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline operator TMt(void) { return (dynamic_cast<TMt&>(*this)); }
	
    /*=============================================================================
    函 数 名:operator TMtExt
    功    能:转化为TMtExt
    参    数:无
    注    意:无
    返 回 值:const TMtExt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline operator const TMtExtCur(void) const { return m_tMtExt; }
	
    /*=============================================================================
    函 数 名:operator TMtExt
    功    能:转化为TMtExt
    参    数:无
    注    意:无
    返 回 值:TMtExt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline operator TMtExtCur(void) { return m_tMtExt; }
	
    /*=============================================================================
    函 数 名:operator TMtStatus
    功    能:转化为TMtStatus
    参    数:无
    注    意:无
    返 回 值:const TMtStatus
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline operator const TMtStatus(void) const { return m_tMtStatus; }
	
    /*=============================================================================
    函 数 名:operator TMtStatus
    功    能:转化为TMtStatus
    参    数:无
    注    意:无
    返 回 值:TMtStatus
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline operator TMtStatus(void) { return m_tMtStatus; }
	
    /*=============================================================================
    函 数 名:operator==
    功    能:比较操作符
    参    数:const TMtStatus &tMtStatus         [in]    TMtStatus
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/27  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 operator==(const TMtStatus &tMtStatus) const;
	
    /*=============================================================================
    函 数 名:operator==
    功    能:比较操作符
    参    数:const TMtExt &tMtExt               [in]    TMtExt
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/08  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 operator==(const TMtExtCur &tMtExt) const;
	
    /*=============================================================================
    函 数 名:operator==
    功    能:比较操作符
    参    数:const TMt &tMt                     [in]    TMt
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/09  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 operator==(const TMt &tMt) const;
	
public:
/*=============================================================================
函 数 名:IsNull
功    能:判断终端是否为空
参    数:无
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/05/09  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 IsNull(void) const;
	
    /*=============================================================================
    函 数 名:GetwID
    功    能:获取16位终端标识符
    参    数:无
    注    意:无
    返 回 值:终端标识符
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline u32 GetwID(void) const;
	
    /*=============================================================================
    函 数 名:SetMt
    功    能:设置TMt
    参    数:const TMt &tMt                     [in]    TMt
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    inline void SetMt(const TMt &tMt);
	
	/*=============================================================================
    函 数 名:SetMt
    功    能:设置TMt
    参    数:                    [in]    tMtExt
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	
    /*=============================================================================
    函 数 名:SetMtStatus
    功    能:设置终端状态
    参    数:const TMtStatus &tMtStatus         [in]    终端状态
    注    意:无
    返 回 值:现有m_tMtStatus与tMtStatus不一样:TRUE; 否则:FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    inline BOOL32 SetMtStatus(const TMtStatus &tMtStatus);
	
    /*=============================================================================
    函 数 名:SyncAliasFromExt
    功    能:从扩展结构同步终端别名
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/12/14  4.0     王昊    创建
    =============================================================================*/
    void SyncAliasFromExt(void);
	
    /*=============================================================================
    函 数 名:ClearMtAlias
    功    能:清空终端别名
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/12/14  4.0     王昊    创建
    =============================================================================*/
    inline void ClearMtAlias(void) { m_tMtAlias.SetNull(); }
	
    /*=============================================================================
    函 数 名:SetMtAlias
    功    能:设置终端别名
    参    数:const TMtAlias &tMtAlias           [in]    终端别名
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    2006/01/09  4.0     顾振华  修改
    =============================================================================*/
	void SetMtAlias(const TMtAlias &tMtAlias);
	
    /*=============================================================================
    函 数 名:SetRecEqp
    功    能:设置录像设备
    参    数:const TEqp &tRecEqp                [in]    录像设备
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SetRecEqp(const TEqp &tRecEqp);
	
    /*=============================================================================
    函 数 名:SetRecProg
    功    能:设置录像进度
    参    数:const TRecProg &tRecProg           [in]    录像进度
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SetRecProg(const TRecProg &tRecProg);
	
    /*=============================================================================
    函 数 名:GetMtAlias
    功    能:获取终端别名
    参    数:const mtAliasType &emType          [in]    别名类型
    注    意:没有该类型则返回空值
    返 回 值:TMtAlias
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    2006/01/09  4.0     顾振华  修改
    =============================================================================*/
    TMtAlias GetMtAlias(const mtAliasType &emType) const;
	
    /*=============================================================================
    函 数 名:GetMtAlias
    功    能:获取终端别名
    参    数:无
    注    意:H.323 ID / H.320 ID > E.164 ID > IP
    返 回 值:TMtAlias
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    2006/01/09  4.0     顾振华  修改
    =============================================================================*/
    TMtAlias GetMtAlias(void) const;
	
    /*=============================================================================
    函 数 名:GetMtAliasFromExt
    功    能:从扩展结构里得获取终端别名
    参    数:TMtAlias &tMtAlias                 [in]    要得的终端别名
    注    意:
    返 回 值:得到字串值返回TRUE，字串为空返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/12  4.0     王昊    创建
    =============================================================================*/
    BOOL32 GetMtAliasFromExt(TMtAlias &tMtAlias) const;
	
    /*=============================================================================
    函 数 名:GetProtocol
    功    能:获取终端协议类型
    参    数:无
    注    意:
    返 回 值:u8 协议类型 PROTOCOL_TYPE_H323 PROTOCOL_TYPE_H320 PROTOCOL_TYPE_SIP
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/01/20  4.0     顾振华  修改
    =============================================================================*/
    inline u8 GetProtocol(void) const
    {
        return m_tMtExt.GetProtocolType();
    }
	
    /*=============================================================================
    函 数 名:GetProtocol
    功    能:终端是否是H320终端
    参    数:无
    注    意:
    返 回 值:TRUE 是 H320
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/01/20  4.0     顾振华  修改
    =============================================================================*/
    inline BOOL32 IsProtocolH320() const
    {
        return (GetProtocol() == PROTOCOL_TYPE_H320);
    }
	
	
    /*=============================================================================
    函 数 名:IsOnline
    功    能:终端是否在线
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsOnline(void) const;
	
    /*=============================================================================
    函 数 名:SetOnline
    功    能:设置是否在线
    参    数:BOOL32 bOnLine                     [in]    是否在线
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	void SetOnline(BOOL32 bOnLine);
	
    /*========================================================================
	函 数 名:IsShown
	功    能:判断呼叫错误信息是否显示过
	参    数:无
	注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
	========================================================================*/
	BOOL32 IsShown(void) const;
	
    /*=============================================================================
    函 数 名:SetShown
    功    能:设置该呼叫错误信息是否显示过
    参    数:BOOL32 bShown                      [in]	是否显示过
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    void SetShown(BOOL32 bShown);
	
    /*=============================================================================
    函 数 名:GetErrorCode
    功    能:获取错误信息码
    参    数:无
    注    意:无
    返 回 值:错误信息码
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    u16 GetErrorCode(void) const { return HIWORD(m_dwOnlineStatus); }
	
    /*=============================================================================
    函 数 名:SetErrorCode
    功    能:设置错误信息码
    参    数:u16 wErrorCode                     [in]    错误信息码
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    void SetErrorCode(u16 wErrorCode);
	
    /*=============================================================================
    函 数 名:GetRecEqp
    功    能:获取录像设备
    参    数:无
    注    意:无
    返 回 值:TEqp
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    TEqp GetRecEqp(void) const { return m_tRecEqp; }
	
    /*=============================================================================
    函 数 名:GetRecProg
    功    能:获取录像进度
    参    数:无
    注    意:无
    返 回 值:TRecProg
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    TRecProg GetRecProg(void) const { return m_tRecProg; }
	
    /*=============================================================================
    函 数 名:GetMtStatus
    功    能:获取终端状态
    参    数:无
    注    意:无
    返 回 值:TMtStatus
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    TMtStatus GetMtStatus(void) const { return m_tMtStatus; }
	
    /*=============================================================================
    函 数 名:GetMtExt
    功    能:获取终端扩展结构
    参    数:无
    注    意:无
    返 回 值:TMtExt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/30  4.0     王昊    创建
    =============================================================================*/
    TMtExtCur GetMtExt(void) const { return m_tMtExt; }
	
    /*=============================================================================
    函 数 名:IsStatusDumb
    功    能:判断终端是否哑音
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusDumb(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusDumb
    功    能:设置终端是否哑音
    参    数:BOOL32 bDumb                       [in]    是否哑音
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	//    void SetStatusMute(BOOL32 bDumb);
	
    /*=============================================================================
    函 数 名:IsStatusQuiet
    功    能:判断终端是否静音
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusQuiet(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusQuiet
    功    能:设置终端是否静音
    参    数:BOOL32 bQuiet                      [in]    是否静音
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	//    void SetStatusQuiet(BOOL32 bQuiet);
	
    /*=============================================================================
    函 数 名:IsStatusInspect
    功    能:判断终端是否选看
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusInspect(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusInspect
    功    能:设置终端是否选看
    参    数:BOOL32 bInspect                    [in]    是否选看
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	//    void SetStatusInspect(BOOL32 bInspect);
	
    /*=============================================================================
    函 数 名:IsStatusVmp
    功    能:判断终端是否画面合成
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusVmp(void) const;
	
	
    /*=============================================================================
    函 数 名:IsStatusRec
    功    能:判断终端是否录像
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusRec(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusRec
    功    能:设置终端是否录像
    参    数:BOOL32 bRec                        [in]    是否录像
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	//    void SetStatusRec(BOOL32 bRec);
	
    /*=============================================================================
    函 数 名:IsStatusTvWall
    功    能:判断终端是否进入电视墙
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusTvWall(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusTvWall
    功    能:设置终端是否进入电视墙
    参    数:BOOL32 bTvWall                     [in]    是否进入电视墙
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    void SetStatusTvWall(BOOL32 bTvWall);
	
	/*=============================================================================
    函 数 名:IsStatusHdu
    功    能:判断终端是否进入电视墙
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/09/30  1.0     邹俊龙    创建
    =============================================================================*/
    BOOL32 IsStatusHdu(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusHdu
    功    能:设置终端是否进入高清电视墙
    参    数:BOOL32 bHdu                     [in]    是否进入高清电视墙
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/09/30  1.0     邹俊龙    创建
    =============================================================================*/
    void SetStatusHdu(BOOL32 bHdu);
	
    /*=============================================================================
    函 数 名:IsStatusMix
    功    能:判断终端是否混音
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusMix(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusMix
    功    能:设置终端是否混音
    参    数:BOOL32 bMix                        [in]    是否混音
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    void SetStatusMix(BOOL32 bMix);
	
    /*=============================================================================
    函 数 名:IsStatusMultiTvWall
    功    能:判断终端是否进入多画面电视墙
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/10/14  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusMultiTvWall(void) const;
	
    /*=============================================================================
    函 数 名:SetStatusMultiTvWall
    功    能:设置终端是否进入多画面电视墙
    参    数:BOOL32 bMultiTvWall                [in]    是否进入多画面电视墙
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/10/14  4.0     王昊    创建
    =============================================================================*/
    void SetStatusMultiTvWall(BOOL32 bMultiTvWall);
	
    /*=============================================================================
    函 数 名:IsStatusDual
    功    能:判断终端是否发送发送双流
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/12/20  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsStatusDual(void) const;
	
    /*=============================================================================
    函 数 名:IsMcu
    功    能:终端是否MCU
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsMcu(void) const;
	
    /*=============================================================================
    函 数 名:IsMMcu
    功    能:终端是否上级MCU
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsMMcu(void) const;
	
    /*=============================================================================
    函 数 名:IsSMcu
    功    能:终端是否下级MCU
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsSMcu(void) const;
	
    /*=============================================================================
    函 数 名:IsLocalMcu
    功    能:判断终端是否是本级MCU
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/13  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsLocalMcu(void) const;
	
    /*=============================================================================
    函 数 名:SetLastRateReqTime
    功    能:设置上一次发送请求所有终端码率的时间
    参    数:u32 dwTime         [in] 时间
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/01/20  4.0     顾振华  创建
    =============================================================================*/
    void SetLastRateReqTime(u32 dwTime) { m_dwLastRateReqTime = dwTime; }
	
    /*=============================================================================
    函 数 名:GetLastRateReqTime
    功    能:获取上一次发送请求所有终端码率的时间
    参    数:
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/01/20  4.0     顾振华  创建
    =============================================================================*/
    inline u32 GetLastRateReqTime() const { return m_dwLastRateReqTime; }
	
    /*=============================================================================
    函 数 名:GetMtVersion
    功    能:获取终端版本信息
    参    数:
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2007/12/27  4.0     王廖荣  创建
    =============================================================================*/
    inline TMtExt2 GetMtVersion() const { return m_tMtVersion; }
	
    /*=============================================================================
    函 数 名:SetMtVersion
    功    能:设置终端版本信息
    参    数:
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2007/12/27  4.0     王廖荣  创建
    =============================================================================*/
    inline void SetMtVersion( const TMtExt2 &tMtExt2 ) { m_tMtVersion = tMtExt2; }
	
    /*=============================================================================
    函 数 名:PrintInfo
    功    能:在telnet里打印调试信息
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    void PrintInfo(void) const;
	
private:
#define MTSTATUS_QUIET_MASK         (u32)0x00000001
#define MTSTATUS_DUMB_MASK          (u32)0x00000002
#define MTSTATUS_INSPECT_MASK       (u32)0x00000004
#define MTSTATUS_VMP_MASK           (u32)0x00000008
#define MTSTATUS_REC_MASK           (u32)0x00000010
#define MTSTATUS_TVWALL_MASK        (u32)0x00000020
#define MTSTATUS_MIX_MASK           (u32)0x00000040
#define MTSTATUS_MULTITVWALL_MASK   (u32)0x00000080
#define MTSTATUS_DUAL_MASK          (u32)0x00000100
#define MTSTATUS_HDU_MASK			(u32)0x00000200
	
    //最低位：是否在线；次低位：是否提示过；高16位：不在线时的错误码
#define MT_ONLINE_MASK              (u32)0x00000001
#define MT_PROMPT_MASK              (u32)0x00000002
#define MT_ERROR_CODE_MASK          (u32)0xFFFF0000
	
    //别名类型
#define MT_ALIAS_IP                 (u8)0x01
#define MT_ALIAS_E164               (u8)0x02
#define MT_ALIAS_H323ID             (u8)0x04
#define MT_ALIAS_H320ID             (u8)0x08
#define MT_ALIAS_H320ALIAS          (u8)0x10
	
	
private:
    typedef struct tagMcsMtAlias
    {
        tagMcsMtAlias() { SetNull(); }
		
        inline void SetNull(void) { memset( this, 0, sizeof (tagMcsMtAlias) ); }
		
        u8           m_byAliasType;              //别名类型(合并字段)
        s8           m_szAlias[MCS_MAXLEN_ALIAS + 128];    //H.323 ID/H.320 Alias 2010
        s8           m_szE164[MAXLEN_E164];      //E.164
        TTransportAddr  m_tIP;                      //IP
        u8              m_byLayer;                  //层号(H320终端专用)
        u8              m_bySlot;                   //槽号(H320终端专用)
        u8              m_byChnnl;                  //通道号(H320终端专用)
    } TMcsMtAlias;
	
	
    TMcsMtAlias m_tMtAlias;             //终端别名
	//    TMtAlias    m_tMtAlias[3];          //  终端别名
    u32         m_dwUseStatus;          //  使用状态
    u32         m_dwOnlineStatus;       //  在线状态
    TMtExtCur   m_tMtExt;               //  终端扩展结构
    TMtExt2     m_tMtVersion;           //  终端版本信息等
    TMtStatus   m_tMtStatus;            //  终端状态
    TEqp        m_tRecEqp;              //  录像设备
    TRecProg    m_tRecProg;             //  录像进度
	
    u32         m_dwLastRateReqTime;    //  记录上一次发送请求所有终端码率的时间
    u8          m_byUsed;               //  是否被占用，由于模拟pcmt在VCS上作为终端上报IP都是一样无法区分，加此字段来做区分 by xts （借鉴多运营商）
	//u8			m_byInVmpMeber;			// 标示此cmt是画面合成器的成员 在UpdateMtVmpStatus ，UpdateMcuMediaSrc 用到

	//	画面合成状态
	u32			m_dwEqpStatus;

    //保存当前终端视频源别名
    TMcsMtVideoSrcAlias     m_tMcsMtVideoSrcAlias;
public:
	TMt&		GetMediaSrc(){ return m_tMediaSrcMt; }
	void		SetMediaSrc( const TMt& tMediaSrc ){ m_tMediaSrcMt = tMediaSrc; }
	u8			GetLockStatus(){ return m_byLockStatus; }
	void		SetLockStatus( u8 byLockStatus ){ m_byLockStatus = byLockStatus; }
	TMixParam&	GetMixParam(){ return m_tMixParam; }
	void		SetMixParam( const TMixParam& tMixParam ){ m_tMixParam = tMixParam; }
    void        SetVideoSrcAlias( const TMcsMtVideoSrcAlias& tMcsMtVideoSrcAlias ) { m_tMcsMtVideoSrcAlias = tMcsMtVideoSrcAlias; }
    const TMcsMtVideoSrcAlias& GetVideoSrcAlias() { return m_tMcsMtVideoSrcAlias; }
	BOOL32		GetVideoSrcAlias( u8 byPort, s8* byAlias, size_t dwMaxLen );
    BOOL32      IsHaveSrcVideoAlias(){ return m_tMcsMtVideoSrcAlias.m_byVideoSrcNum ? TRUE: FALSE; }
    void        SetMtUsed(u8 byUsed){ m_byUsed = byUsed; }
    u8          GetMtUsed() { return m_byUsed; }
	//void        SetMtInVmpMeber(u8 byInVmpMeber){ m_byInVmpMeber = byInVmpMeber; }
    //u8          GetMtInVmpMeber() { return m_byInVmpMeber; }

	void		AddRelatedVmpEqp(u8 byEqpId);
	void		SubRelatedVmpEqp(u8 byEqpId);

private:
	/*=============================================================================
    函 数 名:SetStatusVmp
    功    能:设置终端是否画面合成
    参    数:BOOL32 bVmp                        [in]    是否画面合成
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
    void SetStatusVmp(BOOL32 bVmp);
protected:
	// mcu才有以下数据，mt不存在
    TMt             m_tMediaSrcMt;      //  该MCU媒体源(本级不填)
    u8              m_byLockStatus;     //  该MCU锁定情况(本级不填)
    TMixParam       m_tMixParam;        //  该MCU混音参数(本级不填)	

	//friend void showmtvmpstatus();
	//#ifdef _DEBUG
    /*=============================================================================
    函 数 名:Dump
    功    能:打印本类的成员调试信息
    参    数:CDumpContext &dc                   [in]    打印头
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/25  4.0     王昊    创建
    =============================================================================*/
	//    virtual void Dump(CDumpContext &dc) const;
	//#endif  //  _DEBUG
};

void ReplaceOrEraseMtAllVmpStatus(CMt& cOldMt, CMt& cNewMt);

// CConf保存单个会议信息
// ---------------------------------------------------------------------------------------------
// 多级级联新增代码 by 牟兴茂 2010-7-10

/*---------------------------------------------------------------------
* 类	名：CDataProxy
* 功    能：数据代理类，转换由mcu发送过来的数据类型，初始化IMcu， VecMcuList
* 特殊说明：减少Mcu与CConf的耦合
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/07/16	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CConf;
Interface CDataProxy
{
public:
	virtual ~CDataProxy(){}
	static BOOL32 OnDataComing( void* pClass, void* pData );
	static BOOL32 TransData( CConf* pConf, TConfAllMcuInfo& tConfAllMcuInfo );
};

/*-----------------------------------------------------------
mcu _ mt
| _ mcu _ mt
|  _ mcu  
|  _ mt
IMcu 表示mt，也表示mcu，通过彼此的嵌套，形成级联
-------------------------------------------------------------*/
typedef IItem< CMt, u32 > IMcu;

class CConf : public TConfInfo
{
	friend class CDataProxy;
protected:
	IMcu* m_piMcu; // 所有终端组成的树
	mutable TConfAllMtInfo m_tConfAllMtInfo;
	mutable TConfAllMcuInfo m_tConfAllMcuInfo; // mcu索引表
	u32 m_dwMcuNbr;
public:
	TConfInfoEx		m_tConfInfoEx;						//v4r7新增
	TMcsVmpModule	m_tVmpModuleEx;
public:

	//----------------------------------------------------------------------------
	// 为保证编译能够通过，弃用的函数写在这里
	TplArray<CMt> tplArray;
	const TplArray<CMt>&	GetMtList( u16 dwMcuInd ) const
	{
		return tplArray;
	}
	const TplArray<CMt>&	GetMtList( TMcu& tMcu ) const
	{
		return tplArray;
	}
	
	BOOL32 GetMcuList(u8 *pbyMcuId, s32 &nNum) const { return FALSE; }
	
	BOOL32 IsSMcu(const TMt& tMt) const { return CMtUtility::IsSMcu( tMt ); }
	BOOL32 IsMMcu(const TMt& tMt) const { return CMtUtility::IsMMcu( tMt ); }
	BOOL32 IsMcu(const TMt& tMt) const { return CMtUtility::IsMcu( tMt ); }
	BOOL32 IsLocalMcu( const TMt& tMt) const { return CMtUtility::IsLocalMcu( tMt ); }

	void SetConfInfoEx(const TConfInfoEx &tConfInfoEx) { m_tConfInfoEx = tConfInfoEx; }
	TConfInfoEx& GetConfInfoEx( void ){ return m_tConfInfoEx; }
	
public:
    CConf();
	CConf(const CConf& cConf);
    ~CConf();
public:
/*---------------------------------------------------------------------
* 函 数 名：GetMMcu
* 功    能：获得上级mcu结点
* 参数说明：
* 返 回 值：上级mcu结点指针，如果为空，上级mcu不存在
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/07/31	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	IMcu* GetMMcu() const;
	/*---------------------------------------------------------------------
	* 函 数 名：GetMcuIdx
	* 功    能：获得一个Mcu自己在列表中的索引
	* 参数说明：[in]tMcu 所需要查找的mcu
	* 返 回 值：mcu的索引
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/29	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	u16 GetMcuIdx( const TMcu& tMcu ) const;
	
	/*---------------------------------------------------------------------
	* 函 数 名：ResetOnlineStatus
	* 功    能：重新设置mcu下的终端状态
	* 参数说明：[in] wMcuIdx 所需要更新的mcu
	* 返 回 值：是否重置
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/27	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL32 ResetOnlineStatus( u16 wMcuIdx );
	/*---------------------------------------------------------------------
	* 函 数 名：SetAllMcuInfo
	* 功    能：设置所有mcu列表信息
	* 参数说明：[in] tConfAllMcuInfo mcu索引表
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/23	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL32 SetAllMcuInfo( TConfAllMcuInfo& tConfAllMcuInfo )
	{
		CDataProxy::TransData( this, tConfAllMcuInfo );
		return TRUE;
	}
	
	/*---------------------------------------------------------------------
	* 函 数 名：SetAllMtInfoData
	* 功    能：设置所有终端信息，由于mcu需要切包发送，这里需要一份一份的拷贝
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/23	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL32 SetAllMtInfoData( u32 dwIndex, void* pData, u32 dwBuffLen );
	
	/*---------------------------------------------------------------------
	* 函 数 名：ResetAllMtInConf
	* 功    能：重置所有终端、没有在会议中就删除、在会议中就设置在线状态
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/28	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	void ResetAllMtInConf( IMcu* pMcu );
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetBaseMcu
	* 功    能：返回树根
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/22	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	IMcu* GetBaseMcu() const
	{
		return m_piMcu;
	}
	
	
	/*---------------------------------------------------------------------
	* 函 数 名：HasParentMcu
	* 功    能：是否有父节点
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/22	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL32 HasParentMcu() const
	{
		return !m_tConfAllMtInfo.m_tMMCU.IsNull();
	}
	
	void PrintAllMtInfo();
	/*---------------------------------------------------------------------
	* 函 数 名：GetIMcu
	* 功    能：从Mcu列表里面获得指向树结点的代表mcu的指针IMcu
	* 参数说明：[in]wMcuIdx mcu在列表中的所引
	* 返 回 值：IMcu
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/14	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	IMcu* GetIMcu( u16 wMcuIdx ) const;
	
    /*=============================================================================
    函 数 名:operator=
    功    能:赋值运算符
    参    数:const CConf &cConf                 [in]    源会议
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/07  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    const CConf& operator=(const CConf &cConf);
	
    /*=============================================================================
    函 数 名:operator+=
    功    能:重载运算符, 用新的会议信息更新内容
    参    数:const TConfInfo &tConfInfo         [in]    会议信息
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/07  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    const CConf& operator+=(const TConfInfo &tConfInfo);
	
    /*=============================================================================
    函 数 名:UpdateMtList
    功    能:更新终端列表
    参    数:const TMcu &tMcu                   [in]    列表所属MCU
	const TMtExt *ptMtExt              [in]    终端列表数组
	s32 nNum                           [in]    终端个数
    注    意:ptMtExt里某些终端可能为空
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
	void UpdateMtList( const TMcu &tMcu, TMtExtCur *ptMtExt, s32 nNum );

    
    /*=============================================================================
    函 数 名:UpdateMtList
    功    能:更新终端列表
    参    数:TMcsMtVideoSrcAliasParam *ptParam           [in]    终端视频源列表
    s32 nNum                                    [in]    终端个数
    注    意:ptMtExt里某些终端可能为空
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/04/14  4.0     涂文斌    创建
    =============================================================================*/
    void CConf::UpdateMtList(TMcsMtVideoSrcAliasParam *ptParam, s32 nNum = 1);
   	
	void UpdateMtList( IMcu* pMcu, TMtExtCur *ptMtExt, s32 nNum );

    /*=============================================================================
    函 数 名:UpdateMtList
    功    能:更新终端状态列表
    参    数:const TMcu &tMcu                   [in]    列表所属MCU
	const TMtStatus *ptMtStatus        [in]    终端状态列表数组
	s32 nNum                           [in]    终端个数
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void UpdateMtList(const TMcu &tMcu, const TMtStatus *ptMtStatus, s32 nNum);
	
	void UpdateMtList(IMcu* pMcu, const TMtStatus *ptMtStatus, s32 nNum );
	
    /*=============================================================================
    函 数 名:UpdateMtList
    功    能:更新终端列表
    参    数:u8 byMcuId                         [in]    终端所属MCU Id
	TplArray<CMt> &tplMt               [in]    终端列表
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/05  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void UpdateMtList( u16 byMcuId, TplArray<CMt> &tplMt );
	
	
    /*=============================================================================
    函 数 名:GetMcuInfoList
    功    能:获取MCU信息列表
    参    数:const TMcu &tMcu                   [in]    终端所属MCU
    注    意:无
    返 回 值:终端列表
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
	2010/11/17	v4.1	牟兴茂	节点终端包含的mcuindex其实是它父节点的
    =============================================================================*/
   	const IMcu*	GetMcuInfoList(const TMcu &tMcu) const
	{
		TMcu *ptMcu = const_cast<TMcu*>(&tMcu);
		u16 wMcuIdx = GetMcuIdx( *ptMcu );
		return GetIMcu( wMcuIdx );
	}
	
    /*=============================================================================
    函 数 名:GetMcuInfoList
    功    能:获取MCU信息列表
    参    数:u8 byMcuId                         [in]    终端所属MCU的Mcu Id
    注    意:无
    返 回 值:终端列表
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
	const IMcu*	GetMcuInfoList(u16 byMcuId) const
	{
		return GetIMcu( byMcuId );
	}
	
	
    /*=============================================================================
    函 数 名:GetMtList
    功    能:获取指定MCU终端列表
    参    数:const TMcu &tMcu                   [in]    MCU    [out] tplMt
    注    意:无
    返 回 值:终端列表
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
	2010/12/15  v4.1    沈钦    添加GetAllMtList   GetMtList的vector版本
    =============================================================================*/
	BOOL32	GetMtList( const TMcu& tMcu, TplArray<CMt>& tplMt ) const;
	BOOL32  GetMtListIncludeMcu( IMcu* pMcu, TplArray<CMt>& tplMt ) const;
	BOOL32  GetMtListIncludeMcu( IMcu* pMcu, vector<CMt>& vctMt ) const;
	BOOL32	GetAllMtList( TplArray<CMt>& tplMt ) const;
	BOOL32	GetAllMtList( vector<CMt>& vctMt ) const;
	BOOL32	GetMtList( u16 dwMcuInd, TplArray<CMt>& tplMt ) const;
	BOOL32	GetMtList( u16 dwMcuInd, vector<CMt>& vctMt ) const;
	/*---------------------------------------------------------------------
	* 函 数 名：GetMtCount
	* 功    能：通过传入的mcu索引，获得终端的数量，不包含自身
	* 参数说明：[in] dwMcuId mcu的索引号
	* 返 回 值：终端数量
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/16	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	u32 GetMtCount( u16 dwMcuId ) const 
	{
		IMcu* pMcu = GetIMcu( dwMcuId );
		if ( pMcu == NULL )
		{
			return 0;
		}
		return pMcu->GetItemCount();
	}
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetMtCount
	* 功    能：通过传入的mcu+mtid的混合参数，获得终端的数量，不包含自身
	* 参数说明：[in]dwMtId mcuidx+mcuid的混合体
	* 返 回 值：终端数量
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/16	v1.0		牟兴茂		创建
	调用此接口的时候切记，请勿传入由mcu自己tmt转换过后的mtid，因为这个id里面包含的mcuidx其实是它父节点的
	----------------------------------------------------------------------*/
	u32 GetMtCount( u32 dwMtId ) const 
	{
		IMcu* pMcu = GetIMcu( CMtUtility::GetMcuId( dwMtId ) );
		return pMcu->GetItemCount();
	}
	
    /*=============================================================================
    函 数 名:GetMcuList
    功    能:获取MCU列表
    参    数:TMcu *ptMcu                        [out]   TMcu数组
	s32 &nNum                          [in/out]    数组大小/实际填充大小
    注    意:无
    返 回 值:成功:TRUE; 数组分配过小: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.0	牟兴茂	保留接口，重写所有代码
	2010/11/17	v4.0	牟兴茂	获得节点终端所包含的mcuindex是上级，如果需要获得此节点的index，调用GetMcuIdx接口
    =============================================================================*/
	BOOL32 GetMcuList(TMcu *ptMcu, s32 &nNum) const
    { 
		s32 nMcuCount = GetMcuNum();
		if (nNum < nMcuCount)
		{
			return FALSE;
		}	
		
		nNum = nMcuCount;
		for ( s32 nIndex = 0; nIndex < nMcuCount; nIndex++ )
		{
            u16 wMcuIdx = 0;
			// 获取mcu指针
			IMcu* pMcu = GetVisableIMcuByIndex( nIndex, wMcuIdx );//m_piMcuIndex->GetItemByIndex( nIndex );
			
			if ( pMcu != NULL )
			{
				// 获取代表mcu的cmt结构
				CMt* pMt = pMcu->GetItemData();	
				if ( pMt != NULL )
				{
					ptMcu[nIndex].SetMcuIdx( GetMcuIdx( *pMt ) );
				}			
			}		
		}	
		return TRUE;
	}
	
    /*=============================================================================
    函 数 名:GetMcuList
    功    能:获取MCU列表
    参    数:u8 *pbyMcuId                       [out]   McuId数组
	s32 &nNum                          [in/out]    数组大小/实际填充大小
    注    意:无
    返 回 值:成功:TRUE; 数组分配过小: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 GetMcuList(u16 *pbyMcuId, s32 &nNum) const
    { 
		s32 nMcuNum = GetMcuNum();
		if (nNum < nMcuNum)
		{
			return FALSE;
		}
		
		nNum = nMcuNum;
		u32 dwCounts = 0;
		for (s32 nIndex = 0; nIndex < nNum; nIndex ++)
		{
            u16 wMcuIdx = 0;
			IMcu* pMcu = GetVisableIMcuByIndex( nIndex, wMcuIdx );
			if ( pMcu != NULL )
			{
				pbyMcuId[dwCounts++] = wMcuIdx;		
			}		
		}
		
		return TRUE;
	}
	
    /*=============================================================================
    函 数 名:GetMcuNum
    功    能:获取MCU个数
    参    数:无
    注    意:无
    返 回 值:MCU个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetMcuNum(void) const 
	{ 
		return m_dwMcuNbr;
		//	return m_piMcuIndex->GetItemCount();
	}
	
    /*=============================================================================
    函 数 名:SetAllMtInfo
    功    能:设置TConfAllMtInfo
    参    数:const TConfAllMtInfo &tConfAllMtInfo   [in]    TConfAllMtInfo
    注    意:
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetAllMtInfo(const TConfAllMtInfo &tConfAllMtInfo)
    { 
		m_tConfAllMtInfo = tConfAllMtInfo; 
	}
	
    /*=============================================================================
    函 数 名:GetConfAllMtInfo
    功    能:获取TConfAllMtInfo
    参    数:无
    注    意:无
    返 回 值:const TConfAllMtInfo
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    const TConfAllMtInfo& GetConfAllMtInfo(void) const
    { 
		return m_tConfAllMtInfo; 
	}
	
    /*=============================================================================
    函 数 名:GetConfAllMtInfo
    功    能:获取TConfAllMtInfo
    参    数:无
    注    意:无
    返 回 值:TConfAllMtInfo
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    TConfAllMtInfo& GetConfAllMtInfo(void)
    { 
		return m_tConfAllMtInfo; 
	}
	
	/*=============================================================================
    函 数 名:GetConfAllMtInfo
    功    能:获取TConfAllMtInfo
    参    数:无
    注    意:无
    返 回 值:TConfAllMtInfo
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    TConfAllMcuInfo& GetConfAllMcuInfo(void)
    { 
		return m_tConfAllMcuInfo;
	}
	
    /*=============================================================================
    函 数 名:SetLocalMcuIpAddr
    功    能:设置本级MCU的IP地址
    参    数:u32 dwMcuIpAddr                    [in]    本级MCU的IP地址
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetLocalMcuIpAddr(u32 dwMcuIpAddr)
    { 
		m_dwLocalMcuIp = dwMcuIpAddr;
	}
	
    /*=============================================================================
    函 数 名:SetMcuMediaSrc
    功    能:设置MCU媒体源
    参    数:const TMcu &tMcu                   [in]    TMcu
	const TMt &tMt                     [in]    媒体源
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline void SetMcuMediaSrc(const TMcu &tMcu, const TMt &tMt)
    { 
		SetMcuMediaSrc( tMcu.GetMcuIdx(), tMt );	
	}
	
    /*=============================================================================
    函 数 名:SetMcuMediaSrc
    功    能:设置MCU媒体源
    参    数:u16 byMcuId                         [in]    McuId
	const TMt &tMt                     [in]    媒体源
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
    2006/09/05  4.0     王昊    特殊处理, 混音
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetMcuMediaSrc(u16 byMcuId, const TMt &tMt)
	{
		IMcu* pMcu = GetIMcu( byMcuId );
		if ( pMcu != NULL )
		{
			CMt* pMt = pMcu->GetItemData();
			if ( pMt != NULL )
			{
				pMt->SetMediaSrc( tMt );
			}	
		}
	}
	
    /*=============================================================================
    函 数 名:GetMcuMediaSrc
    功    能:获取MCU媒体源
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:MCU媒体源
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
	切记：传入的这个TMcu是要表示的是你需要的mcu的信息，而不是上级mcu的信息，建议尽量调用下面一个接口
    =============================================================================*/
    const TMt& GetMcuMediaSrc(const TMcu &tMcu) const
	{
		return GetMcuMediaSrc( tMcu.GetMcuIdx() );
	}
	
    /*=============================================================================
    函 数 名:GetMcuMediaSrc
    功    能:获取MCU媒体源
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:MCU媒体源
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    const CMt& GetMcuMediaSrc(u16 byMcuId) const;
	
	
    /*=============================================================================
    函 数 名:SetMcuLockStatus
    功    能:设置MCU锁定状态
    参    数:const TMcu &tMcu                   [in]    MCU
	u8 byLockStatus                    [in]    锁定状态
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetMcuLockStatus(const TMcu &tMcu, u8 byLockStatus)
    { 
		IMcu* pMcu = GetIMcu( tMcu.GetMcuIdx() );
		if ( pMcu != NULL )
		{	
			CMt* pMt = pMcu->GetItemData();
			if ( pMt != NULL )
			{
				pMt->SetLockStatus( byLockStatus );
			}		
		}
	}
	
    /*=============================================================================
    函 数 名:IsMcuLocked
    功    能:MCU是否被锁定
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMcuLocked(const TMcu &tMcu) const
    { 
		return IsMcuLocked( tMcu.GetMcuIdx() );
	}
	
    /*=============================================================================
    函 数 名:IsMcuLocked
    功    能:MCU是否被锁定
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMcuLocked(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:SetMt
    功    能:设置终端完整结构
    参    数:const CMt &cMt                     [in]    终端完整结构
    注    意:无
    返 回 值:成功返回TRUE; 失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 SetMt(const CMt &cMt);
	
    /*=============================================================================
    函 数 名:SetMt
    功    能:设置终端扩展结构
    参    数:const TMtExt &tMtExt               [in]    终端扩展结构
    注    意:无
    返 回 值:成功返回TRUE; 失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SetMt(const TMtExtCur &tMtExt);
	
	
    /*=============================================================================
    函 数 名:SetMt
    功    能:设置终端状态
    参    数:const TMtStatus &tMtStatus         [in]    终端状态
    注    意:无
    返 回 值:现有CMt里的m_tMtStatus与tMtStatus不一样:TRUE; 否则:FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
    =============================================================================*/
    BOOL32 SetMt(const TMtStatus &tMtStatus);
	
	
    /*=============================================================================
    函 数 名:GetMt
    功    能:获取终端
    参    数:const TMt &tMt                     [in]    终端
    注    意:无
    返 回 值:终端完整结构
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    CMt& GetMt(const TMt &tMt) const;
	
	CMt& GetMt( u16 dwMcuId, u8 wEqpId) const;
	
	CMt& GetMt( u32 dwMtId ) const;
	
    /*=============================================================================
    函 数 名:GetLocalMcu
    功    能:获取当前MCU的完整终端结构
    参    数:无
    注    意:无
    返 回 值:当前MCU完整终端结构
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    CMt& GetLocalMcu(void) 
    { 
		// 返回本级mcu
		return m_cLocalMcu;
	}
	
    /*=============================================================================
    函 数 名:GetMcu
    功    能:根据TMcu获取对应的CMt
    参    数:const TMcu &tMcu                   [in]    MCU结构
    注    意:无
    返 回 值:CMt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    CMt& GetMcu(const TMcu &tMcu) const
	{
		return GetMcu( tMcu.GetMcuIdx() );
	}
	
    /*=============================================================================
    函 数 名:GetMcu
    功    能:根据McuId获取对应的CMt
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:CMt
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    CMt& GetMcu(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:IsMtInConf
    功    能:终端是否在受邀列表中
    参    数:const TMt &tMt                     [in]    终端基本结构
    注    意:根据TConfAllMtInfo来判断
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMtInConf(const TMt &tMt) const
    {
		return m_tConfAllMtInfo.MtInConf( tMt.GetMcuIdx(), tMt.GetEqpId() );
	}
	
	BOOL32 IsMtInConf( u16 dwMcuId, u8 wEqpId ) const
    {
		return m_tConfAllMtInfo.MtInConf( dwMcuId, wEqpId );
	}
	
	BOOL32 IsMtInConf( u32 dwMtId ) const
    {
		u16 dwMcuId = CMtUtility::GetMcuId( dwMtId );
		u8  wEqpId	= CMtUtility::GetMtId( dwMtId );
		return m_tConfAllMtInfo.MtInConf( dwMcuId, wEqpId );
	}
	
    /*=============================================================================
    函 数 名:IsMtInConf
    功    能:终端是否在与会列表中
    参    数:const CMt &cMt                     [in]    终端完整结构
    注    意:根据TConfAllMtInfo来判断
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMtJoinConf(const CMt &cMt) const
    { 
		return m_tConfAllMtInfo.MtJoinedConf( cMt.GetMcuIdx(), cMt.GetEqpId() );
	}
	
	BOOL32 IsMtJoinConf( u16 dwMcuId, u8 wEqpId ) const
    {
		return m_tConfAllMtInfo.MtJoinedConf( dwMcuId, wEqpId );
	}
	
	BOOL32 IsMtJoinConf( u32 dwMtId ) const
    {
		u16 dwMcuId = CMtUtility::GetMcuId( dwMtId );
		u8	wEqpId	= CMtUtility::GetMtId( dwMtId );
		return m_tConfAllMtInfo.MtJoinedConf( dwMcuId, wEqpId );
	}
	
    /*=============================================================================
    函 数 名:IsMtInConf
    功    能:终端是否在与会列表中
    参    数:const TMt &tMt                     [in]    终端基本结构
    注    意:根据TConfAllMtInfo来判断
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMtJoinConf(const TMt &tMt) const
    { 
		return m_tConfAllMtInfo.MtJoinedConf( tMt.GetMcuIdx(), tMt.GetEqpId() );
	}
	
	
    /*=============================================================================
    函 数 名:IsMtAvailable
    功    能:判断终端是否可操作
    参    数:const CMt &cMt                     [in]    需要判断的终端
	BOOL32 bIncludeMcu                 [in]    是否允许终端为MCU
    注    意:无
    返 回 值:终端是本级或下级MCU下的终端 : TRUE
	终端是本级MCU或上级MCU下的终端 : FALSE
	终端是上级或下级MCU : bIncludeMcu
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/24  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMtAvaliable(const TMt &tMt, BOOL32 bIncludeMcu = TRUE) const;
	
    /*=============================================================================
    函 数 名:IsMtInSMcu
    功    能:判断终端是否在下级MCU上
    参    数:const CMt &cMt                     [in]    需要判断的终端
	TMcu &tMcu                         [out]   终端所在下级MCU
	BOOL32 bIncludeMcu                 [in]    是否包含下级MCU
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/24  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsMtInSMcu(const TMt &tMt, TMcu &tMcu, BOOL32 bIncludeMcu = TRUE) const;
	
	/*---------------------------------------------------------------------
	* 函 数 名：IsMtInMMcu
	* 功    能：判断终端是不是上级mcu下的终端
	* 参数说明：[in] 终端 [out] tMcu上级终端
	* 返 回 值：是否存在
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/07/30	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
    BOOL32 IsMtInMMcu(const TMt &tMt, TMcu &tMcu ) const;
	
    /*=============================================================================
    函 数 名:GetOnlineMtNum
    功    能:获取在线终端总数
    参    数:无
    注    意:不包括MCU, 从TConfAllMtInfo里提取
    返 回 值:在线终端总数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/24  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetOnlineMtNum(void) const;
	
    /*=============================================================================
    函 数 名:GetOfflineMtNum
    功    能:获取不在线终端总数
    参    数:无
    注    意:不包括MCU, 从TConfAllMtInfo里提取
    返 回 值:不在线终端总数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/24  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetOfflineMtNum(void) const;
	
    /*=============================================================================
    函 数 名:GetMtNum
    功    能:获取终端总个数
    参    数:无
    注    意:不包括MCU, 从TConfAllMtInfo里提取
    返 回 值:终端总个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/15  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetMtNum(void) const;
	
    /*=============================================================================
    函 数 名:GetConfCascadeType
    功    能:获取会议级联类型
    参    数:无
    注    意:无
    返 回 值:emSingleConf、emTwoLayerMMcuConf、emTwoLayerSMcuConf、emThreeLayerConf
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetConfCascadeType(void) const;
	
    /*=============================================================================
    函 数 名:IsConfCascade
    功    能:会议是否级联
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfCascade(void) const 
	{ 
		return (GetConfCascadeType() != emSingleConf);
	}
	
    /*=============================================================================
    函 数 名:SetRecEqp
    功    能:设置会议录像设备
    参    数:const TEqp &tRecEqp                [in]    录像设备
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetRecEqp(const TEqp &tRecEqp)
	{
		if (!m_tStatus.IsNoRecording())
		{
			m_tRecEqp = tRecEqp;
		}
	}
	
    /*=============================================================================
    函 数 名:SetPlayEqp
    功    能:设置会议放像设备
    参    数:const TEqp &tPlayEqp               [in]    录像设备
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetPlayEqp(const TEqp &tPlayEqp)
	{
		if (!m_tStatus.IsNoPlaying())
		{
			m_tPlayEqp = tPlayEqp;
		}
	}
	
    /*=============================================================================
    函 数 名:GetRecEqp
    功    能:获取录像设备
    参    数:无
    注    意:无
    返 回 值:录像设备
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    TEqp& GetRecEqp(void) const 
	{ 
		// conf自行处理
		return const_cast<TEqp&>(m_tRecEqp); 
	}
	
    /*=============================================================================
    函 数 名:GetPlayEqp
    功    能:获取放像设备
    参    数:无
    注    意:无
    返 回 值:放像设备
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    TEqp& GetPlayEqp(void) const { return const_cast<TEqp&>(m_tPlayEqp); }
	
    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:获取会议序列号
    参    数:无
    注    意:界面上SetItemData时用到
    返 回 值:会议序列号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    u32 GetSerialNo(void) const { return m_dwSN; }
	
    /*=============================================================================
    函 数 名:SetSerialNo
    功    能:设置会议序列号
    参    数:u32 dwSN                           [in]    序列号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetSerialNo(u32 dwSN) { m_dwSN = dwSN; }
	
    /*=============================================================================
    函 数 名:IsLockBySelf
    功    能:会议是否被当前会控锁定
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsLockBySelf(void) const { return (m_byLockBySelf > 0); }
	
    /*=============================================================================
    函 数 名:SetLockBySelf
    功    能:设置会议是否被当前会控锁定
    参    数:BOOL32 bLockBySelf                 [in]    BOOL
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetLockBySelf(BOOL32 bLockBySelf)
    { m_byLockBySelf = (bLockBySelf ? true : false); }
	
    /*=============================================================================
    函 数 名:IsConfOngoing
    功    能:是否即时会议
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfOngoing(void) const { return m_tStatus.IsOngoing(); }
	
    /*=============================================================================
    函 数 名:IsConfTemplate
    功    能:是否会议模板,已经废弃.CConf都是即时会议
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfTemplate(void) const { return m_tStatus.IsTemplate(); }
	
    /*=============================================================================
    函 数 名:IsConfScheduled
    功    能:是否预约会议,已经废弃.CConf都是即时会议
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/16  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfScheduled(void) const { return m_tStatus.IsScheduled(); }
	
    /*=============================================================================
    函 数 名:GetSpeaker
    功    能:获取会议发言人
    参    数:bMediaSrc		是否尝试获取MCU媒体源
    注    意:如果发言人是MCU, 则查找该MCU的媒体源
    返 回 值:会议发言人
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    CMt GetSpeaker( BOOL bMediaSrc = TRUE ) const;
	
    /*=============================================================================
    函 数 名:GetChairman
    功    能:获取会议主席
    参    数:bMediaSrc		是否尝试获取MCU媒体源
    注    意:如果主席是MCU, 则查找该MCU的媒体源
    返 回 值:会议注意
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
	2011/10/28	v4.1	邹俊龙	创建
    =============================================================================*/
	CMt GetChairman( BOOL bMediaSrc = TRUE ) const;
	
    /*=============================================================================
    函 数 名:operator==
    功    能:比较运算符
    参    数:const CConf &cConf                 [in]    会议
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline BOOL32 operator==(const CConf &cConf) const
    { return (cConf.GetConfId() == m_cConfId); }
	
    /*=============================================================================
    函 数 名:SetNull
    功    能:清空
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline void SetNull(void);
	
    /*=======================================================================
    函 数 名:HasGetInitValue
    功    能:判断该会议是否从MCU处理到过初始信息, 
	这些信息为会议独享、所有终端状态、所有终端别名、
	下级MCU锁定状态、下级MCU混音参数
    参    数:s32 nType                          [in]    获到的信息类型
	emLockInfo/
	emAllMtStatus/
	emAllMtAlias/
	emAllMcuMediaSrc/
	emAllSMcuLockStatus/
	emAllSMcuMixParam/
	emAllInfo
    注    意:无
    返 回 值:已经得到过返回TRUE, 没有得到过返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =======================================================================*/
    BOOL32 HasGetInitValue(s32 nType);
	
    /*=============================================================================
    函 数 名:SetGetInitValue
    功    能:设置该会议已经从MCU处理到过初始信息, 
	这些信息为会议独享、所有终端状态、所有终端别名、下级MCU锁定状态、
	下级MCU混音参数
    参    数:s32 nType                          [in]    获到的信息类型
	emLockInfo/
	emAllMtStatus/
	emAllMtAlias/
	emAllMcuMediaSrc/
	emAllSMcuLockStatus/
	emAllSMcuMixParam/
	emAllInfo
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    void SetGetInitValue(s32 nType);
	
    /*=============================================================================
    函 数 名:SetMixParam
    功    能:设置混音参数
    参    数:const TMcu &tMcu                   [in]    MCU
	const TMixParam *ptMixParam    [in]    混音参数
    注    意:不设置本级
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetMixParam(const TMcu &tMcu, const TMixParam *ptMixParam)
    { 
		// 通过mcuid找到CItemGroup，修改CMt成员变量
		SetMixParam( tMcu.GetMcuIdx(), ptMixParam );
	}
	
    /*=============================================================================
    函 数 名:SetMixParam
    功    能:设置混音参数
    参    数:u8 byMcuId                         [in]    McuId
	const TMixParam *ptMixParam    [in]    混音参数
    注    意:不设置本级
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetMixParam(u16 byMcuId, const TMixParam *ptMixParam);
	// 通过mcuid找到CItemGroup，修改CMt成员变量
	
    /*=============================================================================
    函 数 名:GetMixParam
    功    能:获取混音参数
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:混音参数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    TMixParam& GetMixParam(const TMcu &tMcu) const
    { 
		// 通过mcuid找到CItemGroup，返回CMt成员变量
		return GetMixParam(tMcu.GetMcuIdx()); 
	}
	
    /*=============================================================================
    函 数 名:GetMixParam
    功    能:获取混音参数
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:混音参数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline TMixParam& GetMixParam(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:IsConfPartMix
    功    能:指定MCU是否部分混音
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfPartMix(const TMcu &tMcu) const
    { 
		// 通过mcuid找到CItemGroup，比较之
		return IsConfPartMix(tMcu.GetMcuIdx()); 
	}
	
    /*=============================================================================
    函 数 名:IsConfPartMix
    功    能:指定MCU是否部分混音
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline BOOL32 IsConfPartMix(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:IsConfEntireMix
    功    能:指定MCU是否全体混音
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfEntireMix(const TMcu &tMcu) const
    { 
		// 通过mcuid找到CItemGroup，比较之
		return IsConfEntireMix(tMcu.GetMcuIdx()); 
	}
	
    /*=============================================================================
    函 数 名:IsConfEntireMix
    功    能:指定MCU是否全体混音
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline BOOL32 IsConfEntireMix(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:IsConfNoMix
    功    能:指定MCU是否没有混音
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/08/04  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 IsConfNoMix(const TMcu &tMcu) const
    { return IsConfNoMix(tMcu.GetMcuIdx()); }
	
    /*=============================================================================
    函 数 名:IsConfNoMix
    功    能:指定MCU是否没有混音
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/08/04  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    inline BOOL32 IsConfNoMix(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:GetMixMember
    功    能:获取混音成员
    参    数:TMt *ptMt                          [out]       混音成员
	s32 &nMtNum                        [in/out]    数组大小
    注    意:包括下级, 只有终端, 不含MCU
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void GetMixMember(TMt *ptMt, s32 &nMtNum) ;
	
    /*=============================================================================
    函 数 名:GetLocalMixNum
    功    能:获取本级混音成员个数
    参    数:无
    注    意:包括下级MCU
    返 回 值:本级混音成员个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/08/07  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetLocalMixNum(void) const;
	
    /*=============================================================================
    函 数 名:GetSMcuMixNum
    功    能:获取下级MCU参与混音的终端个数
    参    数:const TMcu &tMcu                   [in]    MCU
    注    意:无
    返 回 值:终端个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetSMcuMixNum(const TMcu &tMcu) const
    { return GetSMcuMixNum(tMcu.GetMcuIdx()); }
	
    /*=============================================================================
    函 数 名:GetSMcuMixNum
    功    能:获取下级MCU参与混音的终端个数
    参    数:u8 byMcuId                         [in]    McuId
    注    意:无
    返 回 值:终端个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/13  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    s32 GetSMcuMixNum(u16 byMcuId) const;
	
    /*=============================================================================
    函 数 名:GetLocalConfIdx
    功    能:获取会议索引号
    参    数:无
    注    意:无
    返 回 值:会议索引号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/02  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    u8 GetLocalConfIdx(void)
    { 
		// conf自行处理
		return m_tConfAllMtInfo.m_tLocalMtInfo.GetConfIdx(); 
	}
	
	/*---------------------------------------------------------------------
	* 函 数 名：UpdateMtStatus
	* 功    能：通过外设状态更新当前的终端状态
	* 参数说明：PeriEqpStatus& tPeriEqpStatus	[in]	外设状态
				BOOL32 bReset = FALSE			[in]	是否重置
				* 返 回 值：
				* 修改记录：
				* 日期			版本		修改人		修改记录
				* 2010/12/28	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	void UpdateMtStatus( TPeriEqpStatus& tPeriEqpStatus, BOOL32 bReset = FALSE );
	
    /*=============================================================================
    函 数 名:GetMMcuSpeaker
    功    能:获取广播通道终端
    参    数:CMt &cMt                           [out]   广播通道终端
    注    意:会议中必须含有上级MCU
    返 回 值:广播通道有终端:TRUE, 广播通道没有终端:FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/11  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 GetMMcuSpeaker(CMt &cMt) const;
	
    /*=============================================================================
    函 数 名:GetMMcuSpeaker
    功    能:获取广播通道终端
    参    数:TMt &tMt                           [out]   广播通道终端
    注    意:会议中必须含有上级MCU
    返 回 值:广播通道有终端:TRUE, 广播通道没有终端:FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/11  4.0     王昊    创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 GetMMcuSpeaker(TMt &tMt) const;
	
    /*=============================================================================
    函 数 名:GetVideoToMMcuMt
    功    能:获取回传通道终端
    参    数:TMt &tMt                           [out]   回传通道终端
    注    意:会议中必须含有上级MCU
    返 回 值:回传通道有终端:TRUE, 回传通道没有终端:FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/08/11  4.0     王昊    创建
    2005/08/23  4.0     王昊    回传通道可能是VMP, 去掉参数为CMt的接口
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    BOOL32 GetVideoToMMcuMt(TMt &tMt) const;
	
    /*=============================================================================
    函 数 名:SetLastRateReqTime
    功    能:设置上一次发送请求所有终端码率的时间
    参    数:u32 dwTime         [in] 时间
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/01/10  4.0     顾振华  创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    void SetLastRateReqTime(u32 dwTime) { m_dwLastRateReqTime = dwTime; }
	
    /*=============================================================================
    函 数 名:GetLastRateReqTime
    功    能:获取上一次发送请求所有终端码率的时间
    参    数:
    注    意:
    返 回 值:
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/01/10  4.0     顾振华  创建
	2010/07/16	v4.1	牟兴茂	保留接口，重写所有代码
    =============================================================================*/
    u32 GetLastRateReqTime() const { return m_dwLastRateReqTime; }
	
	void SetApplySpeaker( TMt *pMtList, u32 dwMtNum );
	
    void AddApplySpeaker( TMt *pMtList, u32 dwMtNum );
	
    inline void SetPwdEntered(BOOL32 bEntered = TRUE) { m_bPwdEntered = bEntered; }
	
    inline BOOL32 IsPwdEntered() const { return m_bPwdEntered; }

	//设置会场预案请求标志 //modified by twb 2012.11.20
	inline void SetReqConfSchemaFlag(BOOL32 bReqed = TRUE ){ m_bReqConfSchema = bReqed; }

	//是否请求过会场预案 //modified by twb 2012.11.20
	inline BOOL32 IsReqedConfShema() const { return m_bReqConfSchema; }

	//是否可以使用会场预案
	inline void SetHaveSchemaFlag(BOOL32 bCanUse = TRUE) { m_bHaveSchema = bCanUse; }

	inline BOOL32 GetHaveSchemaFlag() const { return m_bHaveSchema; }
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetVisableIMcuByIndex
	* 功    能：从mcuindex的列表里面去获取mcu的指针
	* 参数说明：[in]wIndex 索引
	* 返 回 值：代表mcu节点的指针
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/11/18	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	IMcu* GetVisableIMcuByIndex( u16 wIndex, u16 &wMcuIdx ) const;
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetVisableIMcuByIndex
	* 功    能：从mcuindex的列表里面去获取mcu的指针
	* 参数说明：[in]wIndex 索引
	* 返 回 值：代表mcu节点的指针
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/11/18	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	IMcu* GetVisableIMcuByIndex( u16 wIndex ) const;
	
	/*---------------------------------------------------------------------
	* 函 数 名：HasSameRoot
	* 功    能：检测两个终端是否具备同一个根节点
	* 参数说明：[in] t1 第一个终端 
				[in] t2 第二个终端
				[out] wIndex	根节点的Index，代表mcu
				* 返 回 值：是否具备相同的根节点
				* 修改记录：
				* 日期			版本		修改人		修改记录
				* 2011/08/03	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL	HasSameRoot( TMt t1, TMt t2, u16& wIndex );
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetMtLevel
	* 功    能：获得一个TMt所在的层 在19200下是第1层，以此类推
	* 参数说明：[in] tMt 终端
				[out] wlevel 所在层
				* 返 回 值：是否找到
				* 修改记录：
				* 日期			版本		修改人		修改记录
				* 2011/08/03	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL	GetMtLevel( TMt tMt, u16& wLevel );
	BOOL	GetMtLevel( u16 wIndex, u16& wLevel );

	//  抢答/发言人列表
	vector<TMt>*    GetVecApplySpeaker(); 
	
	TMt	GetMtApplySpeaker(u8 byIdx);

	//  会议调度状态
	void SetBasicVCCStatus(CBasicVCCStatus& cBasicVCCStatus);
	CBasicVCCStatus* GetBasicVCCStatus();  

	//  锁定会议的用户名
	s8*	 GetLockUserName();
	void SetLockUsername(u8 byLockUserName[MAXLEN_PWD + 1]);

	//  锁定会议的会控IP
	void SetLockMcsIp(u32 dwValue);
	u32 GetLockMcsIp();

	//added by twb.2013.3.4 start

	// 更新画面合成器、终端状态
	BOOL32 UpdateVmpParam(u8 byEqpId, TVMPParam_25Mem* ptVmpParam, const s8* pchVmpAlias = NULL, u8 bReplaceEqpId = 255);
	// 清除画面合成器、终端状态
	BOOL32 RemoveVmpParam(u8 byEqpId);

	//更新画面合成器名称
	void  UpdateVmpEqpAlias(const s8* pchEqpAlias, u8 byEqpId, TplArray<TPeriEqpStatus>& tplVmpArray);

	// 检查下级MCU和其媒体源并增加状态
	BOOL32 CheckSMcuAndAddMtStatus(CMt &cMt, u8 byEqpId);
	// 检查下级MCU和其媒体源并清空状态
	BOOL32 CheckSMcuAndClearMtStatus(CMt &cMt, u8 byEqpId);
	// 替换终端状态
	void ReplaceMediaSrcMtVmpStatus(CMt& cMcu, CMt& cOldMediaMt, CMt& cNowMediaMt);
	// 获取广播的画面合成器
	u8 GetBrdstVmpEqpId();
	// 获取Vmp列表
	MAPVMPPARAMINFO* GetVmpList();
	
	// 获得双流终端
	CMt GetDualStreamMt();

private:
	void UpdateVmpStatus(TVMPParam_25Mem* ptVmpParam, u8 byEqpId);
	void RemoveVmpStatus(TVMPParam_25Mem* pTVMPParam_25Mem, u8 byEqpId);

	void UpdateVmpStatus(CMt& cMt, u8 byEqpId);
	void RemoveVmpStatus(CMt& cMt, u8 byEqpId);
	//added by twb.2013.3.4 end
    
private:
	//画面合成器信息
	map<u8, TVmpParamInfo> m_mapVmpInfo;

	u32             m_dwLocalMcuIp;                     //  本级MCUIP
    CMt             m_cLocalMcu;                        //  本级MCU

	u32             m_dwGetInitValue;   //  用于记录是否得到到初始的状态，主要是连接MCU时用到

	u8              m_byLockBySelf;     //  会议是否被本会控锁定

	u32             m_dwLockMcsIP;      //  锁定会议的会控IP
	s8              m_abyLockUserName[MAXLEN_PWD + 1];  //  锁定会议的用户名

	BOOL32          m_bPwdEntered;      //  记录用户是否输入过密码，主要是在MCU要求用户输入密码后记录
	BOOL32			m_bReqConfSchema;	//  记录会议是否已经请求过会场预案 modified by twb 2012.11.20

	BOOL32			m_bHaveSchema;		//	会议是否有预案

	u32             m_dwSN;             //  会议序号

	CBasicVCCStatus m_cVcsConfStatus;   //  会议调度状态

	vector<TMt>     m_vctApplySpeaker;  //  抢答/发言人列表

    TEqp            m_tRecEqp;          //  会议录像设备
    TEqp            m_tPlayEqp;         //  会议放像设备
	
    u32             m_dwLastRateReqTime;//  记录上一次发送请求所有终端码率的时间
public:
	void SetHduBatchPollState(THduPollSchemeInfo& tHduPollSchemeInfo)
	{
		m_tHduPollSchemeInfo = tHduPollSchemeInfo;
	}
	THduPollSchemeInfo& GetHduBatchPollStatus()
	{
		return m_tHduPollSchemeInfo;
	}
private:
	THduPollSchemeInfo m_tHduPollSchemeInfo;	//电视墙批量轮询信息
	
	//预案信息
public:
	vector <TVCSSchema*> m_vctpTVcsSchema;
	//TVCSSchema* m_ptCurSchema;
	void GetSchema( const s8* strSchemaName, TVCSSchema ** pptSchema )
	{
		
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			TVCSSchema* ptSchema = m_vctpTVcsSchema[ nIdx ];
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, strSchemaName ) == 0 )
			{
				*pptSchema = m_vctpTVcsSchema[ nIdx ];
				return;
			}
		}
	}
	
	TVCSSchema* GetSchema( const s8* strSchemaName )
	{
		TVCSSchema* ptSchema = NULL;
		
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, strSchemaName ) == 0 )
			{
				ptSchema = m_vctpTVcsSchema[ nIdx ];
				break;
			}
		}
		
		return ptSchema;
	}
	
	BOOL32 IsSchemaExist( const s8* strSchemaName )
	{		
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, strSchemaName ) == 0 )
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	
	void PopUpSchema( TVCSSchema *ptSchema )
	{
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, ptSchema->m_pbySchemaName ) == 0 )
			{
				//这里要先删除指针
				delete m_vctpTVcsSchema[ nIdx ];
				m_vctpTVcsSchema[ nIdx ] = NULL;
				m_vctpTVcsSchema.erase( m_vctpTVcsSchema.begin() + nIdx );
			}
		}
	}
	
	void SubstituteSchemaName( s8* pbyOldName, s8* pbyNewName )
	{
		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, pbyOldName ) == 0 )
			{
				strcpy( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, pbyNewName );
				break;
			}
		}
	}
	
	void SubstituteSchema( TVCSSchema * const ptSchema )
	{
		BOOL32 bFind = FALSE;

		for ( u32 nIdx = 0; nIdx< m_vctpTVcsSchema.size(); nIdx ++ )
		{
			if ( strcmp( m_vctpTVcsSchema[ nIdx ]->m_pbySchemaName, ptSchema->m_pbySchemaName ) == 0 )
			{
				bFind = TRUE;
				//这里要先删除指针
				if ( ptSchema != m_vctpTVcsSchema[ nIdx ] )
				{
					delete m_vctpTVcsSchema[ nIdx ];
					m_vctpTVcsSchema[ nIdx ] = ptSchema;
				}
				
			}
		}

		if (!bFind)
		{
			m_vctpTVcsSchema.push_back( ptSchema );
		}
	}
public:
	//added by spriner 20100518 下级mcu支持多回传的列表
	//这里保存的是支持多回传的mcu ID
	TMcu m_atMcu[MAXNUM_CONF_MT];//支持多回传的下级mcu
	//改变是否支持多回传的mcuID byCascade 为0的时候删除，为1的时候添加
	/*---------------------------------------------------------------------
	* 函 数 名：SetMcuMutilChannelStatus
	* 功    能：设置mcu是否是多回传
	* 参数说明：[in] wMcuIdx mcu的索引 [in]bOpen 是否打开
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/10/11	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	void SetMcuMutilChannelStatus( const TMt &tMt, BOOL32 bOpen );
	
	/*---------------------------------------------------------------------
	* 函 数 名：GetMcuMutilChannelStatus
	* 功    能：获得mcu的多回传状态
	* 参数说明：
	* 返 回 值：
	* 修改记录：
	* 日期			版本		修改人		修改记录
	* 2010/10/11	v1.0		牟兴茂		创建
	----------------------------------------------------------------------*/
	BOOL32 GetMcuMutilChannelStatus( const TMt &tMt );
	BOOL32 GetMcuMutilChannelStatus( u16 wMcuIdx );
	
	void PrintBasicVcsStatus();

protected:
	// 画面合成单通道轮询
	TVmpPollParam m_tVmpPollParam;
public:
	TVmpPollParam& GetVmpPollParam(){ return m_tVmpPollParam; }
	void SetVmpPollParam( TVmpPollParam& tVmpPollParam )
	{
		m_tVmpPollParam = tVmpPollParam;
	}
};

//CConfTable以数组形式保存连接MCU上所有即时/预约会议列表
class CConfTable
{
public:
    CConfTable();
    CConfTable(const CConfTable &cConfTable);
	
    ~CConfTable() {}
	
    /*=============================================================================
    函 数 名:operator=
    功    能:赋值
    参    数:const CConfTable &cConfTable       [in]    会议列表
    注    意:无
    返 回 值:会议列表
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    const CConfTable& operator=(const CConfTable &cConfTable);
	
    /*=============================================================================
    函 数 名:operator[]
    功    能:根据会议号索引出具体会议
    参    数:const CConfId &cConfId             [in]    会议号
    注    意:无
    返 回 值:会议
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    CConf& operator[](const CConfId &cConfId);
	
    /*=============================================================================
    函 数 名:operator[]
    功    能:根据会议号索引出具体会议
    参    数:u32 dwConfSN                       [in]    会议索引号
    注    意:无
    返 回 值:会议
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    CConf& operator[](u32 dwConfSN);
	
    /*=============================================================================
    函 数 名:GetAt
    功    能:获取会议
    参    数:const CConfId &cConfId             [in]    会议号
	CConf &cConf                       [out]   会议
    注    意:无
    返 回 值:会议存在: TRUE; 不存在: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 GetAt(const CConfId &cConfId, CConf &cConf) const;
	
    /*=============================================================================
    函 数 名:GetAt
    功    能:获取会议
    参    数:u32 dwConfSN                       [in]    会议索引号
	CConf &cConf                       [out]   会议
    注    意:无
    返 回 值:会议存在: TRUE; 不存在: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 GetAt(u32 dwConfSN, CConf &cConf) const;
	
    /*=============================================================================
    函 数 名:SetAt
    功    能:设置会议表
    参    数:CConf &cConf                       [in]    会议
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    void SetAt(CConf &cConf);
	
    /*=============================================================================
    函 数 名:operator-=
    功    能:从会议表中删除一个会议
    参    数:const CConfId &cConfId             [in]    会议号
    注    意:无
    返 回 值:会议列表
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    CConfTable& operator-=(const CConfId &cConfId);
	
    /*=============================================================================
    函 数 名:IsExist
    功    能:判断会议在会议表中是否存在
    参    数:const CConfId &cConfId             [in]    会议号
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsExist(const CConfId &cConfId) const;
	
    /*=============================================================================
    函 数 名:IsExist
    功    能:判断会议在会议表中是否存在
    参    数:u32 dwConfSN                        [in]    会议索引号
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsExist(u32 dwConfSN) const;
	
    /*=============================================================================
    函 数 名:IsEmpty
    功    能:判断会议表中是否为空
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsEmpty(void) const { return (m_nConfNum == 0); }
	
    /*=============================================================================
    函 数 名:ClearContent
    功    能:清除会议表中的所有项
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
	void ClearContent(void);
	
    /*=======================================================================
    函 数 名:GetNextNoInitValueConfId
    功    能:得到下一个还没有得到初始信息的会议ID, 初始信息包括独享、
	终端状态列表、别名、下级MCU锁定状态
    参    数:无
    注    意:无
    返 回 值:会议号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =======================================================================*/
    const CConfId GetNextNoInitValueConfId(void);
	
    /*=======================================================================
    函 数 名:GetNewConfSN
    功    能:得到会议的序列号,每次调用都会得到一个新的号码
    参    数:无
    注    意:无
    返 回 值:新的序列号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =======================================================================*/
    static u32 GetNewConfSN(void) { return ++s_dwConfSN; }
	
    /*=============================================================================
    函 数 名:GetAllConfId
    功    能:获取所有会议号
    参    数:CConfId *pcConfId                  [out]   会议号数组
	s32 nNum                           [in/out]    数组大小/会议个数
    注    意:无论成功与否, 数组大小都会置成会议个数
    返 回 值:数组大小小于会议个数: FALSE; 成功: TRUE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 GetAllConfId(CConfId *pcConfId, s32 &nNum);
	
    /*=============================================================================
    函 数 名:GetAllConfSN
    功    能:获取所有会议索引号
    参    数:u32 *pdwConfSN                     [out]   会议索引号数组
	s32 nNum                           [in/out]    数组大小/会议个数
    注    意:无论成功与否, 数组大小都会置成会议个数
    返 回 值:数组大小小于会议个数: FALSE; 成功: TRUE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/06/03  4.0     王昊    创建
    =============================================================================*/
    BOOL32 GetAllConfSN(u32 *pdwConfSN, s32 nNum);
	
public:
    CConf       m_acConf[MAXNUM_MCU_CONF];          //  所有会议列表
    s32         m_nConfNum;                         //  会议个数
	
private:
    u8          m_abyConfExist[MAXNUM_MCU_CONF];    //  数组里是否有会议
    static u32  s_dwConfSN;                         //  会议递增序号

	CCriticalSection	m_criLock;					//	赋值锁
};

// 会议调度组
class CVcsGroup
{
public:
    CVcsGroup()
    {
        ZeroMemory( &m_tVcsGroupInfo, sizeof(m_tVcsGroupInfo) );
    }
	
    void AddEntry( TVCSEntryInfo &tEntry )
    {
        m_vctVcsEntry.push_back( tEntry );
        m_tVcsGroupInfo.dwEntryNum = m_vctVcsEntry.size();
    }
    void DelEntry( TVCSEntryInfo &tEntry )
    {
		vector<TVCSEntryInfo>::iterator itr = m_vctVcsEntry.begin();
		for ( ; itr != m_vctVcsEntry.end();  )
		{
			if ( strcmp( tEntry.achEntryName, (*itr).achEntryName ) == 0 && tEntry.byEntryType == (*itr).byEntryType )
			{
				m_vctVcsEntry.erase( itr );
				continue;
			}
            itr++;
		}
		
        m_tVcsGroupInfo.dwEntryNum = m_vctVcsEntry.size();
    }
    inline CVcsGroup& operator=( const CVcsGroup &cGroup )
    {
        strcpy( m_tVcsGroupInfo.achGroupName, cGroup.m_tVcsGroupInfo.achGroupName );
        m_tVcsGroupInfo.dwEntryNum = cGroup.m_tVcsGroupInfo.dwEntryNum;
        m_vctVcsEntry = cGroup.m_vctVcsEntry;
		
        return (*this);
    }
	
    TVCSGroupInfo   m_tVcsGroupInfo;
    vector<TVCSEntryInfo>   m_vctVcsEntry;
};

// 添加完成
// --------------------------------------------------------------------

// 录像机配置、专门用于VCS会议模板的录像机
typedef struct tagRecDeploy
{
	TEqp	tEqp; //设备
	TRecStartPara tRecStartPara; // 
}TRecDeploy,*PTRecDeploy;

typedef struct tagMonitorUnion
{
	u32 dwIp;
	u32	dwBasePort;
}TMonitorUnion,*PTMonitorUnion; 

//会议模板/预约会议
class CTemSchConf : public TConfInfo
{
public:
    CTemSchConf() : m_tplMt(0, MTTABLE_STEP), m_bExist(FALSE), m_bIsHasVmpSubChnnl(FALSE), m_tplSubMcu(0, MTTABLE_STEP), m_wPollTime(6)
		,m_byAutoFlag(0){}
    CTemSchConf(const CTemSchConf &cTemSchConf);
    ~CTemSchConf();
	
    CTemSchConf& operator=(const CTemSchConf &cTemSchConf);
	
    /*=============================================================================
    函 数 名:SetNull
    功    能:会议模板置空
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/22  4.0     王昊    创建
    =============================================================================*/
    inline void SetNull(void);
	
    /*=============================================================================
    函 数 名:IsExist
    功    能:会议是否存在
    参    数:无
    注    意:无
    返 回 值:存在:TRUE; 不存在: FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/22  4.0     王昊    创建
    =============================================================================*/
    BOOL32 IsExist(void) const { return m_bExist; }
	
    /*=============================================================================
    函 数 名:SetConfInfo
    功    能:设置会议信息
    参    数:const TConfInfo &tConfInfo         [in]    会议信息
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/22  4.0     王昊    创建
    =============================================================================*/
    inline void SetConfInfo(const TConfInfo &tConfInfo)
    {
        *dynamic_cast<TConfInfo*>(this) = tConfInfo;
        m_bExist = TRUE;
    }
	
    /*=============================================================================
    函 数 名:PrintInfo
    功    能:打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/23  4.0     王昊    创建
    =============================================================================*/
    void PrintInfo(void);
	
	
    /*=============================================================================
    函 数 名:GetAutoFlag
    功    能:获取自动设置参数
    参    数:无
    注    意: vcs自动组呼
    返 回 值: 自动组呼 = VCS_GROUPCHAIRMAN_MODE 
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/04/20  4.0     过全    创建
    =============================================================================*/
	u8   GetAutoFlag(){ return m_byAutoFlag; }
	
	/*=============================================================================
    函 数 名:SetAutoFlag
    功    能:u8 byFlag 获取自动设置参数
    参    数:自动组呼 = VCS_GROUPCHAIRMAN_MODE 
    注    意: vcs自动组呼
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2011/04/20  4.0     过全    创建
    =============================================================================*/
	void SetAutoFlag( u8 byFlag ) { m_byAutoFlag = byFlag; }
	
public:
	BOOL32                  m_bIsHasVmpSubChnnl;
    BOOL32                  m_bExist;       //会议模板是否存在
    TplArray<TAddMtInfo>    m_tplMt;        //终端列表
	TMtAlias				m_tMtAliasBack; //本地备份终端   
    // 顾振华@2006.02.14 电视墙模板支持多个电视墙
    TMultiTvWallModule      m_tTWModules;  //电视墙模板
	TMcsVmpModule           m_tVmpModuleEx;
	THduVmpTotalEx          m_tHduVmpTotalEx;
	
    //准备未来支持多画面电视墙模版
    //TVmpModule            mtVmpTwModule;  /多画面电视墙模版
	
    // 会议调度专用
    TplArray<TAddMtInfo>    m_tplSubMcu;    // 下级MCU列表
    vector<CVcsGroup>       m_vctGroup;     // 会议调度组列表
    THDTvWall               m_tHdTvwall;    // 高清终端作为电视墙
    vector<THduModChnlInfo> m_vctHduModChn; // Hdu通道信息
    
    u16                     m_wPollTime;    //轮询时间
	
	TMonitorUnion			m_tMonitorUnion;//监控联动参数
	TRecDeploy				m_tRecDeploy;	//录像机参数

	TConfInfoEx				m_tConfInfoEx;	//V4R7新增会议信息

private:
	u8                      m_byAutoFlag;   // 7:自动组呼
};

//预约会议/会议模板列表
//注意: 存储未必连续, 查找需要循环到底
class CConfTemSchTable
{
public:
    CConfTemSchTable() {};
    CConfTemSchTable(const CConfTemSchTable &cConfTemSchTable);
    ~CConfTemSchTable() {};
	
    CConfTemSchTable& operator=(const CConfTemSchTable &cConfTemSchTable);
	
    /*=============================================================================
    函 数 名:ReleaseConf
    功    能:清空指定会议
    参    数:const CConfId &cConfId             [in]    会议号
	u8 byTakeMode  [in]    CONF_TAKEMODE_SCHEDULED/CONF_TAKEMODE_TEMPLATE
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/23  4.0     王昊    创建
    =============================================================================*/
    BOOL32 ReleaseConf(const CConfId &cConfId, u8 byTakeMode);
	
    /*=============================================================================
    函 数 名:PrintInfo
    功    能:打印
    参    数:u8 byTakeMode      [in]    CONF_TAKEMODE_SCHEDULED/CONF_TAKEMODE_TEMPLATE
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/23  4.0     王昊    创建
    =============================================================================*/
    void PrintInfo(u8 byTakeMode);
	
    /*=============================================================================
    函 数 名:Clear
    功    能:清空
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/11/24  4.0     王昊    创建
    =============================================================================*/
    void Clear(void);
	
public:
    CTemSchConf m_acConfTemplate[MAXNUM_MCU_CONF];  //会议模板数组
    CTemSchConf m_acScheduleConf[MAXNUM_MCU_CONF];  //预约会议数组
};

//CUserTable用户管理，用一个TplArray存放所用的用户信息
class CUserTable
{
public:
    CUserTable() { Clear(); }
	
    /*=============================================================================
    函 数 名:CUserTable
    功    能:拷贝构造函数
    参    数:const CUserTable &cUserTable       [in]    要复制的用户列表
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	CUserTable(const CUserTable &cUserTable);
	
    virtual ~CUserTable() { Clear(); }
	
    /*=============================================================================
    函 数 名:IsEmpty
    功    能:判断用户表是否为空
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 IsEmpty(void) const;
	
    /*=============================================================================
    函 数 名:IsExist
    功    能:判断给定用户是否在表中存在
    参    数:LPCTSTR pszUserName                [in]    用户名
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 IsExist( const s8* pszUserName ) const;
	
    /*=============================================================================
    函 数 名:operator=
    功    能:重载运算符=
    参    数:const CUserTable &cUserTable       [in]    用于赋值的用户表
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    const CUserTable& operator=(const CUserTable &cUserTable);
	
    /*=============================================================================
    函 数 名:operator[]
    功    能:重载运算符[]
    参    数:s32 nIndex                         [in]    用户在列表中的索引
    注    意:无
    返 回 值:const CUserFullInfo&   返回指定的用户信息
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	const CExUsrInfo& operator[](s32 nIndex) const;
	
	const CExUsrInfo& GetUserAt(s32 nIndex) const;
	
    /*=============================================================================
    函 数 名:GetUser
    功    能:取得指定名称的用户信息
    参    数:LPCTSTR pszUserName                [in]    要获取的用户的名字
	CUserFullInfo * const pcUserInfo   [out]   填充的用户信息
    注    意:无
    返 回 值:成功返回TRUE, 用户不存在读取失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 GetUser( const s8* pszUserName, CExUsrInfo * const pcUserInfo ) const;
	
    /*=============================================================================
    函 数 名:SetUser
    功    能:向列表中增加用户，如果用户不存在则直接加入，如果用户已存在，则更新用户信息
    参    数:const CUserFullInfo &cUserInfo     [in]    要加入的用户信息
    注    意:无
    返 回 值:TRUE表示成功，FALSE表示失败
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 SetUser(const CExUsrInfo &cUserInfo);
	
    /*=============================================================================
    函 数 名:DelUser
    功    能:删除指定用户
    参    数:LPCTSTR lptrUserName               [in]    要删除的用户
    注    意:无
    返 回 值:成功返回TRUE，用户不存在或失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 DelUser(const s8* lptrUserName);
	
    /*=============================================================================
    函 数 名:GetUserNum
    功    能:得到当前用户表中的用户个数
    参    数:无
    注    意:无
    返 回 值:用户的个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	s32 GetUserNum(void) const;
	
    /*=============================================================================
    函 数 名:GetAllUser
    功    能:得到表中所有用户的信息
    参    数:CUserFullInfo *pcUserArray         [out]   用户信息数组
	s32 &nNum                          [in/out]传入的数组大小/实际填充的用户个数
    注    意:无
    返 回 值:成功返回TRUE，失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 GetAllUser(CExUsrInfo *pcUserArray, s32 &nNum);
	
    /*=============================================================================
    函 数 名:Clear
    功    能:清空整个列表
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    inline void Clear(void) { m_tplUserFullInfo.Clear(); }
	
    /*=============================================================================
    函 数 名:PrintInfo
    功    能:将所有的用户信息打印出来
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    void PrintInfo(void);
	
protected:
	TplArray<CExUsrInfo> m_tplUserFullInfo;
	
	//#ifdef _DEBUG
	//public:
	//    /*=============================================================================
	//    函 数 名:Dump
	//    功    能:打印本类的成员调试信息
	//    参    数:CDumpContext &dc                   [in]    打印头
	//    注    意:无
	//    返 回 值:无
	//    -------------------------------------------------------------------------------
	//    修改纪录:
	//    日      期  版本    修改人  修改内容
	//    2005/04/26  4.0     王昊    创建
	//    =============================================================================*/
	//    virtual void Dump(CDumpContext &dc) const;
	//#endif  //  _DEBUG
};

class CVcsUserTable
{
public:
    CVcsUserTable() { Clear(); }
	
    /*=============================================================================
    函 数 名:CUserTable
    功    能:拷贝构造函数
    参    数:const CUserTable &cUserTable       [in]    要复制的用户列表
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	CVcsUserTable(const CVcsUserTable &cUserTable);
	
    virtual ~CVcsUserTable() { Clear(); }
	
    /*=============================================================================
    函 数 名:IsEmpty
    功    能:判断用户表是否为空
    参    数:无
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 IsEmpty(void) const;
	
    /*=============================================================================
    函 数 名:IsExist
    功    能:判断给定用户是否在表中存在
    参    数:LPCTSTR pszUserName                [in]    用户名
    注    意:无
    返 回 值:BOOL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 IsExist(const s8* pszUserName) const;
	
    /*=============================================================================
    函 数 名:operator=
    功    能:重载运算符=
    参    数:const CUserTable &cUserTable       [in]    用于赋值的用户表
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    const CVcsUserTable& operator=(const CVcsUserTable &cUserTable);
	
    /*=============================================================================
    函 数 名:operator[]
    功    能:重载运算符[]
    参    数:s32 nIndex                         [in]    用户在列表中的索引
    注    意:无
    返 回 值:const CUserFullInfo&   返回指定的用户信息
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	const CVCSUsrInfo& operator[](s32 nIndex) const;
	
	const CVCSUsrInfo& GetUserAt(s32 nIndex) const;
	
    /*=============================================================================
    函 数 名:GetUser
    功    能:取得指定名称的用户信息
    参    数:const s8* pszUserName                [in]    要获取的用户的名字
	CUserFullInfo * const pcUserInfo   [out]   填充的用户信息
    注    意:无
    返 回 值:成功返回TRUE, 用户不存在读取失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 GetUser( const s8* pszUserName, CVCSUsrInfo * const pcUserInfo) const;
	
    /*=============================================================================
    函 数 名:SetUser
    功    能:向列表中增加用户，如果用户不存在则直接加入，如果用户已存在，则更新用户信息
    参    数:const CUserFullInfo &cUserInfo     [in]    要加入的用户信息
    注    意:无
    返 回 值:TRUE表示成功，FALSE表示失败
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 SetUser(const CVCSUsrInfo &cUserInfo);
	
    /*=============================================================================
    函 数 名:DelUser
    功    能:删除指定用户
    参    数:const s8* lptrUserName               [in]    要删除的用户
    注    意:无
    返 回 值:成功返回TRUE，用户不存在或失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 DelUser( const s8* lptrUserName );
	
    /*=============================================================================
    函 数 名:GetUserNum
    功    能:得到当前用户表中的用户个数
    参    数:无
    注    意:无
    返 回 值:用户的个数
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	s32 GetUserNum(void) const;
	
    /*=============================================================================
    函 数 名:GetAllUser
    功    能:得到表中所有用户的信息
    参    数:CUserFullInfo *pcUserArray         [out]   用户信息数组
	s32 &nNum                          [in/out]传入的数组大小/实际填充的用户个数
    注    意:无
    返 回 值:成功返回TRUE，失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 GetAllUser(CVCSUsrInfo *pcUserArray, s32 &nNum);
	
    /*=============================================================================
    函 数 名:Clear
    功    能:清空整个列表
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    inline void Clear(void) { m_tplUserFullInfo.Clear(); }
	
    /*=============================================================================
    函 数 名:PrintInfo
    功    能:将所有的用户信息打印出来
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    void PrintInfo(void);
	
protected:
	TplArray<CVCSUsrInfo> m_tplUserFullInfo;
	
	//#ifdef _DEBUG
	//public:
	//    /*=============================================================================
	//    函 数 名:Dump
	//    功    能:打印本类的成员调试信息
	//    参    数:CDumpContext &dc                   [in]    打印头
	//    注    意:无
	//    返 回 值:无
	//    -------------------------------------------------------------------------------
	//    修改纪录:
	//    日      期  版本    修改人  修改内容
	//    2005/04/26  4.0     王昊    创建
	//    =============================================================================*/
	//    virtual void Dump(CDumpContext &dc) const;
	//#endif  //  _DEBUG
};

//用来管理用户组的信息,保存为一个tplarray
class CUserGroupTable
{
public:
	CUserGroupTable();
	virtual ~CUserGroupTable();
    /*函数名： CUserGroupTable
    功    能： 拷贝构造函数
    参    数： const CUserGroupTable& rhs
    返 回 值： 
    说    明： */
	CUserGroupTable(const CUserGroupTable& rhs);
    /*函数名： operator=
    功    能： 赋值函数
    参    数： const CUserGroupTable &rhs
    返 回 值： CUserGroupTable& 
    说    明： */
	CUserGroupTable& operator=(const CUserGroupTable &rhs);
    /*函数名： AddUserGroup
    功    能： 增加一个用户组信息,如果用户组信息已经存在的话就覆盖
    参    数： const TUsrGrpInfo& tUserGroupInfo
    返 回 值： BOOL     如果行为是增加,返回TRUE, 如果行为是覆盖,返回FALSE
    说    明： */
	BOOL AddUserGroup( const TUsrGrpInfo& tUserGroupInfo );
    /*函数名： DelUserGroup
    功    能： 删除一个用户组的信息
    参    数： u8 byGroupId		[in] 用户组的ID
    返 回 值： BOOL     成功返回TRUE,失败返回FALSE
    说    明： */
	BOOL DelUserGroup( u8 byGroupId );
    /*函数名： GetUserGroupNum
    功    能： 获取用户组信息的数目
    参    数： 
    返 回 值： s32		用户组信息的数目
    说    明： */
	s32 GetUserGroupNum() const
	{
		return m_tplUserGroupInfo.Size();
	}
    /*函数名： ModifyUserGroup
    功    能： 修改用户组信息
    参    数： const TUsrGrpInfo& tUserGroupInfo	[in]用户组信息
    返 回 值： BOOL			如果修改成功就返回TRUE,否则返回False
    说    明： */
	BOOL ModifyUserGroup( const TUsrGrpInfo& tUserGroupInfo );
    /*函数名： GetUserGroup
    功    能： 根据用户组ID返回用户组信息
    参    数： u8 byGroupId			[in]用户组ID
    返 回 值： const TUsrGrpInfo&		用户组信息
    说    明： */
	const TUsrGrpInfo& GetUserGroup( u8 byGroupId ) const;
    /*函数名： GetAllUserGroup
    功    能： 返回全部的用户组信息
    参    数：  TUsrGrpInfo* ptUserGroup			[in/out]传入的数组的起始地址
	s32& nNum							[in/out]传入数组的大小/传出实际返回的大小
    返 回 值： BOOL			如果返回了全部信息，则为True，否则为False
    说    明： */
	BOOL GetAllUserGroup( TUsrGrpInfo* ptUserGroup, s32& nNum ) const;
    /*函数名： GroupClear
    功    能： 用户组信息清空
    参    数： 
    返 回 值： void 
    说    明： */
	void GroupClear() { m_tplUserGroupInfo.Clear(); } 
    /*函数名： GroupIsExist
    功    能： 指定用户组ID的用户组信息是否存在
    参    数： u8 byGroupId			[in]用户组的ID
    返 回 值： BOOL 
    说    明： */
	BOOL GroupIsExist( u8 byGroupId ) const;
    /*函数名： GetUserGroupAt
    功    能： 根据在tplarray的索引来返回用户组的信息
    参    数： u32 dwIndex			[in]索引信息
    返 回 值： const TUsrGrpInfo&	用户组信息
    说    明： */
	const TUsrGrpInfo& GetUserGroupAt( u32 dwIndex ) const;
    /*函数名： operator[]
    功    能： 重载[]下标运算符，和GetUserGroupAt()的功能一致
    参    数： s32 dwIndex			[in]索引信息
    返 回 值： TUsrGrpInfo&			用户组信息
    说    明： */
	TUsrGrpInfo& operator[](s32 dwIndex);
    /*函数名： operator[]
    功    能： 重载[]下标运算符，和GetUserGroupAt()的功能一致(const 版本)
    参    数： s32 dwIndex			[in]索引信息
    返 回 值： const TUsrGrpInfo&	用户组信息
    说    明： */
	const TUsrGrpInfo& operator[](s32 dwIndex )const ; 
	
    /*函数名： GrpPrintInfo
    功    能： 打印内部的信息
    参    数： void
    返 回 值： void 
    说    明： */
	void GrpPrintInfo( void ) const;
	
protected:
	TplArray<TUsrGrpInfo>	m_tplUserGroupInfo;		//保存用户组列表
};

//用来统一管理用户和用户组的信息,严格的来说是属于has a的关系, 这里使用
//Mutil-inheritance 是为了直接使用它们的方法
class CUserManage : public CUserTable,
public CUserGroupTable

{
public:
	CUserManage();
	~CUserManage();
    /*函数名： GetGroupAllUser
    功    能： 根据用户组ID返回这个用户组里面所有用户的信息
    参    数：  u8 byGrpId				[in]用户组ID信息
	CExUsrInfo *pcUserInfo	[in/out]用户信息指针
	s32 &nNum				[in/out]传入数组指针的大小/实际返回的数目
    返 回 值： BOOL		得到全部信息返回True,如果传入的数组太小,导致返回部分信息则为FALSE
    说    明： */
	BOOL GetGroupAllUser( u8 byGrpId, CExUsrInfo *pcUserInfo, s32 &nNum ) const ;
    /*函数名： CUserManage
    功    能： 拷贝构造函数
    参    数： const CUserManage& rhs
    返 回 值： 
    说    明： */
	CUserManage( const CUserManage& rhs );
    /*函数名： operator=
    功    能： 重载赋值运算符
    参    数： const CUserManage& rhs
    返 回 值： CUserManage& 
    说    明： */
	CUserManage& operator=(const CUserManage& rhs);
	
    /*函数名： Print
    功    能： 打印内部信息
    参    数： void
    返 回 值： void 
    说    明： */
	void Print(void) const; 
	
private:
/*函数名： Init
功    能： 初始化信息
参    数： const CUserManage& rhs		[in]另外的一个用户管理组信息
返 回 值： void 
    说    明： */
	void Init(const CUserManage& rhs);
};

class CVcsUserManage : public CVcsUserTable,
public CUserGroupTable

{
public:
	CVcsUserManage();
	~CVcsUserManage();
    /*函数名： GetGroupAllUser
    功    能： 根据用户组ID返回这个用户组里面所有用户的信息
    参    数：  u8 byGrpId				[in]用户组ID信息
	CExUsrInfo *pcUserInfo	[in/out]用户信息指针
	s32 &nNum				[in/out]传入数组指针的大小/实际返回的数目
    返 回 值： BOOL		得到全部信息返回True,如果传入的数组太小,导致返回部分信息则为FALSE
    说    明： */
	BOOL GetGroupAllUser( u8 byGrpId, CVCSUsrInfo *pcUserInfo, s32 &nNum ) const ;
    /*函数名： CUserManage
    功    能： 拷贝构造函数
    参    数： const CUserManage& rhs
    返 回 值： 
    说    明： */
	CVcsUserManage( const CVcsUserManage& rhs );
    /*函数名： operator=
    功    能： 重载赋值运算符
    参    数： const CUserManage& rhs
    返 回 值： CUserManage& 
    说    明： */
	CVcsUserManage& operator=(const CVcsUserManage& rhs);
	
    /*函数名： Print
    功    能： 打印内部信息
    参    数： void
    返 回 值： void 
    说    明： */
	void Print(void) const; 
	
private:
/*函数名： Init
功    能： 初始化信息
参    数： const CUserManage& rhs		[in]另外的一个用户管理组信息
返 回 值： void 
    说    明： */
	void Init(const CVcsUserManage& rhs);
};

//CPeriTable将MCU外设统一保存为一个TplArray
class CPeriTable
{
public:
    CPeriTable();
	
    /*=============================================================================
    函 数 名:ClearContent
    功    能:清除所有外设数据
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	void ClearContent(void);
	
    /*=============================================================================
    函 数 名:SetAt
    功    能:加入或刷新外设状态
    参    数:const TPeriEqpStatus &tPeriEqpStatus   [in]    外设状态
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	void SetAt(const TPeriEqpStatus &tPeriEqpStatus);
	
    /*=============================================================================
    函 数 名:GetAt
    功    能:获取指定外设状态
    参    数:const TEqp &tEqp                   [in]    指定外设
	TPeriEqpStatus &tPeriEqpStatus     [out]   外设状态
    注    意:无
    返 回 值:成功返回TRUE, 失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
	BOOL32 GetAt(const TEqp &tEqp, TPeriEqpStatus &tPeriEqpStatus) const;
	
    /*=============================================================================
    函 数 名:SetEqpIP
    功    能:设置外设IP
    参    数:const TEqp &tEqp                   [in]    指定外设
	s32 dwIP                           [in]    外设的IP
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    void SetEqpIP(const TEqp &tEqp, u32 dwIP);
	
    /*=============================================================================
    函 数 名:GetEqpIP
    功    能:查找外设的IP
    参    数:const TEqp &tEqp                   [in]    指定外设
    注    意:无
    返 回 值:外设IP
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/04/26  4.0     王昊    创建
    =============================================================================*/
    u32 GetEqpIP(const TEqp &tEqp) const;
	
	
    /*函数名： SetDcsIP
    功    能： 设置DCS的IP地址
    参    数：  const TEqp &tEqp  [in] DCS的终端标示
	u32 dwIP			  [in] Dcs的IP地址
    返 回 值： void 
    说    明： */
	void SetDcsIP( const TEqp &tEqp, u32 dwIP );
    /*函数名： GetDcsIP
    功    能： 获得dcs的IP地址 
    参    数： const TEqp &tEqp
    返 回 值： u32 
    说    明： */
	u32 GetDcsIP( const TEqp &tEqp );
    /*函数名： SetDcsStatus
    功    能： 设置DCS的状态
    参    数： const TPeriDcsStatus &tDcsStatus  [in] DCS状态
    返 回 值： void 
    说    明： */
	void SetDcsStatus( TPeriDcsStatus &tDcsStatus );
    /*函数名： GetDcsStatus
    功    能： 获取DCS的状态 
    参    数：  const TEqp &tEqp				[in] DCS终端标识
	TPeriEqpStatus &tDcsStatus		[out] Dcs的状态
    返 回 值： BOOL
    说    明： */
	BOOL GetDcsStatus( const TEqp &tEqp, TPeriDcsStatus &tDcsStatus );
	
    /*=============================================================================
    函 数 名:PrintInfo
    功    能:将所有的外设打印出来
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/05/19  4.0     王昊    创建
    =============================================================================*/
    void PrintInfo(void) const;
	
public:
    u32     m_adwEqpIP[MAXNUM_MCU_PERIEQP];     //  外设IP
	u32		m_adwDcsIP[MAXNUM_MCU_DCS];
	
    TplArray<TPeriEqpStatus> m_tplRecorder;     //  保存当前MCU所有录放像机状态
    TplArray<TPeriEqpStatus> m_tplMixer;        //  保存当前MCU所有混音器状态
    TplArray<TPeriEqpStatus> m_tplVmper;        //  保存当前MCU所有视频复合器状态
    TplArray<TPeriEqpStatus> m_tplTvWall;       //  保存当前MCU所有电视墙状态
    TplArray<TPeriEqpStatus> m_tplHDTvWall;     //  保存当前MCU所有高清电视墙状态
    TplArray<TPeriEqpStatus> m_tplAdapter;      //  保存当前MCU所有码流适配器状态
    TplArray<TPeriEqpStatus> m_tplPrsPerCher;   //  保存当前MCU所有重传器状态
    TplArray<TPeriEqpStatus> m_tplMultiTvWall;  //  保存当前MCU所有多画面电视墙状态
	// lrf DCS Status [6/12/2006]
	TplArray<TPeriDcsStatus> m_tplDcs;			//	保存当前MCU所有DCS状态
};

// 记录电视墙轮询信息
class CTwPollParam  
{
public:
	CTwPollParam();
	virtual ~CTwPollParam();
	/*====================================================================
	函 数 名： SetTwPollParam
	功    能： 获取某个通道上的轮询全参数
	算法实现： 
	全局变量： 
	参    数： TTvWallPollParam&	tPollParam	要设置的轮询参数
	返 回 值： void
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08      1.0			邹俊龙					 创建
	======================================================================*/
	void SetTwPollParam( TTvWallPollParam &tPollParam );
	
	/*====================================================================
	函 数 名： GetChnlPollParam
	功    能： 获取某个通道上的轮询全参数
	算法实现： 
	全局变量： 
	参    数： 
	[in]	u8					byEqpId		设备号
	[in]	u8					byChnId		通道号
	[out]	TTvWallPollParam&	tPollInfo	轮询信息
	返 回 值： BOOL 
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08      1.0			邹俊龙                   创建
	======================================================================*/
	BOOL GetChnPollParam( u8 byEqpId, u8 byChnId, TTvWallPollParam &tPollParam );
	
	/*====================================================================
	函 数 名： GetChnlPollParam
	功    能： 获取某个通道上的轮询全参数
	算法实现： 
	全局变量： 
	参    数： 
	[in]	u16					byChnIID	通道及设备号
	[out]	TTvWallPollParam&	tPollInfo	轮询信息
	返 回 值： BOOL 
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2010/01/13      4.0			陈建辉                   创建
	2011/11/08		4.1			邹俊龙					 修改
	======================================================================*/
	BOOL GetChnPollParam( u16 wChnIID, TTvWallPollParam &tPollParam );
	
	/*====================================================================
	函 数 名： GetChnPollState
	功    能： 获取通道轮询状态
	算法实现： 
	全局变量： 
	参    数： 
	[in]	u8	byEqpId		外设编号
	[in]	u8	byChnId		通道编号
	返 回 值： u8	轮询状态
	POLL_STATE_NONE		停止
	POLL_STATE_PAUSE		暂停
	POLL_STATE_NORMAL	运行中
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08		4.1			邹俊龙					 创建
	======================================================================*/
	u8 GetChnPollState( u8 byEqpId, u8 byChnId);
	
	/*====================================================================
	函 数 名： GetChnPollState
	功    能： 获取通道轮询状态
	算法实现： 
	全局变量： 
	参    数： 
	[in]	u8	wChnIID		终端信息
	返 回 值： u8	轮询状态
	POLL_STATE_NONE		停止
	POLL_STATE_PAUSE		暂停
	POLL_STATE_NORMAL	运行中
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08		4.1			邹俊龙					 创建
	======================================================================*/
	u8 GetChnPollState( u16 wChnIId);
	
	/*====================================================================
	函 数 名： IsMtInPollChn
	功    能： 判断终端是否在正在轮询的列表中
	算法实现： 
	全局变量： 
	参    数： 
	[in]	TMt&	tMt		终端信息
	返 回 值： BOOL
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2010/01/13      4.0			陈建辉                   创建
	2011/11/08		4.1			邹俊龙					 跳过已停止的轮询
	======================================================================*/
	BOOL IsMtInPollChn(const TMt &tMt);
	
	/*====================================================================
	函 数 名： Clear
	功    能： 清空某个会议的轮询信息或全部轮询信息
	算法实现： 
	全局变量： 
	参    数： [in]	u8	byConfIdx	会议Idx
	返 回 值： 
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08		4.1			邹俊龙					 修改
	======================================================================*/
	void Clear( u8 byConfIdx = 0 ); 
	
	/*====================================================================
	函 数 名： GerAllPollMt
	功    能： 获取所有正在轮询列表里的终端
	算法实现： 
	全局变量： 
	参    数： [out] vector<TMtPollParam>& vctAllPollParam 正在轮询终端列表
	返 回 值： BOOL
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08		4.1			邹俊龙					 跳过已停止的轮询
	======================================================================*/
	void GetAllPollMt(vector<TMtPollParam>& vctAllPollParam);
	
	/*====================================================================
	函 数 名： Print
	功    能： 打印轮询信息
	算法实现： 
	全局变量： 
	参    数：
	返 回 值： 
	--------------------------------------------------------------------
	修改记录：
	日  期	        版本		修改人		走读人       修改内容
	2011/11/08		4.1			邹俊龙					 修改
	======================================================================*/
	void Print();
	
private:
	mapTwPollParam m_mapTwPollParam;
};

//  空信息
typedef struct tagNull
{
    tagNull()
    {
        tNullMt.SetNull();
		cNullMt.SetNull();
        ZeroMemory(&cNullUser, sizeof (cNullUser));
        ZeroMemory(&cNullVcsUser, sizeof (cNullVcsUser));
		ZeroMemory(&tNullUserGrp, sizeof(tNullUserGrp) );
        cNullMtAlias.SetNull();
        cNullConfId.SetNull();
        ZeroMemory(&cNullMixParam, sizeof (cNullMixParam));
    }
    TMt         tNullMt;
    CMt         cNullMt;
    CConfId     cNullConfId;
    CConf       cNullConf;
    CExUsrInfo   cNullUser;
    CVCSUsrInfo cNullVcsUser;
	TUsrGrpInfo		tNullUserGrp;
    TMtAlias    cNullMtAlias;
    TMixParam cNullMixParam;
} TNull;

extern TNull g_tNull;

//CMsgHeadInfo封装与MCU OSP通信消息的消息头
class CMsgHeadInfo
{
public:
    CMsgHeadInfo() : m_wSerialNo( 0 ),
		m_byMCUID( LOCAL_MCUIDX ),
		m_bySessionID( 0 ),
		m_cConfId( g_tNull.cNullConfId ),
		m_wEventID( 0 ),
		m_wDelayTime( DELAY_TIME_DEFAULT ),
		m_wLength( 0 ),
		m_byChannelIndex( 0 ),
		m_wMask( 0 ),
		m_byTotalPktNum(0),
		m_byCurPktIdx(0),
		m_byEqpId(0){}
	
    CMsgHeadInfo( u16 wEventId, u16 wDelayTime, u16 wLen,
		const CConfId *pcConfId = NULL );
	
	//CMsgHeadInfo中那些位有效
#define MASK_CHNINDEX			(u16)0x0001
#define MASK_MCUID				(u16)0x0002
#define MASK_MTID				(u16)0x0004
#define MASK_SESSIONID			(u16)0x0008
#define MASK_CONFID				(u16)0x0010
#define MASK_EVENTID			(u16)0x0020
#define MASK_DELAYTIME			(u16)0x0040
#define MASK_LENGTH				(u16)0x0080
#define MASK_TOALPKTNUM			(u16)0x0100
#define MASK_CURPKTIDX			(u16)0x0200
#define MASK_EQPID				(u16)0x0400
	
public:
/*=============================================================================
函 数 名:+=
功    能:更新消息头
参    数:CMsgHeadInfo &cLeft                [in/out]    被加消息头
CMsgHeadInfo &cRight               [in]        加入消息头
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/07/05  4.0     王昊    创建
    =============================================================================*/
    friend CMsgHeadInfo& operator+=(CMsgHeadInfo &cLeft, CMsgHeadInfo &cRight);
	
    /*=============================================================================
    函 数 名:IncreaseSerialNo
    功    能:递加消息流水号
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void IncreaseSerialNo(void) { m_wSerialNo++; }
	
    /*=============================================================================
    函 数 名:GetSerialNo
    功    能:得到流水号
    参    数:无
    注    意:无
    返 回 值:当前信息流水号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u16 GetSerialNo(void) const { return m_wSerialNo; }
	
    /*=============================================================================
    函 数 名:SetMCUID
    功    能:设定目标MCUID，并置位有效
    参    数:u16 wMcuId                         [in]    MCU ID
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetMCUID(u8 byMcuId) { m_byMCUID = byMcuId; AddMask(MASK_MCUID); }
	
    /*=============================================================================
    函 数 名:GetMCUID
    功    能:得到目标MCUID
    参    数:无
    注    意:无
    返 回 值:目标MCU ID
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u8 GetMCUID(void) { return ( Avail(MASK_MCUID) ? m_byMCUID : 0 ); }
	
    /*=============================================================================
    函 数 名:SetSessionID
    功    能:设定会话实例号
    参    数:u8 bySessionId                     [in]    会话实例号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetSessionID(u8 bySessionId)
    { m_bySessionID = bySessionId; AddMask(MASK_SESSIONID); }
	
    /*=============================================================================
    函 数 名:GetSessionID
    功    能:得到会话实例号
    参    数:无
    注    意:无
    返 回 值:会话实例号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u8 GetSessionID(void) { return (Avail(MASK_SESSIONID) ? m_bySessionID : 0); }
	
    /*=============================================================================
    函 数 名:SetConfID
    功    能:设置会议号
    参    数:CConfId cConfId                    [in]    会议号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetConfID(CConfId cConfId) { m_cConfId = cConfId; AddMask(MASK_CONFID); }
	
    /*=============================================================================
    函 数 名:GetConfID
    功    能:得到会议号
    参    数:无
    注    意:无
    返 回 值:会议号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    CConfId& GetConfID(void)
    { return (Avail(MASK_CONFID) ? m_cConfId : g_tNull.cNullConfId); }
	
    /*=============================================================================
    函 数 名:SetEventID
    功    能:设置事件号
    参    数:u16 wEventId                       [in]    事件号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetEventID(u16 wEventId) { m_wEventID = wEventId; AddMask(MASK_EVENTID); }
	
    /*=============================================================================
    函 数 名:GetEventID
    功    能:得到事件号
    参    数:无
    注    意:无
    返 回 值:事件号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u16 GetEventID(void) { return (Avail(MASK_EVENTID) ? m_wEventID : 0); }
	
    /*=============================================================================
    函 数 名:SetDelayTime
    功    能:设置MCU等待回复时间
    参    数:u16 wDelayTime                     [in]    等待时间
    注    意:MCU实际是根据该时间是否大于0来判断消息是请求消息还是命令消息，
	因此发送REQ消息此项必须不为0，发送CMD消息此项必须为0
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetDelayTime(u16 wDelayTime)
    { m_wDelayTime = wDelayTime; AddMask(MASK_DELAYTIME); }
	
    /*=============================================================================
    函 数 名:GetDelayTime
    功    能:获取超时时间
    参    数:无
    注    意:无
    返 回 值:超时时间
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u16 GetDelayTime(void) { return (Avail(MASK_DELAYTIME) ? m_wDelayTime : 0); }
	
    /*=============================================================================
    函 数 名:SetLength
    功    能:设置消息体长度
    参    数:u16 wMsgLen                        [in]    消息体长度
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetLength(u16 wMsgLen) { m_wLength = wMsgLen; AddMask(MASK_LENGTH); }
	
    /*=============================================================================
    函 数 名:GetLength
    功    能:设置消息体长度
    参    数:无
    注    意:无
    返 回 值:消息体长度
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u16 GetLength(void) { return (Avail(MASK_LENGTH) ? m_wLength : 0); }
	
    /*=============================================================================
    函 数 名:SetChannelIndex
    功    能:设置通道号(外设操作时比较重要)
    参    数:u8 byChannelIndex                  [in]    通道号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void SetChannelIndex(u8 byChannelIndex)
    { m_byChannelIndex = byChannelIndex; AddMask(MASK_CHNINDEX); }
	
    /*=============================================================================
    函 数 名:GetChannelIndex
    功    能:得到通道号
    参    数:无
    注    意:无
    返 回 值:通道号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    u8 GetChannelIndex(void)
    { return (Avail(MASK_CHNINDEX) ? m_byChannelIndex : 0); }
	/*=============================================================================
    函 数 名:SetTotalPktNum
    功    能:设置总包数
    参    数:u8 byTotalPktNum                  [in]    总包数
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2009/12/23  4.0     陈建辉    创建
    =============================================================================*/
    void SetTotalPktNum(u8 byTotalPktNum)
    { m_byTotalPktNum = byTotalPktNum; AddMask(MASK_TOALPKTNUM); }
	
    /*=============================================================================
    函 数 名:GetChannelIndex
    功    能:得到总包数
    参    数:无
    注    意:无
    返 回 值:通道号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2009/12/23  4.0     陈建辉    创建
    =============================================================================*/
    u8 GetTotalPktNum(void)
    { return (Avail(MASK_TOALPKTNUM) ? m_byTotalPktNum : 0); }
	/*=============================================================================
    函 数 名:SetCurPktIdx
    功    能:设置但前包号(从0开始)
    参    数:u8 byCurPktIdx                  [in]    当前包号
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2009/12/23  4.0     陈建辉    创建
    =============================================================================*/
    void SetCurPktIdx(u8 byCurPktIdx)
    { m_byCurPktIdx = byCurPktIdx; AddMask(MASK_CURPKTIDX); }
	
    /*=============================================================================
    函 数 名:GetCurPktIdx
    功    能:得到当前包号
    参    数:无
    注    意:无
    返 回 值:通道号
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2009/12/23  4.0     陈建辉    创建
    =============================================================================*/
    u8 GetCurPktIdx(void)
    { return (Avail(MASK_CURPKTIDX) ? m_byCurPktIdx : 0); }

	void SetEqpId(u8 byEqpId) { m_byEqpId = byEqpId; AddMask(MASK_EQPID); }
	u8	 GetEqpId(void) 
	{ return (Avail(MASK_EQPID) ? m_byEqpId : 0); }
	
private:
/*=============================================================================
函 数 名:AddMask
功    能:设置有效位
参    数:u8 byMask                          [in]    有效位
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/07/05  4.0     王昊    创建
    =============================================================================*/
    void AddMask(u16 wMask) { m_wMask = m_wMask | wMask; }
	
    /*=============================================================================
    函 数 名:Avail
    功    能:判断位是否有效
    参    数:u8 byByte                          [in]    传入位
    注    意:无
    返 回 值:是否有效
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2005/07/05  4.0     王昊    创建
    =============================================================================*/
    BOOL32 Avail(u16 wByte) { return ( ( wByte & m_wMask ) == wByte ); }
	
private:
    u16	    m_wSerialNo;            // 消息流水号
	// modify by muxingmao 三级级联
    u16	    m_byMCUID;              // 定位目的MCU 
    u8	    m_bySessionID;          // 会话实例号
    CConfId	m_cConfId;              // 会议号
    u16	    m_wEventID;             // 事件号
    u16	    m_wDelayTime;           // 等待时延（单位：秒）
    u16	    m_wLength;              // 消息体长度
    u8	    m_byChannelIndex;       // 通道索引号
	u8      m_byTotalPktNum;        //总包数（用于需要切包发送的消息）
    u8      m_byCurPktIdx;          //当前包索引（从0开始）
    u16	    m_wMask;               // 消息头中的哪些信息是有效的
	u8		m_byEqpId;				// EQPID
	
};

//#ifdef _DEBUG
///*=======================================================================
//函 数 名:Dump
//功    能:打印类的成员调试信息
//参    数:无需说明
//注    意:无
//返 回 值:无
//-------------------------------------------------------------------------
//修改纪录:
//日      期  版本    修改人  修改内容
//2004/03/19  3.0     李洪强  创建
//=======================================================================*/
//void Dump(const TMt &tMt,                       CDumpContext &dc);
//void Dump(const TMtExt &tMtExt,                 CDumpContext &dc);
//void Dump(const TMtStatus &tMtStatus,           CDumpContext &dc);
//void Dump(const TMtAlias &tMtAlias,             CDumpContext &dc);
//void Dump(const TTransportAddr &tTransportAddr, CDumpContext &dc);
//void Dump(const CUserFullInfo &cUserFullInfo,   CDumpContext &dc);
//#endif  //  _DEBUG

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#endif
//8000H MCU 多地址映射结构体
struct TDMZAddress
{	
	TDMZAddress()
	{
		memset(this, 0, sizeof(TDMZAddress));
	}
	
    BOOL32 IsDMZAddressUse() const 
	{
		return (m_byUse == 1);
	}
	u32 GetDMZAddressIP() const 
	{
		return ntohl(m_dwAddressIp);
	}
	
	void SetDMZAddressIP(u32 dwAddressIP)
	{
		if (dwAddressIP != 0)
		{
			m_byUse		  = 1;
			m_dwAddressIp = htonl(dwAddressIP);
		}
		else
		{
			m_byUse		  = 0;
			m_dwAddressIp = 0;
		}
	}

protected:
	u8  m_byUse;         //0 表示不启用，1 表示启用
	u32 m_dwAddressIp;   //多映射地址
	
};

// MCU注册GK密码认证
struct TMcuReGKPassword
{
	
	TMcuReGKPassword()
	{
		m_byUse = 0;
		memset(m_abyMcuReGKPassword, 0, sizeof(m_abyMcuReGKPassword));
	}
	~TMcuReGKPassword()
	{
		m_byUse = 0;
	}
	
	BOOL32 IsPasswordUse() const
	{
		return (m_byUse == 1);
	}
	const s8* GetMcuReGkPassword() const 
	{
		return m_abyMcuReGKPassword;
	}
	
    void SetMcuReGKPassword( const s8* abyMcuReGKPassword )
	{
		if (abyMcuReGKPassword != NULL)
		{
			if (0 == strlen(abyMcuReGKPassword))
			{
				m_byUse = 0;
				memset(m_abyMcuReGKPassword, 0, sizeof(m_abyMcuReGKPassword));
			}
			else
			{
				m_byUse = 1;
				memcpy(m_abyMcuReGKPassword, abyMcuReGKPassword, sizeof(m_abyMcuReGKPassword));	

				if (strlen(abyMcuReGKPassword) >= MCU_RE_GK_PASSWORD_LEN)
				{/*输入的字符个数>=64时*/
					m_abyMcuReGKPassword[MCU_RE_GK_PASSWORD_LEN - 1] = '\0';
				}
				
			}
		}		
	}


protected:
	u8  m_byUse;									   //0 表示不启用，1 表示启用
	s8  m_abyMcuReGKPassword[MCU_RE_GK_PASSWORD_LEN];  //DMZ 地址
};
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//lvguanjiao 2013 0329 MCU 一些基本配置的补充（此结构体的创建是为了兼容以前MCU版本）
//解决MCU端不方便存储的数据，改为界面存储
struct TMcuGeneralCfgImp
{

	TMcuReGKPassword		  m_tMcuReGKPassword; //MCU 注册GK密码
	TDMZAddress				  m_tDMZAddress;      //8000H MCU 多地址映射

	TMcuGeneralCfgImp()
	{
		memset(this, 0, sizeof(TMcuGeneralCfgImp));
	}
	void SetMcuReGKPassword(TMcuReGKPassword &tMcuReGKPassword)
	{
		m_tMcuReGKPassword = tMcuReGKPassword;
	}
	void SetDMZAddress(TDMZAddress& tDMZAddress)
	{
		m_tDMZAddress = tDMZAddress;
	}

};
#ifdef WIN32
#pragma pack( pop )
#endif

#define  MAX_REMARK_LENGTH 55
//单个IP信息
struct TIpInfo
{
protected:
	u32 m_dwIpAddr;                        //Ip地址
    u32 m_dwSubnetMask;                    //子网掩码
    u32 m_dwGWIpAddr;                      //网关 Ip地址
	s8	m_aszRemark[MAX_REMARK_LENGTH];     //备注

public:
	BOOL32 operator == (TIpInfo tIpInfo)
	{
		if (strcmp(this->GetRemark(), tIpInfo.GetRemark()) != 0)
		{
			return FALSE;
		}
		

		if (this->GetIpAddr() != tIpInfo.GetIpAddr() ||
			this->GetSubNetMask() != tIpInfo.GetSubNetMask() ||
			this->GetGWIpAddr() != tIpInfo.GetGWIpAddr())
		{
			return FALSE;
		}
		return TRUE;
	}

	TIpInfo(void) { memset(this, 0, sizeof(TIpInfo)); }
    
    void    SetIpAddr(u32 dwIpAddr) { m_dwIpAddr = htonl(dwIpAddr); }     //host order
    u32     GetIpAddr(void)  { return ntohl(m_dwIpAddr); }
    void    SetSubNetMask(u32 dwMask) { m_dwSubnetMask = htonl(dwMask); }
    u32     GetSubNetMask(void) { return ntohl(m_dwSubnetMask); }
    void    SetGWIpAddr(u32 dwGWIpAddr) { m_dwGWIpAddr = htonl(dwGWIpAddr); }   //host order
    u32     GetGWIpAddr(void) { return ntohl(m_dwGWIpAddr); }
	void    SetRemark(LPCSTR lpszRemark)
    {
        if(NULL != lpszRemark)
        {
            strncpy(m_aszRemark, lpszRemark, sizeof(m_aszRemark));
            m_aszRemark[sizeof(m_aszRemark) - 1] = '\0';
        }        
    }
    LPCSTR GetRemark(void) const { return m_aszRemark; }
};

#define  INNER_GK   0
#define  OUTER_GK   1
#define  VALID_GK   2
//GK信息
struct TGkInfo
{
protected:
	u8     m_byEnableInnerGK;
	u8     m_byInnerGKPort;
	u32    m_dwInnerGKIp;
	u8     m_byTypeOfGk;
	u8     m_byEnableRegGk;

public:
	TGkInfo()
	{
		m_byEnableRegGk = 0;
		m_byEnableInnerGK = 0;
		m_byInnerGKPort = 0;
		m_byTypeOfGk = 0;
		m_dwInnerGKIp = 0;
	}
	//内置GK服务的启用
	void SetEnableInnerGK(u8 bEnableInnerGk)
	{
		m_byEnableInnerGK = bEnableInnerGk;
	}
	u8 GetEnableInnerGk()
	{
		return m_byEnableInnerGK;
	}

	//内置gk端口
	void SetInnerGKPort(u8 byInnerGkPort)
	{
		m_byInnerGKPort = byInnerGkPort;
	}
	u8 GetInnerGKPort()
	{
		return m_byInnerGKPort;
	}

	//内置GK的IP
	void SetInnerGKIp(u32 dwInnerGKIp)
	{
		m_dwInnerGKIp = htonl(dwInnerGKIp);
	}
	u32 GetInnerGKIp()
	{
		return ntohl(m_dwInnerGKIp);
	}

	//注册GK类型
	void SetGkType(u8 byGkType)
	{
		m_byTypeOfGk = byGkType;
	}
	u8 GetGkType()
	{
		return m_byTypeOfGk;
	}

	//是否注册GK
	void SetEnableGkReg(u8 byEnableGkReg)
	{
		m_byEnableRegGk = byEnableGkReg;
	}

	u8 GetEnableGkReg()
	{
		return m_byEnableRegGk;
	}

};

#define  MAX_IPNUM_OF_ONE_NET   16
#define  MAX_NETPORT_NUM        3
//单个网口的信息，包含是否启用DMZ地址，多个IP地址
struct TNetCardInfo
{
protected:
	BOOL32  m_bUseDMZ;
	u32     m_dwDMZIp;
	TIpInfo m_aIpInfo[MAX_IPNUM_OF_ONE_NET];
	u8      m_byIpNum;
public:
	TNetCardInfo()
	{
		memset(this, 0, sizeof(TNetCardInfo));
	}
	BOOL32 IsEnableDMZ()
	{
		return m_bUseDMZ;
	}
	void SetEnableDMZ(BOOL32 bEnableDMZ)
	{
		m_bUseDMZ = bEnableDMZ;
	}
	void SetDMZIp(u32 dwIpAddr)
	{
		m_dwDMZIp = htonl(dwIpAddr);
	}
	u32 GetDMZIp()
	{
		return ntohl(m_dwDMZIp);
	}
	u8 GetNumOfTIpInfo()
	{
		return m_byIpNum;
	}
	void SetNumOfTIpInfo(u8 byNum)
	{
		m_byIpNum = byNum;
	}
	TIpInfo* GetIpInfo(u8 byIndex)
	{
		if (byIndex >= MAX_IPNUM_OF_ONE_NET)
		{
			return NULL;
		}
		return &m_aIpInfo[byIndex];
	}
	BOOL32 IsIpExist(TIpInfo tIpInfo)
	{
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
		{
			if (m_aIpInfo[nIndex].GetIpAddr() == tIpInfo.GetIpAddr())
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL32 IsIpExist(TIpInfo tIpInfo, u8& byIdx)
	{
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
		{
			if (m_aIpInfo[nIndex].GetIpAddr() == tIpInfo.GetIpAddr())
			{
				byIdx = nIndex;
				return TRUE;
			}
		}
		return FALSE;
	}
	BOOL32 AddIpInfo(TIpInfo& tIpInfo)
	{
		if (m_byIpNum >= MAX_IPNUM_OF_ONE_NET || tIpInfo.GetIpAddr() == 0)
		{
			return FALSE;
		}
		
		else
		{
			s32 nIndex = 0;
			for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
			{
				if (m_aIpInfo[nIndex].GetIpAddr() == tIpInfo.GetIpAddr())
				{
					return FALSE;
				}
			}
			m_aIpInfo[m_byIpNum++] = tIpInfo;
			return TRUE;
		}
	}
	BOOL32 ModifIpInfo(u8 byIndex, TIpInfo& tIpInfo)
	{
		if (byIndex > MAX_IPNUM_OF_ONE_NET)
		{
			return FALSE;
		}
		
		else
		{
			s32 nIndex = 0;
			for (nIndex = 0; nIndex < m_byIpNum; nIndex++)
			{
				if (tIpInfo == m_aIpInfo[nIndex] && byIndex != nIndex)
				{
					return FALSE;
				}
			}
			m_aIpInfo[byIndex] = tIpInfo;
			return TRUE;
		}	
	}
	BOOL32 DelIpInfo(u8 byIndex)
	{
		if (byIndex >= MAX_IPNUM_OF_ONE_NET || m_byIpNum == 0)
		{
			return FALSE;
		}
		else
		{
			u8 byLoop = 0;
			for(byLoop = byIndex; byLoop < m_byIpNum; byLoop ++)
			{				
				m_aIpInfo[byLoop] = m_aIpInfo[byLoop + 1];
			}
			m_byIpNum--;
			return TRUE;
		}
	}
};

#define  MIX_NET       0 
#define  WAN_NET       1
#define  LAN_NET       2 
#define  VALID_NET     3
//新的结构体用来保存mcu配置的信息
struct TMcsMcuGeneralCfg
{
protected:
	//8000I新增功能
	BOOL32			   m_bEnablePortBackUp;				//是否网口备份
	BOOL32			   m_bEnableSipService;				//是否启用SIP服务
	TIpInfo			   m_tSipService;					//SIP服务信息
	u8				   m_byInterface;					//当前使用的网口
	TDMZAddress		   m_tDmzAddress;					//DMZ信息      
	TNetCardInfo       m_atNetCardInfo[MAX_NETPORT_NUM];//网口信息
	u8                 m_byTypeofNet;					//所选网络类型：如公网、私网...
	TGkInfo            m_tGkInfo;                       //GK信息
	u8                 m_bySipPort;                     //SIP服务所使用的网口           
	u8                 m_byDebugMode;                   //是否开启调试模式，0不开启，1开启     

public:
	TMcuGeneralCfgImp  m_tMcuGeneralCfgImp;				//GK注册密码信息，DMZ地址信息
	TMcuNetCfg		   m_tNetCardCfg;					//8000E网卡信息
	TMcuGeneralCfg     m_tGeneral;						//mcu基本配置信息
	TMcuEqpCfg         m_tEqp;							//单个网口的配置信息

	void Clear()
	{
		m_byDebugMode = 0;
		m_bEnablePortBackUp = 0;
		m_bEnableSipService = 0;
		m_byInterface = 0;
		m_byTypeofNet = 2;
		m_bySipPort = 0;
		ZeroMemory( &m_tMcuGeneralCfgImp, sizeof(TMcuGeneralCfgImp) );
		ZeroMemory( &m_tNetCardCfg, sizeof( TMcuNetCfg ) );
		ZeroMemory( &m_tGeneral, sizeof( TMcuGeneralCfg ) );
		ZeroMemory( &m_tEqp, sizeof( TMcuEqpCfg ) );
		ZeroMemory( &m_tSipService, sizeof( TIpInfo ) );
		ZeroMemory( &m_tDmzAddress, sizeof(TDMZAddress) );
		ZeroMemory( &m_atNetCardInfo, sizeof(TNetCardInfo) * MAX_NETPORT_NUM);
		ZeroMemory( &m_tGkInfo, sizeof(TGkInfo));
	}

	TMcsMcuGeneralCfg()
	{
		Clear();
	}

	//8000I新增接口

	//以下接口给mcu basic界面使用
	//设置调试模式
	void SetDebugMode(u8 byDebugMode)
	{
		m_byDebugMode = byDebugMode;
	}
	
	//获取调试模式
	u8 GetDebugMode()
	{
		return m_byDebugMode;
	}

	//获取内置SIP IP
	u32 GetInnerSipIp()
	{
		return m_tSipService.GetIpAddr();
	}
	
	//获取mcu IP
	u32 GetMcuIp()
	{
		TIpInfo* ptIpInfo = m_atNetCardInfo[0].GetIpInfo(0);
		if (ptIpInfo == NULL)
		{
			return 0;
		}
		return ptIpInfo->GetIpAddr();	
	}

	//以下为网口、mcu配置需要使用的接口
	//寻找对应IP所在的网口，返回网口.用于内置GK及内置SIP
	u8 GetNetPortOfIp(u32 dwIp)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			//遍历网口
			TNetCardInfo tNetCardInfo = m_atNetCardInfo[byIndex];
			u8 byIdx = 0;
			for (byIdx = 0; byIdx < MAX_IPNUM_OF_ONE_NET; byIdx++)
			{
				//遍历每个IP
				TIpInfo* ptIpInfo = tNetCardInfo.GetIpInfo(byIdx);
				if (ptIpInfo != NULL)
				{
					if (dwIp == ptIpInfo->GetIpAddr())
					{
						return byIndex;
					}
				}			
			}
		}
		return 0;
	}

	//TNewNetCfg是与mcu侧交互所使用的结构体
	//将TNewNetCfg中的数据保存到TMcsMcuGeneralCfg中
	void SetCfgInfo(TNewNetCfg tNewNetCfg)
	{
		//网口选择
		m_byTypeofNet = tNewNetCfg.GetNetworkType();

		//网口备份 or 3网口
		if (tNewNetCfg.GetUseModeType() == MODETYPE_BAK)
		{
			m_bEnablePortBackUp = TRUE;
		}
		else
		{
			m_bEnablePortBackUp = FALSE;
		}

		//网口0 1 2的数据传递
		u32 dwIndex = 0;
		//一共3个网口
		for (dwIndex = 0; dwIndex < MAX_NETPORT_NUM; dwIndex++)
		{
			//单个网口的信息
			TEthCfg tEthCfg;
			tNewNetCfg.GetEthCfg(dwIndex, tEthCfg);
			TNetCardInfo* ptNetCardInfo = &m_atNetCardInfo[dwIndex];
			//DMZ地址
			if (tEthCfg.IsUseDMZ())
			{
				ptNetCardInfo->SetEnableDMZ(TRUE);
				ptNetCardInfo->SetDMZIp(tEthCfg.GetDMZIpAddr());
			}
			else
			{
				ptNetCardInfo->SetEnableDMZ(FALSE);
				ptNetCardInfo->SetDMZIp(tEthCfg.GetDMZIpAddr());
			}
			
			//IP信息,共16个
			u32 dwIdx = 0;
			for (dwIdx = 0; dwIdx < MAX_IPNUM_OF_ONE_NET; dwIdx++)
			{
				//每个IP的信息
				TNetCfg tNetCfg;
				tEthCfg.GetNetCfg(dwIdx, tNetCfg);
				TIpInfo tIpInfo; 
				tIpInfo.SetIpAddr(tNetCfg.GetIpAddr());
				tIpInfo.SetSubNetMask(tNetCfg.GetIpMask());
				tIpInfo.SetGWIpAddr(tNetCfg.GetGwIp());
				tIpInfo.SetRemark(tNetCfg.GetNotes());
				if (dwIdx == 0 && ptNetCardInfo->GetNumOfTIpInfo() != 0)
				{
					ptNetCardInfo->ModifIpInfo(0, tIpInfo);
				}
				else
				{
					ptNetCardInfo->AddIpInfo(tIpInfo);
				}
				
			}
		}
		

		//sip服务
		m_bEnableSipService = tNewNetCfg.IsUseSip();
		u32 dwIp = tNewNetCfg.GetSipIpAddr();
		if (m_bEnableSipService)
		{
			m_bEnableSipService = TRUE;
			m_bySipPort = GetNetPortOfIp(dwIp);
			m_tSipService.SetIpAddr(dwIp);
		}
	}

	//将数据设置到TNewNetCfg中
	void SetCfgInfoForMcu(TNewNetCfg &tNewNetCfg)
	{
		//网口选择
		enNetworkType enType = (enNetworkType)m_byTypeofNet;
		tNewNetCfg.SetNetworkType(enType);
		//网口备份 or 3网口
		if (m_bEnablePortBackUp)
		{
			tNewNetCfg.SetUseModeType(MODETYPE_BAK);
		}
		else
		{
			tNewNetCfg.SetUseModeType(MODETYPE_3ETH);
		}
		//sip服务
		if (m_bEnableSipService)
		{
			tNewNetCfg.SetIsUseSip(TRUE);
			tNewNetCfg.SetSipIpAddr(m_tSipService.GetIpAddr());
		}
		else
		{
			tNewNetCfg.SetIsUseSip(FALSE);
			tNewNetCfg.SetSipIpAddr(0);
		}
		//网口0 1 2的数据传递
		u32 dwIndex = 0;
		//一共3个网口
		for (dwIndex = 0; dwIndex < MAX_NETPORT_NUM; dwIndex++)
		{
			//单个网口的信息
			TEthCfg tEthCfg;
			TNetCardInfo tNetCardInfo = m_atNetCardInfo[dwIndex];
			//DMZ地址
			if (tNetCardInfo.IsEnableDMZ())
			{
				tEthCfg.SetDMZIpAddr(tNetCardInfo.GetDMZIp());
			}
			else
			{
				tEthCfg.SetDMZIpAddr(0);
			}

			//IP信息,共16个
			u32 dwIdx = 0;
		
			for (dwIdx = 0; dwIdx < MAX_IPNUM_OF_ONE_NET; dwIdx++)
			{
				//每个IP的信息
				TNetCfg tNetCfg;
				TIpInfo* ptIpInfo = tNetCardInfo.GetIpInfo(dwIdx);
				if (ptIpInfo != NULL)
				{
					tNetCfg.SetNetRouteParam(ptIpInfo->GetIpAddr(), ptIpInfo->GetSubNetMask(), ptIpInfo->GetGWIpAddr());
					tNetCfg.SetNotes(ptIpInfo->GetRemark());
				}
				tEthCfg.SetNetCfg(dwIdx, tNetCfg);
			}
			tNewNetCfg.SetEthCfg(dwIndex, tEthCfg);
		}
	}

	//8000I的内置GK GK注册等信息
	//将数据设置到TMcuGeneralCfg中
	void SetGeneralCfgForMcu(TMcuGeneralCfg &tMcuGeneralCfg)
	{
		//内置GK服务
		if (m_tGkInfo.GetEnableInnerGk() != 0)
		{
			u32 dwIp = m_tGkInfo.GetInnerGKIp();
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkUsed(TRUE);
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkIpAddr(dwIp);
		}
		else
		{
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkUsed(FALSE);
			tMcuGeneralCfg.m_tGkProxyCfg.SetGkIpAddr(0);
		}	
		//暂时默认放开启用代理，方便测试使用   lbg 2013.11.21
		tMcuGeneralCfg.m_tGkProxyCfg.SetProxyUsed(1);
	}

	//将TMcuGeneralCfg中GK信息保存下来
	void SetGeneralCfg(TMcuGeneralCfg tMcuGeneralCfg)
	{
		if (tMcuGeneralCfg.m_tGkProxyCfg.IsGkUsed())
		{
			m_tGkInfo.SetEnableInnerGK(TRUE);
			u32 dwIp = tMcuGeneralCfg.m_tGkProxyCfg.GetGkIpAddr();
			m_tGkInfo.SetInnerGKIp(dwIp);
			u8 byNetPort = GetNetPortOfIp(dwIp);
			m_tGkInfo.SetInnerGKPort(byNetPort);
		}
		else
		{
			m_tGkInfo.SetEnableInnerGK(FALSE);
			m_tGkInfo.SetInnerGKIp(0);
			m_tGkInfo.SetInnerGKPort(0);
		}

		u32 dwIpGkReg = tMcuGeneralCfg.m_tNet.GetGkIpAddr();
		if (dwIpGkReg == 0)
		{
			m_tGkInfo.SetEnableGkReg(FALSE);
			m_tGkInfo.SetGkType(VALID_GK);
		}
		else
		{
			m_tGkInfo.SetEnableGkReg(TRUE);
			TIpInfo tIpInfo;
			tIpInfo.SetIpAddr(dwIpGkReg);
			if (IsIpInfoExist(tIpInfo))
			{
				m_tGkInfo.SetGkType(INNER_GK);
			}
			else
			{
				m_tGkInfo.SetGkType(OUTER_GK);
			}
		}
	}

	//是否已存在该IP，3个网口中的IP不能重复
	BOOL32 IsIpInfoExist(TIpInfo tIpInfo)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			//each netport
			if (m_atNetCardInfo[byIndex].IsIpExist(tIpInfo))
			{
				return TRUE;			
			}		
		}
		return FALSE;
	}

	//已配置IP的个数
    void GetNumOfIp(u8 &byNum)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			TNetCardInfo tNetCardInfo = m_atNetCardInfo[byIndex];
			u8 byIdx = 0;
			for (byIdx = 0; byIdx < MAX_IPNUM_OF_ONE_NET; byIdx++)
			{
				TIpInfo* ptIpInfo = tNetCardInfo.GetIpInfo(byIdx);
				if (ptIpInfo != NULL)
				{
					u32 dwIp = ptIpInfo->GetIpAddr();
					if (dwIp != 0)
					{
						byNum++;
					}
				}
			}
		}
	}

	//该IP是否已存在
	BOOL32 IsIpInfoExist(TIpInfo tIpInfo, u8 byNetPort, u8 byIdx)
	{
		u8 byIndex = 0;
		for (byIndex = 0; byIndex < MAX_NETPORT_NUM; byIndex++)
		{
			//each netport
			u8 byIdx2 = 0;
			if (m_atNetCardInfo[byIndex].IsIpExist(tIpInfo, byIdx2))
			{
				if (byIndex != byNetPort || byIdx != byIdx2)
				{
					return TRUE;
				}			
			}		
		}
		return FALSE;
	}

	//设置网络类型
	void SetTypeofNet(u8 byType)
	{
		m_byTypeofNet = byType;
	}

	//获取网络类型
	u8 GetTypeofNet()
	{
		return m_byTypeofNet;
	}

	//是否网口备份
	BOOL32 IsEnablePortBackUp()
	{
		return m_bEnablePortBackUp;
	}

	//设置是否网口备份
	void SetEnablePortBackUp(BOOL32 bEnablePortBackUp)
	{
		m_bEnablePortBackUp = bEnablePortBackUp;
	}
	
	//设置单个网口信息
	BOOL32 SetOneNetCardInfo(u8 byNetPort, TNetCardInfo tNetCardInfo)
	{
		if (byNetPort >= MAX_NETPORT_NUM)
		{
			return FALSE;
		}
		else
		{
			m_atNetCardInfo[byNetPort] = tNetCardInfo;

			TIpInfo *ptIpInfo = tNetCardInfo.GetIpInfo(0);
			if(ptIpInfo == NULL)
			{
				return FALSE;
			}		
			m_tEqp.SetMcuIpAddr(ptIpInfo->GetIpAddr());
			m_tEqp.SetMcuSubNetMask(ptIpInfo->GetSubNetMask());
			m_tEqp.SetGWIpAddr(ptIpInfo->GetGWIpAddr());

			m_tNetCardCfg.SetMcuEqpCfg(byNetPort, m_tEqp);
			return TRUE;
		}		
	}

	//获取单个网口信息
	TNetCardInfo* GetOneNetCardInfo(u8 byNetPort)
	{
		if (byNetPort >= MAX_NETPORT_NUM )
		{
			return NULL;
		}
		else
		{
			return &m_atNetCardInfo[byNetPort];
		}
	}

	//设置老版本的单个网口的信息，主要是为了兼容老版本，在lib层进行数据的转换
	void SetMcuEqpCfg(u8 byInterface, TMcuEqpCfg tMcuEqpCfg)
	{
		//非mcu配置的地方还是直接用m_tEqp
		m_tEqp = tMcuEqpCfg;
		m_byInterface = 0;
		TNetCardInfo* ptNetCardInfo = GetOneNetCardInfo(byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}

		TIpInfo* ptIpInfo = ptNetCardInfo->GetIpInfo(0);
		if (ptIpInfo == NULL)
		{
			return;
		}

		ptIpInfo->SetIpAddr(tMcuEqpCfg.GetMcuIpAddr());
		ptIpInfo->SetSubNetMask(tMcuEqpCfg.GetMcuSubNetMask());
		ptIpInfo->SetGWIpAddr(tMcuEqpCfg.GetGWIpAddr());

		ptNetCardInfo->AddIpInfo(*ptIpInfo);
		SetOneNetCardInfo(byInterface, *ptNetCardInfo);
	}

	//获取老版本的单个网口的信息
    TMcuEqpCfg* GetMcuEqpCfg(u8 byInterface, BOOL32 bNewVersion)
	{		
		return &m_tEqp;			
	}

	//将新结构体中的数据保存到老的结构体TMCUEQPCFG中,不能直接在GET函数中做SET操作
	void SetMcuEqpCfgForOldVersion(u8 byInterface, BOOL32 bNewVersion)
	{
		TNetCardInfo *ptNetCardInfo = GetOneNetCardInfo(byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}

		TIpInfo *ptIpInfo = ptNetCardInfo->GetIpInfo(0);
		if(ptIpInfo == NULL)
		{
			return;
		}

		m_tEqp.SetMcuIpAddr(ptIpInfo->GetIpAddr());
		m_tEqp.SetMcuSubNetMask(ptIpInfo->GetSubNetMask());
		m_tEqp.SetGWIpAddr(ptIpInfo->GetGWIpAddr());
	}

	//设置老版本的网口信息（网口0，）
	void SetMcuNetCfg(TMcuNetCfg tMcuNetCfg)
	{
		//非mcu配置的地方还是直接用m_tMcuNetCfg
		m_tNetCardCfg = tMcuNetCfg;
		m_byInterface = tMcuNetCfg.GetInterface();
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < MAXNUM_ETH_INTERFACE; nIndex++)
		{
			SetMcuEqpCfg(nIndex, tMcuNetCfg.m_atMcuEqpCfg[nIndex]);
		}
	}

	//获取老版本的网口信息
	TMcuNetCfg* GetMcuNetCfg()
	{		
		return &m_tNetCardCfg;
	}

	//将新结构体中的数据保存到老的结构体TNETCARDCFG中,不能直接在GET函数中做SET操作
	void SetMcuNetCfgForOldVersion()
	{
		m_tNetCardCfg.SetInterface(m_byInterface);
		s32 nIndex = 0;
		for (nIndex = 0; nIndex < MAXNUM_ETH_INTERFACE; nIndex++)
		{
			TMcuEqpCfg* ptMcuEqpCfg = GetMcuEqpCfg(nIndex);
			m_tNetCardCfg.SetMcuEqpCfg(nIndex, *ptMcuEqpCfg);
		}
	}
	
	//设置老版本的DMZ信息
	void SetDmzIp(TDMZAddress tDMZAddress)
	{
		m_tDmzAddress = tDMZAddress;
		m_byInterface = 0;
		TNetCardInfo* ptNetCardInfo = GetOneNetCardInfo(m_byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}
		
		ptNetCardInfo->SetEnableDMZ(tDMZAddress.IsDMZAddressUse());
		ptNetCardInfo->SetDMZIp(tDMZAddress.GetDMZAddressIP());
		SetOneNetCardInfo(0, *ptNetCardInfo);
	}

	//获取老版本的DMZ信息
	TDMZAddress* GetDmzAddress()
	{
		return &m_tDmzAddress;		
	}

	//将新结构体中的数据保存到老的结构体TDMZAddress中,不能直接在GET函数中做SET操作
	void SetDmzAddressForOldVersion()
	{
		TNetCardInfo *ptNetCardInfo = GetOneNetCardInfo(m_byInterface);
		if (ptNetCardInfo == NULL)
		{
			return;
		}
		m_tDmzAddress.SetDMZAddressIP(ptNetCardInfo->GetDMZIp());
	}

	//是否启用SIP服务
	BOOL32 IsEnableSipService()
	{
		return m_bEnableSipService;
	}

	void SetEnableSipService(BOOL32 bEnableSipService)
	{
		m_bEnableSipService = bEnableSipService;
	}

	void SetSipPort(u8 byPort)
	{
		m_bySipPort = byPort;
	}
	u8 GetSipPort()
	{
		return m_bySipPort;
	}
	void SetSipService(TIpInfo tIpInfo)
	{
		m_tSipService = tIpInfo;
	}

	TIpInfo* GetSipService()
	{
		return &m_tSipService;
	}

	//GK信息
	void SetGkInfo(TGkInfo tGkInfo)
	{
		m_tGkInfo = tGkInfo;
	}
	TGkInfo* GetGkInfo()
	{
		return &m_tGkInfo;
	}


	//TMcuGeneralCfgImp的所有接口提取
	//设置mcu注册GK密码信息(结构体)
	void SetMcuReGKPassword(TMcuReGKPassword &tMcuReGKPassword)
	{
		m_tMcuGeneralCfgImp.m_tMcuReGKPassword = tMcuReGKPassword;
	}

	//设置DMZ地址信息（结构体）
	void SetDMZAddress(TDMZAddress& tDMZAddress)
	{
		m_tMcuGeneralCfgImp.m_tDMZAddress = tDMZAddress;
	}

	//是否启用GK密码
	BOOL32 IsPasswordUse() const
	{
		return m_tMcuGeneralCfgImp.m_tMcuReGKPassword.IsPasswordUse();
	}

	//设置GK注册密码
	void SetMcuReGKPassword( const s8* abyMcuReGKPassword )
	{
		m_tMcuGeneralCfgImp.m_tMcuReGKPassword.SetMcuReGKPassword(abyMcuReGKPassword);
	}

	//获取GK注册密码
	const s8* GetMcuReGkPassword() const 
	{
		return m_tMcuGeneralCfgImp.m_tMcuReGKPassword.GetMcuReGkPassword();
	}

	//是否使用DMZ地址
	BOOL32 IsDMZAddressUse() 
	{
		return m_atNetCardInfo[m_byInterface].IsEnableDMZ();
	}

	//设置DMZ ip地址
	void SetDMZAddressIP(u32 dwAddressIP)
	{
		TDMZAddress tDmzAddress;
		tDmzAddress.SetDMZAddressIP(dwAddressIP);
		SetDmzIp(tDmzAddress);
	}

	//获取DMZ ip地址
	u32 GetDMZAddressIP() const
	{
		return m_tDmzAddress.GetDMZAddressIP();
	}

	//TMcuNetCfg的所有接口提取
	//获取网口接口
	u8 GetInterfaceForNetCfg( void ) const
	{
		return m_tNetCardCfg.GetInterface();
	}

	//设置网口接口
	void SetInterfaceForNetCfg( u8 byInterface)
	{
		m_tNetCardCfg.SetInterface(byInterface);
	}

	//获取网口配置的基本信息
	TMcuEqpCfg *GetMcuEqpCfg(u8 byIdx)
	{
		return m_tNetCardCfg.GetMcuEqpCfg(byIdx);
	}

	//TMcuEqpCfg的所有接口提取
	//设置IP地址
	void SetMcuIpAddr(u32 dwIpAddr)
	{
		m_tEqp.SetMcuIpAddr(dwIpAddr);
	}

	//获取IP地址
	u32 GetMcuIpAddr()
	{
		return m_tEqp.GetMcuIpAddr();
	}

	//设置子网掩码
	void SetMcuSubNetMask(u32 dwMask)
	{
		m_tEqp.SetMcuSubNetMask(dwMask);
	}

	//获取子网掩码
	u32 GetMcuSubNetMask()
	{
		return m_tEqp.GetMcuSubNetMask();
	}

	//设置网关
	void SetGWIpAddr(u32 dwGWIpAddr)
	{
		m_tEqp.SetGWIpAddr(dwGWIpAddr);
	}

	//获取网关
	u32 GetGWIpAddr()
	{
		return m_tEqp.GetGWIpAddr();
	}

	//设置层号
	void SetLayer(u8 byLayer)
	{
		m_tEqp.SetLayer(byLayer);
	}

	//获取层号
	u8 GetLayer()
	{
		return m_tEqp.GetLayer();
	}

	//设置槽号
	void SetSlot(u8 bySlotId)
	{
		m_tEqp.SetSlot(bySlotId);
	}

	//获取槽号
	u8 GetSlot()
	{
		return m_tEqp.GetSlot();
	}

	//设置网口
	void SetInterfaceForEqpCfg(u8 byInterface)
	{
		m_tEqp.SetInterface(byInterface);
	}

	//获取网口
	u8 GetInterfaceForEqpCfg()
	{
		return m_tEqp.GetInterface();
	}

};

// 保存单个MCU的配置界面化信息
class CMcuCfg : public TMcuExt
{
public:
/*=============================================================================
函 数 名:CMcuCfg
功    能:
参    数:
注    意:无
返 回 值:
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/09/09	4.0		顾振华	创建
2006/02/17  4.0     顾振华  从界面移动到lib保存
    =============================================================================*/
	CMcuCfg()
    {
        Clear();
    }
    ~CMcuCfg()
    {
        Clear();
    }
	
    // 从 TMcuExt 结构复制
    CMcuCfg& operator =(const TMcuExt &tMcuExt);
	
	CMcuCfg& operator =(const TMcuEqpCfg &tEqp);
	
	CMcuCfg& operator =(const TMcuGeneralCfg &tGeneral);
	CMcuCfg& operator =(const TMcuNetCfg &tNetCardCfg);
	CMcuCfg& operator =(const CMcuCfg& cMcuCfg);
	
	void SetBrdCfg(s32 nNum, CBrdCfg* pcBrdCfg) ;
	void SetRecCfg(s32 nNum, TEqpRecCfgInfo* pcRecCfg) ;
	void SetVrsCfg(s32 nNum, TEqpVrsRecCfgInfo* pcVrsCfg);
    void SetHDBasCfg(s32 nNum, TEqpBasHDCfgInfo* pcHDBasCfg);
    void SetHDUStyleCfg(s32 nNum, THduStyleCfgInfo* pcHduStyleCfg);
	
    // 根据MCU上报更新单板状态，如果没有找到则返回FALSE
    BOOL32 UpdateBrdStatus(TBoardStatusNotify* const ptStatus) ;
    
	void SetInited(BOOL32 bInit = TRUE)
	{
		m_bInit = bInit;
	}
    BOOL32 IsInited()
    {
        return m_bInit;
    }
    void Clear()
    {
        ZeroMemory(this, sizeof(TMcuExt));
		m_tMcsMcuGeneralCfg.Clear();
        m_tplBrdCfg.Clear();
        m_tplRecCfg.Clear();
        m_tplHDBasCfg.Clear();
        SetInited(FALSE);
    }
	
    // 打印调试信息
	void PrintInfo();

	//是否启用调试模式
	void SetEnableDebug(u8 byEnableDebug)
	{
		m_byEnableDebug = byEnableDebug;
	}
	u8 GetEnableDebug()
	{
		return m_byEnableDebug;
	}
	TMcsMcuGeneralCfg* GetMcsMcuGeneralCfg()
	{
		return &m_tMcsMcuGeneralCfg;
	}
    
public:
    BOOL32              m_bInit;           // FALSE 表示本Mcu还未获取信息
 
	TMcsMcuGeneralCfg   m_tMcsMcuGeneralCfg;
    TplArray<THduStyleCfgInfo> m_tplHduStyleCfgInfo; //xts20090120HDU预案配置信息	
    TplArray<CBrdCfg>   m_tplBrdCfg;    
    TplArray<TEqpRecCfgInfo> m_tplRecCfg;   // 录像机配置
	TplArray<TEqpVrsRecCfgInfo> m_tplVrsCfg;   // Vrs配置
    TplArray<TEqpBasHDCfgInfo> m_tplHDBasCfg;   // 高清BAS板配置 
	TplArray<TCRIBrdExInfo>   m_tplCRIBrdExInfo; //CRI单板的授权信息

protected:
	u8 m_byEnableDebug;    //是否使用调试模式
};





///////////////////////////////////////////////////////////////////////////////
// CMtUtility

/*=============================================================================
函 数 名:GetMcuId
功    能:根据u16得到McuID
参    数:u16 wID                            [in]    u16
注    意:无
返 回 值:McuID
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline u16 CMtUtility::GetMcuId(u32 wID)
{
    return HIWORD16(wID);
}

/*=============================================================================
函 数 名:GetMtId
功    能:根据u16得到MtID
参    数:u16 wID                            [in]    u16
注    意:无
返 回 值:MtID
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline u8 CMtUtility::GetMtId(u32 wID)
{
    return (u8)LOWORD16(wID);
}

/*=============================================================================
函 数 名:GetwID
功    能:将TMt转化为u16
参    数:const TMt &tMt                     [in]    TMt
注    意:无
返 回 值:终端标识号
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline u32 CMtUtility::GetwID(const TMt &tMt)
{
    return MAKEDWORD((u16)tMt.GetMtId(), tMt.GetMcuIdx());
}

/*=============================================================================
函 数 名:GetwID
功    能:将McuID和MtID组合转化为u16
参    数:u8 byMcuId                         [in]    McuID
u8 byMtId                          [in]    MtID
注    意:无
返 回 值:终端标识号
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline u32 CMtUtility::GetwID(u16 byMcuId, u8 byMtId)
{
    return MAKEDWORD(byMtId, byMcuId);
}

/*=============================================================================
函 数 名:IsMcu
功    能:判断终端是否是MCU
参    数:const TMt &tMt                     [in]    终端
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMtUtility::IsMcu(const TMt &tMt)
{
    u8 byMtType = tMt.GetEqpType();
	//	tMt.GetType() == TYPE_MT
	//       &&
    if (  (byMtType == MT_TYPE_MMCU
		|| byMtType == MT_TYPE_SMCU
		|| byMtType == MT_TYPE_LOCALMCU) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:IsSMcu
功    能:判断终端是否是下级MCU
参    数:const TMt &tMt                     [in]    终端
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMtUtility::IsSMcu(const TMt &tMt)
{
    if (tMt.GetType() == TYPE_MT && tMt.GetEqpType() == MT_TYPE_SMCU)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:IsMMcu
功    能:判断终端是否是上级MCU
参    数:const TMt &tMt                     [in]    终端
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMtUtility::IsMMcu(const TMt &tMt)
{
    if (tMt.GetType() == TYPE_MT && tMt.GetEqpType() == MT_TYPE_MMCU)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:IsLocalMcu
功    能:判断终端是否是本级MCU
参    数:const TMt &tMt                     [in]    终端
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/05/13  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMtUtility::IsLocalMcu(const TMt &tMt)
{
    if ( tMt.GetEqpId() == 0 && tMt.GetMcuIdx() == LOCAL_MCUIDX )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*====================================================================
函 数 名： IsSMt
功    能： 判断是否是下级的终端
算法实现： 认为除了上级和本级之外所有的终端都是下级的终端
P.S 算法高效,但是不准确,必须事先确定查询的终端是会议中的终端
全局变量： 
参    数： const TMt &tMt				[in]需要判断的Mt
const TMt &tMMcu			[in]上级MCU
返 回 值： inline BOOL32 
--------------------------------------------------------------------
修改记录：
日  期	      版本		    修改人		走读人    修改内容
2006/5/29      4.0		    刘瑞飞                   创建
======================================================================*/
// inline BOOL32 CMtUtility::IsSMt( const TMt &tMt, const TMt &tMMcu )
// {
// 	if ( tMt.IsLocal() )
// 	{
// 		return FALSE;
// 	}
// 	
// 	u16 byMcuId = tMt.GetMcuIdx();
// 	//判断是否是上级mcu下的终端 
// 	if ( byMcuId == tMMcu.GetMcuIdx() )
// 	{
// 		return FALSE;
// 	}
// 	
// 	return TRUE;
// }

/*====================================================================
函 数 名： IsMMt
功    能： 判断是否是上级的终端 
算法实现： 高效但是不准确,必须由用户来确认终端在会议中
全局变量： 
参    数： const TMt &tMt				[in]需要判断的Mt
const TMt &tMMcu			[in]上级MCU,如果没有是空值
返 回 值： inline BOOL32 
--------------------------------------------------------------------
修改记录：
日  期	      版本		    修改人		走读人    修改内容
2006/5/29      4.0		    刘瑞飞                   创建
======================================================================*/
// inline BOOL32 CMtUtility::IsMMt( const TMt &tMt, const TMt &tMMcu )
// {
// 	if ( tMt.IsLocal() )
// 	{
// 		return FALSE;
// 	}
// 	
// 	u16 byMcuId = tMt.GetMcuIdx();
// 	if ( byMcuId != tMMcu.GetMcuIdx() )
// 	{
// 		return FALSE;
// 	}
// 	return TRUE;
// }


///////////////////////////////////////////////////////////////////////////////
// CBrdCfg

/*=============================================================================
函 数 名:operator=
功    能:赋值操作符
参    数:无
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/09/01  4.0     王昊    创建
=============================================================================*/
inline CBrdCfg& CBrdCfg::operator=(const TBrdCfgInfo &tBrdCfg)
{
    if ( this == &tBrdCfg )
    {
        return (*this);
    }
	
    memcpy( dynamic_cast<TBrdCfgInfo*>(this), &tBrdCfg, sizeof (TBrdCfgInfo) );
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
// CMt

/*=============================================================================
函 数 名:operator=
功    能:全部赋值
参    数:const CMt& cMt                     [in]    全部终端信息
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline CMt& CMt::operator=(const CMt &cMt)
{
    if (this == &cMt)
    {
        return (*this);
    }
	
    memcpy(this, &cMt, sizeof (CMt));

    return (*this);
}

/*=============================================================================
函 数 名:operator=
功    能:基本结构赋值
参    数:const TMt& tMt                     [in]    基本终端信息
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline CMt& CMt::operator=(const TMt &tMt)
{
    if (this == &tMt)
    {
        return (*this);
    }
	
    SetMt(tMt);
    return (*this);
}

/*=============================================================================
函 数 名:operator=
功    能:部分结构赋值
参    数:const TMtExt &tMtExt               [in]    部分终端信息
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
2012/05/29  4.0     徐太松  修改
=============================================================================*/
inline CMt& CMt::operator=(const TMtExtCur &tMtExt)
{
	m_tMtExt = tMtExt;

	if ( strcmp( tMtExt.GetAlias(),"(none)") == 0 )
	{
		
		m_tMtExt.SetAlias("");
	}

    SetMt(tMtExt);
    SyncAliasFromExt();
	
    return (*this);
}

/*=============================================================================
函 数 名:operator=
功    能:部分结构赋值
参    数:const TMtStatus &tMtStatus         [in]    部分终端信息
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/06/14  4.0     王昊    创建
=============================================================================*/
inline CMt& CMt::operator=(const TMtStatus &tMtStatus)
{
    SetMtStatus(tMtStatus);
    return (*this);
}

/*=============================================================================
函 数 名:operator==
功    能:比较操作符
参    数:const TMtStatus &tMtStatus         [in]    TMtStatus
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/27  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMt::operator==(const TMtStatus &tMtStatus) const
{
    if ( GetMcuIdx() == tMtStatus.GetMcuIdx()
        && GetMtId() == tMtStatus.GetMtId() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:operator==
功    能:比较操作符
参    数:const TMtExt &tMtExt               [in]    TMtExt
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/05/08  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMt::operator==(const TMtExtCur &tMtExt) const
{
    if ( GetMcuIdx() == tMtExt.GetMcuIdx()
        && GetMtId() == tMtExt.GetMtId() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:operator==
功    能:比较操作符
参    数:const TMt &tMt                     [in]    TMt
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/05/09  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMt::operator==(const TMt &tMt) const
{
    if ( (*dynamic_cast<TMt*>(const_cast<CMt*>(this))) == tMt)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
函 数 名:IsNull
功    能:判断终端是否为空
参    数:无
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/05/09  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMt::IsNull(void) const
{
    return dynamic_cast<TMt&>(const_cast<CMt&>(*this)).IsNull();
}

/*=============================================================================
函 数 名:GetwID
功    能:获取16位终端标识符
参    数:无
注    意:无
返 回 值:终端标识符
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline u32 CMt::GetwID(void) const
{
    return CMtUtility::GetwID(GetMcuIdx(), GetMtId());
}

/*=============================================================================
函 数 名:SetMt
功    能:设置TMt
参    数:const TMt &tMt                     [in]    TMt
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/26  4.0     王昊    创建
2013/07/03	4.0
=============================================================================*/
inline void CMt::SetMt(const TMt &tMt)
{
	*static_cast<TMt*>( this ) = tMt;
	*static_cast<TMt*>( &m_tMtExt ) = tMt;
	*static_cast<TMt*>( &m_tMtStatus ) = tMt;
	*static_cast<TMt*>( &m_tMtVersion ) = tMt;
}

/*=============================================================================
函 数 名:SetMt
功    能:设置TMt
参    数:const TMt &tMt                     [in]    TMt
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/26  4.0     王昊    创建
=============================================================================*/

/*=============================================================================
函 数 名:SetMtStatus
功    能:设置终端状态
参    数:const TMtStatus &tMtStatus         [in]    终端状态
注    意:无
返 回 值:现有m_tMtStatus与tMtStatus不一样:TRUE; 否则:FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/04/25  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CMt::SetMtStatus(const TMtStatus &tMtStatus)
{
    BOOL32 bRet = (memcmp(&m_tMtStatus, &tMtStatus, sizeof (TMtStatus)) != 0);
    m_tMtStatus = tMtStatus;
    if (tMtStatus.m_tRecState.IsNoRecording())
    {
        ZeroMemory(&m_tRecEqp, sizeof (m_tRecEqp));
        ZeroMemory(&m_tRecProg, sizeof (m_tRecProg));
    }
    return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// CSingleMcuInfo

///////////////////////////////////////////////////////////////////////////////
// CConf

/*=============================================================================
函 数 名:SetNull
功    能:清空
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/06/03  4.0     王昊    创建
=============================================================================*/
inline void CConf::SetNull(void)
{

	TConfInfo::Reset();
	//    ZeroMemory(&m_tTwModule,                    sizeof (m_tTwModule));
	//    ZeroMemory(&m_tVmpModule,                   sizeof (m_tVmpModule));
    ZeroMemory(m_abyLockUserName,               sizeof (m_abyLockUserName));
    ZeroMemory(&m_tRecEqp,                      sizeof (m_tRecEqp));
    ZeroMemory(&m_tPlayEqp,                     sizeof (m_tPlayEqp));
	ZeroMemory(m_atMcu, sizeof(m_atMcu) );
	
    m_dwGetInitValue    = 0;
    m_byLockBySelf      = 0;
    m_dwLockMcsIP       = 0;
    m_bPwdEntered       = FALSE;
	m_bReqConfSchema	= FALSE; //modified by twb 2012.11.20
	m_bHaveSchema		= TRUE;
    m_dwSN              = 0;
    m_dwLastRateReqTime = 0;
	
    m_cVcsConfStatus.RestoreStatus();
	if ( m_piMcu != NULL )
	{
		m_piMcu->DeleteAllItem();
	}
	else
	{
		m_piMcu = new CItemGroup<CMt, u32>;
	}
	
	
	/*	if ( m_piMcuIndex != NULL )
	{
	m_piMcuIndex->DeleteAllItem();
	}
	else
	{
	m_piMcuIndex = new CItemGroup<CMt,u32>();
	}
	*/
    TMtExtCur tMtExt;

    tMtExt.SetMcuIdx( LOCAL_MCUIDX );
	tMtExt.SetEqpId( 0 );
    tMtExt.SetMtType( MT_TYPE_LOCALMCU );
    m_cLocalMcu = tMtExt;
	m_cLocalMcu.SetOnline(TRUE);
	
	m_dwMcuNbr = 0;
	m_vctApplySpeaker.clear();	
}

/*=============================================================================
函 数 名:GetMixParam
功    能:获取混音参数
参    数:u8 byMcuId                         [in]    McuId
注    意:无
返 回 值:混音参数
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/06/13  4.0     王昊    创建
=============================================================================*/
inline TMixParam& CConf::GetMixParam(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return const_cast<TMixParam&>(m_tStatus.m_tMixParam);
    }
    else
    {
		IMcu* pMcu = GetIMcu( byMcuId );
		if ( pMcu == NULL )
		{
			return g_tNull.cNullMixParam;
		}
		CMt* pMt = pMcu->GetItemData();	
		if ( pMt == NULL )
		{
			return g_tNull.cNullMixParam;
		}
		
        return pMt->GetMixParam();
    }
}

/*=============================================================================
函 数 名:IsConfEntireMix
功    能:指定MCU是否全体混音
参    数:u8 byMcuId                         [in]    McuId
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/06/13  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CConf::IsConfEntireMix(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return m_tStatus.IsAutoMixing();
		//        return (m_tStatus.m_tConfMode.IsDiscussMode()
		//                && !m_tStatus.m_tConfMode.IsMixSpecMt());
    }
    else
    {
		TMixParam tMixParam = GetMixParam( byMcuId );
		return    tMixParam.GetMode() == mcuWholeMix
			|| tMixParam.GetMode() == mcuVacWholeMix;
		
		//        return (m_cMcuTable.GetMixParam(byMcuId).m_byMixMode == mcuWholeMix);
    }
}

/*=============================================================================
函 数 名:IsConfPartMix
功    能:指定MCU是否部分混音
参    数:u8 byMcuId                         [in]    McuId
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/06/13  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CConf::IsConfPartMix(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return m_tStatus.IsSpecMixing();
		//        return (m_tStatus.m_tConfMode.IsDiscussMode()
		//                && m_tStatus.m_tConfMode.IsMixSpecMt());
    }
    else
    {
		//        return (m_cMcuTable.GetMixParam(byMcuId).m_byMixMode == mcuPartMix);
		TMixParam tMixParam = GetMixParam( byMcuId );
		return    tMixParam.GetMode() == mcuPartMix
			|| tMixParam.GetMode() == mcuVacPartMix;
    }
}

/*=============================================================================
函 数 名:IsConfNoMix
功    能:指定MCU是否没有混音
参    数:u8 byMcuId                         [in]    McuId
注    意:无
返 回 值:BOOL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2006/08/04  4.0     王昊    创建
=============================================================================*/
inline BOOL32 CConf::IsConfNoMix(u16 byMcuId) const
{
    if (byMcuId == LOCAL_MCUIDX)
    {
        return m_tStatus.IsNoMixing();
        //return ( ! m_tStatus.m_tConfMode.IsDiscussMode() );
    }
    else
    {
		TMixParam tMixParam = GetMixParam( byMcuId );
		return    tMixParam.GetMode() == mcuNoMix
			|| tMixParam.GetMode() == mcuVacMix;
    }
}


/*=============================================================================
函 数 名:SetNull
功    能:会议模板置空
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/22  4.0     王昊    创建
=============================================================================*/
inline void CTemSchConf::SetNull(void)
{
	TConfInfo::Reset();
    m_bExist = FALSE;
	m_bIsHasVmpSubChnnl = FALSE;
	m_tVmpModuleEx.EmptyMember();
	m_tTWModules.Clear();
    m_tplMt.Clear();
	
    m_tplSubMcu.Clear();
	m_tMtAliasBack.SetNull();
	m_vctGroup.clear();

    m_tHdTvwall.SetNull();
    m_vctHduModChn.clear();
	m_tConfInfoEx.Clear();
	m_tHduVmpTotalEx.Empty();
    m_wPollTime = 0;
}

#define INVALID_INDEX 0xFFFFFFFF

class CMcuAddrBook
{
public:
    // 初始化成部分值
    CMcuAddrBook() :m_tplEntry(128, 32),
		m_tplGroup(16, 4)
	{		
	}
    ~CMcuAddrBook()
    {
        Clear();
    }
	
    /*=============================================================================
    函 数 名:Clear
    功    能:清空地址簿
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/
	void Clear()
	{
		m_tplEntry.Clear();
		m_tplGroup.Clear();
	}
	
    /*=============================================================================
    函 数 名:AddEntry
    功    能:向地址簿添加条目
    参    数:TMcuAddrEntry& tEntry          [in]    要添加的条目
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/
	void AddEntry(TMcuAddrEntryCur& tEntry);
	
    /*=============================================================================
    函 数 名:DelEntry
    功    能:从地址簿删除条目
    参    数:u32 dwIndex                    [in]    欲删除条目的EntryIdx
    注    意:无
    返 回 值:TRUE为成功删除，FALSE没有找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/
	BOOL32 DelEntry(u32 dwIndex);
    
    /*=============================================================================
    函 数 名:ModifyEntry
    功    能:从地址簿修改条目
    参    数:TMcuAddrEntry& tEntry          [in]    欲修改的条目，根据其EntryIdx查找
    注    意:无
    返 回 值:TRUE为成功修改，FALSE没有找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/
	BOOL32 ModifyEntry(TMcuAddrEntryCur& tEntry);
	
    /*=============================================================================
    函 数 名:AddGroup
    功    能:向地址簿添加组
    参    数:TMcuAddrGroup& tGroup          [in]    要添加的组
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/
	void AddGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    函 数 名:DelGroup
    功    能:从地址簿删除组
    参    数:u32 dwIndex                    [in]    欲删除组的EntryIdx
    注    意:无
    返 回 值:TRUE为成功删除，FALSE没有找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/    
	BOOL32 DelGroup(u32 dwIndex);
	
    /*=============================================================================
    函 数 名:ModifyGroup
    功    能:从地址簿修改组
    参    数:TMcuAddrGroup& tGroup          [in]    欲修改的组，根据其EntryIdx查找
    注    意:无
    返 回 值:TRUE为成功修改，FALSE没有找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/    
	BOOL32 ModifyGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    函 数 名:AddEntryToGroup
    功    能:向地址簿组添加条目
    参    数:TMcuAddrGroup& tGroup          [in]    添加的条目信息，根据组EntryIdx查找
    注    意:无
    返 回 值:TRUE为成功添加，FALSE没有找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/   
	BOOL32 AddEntryToGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    函 数 名:DelEntryFromGroup
    功    能:从地址簿组删除条目
    参    数:TMcuAddrGroup& tGroup          [in]    删除的条目信息，根据组EntryIdx查找
    注    意:无
    返 回 值:TRUE为成功添加，FALSE没有找到
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/   
	BOOL32 DelEntryFromGroup(TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    函 数 名:GetEntryCount
    功    能:获取地址簿条目数
    参    数:
    注    意:无
    返 回 值:条目数量
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/  
	u32  GetEntryCount()
    {
        return m_tplEntry.Size();
    }
	
    /*=============================================================================
    函 数 名:GetGroupCount
    功    能:获取地址簿组数
    参    数:
    注    意:无
    返 回 值:条目数量
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/  
	u32  GetGroupCount()
    {
        return m_tplGroup.Size();
    }
	
    /*=============================================================================
    函 数 名:IsEntryExist
    功    能:判断地址簿条目是否存在，根据EntryIdx查找
    参    数:TMcuAddrEntry& tEntry        [in]    目标条目
    注    意:无
    返 回 值:TRUE存在，FALSE不存在
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
	BOOL32 IsEntryExist(TMcuAddrEntryCur& tEntry) ;
	
    /*=============================================================================
    函 数 名:IsGroupExist
    功    能:判断地址簿组是否存在，根据EntryIdx查找
    参    数:TMcuAddrGroup& tGroup        [in]    目标组  
    注    意:无
    返 回 值:TRUE存在，FALSE不存在
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
	BOOL32 IsGroupExist(TMcuAddrGroupCur& tGroup) ;
	
    /*=============================================================================
    函 数 名:IsEntryExistInGroup
    功    能:判断地址簿条目是否存在在组中，根据EntryIdx查找
    参    数:
    注    意:无
    返 回 值:TRUE存在，FALSE不存在
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
	BOOL32 IsEntryExistInGroup(TMcuAddrEntryCur& tEntry, TMcuAddrGroupCur& tGroup);
	
    /*=============================================================================
    函 数 名:GetEntryByIndex
    功    能:按照条目索引查找
    参    数:u32 dwIndex                    [in]    条目索引
    注    意:无
    返 回 值:TMcuAddrEntry，如果不存在，则EntryIdx为 INVALID_INDEX
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    TMcuAddrEntryCur GetEntryByIndex(u32 dwIndex);
	
    /*=============================================================================
    函 数 名:GetGroupByIndex
    功    能:按照条目索引查找
    参    数:u32 dwIndex                    [in]    组索引
    注    意:无
    返 回 值:TMcuAddrGroup，如果不存在，则EntryIdx为 INVALID_INDEX
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    TMcuAddrGroupCur GetGroupByIndex(u32 dwIndex);
	
    /*=============================================================================
    函 数 名:GetEntryByName
    功    能:按照条目名称查找
    参    数:LPCTSTR lpszName               [in]    条目名称
	TMcuAddrEntry* ptEntry         [out]   返回的条目
    注    意:无
    返 回 值:u32 条目所在的物理地址，如果没有找到返回 INVALID_INDEX
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    u32 GetEntryByName(const s8* lpszName, TMcuAddrEntryCur* ptEntry);
	
    /*=============================================================================
    函 数 名:GetFirstEntry
    功    能:查找地址簿物理上第一个条目
    参    数:TMcuAddrEntry* ptEntry         [out] 结果条目
    注    意:无
    返 回 值:如果查找成功返回0(物理上第一个)，ptEntry 被填充
	否则返回INVALID_INDEX，结果EntryIdx为 INVALID_INDEX             
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    u32 GetFirstEntry(TMcuAddrEntryCur* ptEntry);
	
    /*=============================================================================
    函 数 名:GetNextEntry
    功    能:根据用户要求查找物理上第dwCurIndex+1开始的dwCount个条目
    参    数:u32 dwCurIndex                 [in]  开始条目的物理位置，不包括本位置的条目
	TMcuAddrEntry* ptEntryTable    [out] 结果条目表，用于填充结果
	u32& dwCount                   [in/out] 请求的条目数量，同时返回结果被实际填充的数量
    注    意:无
    返 回 值:u32 如果能填充1个或以上，则返回最后一个条目的物理位置；
	ptEntryTable 被填充，dwCount 返回实际填充数量
	否则返回INVALID_INDEX，dwCount 返回 0          
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    u32 GetNextEntry(u32 dwCurIndex, TMcuAddrEntryCur* ptEntryTable, u32& dwCount);
	
    /*=============================================================================
    函 数 名:GetFirstGroup
    功    能:查找地址簿物理上第一个组
    参    数:TMcuAddrGroup* ptGroup         [out] 结果组
    注    意:无
    返 回 值:如果查找成功返回0(物理上第一个)，ptGroup 被填充
	否则返回INVALID_INDEX，结果EntryIdx为 INVALID_INDEX             
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    u32 GetFirstGroup(TMcuAddrGroupCur* ptGroup);
	
    /*=============================================================================
    函 数 名:GetNextGroup
    功    能:根据用户要求查找物理上第dwCurIndex+1开始的dwCount个组
    参    数:u32 dwCurIndex                 [in]  开始组的物理位置，不包括本位置的组
	TMcuAddrEntry* ptEntryTable    [out] 结果组表，用于填充结果
	u32& dwCount                   [in/out] 请求的组数量，同时返回结果被实际填充的数量
    注    意:无
    返 回 值:u32 如果能填充1个或以上，则返回最后一个组的物理位置；
	ptEntryTable 被填充，dwCount 返回实际填充数量
	否则返回INVALID_INDEX，dwCount 返回 0          
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/02/23  4.0     顾振华  创建
    =============================================================================*/ 
    u32 GetNextGroup(u32 dwCurIndex, TMcuAddrGroupCur* ptGroupTable, u32& dwCount);
	
    // 调试打印信息
    void PrintInfo();
	
public:
	TplArray<TMcuAddrEntryCur> m_tplEntry;
	TplArray<TMcuAddrGroupCur> m_tplGroup;
};

// 组呼组的信息
typedef struct tagCallGroupInfo
{
    tagCallGroupInfo()
    {
        ZeroMemory( m_achGroupName, sizeof( m_achGroupName ) );
    }
	
    s8     m_achGroupName[MAX_VCSGROUPNAMELEN]; // 组呼组名;
    vector<TMt> m_vctMts;       //组内含有的终端  
	
}TCallGroupInfo, *PTCallGroupInfo;

//隐藏终端信息
typedef struct tagHideMtInfo 
{
public:
    tagHideMtInfo( u32 dwIP = 0, LPCTSTR strName = _T("") )
    {
        m_dwMtIP = dwIP;
        SetMtName( strName );
    }
    tagHideMtInfo( const tagHideMtInfo& tMtInfo )
    {
		//         m_dwMtIP = tMtInfo.m_dwMtIP;
		//         memcpy( m_strMtName, tMtInfo.m_strMtName, MAX_VCSGROUPNAMELEN );
        if ( this == &tMtInfo )
        {
            return ;
        } 
        *this = tMtInfo;
    }
    ~tagHideMtInfo(){}
	
    void SetIP( u32 dwIP ){ m_dwMtIP = dwIP; }
    u32 GetIP(){ return  m_dwMtIP; }
	
    void SetMtName( LPCTSTR pstrName ) { _tcscpy( m_strMtName, pstrName ); }
    LPCTSTR GetMtName(){ return m_strMtName; }
    
private:
    u32 m_dwMtIP;     //主机序
    TCHAR m_strMtName[VCS_MAXLEN_ALIAS];
	
}THideMtInfo, *PTHideMtInfo;

typedef struct tagChnInfo 
{
	u8		m_byDevId;             //通道所在设备ID
	u8		m_byChnId;
	
	tagChnInfo( u8 byDevId=0, u8 byChnlId=0 ){ m_byDevId = byDevId; m_byChnId = byChnlId; }
	void ResetChnl(){ ZeroMemory(this, sizeof(tagChnInfo)); }
	
	void SetChnl( u8 byDevId, u8 byChnlId ){ m_byDevId = byDevId; m_byChnId = byChnlId; }
	
}TChnlInfo, *PTChnlInfo;

typedef struct tagVcsTwChnItem 
{
	TChnlInfo		m_tChnl;			//电视墙板子
	u8				m_bySeq;			//通道序列号
	BOOL32			m_bActive;			//通道是否激活
	tagVcsTwChnItem(){ ZeroMemory(this, sizeof(tagVcsTwChnItem) ); }
}TVcsTwChnItem;

typedef struct tagDataPtr 
{
	tagDataPtr( u8 *pData = NULL, u32 nLen = 0){ m_pData = pData; m_nLen = nLen; }
	void SetNull(){ m_pData = NULL; m_nLen = 0; }
	void Free(){ DEL_PTR(m_pData); m_nLen = 0; }
	
	u8 *m_pData;
	u32 m_nLen;
}TDataPtr;

//---------------------------------------------------------------------
// 监控联动代码
typedef struct tagMonitorUnionInfo
{
	CMt tMt;
}TMonitorUnionInfo;


#define MAX_MSG_BODY_LEN	100

/*---------------------------------------------------------------------
* 类	名：CMonitorUnionMsgHeader
* 功    能：与VCS交互的消息头
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/08/11	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/
class CMonitorUnionMsgHeader
{
public:
	CMonitorUnionMsgHeader() : m_wEvent(0), m_wBodyLen(0){}
	u8 m_wEvent;
	u32 m_wBodyLen;
	u8 m_wBody[MAX_MSG_BODY_LEN];
public:
	void SetEvent( u8 wEvent ){ m_wEvent = wEvent; }
	u8 GetEvent(){ return m_wEvent; }
	BOOL32 SetBody( void* pBody, u32 dwBodyLen )
	{
		if ( dwBodyLen > MAX_MSG_BODY_LEN )
		{
			return FALSE;
		}
		if ( pBody == NULL || dwBodyLen == 0  )
		{
			return TRUE;
		}
		memcpy( m_wBody, pBody, dwBodyLen );
		m_wBodyLen = dwBodyLen;
		return TRUE;
	}
};

/*---------------------------------------------------------------------
* 类	名：tagMtReqJionConfInfo
* 功    能：终端主动请求vcs加入会议所带信息
* 特殊说明：
* 修改记录：
* 日期			版本		修改人		修改记录
* 2010/11/12	v1.0		牟兴茂		创建
----------------------------------------------------------------------*/


//终端主动加入会议  消息体：byEncrypt(1-加密,0-不加密)+
//bySrcDriId(u8)+TMtAlias(tFstAlias)+TMtAlias(tMtH323Alias)+
//TMtAlias(tMtE164Alias)+TMtAlias(tMtAddr)+U16(wCallRate)+u8(bySrcSsnId)
//#pragma pack(1) 
typedef struct tagMtReqJionConfInfo
{
	//由于获取名字的顺序  tFstAlias-》tMtE164Alias-》 tMtH323Alias-》 tMtAddr
	u8			byEncrypt;				// 是否加密
	u8			bySrcDriId;
	TMtAlias	tFstAlias;
	TMtAlias	tMtH323Alias;
	TMtAlias	tMtE164Alias;
	TMtAlias	tMtAddr;
	u16			wCallRate;
	u8			bySrcSsnId;
}TMtReqJionConfInfo; 




#endif      //  _MCSSTRUCT_20050422_H_