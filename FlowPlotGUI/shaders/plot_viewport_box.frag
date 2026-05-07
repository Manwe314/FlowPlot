#version 450

layout(location = 0) in vec2 vLocal;
layout(location = 1) in vec2 vSize;
layout(location = 2) in float vStrokeWidth;
layout(location = 3) in vec4 vFill;
layout(location = 4) in vec4 vStroke;

layout(location = 0) out vec4 outColor;

void main() {
	vec2 distanceToEdge = min(vLocal, vSize - vLocal);
	float edgeDistance = min(distanceToEdge.x, distanceToEdge.y);
	float strokeWidth = clamp(vStrokeWidth, 0.0, min(vSize.x, vSize.y) * 0.5);
	vec4 color = edgeDistance < strokeWidth ? vStroke : vFill;

	if (color.a <= 0.0) {
		discard;
	}
	outColor = color;
}
