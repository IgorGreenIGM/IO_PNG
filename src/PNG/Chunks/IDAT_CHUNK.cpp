
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
IDAT_CHUNK::IDAT_CHUNK(const uint8_t *pixelsBuffer, int s_width, int s_height, int colorChannel)
{
    this->m_type = new uint8_t[4]; // setting the IDAT type (IDAT in Hexadecimal)
    this->m_type[0] = 0x49;        // I
    this->m_type[1] = 0x44;        // D
    this->m_type[2] = 0x41;        // A
    this->m_type[3] = 0x54;        // T

    m_data = deflate_datas(pixelsBuffer, s_width, s_height, colorChannel, m_length); // getting the deflated data output

    // the crc32 calculation algorithm needs the concatened array of the chunk type and the chunk datas
    uint8_t *dataCRC = Utilities::getConcatenedArray(this->m_type, m_data, 4, m_length);
    m_crc32 = CRC32::getCRC32(dataCRC, 4 + m_length);
    delete[] dataCRC;
}

/**
 * @brief Destroy the idat IDAT_CHUNK::IDAT_CHUNK object
 *
 */
IDAT_CHUNK::~IDAT_CHUNK()
{
    delete[] this->m_type;
    delete[] m_data; // freeing the allocated memory
}

/**
 * @brief save the actual IDAT_CHUNK datas(type, length, datas, crc32) to a specific output file stream
 *
 * @param outputStream the output file stream reference
 */
void IDAT_CHUNK::save(std::ofstream &outputStream)
{
    // we start by converting the (> 1 byte) values into arrays of bytes
    uint8_t *lengthArrayPtr = Utilities::int_to_uint8(this->m_length);
    uint8_t *crc32ArrayPtr = Utilities::int_to_uint8(this->m_crc32);

    // then we write chunk datas in the file stream
    Utilities::stream_write(lengthArrayPtr, 4, outputStream);
    Utilities::stream_write(this->m_type, 4, outputStream);
    Utilities::stream_write(this->m_data, this->m_length, outputStream);
    Utilities::stream_write(crc32ArrayPtr, 4, outputStream);

    delete[] lengthArrayPtr;
    delete[] crc32ArrayPtr; // freeing the bytes arrays
}

/**
 * @brief method for generate scanlines from a specified pixels buffer.
 * @details for image size optimisation, this method is based on a simple way : 
 * for each pixel line, we test all the filtering mode and get the one in which the filtered line has the highest values repetitons(lowest set cardinal).
 * @note Now supports multi-threading mode!!
 * @warning case the image height is too small for multi threading supports, the process will be executed in a single thread.
 * @param pixels input pixels buffer
 * @param s_width pixels buffer width
 * @param s_height pixels buffer height
 * @param colorChannel pixels buffer color channel number
 * @return uint8_t* output filtered scanline
 */
uint8_t *IDAT_CHUNK::generate_scanlines(const uint8_t *pixels, int s_width, int s_height, int colorChannel)
{        
    // lambda for generating scanlines...
    auto generate = [this](const uint8_t *pixels, int s_width, int s_height, int colorChannel, bool is_prev_line) -> uint8_t*
    {
        uint8_t *scanlines = new uint8_t[s_height * (1 + s_width * colorChannel)]; // output

        uint8_t *tmp_filtered_line{nullptr};          // temp filtered lines buffer
        std::vector<uint8_t> filters_modes(s_height); // lowest filter modes container
        for (int i = 1; i <= s_height; ++i)           // testing each filter mode and stores the one with lowest Cardinal.
        {
            std::vector<int> filterMode_cardinal;
            for (uint8_t tmp_filter_mode = 0; tmp_filter_mode <= 4; ++tmp_filter_mode)
            {
                tmp_filtered_line = filter_line(pixels + (i - 1) * (s_width * colorChannel), // filtering
                                                s_width * colorChannel,
                                                tmp_filter_mode,
                                                (i - 1) == 0 && !is_prev_line ? false : true,
                                                (i - 1) == 0 && !is_prev_line ? nullptr : pixels + (i - 2) * (s_width * colorChannel),
                                                colorChannel);

                filterMode_cardinal.push_back(Utilities::get_cardinal(tmp_filtered_line, s_width * colorChannel)); // conputing Cardinal
                delete[] tmp_filtered_line;
            }

            // storing filter mode with lowest Cardinal
            auto min = std::min_element(filterMode_cardinal.begin(), filterMode_cardinal.end());
            filters_modes[i - 1] = min - filterMode_cardinal.begin();
            filterMode_cardinal.clear();
        }

        uint8_t **filtered_line = new uint8_t *[s_height]; // filtering all pixels lines with the previously stored filters modes
        for (int i = 1; i <= s_height; i++)
            memcpy(
                scanlines + 1 + (i - 1) * (1 + s_width * colorChannel),
                (filtered_line[i - 1] = filter_line(pixels + (i - 1) * (s_width * colorChannel),
                                                    s_width * colorChannel, filters_modes[i - 1],
                                                    (i - 1) == 0 && !is_prev_line ? false : true,
                                                    (i - 1) == 0 && !is_prev_line ? nullptr : pixels + (i - 2) * (s_width * colorChannel),
                                                    colorChannel)),
                s_width * colorChannel);

        // writing filter mode bit in each line.
        for (int i = 1; i <= s_height; ++i)
            scanlines[(i - 1) * (1 + s_width * colorChannel)] = filters_modes[i - 1];

        for (int i = 0; i < s_height; i++)
            delete[] filtered_line[i];
        filters_modes.clear();

        return scanlines;
    };

    uint8_t *scanlines_out = new uint8_t[s_height * (1 + s_width * colorChannel)]; // output
    int thread_number = std::thread::hardware_concurrency(); // getting logical UC avaible on computer

    // cause this method separates the input buffer in equals parts(in terms of lines : s_height) for computing,
    // we'll round(down) the threads number to correspond as dividing equally the buffer
    const bool is_dividor{s_height % thread_number == 0};
    int eff_threads{thread_number};
    if (!is_dividor)
    {
        for (std::size_t i = thread_number; i >= 1; --i)
            if ((s_height % i) == 0)
            {
                eff_threads = i;
                break;
            }
    }

    // case the image height is too small for multi threading supports, 
    // the process will be executed in a single thread.
    if(s_height < eff_threads)
        return generate(pixels, s_width, s_height, colorChannel, false);

    // threads related declarations. _s suufix means plural
    std::vector<std::thread> task_s;
    std::vector<std::future<uint8_t *>> future_s;
    std::vector<std::promise<uint8_t *>> promise_s(eff_threads);

    std::vector<uint8_t *> result_s; // vector for storing computed results

    // lambda for thread creation
    auto thread_create_s = [&generate](const uint8_t *pixels, int s_width, int s_height, int colorChannel, bool is_prev_line, std::promise<uint8_t *> &p)
    {
        p.set_value(generate(pixels, s_width, s_height, colorChannel, is_prev_line));
    };

    int thread_height = s_height / eff_threads;
    int thread_buff_len{thread_height * s_width * colorChannel};

    // creating and storing threads in out task list
    for (std::size_t i = 0; i < eff_threads; ++i) 
        task_s.emplace_back(std::thread(thread_create_s, pixels + (i * thread_buff_len), s_width, thread_height, colorChannel, (i == 0) ? false : true, std::ref(promise_s[i])));
    
    int i{0};
    for (i = 0; i < eff_threads; ++i) // futures results for each thread
        future_s.emplace_back(promise_s[i].get_future());

    for (i = 0; i < eff_threads; ++i) // waiting for all threads to finish
        task_s[i].join();

    for (i = 0; i < eff_threads; ++i) // storing results
        result_s.push_back(future_s[i].get());

    for (i = 0; i < eff_threads; ++i) // copying results to output buffer
        std::memcpy(scanlines_out + (i * (thread_buff_len + thread_height)), result_s[i], thread_buff_len + thread_height); 

    for (i = 0; i < eff_threads; ++i) // free results for each thread.
        delete[] result_s[i];
    
    return scanlines_out;
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
uint8_t *IDAT_CHUNK::deflate_datas(const uint8_t *pixelBuffer, int s_width, int s_height, int colorChannel, int &deflatedLen)
{
    unsigned long inLen = s_height * (1 + s_width * colorChannel), tmpLen = 0;            // input len of scanlines datas
    uint8_t *scanlines = generate_scanlines(pixelBuffer, s_width, s_height, colorChannel); // generating scanlines from the pixels

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
    deflatedLen = tmpLen;   // copying the deflated data length to the IDAT->length attribut
    delete[] scanlines;

    return deflatedDatas;
}

/**
 * @brief filtering line method
 * @details png format has many filtering options for improving the compression(deflate)
 * then after decompression(inflate), datas needs to be unfiltered, according to the specified filter method
 * @note filtering and unfiltering methods are applied one each line.
 *
 * @param line_in the input line to unfilter
 * @param lineLength the input line length
 * @param filterMode the filter mode of the actual line ( 0 = none, 1 = Sub, 2 = Up, 3 = Average, 4 = Paeth)
 * @param is_prev_line if the actual line have a predecessor line
 * @param unfiltered_prev_line the predecessor line (no filtered)
 * @param colorChannel the number of color channel of the input line
 * @return uint8_t* filtered line
 *
 * @exception std::bad_alloc case filtered line memory allocation failed
 * @exception std::invalid_argument case Invalid filter mode
 */
uint8_t *IDAT_CHUNK::filter_line(const uint8_t *line_in, int lineLength, uint8_t filterMode, bool is_prev_line, const uint8_t *unfiltered_prev_line, uint8_t colorChannel)
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
            line_out[i] = (uint8_t)(line_in[i] - line_in[i - colorChannel]);
        break;

    case 0x2: // filter mode 2(Up)
        if (!is_prev_line)
            memcpy(line_out, line_in, lineLength);

        else
            for (i = 0; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] - unfiltered_prev_line[i]);
        break;

    case 0x3: // filter mode 3(Average)
        if (!is_prev_line)
        {
            memcpy(line_out, line_in, colorChannel);

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] - floor((line_in[i - colorChannel]) / 2));
        }
        else
        {
            for (i = 0; i < colorChannel; i++)
                line_out[i] = (uint8_t)(line_in[i] - (floor((unfiltered_prev_line[i]) / 2)));

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] - floor((line_in[i - colorChannel] + unfiltered_prev_line[i]) / 2));
        }
        break;

    case 0x4: // filter mode 4(Paeth)
        if (!is_prev_line)
        {
            memcpy(line_out, line_in, colorChannel);

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] - Utilities::paeth_predictor(line_in[i - colorChannel], 0, 0));
        }
        else
        {
            for (i = 0; i < colorChannel; i++)
                line_out[i] = (uint8_t)(line_in[i] - unfiltered_prev_line[i]);

            for (i = colorChannel; i < lineLength; i++)
                line_out[i] = (uint8_t)(line_in[i] - Utilities::paeth_predictor(line_in[i - colorChannel], unfiltered_prev_line[i], unfiltered_prev_line[i - colorChannel]));
        }
        break;

    default:
        throw std::invalid_argument("Invalid filter mode is specified : 0x" + std::to_string(filterMode));
        break;
    }
    return line_out;
}