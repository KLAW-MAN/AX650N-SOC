import ast
import re

def parse_p_api(func_api: str) -> dict:
    match = re.search(r'def\s+(\w+)\s*\((.*?)\)\s*->\s*(.*?):', func_api)
    if not match:
        return {}

    function_name = match.group(1).strip()
    params_string = match.group(2).strip()
    return_type = match.group(3).strip()

    param_list = []
    for param in params_string.split(','):
        param = param.strip()
        if ':' in param:
            param_name, param_type = param.split(':', 1)
            param_list.append((param_name.strip(), param_type.strip()))
        else:
            param_list.append((param, 'unknown'))

    return {
        'function_name': function_name,
        'return_type': return_type,
        'parameters': {param[0]: param[1] for param in param_list}
    }


def parse_c_api(c_api: str) -> dict:
    match = re.search(r'c api:\s*(.*)', f"c api: {c_api}", re.IGNORECASE)
    if not match:
        return {}

    c_api_api = match.group(1).strip()

    c_api_params_match = re.search(r'(\w+)\s+(\w+)\s*\((.*?)\)', c_api_api)
    if not c_api_params_match:
        return {}

    return_type = c_api_params_match.group(1).strip()
    function_name = c_api_params_match.group(2).strip()
    c_api_param_string = c_api_params_match.group(3).strip()

    c_api_param_list = []
    for param in c_api_param_string.split(','):
        param = param.strip()
        if ' ' in param:
            param_type, param_name = param.rsplit(' ', 1)
            c_api_param_list.append((param_name, param_type))
        else:
            c_api_param_list.append((param, 'unknown'))

    return {
        'function_name': function_name,
        'return_type': return_type,
        'parameters': {param[0]: param[1] for param in c_api_param_list}
    }

def make_api_comment(c_api, c_dict, p_api, p_dict, module_name_dict):

    def get_class_full_name(class_name: str, module_name_dict):
        for key, module in module_name_dict.items():
            try:
                class_obj = getattr(module, class_name)
                return class_obj.__module__ + '.' + class_obj.__name__
            except AttributeError:
                pass
        return class_name

    BLANK_4 = "    "

    # api_comment = ""
    # c_func = c_dict["function_name"]
    # c_ret = c_dict["return_type"]
    c_param = c_dict["parameters"]
    c_param_name_list = list(c_param.keys())

    p_func = p_dict["function_name"]
    p_ret = p_dict["return_type"]
    p_param = p_dict["parameters"]
    p_param_name_list = list(p_param.keys())

    if len(c_param) != len(p_param):
        print(f'WARN: function "{p_func}" param not match')

    sample = f"{p_ret.replace('tuple[', '').replace(']', '').replace('int', 'ret')} = {p_func}({', '.join(p_param)})"

    desc_comment = f"{BLANK_4}{p_func.replace('_', ' ')}\n"

    c_api_comment = f"{BLANK_4}.. table::\n\n"
    c_api_comment += f"{BLANK_4*2}======================= =====================================================\n"
    c_api_comment += f"{BLANK_4*2}**Language**            **Function Prototype**\n"
    c_api_comment += f"{BLANK_4*2}======================= =====================================================\n"
    c_api_comment += f"{BLANK_4*2}**C**                   `{c_api}`\n"
    c_api_comment += f"{BLANK_4*2}**python**              `{sample}`\n"
    c_api_comment += f"{BLANK_4*2}======================= =====================================================\n"

    param_ret = ""
    for idx, c_param_name in enumerate(c_param_name_list):
        c_param_type = c_param.get(c_param_name, "CTYPE_ERR")
        if c_param_type == "AX_VOID":
            continue

        if idx >= len(p_param_name_list):
            pass
        else:
            p_param_name = p_param_name_list[idx]
            p_param_type = p_param.get(p_param_name, "PTYPE_ERR")
            if "unknown" == p_param_type:
                continue

            py_c_class_name = c_param_type.replace('const ', '')
            if p_param_type == 'int':
                param_ret = param_ret + f"{BLANK_4}:param {p_param_type} {p_param_name}: {p_param_name.replace('_', ' ')}" + "\n"
            else:
                py_c_class_name = c_param_type.replace('const ', '')
                full_class_name = get_class_full_name(py_c_class_name, module_name_dict)
                param_ret = param_ret + f"{BLANK_4}:param {p_param_type} {p_param_name}: {p_param_name.replace('_', ' ')}, see :class:`{full_class_name}`" + "\n"

    if p_ret == "int":
        param_ret = param_ret + f"{BLANK_4}:returns: **ret** (*int*) - 0 indicates success, otherwise failure" + "\n"
    else:
        param_ret = param_ret + f'{BLANK_4}:returns: {p_ret}' + "\n\n"
        param_ret = param_ret + f"{BLANK_4*2}- **DICT** (*dict*) - see :class:`TODO`" + "\n"
        param_ret = param_ret + f"{BLANK_4*2}- **ret** (*int*) - 0 indicates success, otherwise failure." + "\n"
    param_ret_comment = param_ret

    sample_comment = f'{BLANK_4}**Example**\n\n'
    sample_comment += f"{BLANK_4}.. code-block:: python\n\n"
    sample_comment += f"{BLANK_4*2}{sample}\n\n"

    # desc, c_api, param_ret, sample
    api_comment = f'{BLANK_4}"""\n{desc_comment}\n{c_api_comment}\n{param_ret_comment}\n{sample_comment}{BLANK_4}"""\n'
    return api_comment


def parse_py_file(file_path, module_name_dict):
    def type_annotation(annotation):
        if annotation:
            return annotation
        return 'Any'

    with open(file_path, 'r', encoding='utf-8') as file:
        file_content = file.read()

    dict_func_comment = {}
    tree = ast.parse(file_content)
    for node in ast.walk(tree):
        if isinstance(node, ast.FunctionDef):
            func_name = node.name

            params = ', '.join(
                f"{arg.arg}: {type_annotation(ast.get_source_segment(file_content, arg.annotation) if arg.annotation else '')}"
                for arg in node.args.args
            )

            return_type = ast.get_source_segment(file_content, node.returns) if node.returns else 'None'
            docstring = ast.get_docstring(node)

            c_api = ''
            if docstring:
                c_api_lines = []
                in_c_api_section = False

                for line in docstring.splitlines():
                    line = line.strip()
                    if 'c api:' in line:
                        start_index = line.index('c api:') + len('c api:')
                        c_api_content = line[start_index:].strip()
                        c_api_lines.append(c_api_content)
                        in_c_api_section = True
                    elif in_c_api_section:
                        if line:
                            c_api_lines.append(line.strip())
                        else:
                            break
                if c_api_lines:
                    c_api = ' '.join(c_api_lines)
                    c_api = re.sub(r'\s+', ' ', c_api).strip()
                    if not c_api.endswith(';'):
                        c_api += ';'

            if c_api:
                p_api = f"def {func_name}({params}) -> {return_type}:"

                c_dict = parse_c_api(c_api)
                p_dict = parse_p_api(p_api)
                dict_func_comment[p_dict["function_name"]] = make_api_comment(c_api, c_dict, p_api, p_dict, module_name_dict)

    return dict_func_comment


def update_method_comments(api_path: str, dict_func_comment: dict):
    with open(api_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    for function_name, new_comment in dict_func_comment.items():
        for i, line in enumerate(lines):
            if f" {function_name}(" in line:
                lines.insert(i + 1, new_comment)
                j = i + 2
                found_quotes = 0
                while j < len(lines):
                    if '"""' in lines[j]:
                        found_quotes += 1
                        if found_quotes == 2:
                            del lines[j]
                            break
                    del lines[j]
                break

    with open(api_path, 'w', encoding='utf-8') as file:
        file.writelines(lines)