//
// Created by Marcin on 21.05.2020.
//

#include <utility>
#include <string>
#include <fstream>

#include "Shader.h"

Shader::Shader(uint32_t elementSize, std::vector<uint32_t> scatterShader,
               std::vector<uint32_t> copyShader) : elementSize(elementSize), scatterShader(std::move(scatterShader)),
                                                          copyShader(std::move(copyShader)) {}

uint32_t Shader::getElementSize() const {
    return elementSize;
}

const std::vector<uint32_t> &Shader::getScatterShader() const {
    return scatterShader;
}

const std::vector<uint32_t> &Shader::getCopyShader() const {
    return copyShader;
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

Shader::Shader(uint32_t elementSize, const std::string& scatterName, const std::string& copyName) : elementSize(elementSize){
    scatterShader = getShader(scatterName);
    copyShader = getShader(copyName);

}
