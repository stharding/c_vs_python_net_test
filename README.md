Overview
========

This is just a quick experiment designed to compare the performance of C vs
Python for UDP network IO and CPU bound tasks with large packets arriving
at a high (100Hz) rate.

Files
-----

There are two Python files and one C files.

### `server_perf.c` ###

`server_perf.c` is a C implementation of a UDP server which is expecting
packets of the form:

`time_stamp!payload`

where `time_stamp` is a `double` representing the number of seconds since
the epoch and `payload` is expected to be a string of lowercase ascii
characters 10 kilobytes long.

`server_perf` is expecting exactly 1000 of these packets followed by a
packet of the form:

`done!extension`

where done is the literal string 'done' and extension is a string that
will be used to name the file output of the server.

The server does 'representative' work on the packet payload to simulate
doing some real world work. The 'representative' work in this case is
to capitalize all of the ascii characters in the payload.

This work seems to be a good proxy for 'real' work because every byte
in the payload is operated on.

After the work is done for the packet, a timestamp is computed and stored
along with the timestamp in the packet and the converted payload.

once the 'done' packet is received, the server computes average and
median statistics for the differences between the packet timestamps and
the post-computation timestamps. This information is written out to disk
at the end of a file which contains all the timestamp and payload computed
data.

The filename is "`network_test_c_<ext>`" where `<ext>` is the 'extension' in
the 'done' packet.

### `server_perf.py` ###

This file does exactly the same thing as `server_perf.c` but is implemented
in Python. The generated file is "`network_test_python_<ext>`" where `<ext>` is the 'extension' in
the 'done' packet.

### `driver.py` ###

This file sends all the packets described above.

Use
---

The C file `server_perf.c` can be compiled by:


```bash
gcc server_perf.c -o server_perf
```

to run the C test:
run: `./server_perf`

and then run `python driver.py`


to run the Python test:
run: `python server_perf.py`

and then run `python driver.py`

Results
-------

The C test gives me results like:

```
average difference: 0.058503
median difference: 0.000340
```

and the Python test give me results like:

```
mean difference: 0.000897380113602
median difference: 0.000858068466187
```

It seems quite interesting that the C results are quite a bit slower on average
but a little bit faster in the median.


Caveats
-------

I ran these tests on a MacBook Pro (Retina, 15-inch, Mid 2015)

Since the test is sending **huge** UDP packets you may run into this error:

`socket.error: [Errno 40] Message too long`

I had to issue the following command to overcome this error:

`sudo sysctl -w net.inet.udp.maxdgram=65535`

Altering the setting this way will not survive a reboot.
