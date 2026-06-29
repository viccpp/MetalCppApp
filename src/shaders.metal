#include <metal_stdlib>
using namespace metal;

// ---------------------------------------------------------------------------
// Must match Vertex / Uniforms structs in renderer.cpp
// ---------------------------------------------------------------------------
struct VertexIn {
    float3 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct Uniforms {
    float time;
};

// ---------------------------------------------------------------------------
// Vertex shader — rotates the triangle over time
// ---------------------------------------------------------------------------
vertex VertexOut vert_main(VertexIn in               [[stage_in]],
                           constant Uniforms& u       [[buffer(1)]]) {
    float angle = u.time;
    float cosA  = cos(angle);
    float sinA  = sin(angle);

    float2 pos = float2(
        in.position.x * cosA - in.position.y * sinA,
        in.position.x * sinA + in.position.y * cosA
    );

    VertexOut out;
    out.position = float4(pos, 0.0, 1.0);
    out.color    = in.color;
    return out;
}

// ---------------------------------------------------------------------------
// Fragment shader
// ---------------------------------------------------------------------------
fragment float4 frag_main(VertexOut in [[stage_in]]) {
    return in.color;
}
