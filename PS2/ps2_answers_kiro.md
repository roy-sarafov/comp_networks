# תרגיל 2 – פתרונות | Problem Set 2 – Solutions
## 512.4462 מבוא לתקשורת מחשבים

---

## Question 1: Spanning Tree Protocol

### Network Topology

| Bridge | Ports and Segments |
|--------|-------------------|
| B12 | P1→S3, P2→S1 |
| B16 | P1→S5, P2→S1, P3→S4 |
| B24 | P1→S7, P2→S3 |
| B32 | P1→S9, P2→S7, P3→S5, P4→S10 |
| B35 | P1→S8, P2→S6, P3→S4 |
| B55 | P1→S11, P2→S9 |
| B73 | P1→S11, P2→S10, P3→S12 |
| B92 | P1→S4, P2→S2 |

### 1.1 – Normal Spanning Tree

**Root Bridge:** B12 (lowest ID = 12)

**Bridge distances and root ports:**

| Bridge | Distance from Root | Root Port | Via Segment |
|--------|-------------------|-----------|-------------|
| B12 | 0 | — (root) | — |
| B16 | 1 | P2 | S1 |
| B24 | 1 | P2 | S3 |
| B32 | 2 | P3 | S5 (via B16, ID 16 < B24's 24 on S7) |
| B35 | 2 | P3 | S4 (via B16) |
| B92 | 2 | P1 | S4 (via B16) |
| B55 | 3 | P2 | S9 (via B32) |
| B73 | 3 | P2 | S10 (via B32) |

**Designated ports per segment:**

| Segment | Designated Port | Reasoning |
|---------|----------------|-----------|
| S1 | B12-P2 | Root directly on segment (cost 0) |
| S2 | B92-P2 | Only bridge on segment |
| S3 | B12-P1 | Root directly on segment (cost 0) |
| S4 | B16-P3 | Cost 1 (B16) vs cost 2 (B35, B92) |
| S5 | B16-P1 | Cost 1 (B16) vs cost 2 (B32) |
| S6 | B35-P2 | Only bridge on segment |
| S7 | B24-P1 | Cost 1 (B24) vs cost 2 (B32) |
| S8 | B35-P1 | Only bridge on segment |
| S9 | B32-P1 | Cost 2 (B32) vs cost 3 (B55) |
| S10 | B32-P4 | Cost 2 (B32) vs cost 3 (B73) |
| S11 | B55-P1 | Cost 3, B55 (ID 55 < 73) vs B73 |
| S12 | B73-P3 | Only bridge on segment |

**Blocked ports:**
- B32-P2 (on S7) – not root port, not designated
- B73-P1 (on S11) – not root port, not designated

**Non-forwarding bridges:** None. All bridges have at least one designated port and thus forward frames.

---

### 1.2 – B32 Thinks It Is B12

B32 sends BPDUs with bridge ID = 12 and cost = 0 (acts as a second root). The network now has two "roots" both claiming ID 12.

**BPDU tie-breaking between real B12 and fake B12 (B32):** Same root ID, same cost, same bridge ID → compare port IDs. Real B12 uses ports P1, P2; Fake B12 (B32) uses P1, P2, P3, P4.

**Results:**

| Bridge | Distance | Root Port | Reasoning |
|--------|----------|-----------|-----------|
| B12 | 0 | — | Real root |
| B32 | 0 | — | Fake root (thinks it's B12) |
| B16 | 1 | P2 (S1) | Hears (12,0,12,P2) on S1 from real B12 vs (12,0,12,P3) on S5 from B32; P2 < P3 |
| B24 | 1 | P2 (S3) | Hears (12,0,12,P1) on S3 from real B12 vs (12,0,12,P2) on S7 from B32; P1 < P2 |
| B55 | 1 | P2 (S9) | Hears (12,0,12,P1) directly from B32 on S9 |
| B73 | 1 | P2 (S10) | Hears (12,0,12,P4) directly from B32 on S10 |
| B35 | 2 | P3 (S4) | Via B16 on S4 |
| B92 | 2 | P1 (S4) | Via B16 on S4 |

**Designated ports:**

| Segment | Designated Port | Reasoning |
|---------|----------------|-----------|
| S1 | B12-P2 | Real root (cost 0, bridge 12, port P2) |
| S2 | B92-P2 | Only bridge |
| S3 | B12-P1 | Real root (cost 0, bridge 12, port P1) |
| S4 | B16-P3 | Cost 1, bridge 16 |
| S5 | B32-P3 | Fake root (cost 0, bridge 12, port P3) beats B16 (cost 1) |
| S6 | B35-P2 | Only bridge |
| S7 | B32-P2 | Fake root (cost 0, bridge 12, port P2) beats B24 (cost 1) |
| S8 | B35-P1 | Only bridge |
| S9 | B32-P1 | Fake root (cost 0, bridge 12, port P1) |
| S10 | B32-P4 | Fake root (cost 0, bridge 12, port P4) |
| S11 | B55-P1 | Cost 1, B55 (ID 55 < 73) |
| S12 | B73-P3 | Only bridge |

**Blocked ports:**
- B16-P1 (on S5) – B32 is designated on S5
- B24-P1 (on S7) – B32 is designated on S7
- B73-P1 (on S11) – B55 is designated on S11

**Non-forwarding bridge: B24** – Its only ports are P1 (blocked on S7) and P2 (root port on S3). It has no designated port on any segment, so it does not forward any user frames.

---

## Question 2: Linear Chain of Bridges

### Setup
Segments 0, 1, ..., n connected by bridges 1, 2, ..., n (bridge i connects segment i−1 to segment i). Uniform traffic x between every pair of segments.

### 2.1 – Traffic on Bridge i (Linear Chain)

In a linear chain there are no loops, so the spanning tree is the chain itself regardless of which bridge is root. No ports are blocked.

Bridge i carries all traffic that crosses it – i.e., traffic between segments {0, ..., i−1} (left side, i segments) and segments {i, ..., n} (right side, n+1−i segments).

**Total traffic on bridge i:**

$$\text{Traffic}(i) = 2 \cdot i \cdot (n + 1 - i) \cdot x$$

(Factor 2 for both directions: left→right and right→left.)

**Does it depend on root position?** No. Since the topology is a simple line (no loops), the spanning tree equals the full topology regardless of which bridge has the minimum ID. No ports are ever blocked.

### 2.2 – Ring Topology (Bridge 0 Added, Lowest ID)

Bridge 0 connects segment 0 to segment n, forming a ring. Bridge 0 has the smallest ID → it is the root.

Since bridge 0 is root, both its ports (on seg 0 and seg n) are designated. Bridges route toward whichever side reaches bridge 0 faster:
- Bridge i at distance min(i, n+1−i) from root
- Bridges 1, ..., ⌊n/2⌋ route leftward (toward seg 0)
- Bridges ⌈n/2⌉+1, ..., n route rightward (toward seg n)

If n is even, bridge n/2 has equal distance both ways; tie-break by lower bridge ID on the path (bridge 0 vs bridge n, bridge 0 wins → bridge n/2 routes leftward through seg n/2 − 1... Actually the tie-break is by the BPDU: bridge n/2 hears from bridge n/2−1 (dist n/2−1, going left) vs bridge n/2+1 (dist n/2−1, going right). The tie-break is on the bridge ID of the sender: lower ID wins. For simplicity, assume bridge n/2 routes leftward (toward lower-numbered bridges).

The spanning tree splits into two arms from bridge 0. The blocked port is on the segment where the two arms meet (approximately in the middle).

**Traffic on bridge i:**

For i ≤ ⌊n/2⌋ (left arm): bridge i separates segments {0,...,i−1} from segments {i,...,⌊n/2⌋} in its subtree direction. However, since the tree is now two arms from a single root:

Bridge i (left arm) carries traffic between:
- All segments in the subtree "beyond" bridge i (segments {i, ..., ⌊n/2⌋})
- All other segments ({0, ..., i−1} ∪ {⌈n/2⌉+1, ..., n})

$$\text{Traffic}(i) = 2 \cdot (\lfloor n/2 \rfloor - i + 1) \cdot (n + 1 - (\lfloor n/2 \rfloor - i + 1)) \cdot x$$
$$= 2 \cdot (\lfloor n/2 \rfloor + 1 - i) \cdot (n - \lfloor n/2 \rfloor + i) \cdot x$$

By symmetry, for bridge i in the right arm (i > ⌈n/2⌉):

$$\text{Traffic}(i) = 2 \cdot (i - \lceil n/2 \rceil) \cdot (n + 1 - (i - \lceil n/2 \rceil)) \cdot x$$

**Simplified (for large even n, split at n/2):**

- Left arm bridge i (1 ≤ i ≤ n/2): Traffic = 2·(n/2 + 1 − i)·(n/2 + i)·x
- Right arm bridge i (n/2 + 1 ≤ i ≤ n): Traffic = 2·(i − n/2)·(n + 1 − i + n/2)·x

The maximum traffic is on bridges closest to root (i=1 and i=n), approximately n²/2 · x for large n. The middle bridges carry minimal traffic ≈ n·x.

---

## Question 3: Clos Network Cost Optimization

### Rearrangeable Non-Blocking Clos Architecture

For N×N switch with crossbar components of size c×c:
- **Stage 1:** N/c switches, each c×c → cost per switch: c^(3/2)
- **Stage 2 (middle):** c switches, each (N/c)×(N/c) → cost per switch: (N/c)^(3/2)
- **Stage 3:** N/c switches, each c×c → cost per switch: c^(3/2)

(For rearrangeable non-blocking: number of middle switches m = c is sufficient.)

### Total Cost

$$\text{Cost}(c) = 2 \cdot \frac{N}{c} \cdot c^{3/2} + c \cdot \left(\frac{N}{c}\right)^{3/2}$$
$$= 2N\sqrt{c} + \frac{N^{3/2}}{\sqrt{c}}$$

### Optimization

Taking derivative and setting to zero:

$$\frac{d}{dc}\text{Cost} = \frac{N}{\sqrt{c}} - \frac{N^{3/2}}{2c^{3/2}} = 0$$

$$\frac{N}{\sqrt{c}} = \frac{N^{3/2}}{2c^{3/2}} \implies 2c = \sqrt{N}$$

$$\boxed{c^* = \frac{\sqrt{N}}{2}}$$

### Minimum Cost (for large N)

Substituting c* = √N/2:

$$\text{Cost}_{min} = 2N \cdot \frac{N^{1/4}}{\sqrt{2}} + \frac{N^{3/2} \cdot \sqrt{2}}{N^{1/4}} = \frac{2N^{5/4}}{\sqrt{2}} + \sqrt{2} \cdot N^{5/4} = 2\sqrt{2} \cdot N^{5/4}$$

---

## Question 4: Switch Scheduling

### 4.1 – External Scenario: Feasible with OQ, Infeasible with CIOQ (no speedup)

**Switch:** 2×2

| Time Slot | Arrivals |
|-----------|----------|
| t = 1 | Pkt A: input 1 → output 1; Pkt B: input 2 → output 1 |
| t = 2 | Pkt C: input 2 → output 2 |

**With Output Queuing (OQ):**
- t=1: Fabric transfers both A and B to output 1's queue (fabric speed = N × line speed). A departs output 1 at t=1.
- t=2: B departs output 1 at t=2. C transfers to output 2 queue, departs at t=2.
- **Departure schedule: A@1, B@2, C@2. ✓ Feasible.**

**With CIOQ (no speedup):**
- t=1: Partial permutation – at most one packet to output 1. Transfer A (in1→out1). B stays at input 2.
- t=2: Must transfer both B (in2→out1) and C (in2→out2). But both are at **input 2** – a partial permutation allows at most one packet from each input port. Only one of B or C can cross the fabric.
  - If B crosses: B departs out1@t=2 ✓, but C delayed to t=3 ≠ t=2. **✗**
  - If C crosses: C departs out2@t=2 ✓, but B delayed to t=3 ≠ t=2. **✗**
- **Infeasible.** ✗

### 4.2 – Internal Schedule for CIOQ with Speedup 2

With speedup 2, two partial permutations per time slot:

| Time Slot | Permutation 1 | Permutation 2 |
|-----------|--------------|--------------|
| t = 1 | in1 → out1 (A) | in2 → out1 (B) |
| t = 2 | in2 → out2 (C) | — |

**Result:** A and B both reach output 1 by end of t=1. A departs t=1, B departs t=2. C reaches output 2 at t=2, departs t=2. Matches OQ schedule. ✓

---

## Question 5: TS 16×16 Switch Scheduling

### Setup
- 4×4 crossbar running at 4× port speed (4 sub-steps per time slot)
- Input lines: W(ports 1–4), X(ports 5–8), Y(ports 9–12), Z(ports 13–16)
- Output lines: A(ports 1–4), B(ports 5–8), C(ports 9–12), D(ports 13–16)

### Permutation

| Input | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
|-------|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|-----|
| Output| 7 |10 | 5 |11 | 1 |16 |15 |14 | 2 |12 | 4  |13  | 9  | 3  | 6  | 8  |

### Traffic Matrix (connections per input-line/output-line pair)

Group connections by input line → output line:
- **W→A:** none | **W→B:** 1→7, 3→5 | **W→C:** 2→10, 4→11 | **W→D:** none
- **X→A:** 5→1 | **X→B:** none | **X→C:** none | **X→D:** 6→16, 7→15, 8→14
- **Y→A:** 9→2, 11→4 | **Y→B:** none | **Y→C:** 10→12 | **Y→D:** 12→13
- **Z→A:** 14→3 | **Z→B:** 15→6, 16→8 | **Z→C:** 13→9 | **Z→D:** none

Traffic matrix:

|   | A | B | C | D |
|---|---|---|---|---|
| W | 0 | 2 | 2 | 0 |
| X | 1 | 0 | 0 | 3 |
| Y | 2 | 0 | 1 | 1 |
| Z | 1 | 2 | 1 | 0 |

Each row and column sums to 4. ✓

### Decomposition into 4 Permutation Matrices

| Sub-step | Crossbar Config | Connections Realized |
|----------|----------------|---------------------|
| 1 | W→B, X→D, Y→A, Z→C | 3→5, 6→16, 9→2, 13→9 |
| 2 | W→C, X→D, Y→A, Z→B | 2→10, 7→15, 11→4, 15→6 |
| 3 | W→C, X→A, Y→D, Z→B | 4→11, 5→1, 12→13, 16→8 |
| 4 | W→B, X→D, Y→C, Z→A | 1→7, 8→14, 10→12, 14→3 |

**Verification:** All 16 port-to-port connections are covered exactly once. ✓

### Explanation of Computation

1. Classify each of the 16 connections by (input line, output line) pair.
2. Build the 4×4 traffic demand matrix.
3. Decompose the matrix into 4 permutation matrices using iterative selection (each permutation = one valid crossbar configuration connecting each input line to exactly one output line).
4. For each sub-step, assign specific port connections from the available pool for that (input line, output line) pair.

---

## Question 6: Traffic Shaping

### Flow Description
- 10 packets/second, each 1000 bits
- Packet transmission: 1 ms; gap between packets: 1 ms
- Burst duration: 19 ms (10 packets × 1ms + 9 gaps × 1ms)
- Idle period: 981 ms
- Pattern repeats every 1000 ms

### 6.1 – Flow Rate Function R(t)

R(t) is periodic with period T = 1 second = 1000 ms.

Within each period:

$$R(t) = \begin{cases} 1{,}000{,}000 \text{ bps} = 1 \text{ Mbps} & t \in [0,1) \cup [2,3) \cup [4,5) \cup \cdots \cup [18,19) \text{ ms} \\ 0 & \text{otherwise} \end{cases}$$

In other words: R(t) = 1 Mbps during ms intervals [2k, 2k+1) for k = 0, 1, ..., 9 (mod 1000 ms), and R(t) = 0 elsewhere.

### 6.2 – Minimum Buffer Size σ (Drain Rate = 20 kbps)

The buffer fills during the burst and drains during idle. We track maximum buffer occupancy.

At the end of the k-th packet (t = 2k−1 ms, for k = 1, ..., 10):
- Bits input: 1000k
- Bits drained: 20,000 bps × (2k−1)/1000 s = 20(2k−1) = 40k − 20 bits

Buffer occupancy = 1000k − (40k − 20) = 960k + 20

**Maximum at k = 10 (t = 19 ms):**

$$\sigma_{min} = 960 \times 10 + 20 = \boxed{9{,}620 \text{ bits}}$$

**Verification:** After the burst, buffer drains at 20 kbps. Time to empty: 9620/20000 = 481 ms < 981 ms idle period. Buffer empties before next burst. ✓

### 6.3 – Dual Leaky Bucket Parameters

Bits are written to the buffer in groups of 2 bytes (16 bits) at a time.

The dual leaky bucket constraint requires: for any time interval of length t, the cumulative bits sent ≤ min(σ₁ + ρ₁·t, σ₂ + ρ₂·t).

**First bucket (average rate + burst tolerance):**
- ρ₁ = 10,000 bps (average rate = 10 packets × 1000 bits / 1 s)
- σ₁ = max excess above average rate over any interval

  At end of k-th packet (t = 2k−1 ms):
  Excess = bits_sent − ρ₁·t = 1000k − 10,000·(2k−1)/1000 = 1000k − 20k + 10 = 980k + 10

  Maximum at k = 10: **σ₁ = 9,810 bits**

**Second bucket (peak rate constraint):**
- ρ₂ = 1,000,000 bps (peak rate = 1 Mbps during transmission)
- σ₂ = 16 bits (one 2-byte group — the minimum instantaneous burst unit)

**Final parameters:**

$$\boxed{\rho_1 = 10{,}000 \text{ bps}, \quad \sigma_1 = 9{,}810 \text{ bits}, \quad \rho_2 = 1{,}000{,}000 \text{ bps}, \quad \sigma_2 = 16 \text{ bits}}$$
