//
// Created by Marcin on 19.05.2020.
//
#include <cstdint>
#include <vector>
#include <ctime>

#include <VulkanContext.h>
#include <Filter.h>
#include <Shader.h>


int main() {
    srand(time(nullptr));
    VulkanContext context;

    uint32_t additionalDataSize = 2 * 3 * 10;
    auto *additionalData = new uint32_t[additionalDataSize];
    auto *p = reinterpret_cast<float *>(additionalData);
    for (int i = 0; i < additionalDataSize; i++) {
        p[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);;
    }

    Shader shader(2, "comp.spv", 1, additionalData, additionalDataSize);
    int elementNumber = 1024;
    Filter filter(elementNumber, shader, context);

    int inputDataSize = elementNumber * 2;
    auto inputData = new uint32_t[inputDataSize];
    p = reinterpret_cast<float *>(inputData);
    for (int i = 0; i < inputDataSize; i++) {
        p[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

    auto output = filter.execute(inputData);

    int * interp = static_cast<int *>(output);
    for(int i = 0; i< elementNumber; i++){
        if(p[i*2] < p[i*2+1] != interp[i]){
            printf("Difference at %d\n", i);
            return 0;
        }
    }
    printf("All ok\n");
            
            
}
