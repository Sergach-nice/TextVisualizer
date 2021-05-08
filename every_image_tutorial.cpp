#include <iostream>
#include <SFML\Graphics.hpp>
#include "tvisual.h"


void ImageLikeText(sf::RenderWindow &window, const char* image_path, const char*out_file, sf::Font &font, unsigned font_size, unsigned block_size){

    sf::Image img;
    if(!img.loadFromFile(image_path)){std::cout << "\ninvalid path!\n"; return;}

    std::string text_image = "";
    vis::PIXBLOCK *converter = new vis::PIXBLOCK(block_size, CHAR_SET_RE);
    text_image = converter->GenerateBlock(img);
    delete converter;

    sf::Text text(text_image, font, font_size);

    window.clear();
    window.draw(text);
    window.display();
    window.capture().saveToFile(out_file);
    window.close();
}

int main(){
    sf::Font font;
    font.loadFromFile("CourierTTBold.ttf");
    sf::RenderWindow window(sf::VideoMode(1080, 600), "Frame", sf::Style::Close);
    sf::View view = window.getDefaultView();
    view.setSize(1080, 1200);
    view.move(0,300);
    window.setView(view);

    ImageLikeText(window, "input.jpg", "output.png", font, 2, 1);

    return 0;
}
