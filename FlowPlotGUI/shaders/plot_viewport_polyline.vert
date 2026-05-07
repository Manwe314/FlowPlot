#version 450

layout(push_constant) uniform PlotPushConstants {
	float viewportWidth;
	float viewportHeight;
	float imageWidth;
	float imageHeight;
	float cameraCenterX;
	float cameraCenterY;
	float zoom;
	float _pad;
} pc;

struct PolylineVertex {
	float x;
	float y;
	float colorR;
	float colorG;
	float colorB;
	float colorA;
};

layout(set = 0, binding = 4, std430) readonly buffer PolylineVertexBuffer {
	PolylineVertex vertices[];
} polylineBuffer;

layout(location = 0) out vec4 vColor;

void main() {
	PolylineVertex vertex = polylineBuffer.vertices[gl_VertexIndex];
	vec2 worldPosition = vec2(vertex.x, vertex.y);
	vec2 screenPosition = (worldPosition - vec2(pc.cameraCenterX, pc.cameraCenterY)) * max(pc.zoom, 1.0e-6) + vec2(pc.viewportWidth, pc.viewportHeight) * 0.5;

	vec2 clipPosition = vec2(
		(screenPosition.x / max(pc.viewportWidth, 1.0)) * 2.0 - 1.0,
		(screenPosition.y / max(pc.viewportHeight, 1.0)) * 2.0 - 1.0
	);

	gl_Position = vec4(clipPosition, 0.0, 1.0);
	vColor = vec4(vertex.colorR, vertex.colorG, vertex.colorB, vertex.colorA);
}
