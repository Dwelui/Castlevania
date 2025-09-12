#ifndef LOGIFY_H
#define LOGIFY_H

enum LogStatus {
    DEBUG = 4,
    INFO = 5,
    WARNING = 10,
    ERROR = 15
};

void logify_log(enum LogStatus status, const char *fmt, ...);

#endif
