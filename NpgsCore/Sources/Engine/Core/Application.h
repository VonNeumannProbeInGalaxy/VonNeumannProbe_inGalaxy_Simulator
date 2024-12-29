#pragma once

#include <string>
#include <vector>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Engine/Core/Vulkan/VulkanWrappers.h"
#include "Engine/Core/Base.h"

_NPGS_BEGIN

class FApplication
{
private:
	struct FRenderer
	{
		std::vector<FVulkanFramebuffer> Framebuffers;
		FVulkanRenderPass RenderPass;
	};

public:
	FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle, bool bEnableVSync, bool bEnableFullscreen);
	~FApplication();

	void ExecuteMainRender();
	void Terminate();

private:
	bool InitWindow();
	void CreateScreenRender();
	void CreatePipeline();
	void CreateLayout();
	void ShowTitleFps();
	void ProcessInput();

private:
	std::string           _WindowTitle;
	vk::Extent2D          _WindowSize;
	FVulkanPipeline       _Pipeline;
	FVulkanPipelineLayout _Layout;
    FRenderer             _Renderer;
	GLFWwindow*           _Window;
	bool                  _bEnableVSync;
	bool                  _bEnableFullscreen;
};

_NPGS_END

struct GraphicsPipelineCreateInfoPack {
    vk::GraphicsPipelineCreateInfo createInfo;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    // Vertex Input
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCi;
    std::vector<vk::VertexInputBindingDescription> vertexInputBindings;
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;
    // Input Assembly
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCi;
    // Tessellation
    vk::PipelineTessellationStateCreateInfo tessellationStateCi;
    // Viewport
    vk::PipelineViewportStateCreateInfo viewportStateCi;
    std::vector<vk::Viewport> viewports;
    std::vector<vk::Rect2D> scissors;
    uint32_t dynamicViewportCount = 1;
    uint32_t dynamicScissorCount = 1;
    // Rasterization
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCi;
    // Multisample
    vk::PipelineMultisampleStateCreateInfo multisampleStateCi;
    // Depth & Stencil
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCi;
    // Color Blend
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCi;
    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates;
    // Dynamic
    vk::PipelineDynamicStateCreateInfo dynamicStateCi;
    std::vector<vk::DynamicState> dynamicStates;

    GraphicsPipelineCreateInfoPack() {
        SetCreateInfos();
        createInfo.setBasePipelineIndex(-1);
    }

    GraphicsPipelineCreateInfoPack(const GraphicsPipelineCreateInfoPack& other) noexcept {
        createInfo = other.createInfo;
        SetCreateInfos();

        vertexInputStateCi = other.vertexInputStateCi;
        inputAssemblyStateCi = other.inputAssemblyStateCi;
        tessellationStateCi = other.tessellationStateCi;
        viewportStateCi = other.viewportStateCi;
        rasterizationStateCi = other.rasterizationStateCi;
        multisampleStateCi = other.multisampleStateCi;
        depthStencilStateCi = other.depthStencilStateCi;
        colorBlendStateCi = other.colorBlendStateCi;
        dynamicStateCi = other.dynamicStateCi;

        shaderStages = other.shaderStages;
        vertexInputBindings = other.vertexInputBindings;
        vertexInputAttributes = other.vertexInputAttributes;
        viewports = other.viewports;
        scissors = other.scissors;
        colorBlendAttachmentStates = other.colorBlendAttachmentStates;
        dynamicStates = other.dynamicStates;
        UpdateAllArrayAddresses();
    }

    operator vk::GraphicsPipelineCreateInfo& () { return createInfo; }

    void UpdateAllArrays() {
        createInfo.setStageCount(static_cast<uint32_t>(shaderStages.size()));
        vertexInputStateCi.setVertexBindingDescriptionCount(static_cast<uint32_t>(vertexInputBindings.size()));
        vertexInputStateCi.setVertexAttributeDescriptionCount(static_cast<uint32_t>(vertexInputAttributes.size()));
        viewportStateCi.setViewportCount(viewports.size() ? static_cast<uint32_t>(viewports.size()) : dynamicViewportCount);
        viewportStateCi.setScissorCount(scissors.size() ? static_cast<uint32_t>(scissors.size()) : dynamicScissorCount);
        colorBlendStateCi.setAttachmentCount(static_cast<uint32_t>(colorBlendAttachmentStates.size()));
        dynamicStateCi.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        UpdateAllArrayAddresses();
    }

private:
    void SetCreateInfos() {
        createInfo.setPVertexInputState(&vertexInputStateCi)
            .setPInputAssemblyState(&inputAssemblyStateCi)
            .setPTessellationState(&tessellationStateCi)
            .setPViewportState(&viewportStateCi)
            .setPRasterizationState(&rasterizationStateCi)
            .setPMultisampleState(&multisampleStateCi)
            .setPDepthStencilState(&depthStencilStateCi)
            .setPColorBlendState(&colorBlendStateCi)
            .setPDynamicState(&dynamicStateCi);
    }

    void UpdateAllArrayAddresses() {
        createInfo.setPStages(shaderStages.data());
        vertexInputStateCi.setPVertexBindingDescriptions(vertexInputBindings.data());
        vertexInputStateCi.setPVertexAttributeDescriptions(vertexInputAttributes.data());
        viewportStateCi.setPViewports(viewports.data());
        viewportStateCi.setPScissors(scissors.data());
        colorBlendStateCi.setPAttachments(colorBlendAttachmentStates.data());
        dynamicStateCi.setPDynamicStates(dynamicStates.data());
    }
};
