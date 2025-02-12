#ifdef __cplusplus
extern "C" {
#endif



/*
***********************************************************************************

NOTICE:
This document contains information that is proprietary to RADVISION LTD..
No part of this publication may be reproduced in any form whatsoever without
written prior approval by RADVISION LTD..

RADVISION LTD. reserves the right to revise this publication and make changes
without obligation to notify any person of such revisions or changes.

***********************************************************************************
*/

/*
  perInternal.h

  for PER internal usage.

  Ron S.
  14 May 1996

  */

#ifndef _PERINTERNAL_
#define _PERINTERNAL_

#include "per.h"
#include "persimpl.h"
#include "psyntreeStackApi.h"
#include "coderbitbuffer.h"

#include "rvlog.h"

#if defined(RV_DEBUG)
#define RV_CODER_DEBUG
#endif

/* Log sources used by the PER encoder/decoder module */
extern RvLogSource rvPerLogSource; /* PER log source */
extern RvLogSource rvPerErrLogSource; /* PERERR log source */



#define MAX_SPECIAL_NODES 20
#define MAX_INT_SIZE  4


typedef enum
{
  encDecErrorsObjectWasNotFound =0x1, /* 001 */
  encDecErrorsMessageIsInvalid  =0x2, /* 010 */
  encDecErrorsResourcesProblem  =0x4 /* 100 */
} encDecErrors;


/************************************************************************
 * THREAD_CoderLocalStorage
 * Thread specific information for the coder.
 * bufferSize   - Size of allocated buffer
 * buffer       - Encode/decode buffer to use for the given thread
 *                We've got a single buffer for each thread - this allows us to
 *                work will less memory resources, and dynamic size of buffers.
 ************************************************************************/
typedef struct
{
    RvUint32    bufferSize;
    RvUint8*    buffer;
} THREAD_CoderLocalStorage;



typedef struct
{
    HBB           hBB;   /* encoded buffer */
    int           arrayOfSpecialNodes[MAX_SPECIAL_NODES]; /* list for "special" fields */
    unsigned int  currentPositionInArrayOfSpecialNodes;
    /* --- Encoding parameters --- */
    RvBool        isOuterMostValueEmpty; /* see 10.1.3 for complete encoding */

    /* --- decoding parameters --- */
    unsigned int  encodingDecodingErrorBitMask;
    RvUint32      decodingPosition; /* currently decoding... */
    /* last decoded node */
    int           synParent;
    int           valParent;
    RvInt32       fieldId; /* reference from last parent. Debug only */

    /* We open some of the emanagStruct fields here for easier and faster access */
    HPST          hSyn; /* syntax tree */
    HPVT          hVal; /* value tree */
    RvBool        isTolerant; /* RV_TRUE if we're allowing non-valid encodings */

    THREAD_CoderLocalStorage* buf; /* Buffer for encode/decode */
} perStruct;



int perEncNode(IN  HPER hPer,
           IN  int synParent,
           IN  int valParent,
           IN  RvInt32 fieldId,
           IN  RvBool wasTypeResolvedInRunTime);

int perDecNode(IN  HPER hPer,
           IN  int synParent,
           IN  pstFieldSpeciality speciality,
           IN  int valParent,
           IN  RvInt32 fieldId);

int perEncodeComplete(IN  HPER hPer, IN int bitsInUse);

int perEncodeOpenType(IN  HPER hPer,
              IN  RvInt32 offset,  /* beginning of open type encoding */
              IN  RvInt32 fieldId);

#endif
#ifdef __cplusplus
}
#endif



