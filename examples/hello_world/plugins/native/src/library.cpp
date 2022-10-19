#include "library.h"

#include <iostream>

void hello() {
    std::cout << "Hello, World!" << std::endl;
    std::cout << "Okay" << std::endl;
}

void load_lua_bindings() {
    // TODO: invoke this when plugin is loaded, so lua script can access plugin functionalities
}