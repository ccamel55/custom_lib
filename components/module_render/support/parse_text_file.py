import argparse
import os.path
import sys

output_file_extension = "hpp"


def create_header_file(file, namespace, var_name) -> str:
    out = list()

    # print warning message
    out.append("/* \n"
               "    This file has been generated automatically by parse_text_file.py\n"
               "    Do not modify this file directly.\n"
               "*/")

    # define namespace
    out.append("#pragma once")
    out.append("")
    out.append(f'namespace {namespace}')
    out.append('{')

    # define raw string
    out.append(f"constexpr char {var_name}[] = R\"(")

    # print line for line from the opened file
    for line in file.readlines():
        out.append(line.rstrip('\n'))

    # closing bracket
    out.append(")\";")

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
        file_name = os.path.splitext(os.path.basename(file))

        generated_file_name = f"{file_name[0]}_{file_name[1].lstrip('.')}"
        parsed_file_path = f'{output_dir}/{generated_file_name}.{output_file_extension}'

        if os.path.exists(parsed_file_path):
            os.remove(parsed_file_path)

        # write the binary to a hpp file
        input_file_handle = open(f"{input_dir}/{file}", "r")
        write_string = create_header_file(input_file_handle, args.namespace, generated_file_name)

        input_file_handle.close()

        output_file_handle = open(parsed_file_path, "w")
        output_file_handle.write(write_string)

        output_file_handle.close()

    return


if __name__ == '__main__':
    main()
