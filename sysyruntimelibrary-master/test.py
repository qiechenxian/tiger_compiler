
import os

BASE = os.getcwd()

PATH = [
    "functional_test",
    "performance_test",
    "../cmake-build-debug-wsl/complier_2020"
]

PATH_name = [
    "功能测试路径",
    "性能测试路径",
    "可执行文件路径"
]

if_default = input("是否使用默认路径？(y/N):")
if if_default:
    for index, name in enumerate(PATH_name):
        PATH[index] = input("{}:".format(name))

print("\n"+"="*30)
print("测试环境：")
for index, name in enumerate(PATH_name):
    PATH[index] = os.path.abspath(PATH[index])
    print("{}:{}".format(name, PATH[index]))
print("="*30+"\n")

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
        print("{}/{}:{}".format(cnt, total, exe))
        os.system(exe)


function_test_files = os.listdir(os.path.abspath(PATH[0]))
function_test_in = list(filter(lambda x: x.endswith(".sy"), function_test_files))
function_test_out = list(filter(lambda x: x.endswith(".out"), function_test_files))
tester(PATH[0], function_test_in)


performance_test_files = os.listdir(os.path.abspath(PATH[1]))
performance_test_in = list(filter(lambda x: x.endswith(".sy"), performance_test_files))
performance_test_out = list(filter(lambda x: x.endswith(".out"), performance_test_files))
tester(PATH[1], performance_test_in)

