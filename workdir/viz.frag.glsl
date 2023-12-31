// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2023 sdelang

// This is heavily inspired from the following shadertoy demo from windytan:
// https://www.shadertoy.com/view/ftKGRc

#version 130

#define PI 3.14159265358979323846

// Colormap as provided by the `cmap.png` texture.
// Might be unused.
uniform sampler2D cmap;

// Screen resolution in pixels.
uniform vec2 resolution;

// FFT Nx1 sampler2D texture.
// Not 1D due to SFML limitations.
uniform sampler2D fft;

// N, i.e. the number of output bins for the FFT, and the width of the `fft`
// texture.
uniform int fft_size;

// Sample rate of the audio signal in Hz.
// Might have gotten resampled during recording. Typically 44100Hz.
uniform float sample_rate;

// Spiral visual parameters from https://www.shadertoy.com/view/WtjSWt
// Also some other viz related parameters
// Uniforms because the application may need to use the same values.
uniform float spiral_start;
uniform float spiral_dis;
uniform float spiral_width;
uniform float spiral_blur;
uniform float vol_min;
uniform float vol_max;

uniform int smooth_fft; // bool 0/1

// Musical constants
// https://en.wikipedia.org/wiki/12_equal_temperament
const float tune_freq = 220.0;            // FIXME: with the offset this makes less sense
const float tet_root  = 1.05946309435929; // 12th root of 2

void main() {
    vec2  uv     = gl_FragCoord.xy / resolution.xy;
    float aspect = resolution.x / resolution.y;
    
    vec2 uvcorrected = uv - vec2(0.5, 0.5);
    uvcorrected.x   *= aspect;

    float angle      = atan(uvcorrected.y, uvcorrected.x);
    float baseoffset = (length(uvcorrected) - spiral_start);
    float offset     = baseoffset + (angle/(2. * PI)) * spiral_dis;
    float which_turn = floor(offset / spiral_dis);
    float cents      = (which_turn - (angle/(2. * PI))) * 1200.;
    float freq       = tune_freq * pow(tet_root, cents / 100.);
    float bin        = freq / sample_rate;
    float bri;

    if (smooth_fft == 1)
    {
        bri = texture(fft, vec2(bin, 0.25)).r;
    }
    else
    {
        bri = texelFetch(fft, ivec2(int(bin * float(fft_size)), 0), 0).r;
    }

    if (baseoffset <= 0.0) {
        gl_FragColor.rgba = vec4(vec3(0.0), 1.0);
        return;
    }

    bri = (bri - vol_min) / (vol_max - vol_min);
    bri = max(bri, 0.);

    // Control the curve of the color mapping. Try e.g. 2. or 4.
    bri = pow(bri, 1.5);

    // vec3 lineColor = texture(cmap, vec2(bri, 0.25)).rgb;
    vec3 lineColor = vec3(bri*1.0, bri*0.25, bri*0.1 + 0.20*(1.0-baseoffset*2.0));

    float circles = mod(offset, spiral_dis);
    vec3  col     = (
        bin > 1.
        ? vec3(0., 0., 0.)
        : (smoothstep(circles-spiral_blur, circles, spiral_width) -
           smoothstep(circles, circles+spiral_blur, spiral_width)) * lineColor);

    if (baseoffset <= 0.01) {
        col.rgb *= baseoffset / 0.01;
    }

    gl_FragColor   = vec4(col, 1.);
}