def parse_enums(file_path):

    def align_comment(todolist, chr, blanks):
        max_key_length = max(len(line.split(chr)[0].strip()) for line in todolist)

        enum_val_aligned_list = []
        for line in todolist:
            if chr in line:
                key, value = line.split(chr, 1)
                enum_val_aligned_list.append(f"{key.strip().ljust(max_key_length)}{blanks}{chr} {value.strip()}")
            else:
                enum_val_aligned_list.append(line)

        return enum_val_aligned_list

    enums_dict = {}
    current_enum = None

    with open(file_path, 'r') as file:
        lines = file.readlines()

    for line in lines:
        line = line.strip()
        if line == "":
            current_enum = None
            continue

        if '=' in line and 'AX_S32' in line:
            enum_name = line.split('=')[0].strip()
            if '_E' in enum_name:
                current_enum = enum_name
                enums_dict[current_enum] = ""
        elif current_enum is not None:
            enums_dict[current_enum] += line + '\n'

    formatted_output = {}
    for enum_name, values in enums_dict.items():
        output_str = f'"""\n    .. _target to {enum_name.lower()}:\n\n'
        output_str += f'    {enum_name.removesuffix("_E").lower().replace("ax_", "").replace("_", " ")}\n\n'
        output_str += '    .. parsed-literal::\n\n'

        enum_val_list = []
        for value in values.strip().splitlines():
            enum_val_list.append(value)

        enum_val_aligned_list = align_comment(enum_val_list, '=', ' ')
        enum_val_aligned_list = align_comment(enum_val_aligned_list, '#', '    ')

        for enum_val_aligned in enum_val_aligned_list:
            output_str += f'        {enum_val_aligned}\n'

        output_str += '"""'
        formatted_output[enum_name] = output_str

    return formatted_output


def enum_insert_values_to_file(file_path, enum_comment_dict):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    for key, value in enum_comment_dict.items():
        for i, line in enumerate(lines):
            if key in line:
                lines.insert(i + 1, f"{value}\n")
                break

    with open(file_path, 'w') as file:
        file.writelines(lines)