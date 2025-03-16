#version 450

// Uniform buffer for transforms
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


struct Vertex {
	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

// Vertex buffer as storage buffer
layout(binding = 1) readonly buffer VertexBuffer {
    Vertex vertices[];
} vertexBuffer;

// Outputs to fragment shader
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    // Get vertex data using gl_VertexIndex
    Vertex vertex = vertexBuffer.vertices[gl_VertexIndex];
    
    // Transform position
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(vertex.position, 1.0);
    
    // Pass through vertex attributes
    fragColor = vertex.color.rgb;
    fragTexCoord = vec2(vertex.uv_x, vertex.uv_y);
}
