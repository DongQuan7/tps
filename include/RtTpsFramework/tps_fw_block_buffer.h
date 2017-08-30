////////////////////////////////////////////////////////////////
///  Copyright (c) Shanghai United Imaging Healthcare Inc., 2016
///  All rights reserved.
/// 
///  \author  GONG Maoliang  maoliang.gong@united-imaging.com
/// 
///  \file tps_fw_block_buffer.h
/// 
///  \brief class BlockBuffer declaration 
/// 
///  \version 1.0
/// 
///  \date    2016/03/08
////////////////////////////////////////////////////////////////

#ifndef TPS_FW_BLOCK_BUFFER_H_
#define TPS_FW_BLOCK_BUFFER_H_

#include "tps_fw_defs.h"
#include <map>

typedef unsigned char BLOCK_INNER_TYPE;
#define BLOCK_BUFFER_BIT_COUNT sizeof(BLOCK_INNER_TYPE) * 8

namespace McsfGeometry
{
    class SubVolume;
}

TPS_BEGIN_NAMESPACE  // begin namespace tps

/// \class BlockBuffer
/// 
/// \brief this class TODO 
class TPS_FW_DECLSPEC BlockBuffer
{
public:
    struct Block
    {
        BLOCK_INNER_TYPE validation_flag;
        //1 means not empty
        BLOCK_INNER_TYPE non_empty_flag;
        BLOCK_INNER_TYPE* block_buffer;
        unsigned int buffer_size;
        Block() : validation_flag(0), non_empty_flag(0), buffer_size(0), block_buffer(nullptr) {}
        Block(const Block& block) : validation_flag(block.validation_flag), 
            non_empty_flag(block.non_empty_flag), block_buffer(nullptr), buffer_size(0)
        {
            if (block.block_buffer != nullptr)
            {
                buffer_size = block.buffer_size;
                block_buffer = new BLOCK_INNER_TYPE[buffer_size];
                memcpy(block_buffer, block.block_buffer, buffer_size * sizeof(BLOCK_INNER_TYPE));
            }
        }
        ~Block() 
        {
            Release();
        }
        void Release()
        {
            if(block_buffer)
            {
                delete[] block_buffer;
                block_buffer = nullptr;
            }
            validation_flag = 0;
            non_empty_flag = 0;
            buffer_size = 0;
        }
        Block& operator=(const Block& block)
        {
            Release();
            validation_flag = block.validation_flag;
            non_empty_flag = block.non_empty_flag;
            if (block.block_buffer != nullptr)
            {
                buffer_size = block.buffer_size;
                block_buffer = new BLOCK_INNER_TYPE[buffer_size];
                memcpy(block_buffer, block.block_buffer, buffer_size * sizeof(BLOCK_INNER_TYPE));
            }
            return *this;
        }
    };

    //the constructor and destructor
    BlockBuffer(const unsigned int dim);
    BlockBuffer(const BlockBuffer& blockBuffer);
    ~BlockBuffer();
    BlockBuffer& operator=(const BlockBuffer& blockBuffer);

    // request one free bit space, the ID of the BIT will be returned
    // todo: we have no remaining memory checking now
    unsigned int RequestOneFreeBit();

    // using specified bit of the [buffer] to update bit of Block specified by [id]
    // [offset] offset between the first bit to start bit
    // [stride] stride between bits to be used
    // note that the dimension of [buffer] must be big enough to update all bits
    bool UpdateBit(unsigned int id, const char* buffer, int offset = 0, int stride = 8);

    /// update bit using sub volume
    bool UpdateBitViaSubVolume(unsigned int id, McsfGeometry::SubVolume* subVolume, const unsigned int* orgSize);

    // erase one bit buffer
    void EraseBit(unsigned int id);

    // get dimension of every block
    unsigned int GetDim() const;

    const std::map<unsigned short, Block*> GetBlockMap() const;

    // break block id & bit id
    static void BreakID(unsigned int id, unsigned short& blockID, unsigned short& bitID);
    // compose block id & bit id
    static unsigned int ComposeID(unsigned short blockID, unsigned short bitID);

private:
    // 0 means free bit not exist
    void GetFreeBitFromExistingBlock_i(unsigned short& blockID, unsigned short& bitID) const;

private:
    unsigned int mDim;
    // <block id, 32 bit block> map
    std::map<unsigned short, Block*> mBlockMap;
};
TPS_END_NAMESPACE  // end namespace tps

#endif