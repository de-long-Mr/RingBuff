#ifndef RING_BUFF_H
#define RING_BUFF_H

#include "stdint.h"
#include "ring_buff_config.h"


#define ringMEMORY_BASIC_UNIT_512B (512u)
#define ringMEMORY_BASIC_UNIT_1K (1024u)

#ifdef ringBUFF_SIZE
    #define ringCACHE_SIZE (ringBUFF_SIZE)
#else 
    #define ringCACHE_SIZE (ringMEMORY_BASIC_UNIT_512B)
#endif

#if (ringUSER_GLOBAL_LOCK == 1U)
    // #define ringGLOBAL_LOCK_FUN 
    // #define ringGLOBAL_UNLOCK_FUN
    extern void ringGLOBAL_LOCK_FUN(void);
    extern void ringGLOBAL_UNLOCK_FUN(void);
#else 
    #define ringGLOBAL_LOCK_FUN()
    #define ringGLOBAL_UNLOCK_FUN()
#endif

#ifndef NULL
    #define NULL ((void *)(0U))
#endif

typedef struct {
    uint8_t ucCache[ringCACHE_SIZE];
    uint32_t ulEffectiveSize;
    uint32_t ulRemainingSize;
    uint32_t ulHeadIndex;
    uint32_t ulTailIndex;
}xRing_buff_t;

typedef struct {
    uint32_t ulEffectiveSize;
    uint32_t ulRemainingSize;
    uint32_t ulTotalSize;
    uint32_t ulHeadIndex;
    uint32_t ulTailIndex;
    uint8_t  *pucHeadPoint;
}Ring_buff_info_t;

void vRingBuffInit(xRing_buff_t *xRringBuf);
uint32_t ulRingBuffPush(xRing_buff_t *xRringBuf, const uint8_t *pucData, uint32_t ulSize);
uint32_t ulRingBuffPop(xRing_buff_t *xRringBuf, uint32_t ulSize);
uint32_t ulRingBuffRead(xRing_buff_t *xRringBuf, uint8_t *pucBuff, uint32_t ulSize);
uint32_t ulRingBuffReadAndPop(xRing_buff_t *xRringBuf, uint8_t *pucBuff, uint32_t ulSize);
uint32_t ulRingBuffGetEffectiveSize(xRing_buff_t *xRringBuf);
uint32_t ulRingBuffGetRemainingSize(xRing_buff_t *xRringBuf);
uint32_t ulRingBuffGetTotalCapacity(xRing_buff_t *xRringBuf);
int32_t ulRingBuffGetHeadIndex(xRing_buff_t *xRringBuf);
int32_t ulRingBuffGetTailIndex(xRing_buff_t *xRringBuf);
uint8_t *pucRingBuffGetHeadPoint(xRing_buff_t *xRringBuf);
uint8_t *pucRingBuffGetTailPoint(xRing_buff_t *xRringBuf);
void vRingBuffGetInfo(Ring_buff_info_t *xRringBufInfo, xRing_buff_t *xRringBuf);

#endif