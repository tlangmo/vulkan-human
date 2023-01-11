#include <stdio.h>

#include <iostream>

#include "engine.h"

int main(int argc, char* argv[])
{
    Engine engine;
    engine.init(800, 600);
    engine.run();
    engine.cleanup();
    return 0;
}
