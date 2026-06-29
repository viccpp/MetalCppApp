#include "renderer.h"

#include "os/bridge.h"
#include "os/metal.h"

#include <dispatch/dispatch.h>

#include <cstring>
#include <format>
#include <stdexcept>

namespace space307 {

// ---------------------------------------------------------------------------
// Vertex layout (must match shaders.metal)
// ---------------------------------------------------------------------------
struct Vertex {
    float position[3];
    float color[4];
};

// ---------------------------------------------------------------------------
// Uniforms (must match shaders.metal)
// ---------------------------------------------------------------------------
struct Uniforms {
    float time;
    float padding[3];
};

// ---------------------------------------------------------------------------
// Implementation struct
// ---------------------------------------------------------------------------
static const int kMaxFramesInFlight = 3;

struct Renderer::Impl
{
    MTL::Device*              device        = nullptr;
    MTL::CommandQueue*        commandQueue  = nullptr;
    MTL::RenderPipelineState* pipeline      = nullptr;
    MTL::Buffer*              vertexBuffer  = nullptr;
    MTL::Buffer*              uniformBuffers[kMaxFramesInFlight] = {};
    MTL::DepthStencilState*   depthState    = nullptr;

    dispatch_semaphore_t      frameSemaphore;
    int                       frameIndex    = 0;
    float                     time          = 0.0f;
};

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
Renderer::Renderer(void* metalDevice) : _impl(std::make_unique<Impl>())
{
    _impl->device = static_cast<MTL::Device*>(metalDevice);
    _impl->commandQueue = _impl->device->newCommandQueue();
    _impl->frameSemaphore = dispatch_semaphore_create(kMaxFramesInFlight);

    // ---- Load shader library -----------------------------------------------
    NS::Error* error = nullptr;
    NS::String* libPath = NS::String::string(
        "default.metallib", NS::StringEncoding::UTF8StringEncoding);

    MTL::Library* library = _impl->device->newLibrary(
        NS::URL::fileURLWithPath(libPath), &error);

    if (!library) {
        throw std::runtime_error(std::format(
            "Failed to load default.metallib: {}",
            error->localizedDescription()->utf8String()));
    }

    MTL::Function* vertFn = library->newFunction(
        NS::String::string("vert_main", NS::StringEncoding::UTF8StringEncoding));
    MTL::Function* fragFn = library->newFunction(
        NS::String::string("frag_main", NS::StringEncoding::UTF8StringEncoding));

    // ---- Vertex descriptor -------------------------------------------------
    auto* vtxDesc = MTL::VertexDescriptor::alloc()->init();

    // position (attribute 0)
    auto* obj = vtxDesc->attributes()->object(0);
    obj->setFormat(MTL::VertexFormatFloat3);
    obj->setOffset(offsetof(Vertex, position));
    obj->setBufferIndex(0);

    // color (attribute 1)
    obj = vtxDesc->attributes()->object(1);
    obj->setFormat(MTL::VertexFormatFloat4);
    obj->setOffset(offsetof(Vertex, color));
    obj->setBufferIndex(0);

    vtxDesc->layouts()->object(0)->setStride(sizeof(Vertex));

    // ---- Pipeline descriptor -----------------------------------------------
    auto* pipelineDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pipelineDesc->setVertexFunction(vertFn);
    pipelineDesc->setFragmentFunction(fragFn);
    pipelineDesc->setVertexDescriptor(vtxDesc);
    pipelineDesc->colorAttachments()->object(0)
        ->setPixelFormat(MTL::PixelFormatBGRA8Unorm);

    _impl->pipeline = _impl->device->newRenderPipelineState(pipelineDesc, &error);
    if (!_impl->pipeline) {
        throw std::runtime_error(std::format(
            "Pipeline error: {}", error->localizedDescription()->utf8String()));
    }

    pipelineDesc->release();
    vtxDesc->release();
    vertFn->release();
    fragFn->release();
    library->release();

    // ---- Geometry ----------------------------------------------------------
    const Vertex vertices[] = {
        {{ 0.0f,  0.6f, 0.0f}, {1.0f, 0.2f, 0.2f, 1.0f}},
        {{-0.6f, -0.4f, 0.0f}, {0.2f, 1.0f, 0.2f, 1.0f}},
        {{ 0.6f, -0.4f, 0.0f}, {0.2f, 0.4f, 1.0f, 1.0f}},
    };
    _impl->vertexBuffer = _impl->device->newBuffer(
        vertices, sizeof(vertices), MTL::ResourceStorageModeShared);

    // ---- Uniform buffers (triple-buffered) ---------------------------------
    for (int i = 0; i < kMaxFramesInFlight; i++) {
        _impl->uniformBuffers[i] = _impl->device->newBuffer(
            sizeof(Uniforms), MTL::ResourceStorageModeShared);
    }
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
Renderer::~Renderer()
{
    _impl->vertexBuffer->release();
    for (int i = 0; i < kMaxFramesInFlight; i++)
        _impl->uniformBuffers[i]->release();
    _impl->pipeline->release();
    _impl->commandQueue->release();
}

void Renderer::resize(float /*width*/, float /*height*/)
{
    // Update projection matrix here if needed
}

// ---------------------------------------------------------------------------
// Per-frame draw
// ---------------------------------------------------------------------------
void Renderer::draw(void* nextDrawable)
{
    auto* drawable = static_cast<CA::MetalDrawable*>(nextDrawable);
    if (!drawable) return;

    dispatch_semaphore_wait(_impl->frameSemaphore, DISPATCH_TIME_FOREVER);

    _impl->time += 1.0f / 60.0f;
    int idx = _impl->frameIndex;

    // Update uniforms
    Uniforms uniforms { _impl->time };
    std::memcpy(_impl->uniformBuffers[idx]->contents(), &uniforms, sizeof(uniforms));

    // Render pass descriptor
    auto* passDesc = MTL::RenderPassDescriptor::alloc()->init();
    auto* obj = passDesc->colorAttachments()->object(0);
    obj->setTexture(drawable->texture());
    obj->setLoadAction(MTL::LoadActionClear);
    obj->setStoreAction(MTL::StoreActionStore);
    obj->setClearColor(MTL::ClearColor(0.08, 0.08, 0.12, 1.0));

    auto* cmdBuf  = _impl->commandQueue->commandBuffer();
    auto* encoder = cmdBuf->renderCommandEncoder(passDesc);

    encoder->setRenderPipelineState(_impl->pipeline);
    encoder->setVertexBuffer(_impl->vertexBuffer,        0, 0);
    encoder->setVertexBuffer(_impl->uniformBuffers[idx], 0, 1);
    encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));
    encoder->endEncoding();

    cmdBuf->presentDrawable(drawable);

    // Signal semaphore once GPU finishes this frame
    cmdBuf->addCompletedHandler([sem = _impl->frameSemaphore](MTL::CommandBuffer*) {
        dispatch_semaphore_signal(sem);
    });

    cmdBuf->commit();
    passDesc->release();

    _impl->frameIndex = (idx + 1) % kMaxFramesInFlight;
}

void Renderer::mainLoop()
{
    while (!application_should_quit())
    {
        poll_application_events();

        void* drawable = get_next_drawable();
        draw(drawable);
    }
}

} // namespace
