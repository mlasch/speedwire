#include <speedwire.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    speedwire_batch_t* batch_read_ptr;
} inserter_args_t;

const char* generate_line_protocol(speedwire_data_t* data, const char* meas_name);
_Noreturn void* influxdb_inserter(void * arg);