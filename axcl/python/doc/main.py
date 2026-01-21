import os
import sys
from add_cls_comment import *
from add_enum_comment import *
from add_api_comment import *

os.environ['AXCL_LIB_PATH'] = os.path.abspath('../../out/axcl_linux_x86/lib')

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(BASE_DIR+'./..')
sys.path.append(BASE_DIR+'/..')
sys.path.append(BASE_DIR+'/../..')

import axcl
from axcl.utils.axcl_utils import *
from axcl.ivps.axcl_ivps_type import *
from axcl.ivps.axcl_ivps import *


if __name__ == '__main__':
    import importlib
    # import lib
    module_name_dict = {
        'axcl.ax_base_type': None,
        'axcl.ax_global_type': None,
        'axcl.ivps.axcl_ivps_type': None,
        'axcl.ivps.axcl_ivps': None
    }
    for key in module_name_dict.keys():
        module_name_dict[key] = importlib.import_module(key)

    # # global
    # global_type_file = '../axcl/ax_global_type.py'
    # # enum
    # enum_comment_dict = parse_enums(global_type_file)
    # enum_insert_values_to_file(global_type_file, enum_comment_dict)
    # # structure
    # class_comment_dict = parse_file(global_type_file, module_name_dict)
    # class_insert_values_to_file(global_type_file, class_comment_dict)

    # sys_type_file = '../axcl/sys/axcl_sys_type.py'
    # # sys
    # # enum
    # enum_comment_dict = parse_enums(sys_type_file)
    # enum_insert_values_to_file(sys_type_file, enum_comment_dict)

    # ivps
    ivps_api_file = '../axcl/ivps/axcl_ivps.py'
    ivps_type_file = '../axcl/ivps/axcl_ivps_type.py'
    # enum
    enum_comment_dict = parse_enums(ivps_type_file)
    enum_insert_values_to_file(ivps_type_file, enum_comment_dict)
    # structure
    class_comment_dict = parse_file(ivps_type_file, module_name_dict)
    class_insert_values_to_file(ivps_type_file, class_comment_dict)
    # api
    dict_func_comment = parse_py_file(ivps_api_file, module_name_dict)
    update_method_comments(ivps_api_file, dict_func_comment)

