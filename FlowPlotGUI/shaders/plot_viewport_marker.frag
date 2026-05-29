#version 450

layout(location = 0) in vec2 vLocal;
layout(location = 1) in float vHalfSize;
layout(location = 2) in float vStrokeWidth;
layout(location = 3) flat in uint vShape;
layout(location = 4) in vec4 vFill;
layout(location = 5) in vec4 vStroke;

layout(location = 0) out vec4 outColor;

const uint SHAPE_CIRCLE = 0u;
const uint SHAPE_SQUARE = 1u;
const uint SHAPE_DIAMOND = 2u;
const uint SHAPE_TRIANGLE = 3u;

float sdCircle(vec2 p, float radius) {
	return length(p) - radius;
}

float sdSquare(vec2 p, float halfExtent) {
	vec2 q = abs(p) - vec2(halfExtent);
	return length(max(q, vec2(0.0))) + min(max(q.x, q.y), 0.0);
}

float sdEquilateralTriangle(vec2 p, float radius) {
	const float k = sqrt(3.0);
	p.y += radius * 0.5;
	p.x = abs(p.x) - radius;
	p.y = p.y + radius / k;
	if (p.x + k * p.y > 0.0) {
		p = vec2(p.x - k * p.y, -k * p.x - p.y) / 2.0;
	}
	p.x -= clamp(p.x, -2.0 * radius, 0.0);
	return -length(p) * sign(p.y);
}

float sdDiamond(vec2 p, float radius) {
	const float invSqrt2 = 0.7071067811865476;
	return (abs(p.x) + abs(p.y) - radius) * invSqrt2;
}

void main() {
	float d = 0.0;
	if (vShape == SHAPE_CIRCLE) {
		d = sdCircle(vLocal, vHalfSize);
	}
	else if (vShape == SHAPE_SQUARE) {
		d = sdSquare(vLocal, vHalfSize);
	}
	else if (vShape == SHAPE_DIAMOND) {
		d = sdDiamond(vLocal, vHalfSize);
	}
	else {
		d = sdEquilateralTriangle(vLocal, vHalfSize);
	}

	float strokeWidthPx = clamp(vStrokeWidth, 0.0, max(vHalfSize, 0.0));
	float aa = max(fwidth(d), 1.0e-5);
	float shapeAlpha = 1.0 - smoothstep(-aa, aa, d);
	float fillAlpha = 1.0 - smoothstep(
		-strokeWidthPx - aa,
		-strokeWidthPx + aa,
		d
	);
	float strokeAlpha = max(0.0, shapeAlpha - fillAlpha);

	outColor = vFill * fillAlpha + vStroke * strokeAlpha;
	if (outColor.a <= 0.001) {
		discard;
	}
}
