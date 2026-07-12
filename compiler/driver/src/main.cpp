#include "myc/driver/compiler_driver.hpp"

int main(int argc, char* argv[]) {
    myc::driver::CompilerDriver driver;
    return driver.Run(argc, argv);
}
