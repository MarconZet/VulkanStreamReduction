//
// Created by Marcin on 21.05.2020.
//

#include <utility>
#include <string>
#include <fstream>

#include "Shader.h"

Shader::Shader(uint32_t elementSize, std::vector<uint32_t> scatterShader, std::vector<uint8_t> data)
               : elementSize(elementSize), scatterShader(std::move(scatterShader)), additionalData(std::move(data)) {}

uint32_t Shader::getElementSize() const {
    return elementSize;
}

const std::vector<uint32_t> &Shader::getScatterShader() const {
    return scatterShader;
}

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

Shader::Shader(uint32_t elementSize, const std::string& scatterName, std::vector<uint8_t> data)
: elementSize(elementSize), additionalData(std::move(data)){
    scatterShader = getShader(scatterName);
    if(elementSize % 4 != 0){
        throw std::invalid_argument("Element size must be a multiplicative of 4\n");
    }
}

void Shader::setData(const std::vector<uint8_t> &data) {
    Shader::additionalData = data;
}

const std::vector<uint8_t> &Shader::getAdditionalData() const {
    return additionalData;
}
