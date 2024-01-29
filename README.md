# OS Scheduler

This project simulates a CPU scheduler.

## Implemented Algorithms

The scheduler component implements the following three scheduling algorithms:

1. **Non-preemptive Highest Priority First (HPF)**
2. **Shortest Remaining Time Next (SRTN)**
3. **Round Robin (RR)**

## Memory Allocation Algorithms

1. **First Fit:**
  - Scans available memory blocks.
  - Allocates the first block large enough for the process.

2. **Buddy Memory Allocation:**
  - Divides total memory into powers-of-two blocks.
  - Allocates the smallest available block for the process.
  - Splits blocks into buddies if needed.
  - Merges neighboring free blocks after de-allocation.

### 5. Input/Output:
- **Input File (`processes.txt`)**
- **Output Files:**
   - `scheduler.log` 
   - `scheduler.perf`

## How to Run

1. **Compilation:**
```bash
make
```

2. **Execution:**
```bash
make run
```