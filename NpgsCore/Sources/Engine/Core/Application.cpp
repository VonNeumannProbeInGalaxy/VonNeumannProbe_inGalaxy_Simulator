#include "Application.h"

#include <cstdint>
#include <utility>
#include "Engine/Utilities/Logger.h"

_NPGS_BEGIN

FApplication::FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle,
						   bool bEnableVSync, bool bEnableFullscreen)
	:
	_WindowTitle(WindowTitle),
	_WindowSize(WindowSize),
	_Window(nullptr),
	_VulkanBase(FVulkanBase::GetVulkanBaseInstance()),
	_bEnableVSync(bEnableVSync),
	_bEnableFullscreen(bEnableFullscreen)
{
	if (!InitWindow())
	{
		NpgsCoreError("Failed to create window.");
	}
}

FApplication::~FApplication()
{
	_VulkanBase->RemoveDestroySwapchainCallback("DestroyFramebuffers");
	_VulkanBase->RemoveDestroySwapchainCallback("DestroyPipeline");
}

void FApplication::ExecuteMainRender()
{
	CreateScreenRender();
	CreateLayout();
	CreatePipeline();

	const auto& [Framebuffers, RenderPass] = _Renderer;

	vk::FenceCreateFlags FenceCreateFlags{ vk::FenceCreateFlagBits::eSignaled };
	FVulkanFence Fence(_VulkanBase->GetDevice(), FenceCreateFlags);
	FVulkanSemaphore Semaphore_ImageAvailable(_VulkanBase->GetDevice());
	FVulkanSemaphore Semaphore_RenderFinished(_VulkanBase->GetDevice());

	FVulkanCommandBuffer CommandBuffer;
	FVulkanCommandPool CommandPool(_VulkanBase->GetDevice(), _VulkanBase->GetGraphicsQueueFamilyIndex(),
								   vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	CommandPool.AllocateBuffer(vk::CommandBufferLevel::ePrimary, CommandBuffer);

	vk::ClearValue ColorValue({ 0.0f, 0.0f, 0.0f, 1.0f });

	while (!glfwWindowShouldClose(_Window))
	{
		while (glfwGetWindowAttrib(_Window, GLFW_ICONIFIED))
		{
			glfwWaitEvents();
		}

		Fence.WaitAndReset();

		_VulkanBase->SwapImage(Semaphore_ImageAvailable);
		std::uint32_t ImageIndex = _VulkanBase->GetCurrentImageIndex();

		CommandBuffer.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		RenderPass.CommandBegin(CommandBuffer, Framebuffers[ImageIndex], { {}, _WindowSize }, { ColorValue });
		CommandBuffer->bindPipeline(vk::PipelineBindPoint::eGraphics, *_Pipeline);
		CommandBuffer->draw(3, 1, 0, 0);
		RenderPass.CommandEnd(CommandBuffer);
		CommandBuffer.End();

		_VulkanBase->SubmitCommandBufferToGraphics(CommandBuffer, Semaphore_ImageAvailable, Semaphore_RenderFinished, Fence);
		_VulkanBase->PresentImage(Semaphore_RenderFinished);

		ProcessInput();
		glfwPollEvents();
		ShowTitleFps();
	}

	Terminate();
}

void FApplication::Terminate()
{
	_VulkanBase->WaitIdle();
	glfwDestroyWindow(_Window);
	glfwTerminate();
}

bool FApplication::InitWindow()
{
	if (glfwInit() == GLFW_FALSE)
	{
		NpgsCoreError("Failed to initialize GLFW.");
		return false;
	};

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);

	_Window = glfwCreateWindow(_WindowSize.width, _WindowSize.height, _WindowTitle.c_str(), nullptr, nullptr);
	if (_Window == nullptr)
	{
		NpgsCoreError("Failed to create GLFW window.");
		glfwTerminate();
		return false;
	}

	glfwSetWindowUserPointer(_Window, this);
	glfwSetFramebufferSizeCallback(_Window, &FApplication::FramebufferSizeCallback);

	std::uint32_t ExtensionCount = 0;
	const char** Extensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);
	if (Extensions == nullptr)
	{
		NpgsCoreError("Failed to get required instance extensions.");
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}

	for (std::uint32_t i = 0; i != ExtensionCount; ++i)
	{
		_VulkanBase->AddInstanceExtension(Extensions[i]);
	}

	_VulkanBase->AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	vk::Result Result;
	if ((Result = _VulkanBase->CreateInstance()) != vk::Result::eSuccess)
	{
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}

	vk::SurfaceKHR Surface;
	if (glfwCreateWindowSurface(_VulkanBase->GetInstance(), _Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&Surface)) != VK_SUCCESS)
	{
		NpgsCoreError("Failed to create window surface.");
		glfwDestroyWindow(_Window);
		glfwTerminate();
		return false;
	}
	_VulkanBase->SetSurface(Surface);

	if (_VulkanBase->CreateDevice(0) != vk::Result::eSuccess ||
		_VulkanBase->CreateSwapchain(_WindowSize) != vk::Result::eSuccess)
	{
		return false;
	}

	return true;
}

void FApplication::CreateScreenRender()
{
	_Renderer = FRenderer();

	vk::AttachmentDescription AttachmentDescription = vk::AttachmentDescription()
		.setFormat(_VulkanBase->GetSwapchainCreateInfo().imageFormat)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription SubpassDescription = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachments(AttachmentReference);

	vk::SubpassDependency SubpassDependency = vk::SubpassDependency()
		.setSrcSubpass(vk::SubpassExternal)
		.setDstSubpass(0)
		.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setSrcAccessMask(vk::AccessFlagBits::eNone)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
		.setDependencyFlags(vk::DependencyFlagBits::eByRegion);

	vk::RenderPassCreateInfo RenderPassCreateInfo({}, AttachmentDescription, SubpassDescription, SubpassDependency);
	_Renderer.RenderPass = FVulkanRenderPass(_VulkanBase->GetDevice(), RenderPassCreateInfo);

	auto CreateFramebuffers =
	[VulkanBase = _VulkanBase, Renderer = &_Renderer, WindowSize = &_WindowSize]() -> void
	{
		VulkanBase->WaitIdle();
		Renderer->Framebuffers.clear();

		Renderer->Framebuffers.reserve(VulkanBase->GetSwapchainImageCount());

		vk::FramebufferCreateInfo FramebufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(*Renderer->RenderPass)
			.setAttachmentCount(1)
			.setWidth(WindowSize->width)
			.setHeight(WindowSize->height)
			.setLayers(1);

		for (std::uint32_t i = 0; i != VulkanBase->GetSwapchainImageCount(); ++i)
		{
			vk::ImageView Attachment = VulkanBase->GetSwapchainImageView(i);
			FramebufferCreateInfo.setAttachments(Attachment);
			Renderer->Framebuffers.emplace_back(VulkanBase->GetDevice(), FramebufferCreateInfo);
		}
	};

	auto DestroyFramebuffers = [VulkanBase = _VulkanBase, Renderer = &_Renderer]() -> void
	{
		VulkanBase->WaitIdle();
		Renderer->Framebuffers.clear();
	};

	CreateFramebuffers();

	static bool bCallbackAdded = false;
	if (!bCallbackAdded)
	{
		_VulkanBase->AddCreateSwapchainCallback("CreateFramebuffers", CreateFramebuffers);
		_VulkanBase->AddDestroySwapchainCallback("DestroyFramebuffers", DestroyFramebuffers);
		bCallbackAdded = true;
	}
}

void FApplication::CreatePipeline()
{
	static FVulkanShaderModule VertShaderModule(_VulkanBase->GetDevice(), "Sources/Shaders/Triangle.vert.spv");
	static FVulkanShaderModule FragShaderModule(_VulkanBase->GetDevice(), "Sources/Shaders/Triangle.frag.spv");

	static vk::PipelineShaderStageCreateInfo VertShaderStage = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(*VertShaderModule)
		.setPName("main");
	static vk::PipelineShaderStageCreateInfo FragShaderStage = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(*FragShaderModule)
		.setPName("main");

	static std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageCreateInfos{ VertShaderStage, FragShaderStage };

	auto Create = [this, VulkanBase = _VulkanBase]() -> void
	{
		// 先创建新管线，再销毁旧管线
		FGraphicsPipelineCreateInfoPack Pack;
		Pack.GraphicsPipelineCreateInfo.setLayout(*_Layout);
		Pack.GraphicsPipelineCreateInfo.setRenderPass(*_Renderer.RenderPass);
		Pack.InputAssemblyStateCreateInfo.setTopology(vk::PrimitiveTopology::eTriangleList);
		Pack.MultisampleStateCreateInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		Pack.ShaderStages = ShaderStageCreateInfos;

		vk::PipelineColorBlendAttachmentState ColorBlendAttachmentState = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
							   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

		//Pack.DynamicStates.emplace_back(vk::DynamicState::eViewport);
		//Pack.DynamicStates.emplace_back(vk::DynamicState::eScissor);

		Pack.Viewports.emplace_back(0.0f, 0.0f, static_cast<float>(_WindowSize.width),
									static_cast<float>(_WindowSize.height), 0.0f, 1.0f);
		Pack.Scissors.emplace_back(vk::Offset2D(), _WindowSize);
		Pack.ColorBlendAttachmentStates.emplace_back(ColorBlendAttachmentState);

		Pack.Update();

		FVulkanPipeline NewPipeline(_VulkanBase->GetDevice(), Pack);
		_VulkanBase->WaitIdle();
		if (_Pipeline)
		{
			FVulkanPipeline OldPipeline = std::move(_Pipeline);
			_Pipeline = std::move(NewPipeline);
		}
		else
		{
			_Pipeline = std::move(NewPipeline);
		}
	};

	auto Destroy = [this, VulkanBase = _VulkanBase]() -> void
	{
		if (_Pipeline)
		{
			VulkanBase->WaitIdle();
			FVulkanPipeline OldPipeline = std::move(_Pipeline);
		}
	};

	Create();

	_VulkanBase->AddCreateSwapchainCallback("CreatePipeline", Create);
	_VulkanBase->AddDestroySwapchainCallback("DestroyPipeline", Destroy);
}

void FApplication::CreateLayout()
{
	vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo;
	_Layout = FVulkanPipelineLayout(_VulkanBase->GetDevice(), PipelineLayoutCreateInfo);
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
		FrameCount = 0;
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

void FApplication::FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height)
{
	auto* App = reinterpret_cast<FApplication*>(glfwGetWindowUserPointer(Window));

	if (Width == 0 || Height == 0)
	{
		return;
	}

	App->_WindowSize.width  = Width;
	App->_WindowSize.height = Height;
	App->_VulkanBase->WaitIdle();
	App->_VulkanBase->RecreateSwapchain();
}

_NPGS_END
