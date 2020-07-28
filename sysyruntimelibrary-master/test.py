import os
from typing import List

BASE = os.getcwd()

PATH = [
    "functional_test",
    "performance_test",
    "../cmake-build-debug-wsl/compiler_2020"
]


PATH_name = [
    "功能测试路径",
    "性能测试路径",
    "可执行文件路径"
]

print("\n"+"="*90)
print("当前环境：")
for index, name in enumerate(PATH_name):
    PATH[index] = os.path.abspath(PATH[index])
    print("{}:{}".format(name, PATH[index]))
print("="*90+"\n")


if_default = input("是否使用默认路径？(Y/n):")
if if_default:
    for index, name in enumerate(PATH_name):
        PATH[index] = input("{}:".format(name))


command = "{} -S -o {} {} -O2 >{} 2>&1"
executable = os.path.abspath(PATH[2])


def tester(base, in_files):
    total = len(in_files)
    for cnt, in_file in enumerate(in_files):
        prefix = in_file.split('.')[0]
        in_file = os.path.join(base, in_file)
        s_file = os.path.join(base, prefix+".s")
        err_file = os.path.join(base, prefix+".err")
        exe = command.format(executable, s_file, in_file, err_file)
        print("{}/{}:{}".format(cnt+1, total, exe))
        os.system(exe)
        yield s_file


function_test_files = os.listdir(os.path.abspath(PATH[0]))
function_test_in = list(filter(lambda x: x.endswith(".sy"), function_test_files))
function_test_out = list(filter(lambda x: x.endswith(".out"), function_test_files))
function_test_sFile = list(tester(PATH[0], function_test_in))


performance_test_files = os.listdir(os.path.abspath(PATH[1]))
performance_test_in = list(filter(lambda x: x.endswith(".sy"), performance_test_files))
performance_test_out = list(filter(lambda x: x.endswith(".out"), performance_test_files))
performance_test_sFile = list(tester(PATH[1], performance_test_in))

if_run = input("汇编并运行？(Y/n):")
if if_run:
    exit()

assem = "gcc -o {{}} {{}} {}".format(os.path.join(BASE, "libsys.a"))


def assem_proc(file_list: List[str]):
    total = len(file_list)
    for cnt, file in enumerate(file_list):
        outfile = file.split('/')[-1].split('.')[0]
        exe = assem.format(outfile, file)
        print("{}/{}: {}".format(cnt+1, total, exe))
        # os.system(exe)
        yield outfile


function_test_exe = list(assem_proc(function_test_sFile))
performance_test_exe = list(assem_proc(performance_test_sFile))

run = "./{}"
out = "echo $? > {}.out"


def run_proc(file_list):
    total = len(file_list)
    for cnt, file in enumerate(file_list):
        exe = run.format(file)
        print("{}/{}: {}".format(cnt+1, total, exe))
        # os.system(exe)
        exe = out.format(file)
        print(exe)
        # os.system(exe)


run_proc(function_test_exe)
run_proc(performance_test_exe)