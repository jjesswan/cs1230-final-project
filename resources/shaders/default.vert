#version 330 core

// declare where position is using index of place in vao
layout(location = 0) in vec3 obj_space_pos;
layout(location = 1) in vec3 obj_space_normal;

// out variables to be passed into frag shader
out vec4 world_space_pos;
out vec4 world_space_normal;

// uniforms mat4s to store matrices
uniform mat4 m_model; // ctm
uniform mat4 m_view;
uniform mat4 m_proj;

uniform mat3 inverse_transpose_ctm;

void main() {

    // get world space position and normal
    world_space_pos = (m_model)*(vec4(obj_space_pos, 1.0));

    world_space_normal = vec4((inverse_transpose_ctm)*(normalize(obj_space_normal)), 0.0);

    // set gl_position to clip_space position
    gl_Position = (m_proj)*(m_view)*(world_space_pos);
}
