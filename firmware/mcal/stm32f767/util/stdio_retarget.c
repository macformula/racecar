#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <limits.h>
#include <signal.h>
#include <stdint.h>

#include "stdio_retarget.h"

#ifdef HAL_UART_MODULE_ENABLED

#if !defined(OS_USE_SEMIHOSTING)

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

static UART_HandleTypeDef *debug_uart;

void SetDebugUART(UART_HandleTypeDef *huart) {
  debug_uart = huart;

  /* Disable I/O buffering for STDOUT stream, so that
   * chars are sent out as soon as they are printed. */
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
}

// Inspired by: https://github.com/cnoviello/mastering-stm32/blob/024664974feefb74dfa4f234e7a9a8b1d4b0681d/nucleo-f030R8/system/src/retarget/retarget.c#L35-L47
int _write(int fd, char* ptr, int len) {
  HAL_StatusTypeDef hstatus;

  if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
    hstatus = HAL_UART_Transmit(debug_uart, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    if (hstatus == HAL_OK)
      return len;
    else
      return EIO;
  }
  errno = EBADF;
  return -1;
}

// Inspired by: https://github.com/cnoviello/mastering-stm32/blob/024664974feefb74dfa4f234e7a9a8b1d4b0681d/nucleo-f030R8/system/src/retarget/retarget.c#L66-L78
int _read(int fd, char* ptr, int len) {
  HAL_StatusTypeDef hstatus;

  if (fd == STDIN_FILENO) {
    hstatus = HAL_UART_Receive(debug_uart, (uint8_t *) ptr, 1, HAL_MAX_DELAY);
    if (hstatus == HAL_OK)
      return 1;
    else
      return EIO;
  }
  errno = EBADF;
  return -1;
}

#endif  // !defined(OS_USE_SEMIHOSTING)

#endif  // HAL_UART_MODULE_ENABLED