#include <iostream>
#include <cstdlib>

#include "../../../include/PNG/CRC32.h"
#include "../../../include/PNG/Utilities.h"
#include "../../../include/PNG/Chunks/PHYS_CHUNK.h"


/**
 * @brief Construct a new PHYS_CHUNK::PHYS_CHUNK object
 * 
 * @param ppuX the phisical pixel dimension on x axis of the png
 * @param ppuY the phisical pixel dimension on y axis of the png
 * @param unitSpecifier the unit specifier of phisical pixel dimension on each axis
 */
PHYS_CHUNK::PHYS_CHUNK(unsigned int ppuX, unsigned int ppuY, uint8_t unitSpecifier)
{
    m_length = 9;                        // setting up the pHYs chunk data length(always 9)

    m_type = new uint8_t[4];       // setting up  the pHYs type (pHYs in Hexadecimal)
    m_type[0] = 0x70; //p
    m_type[1] = 0x48; //H
    m_type[2] = 0x59; //Y
    m_type[3] = 0x73; //s

    m_ppuX = ppuX;                      // setting the physical pixels dimension on each axis (x and y)
    m_ppuY = ppuY;

    m_unitSpecifier = unitSpecifier;   // setting the unit specifier, (0 = non specified, 1 = meter)

    // physical pixel dimension on x or y axis should not be 0, if 0 appear we considere the chunk absent
    if ( ppuX == 0 || ppuY == 0)  
        m_state = false;
    else
        m_state = true;

    uint8_t *ppuXArrayPtr = Utilities::int_to_uint8(m_ppuX);
    uint8_t *ppuYArrayPtr = Utilities::int_to_uint8(m_ppuY); 

    uint8_t *cat_r1 = Utilities::getConcatenedArray(m_type, ppuXArrayPtr, 4, sizeof(int));
    uint8_t *cat_r2 = Utilities::getConcatenedArray(ppuYArrayPtr, &m_unitSpecifier, sizeof(int), 1);

    //the crc32 calculation algorithm needs the concatened array of the chunk type and the chunk datas
    uint8_t *dataCRC = Utilities::getConcatenedArray(cat_r1, cat_r2, 4 + sizeof(int), sizeof(int) + 1);

    m_crc32 = CRC32::getCRC32(dataCRC, 5 + 2*sizeof(int));     //we calculate the crc and write it into the file stream
    delete[] ppuXArrayPtr;    delete[] ppuYArrayPtr;    delete[] dataCRC;
    delete[] cat_r1; delete[] cat_r2;
}

/**
 * @brief Destroy the phys PHYS_CHUNK::PHYS_CHUNK object
 * 
 */
PHYS_CHUNK::~PHYS_CHUNK()
{
    delete[] m_type;
}

/**
 * @brief save the actual PHYS_CHUNK datas(type, length, datas, crc32) to an output file stream
 * 
 * @param outputStream the output file stream reference
 */
void PHYS_CHUNK::save(std::ofstream &outputStream)
{
    //we start by converting the (> 1 byte) values into arrays of bytes
    uint8_t *lengthArrayPtr = Utilities::int_to_uint8(m_length);
    uint8_t *ppuXArrayPtr = Utilities::int_to_uint8(m_ppuX);
    uint8_t *ppuYArrayPtr = Utilities::int_to_uint8(m_ppuY);
    uint8_t *crc32ArrayPtr = Utilities::int_to_uint8(m_crc32);

    //then we write chunk datas in the file stream
    Utilities::stream_write(lengthArrayPtr, sizeof(int), outputStream);
    Utilities::stream_write(m_type, 4, outputStream);
    Utilities::stream_write(ppuXArrayPtr, sizeof(int), outputStream);
    Utilities::stream_write(ppuYArrayPtr, sizeof(int), outputStream);
    Utilities::stream_write(&m_unitSpecifier, 1, outputStream);
    Utilities::stream_write(crc32ArrayPtr, sizeof(unsigned long), outputStream);

    delete[] lengthArrayPtr;    delete[] crc32ArrayPtr; //freeing the bytes arrays
    delete[] ppuXArrayPtr;     delete[] ppuYArrayPtr;
}

/**
 * @brief get the state of the actual pHYs chunk in the png file
 * @details cause pHYs is a not critical chunk, we need before write it in the png file if its should be present or not
 * this will be usefull when calling the PNG::save() method, to specify if the phys chunk should be written or not
 * 
 * @return true 
 * @return false 
 */
bool PHYS_CHUNK::getState()
{
    return m_state;
}