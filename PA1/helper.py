import sys
import os
import numpy as np
import matplotlib.pyplot as plt

def analyze_server_output(filepath, expected_total_jobs):
    try:
        with open(filepath, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        return None

    arrivals = []
    job_times = []
    queue_occupancies = []
    
    for line in lines:
        parts = line.strip().split()
        if len(parts) >= 6:
            try:
                arrival = int(parts[2])
                departure = int(parts[3])
                q_size = int(parts[4])
                
                job_time = departure - arrival
                
                arrivals.append(arrival)
                job_times.append(job_time)
                queue_occupancies.append(q_size)
            except (ValueError, IndexError):
                continue
    
    if not job_times:
        return None

    avg_job_time = np.mean(job_times)
    med_job_time = np.median(job_times)
    avg_q = np.mean(queue_occupancies)
    med_q = np.median(queue_occupancies)
    
    jobs_processed = len(job_times)
    jobs_dropped = 0
    drop_percentage = 0.0
    if expected_total_jobs > 0:
        jobs_dropped = expected_total_jobs - jobs_processed
        if jobs_dropped < 0: jobs_dropped = 0
        drop_percentage = (jobs_dropped / expected_total_jobs) * 100.0

    return {
        'avg_job_time': avg_job_time,
        'med_job_time': med_job_time,
        'avg_q': avg_q,
        'med_q': med_q,
        'jobs_dropped': jobs_dropped,
        'drop_percentage': drop_percentage,
        'arrivals': arrivals,
        'job_times': job_times,
        'q_sizes': queue_occupancies
    }

def generate_plots(exp_name, data):
    if not data:
        return
    
    arrivals = data['arrivals']
    job_times = data['job_times']
    q_sizes = data['q_sizes']

    min_arrival = min(arrivals)
    time_seconds = [(t - min_arrival) / 1e9 for t in arrivals]

    # Queue Size Plot
    plt.figure(figsize=(10, 5))
    plt.plot(time_seconds, q_sizes, drawstyle='steps-post', color='blue', linewidth=1.5)
    plt.title(f"Queue Size Over Time - {exp_name}")
    plt.xlabel("Time since first arrival (seconds)")
    plt.ylabel("Queue Depth")
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(f"results/{exp_name}_queue_depth.png")
    plt.close()

    # Histogram Plot
    plt.figure(figsize=(10, 5))
    plt.hist(job_times, bins=10, color='orange', edgecolor='black')
    plt.title(f"Job System Times Distribution - {exp_name}")
    plt.xlabel("System Time (nanoseconds)")
    plt.ylabel("Frequency")
    plt.grid(True, axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(f"results/{exp_name}_histogram.png")
    plt.close()

def update_readme(stats):
    readme_content = f"""# Programming Assignment 1: Introduction to Computer Networks 512.4662

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

#### Parameters: (μ=5, λ=3, Jobs=1000)

* **Average Job Time:** {stats['exp1_1']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_1']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_1']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_1']['med_q']:.2f}

#### Parameters: (μ=3, λ=5, Jobs=1000)

* **Average Job Time:** {stats['exp1_2']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_2']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_2']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_2']['med_q']:.2f}

#### Parameters: (μ=50, λ=30, Jobs=1000)

* **Average Job Time:** {stats['exp1_3']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_3']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_3']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_3']['med_q']:.2f}

#### Parameters: (μ=5, λ=3, Jobs=4000)

* **Average Job Time:** {stats['exp1_4']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_4']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_4']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_4']['med_q']:.2f}

#### Parameters: (μ=3, λ=5, Jobs=4000)

* **Average Job Time:** {stats['exp1_5']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_5']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_5']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_5']['med_q']:.2f}

#### Parameters: (μ=50, λ=30, Jobs=4000)

* **Average Job Time:** {stats['exp1_6']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_6']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_6']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_6']['med_q']:.2f}

#### Parameters: (μ=50, λ=35, Jobs=2000)

* **Average Job Time:** {stats['exp1_7']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_7']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_7']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_7']['med_q']:.2f}

#### Parameters: (μ=50, λ=40, Jobs=2000)

* **Average Job Time:** {stats['exp1_8']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_8']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_8']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_8']['med_q']:.2f}

#### Parameters: (μ=50, λ=45, Jobs=2000)

* **Average Job Time:** {stats['exp1_9']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp1_9']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp1_9']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp1_9']['med_q']:.2f}

### 2. Two Clients, Unbounded Queue

(2000 jobs each, μ=50, λ=20)

* **Average Job Time:** {stats['exp2']['avg_job_time']:.2f} ns
* **Median Job Time:** {stats['exp2']['med_job_time']:.2f} ns
* **Average Queue Occupancy:** {stats['exp2']['avg_q']:.2f}
* **Median Queue Occupancy:** {stats['exp2']['med_q']:.2f}

### 3. Single Client, Bounded Queue (Size = 10)

(2000 jobs, checking dropping behavior)

#### Parameters: (μ=50, λ=45)

* **Total Jobs Dropped:** {stats['exp3_1']['jobs_dropped']}
* **Percentage Dropped:** {stats['exp3_1']['drop_percentage']:.2f}%
* **Average Job Time:** {stats['exp3_1']['avg_job_time']:.2f} ns

#### Parameters: (μ=50, λ=48)

* **Total Jobs Dropped:** {stats['exp3_2']['jobs_dropped']}
* **Percentage Dropped:** {stats['exp3_2']['drop_percentage']:.2f}%
* **Average Job Time:** {stats['exp3_2']['avg_job_time']:.2f} ns

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

### 3. Single Client, Unbounded Queue (μ=50, λ=30, Jobs=1000)

#### 3. Queue Size Over Time

![Queue Size](results/exp1_3_queue_depth.png)

#### 3. Job System Times Histogram

![System Times](results/exp1_3_histogram.png)

### 4. Single Client, Unbounded Queue (μ=5, λ=3, Jobs=4000)

#### 4. Queue Size Over Time

![Queue Size](results/exp1_4_queue_depth.png)

#### 4. Job System Times Histogram

![System Times](results/exp1_4_histogram.png)

### 5. Single Client, Unstable Unbounded Queue (μ=3, λ=5, Jobs=4000)

#### 5. Queue Size Over Time

![Queue Size](results/exp1_5_queue_depth.png)

#### 5. Job System Times Histogram

![System Times](results/exp1_5_histogram.png)

### 6. Single Client, Unbounded Queue (μ=50, λ=30, Jobs=4000)

#### 6. Queue Size Over Time

![Queue Size](results/exp1_6_queue_depth.png)

#### 6. Job System Times Histogram

![System Times](results/exp1_6_histogram.png)

### 7. Single Client, Unbounded Queue (μ=50, λ=35, Jobs=2000)

#### 7. Queue Size Over Time

![Queue Size](results/exp1_7_queue_depth.png)

#### 7. Job System Times Histogram

![System Times](results/exp1_7_histogram.png)

### 8. Single Client, Unbounded Queue (μ=50, λ=40, Jobs=2000)

#### 8. Queue Size Over Time

![Queue Size](results/exp1_8_queue_depth.png)

#### 8. Job System Times Histogram

![System Times](results/exp1_8_histogram.png)

### 9. Single Client, Unbounded Queue (μ=50, λ=45, Jobs=2000)

#### 9. Queue Size Over Time

![Queue Size](results/exp1_9_queue_depth.png)

#### 9. Job System Times Histogram

![System Times](results/exp1_9_histogram.png)

### 10. Two Clients, Unbounded Queue (μ=50, λ=20, Jobs=4000 total)

#### 10. Queue Size Over Time

![Queue Size](results/exp2_queue_depth.png)

#### 10. Job System Times Histogram

![System Times](results/exp2_histogram.png)

### 11. Single Client, Bounded Queue (Size=10, μ=50, λ=45, Jobs=2000)

#### 11. Queue Size Over Time

![Queue Size](results/exp3_1_queue_depth.png)

#### 11. Job System Times Histogram

![System Times](results/exp3_1_histogram.png)

### 12. Single Client, Bounded Queue (Size=10, μ=50, λ=48, Jobs=2000)

#### 12. Queue Size Over Time

![Queue Size](results/exp3_2_queue_depth.png)

#### 12. Job System Times Histogram

![System Times](results/exp3_2_histogram.png)
"""
    with open("README.md", "w") as f:
        f.write(readme_content)


if __name__ == '__main__':
    experiments = [
        ("exp1_1", 1000), ("exp1_2", 1000), ("exp1_3", 1000),
        ("exp1_4", 4000), ("exp1_5", 4000), ("exp1_6", 4000),
        ("exp1_7", 2000), ("exp1_8", 2000), ("exp1_9", 2000),
        ("exp2", 4000),
        ("exp3_1", 2000), ("exp3_2", 2000)
    ]
    
    all_stats = {}
    
    for exp_name, expected_jobs in experiments:
        filepath = f"results/{exp_name}_server.tsv"
        data = analyze_server_output(filepath, expected_jobs)
        
        if data:
            all_stats[exp_name] = data
            generate_plots(exp_name, data)
            print(f"Processed {exp_name}")
        else:
            print(f"WARNING: Could not process data for {exp_name}. Using dummy values.")
            all_stats[exp_name] = {
                'avg_job_time': 0, 'med_job_time': 0, 'avg_q': 0, 'med_q': 0,
                'jobs_dropped': 0, 'drop_percentage': 0
            }
    
    update_readme(all_stats)
    print("README.md has been rewritten with new statistics.")