#include "logify.h"

#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

const char *log_status_to_string(int code) {
  switch (code) {
  case 4:
    return "DEBUG";
  case 5:
    return "INFO";
  case 10:
    return "WARNING";
  case 15:
    return "ERROR";
  default:
    return "Unknown Status";
  }
}

void logify_log(enum LogStatus status, const char *fmt, ...) {
  const char *level = log_status_to_string(status);

  struct timeval tv;
  struct tm *tm_info;
  char timestamp[64];
  char message_buf[2048];
  va_list args;

  gettimeofday(&tv, NULL);
  tm_info = localtime(&tv.tv_sec);
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

  va_start(args, fmt);
  vsprintf(message_buf, fmt, args);
  va_end(args);

  printf("[%s.%03ld] [%s] %s\n", timestamp, tv.tv_usec / 1000, level,
         message_buf);
}
