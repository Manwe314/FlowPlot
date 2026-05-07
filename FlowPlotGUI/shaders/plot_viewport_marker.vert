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

struct MarkerInstance {
	float centerX;
	float centerY;
	float size;
	float strokeWidth;
	uint shape;
	float fillR;
	float fillG;
	float fillB;
	float fillA;
	float strokeR;
	float strokeG;
	float strokeB;
	float strokeA;
};

layout(set = 0, binding = 2, std430) readonly buffer MarkerInstanceBuffer {
	MarkerInstance markers[];
} markerBuffer;

layout(location = 0) out vec2 vLocal;
layout(location = 1) out float vHalfSize;
layout(location = 2) out float vStrokeWidth;
layout(location = 3) flat out uint vShape;
layout(location = 4) out vec4 vFill;
layout(location = 5) out vec4 vStroke;

void main() {
	const vec2 unitPositions[6] = vec2[](
		vec2(-1.0, -1.0),
		vec2( 1.0, -1.0),
		vec2( 1.0,  1.0),
		vec2(-1.0, -1.0),
		vec2( 1.0,  1.0),
		vec2(-1.0,  1.0)
	);

	MarkerInstance marker = markerBuffer.markers[gl_InstanceIndex];
	vec2 unitPosition = unitPositions[gl_VertexIndex];
	float zoom = max(pc.zoom, 1.0e-6);
	float halfSizePx = max(marker.size * zoom * 0.5, 0.0);
	float strokeWidthPx = max(marker.strokeWidth * zoom, 0.0);
	float aaPaddingPx = 2.0;
	float quadHalfExtentPx = halfSizePx + strokeWidthPx + aaPaddingPx;
	vec2 screenCenter = (vec2(marker.centerX, marker.centerY) - vec2(pc.cameraCenterX, pc.cameraCenterY)) * zoom + vec2(pc.viewportWidth, pc.viewportHeight) * 0.5;
	vec2 screenPosition = screenCenter + unitPosition * quadHalfExtentPx;

	vec2 clipPosition = vec2(
		(screenPosition.x / max(pc.viewportWidth, 1.0)) * 2.0 - 1.0,
		(screenPosition.y / max(pc.viewportHeight, 1.0)) * 2.0 - 1.0
	);

	gl_Position = vec4(clipPosition, 0.0, 1.0);

	vLocal = unitPosition * quadHalfExtentPx;
	vHalfSize = halfSizePx;
	vStrokeWidth = strokeWidthPx;
	vShape = marker.shape;
	vFill = vec4(marker.fillR, marker.fillG, marker.fillB, marker.fillA);
	vStroke = vec4(marker.strokeR, marker.strokeG, marker.strokeB, marker.strokeA);
}
