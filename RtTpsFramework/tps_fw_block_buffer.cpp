////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_block_buffer.cpp
/// 
///  \brief class BlockBuffer implementation 
/// 
///  \version 1.0
/// 
///  \date    2016/03/09
////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <omp.h>
#include "RtTpsFramework/tps_fw_block_buffer.h"
#include "McsfGeometry/mcsf_geometry_sub_volume.h"

#include "tps_logger.h"

TPS_BEGIN_NAMESPACE   // begin namespace tps

BlockBuffer::BlockBuffer(const unsigned int dim) : mDim(dim)
{
}

BlockBuffer::BlockBuffer(const BlockBuffer& blockBuffer)
{
    this->mDim = blockBuffer.mDim;
    for (auto it = blockBuffer.mBlockMap.begin(); it != blockBuffer.mBlockMap.end(); ++it)
    {
        mBlockMap[it->first] = new Block(*it->second);
    }
}

BlockBuffer::~BlockBuffer()
{
    for (auto it = mBlockMap.begin(); it != mBlockMap.end(); ++it)
    {
        delete it->second;
    }
    mBlockMap.clear();
    mDim = 0;
}

BlockBuffer& BlockBuffer::operator=(const BlockBuffer& blockBuffer)
{
    for (auto it = mBlockMap.begin(); it != mBlockMap.end(); ++it)
    {
        delete it->second;
    }
    mBlockMap.clear();

    mDim = blockBuffer.mDim;
    for (auto it = blockBuffer.mBlockMap.begin(); it != blockBuffer.mBlockMap.end(); ++it)
    {
        mBlockMap[it->first] = new Block(*it->second);
    }
    return *this;
}

unsigned int BlockBuffer::RequestOneFreeBit()
{
    unsigned short blockID, bitID;
    this->GetFreeBitFromExistingBlock_i(blockID, bitID);
    if (blockID == 0 || bitID == 0)
    {
        //add new block
        auto block = new Block();
        //assign id
        blockID = mBlockMap.size() + 1;
        bitID = 1;
        mBlockMap[blockID] = block;
    }
    if (blockID == 0 || bitID == 0)
    {
        TPS_LOG_DEV_ERROR<<"Failed to request free bit.";
        return 0;
    }
    Block* block = mBlockMap[blockID];
    if (!block->validation_flag)
    {
        block->buffer_size = mDim;
        block->block_buffer = new BLOCK_INNER_TYPE[mDim];
        memset(block->block_buffer, 0, mDim * sizeof(BLOCK_INNER_TYPE));
    }
    block->validation_flag |= (0x01<<(bitID - 1));
    block->non_empty_flag &= ~(0x01<<(bitID-1));
    return ComposeID(blockID, bitID);
}

bool BlockBuffer::UpdateBit(unsigned int id, const char* buffer, int offset/* = 0*/, int stride/* = 8*/)
{
    if (buffer == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The input buffer is empty.";
        return false;
    }

    unsigned short blockID, bitID;
    BreakID(id, blockID, bitID);
    if (blockID == 0 || bitID == 0 || bitID > BLOCK_BUFFER_BIT_COUNT)
    {
        TPS_LOG_DEV_ERROR<<"Wrong ID.";
        return false;
    }
    auto it = mBlockMap.find(blockID);
    if (it == mBlockMap.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The block does not exist.";
        return false;
    }
    Block* block = it->second;
    if ((block->validation_flag & (0x1<<(bitID-1))) == 0)
    {
        TPS_LOG_DEV_ERROR<<"The specified bit is invalid.";
        return false;
    }

    BLOCK_INNER_TYPE to1Stencil = 0x1<<(bitID-1);
    BLOCK_INNER_TYPE to0Stencil = ~to1Stencil;
    char ch;
    for (int i = 0, bitLoc = offset; i < mDim; ++i, bitLoc += stride)
    {
        ch = buffer[bitLoc / 8];
        if(ch & (0x01 << (bitLoc % 8)))
        {
            //set to 1
            block->block_buffer[i] |= to1Stencil;
        }
        else
        {
            //set to 0
            block->block_buffer[i] &= to0Stencil;
        }
    }
    block->non_empty_flag |= to1Stencil;
    return true;
}

bool BlockBuffer::UpdateBitViaSubVolume(unsigned int id, 
    McsfGeometry::SubVolume* subVolume, const unsigned int* orgSize)
{
    if (subVolume == nullptr || orgSize == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The input sub-volume | orgSize is empty.";
        return false;
    }
    if (mDim != orgSize[0] * orgSize[1] * orgSize[2])
    {
        TPS_LOG_DEV_ERROR<<"The size of original volume isn't coincident with BlockBuffer's.";
        return false;
    }

    unsigned short blockID, bitID;
    BreakID(id, blockID, bitID);
    if (blockID == 0 || bitID == 0 || bitID > BLOCK_BUFFER_BIT_COUNT)
    {
        TPS_LOG_DEV_ERROR<<"Wrong ID.";
        return false;
    }
    auto it = mBlockMap.find(blockID);
    if (it == mBlockMap.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The block does not exist.";
        return false;
    }
    Block* block = it->second;
    if ((block->validation_flag & (0x1<<(bitID-1))) == 0)
    {
        TPS_LOG_DEV_ERROR<<"The specified bit is invalid.";
        return false;
    }

    char* buffer = subVolume->GetData();
    int offset[3], size[3];
    subVolume->GetOffSet(offset[0], offset[1], offset[2]);
    subVolume->GetSize(size[0], size[1], size[2]);
    BLOCK_INNER_TYPE to1Stencil = 0x1<<(bitID-1);
    BLOCK_INNER_TYPE to0Stencil = ~to1Stencil;
    if (size[0] == 0 || size[1] == 0 || size[2] == 0 || buffer == nullptr)
    {
        block->non_empty_flag &= to0Stencil;
        return true;
    }

    int planeCount = orgSize[0] * orgSize[1];
#pragma omp parallel for
    for (int z = 0 ; z < orgSize[2]; ++z)
    {
        if (z < offset[2] || z > offset[2] + size[2] - 1)
        {
            for (int i = 0; i < planeCount; ++i)
            {
                block->block_buffer[z * planeCount + i] &= to0Stencil;
            }
            continue;
        }

        for (int y = 0; y < orgSize[1]; ++y)
        {
            if (y < offset[1] || y > offset[1] + size[1] -1)
            {
                for (int i = 0; i < orgSize[0]; ++i)
                {
                    block->block_buffer[z * planeCount + y * orgSize[0] + i] &= to0Stencil;
                }
                continue;
            }
            for (int x = 0; x < orgSize[0]; ++x)
            {
                if (x < offset[0] || x > offset[0] + size[0] - 1 || 
                    buffer[(z - offset[2]) * size[1] * size[0] + 
                    (y - offset[1]) * size[0] + x - offset[0]] == 0)
                {
                    block->block_buffer[z * planeCount + y * orgSize[0] + x] &= to0Stencil;
                }
                else
                {
                    block->block_buffer[z * planeCount + y * orgSize[0] + x] |= to1Stencil;
                }
            }
        }
    }
    block->non_empty_flag |= to1Stencil;
    return true;
}

void BlockBuffer::EraseBit(unsigned int id)
{
    unsigned short blockID, bitID;
    BreakID(id, blockID, bitID);
    if (blockID == 0 || bitID == 0 || bitID > 32)
    {
        TPS_LOG_DEV_ERROR<<"Wrong ID.";
        return;
    }
    auto it = mBlockMap.find(blockID);
    if (it == mBlockMap.end() || it->second == nullptr)
    {
        TPS_LOG_DEV_ERROR<<"The block does not exist.";
        return;
    }

    BLOCK_INNER_TYPE to0Stencil = ~(0x1<<(bitID-1));
    it->second->validation_flag &= to0Stencil;
    it->second->non_empty_flag &= to0Stencil;
    //if none of the bits is valid, delete the buffer for saving memory
    if (!it->second->validation_flag)
    {
        it->second->Release();
    }
}

unsigned int BlockBuffer::GetDim() const
{
    return mDim;
}

const std::map<unsigned short, BlockBuffer::Block*> BlockBuffer::GetBlockMap() const
{
    return mBlockMap;
}

void BlockBuffer::BreakID(unsigned int id, unsigned short& blockID, unsigned short& bitID)
{
    blockID = (id >> 16);
    bitID = (id & 0x0000ffff);
}

unsigned int BlockBuffer::ComposeID(unsigned short blockID, unsigned short bitID)
{
    unsigned int id = 0;
    id |= (blockID << 16);
    id |= bitID;
    return id;
}

void BlockBuffer::GetFreeBitFromExistingBlock_i(unsigned short& blockID, unsigned short& bitID) const
{
    blockID = 0, bitID = 0;
    for (auto it = mBlockMap.begin(); it != mBlockMap.end(); ++it)
    {
        if (it->second != nullptr && it->second->validation_flag != 
            BLOCK_INNER_TYPE(~BLOCK_INNER_TYPE(0)))
        {
            blockID = it->first;
            for(; ((0x01<<bitID) & it->second->validation_flag); ++bitID);
            bitID++;
            break;
        }
    }
}
TPS_END_NAMESPACE  // end namespace tps