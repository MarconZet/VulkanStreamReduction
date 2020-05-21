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
    Shader(uint32_t elementSize, std::vector<uint32_t> scatterShader, std::vector<uint32_t> copyShader);

    Shader(uint32_t elementSize, const std::string& scatterName, const std::string& copyName);

    std::vector<uint32_t> getShader(const std::string& name);

    [[nodiscard]] uint32_t getElementSize() const;

    [[nodiscard]] const std::vector<uint32_t> &getScatterShader() const;

    [[nodiscard]] const std::vector<uint32_t> &getCopyShader() const;

private:
    uint32_t elementSize;
    std::vector<uint32_t> scatterShader;
    std::vector<uint32_t> copyShader;
};


#endif //VULKANSTREAMREDUCE_SHADER_H
