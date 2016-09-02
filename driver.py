from socket import *
from time import time, sleep

s = socket(AF_INET, SOCK_DGRAM)

for i in range(1000):
    t = time()
    s.sendto(('{:.30f}'.format(t)) + '!' + 'a' * 1024 * 10, ('localhost', 31337))
    sleep(1. / 1000)

s.sendto('done!10k', ('localhost', 31337))
