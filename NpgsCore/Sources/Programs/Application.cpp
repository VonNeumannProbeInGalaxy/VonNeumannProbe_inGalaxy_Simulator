#include "Application.h"

#include <cstddef>
#include <cstdint>
#include <utility>

#include "Engine/Utils/Logger.h"

_NPGS_BEGIN

FApplication::FApplication(const vk::Extent2D& WindowSize, const std::string& WindowTitle,
                           bool bEnableVSync, bool bEnableFullscreen)
    :
    _VulkanBase(Runtime::Graphics::FVulkanBase::GetInstance()),
    _VulkanCore(_VulkanBase->GetVulkanCore()),
    _WindowTitle(WindowTitle),
    _WindowSize(WindowSize),
    _Window(nullptr),
    _bEnableVSync(bEnableVSync),
    _bEnableFullscreen(bEnableFullscreen)
{
    if (!InitWindow())
    {
        NpgsCoreError("Failed to create application.");
    }
}

FApplication::~FApplication()
{
    RemoveRegisteredCallbacks();
}

void FApplication::ExecuteMainRender()
{
    CreateScreenRender();
    CreatePipelineLayout();
    CreatePipeline();

    const auto& [Framebuffers, RenderPass] = _Renderer;

    std::size_t FramesInFlightCount = 2;

    std::vector<Runtime::Graphics::FVulkanFence> InFlightFences(FramesInFlightCount);
    std::vector<Runtime::Graphics::FVulkanSemaphore> Semaphores_ImageAvailable(FramesInFlightCount);
    std::vector<Runtime::Graphics::FVulkanSemaphore> Semaphores_RenderFinished(FramesInFlightCount);
    std::vector<Runtime::Graphics::FVulkanCommandBuffer> CommandBuffers(FramesInFlightCount);

    vk::FenceCreateFlags FenceCreateFlags{ vk::FenceCreateFlagBits::eSignaled };
    for (std::size_t i = 0; i != FramesInFlightCount; ++i)
    {
        InFlightFences[i] = Runtime::Graphics::FVulkanFence(_VulkanCore->GetDevice(), FenceCreateFlags);
        Semaphores_ImageAvailable[i] = Runtime::Graphics::FVulkanSemaphore(_VulkanCore->GetDevice());
        Semaphores_RenderFinished[i] = Runtime::Graphics::FVulkanSemaphore(_VulkanCore->GetDevice());
    }

    Runtime::Graphics::FVulkanCommandPool CommandPool(_VulkanCore->GetDevice(), _VulkanCore->GetGraphicsQueueFamilyIndex(),
                                   vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    CommandPool.AllocateBuffers(vk::CommandBufferLevel::ePrimary, CommandBuffers);

    vk::ClearValue ColorValue({ 0.0f, 0.0f, 0.0f, 1.0f });

    std::uint32_t CurrentFrame = 0;

    while (!glfwWindowShouldClose(_Window))
    {
        while (glfwGetWindowAttrib(_Window, GLFW_ICONIFIED))
        {
            glfwWaitEvents();
        }

        InFlightFences[CurrentFrame].WaitAndReset();

        _VulkanCore->SwapImage(Semaphores_ImageAvailable[CurrentFrame]);
        std::uint32_t ImageIndex = _VulkanCore->GetCurrentImageIndex();

        CommandBuffers[CurrentFrame].Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        RenderPass.CommandBegin(CommandBuffers[CurrentFrame], Framebuffers[ImageIndex], { {}, _WindowSize }, { ColorValue });
        CommandBuffers[CurrentFrame]->bindPipeline(vk::PipelineBindPoint::eGraphics, *_VulkanPipeline);
        CommandBuffers[CurrentFrame]->draw(3, 1, 0, 0);
        RenderPass.CommandEnd(CommandBuffers[CurrentFrame]);
        CommandBuffers[CurrentFrame].End();

        _VulkanBase->SubmitCommandBufferToGraphics(CommandBuffers[CurrentFrame], Semaphores_ImageAvailable[CurrentFrame],
                                                   Semaphores_RenderFinished[CurrentFrame], InFlightFences[CurrentFrame]);
        _VulkanCore->PresentImage(Semaphores_RenderFinished[CurrentFrame]);

        CurrentFrame = (CurrentFrame + 1) % FramesInFlightCount;

        ProcessInput();
        glfwPollEvents();
        ShowTitleFps();
    }

    Terminate();
}

void FApplication::Terminate()
{
    _VulkanCore->WaitIdle();
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
        _VulkanCore->AddInstanceExtension(Extensions[i]);
    }

    _VulkanCore->AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    vk::Result Result;
    if ((Result = _VulkanCore->CreateInstance()) != vk::Result::eSuccess)
    {
        glfwDestroyWindow(_Window);
        glfwTerminate();
        return false;
    }

    vk::SurfaceKHR Surface;
    if (glfwCreateWindowSurface(_VulkanCore->GetInstance(), _Window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&Surface)) != VK_SUCCESS)
    {
        NpgsCoreError("Failed to create window surface.");
        glfwDestroyWindow(_Window);
        glfwTerminate();
        return false;
    }
    _VulkanCore->SetSurface(Surface);

    if (_VulkanCore->CreateDevice(0) != vk::Result::eSuccess ||
        _VulkanCore->CreateSwapchain(_WindowSize, false) != vk::Result::eSuccess)
    {
        return false;
    }

    return true;
}

void FApplication::CreateScreenRender()
{
    vk::AttachmentDescription AttachmentDescription = vk::AttachmentDescription()
        .setFormat(_VulkanCore->GetSwapchainCreateInfo().imageFormat)
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
    _Renderer.RenderPass = Runtime::Graphics::FVulkanRenderPass(_VulkanCore->GetDevice(), RenderPassCreateInfo);

    auto CreateFramebuffers =
        [VulkanCore = _VulkanCore, Renderer = &_Renderer, WindowSize = &_WindowSize]() -> void
    {
        VulkanCore->WaitIdle();
        Renderer->Framebuffers.clear();
        Renderer->Framebuffers.reserve(VulkanCore->GetSwapchainImageCount());

        vk::FramebufferCreateInfo FramebufferCreateInfo = vk::FramebufferCreateInfo()
            .setRenderPass(*Renderer->RenderPass)
            .setAttachmentCount(1)
            .setWidth(WindowSize->width)
            .setHeight(WindowSize->height)
            .setLayers(1);

        for (std::uint32_t i = 0; i != VulkanCore->GetSwapchainImageCount(); ++i)
        {
            vk::ImageView Attachment = VulkanCore->GetSwapchainImageView(i);
            FramebufferCreateInfo.setAttachments(Attachment);
            Renderer->Framebuffers.emplace_back(VulkanCore->GetDevice(), FramebufferCreateInfo);
        }
    };

    auto DestroyFramebuffers = [VulkanCore = _VulkanCore, Renderer = &_Renderer]() -> void
    {
        VulkanCore->WaitIdle();
        Renderer->Framebuffers.clear();
    };

    CreateFramebuffers();

    static bool bCallbackAdded = false;
    if (!bCallbackAdded)
    {
        RegisterAutoRemovedCallbacks(ECallbackType::kCreateSwapchain, "CreateFramebuffers", CreateFramebuffers);
        RegisterAutoRemovedCallbacks(ECallbackType::kDestroySwapchain, "DestroyFramebuffers", DestroyFramebuffers);
        bCallbackAdded = true;
    }
}

void FApplication::CreatePipelineLayout()
{
    vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo;
    _VulkanPipelineLayout = Runtime::Graphics::FVulkanPipelineLayout(_VulkanCore->GetDevice(), PipelineLayoutCreateInfo);
}

void FApplication::CreatePipeline()
{
    static Runtime::Graphics::FVulkanShaderModule VertShaderModule(_VulkanCore->GetDevice(), "Sources/Engine/Shaders/Triangle.vert.spv");
    static Runtime::Graphics::FVulkanShaderModule FragShaderModule(_VulkanCore->GetDevice(), "Sources/Engine/Shaders/Triangle.frag.spv");

    static vk::PipelineShaderStageCreateInfo VertShaderStage = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(*VertShaderModule)
        .setPName("main");
    static vk::PipelineShaderStageCreateInfo FragShaderStage = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(*FragShaderModule)
        .setPName("main");

    static std::vector<vk::PipelineShaderStageCreateInfo> ShaderStageCreateInfos{ VertShaderStage, FragShaderStage };

#include "Vertices.inc"

    auto Create = [this, VulkanCore = _VulkanCore]() -> void
    {
        // 先创建新管线，再销毁旧管线
        Runtime::Graphics::FGraphicsPipelineCreateInfoPack Pack;
        Pack.GraphicsPipelineCreateInfo.setLayout(*_VulkanPipelineLayout);
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

        Runtime::Graphics::FVulkanPipeline NewPipeline(_VulkanCore->GetDevice(), Pack);
        _VulkanCore->WaitIdle();
        if (_VulkanPipeline)
        {
            Runtime::Graphics::FVulkanPipeline OldPipeline = std::move(_VulkanPipeline);
            _VulkanPipeline = std::move(NewPipeline);
        }
        else
        {
            _VulkanPipeline = std::move(NewPipeline);
        }
    };

    auto Destroy = [VulkanCore = _VulkanCore, VulkanPipeline = &_VulkanPipeline]() -> void
    {
        if (VulkanPipeline)
        {
            VulkanCore->WaitIdle();
            Runtime::Graphics::FVulkanPipeline OldPipeline = std::move(*VulkanPipeline);
        }
    };

    Create();

    static bool bCallbackAdded = false;
    if (!bCallbackAdded)
    {
        RegisterAutoRemovedCallbacks(ECallbackType::kCreateSwapchain, "CreatePipeline", Create);
        RegisterAutoRemovedCallbacks(ECallbackType::kDestroySwapchain, "DestroyPipeline", Destroy);
        bCallbackAdded = true;
    }
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

void FApplication::RegisterAutoRemovedCallbacks(ECallbackType Type, const std::string& Name, const std::function<void()>& Callback)
{
    switch (Type)
    {
    case ECallbackType::kCreateSwapchain:
        _VulkanCore->AddCreateSwapchainCallback(Name, Callback);
        break;
    case ECallbackType::kDestroySwapchain:
        _VulkanCore->AddDestroySwapchainCallback(Name, Callback);
        break;
    case ECallbackType::kCreateDevice:
        _VulkanCore->AddCreateDeviceCallback(Name, Callback);
        break;
    case ECallbackType::kDestroyDevice:
        _VulkanCore->AddDestroyDeviceCallback(Name, Callback);
        break;
    default:
        break;
    }

    std::pair<ECallbackType, std::string> AutoRemovedCallback(Type, Name);
    _AutoRemovedCallbacks.emplace_back(AutoRemovedCallback);
}

void FApplication::RemoveRegisteredCallbacks()
{
    for (const auto& [Type, Name] : _AutoRemovedCallbacks)
    {
        switch (Type)
        {
        case ECallbackType::kCreateSwapchain:
            _VulkanCore->RemoveCreateSwapchainCallback(Name);
            break;
        case ECallbackType::kDestroySwapchain:
            _VulkanCore->RemoveDestroySwapchainCallback(Name);
            break;
        case ECallbackType::kCreateDevice:
            _VulkanCore->RemoveCreateDeviceCallback(Name);
            break;
        case ECallbackType::kDestroyDevice:
            _VulkanCore->RemoveDestroyDeviceCallback(Name);
            break;
        default:
            break;
        }
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
    App->_VulkanCore->WaitIdle();
    App->_VulkanCore->RecreateSwapchain();
}

_NPGS_END
