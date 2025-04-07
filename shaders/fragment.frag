#version 100
precision lowp float;

varying vec4 frag_pos;
varying vec2 frag_uv;

uniform sampler2D ge_texture;
uniform vec2 ge_uv_max;

void main() {
    vec2 uv;
    uv = mod((frag_uv*ge_uv_max), ge_uv_max);
    gl_FragColor = texture2D(ge_texture, uv);
}

