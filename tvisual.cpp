#include "tvisual.h"


std::string vis::to_string(int value){
    std::stringstream s;
    s << value;
    std::string res = s.str();
    return res;
}

std::string vis::DecodeError(int VisErrorCode){
    std::string ret = "Status: {" + to_string(VisErrorCode) + "} | ";
    switch(VisErrorCode){
    case status::make_dir:
        ret += "Invalid directory or impossible to create directory.\n";
    break;
    case status::bad_file:
        ret += "File not found or file haven\'t \'r\' privilege.\n";
    break;
    case status::bad_path:
         ret += "Path is incorrect!\n";
    break;
    case status::success:
        ret += "All right!\n";
    break;
    case status::progress:
        ret += "In progress now...\n";
    break;
    default:
        ret += "Unknown status!\n";
    break;
    }
    return ret;
}

//   Visualizer   PUBLIC: *********************************************************************************************************

vis::Visualizer::Visualizer(const char* VideoDataBase,  sf::Font Font, bool delete_src_directories){
    bool took;
    delete_src_dir = delete_src_directories;

    if(strlen(VideoDataBase) == 0){took = status(vis::status::bad_path); return;}
    DataBase = VideoDataBase;
    if((DataBase.at(DataBase.size() - 1) == '\\') && (DataBase.size() > 4))DataBase.erase(DataBase.size() - 1);
    if(!dir_exist(DataBase.c_str()))_mkdir(DataBase.c_str());
    if(!dir_exist(DataBase.c_str())){took = status(vis::status::make_dir); return;}

    VideoDataPath = DataBase + VIDEO_IDATA;
    if(dir_exist(VideoDataPath.c_str())){if(delete_src_directories)rdir(VideoDataPath.c_str());}
    _mkdir(VideoDataPath.c_str());
    if(!dir_exist(VideoDataPath.c_str())){took = status(vis::status::make_dir); return;}

    TextDataPath = DataBase + TEXT_IDATA;
    if(dir_exist(TextDataPath.c_str()))rdir(TextDataPath.c_str());
    _mkdir(TextDataPath.c_str());
    if(!dir_exist(TextDataPath.c_str())){took = status(vis::status::make_dir); return;}

    RawTextPath = DataBase + TEXT_FOLDER;
    if(dir_exist(RawTextPath.c_str())){if(delete_src_directories)rdir(RawTextPath.c_str());}
    _mkdir(RawTextPath.c_str());
    if(!dir_exist(RawTextPath.c_str())){took = status(vis::status::make_dir); return;}
    v_font = Font;
    took = status(vis::status::success, 1);
}
vis::Visualizer::~Visualizer(){
    if(delete_src_dir){
    rdir(VideoDataPath.c_str());
    rdir(TextDataPath.c_str());
    rdir(RawTextPath.c_str());
    return;
    }

}

bool vis::Visualizer::ConvertVideo(const char* VideoPath, const char* OutputVideoName, sf::RenderWindow &wnd, unsigned text_size, unsigned block_size, sf::Color BackGround, sf::Color TextColor, const char*c_set){

    v_text = sf::Text("", v_font, text_size);
    v_text.setColor(TextColor);
    v_BackGround = BackGround;
    this->wnd = &wnd;
    v_char_set = c_set;
    if(v_status != vis::status::success)return false;
    if(block_size > 0)v_blocksize = block_size;
    else v_blocksize = 1;
    std::cout << VideoPath;
    if(!file_exist(VideoPath)){ return status(vis::status::bad_file);}
    this->VideoPath = VideoPath;

    if(strlen(OutputVideoName) == 0)OutputName = "default";
    else OutputName = OutputVideoName;
    std::cout << "decoding... ";
    v_decode(this->VideoPath.c_str());

    std::cout << " decoding :ok\n";
    std::cout <<  " \"" << OUTPUT_TEXT_FORMAT<< "\" raw text folder  ";

    if(present_as_text()){
        std::cout  << ":ok" << std::endl;
        std::cout << " \"" << OUTPUT_IMAGE_FORMAT<< "\" video idata  \n" ;
        if(present_as_image(wnd)){
            std::cout << " encoding...";
            v_encode((DataBase + '\\' + OutputName).c_str());
            std::cout  << " encoding :ok" << std::endl;
        }else return false;
    }else return false;

    return status(vis::status::success, 1);
}

int vis::Visualizer::GetStatus(){ return v_status; }
bool vis::Visualizer::Visualization(const char* VideoPath, sf::RenderWindow &wnd, unsigned text_size, unsigned block_size, sf::Color BackGround, sf::Color TextColor, const char* c_set){
    v_text = sf::Text("", v_font, text_size);
    v_text.setColor(TextColor);
    v_BackGround = BackGround;
    this->wnd = &wnd;
    v_char_set = c_set;
    if(v_status != vis::status::success)return false;
    if(block_size > 0)v_blocksize = block_size;
    else v_blocksize = 1;
    std::cout << VideoPath;
    if(!file_exist(VideoPath)){ return status(vis::status::bad_file);}
    this->VideoPath = VideoPath;

    std::cout << "decoding... ";
    v_decode(this->VideoPath.c_str());

    std::cout <<  " \"" << OUTPUT_TEXT_FORMAT<< "\" raw text folder  ";
    return present_as_text();
} 
bool vis::Visualizer::toVideo(const char* OutputVideoName, sf::RenderWindow &wnd, sf::Color BackGround, sf::Color TextColor){
    v_text = sf::Text("", v_font, text_size);
    v_text.setColor(TextColor);
    v_BackGround = BackGround;
    this->wnd = &wnd;
    if(strlen(OutputVideoName) == 0)OutputName = "default";
    else OutputName = OutputVideoName;
    if(present_as_image(wnd)){
        std::cout << " encoding...";
        v_encode((DataBase + '\\' + OutputName).c_str());
        std::cout  << " encoding :ok" << std::endl;
    }else return false;
    return status(vis::status::success, 1);
} 

//      PRIVATE: *********************************************************************************************************

inline void vis::Visualizer::v_decode(const char* VideoToDecode){
    std::string command = FFMPEG_FOLDER;
    command += " -i ";
    command += VideoToDecode;
    command += " " + VideoDataPath + "\\image%d";
    command += OUTPUT_IMAGE_FORMAT;
    system(command.c_str());

    command = FFMPEG_FOLDER;
    command += " -i ";
    command += this->VideoPath;
    command += " -vn -ar 44100 -ac 2 -ab 192K -f mp3 ";
    command += VideoDataPath + "\\sound.mp3";
    system(command.c_str());
}

inline void vis::Visualizer::v_encode(const char* VideoToEncode){
    std::string command = FFMPEG_FOLDER;
    command += " -start_number 1 -i ";
    command += TextDataPath;
    command += "\\image%d";
    command += OUTPUT_IMAGE_FORMAT;
    command += " -r 80 -crf 0 -pix_fmt yuv420p ";
    command += VideoDataPath + "\\OLD";
    command += OUTPUT_VIDEO_FORMAT;
    system(command.c_str());

    command = FFMPEG_FOLDER;
    command += " -i ";
    command += VideoDataPath +"\\OLD";
    command += OUTPUT_VIDEO_FORMAT;
    command += " -i ";
    command += VideoDataPath + "\\sound.mp3";
    command += " -c copy -map 0:v:0 -map 1:a:0 ";
    command += VideoDataPath + "\\OLDN";
    command += OUTPUT_VIDEO_FORMAT;
    system(command.c_str());

    command = FFMPEG_FOLDER;
    command += " -i ";
    command += VideoDataPath +"\\OLDN";
    command += OUTPUT_VIDEO_FORMAT;
    command += " -filter_complex \"[0:v]setpts=0.8*PTS[v];[0:a]atempo=1[a]\" -map \"[v]\" -map \"[a]\" ";
    command += VideoToEncode;
    command += OUTPUT_VIDEO_FORMAT;
    system(command.c_str());

}

std::string vis::Visualizer::ReadTextFile(const char* file){
    std::string ret = "";
    if(!file_exist(file))return ret;

    std::fstream ss(file, std::ios_base::in);
    if(!ss.is_open())return ret;

    std::string buff;
    while(std::getline(ss, buff))ret += buff + '\n';
    ss.close();
    return ret;
}

bool vis::Visualizer::file_exist(const char* file){ return (_access(file, 0) == 0); }

bool vis::Visualizer::dir_exist(const char* dir){
    DWORD flag = GetFileAttributesA(dir);
    if(flag == 0xFFFFFFFFUL){
        if(GetLastError() == ERROR_FILE_NOT_FOUND)return false;
    }
    return (flag & FILE_ATTRIBUTE_DIRECTORY);
}

unsigned vis::Visualizer::folder_size(const char* path){
    std::string mpath = path;
    mpath += "\\*";
    unsigned counter = 0;
    WIN32_FIND_DATA ffd;

    HANDLE hFind = FindFirstFileA (mpath.c_str(), &ffd);
    if (hFind != INVALID_HANDLE_VALUE){
        if(strcmp(ffd.cFileName, ".") && strcmp(ffd.cFileName, "..")) counter++;
        while (FindNextFile(hFind, &ffd)){
            if(strcmp(ffd.cFileName, ".") && strcmp(ffd.cFileName, ".."))counter++;
        }
        FindClose(hFind);
    }
    return counter;
}

bool vis::Visualizer::present_as_text(){
    if(v_status != vis::status::success)return false;
    wnd->pollEvent(v_event);
    v_status = vis::status::progress;
    if(!dir_exist(VideoDataPath.c_str())){return status(vis::status::bad_path);}
    int counter = 1;
    std::string image_path;
    std::string text_path;
    sf::Image image;
    sf::Color color;
    vis::PIXBLOCK *pixel_block = new vis::PIXBLOCK(v_blocksize, v_char_set);
    v_text.setPosition(0, 0);
    for(int i = 0; i < folder_size(VideoDataPath.c_str()) - 1; i++){
        wnd->pollEvent(v_event);
        image_path = VideoDataPath + "\\image" + to_string(counter) + OUTPUT_IMAGE_FORMAT;
        text_path = RawTextPath + "\\image" + to_string(counter) + OUTPUT_TEXT_FORMAT;
        if(!image.loadFromFile(image_path))continue;

        std::ofstream file(text_path, std::ios_base::out | std::ios_base::trunc);
        if(!file.is_open())return status(vis::status::bad_file);
        std::string output_string = pixel_block->GenerateBlock(image);
        file << output_string;

        v_text.setString(output_string);
        wnd->clear(v_BackGround);
        wnd->draw(v_text);
        wnd->display();

        file.close();
        counter++;

    }
    system("cls");
    delete pixel_block;
    return status(vis::status::success, 1);
}

bool vis::Visualizer::present_as_image(sf::RenderWindow &wnd){
    if(v_status != vis::status::success)return false;
    wnd.pollEvent(v_event);
    v_status = vis::status::progress;
    std::string image_path;
    std::string text_path;
     sf::Texture texture;
     sf::Image img;
    int counter = 1;
    std::string buffer;
    for(int i = 0; i < folder_size(RawTextPath.c_str()) ; i++){
        wnd.pollEvent(v_event);
        text_path = RawTextPath + "\\image" + to_string(counter) + OUTPUT_TEXT_FORMAT;
        image_path  = TextDataPath + "\\image" + to_string(counter) + OUTPUT_IMAGE_FORMAT;
        buffer = ReadTextFile(text_path.c_str());
        v_text.setString(buffer);
        wnd.clear(v_BackGround);
        wnd.draw(v_text);
        wnd.display();
        texture.loadFromImage(wnd.capture());
        img = texture.copyToImage();

        img.saveToFile(image_path);
        counter++;

    }
    return status(vis::status::success, 1);
}

bool vis::Visualizer::status(int status, bool ret){ v_status = status; return ret; }

void vis::Visualizer::rdir(std::string directory){
    system(("rd /Q /S " + directory).c_str());
}


//   PIXBLOCK   PUBLIC: *********************************************************************************************************

    vis::PIXBLOCK::PIXBLOCK(int sz, std::string char_set){
        size = sz*2;
        cset = char_set;
        if(cset.empty())cset = " .-~oO@";
        block = new uint8_t*[size];
        for(int i = 0; i < size; i++){
            block[i] = new  uint8_t[size];
            for(int g = 0; g < size; g++) block[i][g] = 0;
        }
    }

     vis::PIXBLOCK::~PIXBLOCK(){
        for(int i = 0; i < size; i++)delete [] block[i];
        delete [] block;
    }

    void  vis::PIXBLOCK::clear(){
     for(int i = 0; i < size; i++)
        for(int g = 0; g < size; g++) block[i][g] = 0;
    }

    std::string  vis::PIXBLOCK::GenerateBlock(sf::Image src){

        std::string ret = "";
        unsigned h_offset = 0;
        unsigned w_offset = 0;
        sf::Color cbuff;

        for(;h_offset < src.getSize().y; h_offset += size){
            if( (h_offset + size) > src.getSize().y)h_offset -= (h_offset + size) - src.getSize().y;

            for(w_offset = 0 ;w_offset < src.getSize().x; w_offset += size){
                if( (w_offset + size) > src.getSize().x)w_offset -= (w_offset + size) - src.getSize().x;
                for(int sy = 0; sy < size; sy++){
                    for(int sx = 0; sx < size; sx++){
                        if(h_offset + sy <= src.getSize().y && w_offset + sx <= src.getSize().x){
                        cbuff = src.getPixel(w_offset + sx, h_offset + sy);
                        block[sy][sx] = format(cbuff);
                        }
                    }
                }

                    ret += GetCharFormBlock();
            }
            ret += '\n';
        }
        //std::cout <<ret <<std::endl;
        //system("pause");
        return ret;
    }


//  PRIVATE: *********************************************************************************************************

    char  vis::PIXBLOCK::GetCharFormBlock(){
        int src = 0;
        for(int i = 0; i < size; i++)
            for(int g = 0; g < size; g++) src +=(int)block[i][g];
        unsigned sz = (size*size);
        if(sz == 0)sz = 1;
        src = src/sz;
        int jumper = 255/cset.size();
        for(int i = 0; i < cset.size(); i++){
            if((((i - 1) * jumper) <= src) && (((i + 1) * jumper) >= src))return cset.at(i);
        }
        return char(cset.at(cset.size()-1));
    }

    uint8_t  vis::PIXBLOCK::format(sf::Color &pixel){
        return uint8_t((pixel.r + pixel.g + pixel.b)/3);
    }


