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

struct TextGlyphInstance {
	float x;
	float y;
	float xBasisX;
	float xBasisY;
	float yBasisX;
	float yBasisY;
	float u0;
	float v0;
	float u1;
	float v1;
	uint atlasLayer;
	float distanceRangePx;
	float colorR;
	float colorG;
	float colorB;
	float colorA;
};

layout(set = 0, binding = 1, std430) readonly buffer TextGlyphInstanceBuffer {
	TextGlyphInstance glyphs[];
} glyphBuffer;

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vUv;
layout(location = 2) flat out uint vAtlasLayer;
layout(location = 3) out float vDistanceRangePx;

void main() {
	const vec2 unitPositions[6] = vec2[](
		vec2(0.0, 0.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0),
		vec2(0.0, 0.0),
		vec2(1.0, 1.0),
		vec2(0.0, 1.0)
	);

	TextGlyphInstance glyph = glyphBuffer.glyphs[gl_InstanceIndex];
	vec2 unitPosition = unitPositions[gl_VertexIndex];
	vec2 worldPosition = vec2(glyph.x, glyph.y)
		+ unitPosition.x * vec2(glyph.xBasisX, glyph.xBasisY)
		+ unitPosition.y * vec2(glyph.yBasisX, glyph.yBasisY);
	vec2 screenPosition = (worldPosition - vec2(pc.cameraCenterX, pc.cameraCenterY)) * pc.zoom + vec2(pc.viewportWidth, pc.viewportHeight) * 0.5;

	vec2 clipPosition = vec2(
		(screenPosition.x / max(pc.viewportWidth, 1.0)) * 2.0 - 1.0,
		(screenPosition.y / max(pc.viewportHeight, 1.0)) * 2.0 - 1.0
	);

	gl_Position = vec4(clipPosition, 0.0, 1.0);

	vColor = vec4(glyph.colorR, glyph.colorG, glyph.colorB, glyph.colorA);
	vUv = mix(vec2(glyph.u0, glyph.v0), vec2(glyph.u1, glyph.v1), unitPosition);
	vAtlasLayer = glyph.atlasLayer;
	vDistanceRangePx = glyph.distanceRangePx;
}
