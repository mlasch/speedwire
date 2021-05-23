#include <speedwire.h>
#include <pthread.h>
#include <curl/curl.h>

typedef struct {
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    speedwire_batch_t* batch_read_ptr;
    CURL *curl_handle;
} inserter_args_t;

const char* generate_line_protocol(speedwire_data_t* data, const char* meas_name);
void influxdb_post_request(const char* url, const char*batch_lines, CURL* curl_handle);
_Noreturn void* influxdb_inserter(void * arg);