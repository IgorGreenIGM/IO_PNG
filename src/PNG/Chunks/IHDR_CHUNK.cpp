#include <iostream>
#include <cstdio>
#include <exception>

#include "../../../include/PNG/CRC32.h"
#include "../../../include/PNG/Utilities.h"
#include "../../../include/PNG/Chunks/IHDR_CHUNK.h"


/**
 * @brief Construct a new ihdr IHDR_CHUNK::IHDR_CHUNK chunk object
 * 
 * @param width the width of the png 
 * @param height the height of the png
 * @param bitDepth the bit depth of the png
 * @param colorMode the color mode of the png
 */
IHDR_CHUNK::IHDR_CHUNK(int width, int height, int bitDepth, int colorMode)
{
    m_length = 13;                          //setting up the IHDR data length(always 13)
    
    m_type = new uint8_t[4];          //setting up the IHDR type (IHDR in hexadecimal)
    m_type[0] = 0x49; //I
    m_type[1] = 0x48; //H
    m_type[2] = 0x44; //D
    m_type[3] = 0x52; //R

    m_width = width;                        
    m_height = height;

    m_data = new uint8_t[5];          
    m_data[0] = bitDepth;           //bit Depth
    m_data[1] = colorMode;          //color mode. this plugin can manage is 2(RGB), 6(RGBA), 0(GRAYSCALE), 1(GRAYSCALE + ALPHA)
    m_data[2] = 0x0;                //compression method (always 0, Deflate Algorithm)
    m_data[3] = 0x0;                //filter method (0), the only managed is 0(none).
    m_data[4] = 0x0;                //interlacing method (0)

    uint8_t *widthArrayPtr = Utilities::int_to_uint8(m_width);
    uint8_t *heightArrayPtr = Utilities::int_to_uint8(m_height);

    uint8_t *cat_r1 = Utilities::getConcatenedArray(m_type, widthArrayPtr, 4, 4);
    uint8_t *cat_r2 = Utilities::getConcatenedArray(heightArrayPtr, m_data, 4, 5);

    //the crc32 calculation algorithm needs the concatened array of the chunk type and the chunk datas
    uint8_t *dataCRC = Utilities::getConcatenedArray(cat_r1, cat_r2, 4 + 4, 5 + 4);

    m_crc32 = CRC32::getCRC32(dataCRC, 9 + 2*4); //calculate  crc32 

    delete[] widthArrayPtr;   delete[] heightArrayPtr;  delete[] dataCRC; 
    delete[] cat_r1; delete[] cat_r2;
}

/**
 * @brief Destroy the IHDR_CHUNK::IHDR_CHUNK object
 * 
 */
IHDR_CHUNK::~IHDR_CHUNK()
{
    delete[] m_type;
    delete[] m_data;
}

/**
 * @brief save the actual IEND_CHUNK datas(type, length, datas, crc32) to an output file stream
 * @warning cause computers architectures have different endianess, directly writing a not 8-bits size value on disk will not 
 * guarantee BIG-ENDIAN, which is  Endianess need by the PNG file format. 
 * For this reason, instead of write theses values, we'll write them with our own method, which is platform endianess-independent
 * @see Utilities::stream_write()
 * 
 * @param outputStream the output file stream reference
 */
void IHDR_CHUNK::save(std::ofstream &outputStream)
{ 
    uint8_t *lengthArrayPtr = Utilities::int_to_uint8(m_length);
    uint8_t *widthArrayPtr = Utilities::int_to_uint8(m_width);
    uint8_t *heightArrayPtr = Utilities::int_to_uint8(m_height);
    uint8_t *crc32ArrayPtr = Utilities::int_to_uint8(m_crc32);

    // writing values
    Utilities::stream_write(lengthArrayPtr, 4, outputStream);
    Utilities::stream_write(m_type, 4, outputStream);
    Utilities::stream_write(widthArrayPtr, 4, outputStream);
    Utilities::stream_write(heightArrayPtr, 4, outputStream);
    Utilities::stream_write(m_data, 5, outputStream);
    Utilities::stream_write(crc32ArrayPtr, sizeof(unsigned long), outputStream);

    delete[] lengthArrayPtr;    delete[] crc32ArrayPtr;     //freeing the bytes arrays
    delete[] widthArrayPtr;     delete[] heightArrayPtr;
}

/**
 * @brief get png width
 * 
 * @return int 
 */
int IHDR_CHUNK::get_width()
{
    return this->m_width;
}

/**
 * @brief get png height
 * 
 * @return int 
 */
int IHDR_CHUNK::get_height()
{
    return this->m_height;
}

/**
 * @brief get png bit depth
 * 
 * @return uint8_t 
 */
uint8_t IHDR_CHUNK::get_bitDepth()
{
    return this->m_data[0];
}

/**
 * @brief get png color mode 
 * 
 * @return uint8_t 
 */
uint8_t IHDR_CHUNK::get_colorMode()
{
    return this->m_data[1];
}

/**
 * @brief get png interlacing mode
 * 
 * @return uint8_t 
 */
uint8_t IHDR_CHUNK::get_interlacing()
{
    return this->m_data[4];
}