#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf
{
    mat4 qt_Matrix;
    float qt_Opacity;
    float exposure;
};

layout(binding = 1) uniform sampler2D source;

const float SrgbLinearCutoff = 0.0031308;
const float SrgbLinearScale = 12.92;
const float SrgbPowerScale = 1.055;
const float SrgbPowerOffset = 0.055;
const float SrgbPowerExponent = 1.0 / 2.4;

float LinearToSrgb(float value)
{
    value = max(value, 0.0);

    if (value <= SrgbLinearCutoff)
        return value * SrgbLinearScale;

    return SrgbPowerScale * pow(value, SrgbPowerExponent) - SrgbPowerOffset;
}

vec3 LinearToSrgb(vec3 value)
{
    return vec3(
        LinearToSrgb(value.r),
        LinearToSrgb(value.g),
        LinearToSrgb(value.b)
    );
}

void main()
{
    const vec3 sourceColor = texture(source, qt_TexCoord0).rgb;

    // Exposure is deliberately a pure scene-linear EV adjustment.
    // Highlight reconstruction belongs before this stage and the
    // display/view transform belongs after it.
    const float exposureMultiplier = exp2(exposure);
    const vec3 exposedColor = sourceColor * exposureMultiplier;

    const vec3 displayColor = LinearToSrgb(exposedColor);

    fragColor = vec4(displayColor, 1.0) * qt_Opacity;
}
