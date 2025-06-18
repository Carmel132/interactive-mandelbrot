#include <iostream>
#include <src/window.h>

int main(int argc, char** argv) {
    std::cout << "Hello world!\n";

    Window w{};

    w.run();
    return 0;
}