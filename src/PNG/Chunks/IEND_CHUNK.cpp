#include <iostream>
#include <cstdio>
#include "../../../include/PNG/CRC32.h"
#include "../../../include/PNG/Utilities.h"
#include "../../../include/PNG/Chunks/IEND_CHUNK.h"


/**
 * @brief Construct a new IEND_CHUNK::IEND_CHUNK object
 * 
 */
IEND_CHUNK::IEND_CHUNK()
{
    m_length = 0;                   //setting the IEND chunk length (always 0)

    this->m_type = new uint8_t[4];  //setting the IEND type (IEND in Hexadecimal) 
    this->m_type[0] = 0x49; //I
    this->m_type[1] = 0x45; //E
    this->m_type[2] = 0x4E; //N
    this->m_type[3] = 0x44; //D

    //the crc32 calculation algorithm needs the concatened array of the chunk type and the chunk datas, note that this xhunk doesn't have any datas
    m_crc32 = CRC32::getCRC32(this->m_type, 4);   //calculating the crc32 value 
}

/**
 * @brief Destroy the IEND_CHUNK::IEND_CHUNK object
 * 
 */
IEND_CHUNK::~IEND_CHUNK()
{
    delete[] this->m_type;
}

/**
 * @brief save the actual IEND_CHUNK datas(type, length, crc32) to an output file stream
 * 
 * @param outputStream the output file stream reference
 */
void IEND_CHUNK::save(std::ofstream &outputStream)
{
    //we start by converting the (> 1 byte) values into arrays of bytes
    uint8_t *lengthArrayPtr = Utilities::int_to_uint8(m_length);
    uint8_t *crcArrayPtr = Utilities::int_to_uint8(m_crc32);

    //then we write chunks datas in the file stream
    Utilities::stream_write(lengthArrayPtr, 4, outputStream);
    Utilities::stream_write(this->m_type, 4, outputStream);
    Utilities::stream_write(crcArrayPtr, 4, outputStream);

    delete[] lengthArrayPtr;    delete[] crcArrayPtr; //freeing the allocated memory
}