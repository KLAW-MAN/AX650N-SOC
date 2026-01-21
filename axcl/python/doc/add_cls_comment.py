import ast


class ClassVisitor(ast.NodeVisitor):

    def __init__(self, code):
        self.structures = {}
        self.code = code

    def visit_ClassDef(self, node):
        class_name = node.name
        self.structures[class_name] = {
            'fields': {},
            'field_aliases': {}
        }

        for item in node.body:
            if isinstance(item, ast.Assign):
                for target in item.targets:
                    if isinstance(target, ast.Name) and target.id == '_fields_':
                        for field in item.value.elts:
                            if isinstance(field, ast.Tuple) and len(field.elts) == 2:
                                field_name = ast.get_source_segment(self.code, field.elts[0]).strip('\"')
                                field_type = ast.get_source_segment(self.code, field.elts[1])
                                self.structures[class_name]['fields'][field_name] = field_type
                    elif isinstance(target, ast.Name) and target.id == 'field_aliases':
                        for key_value in item.value.keys:
                            key = ast.get_source_segment(self.code, key_value).strip('\"')
                            value = ast.get_source_segment(self.code, item.value.values[item.value.keys.index(key_value)]).strip('\"')
                            self.structures[class_name]['field_aliases'][key] = value


def parse_file(file_path, module_name_dict):

    def get_class_full_name(class_name: str, module_name_dict):
        for key, module in module_name_dict.items():
            try:
                class_obj = getattr(module, class_name)
                return f"{class_obj.__module__}.{class_obj.__name__}"
            except AttributeError:
                continue
        return class_name

    BLANK_4 = "    "

    _type_str_list = [
        "POINTER(AX_U64)",
        "POINTER(AX_U32)",
        "POINTER(AX_U16)",
        "POINTER(AX_U8)",
        "POINTER(AX_S64)",
        "POINTER(AX_S32)",
        "POINTER(AX_S16)",
        "POINTER(AX_S8)",
        "POINTER(AX_CHAR)",
        "POINTER(AX_LONG)",
        "POINTER(AX_ULONG)",
        "POINTER(AX_ADDR)",
        "POINTER(AX_F32)",
        "POINTER(AX_F64)",
        "c_void_p",
        "AX_U64",
        "AX_U32",
        "AX_U16",
        "AX_U8",
        "AX_S64",
        "AX_S32",
        "AX_S16",
        "AX_S8",
        "AX_CHAR",
        "AX_LONG",
        "AX_ULONG",
        "AX_ADDR",
        "AX_F32",
        "AX_F64"
    ]

    with open(file_path, 'r', encoding='utf-8') as file:
        code = file.read()
    tree = ast.parse(code)
    visitor = ClassVisitor(code)
    visitor.visit(tree)

    cls_comment_dict = {}
    for struct_name, struct_info in visitor.structures.items():
        fields = struct_info['fields']
        field_aliases_mapping = struct_info['field_aliases']
        if len(fields) != len(field_aliases_mapping):
            print(f'WARN: Please check "_fields_" and "field_aliases" of class {struct_name}')

        param_context = ""
        for idx, (field_name, field_type) in enumerate(fields.items()):
            count = field_type.count('*')
            lsb = "[" * count
            rsb = "]" * count
            type_name = field_type.split('*', 1)[0].strip(' ')
            if type_name in _type_str_list:
                type_name = "int"
            elif type_name == "AX_BOOL":
                type_name = "bool"
            elif type_name.endswith("_E"):
                type_name = f":ref:`{type_name} <target to {type_name.lower()}>`"
            else:
                full_class_name = get_class_full_name(type_name, module_name_dict)
                type_name = f":class:`{type_name} <{full_class_name}>`"

            comma = "," if idx < len(fields) - 1 else ""
            enter = "\n" if idx < len(fields) - 1 else ""

            param_context = param_context + f'{BLANK_4*3}"{field_aliases_mapping.get(field_name, "ERROR_ALIASES")}": {lsb}{type_name}{rsb}{comma}{enter}'

        comment = (
            f'{BLANK_4}"""'
            # f'\n{BLANK_4}dict for {struct_name}'
            f'\n{BLANK_4}.. parsed-literal::'
            f'\n\n{BLANK_4}{BLANK_4}dict_{struct_name.replace("AX_", "").replace("_T", "").lower()} = {{'
            f'\n{param_context}'
            f'\n{BLANK_4}{BLANK_4}}}'
            f'\n{BLANK_4}"""'
        )
        cls_comment_dict[struct_name] = comment

    return cls_comment_dict


def class_insert_values_to_file(file_path, class_comment_dict):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    for class_name, class_comment in class_comment_dict.items():
        for i, line in enumerate(lines):
            if f' {class_name}(' in line:
                lines.insert(i + 1, f"{class_comment}\n")
                break

    with open(file_path, 'w') as file:
        file.writelines(lines)