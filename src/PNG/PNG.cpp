
#include "../../include/PNG/PNG.h"
#include "../../include/zlib/zlib.h"
#include "../../include/PNG/Utilities.h"


/**
 * @brief Construct a new PNG::PNG object
 * 
 * @param pixelBuffer the input pixel buffer(raw values) of an image
 * @param s_width  the png width information 
 * @param s_height the png height information
 * @param bitDepth the png bit depth information
 * @param colorMode the png color mode information, only managed are 0(grayscale), 2(RGB true color) and 6(RGBA)
 */
PNG::PNG(const uint8_t *pixelBuffer, int s_width, int s_height, int bitDepth, int colorMode)
{
    m_signature = new uint8_t[8]; // we assign the PNG signature
    m_signature[0] = 0x89;
    m_signature[1] = 0x50;
    m_signature[2] = 0x4E;
    m_signature[3] = 0x47;
    m_signature[4] = 0x0D;
    m_signature[5] = 0x0A;
    m_signature[6] = 0x1A;
    m_signature[7] = 0x0A;

    uint8_t colorChannels {0};
    colorChannels = colorMode == 0x0 ? 1 * (bitDepth / 8):
                    colorMode == 0x1 ? 2 * (bitDepth / 8):
                    colorMode == 0x2 ? 3 * (bitDepth / 8):
                    colorMode == 0x6 ? 4 * (bitDepth / 8): 0;

    // copying pixel buffer
    m_pixelBuffer = new uint8_t[s_width * s_height * colorChannels];
    memcpy(m_pixelBuffer, pixelBuffer, s_width * s_height * colorChannels);

    // setting up criticals png Chunks, calling constructors
    m_IHDR = new IHDR_CHUNK(s_width, s_height, bitDepth, colorMode);
    m_IEND = new IEND_CHUNK();
}


/**
 * @brief Construct a new PNG::PNG object (by copy)
 * 
 * @param png object to be copied
 */
PNG::PNG(const PNG &png_src)
{
    this->m_signature = new uint8_t[8];
    memcpy(this->m_signature, png_src.m_signature, 8);

    this->m_IHDR = new IHDR_CHUNK(png_src.m_IHDR->m_width, png_src.m_IHDR->m_height, png_src.m_IHDR->m_data[0], png_src.m_IHDR->m_data[1]);

    if (png_src.m_pHYs != nullptr) // if source png has a pHYs chunk,
        this->m_pHYs = new PHYS_CHUNK(png_src.m_pHYs->m_ppuX, png_src.m_pHYs->m_ppuY, png_src.m_pHYs->m_unitSpecifier);

    uint8_t colorChannels {0};
    colorChannels = m_IHDR->m_data[1] == 0x0 ? 1 * (png_src.get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x1 ? 2 * (png_src.get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x2 ? 3 * (png_src.get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x6 ? 4 * (png_src.get_bitDepth() / 8): 0;
    
    // copying pixel buffer
    m_pixelBuffer = new uint8_t[png_src.m_IHDR->m_width * png_src.m_IHDR->m_height * colorChannels];
    memcpy(m_pixelBuffer, png_src.m_pixelBuffer, png_src.m_IHDR->m_width * png_src.m_IHDR->m_height * colorChannels);

    this->m_IEND = new IEND_CHUNK();
}


/**
 * @brief affectation operator overloading
 * 
 * @param png_src object to be copied
 * @return PNG object
 */
PNG &PNG::operator=(const PNG &png_src)
{
    this->m_signature = new uint8_t[8];
    memcpy(this->m_signature, png_src.m_signature, 8);

    this->m_IHDR = new IHDR_CHUNK(png_src.m_IHDR->m_width, png_src.m_IHDR->m_height, png_src.m_IHDR->m_data[0], png_src.m_IHDR->m_data[1]);
    
    if (png_src.m_pHYs != nullptr) // if source object has a pHYs chunk,
        this->m_pHYs = new PHYS_CHUNK(png_src.m_pHYs->m_ppuX, png_src.m_pHYs->m_ppuY, png_src.m_pHYs->m_unitSpecifier);

    uint8_t colorChannels {0};
    colorChannels = m_IHDR->m_data[1] == 0x0 ? 1 * (png_src.get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x1 ? 2 * (png_src.get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x2 ? 3 * (png_src.get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x6 ? 4 * (png_src.get_bitDepth() / 8): 0;
        
    // copying pixel buffer
    m_pixelBuffer = new uint8_t[png_src.m_IHDR->m_width * png_src.m_IHDR->m_height * colorChannels];
    memcpy(m_pixelBuffer, png_src.m_pixelBuffer, png_src.m_IHDR->m_width * png_src.m_IHDR->m_height * colorChannels);

    this->m_IEND = new IEND_CHUNK();

    return *this;
}


/**
 * @brief Construct a new PNG::PNG object
 * 
 * @param path the file path of the png file to read
 */
PNG::PNG(const std::string &path)
{
    int s_width(0), s_height(0), pixelsBufferLen(0);
    uint8_t bitDepth(0), colorMode(0), colorChannel(0);

    // read pixels from png file(decoding)
    uint8_t *tmp = PNG::readPixels(path, s_width, s_height, bitDepth, colorMode, colorChannel, pixelsBufferLen);

    // copy the parsed pixelBuffer in our own buffer;
    m_pixelBuffer = new uint8_t[pixelsBufferLen];
    memcpy(m_pixelBuffer, tmp, pixelsBufferLen);

    m_signature = new uint8_t[8]; // we assign the PNG signature
    m_signature[0] = 0x89;
    m_signature[1] = 0x50;
    m_signature[2] = 0x4E;
    m_signature[3] = 0x47;
    m_signature[4] = 0x0D;
    m_signature[5] = 0x0A;
    m_signature[6] = 0x1A;
    m_signature[7] = 0x0A;

    // setting up png basics Chunks
    m_IHDR = new IHDR_CHUNK(s_width, s_height, bitDepth, colorMode);
    m_IEND = new IEND_CHUNK();

    // setting up png Ancilliary Chunks
    std::ifstream png_in(path, std::ios::binary);

    int pos = Utilities::f_strchr(png_in, "pHYs", 0);
    if(pos != -1) // if PhYs Chunk is present
    {
        int ppuX(0), ppuY(0);
        uint8_t unitSpecifier(0);
        
        png_in.seekg(pos, std::ios::beg);
        png_in.seekg(4, std::ios::cur); // skip word "pHYs"

        uint8_t *ppuXArrayPtr = new uint8_t[4]; // read the physical pixel dimension
        for (int i = 0; i < 4; i++)
            png_in >> std::noskipws >> ppuXArrayPtr[i];     // reading
        ppuX = Utilities::uint8_to_int(ppuXArrayPtr); // storing
        delete[] ppuXArrayPtr;

        uint8_t *ppuYArrayPtr = new uint8_t[4]; // we read the physical pixel dimension
        for (int i = 0; i < 4; i++)
            png_in >> std::noskipws >> ppuYArrayPtr[i];     // reading
        ppuY = Utilities::uint8_to_int(ppuYArrayPtr); // storing
        delete[] ppuYArrayPtr;

        png_in >> std::noskipws >> unitSpecifier; // we store the unit specifier
        m_pHYs = new PHYS_CHUNK(ppuX, ppuY, unitSpecifier);
    }

    
    delete[] tmp;
}


/**
 * @brief Destroy the PNG::PNG object
 * 
 */
PNG::~PNG()
{
    delete[] m_signature; delete[] m_pixelBuffer;
    delete m_IHDR;  delete m_pHYs;  delete m_IDAT;  delete m_IEND;
}


/**
 * @brief writing the actual png in a specific directory path
 * 
 * @param path the path to store the png file
 * @param compress_mode output compression level(according to zlib modes)
 * @see IHDR_CHUNK::save
 * @see PHYS_CHUNK::save
 * @see IDAT_CHUNK::save
 * @see IEND_CHUNK::save
 * 
 * @exception std::runtime_error if cannot create file as specified path 
 */
void PNG::save(const std::string &path, int compress_mode)
{
    std::ofstream output_stream(path.c_str(), std::ios::out | std::ios::binary); // Opening the output file stream

    if (output_stream.is_open())
    {
        Utilities::stream_write(m_signature, 8, output_stream);
        m_IHDR->save(output_stream); 

        if (m_pHYs != nullptr) // cause pHYs is an auxiliary chunk, we write it only if its present
            m_pHYs->save(output_stream); 

        uint8_t colorChannels {0};
        colorChannels = m_IHDR->m_data[1] == 0x0 ? 1 * (this->get_bitDepth() / 8):
                        m_IHDR->m_data[1] == 0x1 ? 2 * (this->get_bitDepth() / 8):
                        m_IHDR->m_data[1] == 0x2 ? 3 * (this->get_bitDepth() / 8):
                        m_IHDR->m_data[1] == 0x6 ? 4 * (this->get_bitDepth() / 8): 0;
        
        m_IDAT = new IDAT_CHUNK(m_pixelBuffer, m_IHDR->get_width(), m_IHDR->get_height(), colorChannels, compress_mode);
        m_IDAT->save(output_stream);

        m_IEND->save(output_stream);
    }
    else
        throw std::runtime_error("PNG::save() - Enable to create file at specified path : " + path);

    output_stream.close();
}


/**
 * @brief method for parsing and extracting informations from a specified PNG file
 * @warning only managed are grayscale and rgb images, no indexed colors
 * 
 * @param path the png file path to read/parse
 * @param s_width  the png width information 
 * @param s_height the png height information
 * @param bitDepth the png bit depth information
 * @param colorMode the png color mode information, only managed are 0(grayscale), 2(RGB true color) and 6(RGBA)
 * @param colorChannel the png color Channels according to the colorMode (colorMode->colorChannel)  0 -> 1, 2 -> 3, 6 -> 4
 * @param pixelsBufferLen the length of the pixels Buffer of the png
 * @return either nullptr if an error occurred or the pixels buffer, type uint8_t
 * 
 * @exception std::runtime_error if cannot png file as specified path
 * @exception std::runtime_error if bit depth is different than 8 or 16
 * @exception std::runtime_error if color mode is diffrent than 0(grayscale), 1(grayscale with alpha), 2(RGB), 4(RGBA)
 */
uint8_t *PNG::readPixels(const std::string &path, int &s_width, int &s_height, uint8_t &bitDepth, uint8_t &colorMode, uint8_t &colorChannel, int &pixelsBufferLen)
{
    /*for a dynamic parsing purpose(only reading criticals chunk and pHYs), we need to know the exact position of each chunk, characterized
      here by the chunk type, stored as "word" in a string*/

    std::ifstream input(path, std::ios::in | std::ios::binary); // we start opening the png stream, in binary modes
    if (input.is_open())                         // if opening is okk
    {
        std::string word = "IHDR"; // we start by placing the cursor infront of the header chunk and skip the chunk type ("IHDR")
        input.seekg(Utilities::f_strchr(input, word, 0) + 4);

        // we read the png width and store it
        uint8_t *widthArrayPtr = new uint8_t[4];
        for (int i = 0; i < 4; i++)
            input >> std::noskipws >> widthArrayPtr[i];
        s_width = Utilities::uint8_to_int(widthArrayPtr);
        delete[] widthArrayPtr;

        // same with the png height
        uint8_t *heightArrayPtr = new uint8_t[4];
        for (int i = 0; i < 4; i++)
            input >> std::noskipws >> heightArrayPtr[i];
        s_height = Utilities::uint8_to_int(heightArrayPtr);
        delete[] heightArrayPtr;

        // same with the bitDepth annd color mode
        input >> std::noskipws >> bitDepth;
        if (bitDepth != 0x8 && bitDepth != 0x10)
            throw(std::runtime_error("Invalid PNG bit depth, must be 8 or 16"));

        int channel_size = bitDepth / 8; // represents in how many bytes values are stored for each channel.
        input >> std::noskipws >> colorMode;
        // according to the parsed color mode value, we set the color channel for the output pixelsBuffer.
        if (colorMode == 0)
            pixelsBufferLen = s_height * s_width * (colorChannel = 1*channel_size); // for grayscale images
        else if (colorMode == 4)
            pixelsBufferLen = s_height * s_width * (colorChannel = 2*channel_size); // for grayscale alpha images
        else if (colorMode == 2)
            pixelsBufferLen = s_height * s_width * (colorChannel = 3*channel_size); // for RGB true color images
        else if (colorMode == 6)
            pixelsBufferLen = s_height * s_width * (colorChannel = 4*channel_size); // for RGBA images
        else
            throw std::runtime_error("Only Color modes 0(grayscale), 1(grayscale with alpha), 2(RGB true color) and 6(RGBA) are managed");

        // IDAT chunks parsing, can be single or multiples
        word = "IDAT";
        input.seekg(0, std::ios::beg);
        const std::vector<int> &positions(Utilities::f_strchr(input, word)); // getting all the IDAT chunks positions in a vector

        int deflatedLength(0);
        int *deflatedLengths = new int[positions.size()]; // array which will containt the IDAT chunks deflated datas lengths
        for (std::size_t i = 0; i < positions.size(); i++)        // reading and storing each IDAT chunk length
        {
            input.seekg(positions[i], std::ios::beg);
            input.seekg(-4, std::ios::cur);

            // we read and store the actual chunk length
            uint8_t *deflatedLengthPtr = new uint8_t[4];
            for (int j = 0; j < 4; j++)
                input >> std::noskipws >> deflatedLengthPtr[j];                   // reading
            deflatedLengths[i] = Utilities::uint8_to_int(deflatedLengthPtr);      // storing
            delete[] deflatedLengthPtr;
            deflatedLength += deflatedLengths[i];
        }

        // then now we have all the lengths of each IDAT chunks, next step is fo read deflated datas
        int k(0);
        uint8_t *deflatedBuffer = new uint8_t[deflatedLength]; // mem allocation for the deflated buffer
        for (int i = 0; i < positions.size(); i++)
        {
            input.seekg(positions[i], std::ios::beg); // positioning on each chunk start
            input.seekg(4, std::ios::cur);            // skipping the chunk type, ("IDAT" word)

            for (int j = 0; j < deflatedLengths[i]; j++) // copying the deflated data of each chunk in the buffer
            {
                input >> std::noskipws >> deflatedBuffer[k];
                k++;
            }
        }

        // now we'll inflate(decompress) the deflated pixels and store it into a scanlines buffer
        uint8_t *scanlines = new uint8_t[pixelsBufferLen + s_height];
        unsigned long scanlinesLength(pixelsBufferLen + s_height);
        uncompress(scanlines, &scanlinesLength, deflatedBuffer, deflatedLength); // decompressing...

        // next step is to unfilter each scanline and return the raw buffer
        uint8_t *unfilteredLine[s_height];
        uint8_t *rawBuffer = new uint8_t[pixelsBufferLen]; // the raw buffer memory allocation

        for (int i = 1; i <= s_height; i++)
            memcpy(rawBuffer + (i - 1) * (s_width * colorChannel),                                                                                  // the destination
                   (unfilteredLine[i - 1] = unfilter_line(scanlines + 1 + (i - 1) * (s_width * colorChannel + 1), s_width * colorChannel, // the source which is the result of the unfiltering each scanline
                                                          scanlines[(i - 1) * (s_width * colorChannel + 1)], ((i - 1) == 0) ? false : true,
                                                          ((i - 1) == 0) ? nullptr : rawBuffer + (i - 2) * (s_width * colorChannel), colorChannel)),
                s_width * colorChannel                                                                                                           // the copying length (which is the unfiltered line length)
            );

        input.clear();                                     // clearing the input stream
        delete[] deflatedBuffer; delete[] scanlines; delete[] deflatedLengths; // freeing allocated memory...
        for (int i = 0; i < s_height; i++)  delete[] unfilteredLine[i];

        return rawBuffer; // returning the pixelsBuffer
    }
    else
        throw std::runtime_error("Enable to open the file \"" + path + "\"" );
}


/**
 * @brief unfiltering line method
 * @details png format has many filtering options for improving the compression(deflate)
 * then after decompression(inflate), datas needs to be unfiltered, according to the specified filter method
 * @note filtering and unfiltering methods are applied one each line.
 *
 * @param line_in the input line to unfilter
 * @param lineLength the input line length
 * @param filterMode the filter mode of the actual line ( 0 = none, 1 = Sub, 2 = Up, 3 = Average, 4 = Paeth)
 * @param is_prev_line if the actual line have a predecessor line
 * @param unfiltered_prev_line the predecessor line (already unfiltered)
 * @param colorChannel the number of color channel of the input line
 * @return uint8_t* unfiltered line
 *
 * @exception std::bad_alloc case unfiltered line memory allocation failed
 * @exception std::invalid_argument case Invalid filter mode
 */
uint8_t *PNG::unfilter_line(const uint8_t *line_in, int lineLength, uint8_t filterMode, bool is_prev_line, const uint8_t *unfiltered_prev_line, uint8_t colorChannel)
{
    int i(0);
    uint8_t *line_out = new uint8_t[lineLength]; // unfiltered output buffer
    if (!line_out)
        throw std::bad_alloc();

    switch (filterMode)
    {
    case 0x0: // filter mode 0(none), output buffer is the same as the in buffer
        memcpy(line_out, line_in, lineLength);
        break;

    case 0x1: // filter mode 1(Sub),
        memcpy(line_out, line_in, colorChannel);

        for (i = colorChannel; i < lineLength; i++)
            line_out[i] = (uint8_t)(line_in[i] + line_out[i - colorChannel]);
        break;

    case 0x2: // filter mode 2(Up)
        if (!is_prev_line)
            memcpy(line_out, line_in, lineLength);

        else
            for (i = 0; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] + unfiltered_prev_line[i]);
        break;

    case 0x3: // filter mode 3(Average)
        if (!is_prev_line)
        {
            memcpy(line_out, line_in, colorChannel);

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] + floor((line_out[i - colorChannel]) / 2));
        }
        else
        {
            for (i = 0; i < colorChannel; i++)
                line_out[i] = (uint8_t)(line_in[i] + (floor((unfiltered_prev_line[i]) / 2)));

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] + floor((line_out[i - colorChannel] + unfiltered_prev_line[i]) / 2));
        }
        break;

    case 0x4: // filter mode 4(Paeth)
        if (!is_prev_line)
        {
            memcpy(line_out, line_in, colorChannel);

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] + Utilities::paeth_predictor(line_out[i - colorChannel], 0, 0));
        }
        else
        {
            for (i = 0; i < colorChannel; i++)
                line_out[i] = (uint8_t)(line_in[i] + unfiltered_prev_line[i]); 

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] + Utilities::paeth_predictor(line_out[i - colorChannel], unfiltered_prev_line[i], unfiltered_prev_line[i - colorChannel]));
        }
        break;

    default:
        throw std::invalid_argument("Invalid filter mode is specified : 0x" + std::to_string(filterMode));
        break;
    }
    return line_out;
}

/**
 * @brief get png width
 * 
 * @return int 
 */
int PNG::get_width() const noexcept
{
    return m_IHDR->get_width();
}

/**
 * @brief get png height
 * 
 * @return int 
 */
int PNG::get_height() const noexcept
{
    return this->m_IHDR->get_height();
}

/**
 * @brief get png bit depth
 * 
 * @return uint8_t 
 */
uint8_t PNG::get_bitDepth() const noexcept
{
    return this->m_IHDR->get_bitDepth();
}

/**
 * @brief get png color mode 
 * 
 * @return uint8_t 
 */
uint8_t PNG::get_colorMode() const noexcept
{
    return this->m_IHDR->get_colorMode();
}

/**
 * @brief get png interlacing mode
 * 
 * @return uint8_t 
 */
uint8_t PNG::get_interlacing() const noexcept
{
    return this->m_IHDR->get_interlacing();
}

/**
 * @brief get raw pixels inside a png
 * 
 * @return uint8_t* raw pixels buffer
 */
uint8_t *PNG::get_raw_pixels() const
{
    using namespace std::literals;

    uint8_t colorChannels {0};
    colorChannels = m_IHDR->m_data[1] == 0x0 ? 1 * (this->get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x1 ? 2 * (this->get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x2 ? 3 * (this->get_bitDepth() / 8):
                    m_IHDR->m_data[1] == 0x6 ? 4 * (this->get_bitDepth() / 8): 0;

    int pixels_len = this->m_IHDR->m_width * this->m_IHDR->m_height * colorChannels;  

    uint8_t *output = nullptr;
    try
    {
        output = new uint8_t[pixels_len];
    }
    catch(const std::exception &exception)
    {
        throw std::runtime_error("Error : no memory avaible for getting PNG raw pixels : \n"s + exception.what());
    }

    std::memcpy(output, this->m_pixelBuffer, pixels_len);
    return output;
}
