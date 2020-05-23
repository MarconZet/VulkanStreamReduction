//
// Created by Marcin on 22.05.2020.
//

#ifndef VULKANSTREAMREDUCE_SCATTERPIPELINE_H
#define VULKANSTREAMREDUCE_SCATTERPIPELINE_H


#include "Pipeline.h"

class ScatterPipeline : Pipeline {
    VkDescriptorSetLayout createDescriptorSetLayout();
};


#endif //VULKANSTREAMREDUCE_SCATTERPIPELINE_H
