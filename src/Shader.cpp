//
// Created by Marcin on 21.05.2020.
//

#include <utility>
#include <string>
#include <fstream>

#include "Shader.h"


std::vector<uint32_t> Shader::getShader(const std::string& name){
    std::ifstream shaderFile(name, std::ios::binary | std::ios::ate);
    std::streamsize shaderSize = shaderFile.tellg();
    shaderFile.seekg(0, std::ios::beg);

    std::vector<uint32_t> shader(shaderSize / 4);
    if (!shaderFile.read(reinterpret_cast<char *>(shader.data()), shaderSize)) {
        throw std::runtime_error("Failed to load shader");
    }

    return shader;
}

Shader::Shader(uint32_t elementSize, const std::string &scatterName, uint32_t outputElementSize,
               void *additionalData,
               uint32_t additionalDataSize)
: inputElementSize(elementSize), additionalData(additionalData), outputElementSize(outputElementSize), additionalDataSize(additionalDataSize) {
    mapShader = getShader(scatterName);
}

uint32_t Shader::getInputElementSize() const {
    return inputElementSize;
}

uint32_t Shader::getOutputElementSize() const {
    return outputElementSize;
}

const std::vector<uint32_t> &Shader::getMapShader() const {
    return mapShader;
}

void *Shader::getAdditionalData() const {
    return additionalData;
}

uint32_t Shader::getAdditionalDataSize() const {
    return additionalDataSize;
}
