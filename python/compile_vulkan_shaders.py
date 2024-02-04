import argparse
import os.path
import subprocess
import sys

fragment_shader_file_extension = "frag"
vertex_shader_file_extension = "vert"

output_file_extension = "hpp"


def bin_to_array(data, namespace, var_name) -> str:
    out = list()

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

    parsed_arguments.add_argument("-i", "--shader_in",
                                  help="Input shader directory.", required=True)

    parsed_arguments.add_argument("-o", "--header_out",
                                  help="Output shader directory.", required=True)

    parsed_arguments.add_argument("-n", "--namespace",
                                  help="Namespace the generated array belongs to", required=True)

    args = parsed_arguments.parse_args()

    input_dir = args.shader_in
    output_dir = args.header_out

    if not os.path.isdir(input_dir):
        print(f'Input {input_dir} is not a directory', file=sys.stderr)
        exit(1)

    if not os.path.isdir(output_dir):
        print(f'Output {output_dir} is not a directory', file=sys.stderr)
        exit(1)

    for file in os.listdir(input_dir):
        file_name = os.path.splitext(os.path.basename(file))

        # make sure it's a frag or vert file
        is_vertex_shader = file_name[1] == f".{vertex_shader_file_extension}"
        is_fragment_shader = file_name[1] == f".{fragment_shader_file_extension}"

        if not is_fragment_shader and not is_vertex_shader:
            continue

        generated_file_name = f"{file_name[0]}_{vertex_shader_file_extension if is_vertex_shader else fragment_shader_file_extension}"

        compiled_shader_path = f'{output_dir}/{generated_file_name}.spv'
        parsed_shader_path = f'{output_dir}/{generated_file_name}.{output_file_extension}'

        # compile each fragment/vertex shader
        compile_shader_arg = ["glslc", f'{input_dir}/{file}', "-o", compiled_shader_path]

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
