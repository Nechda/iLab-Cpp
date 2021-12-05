#include "App.hpp"

int main() {
    App app;
    try {
        app.run();
    } catch (std::exception &e) {
        printf("what() = %s", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
