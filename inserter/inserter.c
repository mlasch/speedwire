#include <inserter.h>
#include <speedwire.h>
#include <stdlib.h>
#include <string.h>


const char* generate_line_protocol(speedwire_data_t* data, const char* meas_name) {
    char * line_buffer;
    line_buffer = malloc(strlen(meas_name));
    strcpy(line_buffer, meas_name);
    strcat(line_buffer, " ");

    char elem_buffer[100];
    for (obis_data_t * obis_ptr=data->obis_data_list;obis_ptr != NULL; obis_ptr = obis_ptr->next) {
        printf("LINE %s: %ld\n", obis_ptr->property_name, obis_ptr->counter);

        snprintf(elem_buffer, 100, "%s=%ld", obis_ptr->property_name, obis_ptr->counter);
        line_buffer = realloc(line_buffer, strlen(line_buffer) + strlen(elem_buffer));
        strcat(line_buffer, elem_buffer);
        if (obis_ptr->next) {
            strcat(line_buffer, ",");
        }
    }
    snprintf(elem_buffer, 100, " %ld%09ld", data->timestamp.tv_sec,data->timestamp.tv_nsec);
    line_buffer = realloc(line_buffer, strlen(line_buffer) + strlen(elem_buffer));
    strcat(line_buffer, elem_buffer);

    return line_buffer;
}

_Noreturn void* influxdb_inserter(void* arg) {
    inserter_args_t* inserter_args = (inserter_args_t*) arg;
    printf("Start influxdb inserter");
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