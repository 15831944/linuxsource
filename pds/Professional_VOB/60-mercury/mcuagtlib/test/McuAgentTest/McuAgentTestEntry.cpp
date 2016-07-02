
// mcuagentTestEntry.cpp
//#pragma once 

#include "Testevent.h"
#include "McuAgentTestEntry.h"
#include "abnormitytest.h"
#include "boardaction.h"
#include "NmsActTest.h"
#include "McuAction.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CMcuActTest );
CPPUNIT_TEST_SUITE_REGISTRATION(CBoardActTest);
CPPUNIT_TEST_SUITE_REGISTRATION(CAbnormityTest);
CPPUNIT_TEST_SUITE_REGISTRATION(CNmsActTest);

CTestEntry theApp;

/*=============================================================================
  �� �� ���� InitInstance
  ��    �ܣ� ���Գ������
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� BOOL 
=============================================================================*/
BOOL CTestEntry::InitInstance()
{
    OspInit(TRUE);

    g_cSimuMcuApp.CreateApp("SimuMcu", APP_SIMUMCUSSN, 60);

    g_cSimuBrdApp.CreateApp("SimuBrd", APP_SIMUBRDSSN, 61);
    post(MAKEIID(APP_SIMUBRDSSN, 1), BOARD_POWERON);
    
    g_cSimuNmsApp.CreateApp("SimuNms", APP_SIMUNMSSSN, 62);

    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	
    CppUnit::MfcUi::TestRunner runner;
    runner.addTest( registry.makeTest() );
	
	runner.run( );
	
	return TRUE;

}

/*=============================================================================
  �� �� ���� ExitInstance
  ��    �ܣ� �˳����Գ���
  �㷨ʵ�֣� 
  ȫ�ֱ����� 
  ��    ���� 
  �� �� ֵ�� int 
=============================================================================*/
int CTestEntry::ExitInstance()
{
    OspQuit();
	return CWinApp::ExitInstance();
}