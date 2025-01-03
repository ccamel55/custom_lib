#ifndef GEOMETRY_CB_H
#define GEOMETRY_CB_H

struct constant_buffer_t {
    float4x4 mvp_matrix;
    float4x4 model_matrix;
    float4x4 view_matrix;
    float4x4 projection_matrix;
};

#endif //GEOMETRY_CB_H
