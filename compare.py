import sys

file1 = sys.argv[1]
file2 = sys.argv[2]

str1 = open(file1, "r")
str2 = open(file2, "r")

curr_line = 1
for line1 in str1:
    line2 = str2.readline()

    if(line1 != line2):
        print("Line ", curr_line, " isn't equal between files!")
        print(line1)
        print(line2)
        break;
    curr_line += 1
    