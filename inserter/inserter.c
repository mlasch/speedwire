#include <inserter.h>
#include <speedwire.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>


const char* generate_line_protocol(speedwire_data_t* data, const char* meas_name) {
    char * line_buffer = NULL;
    char * save_ptr = NULL;
    if (asprintf(&line_buffer, "%s ", meas_name) < 0) {
        return NULL;
    }

    for (obis_data_t * obis_ptr=data->obis_data_list;obis_ptr != NULL; obis_ptr = obis_ptr->next) {
//        printf("LINE %s: %ld\n", obis_ptr->property_name, obis_ptr->counter);
        if (asprintf(&save_ptr, "%s%s=%ld", line_buffer, obis_ptr->property_name, obis_ptr->counter) < 0) {
            return NULL;
        }
        free(line_buffer);
        line_buffer = save_ptr;

        if (obis_ptr->next) {
            if (asprintf(&save_ptr, "%s,", line_buffer) < 0) {
                return NULL;
            }
            free(line_buffer);
            line_buffer = save_ptr;
        }
    }
    if (asprintf(&save_ptr, "%s %ld%09ld", line_buffer, data->timestamp.tv_sec,data->timestamp.tv_nsec) < 0) {
        return NULL;
    }
    free(line_buffer);
    line_buffer = save_ptr;

    return line_buffer;
}

const char* generate_line_batch(speedwire_batch_t* batch_temp, const char* measurement) {
    char* batch_lines = malloc(1);
    *batch_lines = '\0';
    const char* new_line = NULL;

    for (speedwire_batch_t* ptr = batch_temp; ptr != NULL; ptr = ptr->next) {
        new_line = generate_line_protocol(ptr->speedwire_data, measurement);
        if (new_line == NULL)
            return NULL;
        size_t s1 = strlen(batch_lines);
        size_t s2 = strlen(new_line);
        batch_lines = realloc(batch_lines, s1 + s2 + 1);
        strcat(batch_lines, new_line);
        if (ptr->next != NULL) {
            batch_lines = realloc(batch_lines, s1 + s2 + 2);
            strcat(batch_lines, "\n");
        }

        free((char*)new_line);
        new_line = NULL;
    }
    return batch_lines;
}

void influxdb_post_request(const char* url, const char*batch_lines, CURL* curl_handle) {
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, batch_lines);

    CURLcode curl_code = curl_easy_perform(curl_handle);

    if (curl_code) {
        fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(curl_code));
    } else {
        printf("inserted\n");
    }
}

_Noreturn void* influxdb_inserter(void* arg) {
    inserter_args_t* inserter_args = (inserter_args_t*) arg;
    printf("Start influxdb inserter\n");
    speedwire_batch_t * batch_temp = NULL;
    for(;;) {
        pthread_mutex_lock(&inserter_args->mtx);
        pthread_cond_wait(&inserter_args->cv, &inserter_args->mtx);
        batch_temp = inserter_args->batch_read_ptr;
        inserter_args->batch_read_ptr = NULL;
        pthread_mutex_unlock(&inserter_args->mtx);
        // convert batch to batch_lines, do curl
        const char* batch_lines;
        batch_lines = generate_line_batch(batch_temp, inserter_args->measurement);

        printf("%s\n", batch_lines);
        influxdb_post_request(inserter_args->url, (const char*)batch_lines, inserter_args->curl_handle);
        free((char*)batch_lines);
        speedwire_free_batch(batch_temp);
    }
    return NULL;
}
