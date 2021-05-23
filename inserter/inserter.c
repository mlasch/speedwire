#include <inserter.h>
#include <speedwire.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char* generate_line_protocol(speedwire_data_t* data, const char* meas_name) {
    char * line_buffer = NULL;
    if (asprintf(&line_buffer, "%s ", meas_name) < 0) {
        return NULL;
    }

    for (obis_data_t * obis_ptr=data->obis_data_list;obis_ptr != NULL; obis_ptr = obis_ptr->next) {
        printf("LINE %s: %ld\n", obis_ptr->property_name, obis_ptr->counter);
        if (asprintf(&line_buffer, "%s%s=%ld", line_buffer, obis_ptr->property_name, obis_ptr->counter) < 0) {
            return NULL;
        }

        if (obis_ptr->next) {
            if (asprintf(&line_buffer, "%s,", line_buffer) < 0) {
                return NULL;
            }
        }
    }
    if (asprintf(&line_buffer, "%s %ld%09ld", line_buffer, data->timestamp.tv_sec,data->timestamp.tv_nsec) < 0) {
        return NULL;
    }

    return line_buffer;
}

_Noreturn void* influxdb_inserter(void* arg) {
    inserter_args_t* inserter_args = (inserter_args_t*) arg;
    printf("Start influxdb inserter\n");
    speedwire_batch_t * batch_temp = NULL;
    for(;;) {
        pthread_mutex_lock(&inserter_args->mtx);
        pthread_cond_wait(&inserter_args->cv, &inserter_args->mtx);
        batch_temp = inserter_args->batch_read_ptr;
        pthread_mutex_unlock(&inserter_args->mtx);
        // convert batch to line, do curl
        for (speedwire_batch_t* ptr = inserter_args->batch_read_ptr;ptr != NULL; ptr=ptr->next) {
            const char* line = generate_line_protocol(ptr->speedwire_data, "emeter");
            printf("%s\n", line);
            free((char*) line);
//            obis_data_t* obis_ptr = ptr->speedwire_data->obis_data_list;
//            while (obis_ptr != NULL) {
//
//                printf("%s: %ld\n", obis_ptr->property_name, obis_ptr->counter);
//
//                obis_ptr=obis_ptr->next;
//            }
        }

        speedwire_free_batch(batch_temp);
    }
    return NULL;
}