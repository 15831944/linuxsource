@echo ���ٱ���mtadplib��Ԫ����
@goto Stop

cd SimMcu\\prj_win32
call compile
cd..
cd..

cd SimMt\\prj_win32
call compile
cd..
cd..

cd SimSMcu\\prj_win32
call compile
cd..
cd..

:Stop
