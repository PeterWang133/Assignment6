
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

#### 10 Threads

export MSORT_THREADS=10
/usr/bin/time -p ./tmsort 75100000 < input dataa.txt

Sorting portion timings:

1. 14.002196 seconds
2. 13.453534 seconds
3. ______ seconds
4. ______ seconds

#### 20 Threads

export MSORT_THREADS=20
/usr/bin/time -p ./tmsort 75100000 < input dataa.txt

Sorting portion timings:

1. 13.69 seconds
2. ______ seconds
3. ______ seconds
4. ______ seconds


*repeat sections as needed*

## Host 2: [CodeSpace]
- CPU: GitHub Codespaces virtual CPU
- Cores: 2
- Cache size (if known): 
   - L1d cache: 32 KiB
   - L1i cache: 32 KiB
   - L2 cache: 512 KiB
   - L3 cache: 32 MiB
- RAM: 8589934592 bytes = 8 GB
- Storage
Filesystem      Size  Used Avail Use% Mounted on
overlay          32G   11G   20G  36% /
tmpfs            64M     0   64M   0% /dev
shm              64M  8.0K   64M   1% /dev/shm
/dev/root        29G   24G  5.3G  82% /vscode
/dev/sdb1        44G   18G   24G  43% /tmp
/dev/loop5       
32G   11G   20G  36% /workspaces
- OS: GitHub Codespaces Linux Environment

### Input and data
This command generates one billion random integers between 1 and 100,000,000 and writes them to a file called input_dataa.txt.

shuf -i1-100000000 > thousand-million.txt

I then sorted 75100000 elements from this data set using msort with the following command:

./tmsort 75100000 < thousand-million.txt

The sorting time for these 75100000 elements was:

The sorting time for these 75100000 elements was 14.717602 seconds.

#### 1 Thread

MSORT_THREADS=1 ./tmsort 75100000 < thousand-million.txt > /dev/null

Sorting portion timings:

1. 14.649396 seconds
2. 14.627389 seconds
3. 14.717335 seconds
4. 14.631621 seconds

#### 2 Threads

MSORT_THREADS=2 ./tmsort 75100000 < thousand-million.txt > /dev/null

1. 12.654202 seconds
2. 12.713823 seconds
3. 12.780484 seconds
4. 12.602964 seconds

#### 10 Threads

Command used to run experiment:
MSORT_THREADS=10 ./tmsort 75100000 < thousand-million.txt > /dev/null

Sorting portion timings:

1. 13.111056 seconds
2. 12.895676 seconds
3. ______ seconds
4. ______ seconds

#### 20 Threads

MSORT_THREADS=20 ./tmsort 75100000 < thousand-million.txt > /dev/null

Sorting portion timings:

1. 13.171604 seconds
2. 13.452209 seconds
3. 13.362424 seconds
4. 13.504239 seconds


## Observations and Conclusions

*Reflect on the experiment results and the optimal number of threads for your concurrent merge sort implementation on different hosts or platforms. Try to explain why the performance stops improving or even starts deteriorating at certain thread counts.*


