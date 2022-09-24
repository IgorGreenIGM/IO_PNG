#ifndef _PHYS_CHUNK_H_INCLUDED_
#define _PHYS_CHUNK_H_INCLUDED_

#include <cstdio>
#include <fstream>

/**
 * @brief pHYs CHUNK class, AUXILIARY.
 * 
 */
class PHYS_CHUNK
{
    public : 
        PHYS_CHUNK(unsigned int ppuX, unsigned int ppuY, uint8_t unitSpecifier);
        ~PHYS_CHUNK();
        
        void save(std::ofstream &outputStream);
        bool get_state();

    private :
        bool m_state = false; /**< indicates state of the chunk, if it should be written or not ine the PNG file. @see PNG::save() */
        int m_length; /**< the length of the CHUNK */
        uint8_t *m_type = nullptr; /**< the type of the CHUNK corresponding to the name of the chunk in hexadecimal*/
        unsigned int m_ppuX; /**< the physical pixel dimension (on x axis)*/
        unsigned int m_ppuY; /**< the physical pixel dimension (on y axis)*/
        uint8_t m_unitSpecifier; /**< the unit specifier of the phisical pixel dimension on x and y axis*/
        unsigned long m_crc32; /**< the crc32 value computed from the concatened buffers of type and datas*/
    
    friend class PNG;
};  

#endif // _PHYS_CHUNK_H_INCLUDED_