//
// Created by Sebastian Borsch on 01.07.23.
//

#ifndef JAREP_RENDERER_HPP
#define JAREP_RENDERER_HPP

#include <iostream>
class Renderer {
    public:
        Renderer();
        ~Renderer();

        void test(){
            std::cout<<"Hi from the rendering department!"<<std::endl;
        }



};

Renderer::Renderer() {

}

Renderer::~Renderer() {

}


#endif //JAREP_RENDERER_HPP
