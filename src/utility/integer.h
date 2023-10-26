//
// Created by Ryen Zhao on 26/10/2023.
//
#ifndef PIXEL_ENGINE_INTEGER_H
#define PIXEL_ENGINE_INTEGER_H

#include <string>

class Integer{
public:

    // Initialization functions
    Integer();
    Integer(std::string target);
    ~Integer();
private:
    // Variables
    int length;
    int digit;
    int unit_bits;

};
#endif //PIXEL_ENGINE_INTEGER_H
