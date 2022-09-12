#ifndef _UTILITIES_H_INCLUDED_
#define _UTILITIES_H_INCLUDED_

#include <iostream>
#include <fstream>
#include <vector>

/**
 * @brief a set of method, not diretly useful for pixels managing or image processing but usefull for facilitate work
 */
namespace Utilities
{
    bool is_bigEndian(void);

    uint8_t *int_to_uint8(int number);
    int uint8_to_int(uint8_t *ptr);

    uint8_t *invertArray(uint8_t *array, int len);
    uint8_t *getConcatenedArray(uint8_t *array1, uint8_t *array2, int len1, int len2);

    int f_len(std::ifstream &input);
    int f_strchr(std::ifstream &input, std::string word, int limit);
    std::vector<int> f_strchr(std::ifstream &input, const std::string word);

    void stream_write(const uint8_t *src, int src_size, std::ofstream &ouputStream);

    void flipPixels(uint8_t *pixelsBuffer, int s_width, int s_heigth, int colorChannel);
    int PaethPredictor(uint8_t left, uint8_t up, uint8_t upperLeft);

    template <typename T>
    void quicksort(T *array, int low, int high);
};

#endif //_UTILITIES_H_INCLUDED_