
#include <src/window.h>
#include <limits>
auto main(int argc, char** argv) -> int {
    std::cout << std::numeric_limits<double>::digits10 << ", " << std::numeric_limits<long double>::digits10 << "\n";
    Window w{};

    w.run();
    return 0;
}