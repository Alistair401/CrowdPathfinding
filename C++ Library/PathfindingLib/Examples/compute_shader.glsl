#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430) buffer unit_buffer
{ 
    vec4 units[];
};

layout (std430) buffer count_buffer
{ 
    uint counts[];
};

layout (std430) buffer index_buffer
{ 
    uint index[];
};

layout (std430) buffer neighbor_buffer
{ 
    vec4 neighbors[];
};

layout (std430) buffer output_buffer
{
    vec4 outputs[];
};

const float cohesion = 0.001f;
const float separation = 1.0f;

void main(){
    vec4 pos = units[gl_WorkGroupID.x];
    vec4 resultant = vec4(0.0);
    if (counts[gl_WorkGroupID.x] > 0)
    {
        vec4 separation_force;
        vec4 cohesion_force;
        for (int i = 0; i < counts[gl_WorkGroupID.x]; i++){
            vec4 neighbor_pos = neighbors[index[gl_WorkGroupID.x] + i];
            vec4 separating_vector = pos - neighbor_pos;
            separation_force = separation_force + (separating_vector / dot(separating_vector,separating_vector));
            cohesion_force = cohesion_force + neighbor_pos;
        }
        cohesion_force = ((cohesion_force / counts[gl_WorkGroupID.x]) - pos) * cohesion;
        separation_force = separation_force * separation;
        resultant = separation_force + cohesion_force;
    }
    outputs[gl_WorkGroupID.x] = resultant;
}
