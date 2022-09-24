
#include "../../include/PNG/CRC32.h"


bool CRC32::crc_table_computed = false; //static bool for test if the table is already computes or not
unsigned long CRC32::crc_table[255];    //crc table static var exempt recomputation many crc_table recompution

/**
 * @brief crc calculation method
 * 
 * @param chunkDatas the buffer that crc32 should be performed on
 * @param chunkDatasLen the size of the input buffer (chunkDatas)
 * @return the crc32 calculated
 */
unsigned long CRC32::getCRC32(uint8_t *chunkDatas, int chunkDatasLen)
{
    return CRC32_update(0xffffffffL, chunkDatas, chunkDatasLen) ^ 0xffffffffL;
}

/**
 * @brief crc table computing method
 * 
 */
void CRC32::CRC32_table_compute()
{
    unsigned long c;
    int i, j;
    for (i = 0; i < 256; i++)
    {
        c = i;
        for (j = 0; j < 8; j++)
        {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[i] = c;
    }
    crc_table_computed = true;
}

/**
 * @brief crc32 update method
 * 
 * @param crc 
 * @param dataCHUNK 
 * @param len 
 * @return unsigned long 
 */
unsigned long CRC32::CRC32_update(unsigned long crc, uint8_t *dataCHUNK, int len)
{
    if (!crc_table_computed)
        CRC32_table_compute();
        
    for (int i = 0; i < len; i++)
        crc = crc_table[(crc ^ dataCHUNK[i]) & 0xff] ^ (crc >> 8);

    return crc;
}