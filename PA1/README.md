# Programming Assignment 1: Introduction to Computer Networks 512.4662

**Students:**
*   Roy Sarafov, 209619477, roysarafov@mail.tau.ac.il
*   Yoav Dychtwald, 209518299, yoavhaid@mail.tau.ac.il

## Submission Contents
*   `server.c`: Implementation of the dual-thread UDP server using POSIX threads and `sys/queue.h` for dropping-tail queue management.
*   `client.c`: Implementation of the UDP client that generates job demands via Poisson process distributions.
*   `Makefile`: Compilation script configured with strict safety and sanitization flags.
*   `README.pdf`: This document containing experiment statistics, graphics, and system analysis.

## Experiment Results

### 1. Single Client, Unbounded Queue
*(Queue size set to >4000 to guarantee unbounded behavior)*

**Parameters: (μ=5, λ=3)**
*   **Average Job Time:** 1923047.09 ns
*   **Median Job Time:** 1124405.50 ns
*   **Average Queue Occupancy:** 1.43
*   **Median Queue Occupancy:** 1.00

**Parameters: (μ=3, λ=5)**
*   **Average Job Time:** 61624987.49 ns
*   **Median Job Time:** 42834634.50 ns
*   **Average Queue Occupancy:** 40.15
*   **Median Queue Occupancy:** 32.00

*(Additional results for 2000 and 4000 jobs)*

**Parameters: (μ=50, λ=30, Jobs=2000)**
*   **Average Job Time:** 2688973.20 ns
*   **Median Job Time:** 802747.50 ns
*   **Average Queue Occupancy:** 5.26
*   **Median Queue Occupancy:** 2.00

**Parameters: (μ=50, λ=35, Jobs=2000)**
*   **Average Job Time:** 752885.05 ns
*   **Median Job Time:** 309838.50 ns
*   **Average Queue Occupancy:** 1.41
*   **Median Queue Occupancy:** 1.00

**Parameters: (μ=50, λ=30, Jobs=4000)**
*   **Average Job Time:** 32309352.70 ns
*   **Median Job Time:** 32592739.00 ns
*   **Average Queue Occupancy:** 122.94
*   **Median Queue Occupancy:** 124.00

### 2. Two Clients, Unbounded Queue
*(2000 jobs each, μ=50, λ=20)*

*   **Average Job Time:** 433698650.26 ns
*   **Median Job Time:** 240514250.50 ns
*   **Average Queue Occupancy:** 851.20
*   **Median Queue Occupancy:** 871.00

### 3. Single Client, Bounded Queue (Size = 10)
*(2000 jobs, checking dropping behavior)*

**Parameters: (μ=50, λ=45)**
*   **Total Jobs Dropped:** 0
*   **Percentage Dropped:** 0.00%
*   **Average Job Time:** 309397.63 ns

**Parameters: (μ=50, λ=48)**
*   **Total Jobs Dropped:** 7
*   **Percentage Dropped:** 0.35%
*   **Average Job Time:** 502740.97 ns

---

## Visualizations

### 1. Single Client, Unbounded Queue (μ=5, λ=3, Jobs=1000)
#### Queue Size Over Time
![Queue Size](results/exp1_1_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp1_1_histogram.png)

### 2. Single Client, Unstable Unbounded Queue (μ=3, λ=5, Jobs=1000)
#### Queue Size Over Time
![Queue Size](results/exp1_2_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp1_2_histogram.png)

### 3. Single Client, Unbounded Queue (μ=50, λ=30, Jobs=2000)
#### Queue Size Over Time
![Queue Size](results/exp1_3_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp1_3_histogram.png)

### 4. Single Client, Unbounded Queue (μ=50, λ=35, Jobs=2000)
#### Queue Size Over Time
![Queue Size](results/exp1_4_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp1_4_histogram.png)

### 5. Single Client, Unbounded Queue (μ=50, λ=30, Jobs=4000)
#### Queue Size Over Time
![Queue Size](results/exp1_5_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp1_5_histogram.png)

### 6. Two Clients, Unbounded Queue (μ=50, λ=20, Jobs=4000 total)
#### Queue Size Over Time
![Queue Size](results/exp2_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp2_histogram.png)

### 7. Single Client, Bounded Queue (Size=10, μ=50, λ=45, Jobs=2000)
#### Queue Size Over Time
![Queue Size](results/exp3_1_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp3_1_histogram.png)

### 8. Single Client, Bounded Queue (Size=10, μ=50, λ=48, Jobs=2000)
#### Queue Size Over Time
![Queue Size](results/exp3_2_queue_depth.png)

#### Job System Times Histogram
![System Times](results/exp3_2_histogram.png)
