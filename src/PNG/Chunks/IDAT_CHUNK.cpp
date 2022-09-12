#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdio>

#include "../../../include/zlib/zlib.h"
#include "../../../include/PNG/CRC32.h"
#include "../../../include/PNG/Utilities.h"
#include "../../../include/PNG/Chunks/IDAT_CHUNK.h"


/**
 * @brief Construct a new IDAT_CHUNK::IDAT_CHUNK object
 * 
 * @param pixelsBuffer the pixels buffer (raw values) pointer
 * @param s_width the png width (according to the pixelsBuffer)
 * @param s_height the png height (according to the pixelsBuffer)
 * @param colorChannel the png color channel number
 */
IDAT_CHUNK::IDAT_CHUNK(uint8_t *pixelsBuffer, int s_width, int s_height, int colorChannel)
{
    m_type = new uint8_t[4];                                  //setting the IDAT type (IDAT in Hexadecimal) 
    m_type[0] = 0x49; //I
    m_type[1] = 0x44; //D
    m_type[2] = 0x41; //A
    m_type[3] = 0x54; //T

    m_data = deflateDatas(pixelsBuffer, s_width, s_height, colorChannel, m_length); //getting the deflated data output

    //the crc32 calculation algorithm needs the concatened array of the chunk type and the chunk datas
    uint8_t *dataCRC = Utilities::getConcatenedArray(m_type, m_data, 4, m_length);
    m_crc32 = CRC32::getCRC32(dataCRC, 4 + m_length);
    delete[] dataCRC;
}


/**
 * @brief Destroy the idat IDAT_CHUNK::IDAT_CHUNK object
 * 
 */
IDAT_CHUNK::~IDAT_CHUNK()
{
    delete[] m_type; delete[] m_data;  //freeing the allocated memory
}


/**
 * @brief save the actual IDAT_CHUNK datas(type, length, datas, crc32) to a specific output file stream
 * 
 * @param outputStream the output file stream reference
 */
void IDAT_CHUNK::save(std::ofstream &outputStream)
{
    //we start by converting the (> 1 byte) values into arrays of bytes
    uint8_t *lengthArrayPtr = Utilities::int_to_uint8(m_length);
    uint8_t *crc32ArrayPtr = Utilities::int_to_uint8(m_crc32);

    //then we write chunk datas in the file stream
    Utilities::stream_write(lengthArrayPtr, sizeof(int), outputStream);
    Utilities::stream_write(m_type, 4, outputStream);
    Utilities::stream_write(m_data, m_length, outputStream);
    Utilities::stream_write(crc32ArrayPtr, sizeof(int), outputStream);

    delete[] lengthArrayPtr; delete[] crc32ArrayPtr;   //freeing the bytes arrays
}

/**
 * @brief generatig scanlines from an input pixels buffer
 * 
 * @param pixels the input pixels buffer
 * @param s_width the number of pixels in the pixels buffer (width) 
 * @param s_height the number of pixels in the pixels buffer (height)
 * @param colorChannel the number of color channel in the pixels buffer
 * @return a pointer to the scanlines buffer generated
 */
uint8_t *IDAT_CHUNK::generateScanlines(uint8_t *pixels, int s_width, int s_height, int colorChannel)
{
    int eachScanlineLength = 1 + s_width * colorChannel, i = 1, j = 0; // one scanline length

    uint8_t *scanlines = new uint8_t[s_height * eachScanlineLength]; // memory allocation for the scanline output

    memset(scanlines, 0, s_height * eachScanlineLength); // we set all the output values to 0

    // then we copy pixels elements in the output, skipping the fisrt output values, that should ever be 0(none filtering method)
    for (i = 1, j = 0; i <= s_height && j <= s_height; i++, j++)
        memcpy(scanlines + 1 + (i - 1) * eachScanlineLength, pixels + j * (eachScanlineLength - 1), eachScanlineLength - 1);

    return scanlines;
}

/**
 * @brief deflate() an input pixels buffer method 
 * 
 * @param pixelBuffer the input pixels buffer
 * @param s_width the number of pixels in the pixels buffer (width)
 * @param s_height the number of pixels in the pixels buffer (height)
 * @param colorChannel the number of color channel in the pixels buffer
 * @param deflatedLen a reference for getting the output defalted length
 * @return a pointer to the deflated datas buffer
 */
uint8_t *IDAT_CHUNK::deflateDatas(uint8_t *pixelBuffer, int s_width, int s_height, int colorChannel, int &deflatedLen)
{
    unsigned long inLen = s_height * (1 + s_width * colorChannel), tmpLen = 0;                  // input len of scanlines datas
    uint8_t *scanlines = generateScanlines(pixelBuffer, s_width, s_height, colorChannel); // generating scanlines from the pixels

    uint8_t *deflatedDatas = nullptr; // setting up the deflated datas output
    int result = 0;

    // initialising zlib
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    defstream.avail_in = inLen;
    defstream.next_in = (Bytef *)scanlines;
    defstream.avail_out = 0;
    defstream.next_out = (Bytef *)deflatedDatas;

    if ((result = deflateInit(&defstream, Z_BEST_COMPRESSION)) == Z_OK)
    {
        // calculate the actual length and update zlib structure
        unsigned long estimateLen = deflateBound(&defstream, inLen);
        deflatedDatas = new uint8_t[estimateLen];
        if (deflatedDatas != nullptr)
        {
            // updation zlib configuration
            defstream.avail_out = (uInt)estimateLen;
            defstream.next_out = (Bytef *)deflatedDatas;

            // do the compression
            deflate(&defstream, Z_FINISH);
            tmpLen = (uint8_t *)defstream.next_out - deflatedDatas;
        }
    }
    deflateEnd(&defstream); // end of deflating algorithm
    deflatedLen = tmpLen;   // copying the defalted data length to the IDAT->length attribut
    delete[] scanlines;

    return deflatedDatas;
}