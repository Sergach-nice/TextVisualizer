#ifndef TVISUAL_H
#define TVISUAL_H

#include <SFML\Graphics.hpp>
#include <windows.h>
#include <direct.h>
#include <fstream>
#include <sstream>
#include <iostream>
//********************************************************************************

#define CHAR_SET "@Oo~-. "
#define CHAR_SET_RE " .-~oO@"
#define OUTPUT_VIDEO_FORMAT ".mp4"
#define OUTPUT_TEXT_FORMAT ".txt"
#define OUTPUT_IMAGE_FORMAT ".png"
#define VIDEO_IDATA "\\VID"
#define TEXT_IDATA "\\TID"
#define TEXT_FOLDER "\\RTF"
#define FFMPEG_FOLDER "ffmpeg"  /// as for me, i already installed it in main folder ( i have used it for CMD )
                                                           /// https://blog.pruffme.com/instrukciya-po-ustanovke-i-rabote-s-programmoj-ffmpeg-na-windows/
//********************************************************************************

namespace vis{

std::string to_string(int value);

namespace status{
    enum status{
        make_dir = -3,
        bad_file,
        bad_path,
        success,
        progress};
}

std::string DecodeError(int VisErrorCode);

class PIXBLOCK{
private:
    uint8_t **block;/// for conversion
    int size;
    std::string cset;
    char GetCharFormBlock();          /// converts size*size pixels -> 1 symbol
    uint8_t format(sf::Color &pixel); /// rgb to 0...255

public:
    PIXBLOCK(int sz, std::string char_set);
    ~PIXBLOCK();
    void clear();
    std::string GenerateBlock(sf::Image src); /// image -> ([block with pixels]->symbol)*height*width -> text


};


class Visualizer{

private:
    int v_status;
    int v_process_stat; // ???
    unsigned v_blocksize;
    sf::Font v_font;
    sf::Text v_text;
    sf::Event v_event;
    sf::RenderWindow *wnd;
    sf::Color v_BackGround;
    std::string v_char_set;
    std::string VideoPath;
    std::string OutputName;
    std::string DataBase;
    std::string VideoDataPath;
    std::string TextDataPath;
    std::string RawTextPath;
    bool delete_src_dir;

    inline void v_decode(const char* VideoToDecode); ///.mp4 to .png   {1}    ?inline :)
    inline void v_encode(const char* VideoToEncode); ///.png to .mp4   {4}
    std::string ReadTextFile(const char* file);      /// reads all stuff from file
    bool file_exist(const char* file);
    bool dir_exist(const char* dir);
    unsigned folder_size(const char* path);
    bool present_as_text();                         /// .png to .txt    {2}
    bool present_as_image(sf::RenderWindow &wnd);   /// .txt to .png   {3}
    bool status(int status, bool ret = 0);
    void rdir(std::string src);                     ///remove directory


public:
    ///       (BaseDirectory,                      Font, clear all 'short' directories  )
    Visualizer(const char* VideoDataBase, sf::Font Font, bool delete_src_directories = 0);
    ~Visualizer();
    ///              (in Video,                                out Name!,                Window,   text (font size), pixel_block_to_symbol_size, FrameBackGround,         TextColor,      char_set!!!!)
    bool ConvertVideo(const char* VideoPath, const char* OutputVideoName, sf::RenderWindow &wnd, unsigned text_size, unsigned block_size, sf::Color BackGround, sf::Color TextColor, const char* c_set);
    ///(vis::status)
    int GetStatus();
    /// Saves result of visualization i "RTF"
    bool Visualization(const char* VideoPath, sf::RenderWindow &wnd, unsigned text_size, unsigned block_size, sf::Color BackGround, sf::Color TextColor, const char* c_set);
    /// convert RTF to video
    bool toVideo(const char* OutputVideoName, sf::RenderWindow &wnd, unsigned text_size, sf::Color BackGround, sf::Color TextColor);
};


}

#endif // TVISUAL_H
