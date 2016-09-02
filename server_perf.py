from socket import *
from time import time

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('', 31337))

socket_data = []
result_name = ''

while True:
    data = s.recv(20000)
    if "done" in data:
        result_name = data.split('!')[1]
        break

    packet_time, packet_payload = data.split('!')
    packet_payload = packet_payload.upper()
    packet_time = float(packet_time)
    done_time = time()
    socket_data.append((done_time, packet_time, packet_payload))

with open('network_test_python_' + result_name, 'w') as f:
    diffs = []
    for l in socket_data:
        diff = l[0] - l[1]
        diffs.append(diff)
        f.write('{:.30f}\t{:.30f}\t{:.30f}\t{}\n'.format(l[0], l[1], diff, l[2]))

    f.write('\n\nStats:\n')
    mean = str(sum(diffs) / len(diffs))
    median = str(sorted(diffs)[len(diffs) / 2])
    f.write('mean: ' + mean + '\n')
    f.write('median: ' + median + '\n')
    print 'mean:', mean
    print 'median:', median
