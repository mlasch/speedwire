#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct channel {
    uint8_t index;
    const char *name;
};

struct channel channel_list[35] = {{1, "sum_effective_power_plus"},
                                   {2, "sum_effective_power_minus"},
                                   {3, "sum_reactive_power_plus"},
                                   {4, "sum_reactive_power_minus"},
                                   {9, "sum_apparent_power_plus"},
                                   {10, "sum_apparent_power_minus"},
                                   {13, "sum_power_factor"},
                                   {21, "l1_effective_power_plus"},
                                   {22, "l1_effective_power_minus"},
                                   {23, "l1_reactive_power_plus"},
                                   {24, "l1_reactive_power_minus"},
                                   {29, "l1_apparent_power_plus"},
                                   {30, "l1_apparent_power_minus"},
                                   {31, "l1_current"},
                                   {32, "l1_voltage"},
                                   {33, "l1_unknown"},
                                   {41, "l2_effective_power_plus"},
                                   {42, "l2_effective_power_minus"},
                                   {43, "l2_reactive_power_plus"},
                                   {44, "l2_reactive_power_minus"},
                                   {49, "l2_apparent_power_plus"},
                                   {50, "l2_apparent_power_minus"},
                                   {51, "l2_current"},
                                   {52, "l2_voltage"},
                                   {53, "l2_unknown"},
                                   {61, "l3_effective_power_plus"},
                                   {62, "l3_effective_power_minus"},
                                   {63, "l3_reactive_power_plus"},
                                   {64, "l3_reactive_power_minus"},
                                   {69, "l3_apparent_power_plus"},
                                   {70, "l3_apparent_power_minus"},
                                   {71, "l3_current"},
                                   {72, "l3_voltage"},
                                   {73, "l3_unknown"}};

const char *lookup_channel_name(uint8_t index) {
    for (uint8_t i = 0; i < 35; i++) {
        if (channel_list[i].index == index) {
            return channel_list[i].name;
        }
    }
    printf("channel name not found %d\n", index);
    return NULL;
}
