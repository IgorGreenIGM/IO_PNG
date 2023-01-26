<h2>📚 Resume</h2>

This is a full Modern C++ Object Oriented library for PNG file format Encode/Decode.<br>
This library designed to be tiny as possible, purpose severals functions that make easier Pixels Manipulations inside PNG images.

<h2>💡 Idea</h2>

...Some day, I was looking for a structured PNG library written in C++ for my project. but I could not find anything simple enough, lightweight and Modern. Everything what I tried required 'old c methods' to integrate or had some limitations or was too heavy. I wanted something powerfull and small enough, following new coding 'standards'. <br>This was the reason why I decided to write this Encoder/Decoder, which no need to manipulate directly memory, with OOP and exceptions features. 

<h2>🛠️ Features</h2>

- full OOP paradigm
- Exceptions Handling
- multiple IDAT chunks
- auto-detect endianess
- PHYS additionnal chunk
- CRC32 computing algortithm
- Hardware-independent processing
- compress ratio option for encode 
- Simple and double bit Depths (8 & 16)
- Partial Parsing(rapid informations retrieve)
- Various colors modes (grayscale, grayscale alpha, RGB, RGBA)
- MultiThreading dynamic scanline filtering(better time-size compress ratio)  

<h2>⚙️ Building</h2>
Makefile and Windows compiling files are provided, just copy src and includes files in your project folder.<br>
This project use zlib, so to avoid using dll is to directly include zlib source into your project. 

<h2>🏴󠁶󠁥󠁷󠁿 Dependencies </h2>
zlib 1.2.3 
