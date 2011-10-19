#ifndef LOGPRINTF_H
#define LOGPRINTF_H

typedef void (*logprintf_t)(const char *format, ...);

extern logprintf_t logprintf;

#endif