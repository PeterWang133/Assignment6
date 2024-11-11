
# Threaded Merge Sort Experiments

## Host 1: [Krishs-MacBook-Pro.local]

The approximate number of processes running before you start each experiment through running ps aux | wc -l was 568.

- CPU: Apple 1 Max
- Cores: 10
- Cache size (if known):
   - L1 Instruction Cache: 131072 bytes = 128 KB
   - L1 Data Cache: 65536 bytes = 64 KB
   - L2 Cache: 4194304 bytes = 4 MB
- RAM: 34359738368 bytes = 32 GB
- Storage (if known): 
Filesystem       Size   Used  Avail Capacity iused      ifree %iused  Mounted on
/dev/disk3s1s1  1.8Ti   22Gi  1.1Ti     2%  500632 4293967565    0%   /
- OS: 
ProductName:	macOS
ProductVersion:	12.3
BuildVersion:	21E230

### Input data

*Briefly describe how large your data set is and how you created it. Also include how long `msort` took to sort it.*

This command generates one billion random integers between 1 and 100,000,000 and writes them to a file called input_dataa.txt.

python3 -c "import random; [print(random.randint(1, 100000000)) for _ in range(1000000000)]" > input_dataa.txt

I then sorted 75100000 elements from this data set using msort with the following command:

/usr/bin/time -p ./msort 75100000 < input_dataa.txt

The sorting time for these 75100000 elements was:

The sorting time for these 75100000 elements was 13.779168 seconds.

### Experiments

*Replace X, Y, Z with the number of threads used in each experiment set.*

#### 1 Thread

Command used to run experiment: Command used to run experiment: /usr/bin/time -p ./tmsort 75100000 < input dataa.txt

Sorting portion timings:

1. 13.813578 seconds
2. 13.770955 seconds
3. 13.775084 seconds
4. 14.031423 seconds

#### 2 Threads

export MSORT_THREADS=2
/usr/bin/time -p ./tmsort 75100000 < input dataa.txt

1. 13.644825 seconds
2. 13.947518 seconds
3. 13.642553 seconds
4. ______ seconds

#### Z Threads

Command used to run experiment: 10

Sorting portion timings:

1. 14.002196 seconds
2. 13.142133 seconds
3. ______ seconds
4. ______ seconds

#### 20 Threads

Command used to run experiment: 100

Sorting portion timings:

1. 13.69 seconds
2. ______ seconds
3. ______ seconds
4. ______ seconds


*repeat sections as needed*

## Host 2: [NAME]

*use Host 1 template to fill this section*


## Observations and Conclusions

*Reflect on the experiment results and the optimal number of threads for your concurrent merge sort implementation on different hosts or platforms. Try to explain why the performance stops improving or even starts deteriorating at certain thread counts.*


