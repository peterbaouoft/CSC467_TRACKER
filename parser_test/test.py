from os import listdir
from os.path import isfile, join
import subprocess

test_dir = ["basic_test"]
command_to_exec = "../compiler467 -Tp"

def get_test_file_list():
    # Get all of the files in a directory
    filelist = [f for f in listdir(test_dir[0]) if isfile(join(test_dir[0], f))]
    return filelist

def create_test_output_file_name(file_name):
    return file_name.replace(".txt", "_output.txt")

def collect_output_file_result(file_name):
    corresponding_output_name = join("basic_test_output", create_test_output_file_name(file_name))
    # print("The currently tested output file is {}".format(corresponding_output_name))

    with file(corresponding_output_name) as f:
        s = f.read()
        # print("The output content is {}".format( s))
    return s

def main():
    test_file_list = get_test_file_list()
    failure_count = 0
    for test_file in test_file_list:
        # todo, have multiple test directories using a loop
        test_file_name = join(test_dir[0], test_file)
        # print("The currently tested test file is {}".format(join(test_dir[0], test_file)))

        # Create constructed commands
        command_arr = (command_to_exec + " " + test_file_name).split()
        # print("The command to execute is {}".format(command_arr))

        # use subprocess to collect the result
        ps = subprocess.Popen(command_arr, stdout=subprocess.PIPE)
        output = ps.communicate()[0]
        # print("{}".format(output))

        # collect the output
        expected_output = collect_output_file_result(test_file)

        if (output == expected_output):
            print("Test {} passes".format(test_file_name))
        else:
            print("Error on test {}".format(test_file_name))
            failure_count +=1

    if (failure_count):
        print("Total number of failures are {}".format(failure_count))

if __name__ == "__main__":
    main()
