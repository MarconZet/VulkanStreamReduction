//
// Created by Marcin on 21.05.2020.
//

#ifndef VULKANSTREAMREDUCE_SHADER_H
#define VULKANSTREAMREDUCE_SHADER_H

#include <vector>
#include <string>

#include "vulkan/vulkan.h"

class Shader {
public:

    Shader(uint32_t elementSize, const std::string &scatterName, uint32_t outputElementSize, uint32_t *additionalData,
           uint32_t additionalDataSize);

    std::vector<uint32_t> getShader(const std::string& name);

    [[nodiscard]] uint32_t getInputElementSize() const;

    [[nodiscard]] uint32_t getOutputElementSize() const;

    [[nodiscard]] const std::vector<uint32_t> &getMapShader() const;

    uint32_t *getAdditionalData() const;

    uint32_t getAdditionalDataSize() const;


private:
    uint32_t inputElementSize;
    uint32_t outputElementSize;
    std::vector<uint32_t> mapShader;
    uint32_t * additionalData;
    uint32_t  additionalDataSize;
};


#endif //VULKANSTREAMREDUCE_SHADER_H
