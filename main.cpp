#include <src/window.h>
#include <windows.h>

auto main(int argc, char** argv) -> int {
    Window window{};

    window.run();
    return 0;
}

int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return main(0, nullptr);
}