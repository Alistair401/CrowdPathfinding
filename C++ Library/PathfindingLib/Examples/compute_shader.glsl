#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Unit
{
    vec3 position;
    vec3 target;
    uint neighbor_count;
    uint neighbor_start_index;
};

struct Neighbor
{
    vec3 position;
    vec3 target;
};

struct Result
{
    vec3 force;
    uint request_path;
};

layout (std430) buffer unit_buffer
{ 
    Unit units[];
};

layout (std430) buffer neighbor_buffer
{ 
    Neighbor neighbors[];
};

layout (std430) buffer output_buffer
{
    Result outputs[];
};

const float cohesion = 0.06f;
const float separation = 1.0f;
const float following = 0.004f;
const float target_similarity_threshold = 50.0f;

void main(){
    Unit current = units[gl_WorkGroupID.x];
    vec3 resultant = vec3(0.0);
    uint path_required = 1;
    if (current.neighbor_count > 0)
    {
        vec3 separation_force = vec3(0.0);
        vec3 cohesion_force = vec3(0.0);
        vec3 following_force = vec3(0.0);

        bool found_leader = false;
        float leader_to_target_distance = 1.0/0.0;
        uint leader_index = 0;

        for (uint i = 0; i < current.neighbor_count; i++){
            uint neighbor_index = current.neighbor_start_index + i;
            Neighbor neighbor = neighbors[neighbor_index];

            vec3 separation = current.position - neighbor.position;
            vec3 separation_normalized = normalize(separation);

            separation_force += separation_normalized / length(separation);
            cohesion_force += neighbor.position;

            vec3 current_target_to_neighbor_target = neighbor.target - current.target;
            vec3 current_to_current_target = current.target - current.position;
            vec3 neighbor_to_current_target = current.target - neighbor.position;

            float neighbor_target_similarity = length(current_target_to_neighbor_target);
            float current_to_target_distance = length(current_to_current_target);
            float neighbor_to_target_distance = length(neighbor_to_current_target);

            if (neighbor_target_similarity < target_similarity_threshold 
            && neighbor_to_target_distance < current_to_target_distance
            && neighbor_to_target_distance < leader_to_target_distance)
            {
                found_leader = true;
                leader_index = neighbor_index;
                leader_to_target_distance = neighbor_to_target_distance;
            }
        }

        if (found_leader){
            path_required = 0;
            Neighbor leader = neighbors[leader_index];
            following_force = leader.position - current.position;
        }

        cohesion_force = normalize((cohesion_force / current.neighbor_count) - current.position);

        resultant = (separation * separation_force) + (cohesion * cohesion_force) + (following * following_force);
    }

    outputs[gl_WorkGroupID.x] = Result(resultant,path_required);
}
