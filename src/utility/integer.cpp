//
// Created by Ryen Zhao on 26/10/2023.
//
#include "integer.h"

Integer::Integer() {
    Integer::length = 1;
    Integer::digit = {0};
    Integer::unit_bits = 8;
}
Integer::Integer(std::string target) {
    Integer::length = target.length();

}
