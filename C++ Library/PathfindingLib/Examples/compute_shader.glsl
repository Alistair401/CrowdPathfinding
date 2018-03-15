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

const float cohesion = 0.06f;
const float separation = 1.0f;

void main(){
    vec4 pos = units[gl_WorkGroupID.x];
    vec4 resultant = vec4(0.0);
    if (counts[gl_WorkGroupID.x] > 0)
    {
        vec4 separation_force = vec4(0.0);
        vec4 cohesion_force = vec4(0.0);
        uint neighbor_count = counts[gl_WorkGroupID.x];
        for (uint i = 0; i < neighbor_count; i++){
            vec4 neighbor_pos = neighbors[index[gl_WorkGroupID.x] + i];
            vec4 separation = pos - neighbor_pos;
            vec4 separation_normalized = normalize(separation);
            separation_force += separation_normalized / length(separation);
            cohesion_force += neighbor_pos;
        }
        cohesion_force = normalize((cohesion_force / neighbor_count) - pos);
        resultant = (separation * separation_force) + (cohesion * cohesion_force);
    }
    outputs[gl_WorkGroupID.x] = resultant;
}
