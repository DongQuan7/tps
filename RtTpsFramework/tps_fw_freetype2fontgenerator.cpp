//////////////////////////////////////////////////////////////////////////
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2014
///  All rights reserved.
///
///  \author  CHUNHUI SHI (mailto:chunhui.shi@united-imaging.com)
///
///  \file   tps_fw_freetype2fontgenerator.cpp
///  \brief  generator font using FreeType2 API
///  \
///  class
///  \version 1.0
///  \date  2015/01/01
///  \
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include "RtTpsFramework/tps_fw_freetype2fontgenerator.h"
#include "RtTpsDatabaseWrapper/rt_tps_database_defs.h"


// Try to figure out what endian this machine is using. 
// Note that the test below might fail for cross compilation; 
// additionally, multi-byte characters are implementation-defined in C preprocessors.

#if (('1234' >> 24) == '1')
#elif (('4321' >> 24) == '1')
#define BIG_ENDIAN
#else
#error "Couldn't determine the endianness!"
#endif

#define PIXEL_HEIGHT    90
#define PIXEL_WIDTH     90

TPS_BEGIN_NAMESPACE

void RasterCallback(const int y,
                    const int count,
                    const FT_Span * const spans,
                    void * const user){
#ifdef DEBUG_PRINT
        printf("RasterCallback(%d)\n", GetCurrentThreadId());
#endif

    Spans *sptr = (Spans *)user;
    for (int i = 0; i < count; ++i){
        sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
    }
}

// Set up the raster parameters and render the outline.
void RenderSpans(const FT_Library &library, FT_Outline * const outline, Spans *spans) {

#ifdef DEBUG_PRINT
    printf("RenderSpans(%d)\n", GetCurrentThreadId());
#endif

    FT_Raster_Params params;
    memset(&params, 0, sizeof(params));
    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
    params.gray_spans = RasterCallback;
    params.user = spans;

    FT_Outline_Render(library, outline, &params);
}

IMPLEMENT_SINGLETON(FTStringBufferGenerator);

FTStringBufferGenerator::FTStringBufferGenerator(void) 
    : mLibrary(nullptr), mFace(nullptr), mStringText(L""), mFontBuffer(nullptr), mIsInited(false), 
    mFontCol(WHITE_VALUE), mOutlineCol(BLACK_VALUE), mOutlineWidth(0.5f), mSpacing(5){

}

FTStringBufferGenerator::~FTStringBufferGenerator(void) {
    if (nullptr!=mFontBuffer) {
        delete[] mFontBuffer;
        mFontBuffer = nullptr;
    }

    if(!mFontCollection.empty())
    {
        std::for_each(mFontCollection.begin(), mFontCollection.end(), 
            [&](std::pair<int, FontInfo> it)
        {
            std::for_each(it.second.mCharmap.begin(), it.second.mCharmap.end(), 
                [&](std::pair<wchar_t, CharBuf*> iter)
            {
                if(iter.second != nullptr)
                {
                    delete iter.second;
                    iter.second = nullptr;
                }
            });

        });
        mFontCollection.clear();
    }

    FT_Done_Face(mFace);
    FT_Done_FreeType(mLibrary);
    mLibrary = nullptr;
    mFace = nullptr;
}

bool FTStringBufferGenerator::Init(/*const std::string& strFontFilePath*/)
{
    const std::string& strFontFilePath = "C:/WINDOWS/Fonts/simsun.ttc";

#ifdef DEBUG_PRINT
        printf("FTStringBufferGenerator::Initialize(%d)\n", GetCurrentThreadId());
#endif
        if(mIsInited){
            return true;
        }
        // prepare face
        std::ifstream fontFile(strFontFilePath.c_str(), std::ios::binary);
        if(!fontFile){
            fprintf(stderr, "Font file does not exist!\n");
            TPS_LOG_DEV_ERROR<<"Font file does not exist!";
            return false;
        }
        fontFile.seekg(0, std::ios::end);
        std::fstream::pos_type fontFileSize = fontFile.tellg();
        fontFile.seekg(0);

        DEL_ARRAY(mFontBuffer);
        mFontBuffer = new unsigned char[fontFileSize];
        fontFile.read((char *)mFontBuffer, fontFileSize);

        fontFile.close();

        FT_Error error = FT_Init_FreeType(&mLibrary);
        if(error)
        {
            TPS_LOG_DEV_ERROR<<"Failed to init Freetype2 library!";
            DEL_ARRAY(mFontBuffer);
            mLibrary = 0;
            return false;
        }

        error = FT_New_Memory_Face(mLibrary, mFontBuffer, fontFileSize, 0, &mFace);
        if (error)
        {
            TPS_LOG_DEV_ERROR<<"Failed to FT_New_Memory_Face!"; 
            DEL_ARRAY(mFontBuffer);
            FT_Done_FreeType(mLibrary);
            return false;
        }

        float res = 0.0f;
        unsigned int tmp;
        static unsigned int seed = 0x13371337;

        for(int i = 8; i < 12; ++i){ // Generate font size of 8 to 12 by default
            float resa[3];
            for(int j = 0; j<3; ++j){
                seed *= 16807;
                tmp = seed ^ (seed >> 4) ^ (seed << 15);
                *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;
                resa[j] = res - 1;
            }
            //cout<<"Generate Font Size["<<i<<"]:"<<resa[0]*255<<" "<<resa[1]*255<<" "<<resa[2]*255<<endl;
            GenerateCharDictionary(i, Pixel32(255 * resa[0], 255 * resa[1], 255 * resa[2]));
        }
        GenerateCharDictionary(12, WHITE_VALUE);
        //GenerateCharDictionary(20, GOLD_VALUE);

        mIsInited = true;
        return true;
}

void FTStringBufferGenerator::GenerateCharDictionary(int fontSize, const Pixel32 &fontCol)
{
    if(mFace == nullptr)
    {
        Init();
    }

    typedef multimap<int, FontInfo>::iterator multiMapItor;
    std::pair<multiMapItor, multiMapItor> pos = mFontCollection.equal_range(fontSize);
    for(auto it = pos.first; it != pos.second; ++it){
        if(it->second.mFontCol.r == fontCol.r && it->second.mFontCol.g == fontCol.g && it->second.mFontCol.b == fontCol.b){
            TPS_LOG_DEV_INFO<<"Font already exist";
            return;
        }
    }

    const std::wstring wsstr = L" !\"#$%&'()*+'-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^-`abcdefghijklmnopqrstuvwxyz{|}~°";
    std::set<wchar_t> charset;
    int ymin = 999, ymax = -999;
    std::map<wchar_t, CharBuf*> char_map;
    for(int i = 0; i < wsstr.length(); ++i){
        const wchar_t pre_wch = (i == 0) ? L'' : wsstr[i - 1];
        const wchar_t cur_wch = wsstr[i];
        if (cur_wch =='\0'){
            continue;
        }
        if(charset.find(cur_wch) != charset.end()){
            continue;
        }
        charset.insert(pre_wch);

        Rect rect;
        Pixel32* pxl = nullptr;
        float bearingX = 0;
        float bearingY = 0;
        float advance = 0;
        float kerning = 0;
        int imgSize = 0;

        // for concurrency
        boost::lock_guard<boost::mutex> lg(mMutex);
#ifdef DEBUG_PRINT
        printf("FTStringBufferGenerator::PrepareOneCharBuffer(%d)\n", GetCurrentThreadId());
#endif
        // Set the font size
        if (FT_Set_Char_Size(mFace, fontSize << 6, fontSize << 6, PIXEL_WIDTH, PIXEL_HEIGHT) == 0) {
            //if (FT_Set_Pixel_Sizes(mFace, mSize, mSize) == 0) {

            // Load the glyph we are looking for.
            FT_UInt gindex = FT_Get_Char_Index(mFace, cur_wch);
            if (FT_Load_Glyph(mFace, gindex, FT_LOAD_NO_BITMAP) == 0) {
                // Need an outline for this to work.
                if (mFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
                    // Render the basic glyph to a span list.
                    Spans spans;
                    RenderSpans(mLibrary, &mFace->glyph->outline, &spans);

                    // Set up a stroker.
                    FT_Stroker stroker;
                    FT_Stroker_New(mLibrary, &stroker);
                    FT_Stroker_Set(stroker,
                        (int)(mOutlineWidth * 64),
                        FT_STROKER_LINECAP_ROUND,
                        FT_STROKER_LINEJOIN_ROUND,
                        0);

                    FT_Glyph glyph;
                    if(FT_Get_Glyph(mFace->glyph, &glyph) == 0)
                        FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);

                    // We need another span list to store the outline.
                    Spans outlineSpans;
                    if (glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
                        // Render the outline spans to the span list
                        FT_Outline *o =
                            &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;
                        RenderSpans(mLibrary, o, &outlineSpans);
                    }

                    // read out kerning from the glyph
                    if (FT_HAS_KERNING(mFace)) {
                        FT_UInt previous = FT_Get_Char_Index(mFace, pre_wch);
                        FT_Vector delta;
                        FT_Get_Kerning(mFace, previous, gindex, 
                            FT_KERNING_DEFAULT, &delta);
                        kerning = delta.x >> 6; // pen_x += delta.x >> 6;
                    }

                    // Done, clean it up
                    FT_Stroker_Done(stroker);
                    FT_Done_Glyph(glyph);

                    // Now we need to put it all together.
                    if (!spans.empty()) {
                        // Figure out what the bounding rect is for both the span lists.
                        rect.xmin = spans.front().x; rect.xmax = spans.front().x;
                        rect.ymin = spans.front().y; rect.ymax = spans.front().y;
                        for (Spans::iterator it = spans.begin(); it != spans.end(); ++it) {
                            rect.Include(Vec2(it->x, it->y));
                            rect.Include(Vec2((it->x + it->width - 1), it->y));
                        }
                        for (Spans::iterator it = outlineSpans.begin();
                            it != outlineSpans.end(); ++it) {
                                rect.Include(Vec2(it->x, it->y));
                                rect.Include(Vec2((it->x + it->width - 1), it->y));
                        }
                        if(rect.ymin < ymin){ymin = rect.ymin;}
                        if(rect.ymax > ymax){ymax = rect.ymax;}

                        bearingX = mFace->glyph->metrics.horiBearingX >> 6;
                        bearingY = mFace->glyph->metrics.horiBearingY >> 6;
                        advance = mFace->glyph->advance.x >> 6;

                        // Get metrics of our image.
                        int imgWidth = rect.Width();
                        int imgHeight = rect.Height();
                        imgSize = imgWidth * imgHeight;

                        // Allocate data for our image and clear it out to transparent.
                        pxl = new Pixel32[imgSize];
                        memset(pxl, 0, sizeof(Pixel32) * imgSize);

                        // Loop over the outline spans and draw them into the image.
                        for (Spans::iterator it = outlineSpans.begin(); it != outlineSpans.end(); ++it) {
                            for (int w = 0; w < it->width; ++w)
                                pxl[(int)((imgHeight - 1 - (it->y - rect.ymin)) * imgWidth + it->x - rect.xmin + w)] =
                                Pixel32(mOutlineCol.r, mOutlineCol.g, mOutlineCol.b, it->coverage);
                        }

                        // Then loop over the regular glyph spans and blend them into the image.
                        for (Spans::iterator it = spans.begin(); it != spans.end(); ++it) {
                            for (int w = 0; w < it->width; ++w) {
                                Pixel32 &dst =
                                    pxl[(int)((imgHeight - 1 - (it->y - rect.ymin)) * imgWidth + it->x - rect.xmin + w)];
                                Pixel32 src = Pixel32(fontCol.r, fontCol.g, fontCol.b, it->coverage);
                                dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
                                dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
                                dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
                                dst.a = MIN(255, dst.a + src.a);
                            }
                        }
                    }
                    else{
                        //FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
                        bearingX = mFace->glyph->metrics.horiBearingX >> 6;
                        bearingY = mFace->glyph->metrics.horiBearingY >> 6;
                        advance = mFace->glyph->advance.x >> 6;

                        rect.xmin = 0; rect.xmax = advance;
                        rect.ymin = 0; rect.ymax = mFace->glyph->advance.y >> 6;

                        int imgWidth = rect.Width();
                        int imgHeight = rect.Height();
                        imgSize = imgWidth * imgHeight;
                        pxl = new Pixel32[imgSize];
                        memset(pxl, 0, sizeof(Pixel32) * imgSize);
                    }
                }
            }
        }
        Pixel32* buf_ready = new Pixel32[imgSize];
        std::memcpy(buf_ready, pxl, sizeof(Pixel32) * imgSize);

        //SaveToTrueColorBitmap_i(L"D:\\temp\\String.bmp", (unsigned char*)*buf_ready, imgWidth, imgHeight);
        char_map[wsstr[i]] = new CharBuf(rect, buf_ready, bearingX, bearingY, advance, kerning, cur_wch);
        DEL_ARRAY(buf_ready);
        
        delete[] pxl;
        pxl = nullptr;

    }// end of processing each char

    mNewGeneratedFont = FontInfo(char_map, fontSize, fontCol, ymin, ymax);
    mFontCollection.insert(std::make_pair(fontSize, mNewGeneratedFont));
}

bool FTStringBufferGenerator::GenerateRasterBuffer(const std::wstring& strText,
    Pixel32** pixBuf, int& strWidth, int& strHeight, int fontSize){

#ifdef DEBUG_PRINT
    printf("FTStringBufferGenerator::GenerateRasterBuffer(%d)\n", GetCurrentThreadId());
#endif
    if (strText.empty()){
        TPS_LOG_DEV_ERROR<<"GenerateRasterBuffer is empty";
        return false;
    }

    std::vector<CharBuf*> stringBuf;
    typedef multimap<int, FontInfo>::iterator multiMapItor;
    std::pair<multiMapItor, multiMapItor> pos = mFontCollection.equal_range(fontSize);
    auto it = pos.first;

    FontInfo my = it->second;

    int minY = my.mAlignYmin;
    int maxY = my.mAlignYmax;
    for(int i = 0; i < strText.length(); i++){
        auto iter = my.mCharmap.find(strText[i]);
        if(iter == my.mCharmap.end()){
            TPS_LOG_DEV_INFO<<"Character "<<strText[i]<<" not supported";
            continue;
        }else{
            stringBuf.push_back(iter->second);
        }
    }

    AssembleStringBuffer(stringBuf, *pixBuf, strWidth, strHeight, minY, maxY);

    return true;

}

bool FTStringBufferGenerator::GenerateRasterBuffer(const std::wstring& strText,
    Pixel32** pixBuf, int& strWidth, int& strHeight, int fontSize, Pixel32 fontCol){

#ifdef DEBUG_PRINT
        printf("FTStringBufferGenerator::GenerateRasterBuffer(%d)\n", GetCurrentThreadId());
#endif
        if (strText.empty()){
            TPS_LOG_DEV_ERROR<<"GenerateRasterBuffer is empty";
            return false;
        }

        std::vector<CharBuf*> stringBuf;
        int minY = 999;
        int maxY = -999;
        typedef multimap<int, FontInfo>::iterator multiMapItor;
        std::pair<multiMapItor, multiMapItor> pos = mFontCollection.equal_range(fontSize);
        auto it = pos.first;
        for(; it != pos.second; ++it){
            if(it->second.mFontCol.r == fontCol.r && it->second.mFontCol.g == fontCol.g && it->second.mFontCol.b == fontCol.b){
                FontInfo my = it->second;
                minY = my.mAlignYmin;
                maxY = my.mAlignYmax;
                for(int i = 0; i < strText.length(); i++){
                    auto iter = my.mCharmap.find(strText[i]);
                    if(iter == my.mCharmap.end()){
                        TPS_LOG_DEV_INFO<<"Character "<<strText[i]<<" not supported";
                        continue;
                    }else{
                        stringBuf.push_back(iter->second);
                    }
                }
                break;
            }
        }
        if(it == pos.second)
		{
            //"Can't find the specified font. Will generate this font";
            GenerateCharDictionary(fontSize, fontCol);
            minY = mNewGeneratedFont.mAlignYmin;
            maxY = mNewGeneratedFont.mAlignYmax;
            for(int i = 0; i < strText.length(); i++){
                auto iter = mNewGeneratedFont.mCharmap.find(strText[i]);
                if(iter != mNewGeneratedFont.mCharmap.end())
                    stringBuf.push_back(iter->second);
            }
        }

        AssembleStringBuffer(stringBuf, *pixBuf, strWidth, strHeight, minY, maxY);

        //SaveToTrueColorBitmap_i(L"D:\\temp\\a.bmp", (unsigned char*)*pixBuf, bufHeight, strWidth);
        return true;
}


void FTStringBufferGenerator::AssembleStringBuffer(const std::vector<CharBuf*>& stringBuf,
    Pixel32*& pixelBuf, int& strWidth, int& strHeight, int minY, int maxY) {
        // for concurrency
        boost::lock_guard<boost::mutex> lg(mMutex);
#ifdef DEBUG_PRINT
        printf("FTStringBufferGenerator::AssembleStringBuffer(%d)\n", GetCurrentThreadId());
#endif
        if(stringBuf.empty()){
            return;
        }
        // find out string buffer margin
        int penX = 0;
        int strBufYMin = stringBuf[0]->mcharRect.ymin;
        int strBufYMax = stringBuf[0]->mcharRect.ymax;

        for (int n = 0; n < stringBuf.size(); n++) {
            CharBuf* pChar = stringBuf[n];
            penX += pChar->mKerning + pChar->mAdvance;
            strBufYMin = (strBufYMin > pChar->mcharRect.ymin) ? 
                pChar->mcharRect.ymin:strBufYMin;
            strBufYMax = (strBufYMax < pChar->mcharRect.ymax) ? 
                pChar->mcharRect.ymax:strBufYMax;
        }

        // allocate the buffer
        // strHeight = strBufYMax - strBufYMin + 1;
        strHeight = maxY - minY + 1;

        strWidth = penX + 1;
        int iImgSize = strWidth * strHeight;
        pixelBuf = new Pixel32[iImgSize];
        memset(pixelBuf, 0, sizeof(Pixel32) * iImgSize);

        // prebrush the buffer
        //for (int n=0; n<iImgHeight; ++n) {
        //    for (int m=0; m<iImgWidth; ++m) {
        //        pStrBuf[n*iImgWidth+m].b = 255;
        //        pStrBuf[n*iImgWidth+m].g = 0;
        //        pStrBuf[n*iImgWidth+m].r = 0;
        //        pStrBuf[n*iImgWidth+m].a = 0;
        //    }
        //}
        //SaveToTrueColorBitmap_i(L"D:\\temp\\tempString.bmp", (unsigned char*)pStrBuf,
        //    iImgHeight, iImgWidth);

        // copy char buffer into string buffer
        Pixel32* pStrBufStart = pixelBuf;
        int startX = 0;
        int startXX = 0;
        int startY = 0;
        for (int n=0; n<stringBuf.size(); n++) {
            CharBuf* pChar = stringBuf[n];
            Rect& tRect = pChar->mcharRect;
            Pixel32* pCharBuf = pChar->mpcharBufData;

            // calculate the Starting Point(Top Left) of this char in string buffer block
            startXX = startX + tRect.xmin;
            startXX = startXX < 0 ? 0: startXX;
            startY = maxY - tRect.ymax;

            pStrBufStart = pixelBuf + startY*strWidth + startXX;
           
            for (int m=0; m<tRect.Height(); ++m) {
                memcpy(pStrBufStart, pCharBuf, sizeof(Pixel32)*tRect.Width());
                pStrBufStart += strWidth;
                pCharBuf += tRect.Width();
            }

            startX += pChar->mAdvance + pChar->mKerning;
        }

        int* pLine = new int[strWidth];
        for (int iLine = 0; iLine < strHeight / 2; ++iLine) {
            int* pSrc = (int*)pixelBuf + strWidth * iLine;
            int* pDst = (int*)pixelBuf + strWidth * (strHeight - 1 - iLine);

            // swap pSrc and pDst
            memcpy(pLine, pSrc, strWidth * 4);
            memcpy(pSrc, pDst, strWidth * 4);
            memcpy(pDst, pLine, strWidth * 4);
        }

        delete []pLine;
        pLine = nullptr;
}


TPS_END_NAMESPACE
