//
// Created by Marcin on 19.05.2020.
//
#include <stdint.h>

#include "src/VulkanContext.h"
#include "src/Filter.h"


int main(){
    VulkanContext context;
    uint32_t size = 4;
    uint32_t length = 1024 * 16;
    Filter filter(size, length, context);
    filter.execute();
}
