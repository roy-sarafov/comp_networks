#!/bin/bash
make

mkdir -p results

# Helper to run a single experiment
# Usage: run_exp <name> <num_jobs> <q_size> <seed> <lambda> <mu>
run_exp() {
    NAME=$1
    JOBS=$2
    QSIZE=$3
    SEED=$4
    LAMBDA=$5
    MU=$6

    echo "Running experiment: $NAME (Jobs: $JOBS, Lambda: $LAMBDA, Mu: $MU, QSize: $QSIZE)"

    # Start server in background
    ./server 9000 $JOBS $QSIZE > results/${NAME}_server.tsv &
    SERVER_PID=$!

    # Give server a moment to bind
    sleep 0.5

    # Start client
    ./client 127.0.0.1 9000 $JOBS $SEED $LAMBDA $MU > results/${NAME}_client.tsv

    # Wait for server to finish
    wait $SERVER_PID
}

# Helper to run an experiment with TWO clients
# Usage: run_exp_two_clients <name> <num_jobs_each> <q_size> <lambda> <mu>
run_exp_two_clients() {
    NAME=$1
    JOBS_EACH=$2
    QSIZE=$3
    LAMBDA=$4
    MU=$5
    TOTAL_JOBS=$((JOBS_EACH * 2))

    echo "Running experiment: $NAME (2 Clients, $JOBS_EACH jobs each, Lambda: $LAMBDA, Mu: $MU, QSize: $QSIZE)"

    ./server 9000 $TOTAL_JOBS $QSIZE > results/${NAME}_server.tsv &
    SERVER_PID=$!

    sleep 0.5

    ./client 127.0.0.1 9000 $JOBS_EACH 42 $LAMBDA $MU > results/${NAME}_client1.tsv &
    CLIENT1_PID=$!

    ./client 127.0.0.1 9000 $JOBS_EACH 43 $LAMBDA $MU > results/${NAME}_client2.tsv &
    CLIENT2_PID=$!

    wait $CLIENT1_PID
    wait $CLIENT2_PID
    wait $SERVER_PID
}

echo "=== 1. Single Client, Unbounded Queue ==="
# >4000 q_size guarantees unbounded for these tests
run_exp "exp1_1" 1000 5000 42 3.0 5.0
run_exp "exp1_2" 1000 5000 42 5.0 3.0

run_exp "exp1_3" 2000 5000 42 30.0 50.0
run_exp "exp1_4" 2000 5000 42 35.0 50.0

run_exp "exp1_5" 4000 5000 42 30.0 50.0

echo "=== 2. Two Clients, Unbounded Queue ==="
# 2000 jobs each, total 4000.
run_exp_two_clients "exp2" 2000 5000 20.0 50.0

echo "=== 3. Single Client, Bounded Queue (Size = 10) ==="
run_exp "exp3_1" 2000 10 42 45.0 50.0
run_exp "exp3_2" 2000 10 42 48.0 50.0

echo "Experiments complete. Generating statistics, plots, and updating README.md..."

# This script performs the calculations, creates graphs, and automatically rewrites the README
python3 helper.py

echo "Done! The README.md has been updated and your graphs are in the results/ directory."
