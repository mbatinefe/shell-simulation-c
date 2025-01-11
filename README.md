# TreePipe Command Implementation

A C program that simulates a shell command called TreePipe which creates a full binary tree of processes with inter-process communication using Unix pipes.

## Overview
TreePipe is a command that creates a binary tree where:

- Each node is a process
- Left children perform addition operations
- Right children perform multiplication operations
- Processes communicate using Unix pipes
- Tree traversal follows post-order traversal
  
![output](https://github.com/user-attachments/assets/7921e9f3-e7f0-4fb7-a3c2-3dadf1e8fe49)

## Components
- treePipe.c - Main implementation
- pl.c - Left child program (addition)
- pr.c - Right child program (multiplication)
- Makefile - Build configuration

## Building
```bash
make all      # Builds all executables
make clean    # Removes compiled files
```

## Usage
```bash
./treePipe <current_depth> <max_depth> <left_right>
```
### Parameters
- current_depth: Current depth in tree (0 for root)
- max_depth: Maximum depth of the tree
- left_right: 0 for left child (addition), 1 for right child (multiplication)

## Input/Output Format
- Each node takes two integers (num1, num2) as input
- Leaf nodes use default num2 = 1
- Root node gets num1 from user input
- Each node outputs computation result

## Implementation Details
- Uses Unix pipes for inter-process communication
- Follows post-order traversal
- File descriptors modified to redirect STDOUT/STDIN
- Uses fork(), wait(), execvp() system calls
- Maximum 10 characters for pipe data transfer

## Example Run
``` bash
./treePipe 0 2 0

> Current depth: 0, lr: 0
Please enter num1 for the root: 2
---> Current depth: 1, lr: 0
...
The final result is: 55
```
