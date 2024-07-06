import argparse
import os.path
import subprocess
import sys

input_file_extension = "hlsl"

pixel_shader_file_extension = "ps"
vertex_shader_file_extension = "vs"

output_file_extension = "hpp"


def bin_to_array(data, namespace, var_name) -> str:
    out = list()

    # print warning message
    out.append("/* \n"
               "    This file has been generated automatically by compile_vulkan_shader.py\n"
               "    Do not modify this file directly.\n"
               "*/")

    # define namespace
    out.append("#pragma once")
    out.append("")
    out.append(f'namespace {namespace}')
    out.append('{')

    # size of array and array
    out.append(f'constexpr unsigned int {var_name}_length = {len(data)};')
    out.append(f'constexpr unsigned char {var_name}[] = {{')

    current_line = [data[i:i + 12] for i in range(0, len(data), 12)]

    for i, x in enumerate(current_line):
        line = ', '.join([hex(c) for c in x])
        out.append(f'   {line}{"," if i < len(current_line) - 1 else ""}')

    # closing bracket
    out.append('};')

    # namespace closing bracket
    out.append('}')

    return '\n'.join(out)


def main():
    parsed_arguments = argparse.ArgumentParser()

    parsed_arguments.add_argument("-i", "--input_directory",
                                  help="Input directory.", required=True)

    parsed_arguments.add_argument("-o", "--output_directory",
                                  help="Output directory.", required=True)

    parsed_arguments.add_argument("-n", "--namespace",
                                  help="Namespace the generated string belongs to", required=True)

    parsed_arguments.add_argument("-e", "--dxc",
                                  help="Path to DXC executable", required=True)

    parsed_arguments.add_argument("-vk", "--vulkan",
                                  help="Generate shaders with vulkan support", action='store_true')

    args = parsed_arguments.parse_args()

    input_dir = args.input_directory
    output_dir = args.output_directory

    if not os.path.isdir(input_dir):
        print(f'Input {input_dir} is not a directory', file=sys.stderr)
        exit(1)

    if not os.path.isdir(output_dir):
        print(f'Output {output_dir} is not a directory', file=sys.stderr)
        exit(1)

    for file in os.listdir(input_dir):
        file_name = file.split(".")

        if file_name[-1] != input_file_extension:
            continue

        # make sure it's a frag or vert file
        is_vertex_shader = file_name[-2] == vertex_shader_file_extension
        is_pixel_shader = file_name[-2] == pixel_shader_file_extension

        if not is_pixel_shader and not is_vertex_shader:
            continue

        generated_file_name = f"{file_name[0]}_{file_name[-2]}"

        compiled_shader_path = f'{output_dir}/{generated_file_name}.spv'
        parsed_shader_path = f'{output_dir}/{generated_file_name}.{output_file_extension}'

        if os.path.exists(parsed_shader_path):
            os.remove(parsed_shader_path)

        # compile each pixel/vertex shader (entry point of main)
        compile_shader_arg = [args.dxc, "-spirv", "-E", "main", f'{input_dir}/{file}', "-Fo", compiled_shader_path]

        if args.vulkan:
            for i in range(10):
                compile_shader_arg += ["-fvk-t-shift", "0", str(i)]
                compile_shader_arg += ["-fvk-s-shift", "128", str(i)]
                compile_shader_arg += ["-fvk-b-shift", "256", str(i)]
                compile_shader_arg += ["-fvk-u-shift", "384", str(i)]

        if is_pixel_shader:
            compile_shader_arg.append("-T")
            compile_shader_arg.append("ps_6_1")
        else:
            compile_shader_arg.append("-T")
            compile_shader_arg.append("vs_6_1")

        try:
            subprocess.check_output(compile_shader_arg)
        except subprocess.CalledProcessError as e:
            print(e.output, file=sys.stderr)
            exit(-1)

        # write the binary to a hpp file
        input_file_handle = open(compiled_shader_path, "rb")
        write_string = bin_to_array(input_file_handle.read(), args.namespace, generated_file_name)

        input_file_handle.close()

        output_file_handle = open(parsed_shader_path, "w")
        output_file_handle.write(write_string)

        output_file_handle.close()

    return


if __name__ == '__main__':
    main()
