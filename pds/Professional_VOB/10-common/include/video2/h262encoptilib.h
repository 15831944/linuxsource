/***************************************************************************************
文件名	    ： h262encoptilib.h
相关文件	：
文件实现功能：mepg2(h262)解码优化库
作者		：胡小鹏，奚荣勇
版本		：1.0
-------------------------------------------------------------------------------
日  期		版本		修改人		走读人    修改内容
07/07/2006	1.0         胡小鹏                 创建
***************************************************************************************/
#ifndef  _H262ENCOPTILIB_H_
#define  _H262ENCOPTILIB_H_

/*=====================================================================
函数名       :  H262DCT_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： ps16Block地址需要32位对齐
======================================================================*/
void H262DCT_map(s16* ps16Block);

/*====================================================================
函数名       :  H262IDCT_map
功能         ： 8×8模块的DCT变换 
引用全局变量 ： 无
输入参数说明 ： s16* ps16Block   指向8×8模块
返回值说明   ： 无
特殊说明     ： pBlock地址需要32位对齐
======================================================================*/
void H262IDCT_map(s16* ps16Block);

/*=============================================================================
函 数 名： H262SubDCT8x8_map
功    能： 8x8块DCT
算法实现： 无
全局变量： 无
参    数： ps16Dct        DCT变换系数
           pu8Src         原始图像
           pu8Dst         目的图像
返 回 值： 无 
=============================================================================*/
void H262SubDCT8x8_map(s16* ps16DCT, u8* pu8Src, u8* pu8Dst);

/*=============================================================================
函 数 名： H262SubDCT16x16_map                                                       
功    能： 16x16块DCT                                                          
算法实现： 无                                                                  
全局变量： 无                                                                  
参    数： ps16Dct        DCT变换系数                                          
           pu8Src         原始图像                                             
           pu8Dst         目的图像                                             
           l32IsField     是否使用Field DCT标志                                
返 回 值： 无                                         
=============================================================================*/
void H262SubDCT16x16_map(s16* ps16DCT, u8* pu8Src, u8* pu8Dst, l32 l32IsField);

/*=============================================================================
函 数 名： H262AddiDCT8x8_map
功    能： 8x8反DCT变换
算法实现： 无
全局变量： 无
参    数： ps16Dct        DCT变换系数
           pu8Ref         预测图像
           pu8Rec         目的图像
返 回 值： 无
=============================================================================*/
void H262AddiDCT8x8_map(s16* ps16DCT, u8* pu8Rec, u8* pu8Ref);

/*=============================================================================
函 数 名： H262AddiDCT16x16_map
功    能： 16x16反DCT变换
算法实现： 无
全局变量： 无 
参    数： ps16Dct        DCT变换系数
           pu8Ref         预测图像
           pu8Rec         目的图像
           l32IsField     是否使用Field DCT标志 
返 回 值： 无
=============================================================================*/
void H262AddiDCT16x16_map(s16* ps16DCT, u8* pu8Rec, u8* pu8Ref, l32 l32IsField);

/*=====================================================================
函数名       :  H262Deviation16x16_map
功能         ： 求宏块内各象素值与平均值差的绝对值之和
引用全局变量 ： 无
输入参数说明 ：  const u8  *pu8MB     ：指向宏块指针
                 l32       l32Stride  ：宏块步长

返回值说明   ： 宏块内各象素值与平均值差的绝对值之和
特殊说明     ： 无
=====================================================================*/
u32 H262Deviation16x16_map(const u8 *pu8Cur, l32 l32Stride);

/*=============================================================================
函 数 名： H262QuantNonIntra8x8_map
功    能： 8x8块inter块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
=============================================================================*/	
l32 H262QuantNonIntra8x8_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iQuant);

/*=============================================================================
函 数 名： H262QuantNonIntra16x16_map
功    能： 16x16块inter块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
=============================================================================*/	
l32 H262QuantNonIntra16x16_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP);

/*===========================================================================
函数名	    H262IQuantNonIntra8x8_map
功能		：Mpeg2对8x8的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *pInCoeff    待量化的8x8块
              s16  *oOutCoeff   存放量化后结果
              u8  iDcScaler    DC系数的量化参数 
              u8  iQuant       量化参数
注意：
返回值说明  ：
=============================================================================*/	
void H262IQuantNonIntra8x8_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iQuant);

/*===========================================================================
函数名	    H262IQuantNonIntra16x16_map
功能		：Mpeg2对16x16的Inter块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *pInCoeff    待量化的8x8块
              s16  *oOutCoeff   存放量化后结果
              u8  iDcScaler    DC系数的量化参数 
              u8  iQuant       量化参数
注意：
返回值说明  ：
=============================================================================*/	
void H262IQuantNonIntra16x16_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iQuant);

/*======================================================================
函数名       :  H262SAD16_map
功能         :  16模块SAD 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Cur      ：当前宏块指针
                l32 l32CurStride ：当前宏块步长
                u8  *pu8Ref      ：参考宏块指针
                l32 l32RefStride ：参考宏块步长
                l32Height        ：宏块高度
返回值说明   :  当前宏块与参考宏块之差绝对值的和
特殊说明     :  无
======================================================================*/
u32 H262SAD16_map(const u8* pu8Cur, l32 l32CurStride, const u8* pu8Ref, l32 l32RefStride, l32 l32Height);

/*=====================================================================
函数名       :  H262PixAvgH17_map
功能         ： 源宏块的水平方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void H262PixAvgH17_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32DstStride, l32 l32Width, l32 l32Height);

/*=====================================================================
函数名       :  H262PixAvgH_map
功能         ： 源宏块的水平方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void H262PixAvgH_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32DstStride, l32 l32Width, l32 l32Height);

/*=====================================================================
函数名       :  H262PixAvgV_map
功能         ： 源宏块的垂直方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void H262PixAvgV_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32DstStride, l32 l32Width, l32 l32Height);

/*=====================================================================
函数名       :  H262PixAvgC_map
功能         ： 源宏块的中心方向插值
引用全局变量 ： 无
输入参数说明 ：  pu8Cur        ：源宏块指针
                 l32CurStride  ：源宏块步长
                 pu8Dst        ：目标宏块指针
                 l32DstStride  ：目标宏块步长
                 l32Width      ：宽度（为8的倍数）
                 l32Height     ：高度
返回值说明   ： 无
特殊说明     ： 无
======================================================================*/
void H262PixAvgC_map(const u8* pu8Cur, l32 l32CurStride, u8* pu8Dst, l32 l32DstStride, l32 l32Width, l32 l32Height);

/*=============================================================================
函 数 名： H262QuantIntra8x8_map
功    能： 8x8块intra块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无
=============================================================================*/	
l32 H262QuantIntra8x8_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iDcScaler, u8 iQuant);

/*=============================================================================
函 数 名： H262QuantIntra16x16_map
功    能： 16x16块intra块量化
算法实现： 无
全局变量： 无
参    数： ps16DctIn        DCT变换输入系数
           ps16DCTOut       量化后输出系数
           l32DCScaler      DC系数量化精度
           l32QP            量化步长
返 回 值： 无 
-------------------------------------------------------------------------------
修改记录:
日  期			版本		修改人				修改内容
2006/06/28		1.0			奚荣勇				  创建
=============================================================================*/	
l32 H262QuantIntra16x16_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iDcScaler, u8 iQuant);

/*===========================================================================
函数名	    H262IQuantIntra8x8_map
功能		：Mpeg2对8x8的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *pInCoeff    待量化的8x8块
              s16  *oOutCoeff   存放量化后结果
              u8  iDcScaler    DC系数的量化参数 
              u8  iQuant       量化参数
注意：
返回值说明  ：
=============================================================================*/	
void H262IQuantIntra8x8_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iDcScaler, u8 iQuant);

/*===========================================================================
函数名	    H262IQuantIntra16x16_map
功能		：Mpeg2对16x16的Intra块做反量化
算法实现	：用右移代替除法
参数说明    ：s16  *pInCoeff    待量化的8x8块
              s16  *oOutCoeff   存放量化后结果
              u8  iDcScaler    DC系数的量化参数 
              u8  iQuant       量化参数
注意：
返回值说明  ：
=============================================================================*/	
void H262IQuantIntra16x16_map(s16 *pInCoeff, s16 *pOutCoeff, u8 iDcScaler, u8 iQuant);

/*======================================================================
函数名       :  H262MemCopy8x4_map
功能         :  8x4块数据拷贝 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                l32  l32SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
返回值说明   :  无
特殊说明     :  无
======================================================================*/
void H262MemCopy8x4_map(const u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride);

/*======================================================================
函数名       :  H262MemCopy8x8_map
功能         :  8x8块数据拷贝 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                l32  l32SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
返回值说明   :  无
特殊说明     :  无
======================================================================*/
void H262MemCopy8x8_map(const u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride);

/*====================================================================
函数名       :  H262MemCopy16x8_map
功能         :  16x8块数据拷贝 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                l32  l32SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
返回值说明   :  无
特殊说明     :  无
======================================================================*/
void H262MemCopy16x8_map(const u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride);

/*======================================================================
函数名       :  H262MemCopy16x16_map
功能         :  16x16块数据拷贝 
引用全局变量 :  无
输入参数说明 :  u8  *pu8Src        ：源块指针
                l32  l32SrcStride  ：源块步长
                u8  *pu8Dst        ：目标块指针
                l32  l32DstStride  ：目标块步长
返回值说明   :  无
特殊说明     :  无
======================================================================*/
void H262MemCopy16x16_map(const u8 *pu8Src, l32 l32SrcStride, u8 *pu8Dst, l32 l32DstStride);

/*=============================================================================
函 数 名: IsFrameorField
功    能: 判断是帧图像还是场图像
算法实现: 无
全局变量: 无
参    数: pu8Src            输入图像指针[in]
          l32Wid            图像宽度[in]
          l32Hei            图像高度[in]
返 回 值: FRAME/FIELD 
=============================================================================*/
l32 IsFrameorField_map(u8 *pu8Src, l32 l32Wid, l32 l32Hei);

/*=============================================================================
函 数 名： H262AddiDCT16x16StrideNoFld_map
功    能： 16x16反DCT变换
算法实现： 无
全局变量： 无 
参    数： ps16Dct        DCT变换系数
           pu8Ref         预测图像
           pu8Rec         目的图像
           l32IsField     是否使用Field DCT标志 
返 回 值： 无
=============================================================================*/
void H262AddiDCT16x16StrideNoFld_map(s16* ps16DCT, u8* pu8Rec, u8* pu8Ref, l32 l32RefStride);

/*============================================================================
函 数 名： H262AddiDCT8x8Stride_map
功    能： 8x8反DCT变换
算法实现： 无
全局变量： 无
参    数： ps16Dct        DCT变换系数
           pu8Ref         预测图像
           pu8Rec         目的图像
返 回 值： 无
=============================================================================*/
void H262AddiDCT8x8Stride_map(s16* ps16DCT, u8* pu8Rec, u8* pu8Ref, l32 l32RefStride);

/*=============================================================================
函 数 名： H262SubDCT16x16StrideNoFld_map                                                       
功    能： 16x16块DCT                                                          
算法实现： 无                                                                  
全局变量： 无                                                                  
参    数： ps16Dct        DCT变换系数                                          
           pu8Src         原始图像                                             
           pu8Dst         目的图像                                             
           l32IsField     是否使用Field DCT标志                                
返 回 值： 无                                                             
=============================================================================*/
void H262SubDCT16x16StrideNoFld_map(s16* ps16DCT, u8* pu8Src, u8* pu8Dst, l32 l32DstStride);

/*=============================================================================
函 数 名： H262SubDCT8x8Stride_map
功    能： 8x8块DCT
算法实现： 无
全局变量： 无
参    数： ps16Dct        DCT变换系数
           pu8Src         原始图像
           pu8Dst         目的图像
返 回 值： 无 
=============================================================================*/
void H262SubDCT8x8Stride_map(s16* ps16DCT, u8* pu8Src, u8* pu8Dst, l32 l32DstStride);

/*=============================================================================
函 数 名： H262AddiDCTIQuant8x8_map
功    能： 16x16反DCT变换，Dequant
算法实现： 无
全局变量： 无 
参    数： ps16DCTIn     ：输入系数指针
           ps16DCTOut    ：输出系数指针
           l32Qp         ：量化系数
           pu8Rec        ：重建图像指针
           pu8Ref        ：参考图像指针
           l32RefStride  ：参考图像步长
           l32IsIntra    ：是否为Intra块(Intra : 1; Inter : 0)
           l32DcScaler   ：DC系数的量化精度(Intra块才会用到)
返 回 值： 无
=============================================================================*/
void H262AddiDCTIQuant8x8_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP, u8* pu8Rec, l32 l32RecStride,
                                      u8* pu8Ref, l32 l32RefStride, l32 l32IsIntra, l32 l32DcScaler);

/*=============================================================================
函 数 名： H262AddiDCTIQuant16x16_map
功    能： 16x16反DCT变换，Dequant
算法实现： 无
全局变量： 无 
参    数： ps16DCTIn     ：输入系数指针
           ps16DCTOut    ：输出系数指针
           l32Qp         ：量化系数
           pu8Rec        ：重建图像指针
           pu8Ref        ：参考图像指针
           l32RefStride  ：参考图像步长
           l32IsIntra    ：是否为Intra块(Intra : 1; Inter : 0)
           l32DcScaler   ：DC系数的量化精度(Intra块才会用到)
返 回 值： 无
=============================================================================*/
void H262AddiDCTIQuant16x16_map(s16* ps16DCTIn, s16* ps16DCTOut, l32 l32QP, u8* pu8Rec,
                                        u8* pu8Ref, l32 l32RefStride, l32 l32IsIntra, l32 l32DcScaler);

/*=============================================================================
函 数 名： H262SubDCTQuant8x8_map
功    能： 16x16计算残差，DCT和Quant
算法实现： 无
全局变量： 无 
参    数： ps16Dct      ：DCT变换系数
           pu8Src       ：输入图像指针
           pu8Ref       ：参考图像指针
           l32RefStride ：参考图像的步长
           ps16DCTOut   ：DCT变换，量化后的系数
           l32QP        ：量化系数
           l32IsIntra   ：Intra 或者Inter块的标志位(Intra : 1; Inter : 0)
           l32DcScaler  : DC系数的量化精度(Intra块才会用到)
返 回 值： 无
=============================================================================*/
l32 H262SubDCTQuant8x8_map(s16* ps16DCT, u8* pu8Src, l32 l32SrcStride, u8* pu8Ref, l32 l32RefStride, s16* ps16DCTOut, l32 l32Qp,
                                   l32 l32IsIntra, l32 l32DcScaler);

/*=============================================================================
函 数 名： H262SubDCTQuant16x16_map
功    能： 16x16计算残差，DCT和Quant
算法实现： 无
全局变量： 无 
参    数： ps16Dct      ：DCT变换系数
           pu8Src       ：输入图像指针
           pu8Ref       ：参考图像指针
           l32RefStride ：参考图像的步长
           ps16DCTOut   ：DCT变换，量化后的系数
           l32QP        ：量化系数
           l32IsIntra   ：Intra 或者Inter块的标志位(Intra : 1; Inter : 0)
           l32DcScaler  : DC系数的量化精度(Intra块才会用到)
返 回 值： 无
=============================================================================*/
l32 H262SubDCTQuant16x16_map(s16* ps16DCT, u8* pu8Src, u8* pu8Ref, l32 l32RefStride, s16* ps16DCTOut, l32 l32QP,
                                l32 l32IsIntra, l32 l32DcScaler);

#endif  //end _H262ENCOPTILIB_H_
