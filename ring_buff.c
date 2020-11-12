#include "ring_buff.h"

static void prvMemoryCopy2RingBuff(xRing_buff_t *xRringBuf, const uint8_t *pucSrc, uint32_t ulSize)
{
    uint32_t ulLoopTimes;
    uint32_t ulIndex = xRringBuf->ulTailIndex;

    if ( xRringBuf->ulEffectiveSize > 0U ) {
        ++ulIndex;
    }
    for ( ulLoopTimes = 0; ulLoopTimes < ulSize; ++ulLoopTimes, ++ulIndex ) {
        ulIndex %= ringCACHE_SIZE;
        xRringBuf->ucCache[ulIndex] = pucSrc[ulLoopTimes];
    }
}

static void prvMemoryCopyFromRingBuff(xRing_buff_t *xRringBuf, uint8_t *pucBuff, uint32_t ulSize)
{
    uint32_t ulLoopTimes;
    uint32_t ulIndex = xRringBuf->ulHeadIndex;

    for ( ulLoopTimes = 0U; ulLoopTimes < ulSize; ++ulLoopTimes, ++ulIndex ) {
        ulIndex %= ringCACHE_SIZE;
        pucBuff[ulLoopTimes] = xRringBuf->ucCache[ulIndex];
    }
}

static void prvMemorySetZero(xRing_buff_t *xRringBuf, uint32_t ulSize)
{
    uint32_t ulLoopTimes;
    uint32_t ulIndex = xRringBuf->ulHeadIndex;

    for ( ulLoopTimes = 0U; ulLoopTimes < ulSize; ++ulLoopTimes, ++ulIndex ) {
        ulIndex %= ringCACHE_SIZE;
        xRringBuf->ucCache[ulIndex] = 0U;
    }
}

void vRingBuffInit(xRing_buff_t *xRringBuf)
{
    if ( xRringBuf != NULL ) {
        ringGLOBAL_LOCK_FUN();
        uint32_t ulLoopTimes;
        uint8_t *pucPointer = (uint8_t *)xRringBuf;

        for ( ulLoopTimes = 0U; ulLoopTimes < sizeof(xRing_buff_t); ++ulLoopTimes ) {
            pucPointer[ulLoopTimes] = 0U;
        }
        xRringBuf->ulRemainingSize = ringCACHE_SIZE;
        ringGLOBAL_UNLOCK_FUN();
    }
}

uint32_t ulRingBuffPush(xRing_buff_t *xRringBuf, const uint8_t *pucData, uint32_t ulSize)
{
    uint32_t ulRet = 0U;

    if ( ( xRringBuf != NULL ) && ( pucData != NULL ) && ( ulSize != 0U ) ) {
        ringGLOBAL_LOCK_FUN();
        if ( xRringBuf->ulRemainingSize > 0U ) {
            if ( xRringBuf->ulRemainingSize < ulSize ) {
                ulRet = xRringBuf->ulRemainingSize;
            }
            else {
                ulRet = ulSize;
            }
            prvMemoryCopy2RingBuff(xRringBuf, pucData, ulRet);
            xRringBuf->ulTailIndex += ulRet;
            if ( xRringBuf->ulEffectiveSize == 0U ) {
                --xRringBuf->ulTailIndex;
            }
            xRringBuf->ulTailIndex %= ringCACHE_SIZE;
            xRringBuf->ulEffectiveSize += ulRet;
            xRringBuf->ulRemainingSize -= ulRet;
        }
        ringGLOBAL_UNLOCK_FUN();
    }
    return ulRet;
}

uint32_t ulRingBuffPop(xRing_buff_t *xRringBuf, uint32_t ulSize)
{
    uint32_t ulRet = 0U;

    if ( ( xRringBuf != NULL ) && ( ulSize != 0U ) ) {
        ringGLOBAL_LOCK_FUN();
        if ( xRringBuf->ulEffectiveSize > 0U ) {
            if ( xRringBuf->ulEffectiveSize < ulSize ) {
                ulRet = xRringBuf->ulEffectiveSize;
            }
            else {
                ulRet = ulSize;
            }
            prvMemorySetZero(xRringBuf, ulRet);
            xRringBuf->ulHeadIndex += ulRet;
            xRringBuf->ulEffectiveSize -= ulRet;
            xRringBuf->ulRemainingSize += ulRet;
            if ( xRringBuf->ulEffectiveSize == 0U ) {
                --xRringBuf->ulHeadIndex;
            }
            xRringBuf->ulHeadIndex %= ringCACHE_SIZE;
        }
        ringGLOBAL_UNLOCK_FUN();
    }
    return ulRet;
}

uint32_t ulRingBuffRead(xRing_buff_t *xRringBuf, uint8_t *pucBuff, uint32_t ulSize)
{
    uint32_t ulRet = 0U;

    if ( ( xRringBuf != NULL ) && ( pucBuff != NULL ) && ( ulSize != 0U ) ) {
        ringGLOBAL_LOCK_FUN();
        if ( xRringBuf->ulEffectiveSize > 0U ) {
            if ( xRringBuf->ulEffectiveSize < ulSize ) {
                ulRet = xRringBuf->ulEffectiveSize;
            }
            else {
                ulRet = ulSize;
            }
            prvMemoryCopyFromRingBuff(xRringBuf, pucBuff, ulRet);
        }
        ringGLOBAL_UNLOCK_FUN();
    }
    return ulRet;
}

uint32_t ulRingBuffReadAndPop(xRing_buff_t *xRringBuf, uint8_t *pucBuff, uint32_t ulSize)
{
    uint32_t ulRet = 0U;

    if ( ( xRringBuf != NULL ) && ( pucBuff != NULL ) && ( ulSize != 0U ) ) {
        ringGLOBAL_LOCK_FUN();
        if ( xRringBuf->ulEffectiveSize > 0U ) {
            if ( xRringBuf->ulEffectiveSize < ulSize ) {
                ulRet = xRringBuf->ulEffectiveSize;
            }
            else {
                ulRet = ulSize;
            }
            prvMemoryCopyFromRingBuff(xRringBuf, pucBuff, ulRet);
            prvMemorySetZero(xRringBuf, ulRet);
            xRringBuf->ulHeadIndex += ulRet;
            xRringBuf->ulEffectiveSize -= ulRet;
            xRringBuf->ulRemainingSize += ulRet;
            if ( xRringBuf->ulEffectiveSize == 0U ) {
                --xRringBuf->ulHeadIndex;
            }
            xRringBuf->ulHeadIndex %= ringCACHE_SIZE;
        }
        ringGLOBAL_UNLOCK_FUN();
    }
    return ulRet;
}

uint32_t ulRingBuffGetEffectiveSize(xRing_buff_t *xRringBuf)
{
    uint32_t ulRet = 0U;

    if ( xRringBuf != NULL ) {
        ringGLOBAL_LOCK_FUN();
        ulRet = xRringBuf->ulEffectiveSize;
        ringGLOBAL_UNLOCK_FUN();
    }
    return ulRet;
}

uint32_t ulRingBuffGetRemainingSize(xRing_buff_t *xRringBuf)
{
    uint32_t ulRet = 0U;

    if ( xRringBuf != NULL ) {
        ringGLOBAL_LOCK_FUN();
        ulRet = xRringBuf->ulRemainingSize;
        ringGLOBAL_UNLOCK_FUN();
    }
    return ulRet;
}

uint32_t ulRingBuffGetTotalCapacity(xRing_buff_t *xRringBuf)
{
    uint32_t ulRet = 0U;

    if ( xRringBuf != NULL ) {
        ringGLOBAL_LOCK_FUN();
        ulRet = xRringBuf->ulRemainingSize + xRringBuf->ulEffectiveSize;
        ringGLOBAL_UNLOCK_FUN();
        if ( ulRet != ringCACHE_SIZE ) {
            ulRet = 0;
        }
    }
    return ulRet;
}


