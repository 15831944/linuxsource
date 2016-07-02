/*! \file libsswrap.h
    \brief �ṩ����ƽ������֧��.

    ƽ������ģ�������ϲ�Ӧ��ͨ��API���÷���ָ��Ŀ�صı��ĵ�ƽ����������,
    ������һ����Χ�ڵ�ͻ������.

    ��Ȩ���� 2007 kedacom \n
    �汾: 1.0 \n
    ��ʷ:  \n
      - ��ʼ�汾.     ��Ԫһ(070131)

    \author ��Ԫһ
    \date 2007-1-30
	
    \2013/3/8��������������ã��Լ������õ�ʱ��
    \author ��ר
*/

/** @defgroup ss ƽ������
 *  �ṩ����ƽ������֧��.
 *  @{
 */


#ifndef SSWRAP_H
#define SSWRAP_H

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * �����붨��.
 */
enum ErrCode
{
    ENOSUPP,  ///< �ں˲�֧��
    ENOROUTE, ///< Ŀ�ص�ַ���ɴ�
    ENORULE,  ///< ���򲻴���
    EINTERNAL,///< �ڲ�����
    ETOOMANY, ///< ̫��Ĺ���
};

extern int SSErrno; ///< ƽ������ģ�������,��ͨ���˱�����ȡ����ϸ��

/**
 * ���/�޸�һ��ƽ�����͹���,����˿ں�Ϊ0,��ƥ��˿ں�.\n
 * <b>ע��</b>: ��ֵ���ʱ�����ڻ��ߵ���ƽ������,�����������Ŀǰ�����̰߳�ȫ��
 *
 * \param dwDestIp     Ҫ�������ٵı��ĵ�Ŀ��IP,����Ϊ�����ֽ���.
 * \param wDestPort    Ҫ�������ٵı��ĵ�Ŀ�ض˿�,����Ϊ�����ֽ���.
 * \param dwRate       ���ĵ�ƽ����������,��λΪKBps
 * \param dwPeak       ���ĵķ�ֵ����,��λΪKBps.
 * \param dwSec        ��ֵ���ʳ���ʱ��
 * \return ����ɹ�����1,ʧ���򷵻�0 <br>
 *         ����ͨ��SSErrno������ȡ����ϸ��,���ܵĴ������: <br>
 *           - <b>EINVAL</b>    ��Ч�Ĳ��� <br>
 *           - <b>ENOROUTE</b>  Ŀ�ص�ַ���ɴ�. <br>
 *           - <b>ENOSUPP</b>   �ں˲�֧��. <br>
 *           - <b>ETOOMANY</b>  ̫��Ĺ���.<br>
 *           - <b>EINTERNAL</b> �ڲ�����.<br>
 */
s32 SetSSRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/**
 * ɾ��һ��ƽ�����͹���,����˿ں�Ϊ0,��ƥ��˿ں�.<br>
 * <b>ע��</b>: �������Ŀǰ�����̰߳�ȫ��.
 *
 * \param dwDestIp   Ҫɾ���Ĺ����Ŀ��IP.
 * \param wDestPort  Ҫɾ���Ĺ����Ŀ�ض˿�.
 * \return ����ɹ�����1,ʧ���򷵻�0 <br>
 *         ����ͨ��SSErrno������ȡ����ϸ��,���ܵĴ������: <br>
 *           - <b>EINVAL</b>    ��Ч�Ĳ��� <br>
 *           - <b>ENOROUTE</b>  Ŀ�ص�ַ���ɴ�. <br>
 *           - <b>ENOSUPP</b>   �ں˲�֧��. <br>
 *           - <b>EINTERNAL</b> �ڲ�����.<br>
 */
s32 UnsetSSRule(u32 dwDestIp, u16 wDestPort);

/**
 * ���/�޸�һ���鲥ƽ�����͹���,����˿ں�Ϊ0,��ƥ��˿ں�.\n
 * <b>ע��</b>: ��ֵ���ʱ�����ڻ��ߵ���ƽ������,�����������Ŀǰ�����̰߳�ȫ��
 *
 * \param dwSrcIp     �����鲥���ĵı�������ip��ַ,����Ϊ�����ֽ���.
 * \param wDestPort    Ҫ�������ٵı��ĵ�Ŀ�ض˿�,����Ϊ�����ֽ���.
 * \param dwRate       ���ĵ�ƽ����������,��λΪKBps
 * \param dwPeak       ���ĵķ�ֵ����,��λΪKBps.
 * \param dwSec        ��ֵ���ʳ���ʱ��
 * \return ����ɹ�����1,ʧ���򷵻�0 <br>
 *         ����ͨ��SSErrno������ȡ����ϸ��,���ܵĴ������: <br>
 *           - <b>EINVAL</b>    ��Ч�Ĳ��� <br>
 *           - <b>ENOROUTE</b>  Ŀ�ص�ַ���ɴ�. <br>
 *           - <b>ENOSUPP</b>   �ں˲�֧��. <br>
 *           - <b>ETOOMANY</b>  ̫��Ĺ���.<br>
 *           - <b>EINTERNAL</b> �ڲ�����.<br>
 */
s32 SetMcSSRule(u32 dwSrcIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/**
 * ɾ��һ��ƽ�����͹���,����˿ں�Ϊ0,��ƥ��˿ں�.<br>
 * <b>ע��</b>: �������Ŀǰ�����̰߳�ȫ��.
 *
 * \param dwDestIp   Ҫɾ���Ĺ����Դip
 * \param wDestPort  Ҫɾ���Ĺ����Ŀ�ض˿�.
 * \return ����ɹ�����1,ʧ���򷵻�0 <br>
 *         ����ͨ��SSErrno������ȡ����ϸ��,���ܵĴ������: <br>
 *           - <b>EINVAL</b>    ��Ч�Ĳ��� <br>
 *           - <b>ENOROUTE</b>  Ŀ�ص�ַ���ɴ�. <br>
 *           - <b>ENOSUPP</b>   �ں˲�֧��. <br>
 *           - <b>EINTERNAL</b> �ڲ�����.<br>
 */
s32 UnsetMcSSRule(u32 dwSrcIp, u16 wDestPort);


/**
 * �����нӿ��Ϲر�ƽ�����͹���.<br>
 * <b>ע��</b>: �������Ŀǰ�����̰߳�ȫ��.
 *
 * \return ����ɹ�����1,ʧ���򷵻�0 <br>
 *         ����ͨ��SSErrno������ȡ����ϸ��,���ܵĴ������: <br>
 *           - <b>ENOSUPP</b>   �ں˲�֧��. <br>
 *           - <b>EINTERNAL</b> �ڲ�����.<br> 
 */
s32 CloseSmoothSending();

/**
 * ��ʾ��ǰ����ƽ������.<br>
 * <b>ע��</b>: �������Ŀǰ�����̰߳�ȫ��.
 *
 * \return ����ɹ�����1,ʧ���򷵻�0 <br>
 *         ����ͨ��SSErrno������ȡ����ϸ��,���ܵĴ������: <br>
 *           - <b>ENOSUPP</b>   �ں˲�֧��. <br>
 *           - <b>EINTERNAL</b> �ڲ�����.<br> 
 */
s32 ShowSSRule();


//2013/3/08�����ӵĹ���������ӽӿ�
/************************************************/
/*
**�ӿڣ�s32 SsInt(void);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**ִ��������������ƽ�����͹����ʼ���á�
**���ã�ʹ�õ��øú����Ľ��̼����ӽ��̵�pioֵ-20��pio��ֵԽС�����ȼ���Խ�ߣ��������ļ���ʼ����
**Add by����ר ---------2013/3/8------------
*/
s32 SsInit(void);

/*
**�ӿڣ�s32 BatchAddSSRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**���ã��򻺴������һ�����򣬹�����Ч��
**Add by����ר ---------2013/3/8------------
*/
s32 BatchAddSSRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/*
**�ӿڣ�s32 BatchExecSSRule(void);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**���ã���Ч������������ӵĹ���
**Add by����ר ---------2013/3/8------------
*/
s32 BatchExecSSRule(void);

/*
**�ӿڣ�s32 BatchAddUnsetSSRule(u32 dwDestIp, u16 wDestPort);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**���ã��򻺴������һ��unset���򣬹�����Ч��
**Add by����ר ---------2013/3/8------------
*/
s32 BatchAddUnsetSSRule(u32 dwDestIp, u16 wDestPort);

/*
**�ӿڣ�s32 BatchExecUnsetSSRule(void);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**���ã���Ч������������ӵ�unset����
**Add by����ר ---------2013/3/8------------
*/
s32 BatchExecUnsetSSRule(void);

/*
**�ӿڣ�s32 BatchAddMcRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**���ã��򻺴������һ���鲥ƽ�����͹��򣬹�����Ч��
**Add by����ר ---------2013/3/8------------
*/
s32 BatchAddMcRule(u32 dwDestIp, u16 wDestPort, u32 dwRate, u32 dwPeak, u32 dwSec);

/*
**�ӿڣ�s32 BatchExecMcRule(void);
**����ֵ������ִ�гɹ�����1��ʧ�ܷ���0��
**����:��Ч������������ӵ��鲥ƽ�����͹���
**Add by����ר ---------2013/3/8------------
*/
s32 BatchExecMcRule(void);



#ifdef __cplusplus
}
#endif

/* @} */

#endif /* SS_H */

