//
// Created by Marcin on 18.05.2020.
//

#ifndef VULKANSTREAMREDUCE_COMMON_H
#define VULKANSTREAMREDUCE_COMMON_H

#include <string>
#include <stdexcept>

#define THROW_ON_FAIL(result) if (VK_SUCCESS != (result)) { throw std::runtime_error("Vulkan error"); }

#endif //VULKANSTREAMREDUCE_COMMON_H
