#ifndef _CRC_32_H_INCLUDED_
#define _CRC_32_H_INCLUDED_

#include <iostream>

/**
 * @brief CRC32 class, for CRC32 algorithm. 
 * 
 */
class CRC32
{
    public :
        CRC32();
        ~CRC32();
        void crc_table_compute();
        static unsigned long getCRC32(uint8_t *chunkDatas, int chunkLen);
        static unsigned long CRC32_update(unsigned long crc, uint8_t *dataCHUNK, int len);
        static void CRC32_table_compute(void);

        static unsigned long crc_table[255];
        static bool crc_table_computed;
};

#endif //_CRC_32_H_INCLUDED_