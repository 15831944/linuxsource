
MP模块

=============================================================================================
版本号：mp40.01.01.26.091214

一、接口改变，新增功能

    对组播包，进行medianet转发，针对卫星会议

二、修改的bug(对应bug单中编号)

    
三、文档更新情况
         
四、代码走读情况     

五、单元测试情况
   
六、系统测试建议
           
七、提交人
      顾振华
=============================================================================================

=============================================================================================
版本号：mp40.01.01.25.081008
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

	FU_A 检测支持——H264关键帧
    
三、文档更新情况
         
四、代码走读情况     

五、单元测试情况
   
六、系统测试建议
           
七、提交人
      顾振华
=============================================================================================

=============================================================================================
版本号：mp40.01.01.24.080806
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

	暂时放开H.264 sps/pps，不认为其引导关键帧，用于测试
    
三、文档更新情况
         
四、代码走读情况     

五、单元测试情况
   
六、系统测试建议
           
七、提交人
      顾振华
=============================================================================================


=============================================================================================
版本号：mp40.01.01.23.080530
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

	交换残留问题 修正
    
三、文档更新情况
         
四、代码走读情况
      顾振华

五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.22.080527
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

    周广程：
	bug2104:会议设置当前发言人看上一发言人时，切换发言人，开启强制广播，当前发言人图像卡死
	bug2092:当把某终端拖入电视墙后，再进行会议轮询操作，当轮询到拖入电视墙前一终端时，电视墙终端图像卡死
   
三、文档更新情况
         
四、代码走读情况
      顾振华

五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.21.071030
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

    MP不再建立从源+1端口到目标+1端口的RTCP交换，该交换基本没有使用，
	同时会与PRS的RTCP交换重叠
   
三、文档更新情况
         
四、代码走读情况
      顾振华

五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================


=============================================================================================
版本号：mp40.01.01.20.070912
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

    MP交换逻辑优化
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.19.070725
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

    1、介于HDSC接入能力过强，对MP的交换进行扩容一倍支持
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.18.070719
一、接口改变，新增功能

二、修改的bug(对应bug单中编号)

    1、介于交换表容量，RTCP交换不再记入交换表
    2、交换表增删表项 策略修正
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.17.070319
一、接口改变，新增功能
    
    代码整理、优化

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.16.070211
一、接口改变，新增功能
    
    MP能力限制支持

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.15.070112
一、接口改变，新增功能
    
    增加定时监测码流转发情况，超限则上报会控的功能——周广程

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.14.070111
一、接口改变，新增功能
    
    增加对码流转发能力的限制——周广程

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.13.061128

一、接口改变，新增功能
    
    OSP TELNET 增加授权支持

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.12.060912

一、接口改变，新增功能
    

二、修改的bug(对应bug单中编号)

Bug00007211  切换发言人导致Sony图像瞬间花屏明显  
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      张宝卿
=============================================================================================

=============================================================================================
版本号：mp40.01.01.11.060511

一、接口改变，新增功能

    增加是否等待关键帧选项

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      刘辉云
=============================================================================================

=============================================================================================
版本号：mp40.01.01.11.060429

一、接口改变，新增功能

    LINUX下消息API接口支持

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      许世林
=============================================================================================
=============================================================================================
版本号：mp40.01.01.09.060427

一、接口改变，新增功能

    修改Linux下Mp启动方式
二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      许世林
=============================================================================================
版本号：mp40.01.01.08.060407

一、接口改变，新增功能

    增加配置文件

二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      许世林
=============================================================================================
版本号：mp40.01.01.08.060303

一、接口改变，新增功能

    完善外设断链处理逻辑，增加取主备倒换状态
二、修改的bug(对应bug单中编号)
   
三、文档更新情况
         
四、代码走读情况
      
五、单元测试情况
   
六、系统测试建议
           
七、模块负责人
      许世林
=============================================================================================
版本号：mp40.01.01.07.060206

一、接口改变，新增功能
1.将交换表信息new出，而不再采用原来的全局内存
2.将归一重整参数信息new出，而不再采用原来的全局内存
3.这样可以大大减小mplib的文件长度
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      许世林

=============================================================================================

=============================================================================================

版本号：mp40.01.01.06.060123

一、接口改变，新增功能
    Bug00004941,标题：8000MCU设置归一重整，其他厂商终端无法看到本地画面
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      许世林

=============================================================================================

=============================================================================================

版本号：mp40.01.01.06.060121

一、接口改变，新增功能
    更新confidx策略改变影响的功能代码
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      许世林

=============================================================================================

=============================================================================================

版本号：mp40.01.01.06.0501130

一、接口改变，新增功能
    增加对RTP头的载荷值得的归一处理
    将 MT.RTCP -> PRS 时，考虑到防火墙支撑，需将交换包的源ip、port映射为 MT.RTP，以便PRS重传
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      李博

=============================================================================================
=============================================================================================

版本号：mp36.01.01.04.050514

一、接口改变，新增功能
    多板MP码流SSRC、SN、TS归一重整处理更新
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      李博

=============================================================================================

版本号：mp36.01.01.04.050512

一、接口改变，新增功能

    新增多板MP码流SSRC、SN、TS归一重整处理
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      李博

=============================================================================================

版本号：mp36.01.01.03.050430

一、接口改变，新增功能

    新增MP码流SSRC、SN、TS归一重整处理
     
二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   
    
五、单元测试情况
   

六、系统测试建议

            
七、模块负责人
      李博

=============================================================================================

版本号：mp35.01.01.02.041214

一、接口改变，新增功能

             
是否验证 ： 是
项目经理确认: 是

二、修改的bug(对应bug单中编号)
   
   Bug00003474

三、文档更新情况
         

四、代码走读情况
   检查人: 张斯红

五、单元测试情况
   通过测试

六、系统测试建议

    请测试是否能正确编译
        
七、模块负责人
      胡昌威 
=============================================================================================
MP模块

版本号：mp35.01.01.01.1008

一、接口改变，新增功能

    第一次提交
     
            
是否验证 ： 是
项目经理确认: 是

二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   检查人: 张斯红

五、单元测试情况
   通过测试

六、系统测试建议

    请测试是否能正确编译
        
七、模块负责人
      胡昌威 

=============================================================================================


=============================================================================================

MP模块

版本号：mp35.01.01.01.1009

一、接口改变，新增功能

    dataswitch.h改到平台组，编译makefile更新
     
            
是否验证 ： 是
项目经理确认: 是

二、修改的bug(对应bug单中编号)
   

三、文档更新情况
         

四、代码走读情况
   检查人: 张斯红

五、单元测试情况
   通过测试

六、系统测试建议

    请测试是否能正确编译
        
七、模块负责人
      胡昌威 
  
     



   