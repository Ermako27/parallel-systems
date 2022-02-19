import sys
from random import randint

def generateMatrix(size):
    intSize = int(size)
    f = open("{0}.txt".format(size), "w");
    f.write("{0} {0}\n".format(size))

    for i in range(0, intSize):
        for j in range(0, intSize):
            if (i == j):
                f.write("{0} ".format(9999))
            elif (j == (intSize - 1)):
                f.write("{0}\n".format(randint(1,50)));
            else:
                f.write("{0} ".format(randint(1,50)));
    f.close()  


if __name__ == "__main__":
    generateMatrix(sys.argv[1])
