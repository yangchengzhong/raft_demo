#ifndef __LOG_H__
#define __LOG_H__

void log_with_time(int level, const char *fmt, ...);

#define LOGD(format, ...) log_with_time(0, "[DEBUG][%s:%d] " format"\n", __FILE__, __LINE__, ##__VA_ARGS__); 

#define LOGI(format, ...) log_with_time(1, "[INFO][%s:%d] " format"\n", __FILE__, __LINE__, ##__VA_ARGS__); 

#define LOGE(format, ...) log_with_time(2, "[ERROR][%s:%d] " format"\n", __FILE__, __LINE__, ##__VA_ARGS__); 
    

#endif

