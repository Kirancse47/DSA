# Difference Array / Range Update — Q&A
> Source: `Kirancse47/DSA` -> `DiffArrTechnique/` — 3 problems

**Cheat Sheet:**
- **Difference Array Concept:** To add a value `v` to a range `[l, r]` in an array, apply `diff[l] += v` and `diff[r+1] -= v`.
- **Reconstruction:** A single pass of prefix sums over the `diff` array reconstructs the final values, turning $Q$ range updates from $O(Q \times n)$ into $O(Q + n)$.
- **Contrast with Prefix Sum:** Prefix sums are for $O(1)$ range *queries* on a static array. Difference arrays are for $O(1)$ range *updates* with a final single read.
- **When to use Segment Tree/BIT instead:** If you need to interleave range updates AND range queries dynamically, a difference array cannot answer queries mid-process without an $O(n)$ rebuild.
- **Canonical Snippet:**
  ```cpp
  vector<int> diff(n + 1, 0);
  for (auto& op : updates) {
      diff[op.l] += op.v;
      diff[op.r + 1] -= op.v;
  }
  for (int i = 0; i < n; i++) {
      if (i > 0) diff[i] += diff[i - 1];
      arr[i] += diff[i];
  }
  ```

## 1. 2D Difference Array

**Q:** Given a 2D matrix and multiple operations to add a value `v` to a subgrid `(r1, c1)` to `(r2, c2)`, return the final matrix after all operations.

**A:** 1D Difference Array applied row by row.

**Explanation:**
- A full 2D difference array would use `diff[r1][c1] += v`, `diff[r2+1][c1] -= v`, `diff[r1][c2+1] -= v`, `diff[r2+1][c2+1] += v`.
- Alternatively (as implemented here), apply a 1D difference array to each row independently. For an operation, loop over rows `i` from `r1` to `r2` and apply `diff[i][c1] += v` and `diff[i][c2+1] -= v`.
- After processing all operations, compute the row-wise prefix sum and add the results to the original matrix.

**Complexity:** Time $O(Q \times (\text{r2} - \text{r1}) + n \times m)$, Space $O(n \times m)$ where $n, m$ are matrix dimensions and $Q$ is number of operations.

```cpp
    vector<vector<int>> applyDiff2D(vector<vector<int>>& mat,vector<vector<int>>& opr) {
        int n=mat.size(),m=mat[0].size();
        vector<vector<int>> diff(n,vector<int>(m+1,0));
        for(auto it : opr){
            int v=it[0],r1=it[1],c1=it[2],r2=it[3],c2=it[4];
            for(int i=r1;i<=r2;i++){
                diff[i][c1]+=v;
                diff[i][c2+1]-=v;
            }
        }
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(j!=0) diff[i][j]+=diff[i][j-1];
                mat[i][j]+=diff[i][j];
            }
        }
        return mat;
    }

gfg link - https://www.geeksforgeeks.org/problems/2-d-difference-array/1
```

## 2. 3355. Zero Array Transformation I

**Q:** Given an array and operations where each operation allows decrementing values in range `[l, r]` by up to 1, determine if it is possible to make all elements zero or less.

**A:** Difference Array to track maximum possible decrements.

**Explanation:**
- We want to apply the maximum possible decrement of 1 for every operation range `[l, r]`.
- Use a difference array to record `diff[l] -= 1` and `diff[r+1] += 1`.
- Compute the prefix sum of the difference array and add it to the original array elements.
- If any element remains strictly greater than 0 after applying all overlapping decrements, return false.

**Complexity:** Time $O(n + Q)$, Space $O(n)$ where $n$ is array size and $Q$ is operations count.

```cpp
    bool isZeroArray(vector<int>& arr, vector<vector<int>>& opr) {
        int n=arr.size();
        vector<int> diffA(n+1,0);
        for(auto it : opr){
            int l=it[0],r=it[1];
            diffA[l]-=1;
            diffA[r+1]+=1;
        }
        for(int i=0;i<n;i++){
            if(i!=0) diffA[i]+=diffA[i-1];
            arr[i]+=diffA[i];
            if(arr[i]>0) return false;
        }
        return true;
    }
```

## 3. Difference Array

**Q:** Given an array and a set of queries to add a value `v` to all elements in range `[l, r]`, return the updated array.

**A:** Standard 1D Difference Array.

**Explanation:**
- Create a `diff` array of size `n+1` initialized to 0.
- For each update, add `v` to `diff[l]` and subtract `v` from `diff[r+1]`.
- Perform a prefix sum over the `diff` array.
- Add the corresponding prefix sum value `diff[i]` to each element `arr[i]` of the original array.

**Complexity:** Time $O(n + Q)$, Space $O(n)$ where $n$ is array size and $Q$ is operations count.

```cpp
    vector<int> diffArray(vector<int>& arr, vector<vector<int>>& opr) {
        int n=arr.size();
        vector<int> diffA(n+1,0);
        for(auto it : opr){
            int l=it[0],r=it[1],v=it[2];
            diffA[l]+=v;
            diffA[r+1]-=v;
        }
        for(int i=0;i<n;i++){
            if(i!=0) diffA[i]+=diffA[i-1];
            arr[i]+=diffA[i];
        }
        return arr;
    }

gfg link - https://www.geeksforgeeks.org/problems/difference-array/1
```
