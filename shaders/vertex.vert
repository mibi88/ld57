#version 100
precision highp float;

attribute vec4 ge_vertex;
attribute vec2 ge_uv;

varying vec4 frag_pos;
varying vec2 frag_uv;

uniform mat4 ge_projection_mat;
uniform mat4 ge_view_mat;
uniform mat4 ge_model_mat;

void main() {
    frag_pos = ge_projection_mat*ge_view_mat*ge_model_mat*ge_vertex;
    frag_uv = ge_uv;
    gl_Position = frag_pos;
}

