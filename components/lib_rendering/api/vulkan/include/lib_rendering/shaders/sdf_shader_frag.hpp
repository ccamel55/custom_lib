#pragma once

namespace lib::rendering::vulkan::shaders
{
constexpr unsigned int sdf_shader_frag_length = 1268;
constexpr unsigned char sdf_shader_frag[] = {
   0x3, 0x2, 0x23, 0x7, 0x0, 0x0, 0x1, 0x0, 0xb, 0x0, 0xd, 0x0,
   0x2e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x11, 0x0, 0x2, 0x0,
   0x1, 0x0, 0x0, 0x0, 0xb, 0x0, 0x6, 0x0, 0x1, 0x0, 0x0, 0x0,
   0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
   0x0, 0x0, 0x0, 0x0, 0xe, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x1, 0x0, 0x0, 0x0, 0xf, 0x0, 0x8, 0x0, 0x4, 0x0, 0x0, 0x0,
   0x4, 0x0, 0x0, 0x0, 0x6d, 0x61, 0x69, 0x6e, 0x0, 0x0, 0x0, 0x0,
   0x11, 0x0, 0x0, 0x0, 0x27, 0x0, 0x0, 0x0, 0x2a, 0x0, 0x0, 0x0,
   0x10, 0x0, 0x3, 0x0, 0x4, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x3, 0x0, 0x3, 0x0, 0x2, 0x0, 0x0, 0x0, 0xc2, 0x1, 0x0, 0x0,
   0x4, 0x0, 0xa, 0x0, 0x47, 0x4c, 0x5f, 0x47, 0x4f, 0x4f, 0x47, 0x4c,
   0x45, 0x5f, 0x63, 0x70, 0x70, 0x5f, 0x73, 0x74, 0x79, 0x6c, 0x65, 0x5f,
   0x6c, 0x69, 0x6e, 0x65, 0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69,
   0x76, 0x65, 0x0, 0x0, 0x4, 0x0, 0x8, 0x0, 0x47, 0x4c, 0x5f, 0x47,
   0x4f, 0x4f, 0x47, 0x4c, 0x45, 0x5f, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64,
   0x65, 0x5f, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69, 0x76, 0x65, 0x0,
   0x5, 0x0, 0x4, 0x0, 0x4, 0x0, 0x0, 0x0, 0x6d, 0x61, 0x69, 0x6e,
   0x0, 0x0, 0x0, 0x0, 0x5, 0x0, 0x6, 0x0, 0x9, 0x0, 0x0, 0x0,
   0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x64, 0x5f, 0x74, 0x65, 0x78, 0x74,
   0x75, 0x72, 0x65, 0x0, 0x5, 0x0, 0x6, 0x0, 0xd, 0x0, 0x0, 0x0,
   0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x5f, 0x73, 0x61, 0x6d, 0x70,
   0x6c, 0x65, 0x72, 0x0, 0x5, 0x0, 0x5, 0x0, 0x11, 0x0, 0x0, 0x0,
   0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x5f, 0x75, 0x76, 0x0,
   0x5, 0x0, 0x6, 0x0, 0x15, 0x0, 0x0, 0x0, 0x6f, 0x75, 0x74, 0x6c,
   0x69, 0x6e, 0x65, 0x5f, 0x66, 0x61, 0x63, 0x74, 0x6f, 0x72, 0x0, 0x0,
   0x5, 0x0, 0x5, 0x0, 0x1d, 0x0, 0x0, 0x0, 0x73, 0x64, 0x66, 0x5f,
   0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x0, 0x5, 0x0, 0x5, 0x0,
   0x27, 0x0, 0x0, 0x0, 0x6f, 0x75, 0x74, 0x5f, 0x63, 0x6f, 0x6c, 0x6f,
   0x72, 0x0, 0x0, 0x0, 0x5, 0x0, 0x6, 0x0, 0x2a, 0x0, 0x0, 0x0,
   0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x5f, 0x63, 0x6f, 0x6c,
   0x6f, 0x72, 0x0, 0x0, 0x47, 0x0, 0x4, 0x0, 0xd, 0x0, 0x0, 0x0,
   0x22, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x47, 0x0, 0x4, 0x0,
   0xd, 0x0, 0x0, 0x0, 0x21, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x47, 0x0, 0x4, 0x0, 0x11, 0x0, 0x0, 0x0, 0x1e, 0x0, 0x0, 0x0,
   0x1, 0x0, 0x0, 0x0, 0x47, 0x0, 0x4, 0x0, 0x27, 0x0, 0x0, 0x0,
   0x1e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x47, 0x0, 0x4, 0x0,
   0x2a, 0x0, 0x0, 0x0, 0x1e, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x13, 0x0, 0x2, 0x0, 0x2, 0x0, 0x0, 0x0, 0x21, 0x0, 0x3, 0x0,
   0x3, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x16, 0x0, 0x3, 0x0,
   0x6, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x17, 0x0, 0x4, 0x0,
   0x7, 0x0, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0,
   0x20, 0x0, 0x4, 0x0, 0x8, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x7, 0x0, 0x0, 0x0, 0x19, 0x0, 0x9, 0x0, 0xa, 0x0, 0x0, 0x0,
   0x6, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0,
   0x0, 0x0, 0x0, 0x0, 0x1b, 0x0, 0x3, 0x0, 0xb, 0x0, 0x0, 0x0,
   0xa, 0x0, 0x0, 0x0, 0x20, 0x0, 0x4, 0x0, 0xc, 0x0, 0x0, 0x0,
   0x0, 0x0, 0x0, 0x0, 0xb, 0x0, 0x0, 0x0, 0x3b, 0x0, 0x4, 0x0,
   0xc, 0x0, 0x0, 0x0, 0xd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x17, 0x0, 0x4, 0x0, 0xf, 0x0, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0,
   0x2, 0x0, 0x0, 0x0, 0x20, 0x0, 0x4, 0x0, 0x10, 0x0, 0x0, 0x0,
   0x1, 0x0, 0x0, 0x0, 0xf, 0x0, 0x0, 0x0, 0x3b, 0x0, 0x4, 0x0,
   0x10, 0x0, 0x0, 0x0, 0x11, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0,
   0x20, 0x0, 0x4, 0x0, 0x14, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x6, 0x0, 0x0, 0x0, 0x2b, 0x0, 0x4, 0x0, 0x6, 0x0, 0x0, 0x0,
   0x16, 0x0, 0x0, 0x0, 0x0, 0x0, 0xe0, 0x3e, 0x2b, 0x0, 0x4, 0x0,
   0x6, 0x0, 0x0, 0x0, 0x17, 0x0, 0x0, 0x0, 0x0, 0x0, 0x10, 0x3f,
   0x15, 0x0, 0x4, 0x0, 0x18, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0,
   0x0, 0x0, 0x0, 0x0, 0x2b, 0x0, 0x4, 0x0, 0x18, 0x0, 0x0, 0x0,
   0x19, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x17, 0x0, 0x4, 0x0,
   0x1e, 0x0, 0x0, 0x0, 0x6, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0,
   0x20, 0x0, 0x4, 0x0, 0x26, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0,
   0x7, 0x0, 0x0, 0x0, 0x3b, 0x0, 0x4, 0x0, 0x26, 0x0, 0x0, 0x0,
   0x27, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0, 0x0, 0x20, 0x0, 0x4, 0x0,
   0x29, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x3b, 0x0, 0x4, 0x0, 0x29, 0x0, 0x0, 0x0, 0x2a, 0x0, 0x0, 0x0,
   0x1, 0x0, 0x0, 0x0, 0x2b, 0x0, 0x4, 0x0, 0x6, 0x0, 0x0, 0x0,
   0x2d, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x3d, 0x36, 0x0, 0x5, 0x0,
   0x2, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x3, 0x0, 0x0, 0x0, 0xf8, 0x0, 0x2, 0x0, 0x5, 0x0, 0x0, 0x0,
   0x3b, 0x0, 0x4, 0x0, 0x8, 0x0, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0,
   0x7, 0x0, 0x0, 0x0, 0x3b, 0x0, 0x4, 0x0, 0x14, 0x0, 0x0, 0x0,
   0x15, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0, 0x3b, 0x0, 0x4, 0x0,
   0x8, 0x0, 0x0, 0x0, 0x1d, 0x0, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x3d, 0x0, 0x4, 0x0, 0xb, 0x0, 0x0, 0x0, 0xe, 0x0, 0x0, 0x0,
   0xd, 0x0, 0x0, 0x0, 0x3d, 0x0, 0x4, 0x0, 0xf, 0x0, 0x0, 0x0,
   0x12, 0x0, 0x0, 0x0, 0x11, 0x0, 0x0, 0x0, 0x57, 0x0, 0x5, 0x0,
   0x7, 0x0, 0x0, 0x0, 0x13, 0x0, 0x0, 0x0, 0xe, 0x0, 0x0, 0x0,
   0x12, 0x0, 0x0, 0x0, 0x3e, 0x0, 0x3, 0x0, 0x9, 0x0, 0x0, 0x0,
   0x13, 0x0, 0x0, 0x0, 0x41, 0x0, 0x5, 0x0, 0x14, 0x0, 0x0, 0x0,
   0x1a, 0x0, 0x0, 0x0, 0x9, 0x0, 0x0, 0x0, 0x19, 0x0, 0x0, 0x0,
   0x3d, 0x0, 0x4, 0x0, 0x6, 0x0, 0x0, 0x0, 0x1b, 0x0, 0x0, 0x0,
   0x1a, 0x0, 0x0, 0x0, 0xc, 0x0, 0x8, 0x0, 0x6, 0x0, 0x0, 0x0,
   0x1c, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x31, 0x0, 0x0, 0x0,
   0x16, 0x0, 0x0, 0x0, 0x17, 0x0, 0x0, 0x0, 0x1b, 0x0, 0x0, 0x0,
   0x3e, 0x0, 0x3, 0x0, 0x15, 0x0, 0x0, 0x0, 0x1c, 0x0, 0x0, 0x0,
   0x3d, 0x0, 0x4, 0x0, 0x7, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x0, 0x0,
   0x9, 0x0, 0x0, 0x0, 0x4f, 0x0, 0x8, 0x0, 0x1e, 0x0, 0x0, 0x0,
   0x20, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x0, 0x0, 0x1f, 0x0, 0x0, 0x0,
   0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0,
   0x3d, 0x0, 0x4, 0x0, 0x6, 0x0, 0x0, 0x0, 0x21, 0x0, 0x0, 0x0,
   0x15, 0x0, 0x0, 0x0, 0x51, 0x0, 0x5, 0x0, 0x6, 0x0, 0x0, 0x0,
   0x22, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
   0x51, 0x0, 0x5, 0x0, 0x6, 0x0, 0x0, 0x0, 0x23, 0x0, 0x0, 0x0,
   0x20, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x51, 0x0, 0x5, 0x0,
   0x6, 0x0, 0x0, 0x0, 0x24, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0,
   0x2, 0x0, 0x0, 0x0, 0x50, 0x0, 0x7, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x25, 0x0, 0x0, 0x0, 0x22, 0x0, 0x0, 0x0, 0x23, 0x0, 0x0, 0x0,
   0x24, 0x0, 0x0, 0x0, 0x21, 0x0, 0x0, 0x0, 0x3e, 0x0, 0x3, 0x0,
   0x1d, 0x0, 0x0, 0x0, 0x25, 0x0, 0x0, 0x0, 0x3d, 0x0, 0x4, 0x0,
   0x7, 0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x1d, 0x0, 0x0, 0x0,
   0x3d, 0x0, 0x4, 0x0, 0x7, 0x0, 0x0, 0x0, 0x2b, 0x0, 0x0, 0x0,
   0x2a, 0x0, 0x0, 0x0, 0x85, 0x0, 0x5, 0x0, 0x7, 0x0, 0x0, 0x0,
   0x2c, 0x0, 0x0, 0x0, 0x28, 0x0, 0x0, 0x0, 0x2b, 0x0, 0x0, 0x0,
   0x3e, 0x0, 0x3, 0x0, 0x27, 0x0, 0x0, 0x0, 0x2c, 0x0, 0x0, 0x0,
   0xfd, 0x0, 0x1, 0x0, 0x38, 0x0, 0x1, 0x0
};
}