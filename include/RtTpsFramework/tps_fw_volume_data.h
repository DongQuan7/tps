#ifndef TPS_FW_VOLUME_DATA_H_
#define TPS_FW_VOLUME_DATA_H_

#include "tps_fw_defs.h"

TPS_BEGIN_NAMESPACE  // begin namespace tps

template<typename T> class VolumeData
{
public:
    VolumeData() : mData(nullptr), mDefault(static_cast<T>(0))
    {
        memset(mSize, 0, 3 * sizeof(int));
        memset(mStart, 0, 3 * sizeof(int));
    }
    ~VolumeData()
    {
        Clear();
    }
    VolumeData(const VolumeData& volume)
    {
        memcpy(mStart, volume.mStart, 3 * sizeof(int));
        memcpy(mSize, volume.mSize, 3 * sizeof(int));
        mData = new T[mSize[0] * mSize[1] * mSize[2]];
        memcpy(mData, volume.mData, mSize[0] * mSize[1] * mSize[2] * sizeof(T));
        mDefault = volume.mDefault;
    }
    VolumeData& operator=(const VolumeData& volume)
    {
        ClearData();

        memcpy(mStart, volume.mStart, 3 * sizeof(int));
        memcpy(mSize, volume.mSize, 3 * sizeof(int));
        mData = new T[mSize[0] * mSize[1] * mSize[2]];
        memcpy(mData, volume.mData, mSize[0] * mSize[1] * mSize[2] * sizeof(T));
        mDefault = volume.mDefault;
        return *this;
    }

    void SetSize(const int* size)
    {
        if (size == nullptr)
        {
            throw std::exception("size is nullptr.");
        }
        int newSize = size[0] * size[1] * size[2];
        int oldSize = mSize[0] * mSize[1] * mSize[2];
        if (oldSize != newSize)
        {
            this->ClearData();
            mData = newSize ? new T[newSize] : nullptr;
        }
        memcpy(mSize, size, 3 * sizeof(int));
        if (sizeof(T) == sizeof(char))
        {
            char val = *(char*)(&mDefault);
            memset(mData, val, newSize * sizeof(T));
        }
        else
        {
            int plane_size = mSize[0] * mSize[1];
            for (int i = 0; i < plane_size; ++i)
            {
                mData[i] = mDefault;
            }
            for (int i = 1; i < mSize[2]; ++i)
            {
                memcpy(mData + i * plane_size, mData, plane_size * sizeof(T));
            }
        }
    }

    inline void GetSize(int* size) const
    {
        if (size == nullptr)
        {
            throw std::exception("size is nullptr.");
        }

        memcpy(size, mSize, 3 * sizeof(int));
    }

    inline void SetStart(const int* start)
    {
        if (start == nullptr)
        {
            throw std::exception("start is nullptr.");
        }

        memcpy(mStart, start, 3 * sizeof(int));
    }

    inline void GetStart(int* start) const
    {
        if (start == nullptr)
        {
            throw std::exception("start is nullptr.");
        }

        memcpy(start, mStart, 3 * sizeof(int));
    }

    inline T* GetData() const { return mData; } 

    inline void ClearData()
    {
        if (mData)
        {
            delete[] mData;
            mData = nullptr;
        }
    }

    inline void Clear()
    {
        memset(mSize, 0, 3 * sizeof(int));
        memset(mStart, 0, 3 * sizeof(double));
        ClearData();
    }

    inline bool IsEmpty() const
    {
        return mData == nullptr;
    }
    //inline T& operator()(int x, int y, int z)
    //{
    //    x -= mOffset[0];
    //    y -= mOffset[1];
    //    z -= mOffset[2];
    //    return mData[mSize[0] * mSize[1] * z + mSize[0] * y + x];
    //}

    inline T Deault() const { return mDefault; }
protected:
    int     mStart[3];
    int     mSize[3];
    T*      mData;
    T       mDefault;
};

TPS_END_NAMESPACE  // end namespace tps

#endif // tps_fw_volume_data_h__
