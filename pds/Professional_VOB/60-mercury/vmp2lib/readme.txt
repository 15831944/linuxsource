BAS2 HD MT模块
==============================================================================
版本号：bas40.05.02.33.090516

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
    bug17245：实时更新发送地址
   
三、文档更新情况
	readme.txt
    mcuver.h
    basinst.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
    通过

六、系统测试建议
	多mp拔插切换 测试
	        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.32.081229-4.5.2

一、接口改变，新增功能 
    加密载荷设置修正。  

二、修改的bug(对应bug单中编号) 
   
三、文档更新情况
	readme.txt
	mcuver.h
	basinst.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况

六、系统测试建议
	在加密载荷会议下响应start和change，测试图像是否正常。
	        
七、提交人
    周文
==============================================================================

==============================================================================
版本号：bas40.05.02.31.081014-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
	QCIF/SQCIF 过滤修正；
   
三、文档更新情况
	readme.txt
	mcuver.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	高清终端 进 8000A 4画面以上合成 测试

六、系统测试建议
	无
	        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.30.081009-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
	请求关键帧过滤 修正
   
三、文档更新情况
	readme.txt
	mcuver.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	无
	        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.29.081007-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
	bug6858/7124 尝试修正：屏蔽重复的medianet参数设置操作
   
三、文档更新情况
	readme.txt
	mcuver.h	basinst.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	无
	        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.28.080923-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
	bug6926修正
   
三、文档更新情况
	readme.txt
	mcuver.h	basinst.h	evbas.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	无
	        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.27.080919-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
	保护bas不被cif的码流冲死
   
三、文档更新情况
	readme.txt
	mcuver.h
	basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	无
	        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.26.080904-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    
	1、第一路最小量化参数调整到10，第二路调整到30；
	2、单方面对D1进行细分保护，稍后整体移交给MCU；
	3、单方面对帧率保护，30帧的情况均降低到25帧处理，减轻dsp6437的压力；
	4、修正对底层帧率设置的手误；
	5、MCU_HDBAS_SETADAPTPARAM_CMD 回复屏蔽；
	6、对开启适配兼容性重入的临时调整；
   
三、文档更新情况
	readme.txt
	mcuver.h
	basinst.h
	basinst.h
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
    1、进行HD适配，查看第一路和第二路的最小量化参数，查看帧率；
	2、对D1情况下相关的输入输出，查看编解码的宽；
	3、起停双流适配，看bas是否工作正常；
        
七、提交人
    张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.26.080902-4.5.2

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    BUG6335: bas进行升级成功后，再重启一次bas会导致版本回退到老的版本
   
三、文档更新情况
	readme.txt basapp.cpp mcuver basapp.h
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
    进行升级测试
        
七、提交人
      周广程
==============================================================================

==============================================================================
版本号：bas40.05.02.25.080822

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 
    修正打印异常
   
三、文档更新情况
	readme.txt basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
    打开打印信息
        
七、提交人
      张永强
==============================================================================
==============================================================================
版本号：bas40.05.02.25.080822

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 

	1、Deamon实例向普通实例转发消息是没有把从CServMsg中取到的通道号加1转换成实例号
	2、放开所有通道
   
三、文档更新情况
	readme.txt basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	系统集成测试HD第一路码流 和 HD双流适配
        
七、提交人
      周广程
==============================================================================


==============================================================================
版本号：bas40.05.02.24.080822

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 

	版本号校验修正
   
三、文档更新情况
	mcuver.h readme.txt basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	集成配置高清BAP板
        
七、提交人
      张宝卿
==============================================================================


==============================================================================
版本号：bas40.05.02.23.080821

一、接口改变，新增功能   

二、修改的bug(对应bug单中编号) 

	1、通道索引约定调整，有baslib统一处理；
	2、增加适配帧率参数的设置；
   
三、文档更新情况
	mcuver.h readme.txt basinst.cpp
         
四、代码走读情况
    走读人：

五、单元测试情况
	未提交单元测试

六、系统测试建议
	系统集成测试HD第一路码流 和 HD双流适配
        
七、提交人
      张宝卿
==============================================================================

==============================================================================
版本号：bas40.05.02.22.080819

一、接口改变，新增功能   

	在InstanceEntry中增加对收到消息的打印

二、修改的bug(对应bug单中编号) 

   
三、文档更新情况
	mcuver.h readme.txt basinst.cpp
         
四、代码走读情况
    
五、单元测试情况

六、系统测试建议
        
七、提交人
      周广程
==============================================================================

==============================================================================
版本号：bas40.05.02.21.080814

一、接口改变，新增功能   

	1、修正上报通道状态时写错的消息号
	2、增加DEAMON向Instance发送的要求上报通道状态的消息和处理函数

二、修改的bug(对应bug单中编号) 

   
三、文档更新情况
	evbas.h basinst.h basinst.cpp
         
四、代码走读情况
    
五、单元测试情况

六、系统测试建议
        
七、提交人
      顾振华
==============================================================================

==============================================================================
版本号：bas40.01.01.19.080808

一、接口改变，新增功能   

	版本提交

二、修改的bug(对应bug单中编号) 

   
三、文档更新情况
         
四、代码走读情况
    
五、单元测试情况

六、系统测试建议
        
七、提交人
      顾振华
==============================================================================


   