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

struct BoxInstance {
	float x;
	float y;
	float w;
	float h;
	float strokeWidth;
	float fillR;
	float fillG;
	float fillB;
	float fillA;
	float strokeR;
	float strokeG;
	float strokeB;
	float strokeA;
};

layout(set = 0, binding = 0, std430) readonly buffer BoxInstanceBuffer {
	BoxInstance boxes[];
} boxBuffer;

layout(location = 0) out vec2 vLocal;
layout(location = 1) out vec2 vSize;
layout(location = 2) out float vStrokeWidth;
layout(location = 3) out vec4 vFill;
layout(location = 4) out vec4 vStroke;

void main() {
	const vec2 unitPositions[6] = vec2[](
		vec2(0.0, 0.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0),
		vec2(0.0, 0.0),
		vec2(1.0, 1.0),
		vec2(0.0, 1.0)
	);

	BoxInstance box = boxBuffer.boxes[gl_InstanceIndex];
	vec2 unitPosition = unitPositions[gl_VertexIndex];
	vec2 worldPosition = vec2(box.x, box.y) + unitPosition * vec2(box.w, box.h);
	vec2 screenPosition = (worldPosition - vec2(pc.cameraCenterX, pc.cameraCenterY)) * pc.zoom + vec2(pc.viewportWidth, pc.viewportHeight) * 0.5;

	vec2 clipPosition = vec2(
		(screenPosition.x / max(pc.viewportWidth, 1.0)) * 2.0 - 1.0,
		(screenPosition.y / max(pc.viewportHeight, 1.0)) * 2.0 - 1.0
	);

	gl_Position = vec4(clipPosition, 0.0, 1.0);

	vLocal = unitPosition * vec2(box.w, box.h) * pc.zoom;
	vSize = vec2(box.w, box.h) * pc.zoom;
	vStrokeWidth = box.strokeWidth * pc.zoom;
	vFill = vec4(box.fillR, box.fillG, box.fillB, box.fillA);
	vStroke = vec4(box.strokeR, box.strokeG, box.strokeB, box.strokeA);
}
