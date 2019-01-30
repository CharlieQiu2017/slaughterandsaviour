#version 330 core

uniform sampler2D texturedata;
in  vec2 ex_Color;
out vec4 out_Color;

void main(void) {
    vec4 t = texture(texturedata,ex_Color);
    out_Color = t;
}