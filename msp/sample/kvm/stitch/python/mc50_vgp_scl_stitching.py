import math
import sys
import json
from enum import Enum
from loguru import logger


class ALIGN(Enum):
    FLOOR = 0
    CEILING = 1
    ROUND = 2


def align(var: int, align_num: int, align_method=ALIGN.FLOOR) -> int:
    if (var % align_num) != 0:
        if align_method == ALIGN.FLOOR:
            k = math.floor(var / align_num)
        elif align_method == ALIGN.CEILING:
            k = math.ceil(var / align_num)
        else:
            k = round(var / align_num)
        return k * align_num
    else:
        return var


input_config_fn = sys.argv[1]
output_config_fn = sys.argv[2]

with open(input_config_fn, 'r') as ifs:
    config = json.load(ifs)

source_total_width = config['source_total_width']
partition_num = config['partition_num']
scale_f_bits = config['scl_reg_f_bits']
disp_width = []
disp_width_sum = 0
for n in range(partition_num):
    disp_width.append(config[f'disp_width_par{n}'])
    disp_width_sum += disp_width[n]
width_align = config['width_align']

output_config = {}
for n in range(partition_num):
    output_config[f'partition{n}'] = {}

scale = source_total_width / disp_width_sum
scale = math.floor(scale*2**scale_f_bits)/2**scale_f_bits

# partition 0
crop_left = 0
offset = 0
crop_right = 0
output_width = disp_width[0] + crop_left
output_width_aligned = align(output_width, width_align, ALIGN.CEILING)
crop_right = output_width_aligned - output_width
while crop_right % width_align != 0:
    output_width_aligned += width_align
    crop_right = output_width_aligned - output_width
source_width_end = math.floor((output_width - 1 + 1 + 0.5) * scale - 0.5) + offset
source_width = math.floor((output_width_aligned - 1 + 0.5) * scale - 0.5) + 1 + 1
source_width_aligned = align(source_width, width_align, ALIGN.CEILING)
output_config[f'partition{0}']['ori_crop_start'] = offset
output_config[f'partition{0}']['ori_crop_length'] = source_width_aligned
output_config[f'partition{0}']['output_length'] = output_width_aligned
output_config[f'partition{0}']['output_crop_on_left'] = crop_left
output_config[f'partition{0}']['output_crop_on_right'] = crop_right
output_config[f'partition{0}']['output_right_end'] = source_width_end
output_config[f'partition{0}']['scale'] = scale

# partition 1 ~ n-2
for n in range(1, partition_num - 1):
    extra_left = 0
    crop_left = math.ceil((extra_left + 0.5) / scale - 0.5)
    offset = output_config[f'partition{n - 1}']['output_right_end'] - extra_left
    if (1 / scale) % 2 != 0:
        while crop_left % width_align != 0 or offset % width_align != 0:
            extra_left += 1
            offset -= 1
            crop_left = math.ceil((extra_left + 0.5) / scale - 0.5)
    else:
        if offset % width_align != 0:
            extra_left += 1
            offset -= 1
            crop_left = math.ceil((extra_left + 0.5) / scale - 0.5)
        if crop_left % width_align != 0:
            crop_left += 1
    crop_right = 0
    output_width = disp_width[n] + crop_left
    output_width_aligned = align(output_width, width_align, ALIGN.CEILING)
    crop_right = output_width_aligned - output_width
    while crop_right % width_align != 0:
        output_width_aligned += width_align
        crop_right = output_width_aligned - output_width
    source_width_end = math.floor((output_width - 1 + 1 + 0.5) * scale - 0.5) + offset
    source_width = math.floor((output_width_aligned - 1 + 0.5) * scale - 0.5) + 1 + 1
    source_width_aligned = align(source_width, width_align, ALIGN.CEILING)
    output_config[f'partition{n}']['ori_crop_start'] = offset
    output_config[f'partition{n}']['ori_crop_length'] = source_width_aligned
    output_config[f'partition{n}']['output_length'] = output_width_aligned
    output_config[f'partition{n}']['output_crop_on_left'] = crop_left
    output_config[f'partition{n}']['output_crop_on_right'] = crop_right
    output_config[f'partition{n}']['output_right_end'] = source_width_end
    output_config[f'partition{n}']['scale'] = scale

# partition n-1
extra_left = 0
crop_left = math.ceil((extra_left + 0.5) / scale - 0.5)
offset = output_config[f'partition{partition_num - 2}']['output_right_end'] - extra_left
if (1 / scale) % 2 != 0:
    while crop_left % width_align != 0 or offset % width_align != 0:
        extra_left += 1
        offset -= 1
        crop_left = math.ceil((extra_left + 0.5) / scale - 0.5)
else:
    if offset % width_align != 0:
        extra_left += 1
        offset -= 1
        crop_left = math.ceil((extra_left + 0.5) / scale - 0.5)
    if crop_left % width_align != 0:
        crop_left += 1
crop_right = 0
output_width = disp_width[partition_num - 1] + crop_left
if offset + (math.floor((output_width - 1 + 0.5) * scale - 0.5) + 1) > source_total_width:
    logger.error(f"There is NOT enough source pixel for last partition to output {disp_width[partition_num - 1]} pixels.")
    assert(0)
output_width_aligned = align(output_width, width_align, ALIGN.CEILING)
crop_right = output_width_aligned - output_width
while crop_right % width_align != 0:
    output_width_aligned += width_align
    crop_right = output_width_aligned - output_width
source_width_end = math.floor((output_width - 1 + 1 + 0.5) * scale - 0.5) + offset
source_width = math.floor((output_width_aligned - 1 + 0.5) * scale - 0.5) + 1 + 1
source_width_aligned = align(source_width, width_align, ALIGN.CEILING)
pad_num = max(offset + source_width_aligned - source_total_width, 0)
pad_num_aligned = align(pad_num, width_align, ALIGN.CEILING)

output_config['ori_pad_num_end'] = pad_num_aligned
output_config[f'partition{partition_num - 1}']['ori_crop_start'] = offset
output_config[f'partition{partition_num - 1}']['ori_crop_length'] = source_width_aligned
output_config[f'partition{partition_num - 1}']['output_length'] = output_width_aligned
output_config[f'partition{partition_num - 1}']['output_crop_on_left'] = crop_left
output_config[f'partition{partition_num - 1}']['output_crop_on_right'] = crop_right
output_config[f'partition{partition_num - 1}']['output_right_end'] = source_width_end
output_config[f'partition{partition_num - 1}']['scale'] = scale

with open(output_config_fn, 'w') as ifs:
    json.dump(output_config, ifs, indent=4)
