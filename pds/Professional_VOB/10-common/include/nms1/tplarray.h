/*=============================================================================
模   块   名: 网管公用模块库
文   件   名: tplarray.h
相 关  文 件: 无
文件实现功能: 管理向量型数据的模板类
作        者: 李洪强
版        本: V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
说        明: 1.主要的功能是完成数组数据的存储和管理。
              2.本类适用的数据类型为简单的数据，即不需要另外申请空间，可以用memcpy实现赋值的功能。
              3.本模板类不支持嵌套, 不能和TplHash混用
              4.不需要MFC支持
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2004/12/18  3.5     李洪强      创建
2005/12/08  4.0     王昊        分离出单独文件
=============================================================================*/

#ifndef _TPLARRAY_20051208_H_
#define _TPLARRAY_20051208_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_ARRAY_STEP              (int)10 //  默认空间增加步长

template<class TItem> class TplArray
{
public:
    /*=============================================================================
    函 数 名:TplArray
    功    能:构造函数
    参    数:int nInitBufSize               [in]    初始分配空间的大小
             int nStep                      [in]    空间增加步长
    注    意:步长绝对不能为0, 大小可以为0
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    TplArray(int nInitBufSize = DEFAULT_ARRAY_STEP, int nStep = DEFAULT_ARRAY_STEP);

    /*=============================================================================
    函 数 名:TplArray
    功    能:拷贝构造函数
    参    数:const TplArray &tplArray       [in]    模板源地址
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    TplArray(const TplArray &tplArray);

    /*=============================================================================
    函 数 名:~TplArray
    功    能:析构函数
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    ~TplArray() { delete [] m_ptData; }

    /*=============================================================================
    函 数 名:operator=
    功    能:重载=运算符
    参    数:const TplArray &tplArray       [in]    用于赋值的源数组
    注    意:无
    返 回 值:this
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    2004/12/22  3.5     李洪强  取消memcpy,改用=号赋值
    =============================================================================*/
    inline TplArray& operator=(const TplArray &tplArray);

    /*=============================================================================
    函 数 名:Add
    功    能:在数组的末尾增加一组元素
    参    数:const TItem *pTItemTable       [in]    要增加的一组数据
             int nCount                     [in]    要增加的元素个数
    注    意:无
    返 回 值:最后一个元素的索引
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    int Add(const TItem *pTItemTable, int nCount = 1);

    /*=============================================================================
    函 数 名:Insert
    功    能:在指定位置插入一组元素
    参    数:int nIndex                     [in]    要插入的位置,位置的范围在当前数组大小的范围内
             TItem *pTItemTable             [in]    要插入的元素数组
             int nCount                     [in]    要插入的元素个数
    注    意:无
    返 回 值:成功返回插入的第一个元素的索引,失败返回-1
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    int Insert(int nIndex, TItem *pTItemTable, int nCount = 1);

    /*=============================================================================
    函 数 名:Delete
    功    能:删除指定位置的一组元素
    参    数:int nIndex                     [in]    删除元素的索引，在当前数组大小范围内
             int nCount                     [in]    删除元素的个数
    注    意:无
    返 回 值:成功返回实际删除的个数，失败返回-1
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    int Delete(int nIndex, int nCount = 1);

    /*=============================================================================
    函 数 名:SetAt
    功    能:修改指定位置的元素
    参    数:int nIndex                     [in]    要修改元素的索引值
             int nCount                     [in]    新的元素指针
    注    意:无
    返 回 值:成功返回TRUE, 失败返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline BOOL SetAt(int nIndex, TItem *pTItem);

    /*=============================================================================
    函 数 名:GetAt
    功    能:得到指定位置的元素
    参    数:int nIndex                     [in]    要获取元素的索引值
    注    意:无
    返 回 值:成功返回正确的元素，失败返回空元素
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline TItem& GetAt(int nIndex) const;

    /*=============================================================================
    函 数 名:GetItemPtr
    功    能:得到指定元素的指针
    参    数:int nIndex                     [in]    要获取的元素的索引
    注    意:无
    返 回 值:成功返回指针，失败返回NULL
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline const TItem* GetItemPtr(int nIndex) const;

    /*=============================================================================
    函 数 名:IsEmpty
    功    能:判断数组是否为空
    参    数:无
    注    意:无
    返 回 值:空返回TRUE，非空返回FALSE
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline BOOL IsEmpty(void) const { return m_nCount == 0; }

    /*=============================================================================
    函 数 名:Size
    功    能:获取当前数组大小
    参    数:无
    注    意:无
    返 回 值:当前数组大小
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline int Size(void) const { return m_nCount; }

    /*=============================================================================
    函 数 名:Clear
    功    能:释放所有申请的空间
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline void Clear(void);

    /*=============================================================================
    函 数 名:operator[]
    功    能:重载[]运算符
    参    数:int nIndex                     [in]    要取得元素的索引值
    注    意:无
    返 回 值:索引出的元素
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline TItem& operator[](int nIndex) const { return GetAt(nIndex); }

    /*=============================================================================
    函 数 名:GetData
    功    能:获取数据指针
    参    数:无
    注    意:有可能为空指针
    返 回 值:数据指针
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18	3.5		李洪强	创建
    =============================================================================*/
    inline TItem* GetData(void) const { return m_ptData; }

public:
    /*=============================================================================
    函 数 名:SetInvalidItem
    功    能:自定义无效的元素值
    参    数:const TItem &tItem             [in]    无效元素值
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    static inline void SetInvalidItem(const TItem &tItem);

    /*=============================================================================
    函 数 名:GetInvalidItem
    功    能:返回无效的元素
    参    数:无
    注    意:无
    返 回 值:无效元素值
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    static inline const TItem& GetInvalidItem(void) { return s_tInvalidItem; }

private:
    /*=============================================================================
    函 数 名:InitInvalid
    功    能:初始化无效元素
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    static inline void InitInvalid(void);

    /*=============================================================================
    函 数 名:MallocBuffer
    功    能:为数组分配内存
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    inline void MallocBuffer(void);

    /*=============================================================================
    函 数 名:GetSpace
    功    能:为指定大小的元素分配空间
    参    数:int nNewItemCount              [in]    新增元素的个数
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2004/12/18  3.5     李洪强  创建
    =============================================================================*/
    inline void GetSpace(int nNewItemCount);

protected:
    TItem           *m_ptData;      //  记录数组元素的数据指针

private:
    int             m_nMaxSize;     //  当前数组的最大元素个数
    int             m_nCount;       //  当前数组实际的元素个数
    int             m_nStep;        //  再分配空间时的递增的步长
    static TItem    s_tInvalidItem; //  无效的元素
    static bool     s_bInitInavlid; //  是否初始化过无效元素
};

template<class TItem> TItem TplArray<TItem>::s_tInvalidItem;
template<class TItem> bool TplArray<TItem>::s_bInitInavlid = false;

/*=============================================================================
函 数 名:TplArray
功    能:构造函数
参    数:int nInitBufSize               [in]    初始分配空间的大小
         int nStep                      [in]    空间增加步长
注    意:步长绝对不能为0, 大小可以为0
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18  3.5     李洪强  创建
=============================================================================*/
template<class TItem>
TplArray<TItem>::TplArray(int nInitBufSize /* = DEFAULT_ARRAY_STEP */,
                          int nStep /* = DEFAULT_ARRAY_STEP */)
        : m_nMaxSize(nInitBufSize), m_nCount(0), m_nStep(nStep), m_ptData(NULL)
{
    ASSERT(nStep > 0);
    nStep = (nStep == 0 ? DEFAULT_ARRAY_STEP : nStep);
    MallocBuffer();
    InitInvalid();
}

/*=============================================================================
函 数 名:TplArray
功    能:拷贝构造函数
参    数:const TplArray &tplArray       [in]    模板源地址
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18  3.5     李洪强  创建
=============================================================================*/
template<class TItem>
TplArray<TItem>::TplArray(const TplArray &tplArray)
        : m_nMaxSize(tplArray.m_nCount), m_nCount(tplArray.m_nCount),
          m_nStep(tplArray.m_nStep), m_ptData(NULL)
{
    m_nStep = (m_nStep == 0 ? DEFAULT_ARRAY_STEP : m_nStep);
    MallocBuffer();
    if ( tplArray.m_ptData != NULL )
    {
        memcpy( m_ptData, tplArray.m_ptData, sizeof (TItem) * m_nCount );
    }
}

/*=============================================================================
函 数 名:operator=
功    能:重载=运算符
参    数:const TplArray &tplArray       [in]    用于赋值的源数组
注    意:无
返 回 值:this
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
2004/12/22  3.5     李洪强  取消memcpy,改用=号赋值
=============================================================================*/
template<class TItem>
inline TplArray<TItem>& TplArray<TItem>::operator=(const TplArray &tplArray)
{
    if ( &tplArray != this )
    {
        m_nMaxSize = tplArray.m_nCount;
        m_nStep = tplArray.m_nStep;
        m_nCount = tplArray.m_nCount;

        MallocBuffer();
        if ( tplArray.m_ptData != NULL )
        {
            memcpy( m_ptData, tplArray.m_ptData, sizeof (TItem) * m_nCount );
        }
    }

    return (*this);
}

/*=============================================================================
函 数 名:Add
功    能:在数组的末尾增加一组元素
参    数:const TItem *pTItemTable       [in]    要增加的一组数据
         int nCount                     [in]    要增加的元素个数
注    意:无
返 回 值:最后一个元素的索引
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
int TplArray<TItem>::Add(const TItem *pTItemTable, int nCount /* = 1 */)
{
    ASSERT(nCount > 0);
    ASSERT(pTItemTable != NULL);

    GetSpace( nCount );
    memcpy( m_ptData + m_nCount, pTItemTable, nCount * sizeof (TItem) );
    m_nCount += nCount;

    return m_nCount - 1;
}

/*=============================================================================
函 数 名:Insert
功    能:在指定位置插入一组元素
参    数:int nIndex                     [in]    要插入的位置,位置的范围在当前数组大小的范围内
         TItem *pTItemTable             [in]    要插入的元素数组
         int nCount                     [in]    要插入的元素个数
注    意:无
返 回 值:成功返回插入的第一个元素的索引,失败返回-1
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
int TplArray<TItem>::Insert(int nIndex, TItem *pTItemTable, int nCount /* = 1 */)
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex <= m_nCount);
    ASSERT(nCount > 0);
    ASSERT(pTItemTable != NULL);

    if ( nIndex > m_nCount || nCount <= 0 ) 
    {
        return -1;
    }

    GetSpace( nCount );

    memmove( m_ptData + nIndex + nCount, m_ptData + nIndex, 
             (m_nCount - nIndex) * sizeof (TItem) );
    memcpy( m_ptData + nIndex, pTItemTable, nCount * sizeof (TItem) );

    m_nCount += nCount;

    return nIndex;
}

/*=============================================================================
函 数 名:Delete
功    能:删除指定位置的一组元素
参    数:int nIndex                     [in]    删除元素的索引，在当前数组大小范围内
         int nCount                     [in]    删除元素的个数
注    意:无
返 回 值:成功返回实际删除的个数，失败返回-1
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
int TplArray<TItem>::Delete(int nIndex, int nCount /* = 1 */)
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nCount);
    if ( nIndex < 0 || nIndex >= m_nCount ) 
    {
        return -1;
    }

    //防止删除越界
    nCount = (m_nCount > nIndex + nCount) ? nCount : (m_nCount - nIndex);

    memmove( m_ptData + nIndex, m_ptData + nIndex + nCount,
             (m_nCount - nCount - nIndex) * sizeof (TItem) );
    memset( m_ptData + (m_nCount - nCount), 0, nCount * sizeof (TItem) );
    m_nCount -= nCount;

    return nCount;
}

/*=============================================================================
函 数 名:SetAt
功    能:修改指定位置的元素
参    数:int nIndex                     [in]    要修改元素的索引值
         int nCount                     [in]    新的元素指针
注    意:无
返 回 值:成功返回TRUE, 失败返回FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
inline BOOL TplArray<TItem>::SetAt(int nIndex, TItem *pTItem)
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nCount);
    ASSERT(pTItem != NULL);

    if ( nIndex < 0 || nIndex >= m_nCount || pTItem == NULL ) 
    {
        return FALSE;
    }
    else
    {
        memcpy( m_ptData + nIndex, pTItem, sizeof (TItem) );
        return TRUE;
    }
}

/*=============================================================================
函 数 名:GetAt
功    能:得到指定位置的元素
参    数:int nIndex                     [in]    要获取元素的索引值
注    意:无
返 回 值:成功返回正确的元素，失败返回空元素
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
inline TItem& TplArray<TItem>::GetAt(int nIndex) const
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nCount);

    if (nIndex < 0 || nIndex >= m_nCount) 
    {
        return s_tInvalidItem;
    }
    else
    {
        return m_ptData[nIndex];
    }
}

/*=============================================================================
函 数 名:GetItemPtr
功    能:得到指定元素的指针
参    数:int nIndex                     [in]    要获取的元素的索引
注    意:无
返 回 值:成功返回指针，失败返回NULL
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
inline const TItem* TplArray<TItem>::GetItemPtr(int nIndex) const
{
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < m_nCount);

    if (nIndex < 0 || nIndex >= m_nCount) 
    {
        return NULL;
    }
    else
    {
        return m_ptData + nIndex;
    }
}

/*=============================================================================
函 数 名:Clear
功    能:释放所有申请的空间
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18	3.5		李洪强	创建
=============================================================================*/
template<class TItem>
inline void TplArray<TItem>::Clear(void)
{
    if ( m_ptData != NULL )
    {
        delete [] m_ptData;
        m_ptData = NULL;
    }
    m_nCount = 0;
    m_nMaxSize = 0;
}

/*=============================================================================
函 数 名:SetInvalidItem
功    能:自定义无效的元素值
参    数:const TItem &tItem             [in]    无效元素值
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18  3.5     李洪强  创建
=============================================================================*/
template<class TItem>
inline void TplArray<TItem>::SetInvalidItem(const TItem &tItem)
{
    s_tInvalidItem = tItem;
    s_bInitInavlid = true;
}

/*=============================================================================
函 数 名:InitInvalid
功    能:初始化无效元素
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18  3.5     李洪强  创建
=============================================================================*/
template<class TItem>
inline void TplArray<TItem>::InitInvalid(void)
{
    if ( ! s_bInitInavlid ) 
    {
        memset( &s_tInvalidItem, 0, sizeof (TItem) );
        s_bInitInavlid = true;
    }
}

/*=============================================================================
函 数 名:MallocBuffer
功    能:为数组分配内存
参    数:无
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18  3.5     李洪强  创建
=============================================================================*/
template<class TItem>
inline void TplArray<TItem>::MallocBuffer(void)
{
    if ( m_ptData != NULL )
    {
        delete [] m_ptData;
        m_ptData = NULL;
    }

    if ( m_nMaxSize > 0 )
    {
        m_ptData = new TItem [m_nMaxSize];
        ASSERT( m_ptData != NULL );
        memset( m_ptData, 0, m_nMaxSize * sizeof (TItem) );
    }
}

/*=============================================================================
函 数 名:GetSpace
功    能:为指定大小的元素分配空间
参    数:int nNewItemCount              [in]    新增元素的个数
注    意:无
返 回 值:无
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2004/12/18  3.5     李洪强  创建
=============================================================================*/
template<class TItem>
inline void TplArray<TItem>::GetSpace(int nNewItemCount)
{
    int nNewCount = m_nCount + nNewItemCount;
    if (m_nStep == 0)
    {
        m_nStep = DEFAULT_ARRAY_STEP;
    }
    if (nNewCount > m_nMaxSize) 
    {
        m_nMaxSize = (nNewCount / m_nStep + 1 ) * m_nStep;

        TItem *ptOldTable = m_ptData;
        m_ptData = new TItem [m_nMaxSize];
        if ( ptOldTable != NULL )
        {
            memcpy( m_ptData, ptOldTable, m_nCount * sizeof (TItem) );
            delete [] ptOldTable;
        }
    }
}

#endif  //  _TPLARRAY_20051208_H_
