/****************************************************************************
ģ����      ��������Ϣ����ģ��
�ļ���      ��exceptioncatch.h
����ļ�    ��exceptioncatch.cpp
�ļ�ʵ�ֹ��ܣ���������
����        ��������
�汾        ��V1.0 Copyright(C) 2009-2009 KDC, All rights reserved.
��ע        ��ֻ������Windows OS 
�÷�        ��1. �����cpp�ļ���include "exceptioncatch.h" 
              2. �����߳���ڵ���CExceptionCatch::Start()���ر�����Ϣ����ģ��
              3. �����ļ��ڳ���ִ��Ŀ¼�µ�KDCException.txt�ļ���Ҳ�����Զ�������ļ�CExceptionCatch::LogToFile("�ļ�ȫ·��")
              4. ����CExceptionCatch::Free()ж�ر�����Ϣģ��
-----------------------------------------------------------------------------
�޸ļ�¼��
����       �汾    �޸���    �޸����� 
2009/03/16 1.0     ������    ����
2013/07/17 2.0     �޿���    �޸���֧��UNICODE
*****************************************************************************/

#if !defined(EXCEPTION_CATCH_H)
#define EXCEPTION_CATCH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "stdafx.h"

#include <imagehlp.h>
#include <stdio.h>
#include <assert.h>
#include "unicode.h"

#pragma comment (lib, "imagehlp")

//��������ļ�·��
static TCHAR m_pszLogFile[MAX_PATH] = {0};

//������ϢException Filter
static LPTOP_LEVEL_EXCEPTION_FILTER m_pPreExcepAddress;

//������Ϣ�����
class CExceptionOutput  
{
public:
	CExceptionOutput()
	{
			ZeroMemory ( m_abyFilePath, sizeof(m_abyFilePath) );
			_tcscpy( m_abyFilePath, _T("Output.txt") );
	}
	virtual ~CExceptionOutput(){}
	CExceptionOutput( LPCTSTR pch )
	{
		assert ( pch != NULL );
		_tcscpy ( m_abyFilePath, pch );
	}
	
	//��������ļ���
	void SetFileName(LPCTSTR pch)
	{
		assert (pch != NULL);
		_tcscpy ( m_abyFilePath, pch );
	}

	LPCTSTR GetFileName()
	{
		return m_abyFilePath;
	}
	
	//�����
	void WriteString( LPCTSTR pstr )
	{
	#ifdef UNICODE
		CreateFile();
	#endif
		FILE *fp = _tfopen( m_abyFilePath, _T("ab") );
		if ( fp != NULL )
		{
			fwrite( pstr, sizeof(TCHAR), _tcslen(pstr), fp );
			fclose( fp );
		}
	}
	
	//����һ��
	void NewLine()
	{
	#ifdef UNICODE
		CreateFile();
	#endif
		LPCTSTR lpszRtn = _T("\n");
		FILE *fp = _tfopen( m_abyFilePath, _T("ab") );
		if ( fp != NULL )
		{
			fwrite( lpszRtn, sizeof(TCHAR), _tcslen(lpszRtn), fp );
			fclose( fp );
		}
	}
	
	//��ó���ִ��·��
	static CString GetModuleFullPath(void)
	{
		TCHAR achDllName[MAX_PATH] = _T("");
		CString csPath = _T("");
		
		GetModuleFileName(AfxGetInstanceHandle(), achDllName, sizeof (achDllName));
		
		csPath = achDllName;
		csPath = csPath.Left( csPath.ReverseFind('\\') + 1 );
		
		return csPath;
	}

#ifdef UNICODE
	void CreateFile()
	{
		FILE *fp = _tfopen( m_abyFilePath, _T("rb+") );
		if ( fp != NULL )
		{
			fseek( fp, 0, SEEK_END );
			size_t dwSize = ftell( fp );
			fseek( fp, 0, SEEK_SET );
			if ( dwSize > 0 )
			{
				TCHAR byHead = 0;
				fread( &byHead, sizeof(TCHAR), 1, fp );
				if ( byHead != 0xFEFF )
				{
					fclose( fp );
					fp = _tfopen( m_abyFilePath, _T("wb+") );
					if ( fp )
					{
						dwSize = 0;
					}
				}			
			}
			if ( dwSize == 0 && fp )
			{
				TCHAR byUtf16Head = 0xFEFF;
				fwrite( &byUtf16Head, sizeof(TCHAR), 1, fp );
				fclose( fp );
			}
		}
	}
#endif
private:
	TCHAR m_abyFilePath[MAX_PATH];
};

//������Ϣ������
class CExceptionCatch
{
public:
	static void Start()
	{
		if (NULL == m_pPreExcepAddress) 
		{
			SetErrorMode (SEM_NOGPFAULTERRORBOX);
			m_pPreExcepAddress = SetUnhandledExceptionFilter (CallBackDebugInfo);
		}
	}
	
	
	static void Free()
	{
		SetErrorMode (0);
		SetUnhandledExceptionFilter (m_pPreExcepAddress);
	}
	
	static void LogToFile( LPCTSTR pszFile)
	{
		_tcsncpy( m_pszLogFile, pszFile, _countof( m_pszLogFile ) );
	}
protected:
	static LONG WINAPI CallBackDebugInfo(EXCEPTION_POINTERS *pException)
	{
		// �õ��쳣����Ϣ
		PEXCEPTION_RECORD pExcp = pException->ExceptionRecord;
		while (NULL != pExcp) 
		{
			DealExcep (pExcp);
			pExcp = pExcp->ExceptionRecord;
		}
		
		//��ӡ��ջ��Ϣ
		ImgStackWalk (pException->ContextRecord);
		if (m_pPreExcepAddress) 
		{
			return m_pPreExcepAddress (pException);
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}
	static void	DealExcep(PEXCEPTION_RECORD pExcp)
	{
		TCHAR szFileInfo[3 * MAX_PATH] = {0};
		TCHAR szExceptionInfo[MAX_PATH] = {0};
		
		// ��ӡ��ʾͷ
		_stprintf( szExceptionInfo, _T("======================== ������Ϣ ==========================\n") );
		_tcscpy (szFileInfo, szExceptionInfo);
		
		// ��õ�ǰʱ��
		SYSTEMTIME st;
		GetLocalTime(&st);  
		_stprintf(szExceptionInfo, _T("����ʱ�䣺 %4d/%.2d/%.2d %.2d:%.2d:%.2d\n"),
			st.wYear, st.wMonth,  st.wDay,
			st.wHour, st.wMinute, st.wSecond);
		_tcscat(szFileInfo, szExceptionInfo);
		
		_tcscat(szFileInfo, _T("����ԭ�� "));
		
		// �ж��쳣ԭ��
		switch (pExcp->ExceptionCode)
		{
		case STATUS_ACCESS_VIOLATION:
			_stprintf(szExceptionInfo, _T("�Ƿ��ڴ����\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			_stprintf(szExceptionInfo, _T("�������Խ��\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_BREAKPOINT:
			_stprintf(szExceptionInfo, _T("����breakpoint\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			_stprintf(szExceptionInfo, _T("The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			_stprintf(szExceptionInfo, _T("One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: 
			_stprintf(szExceptionInfo, _T("�����0. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_INEXACT_RESULT: 
			_stprintf(szExceptionInfo, _T("The result of a floating-point operation cannot be represented exactly as a decimal fraction. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			_stprintf(szExceptionInfo, _T("This exception represents any floating-point exception not included in this list. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_OVERFLOW :
			_stprintf(szExceptionInfo, _T("The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_STACK_CHECK :
			_stprintf(szExceptionInfo, _T("The stack overflowed or underflowed as the result of a floating-point operation. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_FLT_UNDERFLOW :
			_stprintf(szExceptionInfo, _T("The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION :
			_stprintf(szExceptionInfo, _T("The thread tried to execute an invalid instruction. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_IN_PAGE_ERROR :
			_stprintf(szExceptionInfo, _T("The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_INT_OVERFLOW: 
			_stprintf(szExceptionInfo, _T("The result of an integer operation caused a carry out of the most significant bit of the result. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_INVALID_DISPOSITION :
			_stprintf(szExceptionInfo, _T("(���쳣���������ϵͳʹ�õĴ���)An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION :
			_stprintf(szExceptionInfo, _T("(���ɳ����쳣,�����޷��ָ�ִ��,�쳣�������̲�Ӧ��������쳣)The thread tried to continue execution after a noncontinuable exception occurred. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_PRIV_INSTRUCTION :
			_stprintf(szExceptionInfo, _T("The thread tried to execute an instruction whose operation is not allowed in the current machine mode. \n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		case EXCEPTION_SINGLE_STEP :
			_stprintf(szExceptionInfo, _T("A trace trap or other single-instruction mechanism signaled that one instruction has been executed.	\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
//		case STATUS_STACK_OVERFLOW:
/////		case EXCEPTION_STACK_OVERFLOW:
//			_stprintf (szExceptionInfo, _T("��ջ���\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
// 			break;
		case STATUS_INTEGER_DIVIDE_BY_ZERO:
			_stprintf (szExceptionInfo, _T("���ͳ���Ϊ0\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		default:
			_stprintf (szExceptionInfo, _T("�����쳣\n�쳣���� = %8x\n"), pExcp->ExceptionCode);
			break;
		}
		
		// �쳣��ַ
		lstrcat (szFileInfo, szExceptionInfo);
		_stprintf (szExceptionInfo, _T("\n�쳣��ַ = 0x%.8x \n�쳣ģ��: "), pExcp->ExceptionAddress);
		lstrcat (szFileInfo, szExceptionInfo);
		
		// �õ��쳣���ڵ�module
		MEMORY_BASIC_INFORMATION mem;
		VirtualQuery (pExcp->ExceptionAddress, &mem, sizeof (MEMORY_BASIC_INFORMATION));
		GetModuleFileName ((HMODULE)mem.AllocationBase, szExceptionInfo, sizeof (szExceptionInfo));
		lstrcat (szFileInfo, szExceptionInfo);
		
		// ��λ�쳣��ƫ��λ��(��Ե�ַ)
		PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)(mem.AllocationBase);
		PIMAGE_NT_HEADERS pNts = (PIMAGE_NT_HEADERS)((PBYTE)pDos + pDos->e_lfanew);
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION (pNts);
		
		// ���ƫ��λ��(��Ե�ַ)
		DWORD dRva = (DWORD)pExcp->ExceptionAddress - (DWORD)mem.AllocationBase;
		
		//  ����ÿ��section.
		for (WORD wCnt = 0; wCnt < pNts->FileHeader.NumberOfSections; ++wCnt) 
		{
			DWORD dStart = pSection->VirtualAddress;
			DWORD dEnd = dStart + max (pSection->SizeOfRawData, pSection->Misc.VirtualSize);
			if (dRva >= dStart && dRva <= dEnd) 
			{
				_stprintf (szExceptionInfo, _T("\nSection name: %s - offset(rva) : 0x%.8x\n"), CA2T( LPCSTR( pSection->Name ) ), dRva - dStart);
				_tcscat (szFileInfo, szExceptionInfo);
				break;
			}
			++pSection;
		}
		
		// ���û�������ļ�������Ĭ��ֵ
		if ( _tcslen(m_pszLogFile) == 0 ) 
		{
			_tcscpy(m_pszLogFile,  CExceptionOutput::GetModuleFullPath() + _T("KDCException.txt"));
		} 
		
		// �����ӡ���ļ�
		CExceptionOutput out(m_pszLogFile);
		out.WriteString (szFileInfo);
		out.NewLine();
		out.NewLine();
    }

	static void	ImgStackWalk(PCONTEXT pCon)
	{
		TCHAR szFileInfo[MAX_PATH * 2] = {0};
		TCHAR szExceptionInfo[MAX_PATH] = {0};
	
		// �Ȼ�ȡģ������
		MEMORY_BASIC_INFORMATION mem;
		VirtualQuery((PVOID)pCon->Eip, &mem, sizeof (MEMORY_BASIC_INFORMATION));
		GetModuleFileName( (HMODULE)mem.AllocationBase, szExceptionInfo, _countof(szExceptionInfo) );
		_tcscpy(szFileInfo, szExceptionInfo);
		
		
		if (SymInitialize(GetCurrentProcess (), NULL, TRUE)) 
		{
			STACKFRAME sf;
			memset(&sf, 0x00, sizeof (STACKFRAME));
			
			// ��ʼ��STACKFRAME����ʾ��ǰ�̵߳ĵ��ö�ջ�е�һ���������ã��ṹ��ֻ������Intel CPU
			sf.AddrPC.Offset       = pCon->Eip;
			sf.AddrPC.Mode         = AddrModeFlat;
			sf.AddrStack.Offset    = pCon->Esp;
			sf.AddrStack.Mode      = AddrModeFlat;
			sf.AddrFrame.Offset    = pCon->Ebp;
			sf.AddrFrame.Mode      = AddrModeFlat;
			
			
			while (true) 
			{
				if (!StackWalk (IMAGE_FILE_MACHINE_I386,
					GetCurrentProcess (),
					GetCurrentThread (),
					&sf,
					pCon,
					NULL,
					SymFunctionTableAccess,
					SymGetModuleBase,
					NULL))
				{
					break;
				}
				
				
				// ����ջ֡���������˳�
				if ( 0 == sf.AddrFrame.Offset ) 
				{
					break;
				}
				
				// ����image����
				BYTE imgBuf[sizeof (IMAGEHLP_SYMBOL) + 512];
				PIMAGEHLP_SYMBOL pSymbol = reinterpret_cast <PIMAGEHLP_SYMBOL> (imgBuf);
				pSymbol->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL);
				pSymbol->MaxNameLength = 512;
				DWORD dLen = 0;
				if (SymGetSymFromAddr (GetCurrentProcess(), sf.AddrPC.Offset,
					&dLen, pSymbol))
				{
					_stprintf (szExceptionInfo, _T("\nname : %s - location: %x"), pSymbol->Name, dLen);
					_tcscat (szFileInfo, szExceptionInfo);
				}
			}
			SymCleanup (GetCurrentProcess ());
		}
		
		//���û�������ļ�������Ĭ��ֵ
		if ( _tcslen( m_pszLogFile ) == 0 ) 
		{
			_tcscpy( m_pszLogFile, CExceptionOutput::GetModuleFullPath() + _T("KDCException.txt") );
		} 
		
		//������ļ�
		CExceptionOutput out(m_pszLogFile);
		out.WriteString( _T("---------------------- Trips of Stack ----------------------\n") );
		out.WriteString( szFileInfo );
		out.NewLine();
		out.NewLine();
	}
protected:
	///static	LPTOP_LEVEL_EXCEPTION_FILTER m_pPreExcepAddress;
	///static	char	m_pszLogFile[MAX_PATH];
	
};

#endif // !defined(EXCEPTION_CATCH_H)
