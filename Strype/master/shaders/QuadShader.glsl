#type vertex
#version 330 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_ObjectID;

uniform mat4 u_ViewProjection;

out vec4 v_Colour;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_ObjectID;

void main()
{
    v_Colour = a_Colour;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_ObjectID = a_ObjectID;
    gl_Position = u_ViewProjection * vec4(a_Position.xyz, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 colour;
layout(location = 1) out float objectID;

in vec4 v_Colour;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_ObjectID;

uniform sampler2D u_Textures[32];

void main()
{
    colour = v_Colour * texture(u_Textures[int(v_TexIndex)], v_TexCoord);
    objectID = v_ObjectID;
}
