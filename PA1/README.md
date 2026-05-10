# Programming Assignment 1: Introduction to Computer Networks 512.4662

**Students:**

* Roy Sarafov, 209619477, <roysarafov@mail.tau.ac.il>
* Yoav Dychtwald, 209518299, <yoavhaid@mail.tau.ac.il>

## Submission Contents

* `server.c`: Implementation of the dual-thread UDP server using POSIX threads and `sys/queue.h` for dropping-tail queue management.
* `client.c`: Implementation of the UDP client that generates job demands via Poisson process distributions.
* `Makefile`: Compilation script configured with strict safety and sanitization flags.
* `README.pdf`: This document containing experiment statistics, graphics, and system analysis.

## Experiment Results

### 1. Single Client, Unbounded Queue

(Queue size set to >4000 to guarantee unbounded behavior)

#### Parameters: (μ=5, λ=3)

* **Average Job Time:** 1992736.09 ns
* **Median Job Time:** 1176258.00 ns
* **Average Queue Occupancy:** 1.48
* **Median Queue Occupancy:** 1.00

#### Parameters: (μ=3, λ=5)

* **Average Job Time:** 52694451.03 ns
* **Median Job Time:** 39860606.50 ns
* **Average Queue Occupancy:** 50.49
* **Median Queue Occupancy:** 46.00

(Additional results for 2000 and 4000 jobs)

#### Parameters: (μ=50, λ=30, Jobs=2000)

* **Average Job Time:** 537437.44 ns
* **Median Job Time:** 317950.50 ns
* **Average Queue Occupancy:** 1.46
* **Median Queue Occupancy:** 1.00

#### Parameters: (μ=50, λ=35, Jobs=2000)

* **Average Job Time:** 1243732.54 ns
* **Median Job Time:** 580215.50 ns
* **Average Queue Occupancy:** 2.54
* **Median Queue Occupancy:** 2.00

#### Parameters: (μ=50, λ=30, Jobs=4000)

* **Average Job Time:** 360597.11 ns
* **Median Job Time:** 211846.00 ns
* **Average Queue Occupancy:** 1.26
* **Median Queue Occupancy:** 1.00

### 2. Two Clients, Unbounded Queue

(2000 jobs each, μ=50, λ=20)

* **Average Job Time:** 614140718.85 ns
* **Median Job Time:** 222743150.00 ns
* **Average Queue Occupancy:** 838.72
* **Median Queue Occupancy:** 840.50

### 3. Single Client, Bounded Queue (Size = 10)

(2000 jobs, checking dropping behavior)

#### Parameters: (μ=50, λ=45)

* **Total Jobs Dropped:** 7
* **Percentage Dropped:** 0.35%
* **Average Job Time:** 554251.03 ns

#### Parameters: (μ=50, λ=48)

* **Total Jobs Dropped:** 0
* **Percentage Dropped:** 0.00%
* **Average Job Time:** 287908.24 ns

---

## Visualizations

### 1. Single Client, Unbounded Queue (μ=5, λ=3, Jobs=1000)

#### 1. Queue Size Over Time

![Queue Size](results/exp1_1_queue_depth.png)

#### 1. Job System Times Histogram

![System Times](results/exp1_1_histogram.png)

### 2. Single Client, Unstable Unbounded Queue (μ=3, λ=5, Jobs=1000)

#### 2. Queue Size Over Time

![Queue Size](results/exp1_2_queue_depth.png)

#### 2. Job System Times Histogram

![System Times](results/exp1_2_histogram.png)

### 3. Single Client, Unbounded Queue (μ=50, λ=30, Jobs=2000)

#### 3. Queue Size Over Time

![Queue Size](results/exp1_3_queue_depth.png)

#### 3. Job System Times Histogram

![System Times](results/exp1_3_histogram.png)

### 4. Single Client, Unbounded Queue (μ=50, λ=35, Jobs=2000)

#### 4. Queue Size Over Time

![Queue Size](results/exp1_4_queue_depth.png)

#### 4. Job System Times Histogram

![System Times](results/exp1_4_histogram.png)

### 5. Single Client, Unbounded Queue (μ=50, λ=30, Jobs=4000)

#### 5. Queue Size Over Time

![Queue Size](results/exp1_5_queue_depth.png)

#### 5. Job System Times Histogram

![System Times](results/exp1_5_histogram.png)

### 6. Two Clients, Unbounded Queue (μ=50, λ=20, Jobs=4000 total)

#### 6. Queue Size Over Time

![Queue Size](results/exp2_queue_depth.png)

#### 6. Job System Times Histogram

![System Times](results/exp2_histogram.png)

### 7. Single Client, Bounded Queue (Size=10, μ=50, λ=45, Jobs=2000)

#### 7. Queue Size Over Time

![Queue Size](results/exp3_1_queue_depth.png)

#### 7. Job System Times Histogram

![System Times](results/exp3_1_histogram.png)

### 8. Single Client, Bounded Queue (Size=10, μ=50, λ=48, Jobs=2000)

#### 8. Queue Size Over Time

![Queue Size](results/exp3_2_queue_depth.png)

#### 8. Job System Times Histogram

![System Times](results/exp3_2_histogram.png)
