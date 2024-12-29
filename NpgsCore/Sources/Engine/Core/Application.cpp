#include "Application.h"

#include <cstdint>

#include "Engine/Core/Vulkan/VulkanBase.h"
#include "Engine/Utilities/Logger.h"

_NPGS_BEGIN

FApplication::FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle,
						   bool bEnableVSync, bool bEnableFullscreen)
	:
	_WindowTitle(WindowTitle),
	_WindowSize(WindowSize),
	_Window(nullptr),
	_bEnableVSync(bEnableVSync),
	_bEnableFullscreen(bEnableFullscreen)
{
	if (!InitWindow())
	{
		NpgsCoreError("Error: Failed to create window.");
	}
}

FApplication::~FApplication()
{
	Terminate();
}

void FApplication::ExecuteMainRender()
{
	CreateScreenRender();
	const auto& [Framebuffers, RenderPass] = _Renderer;
	CreateLayout();
	CreatePipeline();

	vk::FenceCreateFlags FenceCreateFlags{ vk::FenceCreateFlagBits::eSignaled };
	FVulkanBase* VulkanBase = FVulkanBase::GetVulkanBaseInstance();
	FVulkanFence VulkanFence(VulkanBase->GetDevice(), FenceCreateFlags);
	FVulkanSemaphore VulkanSemaphore_ImageAvailable(VulkanBase->GetDevice());
	FVulkanSemaphore VulkanSemaphore_RenderFinished(VulkanBase->GetDevice());

	FVulkanCommandBuffer CommandBuffer;
	FVulkanCommandPool CommandPool(VulkanBase->GetDevice(), VulkanBase->GetGraphicsQueueFamilyIndex(),
								   vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	CommandPool.AllocateBuffer(vk::CommandBufferLevel::ePrimary, CommandBuffer);

	vk::ClearValue ColorValue({ 0.0f, 0.0f, 0.0f, 1.0f });

	while (!glfwWindowShouldClose(_Window))
	{
		while (glfwGetWindowAttrib(_Window, GLFW_ICONIFIED))
		{
			glfwWaitEvents();
		}

		VulkanFence.WaitAndReset();

		VulkanBase->SwapImage(VulkanSemaphore_ImageAvailable);
		std::uint32_t ImageIndex = VulkanBase->GetCurrentImageIndex();

		CommandBuffer.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		RenderPass.CommandBegin(CommandBuffer, Framebuffers[ImageIndex], { {}, _WindowSize }, { ColorValue });
		CommandBuffer.GetCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, _Pipeline.GetPipeline());
		CommandBuffer.GetCommandBuffer().draw(3, 1, 0, 0);
		RenderPass.CommandEnd(CommandBuffer);
		CommandBuffer.End();

		VulkanBase->SubmitCommandBufferToGraphics(CommandBuffer, VulkanSemaphore_ImageAvailable,
												  VulkanSemaphore_RenderFinished, VulkanFence);
		VulkanBase->PresentImage(VulkanSemaphore_RenderFinished);

		ProcessInput();
		glfwPollEvents();
		ShowTitleFps();
	}

	Terminate();
}

void FApplication::Terminate()
{
	FVulkanBase::GetVulkanBaseInstance()->WaitIdle();
	glfwTerminate();
}

bool FApplication::InitWindow()
{
	if (glfwInit() == GLFW_FALSE)
	{
		NpgsCoreError("Error: Failed to initialize GLFW.");
		return false;
	};

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	_Window = glfwCreateWindow(_WindowSize.width, _WindowSize.height, _WindowTitle.c_str(), nullptr, nullptr);
	if (_Window == nullptr)
	{
		NpgsCoreError("Error: Failed to create GLFW window.");
		glfwTerminate();
		return false;
	}

	std::uint32_t ExtensionCount = 0;
	const char** Extensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);
	if (Extensions == nullptr)
	{
		NpgsCoreError("Error: Failed to get required instance extensions.");
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}

	FVulkanBase* VulkanBase = FVulkanBase::GetVulkanBaseInstance();
	for (std::uint32_t i = 0; i != ExtensionCount; ++i)
	{
		VulkanBase->AddInstanceExtension(Extensions[i]);
	}

	VulkanBase->AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	vk::Result Result;
	if ((Result = VulkanBase->CreateInstance()) != vk::Result::eSuccess)
	{
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}

	vk::SurfaceKHR Surface;
	if (glfwCreateWindowSurface(VulkanBase->GetInstance(), _Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&Surface)) != VK_SUCCESS)
	{
		NpgsCoreError("Error: Failed to create window surface.");
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}
	VulkanBase->SetSurface(Surface);

	if (VulkanBase->CreateDevice(0) != vk::Result::eSuccess ||
		VulkanBase->CreateSwapchain(_WindowSize) != vk::Result::eSuccess)
	{
		return false;
	}

	return true;
}

void FApplication::CreateScreenRender()
{
	static FRenderer Renderer;
	FVulkanBase* VulkanBase = FVulkanBase::GetVulkanBaseInstance();

	vk::AttachmentDescription AttachmentDescription = vk::AttachmentDescription()
		.setFormat(VulkanBase->GetSwapchainCreateInfo().imageFormat)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription SubpassDescription = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&AttachmentReference);

	vk::SubpassDependency SubpassDependency = vk::SubpassDependency()
		.setSrcSubpass(vk::SubpassExternal)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setSrcAccessMask(vk::AccessFlagBits::eNone)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	vk::RenderPassCreateInfo RenderPassCreateInfo({}, 1, &AttachmentDescription, 1, &SubpassDescription, 1, &SubpassDependency);
	Renderer.RenderPass = FVulkanRenderPass(VulkanBase->GetDevice(), RenderPassCreateInfo);

	auto CreateFramebuffers = [&]() -> void
	{
		vk::FramebufferCreateInfo FramebufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(Renderer.RenderPass.GetRenderPass())
			.setAttachmentCount(1)
			.setWidth(_WindowSize.width)
			.setHeight(_WindowSize.height)
			.setLayers(1);

		for (std::uint32_t i = 0; i != VulkanBase->GetSwapchainImageCount(); ++i)
		{
			vk::ImageView Attachment = VulkanBase->GetSwapchainImageView(i);
			FramebufferCreateInfo.setPAttachments(&Attachment);
			Renderer.Framebuffers.emplace_back(VulkanBase->GetDevice(), FramebufferCreateInfo);
		}
	};

	auto DestroyFramebuffers = []() -> void
	{
		Renderer.Framebuffers.clear();
	};

	CreateFramebuffers();

	static bool bCallbackAdded = false;
	if (!bCallbackAdded)
	{
		VulkanBase->AddCreateSwapchainCallback(CreateFramebuffers);
		VulkanBase->AddDestroySwapchainCallback(DestroyFramebuffers);
		bCallbackAdded = true;
	}

	_Renderer = Renderer;
}

void FApplication::CreatePipeline()
{
	FVulkanBase* VulkanBase = FVulkanBase::GetVulkanBaseInstance();
	static FVulkanShaderModule VertShader(VulkanBase->GetDevice(), "Sources/Shaders/Triangle.vert.spv");
	static FVulkanShaderModule FragShader(VulkanBase->GetDevice(), "Sources/Shaders/Triangle.frag.spv");
	static vk::PipelineShaderStageCreateInfo VertShaderStage = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(VertShader.GetShaderModule())
		.setPName("main");
	static vk::PipelineShaderStageCreateInfo FragShaderStage = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(FragShader.GetShaderModule())
		.setPName("main");

	static vk::PipelineShaderStageCreateInfo shaderStageCreateInfos_triangle[2] = { VertShaderStage, FragShaderStage };

	auto Create = [&]() -> void
	{
		GraphicsPipelineCreateInfoPack Pack;
		Pack.createInfo.layout = _Layout.GetPipelineLayout();
		Pack.createInfo.renderPass = _Renderer.RenderPass.GetRenderPass();
		Pack.inputAssemblyStateCi.topology = vk::PrimitiveTopology::eTriangleList;
		Pack.viewports.emplace_back(0.f, 0.f, float(_WindowSize.width), float(_WindowSize.height), 0.f, 1.f);
		Pack.scissors.emplace_back(vk::Offset2D{}, _WindowSize);
		Pack.multisampleStateCi.rasterizationSamples = vk::SampleCountFlagBits::e1;
		vk::PipelineColorBlendAttachmentState ColorBlendAttachmentState = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		Pack.colorBlendAttachmentStates.push_back(ColorBlendAttachmentState);
		Pack.UpdateAllArrays();
		Pack.createInfo.stageCount = 2;
		Pack.createInfo.pStages = shaderStageCreateInfos_triangle;
		_Pipeline = FVulkanPipeline(VulkanBase->GetDevice(), Pack);
	};
	
	auto Destroy = [this]() -> void
	{
		_Pipeline.~FVulkanPipeline();
	};

	VulkanBase->AddCreateSwapchainCallback(Create);
	VulkanBase->AddDestroySwapchainCallback(Destroy);
	Create();
}

void FApplication::CreateLayout()
{
	FVulkanBase* VulkanBase = FVulkanBase::GetVulkanBaseInstance();
	vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo;
	_Layout = FVulkanPipelineLayout(VulkanBase->GetDevice(), PipelineLayoutCreateInfo);
}

void FApplication::ShowTitleFps()
{
	static double CurrentTime   = 0.0;
	static double PreviousTime  = glfwGetTime();
	static double LastFrameTime = 0.0;
	static double DeltaTime     = 0.0;
	static int    FrameCount    = 0;

	CurrentTime   = glfwGetTime();
	DeltaTime     = CurrentTime - LastFrameTime;
	LastFrameTime = CurrentTime;
	++FrameCount;
	if (CurrentTime - PreviousTime >= 1.0)
	{
		glfwSetWindowTitle(_Window, (std::string(_WindowTitle) + " " + std::to_string(FrameCount)).c_str());
		FrameCount   = 0;
		PreviousTime = CurrentTime;
	}
}

void FApplication::ProcessInput()
{
	if (glfwGetKey(_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(_Window, GL_TRUE);
	}
}

_NPGS_END
