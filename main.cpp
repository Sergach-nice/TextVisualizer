#include <iostream>
#include <SFML\Graphics.hpp>
#include "tvisual.h"

#define InVideo "..."
#define VName "default"

int main(){


    sf::Font font;
    font.loadFromFile("CourierTTBold.ttf");
    vis::Visualizer* visual = new vis::Visualizer("D:\\TEST", font, 1);


    if(visual->GetStatus() == vis::status::success){


        sf::ContextSettings settings;
        //settings.antialiasingLevel = 8; //?????????

        sf::RenderWindow window(sf::VideoMode(1400, 1000), "Frame", sf::Style::Resize, settings);
                                                 //play with this settings
        sf::View view = window.getDefaultView(); //
        view.setSize(1400, 760*2);               //
        view.move(-10, 220);                     //
        window.setView(view);                    //

                                                 //       3, 8 :)
        if(!visual->ConvertVideo(InVideo, VName, window, 3, 10, sf::Color::Black, sf::Color::White, CHAR_SET_RE))std::cout << vis::DecodeError(visual->GetStatus());
        window.close();
    }else std::cout << vis::DecodeError(visual->GetStatus());

    delete visual;
    return 0;
}











