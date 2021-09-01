#version 120
precision mediump float;

attribute vec4 position;
attribute vec3 normal;
attribute vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 model;

varying vec3 Normal;
varying vec3 WorldPos;
varying vec2 TexCoords;

void main()
{

    // Outputs the positions/coordinates of all vertices
    gl_Position = u_MVP * position;

    // Outputs Normals
    Normal = mat3(model) * normal;

    // Outputs Current Position
    WorldPos = vec3(model * position);

    // Texture Coordinates
    TexCoords = texCoord;
}