#version 330 core

out vec4 FragColor;

layout(std140) uniform color_ubo
{
   float r;
   float g;
   float b;
};

void main()
{
   FragColor = vec4(r, g, b, 1.0f);
}
