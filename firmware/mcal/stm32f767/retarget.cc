/// @author Blake Freer
/// @date 2024-04-12
/// @brief Provide empty implementations for undefined functions. The
/// implementations are weak to enable redefinition if needed.

// extern "C" {} is required since the functions are declared in the HAL which
// is written in C
// extern "C" {

// __attribute__((weak)) int _isatty(int fd) {
//     return 0;
// }

// __attribute__((weak)) int _write(int fd, char* ptr, int len) {
//     return 0;
// }

// __attribute__((weak)) int _close(int fd) {
//     return 0;
// }

// __attribute__((weak)) int _lseek(int fd, int ptr, int dir) {
//     return 0;
// }

// __attribute__((weak)) int _read(int fd, char* ptr, int len) {
//     return 0;
// }

// __attribute__((weak)) int _fstat(int fd, struct stat* st) {
//     return 0;
// }
// }