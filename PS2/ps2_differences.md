# Differences: ps2-26-answers.md vs PS2.docx

## Question 1.1 – Spanning Tree (Normal)

| Item | ps2-26-answers.md | PS2.docx |
|------|-------------------|----------|
| B32 root port | **P3 (S5, via B16)** | **P1 (S7, via B24)** |
| B32 reasoning | B16 (ID 16) < B24 (ID 24) on tie-break | Routes via S7→B24 |
| B35 root port | **P3 (S4)** | **P2 (S4)** |
| S6 designated port | **B35-P2** | **B35-P3** |
| S8 designated port | **B35-P1** | **B32-P3** (wins tie on lower bridge ID) |
| S9 designated port | **B32-P1** | **B32-P2** |

**Root cause:** Different port-to-segment mappings for B32 and B35 (different reading of the diagram).

---

## Question 1.2 – B32 Thinks It Is B12

| Item | ps2-26-answers.md | PS2.docx |
|------|-------------------|----------|
| B35 distance | **2** (via S4→B16) | **1** (via S8 to fake B12) |
| B35 root port | **P3 (S4)** | **P1 (S8)** |
| S4 designated | B16-P3 | B16-P3 (same, but B35 blocks its port on S4) |
| S8 designated | **B35-P1** (only bridge) | **Fake B12 (B32)-P3** (distance 0) |
| Non-forwarding bridges | **B24** | Network "severed" into two isolated trees; B24 blocks S7 port, B35 blocks S4 port |
| Network connectivity | Connected (single tree) | **Split into two disconnected trees** |

**Root cause:** If B32-P3 connects to S8 (docx interpretation), then fake B12 directly reaches S8 and B35. If B32-P3 connects to S5 (md interpretation), it doesn't.

---

## Question 2.2 – Ring Topology

| Item | ps2-26-answers.md | PS2.docx |
|------|-------------------|----------|
| Detail level | Full formula with left/right arm split | Qualitative description only (split at ~n/2) |
| Explicit formula | 2·(n/2+1−i)·(n/2+i)·x for left arm | Not provided |

---

## Question 3 – Clos Network

Both agree: **c* = √N/2**, cost = 2√2·N^(5/4). ✓ No differences.

---

## Question 4 – Switch Scheduling

Both use identical example (2×2 switch, packets A/B/C) and same reasoning. ✓ No differences.

---

## Question 5 – TS 16×16 Switch

| Item | ps2-26-answers.md | PS2.docx |
|------|-------------------|----------|
| Sub-step details | Full table with crossbar config + specific port connections | Sub-steps listed but specific connections appear as missing/equation objects |

Cannot fully compare due to embedded equations in docx.

---

## Question 6 – Traffic Shaping

### 6.1 – R(t)
Both agree: R(t) = 1 Mbps during [2k, 2k+1) ms for k=0..9, zero otherwise. ✓

### 6.2 – Buffer Size
Both agree: **σ = 9,620 bits**. ✓

### 6.3 – Dual Leaky Bucket

| Parameter | ps2-26-answers.md | PS2.docx |
|-----------|-------------------|----------|
| ρ₁ | 10,000 bps | 10,000 bps ✓ |
| σ₁ | **9,810 bits** (exact) | **Rounded up to nearest 16-bit group** (~9,824 bits) |
| ρ₂ | 1,000,000 bps | 1,000,000 bps ✓ |
| σ₂ | 16 bits | 16 bits ✓ |

**Difference:** PS2.docx rounds σ₁ to the nearest multiple of 16 bits since data arrives in 2-byte groups.

---

## Summary

| Question | Status |
|----------|--------|
| Q1.1 | **Different** – topology interpretation |
| Q1.2 | **Different** – topology + network split conclusion |
| Q2.1 | Same |
| Q2.2 | Same concept, md has more detail |
| Q3 | Same |
| Q4 | Same |
| Q5 | Cannot fully compare |
| Q6.1 | Same |
| Q6.2 | Same |
| Q6.3 | Minor difference (σ₁ rounding) |


## kiro suggestions
B32 root port - should be from kiro version
B35 port labals - the gemini is correct
Q1.2 network split - the kiro is right (no split)
Q6.3 rounding - the gemini is correct.
