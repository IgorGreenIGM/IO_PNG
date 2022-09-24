
#include "../../include/PNG/Utilities.h"

/**
 * @brief method for converting an integer value into an array of uint8_t
 *
 * @param number the number to convert
 * @return the pointer to the converted array
 */
uint8_t *Utilities::int_to_uint8(int number)
{
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&number);
    uint8_t *result(nullptr);

    if (is_bigEndian())
        result = ptr;
    else
    {
        result = invertArray(ptr, 4);
        delete[] ptr;
    }
    return result;
}

/**
 * @brief method for converting an uint8_t array to an integer value
 *
 * @param ptr a pointer to the uint8_t array
 * @return the integer value
 */
int Utilities::uint8_to_int(uint8_t *ptr)
{
    int result(0);
    for (int i = 0; i < 4; i++)
        result += ptr[i] * pow(256, 4 - 1 - i);

    return result;
}

/**
 * @brief method for inverting an array of uint8_t
 *
 * @param array the input array
 * @param len the input array size
 * @return a pointer to the inverted array
 */
uint8_t *Utilities::invertArray(uint8_t *array, int len)
{
    uint8_t *invertedArray = new uint8_t[len];
    for (int i = 0; i < len; i++)
        invertedArray[i] = array[len - i - 1];
    return invertedArray;
}

/**
 * @brief method for concat 02 arrays of uint8_t
 *
 * @param array1 the first input array
 * @param array2 the second inpiut array
 * @param len1 the first input array size
 * @param len2 the second input array size
 * @return a pointer to the concated array result
 */
uint8_t *Utilities::getConcatenedArray(uint8_t *array1, uint8_t *array2, int len1, int len2)
{
    uint8_t *concatenedArray = new uint8_t[len1 + len2];
    memcpy(concatenedArray, array1, len1);
    memcpy(concatenedArray + len1, array2, len2);
    return concatenedArray;
}

/**
 * @brief method for detecting the endianess of the actual computer,
 * @return either true if it's Big endian or false if Little endian
 */
bool Utilities::is_bigEndian(void)
{
    int number(1);
    uint8_t *arr = reinterpret_cast<uint8_t *>(&number);

    if (arr[0] == 0)
        return true;
    else
        return false;
    delete[] arr;
}

/**
 * @brief method for calculating an input file length
 *
 * @param input the input file stream
 * @return the file length
 */
int Utilities::f_len(std::ifstream &input)
{
    int prev_pos = input.tellg(); // storing the initial cursor position
    input.seekg(0, std::ios::end);

    int total_len(input.tellg());    // calculating the length
    input.seekg(prev_pos, std::ios::beg); // restoring the initial cursor position
    return total_len;
}

/**
  @brief method for getting the cursor position of the first occurence of a string in an input file stream
  @param input the the input file stream
  @param word the string to search
  @param limit the limit length inwhich to search the word, default = 0 occurs research in the limit specified length
  @return -1 either the string is not found or the word position(word included)
 */
int Utilities::f_strchr(std::ifstream &input, std::string word, int limit)
{
    int prev_pos = input.tellg(); // saving the previous cursor position
    input.seekg(0, std::ios::beg);     

    bool is_found(false);
    uint8_t actual(0);
    std::size_t i(0), j(0);

    if (!limit) // if the limits is defaut (0), we'll search word in entire file 
        limit = f_len(input);

    for (i = 0, j = 0; i < limit && j < word.size(); i++)
    {
        input >> std::noskipws >> actual;   // reading, disabling caracters skipping
        if (actual == word.c_str()[j])
        {
            is_found = true;
            j++;
        }
        else
        {
            is_found = false;
            j = 0;
        }
    }
    input.seekg(prev_pos, std::ios::beg); // re-placing the cursor at its previous position
    if (is_found && j == word.size())
        return (i - word.size());

    return -1;
}

/**
  @brief method for getting the cursor position of the first occurence of a string in an input file stream
  @param input the the input file stream
  @param word the string to search
  @return a vector including positions of specified word occurences
 */
std::vector<int> Utilities::f_strchr(std::ifstream &input, const std::string word)
{
    int prev_pos = input.tellg(); // saving the previous cursor position
    input.seekg(0, std::ios::beg);

    std::vector<int> positions; 
    int pos(0), i(0);

    do
    {
        bool is_found(false);
        uint8_t actual(0);
        int j(0);

        int limit = f_len(input);                      
        for (j = 0; i < limit && j < word.size(); i++) 
        {
            input >> std::noskipws >> actual;   // reading, disabling specials caracters skipping 
            if (actual == word.c_str()[j]) 
            {
                is_found = true;
                j++;
            }
            else
            {
                is_found = false; 
                j = 0;
            }
        }

        // if an occurence is found, we store it position in the vector
        if (is_found && j == word.size())
        {
            pos = i - word.size();
            positions.push_back(pos);
        }
        // else we indicate there's no others occurences after this
        else
            pos = -1;
    } while (pos != -1);

    input.seekg(prev_pos, std::ios::beg); // re-place the cursor in the previous position
    return positions;                
}


/**
 * @brief flipping bottom-left pixelsBuffer to top-left
 * @param pixelsBuffer the pixelsBuffer input
 * @param s_width the the pixels buffer image width
 * @param s_heigth the pixels buffer image height
 * @param colorChannel the number of channels in the pixel buffer
 *
 */
void Utilities::flipPixels(uint8_t *pixelsBuffer, int s_width, int s_heigth, int colorChannel)
{
    int totalLength = s_width * s_heigth * colorChannel;
    int oneLineLength = s_width * colorChannel;

    uint8_t *tmp = new uint8_t[totalLength];
    memcpy(tmp, pixelsBuffer, totalLength);
    for (int i = 0; i < s_heigth; i++)
        memcpy(pixelsBuffer + oneLineLength * i, tmp + totalLength - oneLineLength * (i + 1), oneLineLength);
    delete[] tmp;
}

/**
 * @brief the paeth predictor method
 *
 * @param left the left pixel value
 * @param up the up pixem value
 * @param upperLeft the upperleft pixel value
 * @return the paeth predicted value
 */
int Utilities::paeth_predictor(uint8_t left, uint8_t up, uint8_t upperLeft)
{
    int p = (left + up - upperLeft);
    int p_left = abs(p - left), p_up = abs(p - up), p_upperLeft = abs(p - upperLeft);

    if (p_left <= p_up && p_left <= p_upperLeft)
        return left;
    else if (p_up <= p_upperLeft)
        return up;
    else
        return upperLeft;
}


/**
 * @brief method for writing a uint8_t buffer in a specific file stream
 *
 * @param src the source buffer
 * @param src_size the source buffer size
 * @param ouputStream the file stream in which to write
 */
void Utilities::stream_write(const uint8_t *src, int src_size, std::ofstream &ouputStream)
{
    for (int i = 0; i < src_size; i++)
        ouputStream << std::noskipws << src[i];
}


/**
 * @brief Method for counting the number of elements(without redundance) inside an input buffer. 
 * 
 * @param buffer input buffer
 * @param buffer_len input buffer size
 * @return int the number differents values inside the buffer
 */
int Utilities::get_cardinal(uint8_t *buffer, int buffer_len) noexcept
{       
    std::vector<uint8_t> computed;
    for(int i = 0; i < buffer_len; ++i)
    {
        bool compute = true;
        for(auto value : computed)
            if(buffer[i] == value)
            {
                compute = false;
                break;
            }
 
        if(compute)
            computed.push_back(buffer[i]);
    }

    return static_cast<int>(computed.size());
}