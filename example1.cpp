//
// Created by Marcin on 25.05.2020.
//

#include <cstdint>
#include <vector>
#include <ctime>

#include <VulkanContext.h>
#include <Filter.h>
#include <Shader.h>

int test1();

int test2();

int main() {
    test1();
    test2();
}

int test1() {
    srand(time(nullptr));
    VulkanContext context;

    uint32_t additionalDataSize = 1;
    int *additionalData = new int[additionalDataSize];
    additionalData[0] = 10;

    Shader shader(1, "example1.spv", 1, additionalData, additionalDataSize);

    int elementNumber = 1024;
    Filter filter(elementNumber, shader, context);

    int inputDataSize = elementNumber;
    int *inputData = new int[inputDataSize];
    for (int i = 0; i < inputDataSize; i++) {
        inputData[i] = rand() % 1000;
    }

    void *output = filter.execute(inputData);

    int *interp = static_cast<int *>(output);
    for (int i = 0; i < elementNumber; i++) {
        if (inputData[i] + additionalData[0] != interp[i]) {
            printf("Difference at %d\n", i);
            return 0;
        }
    }
    printf("All ok\n");


}

int test2() {
    srand(time(nullptr));
    VulkanContext context;

    uint32_t additionalDataSize = 2 * 3 * 10;
    void *additionalData = new float[additionalDataSize];

    auto *p = reinterpret_cast<float *>(additionalData);
    for (int i = 0; i < additionalDataSize; i++) {
        p[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);;
    }

    Shader shader(2, "example2.spv", 1, additionalData, additionalDataSize);
    int elementNumber = 1024;
    Filter filter(elementNumber, shader, context);

    int inputDataSize = elementNumber * 2;
    void *inputData = new float[inputDataSize];
    p = reinterpret_cast<float *>(inputData);
    for (int i = 0; i < inputDataSize; i++) {
        p[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

    void *output = filter.execute(inputData);

    int *interp = static_cast<int *>(output);
    for (int i = 0; i < elementNumber; i++) {
        if (p[i * 2] < p[i * 2 + 1] != interp[i]) {
            printf("Difference at %d\n", i);
            return 0;
        }
    }
    printf("All ok\n");


}
