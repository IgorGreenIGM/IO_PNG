// SIMPLE FILE DECODE EXEMPLE

#include <iostream>
#include <utility>
#include "../include/PNG/PNG.h"

int main()
{
    // decode then encode png file
    PNG png_in("example.png"); 
    png_in.save("out.png", PNG::COMPRESS::BEST);
    
    // decode then retrive png informations
    auto pixels = std::unique_ptr<uint8_t>(png_in.get_raw_pixels());    
    int width = png_in.get_width();
    int height = png_in.get_height();
    int color_mode = png_in.get_colorMode();
    
    std::cout << "dimensions " << with << "x" << height << "color mode : " << color_mode << std::endl;
    return 0;
}
