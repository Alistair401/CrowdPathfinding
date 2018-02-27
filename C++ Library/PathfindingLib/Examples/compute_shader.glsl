#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430) buffer Units
{ 
    vec4 units[];
};

layout (std430) buffer Counts
{ 
    int counts[];
};

layout (std430) buffer Nearby
{ 
    vec4 nearby[];
};

void main(){
    units[0].x = 4.0;
    units[0].z = 1.0;
}
