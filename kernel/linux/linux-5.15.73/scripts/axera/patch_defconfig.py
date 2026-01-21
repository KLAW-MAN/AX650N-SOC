#encoding=utf-8
import sys
import os

def usage():
    '''usage:
    {} in_defconfig1 in_defconfig2 ... out_kernel_defconfig
    '''.format(sys.argv[0])

def patch_defconfig(srcList, defconfig):
    if not os.path.exists(defconfig):
        print("[error] cannot find deconfig file: {}".format(defconfig))
        return False

    in_lines = []
    for src in srcList:
        if not os.path.exists(src):
            print("[error] cannot find deconfig file: {}".format(src))
            return False
        with open(src, "rt") as fIn:
            fIn_lines = fIn.readlines()
        for line in fIn_lines:
            line = line.lstrip()
            if line and line[0] != '#':
                if line in in_lines:
                    print("[warning] found duplicated config [{}] in {}, ignore!".format(line, src))
                else:
                    in_lines.append(line)

    with open(defconfig, "rt") as fOut:
        fOut_lines = fOut.readlines()

    append_lines=[]
    for line_in in in_lines:
        config=line_in.strip().split("=")[0]
        match_line = "# {} is not set".format(config)
        bFound = False
        idx = -1
        for line_out in fOut_lines:
            idx = idx + 1
            if line_out.find(match_line) != -1:
                bFound = True
                fOut_lines[idx] = line_in
        if not bFound:
            append_lines.append(line_in)

    with open(defconfig, "wt") as fOut:
        fOut.writelines(fOut_lines)
        fOut.writelines(append_lines)

    print("patch {} done!".format(defconfig))

if __name__ == "__main__":
    args_count = len(sys.argv)
    if args_count < 3:
        usage()
        sys.exit(0)
    src = sys.argv[1:args_count-1]
    dst = sys.argv[-1]
    patch_defconfig(src, dst)
