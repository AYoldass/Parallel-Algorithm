# OPTIMIZED SpMV FINAL - ADAPTIVE BUCKET SIZE

**Modular Design + OPTIMIZED Bucket Methods!**  
**5 Methods with ADAPTIVE bucket sizing for better performance**

## 🚀 Quick Start

```bash
./run_all.sh
```

## ⚡ WHAT'S OPTIMIZED

### CSR+Bucket Parallel (Method 4)
- **ADAPTIVE bucket size** instead of fixed 1024
- Ensures 4× more buckets than threads
- Expected: 3-4 GFlop/s (vs 1.6 before)

### BCSR+Bucket Parallel (Method 5)
- **ADAPTIVE bucket size** for block rows
- Better parallelism
- Expected: 1-2 GFlop/s (vs 0.5 before)

## 📊 Expected Improvement

**Before:** 2 buckets → poor parallelism  
**After:** 32+ buckets → good parallelism!

Run and see the results!
