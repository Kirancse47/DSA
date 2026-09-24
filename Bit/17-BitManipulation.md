# Bit Manipulation — Q&A

> Source: `Kirancse47/DSA` -> `Bit/` — 31 problems

Ordering: Lexicographical by filename

## Cheat sheet

| Trick | Operation | What it does | Problems utilizing this |
|---|---|---|---|
| Parity check | `x & 1` | Extract the lowest bit (1 if odd, 0 if even). | Longest Consecutive 1's, Number is sparse or not |
| Right Shift | `x >> 1` | Integer division by 2, or shifting bits down. | Reverse Bits, Find first set bit |
| Clear lowest bit | `x & (x-1)` | Strips the lowest set bit. Basis for Brian Kernighan's algorithm. | Minimum Bit Flips to Convert Number, Power of 2, Number of 1 Bits |
| Isolate lowest bit | `x & -x` | Extracts the lowest set bit (rightmost 1) using two's complement. | Find first set bit, Non Repeating Numbers, Rightmost different bit |
| Set $i$-th bit | `x | (1<<i)` | Forces the $i$-th bit to 1. | Set kth bit, Copy Set Bits in Range, Maximum AND Value |
| Clear $i$-th bit | `x & ~(1<<i)` | Forces the $i$-th bit to 0. | (General utility) |
| Toggle $i$-th bit | `x ^ (1<<i)` | Flips the $i$-th bit. | Toggle bits given range |
| Test $i$-th bit | `(x >> i) & 1` | Checks if the $i$-th bit is 1. | Check whether K-th bit is set or not, Find element occuring once... |
| Power of 2 check | `x & (x-1) == 0` | Verifies if $x$ has exactly one set bit (if $x > 0$). | Power of 2, Find position of set bit |
| XOR properties | `a^a=0`, `a^0=a` | Cancels pairs. Used heavily in sequence/missing number problems. | Party of Couples, Missing number in array, Find the Difference |

> **Warning:** C++ left shifts `1 << i` produce signed integers by default. Shifting into the sign bit (like `1 << 31`) triggers undefined behavior in signed types; use `1U << i` or `1LL << i` when dealing with wide masks or upper bits. Left-shifting negative numbers is also UB.


## 1. 1720. Decode XORed Array

**Q:** Decode an XORed array where each element is `encoded[i] = arr[i] XOR arr[i + 1]` given the encoded array and the `first` element of the original array, returning the original array.

**A:** Inverse XOR property (`a = b ^ c` if `c = a ^ b`).

**Explanation:**
- Uses the fundamental property that XOR is its own inverse: if `c = a ^ b`, then `a = c ^ b`.
- Iteratively computes `arr[i+1] = arr[i] ^ encoded[i]`.
- Keeps a running current value that updates via XOR and pushes directly to the result.

**Complexity:** Time O(n), Space O(n) for the output array.

```cpp
There is a hidden integer array arr that consists of n non-negative integers.
It was encoded into another integer array encoded of length n - 1, such that encoded[i] = arr[i] XOR arr[i + 1]. For example, if arr = [1,0,2,1], then encoded = [1,2,3].
You are given the encoded array. You are also given an integer first, that is the first element of arr, i.e. arr[0].
Return the original array arr. It can be proved that the answer exists and is unique.

Example 1:

Input: encoded = [1,2,3], first = 1
Output: [1,0,2,1]
Explanation: If arr = [1,0,2,1], then first = 1 and encoded = [1 XOR 0, 0 XOR 2, 2 XOR 1] = [1,2,3]
Example 2:

Input: encoded = [6,2,7,3], first = 4
Output: [4,2,0,7,4]

```
    vector<int> decode(vector<int>& A, int cur) {
        vector<int> res = {cur};
        for (int& a: A)
            res.push_back(cur ^= a);
        return res;
    }
```

Explanation : 
According to Xor property
The inverse of XOR is XOR!

If you have:
c = a^b;
You can get a or b back if you have the other value available:
a = c^b; // or b^c (order is not important)
b = c^a; // or a^c

encoded[i] = arr[i] ^ arr[i+1]

Here we have an encoded array, and we want a resultant array, so we can interchange

res[0] = first
res[i+1] = res[i] ^ encoded[i]
```

---

## 2. 2220. Minimum Bit Flips to Convert Number

**Q:** Find the minimum number of bit flips to convert an integer `start` to an integer `goal`.

**A:** XOR the two numbers and count set bits using Brian Kernighan's algorithm.

**Explanation:**
- Taking `start ^ goal` produces a number with set bits at every position where the two integers differ.
- The `while(res)` loop clears the lowest set bit in each iteration using `res = res & (res - 1)`.
- Counting the number of iterations yields the exact number of differences (flips needed).

**Complexity:** Time O(k) where k is the number of bit differences, Space O(1).

```cpp
    int minBitFlips(int start, int goal) {
        int res = start ^ goal;
        int ans=0;
        while(res){
            ans++;
            res=(res & res-1);
        }
        return ans;
    }
```

---

## 3. 2401. Longest Nice Subarray

**Q:** Find the length of the longest subarray where the bitwise AND of every pair of elements is 0.

**A:** Sliding window tracking the running XOR (or sum) of elements.

**Explanation:**
- The condition that pairwise AND is 0 means that no two elements share a set bit.
- Thus, the XOR sum of the subarray is exactly equal to the arithmetic sum.
- The sliding window expands by adding `nums[j]`; if the XOR sum deviates from the arithmetic sum, it shrinks from the left by removing `nums[i]` until the condition is met again.

**Complexity:** Time O(n), Space O(1).

```cpp
    int longestNiceSubarray(vector<int>& nums) {
        int n=nums.size();
        int i=0,j=0;
        int xors=0;
        int curs=0;
        int ans=1;
        while(j<n){
            curs+=nums[j];
            xors^=nums[j];
            while(xors!=curs){
                curs-=nums[i];
                xors^=nums[i];
                i++;
            }
            ans = max(ans,j-i+1);
            j++;
        }
        return ans;
    }
```

---

## 4. 2425. Bitwise XOR of All Pairings

**Q:** Compute the bitwise XOR of all possible pairings between elements of two arrays.

**A:** Pairwise XOR parity analysis.

**Explanation:**
- Each element in `nums1` is XORed with every element in `nums2`, meaning it contributes to the final result exactly `n2` times.
- Because `a ^ a = 0`, if `n2` is even, elements of `nums1` cancel out. If odd, they contribute their total XOR sum.
- The same logic applies to elements of `nums2` being paired `n1` times, requiring independent parity checks for both lengths.

**Complexity:** Time O(n1 + n2), Space O(1).

```cpp
    int xorSum(vector<int>& a){
        int xs=0;
        for(int i=0;i<a.size();i++) xs^=a[i];
        return xs;
    }
    int xorAllNums(vector<int>& nums1, vector<int>& nums2) {
        int n1=nums1.size(),n2=nums2.size(),ans=0;
        if(n1&1) ans^=xorSum(nums2);
        if(n2&1) ans^=xorSum(nums1);
        return ans;
    }
```

---

## 5. 2429. Minimize XOR

**Q:** Find an integer `x` with the same number of set bits as `num2` that minimizes `x XOR num1`.

**A:** Greedy bit setting matching `num1`'s highest bits first.

**Explanation:**
- Counts set bits in both numbers using Brian Kernighan's algorithm (`x & (x-1)`).
- If `num2` has fewer set bits, it removes the lowest set bits from `num1` (by checking bit positions from 0 up to 31).
- If `num2` has more set bits, it adds set bits to `num1` starting from the lowest unset bits (position 0 up to 31) to minimize the added numerical value.

**Complexity:** Time O(32) loop, Space O(1).

```cpp
    int minimizeXor(int num1, int num2) {
        int noSet1=0,noSet2=0;
        while(num2){
            noSet2++;
            num2=(num2 & num2-1);
        }
        int tmp=num1;
        while(tmp){
            noSet1++;
            tmp=(tmp & tmp-1);
        }
        int ans=0;
        if(noSet1==noSet2) return num1;
        else if(noSet2>noSet1){
            int diff=noSet2-noSet1;
            ans=num1;
            for(int i=0;i<31 && diff;i++){
                if(!(num1 & (1<<i))){
                    ans=(ans | (1<<i));
                    diff--;
                }
            }
        }else{
            for(int i=31;i>=0 && noSet2;i--){
                if((num1 & (1<<i))){
                    ans=(ans | (1<<i));
                    noSet2--;
                }
            }
        }
        return ans;
    }
```

---

## 6. 2997. Minimum Number of Operations to Make Array XOR Equal to K

**Q:** Find the minimum operations (flipping bits of any element) to make the XOR of all elements equal to `K`.

**A:** Count differing bits between `K` and the total XOR sum.

**Explanation:**
- Computes the initial XOR sum of all elements in the array.
- Compares the total XOR sum with `K` bit by bit by repeatedly taking modulo 2 and dividing by 2.
- Each differing bit requires exactly one operation to flip anywhere in the array.

**Complexity:** Time O(n + 32) array scan and bit loop, Space O(1).

```cpp
    int minOperations(vector<int>& nums, int k) {
        int finalXor = 0;
        for (int n : nums) finalXor = finalXor ^ n;
        int count = 0;
        while (k || finalXor){
            if ((k % 2) != (finalXor % 2)) count++;
            k /= 2;
            finalXor /= 2;
        }
        return count;
    }
```

---

## 7. 371. Sum of Two Integers

**Q:** Calculate the sum of two integers without using the `+` or `-` operators.

**A:** Simulate addition using bitwise XOR and left-shifted AND.

**Explanation:**
- `a ^ b` performs addition without carrying, isolating bits where only one operand is 1.
- `a & b` finds the carry bits where both operands are 1.
- The carry is shifted left by 1 position `(carry << 1)` to be added in the next place value until there is no carry left.
- Note: This code uses `unsigned carry` to prevent undefined behavior on left-shifting negative numbers in C++.

**Complexity:** Time O(32) bit operations, Space O(1).

```cpp
    int getSum(int a, int b) {
       while(b!=0){
            unsigned carry=a&b;
            a=a^b;
            b=carry<<1;
        }
        return a;
    }
```

---

## 8. 389. Find the Difference

**Q:** Identify the one extra character added to string `t` which was formed by shuffling string `s`.

**A:** Cancel out identical characters using cumulative XOR.

**Explanation:**
- Because every original character appears exactly twice (once in `s`, once in `t`), they cancel out completely via `a ^ a = 0`.
- The single added character appears an odd number of times (once), meaning it remains in the accumulator.
- Characters are implicitly cast to integers and XORed together, safely recovering the outlier.

**Complexity:** Time O(n), Space O(1).

```cpp
    char findTheDifference(string s, string t) {
        int xors=0;
        for(auto it : t) xors^=(it);
        for(auto it : s) xors^=(it);
        return xors;
    }
```

---

## 9. Bit Difference

**Q:** Count the number of bits to flip to convert integer `a` to `b`.

**A:** XOR and Brian Kernighan's popcount.

**Explanation:**
- The expression `a ^ b` sets bits exactly where `a` and `b` differ.
- The loop continuously strips the lowest set bit using `xors = xors & (xors - 1)` and increments a counter.
- This efficiently counts the set bits in the XOR difference.

**Complexity:** Time O(k) where k is the number of differing bits, Space O(1).

```cpp
    int countBitsFlip(int a, int b){
        // Your logic here
        int xors=a^b;
        int c=0;
        while(xors){
            c++;
            xors=(xors & xors-1);
        }
        return c;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/bit-difference-1587115620/1
```

---

## 10. Check whether K-th bit is set or not

**Q:** Determine if the `K`-th bit from the right is 1.

**A:** Mask the bit with a left shift and perform bitwise AND.

**Explanation:**
- `1 << k` creates a mask with exactly the `K`-th bit set to 1.
- By ANDing `n` with this mask, all other bits are cleared.
- If the result is non-zero (or true when implicitly cast), the bit was set.

**Complexity:** Time O(1), Space O(1).

```cpp
    bool checkKthBit(int n, int k){
        return (n & (1<<k));
    }
    
    Link - https://practice.geeksforgeeks.org/problems/check-whether-k-th-bit-is-set-or-not-1587115620/1
```

---

## 11. Copy Set Bits in Range

**Q:** Copy set bits from `y` to `x` only for bit positions in the inclusive range `[l, r]`.

**A:** Iterate positions and selectively set bits in `x`.

**Explanation:**
- Loops from position `l` to `r` and checks if the `i-1`-th bit is set in `y` (uses 1-based indexing).
- If `y & (1 << i-1)` is non-zero, it forces the corresponding bit in `x` to 1 using `x | (1 << i-1)`.
- This applies the bits individually instead of generating a range mask.

**Complexity:** Time O(r - l) fixed width loop, Space O(1).

```cpp
    int setSetBit(int x, int y, int l, int r){
        // code here
        for(int i=l;i<=r;i++){
            if(y&(1<<i-1)){
               x=(x|(1<<i-1)); 
            }
        }
        return x;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/copy-set-bits-in-range0623/1
```

---

## 12. Count Subarrays with given XOR

**Q:** Count the number of contiguous subarrays whose bitwise XOR evaluates to `k`.

**A:** Prefix XOR array tracked in a hash map.

**Explanation:**
- Maintains a running `curx` (prefix XOR) while scanning the array.
- To find a subarray ending at the current element with XOR `k`, it looks for a previous prefix XOR equal to `curx ^ k`.
- A hash map records the frequencies of every prefix XOR seen so far, allowing O(1) lookups for matching subarrays.

**Complexity:** Time O(n) average array scan, Space O(n) for the hash map.

```cpp
    long subarrayXor(vector<int> &arr, int k) {
        long ans=0;
        unordered_map<int,int> mp;
        int curx=0;
        mp[0]++;
        for(auto it : arr){
            curx^=it;
            ans+=mp[curx^k];
            mp[curx]++;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/count-subarray-with-given-xor/1
```

---

## 13. Count total set bits

**Q:** Count the total number of set bits in all integers from 1 to `n`.

**A:** Iterate and apply Brian Kernighan's algorithm on every number.

**Explanation:**
- Loops linearly from 1 to `n`.
- For each integer, strips the lowest set bit repeatedly with `t = t & (t - 1)` and increments the total sum.
- Note: This is a brute-force approach `O(n log n)`, much slower than the optimal O(log n) block-counting pattern.

**Complexity:** Time O(n log n) total bits, Space O(1).

```cpp
    int countSetBits(int n){
        int ans=0;
        for(int i=1;i<n+1;i++){
            int t=i;
            while(t){
                ans++;
                t=(t & t-1);
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/count-total-set-bits-1587115620/1
```

---

## 14. Find element occuring once when all other are present thrice

**Q:** Identify the single element in an array where every other element appears exactly three times.

**A:** Vertical bit-slice counting modulo 3.

**Explanation:**
- Iterates over all 32 possible bit positions, independent of the array length.
- For each bit position, sums the bits across all elements in the array.
- Since triplet elements contribute a multiple of 3 to the sum, taking modulo 3 isolates the bit contributed by the single outlier, which is accumulated back into `ans`.

**Complexity:** Time O(n * 32) matrix scan, Space O(1).

```cpp
    int singleElement(int arr[] ,int N) {
        int ans=0;
        for(int i=0;i<32;i++){
            int cnt=0;
            for(int j=0;j<N;j++){
                if(arr[j]>>i & 1) cnt++;
            }
            if(cnt%3==1){
                ans+=(1<<i);
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/find-element-occuring-once-when-all-other-are-present-thrice/1
```

---

## 15. Find first set bit

**Q:** Find the 1-based position of the lowest set bit of an integer `n`.

**A:** Isolate the lowest set bit and find its index using log2.

**Explanation:**
- The expression `n & -n` exploits two's complement to yield a value with only the lowest set bit preserved.
- The `log2` function efficiently computes the power of 2, mapping the isolated bit to its 0-based index.
- Adding 1 converts this index to the required 1-based position.

**Complexity:** Time O(1), Space O(1).

```cpp
    unsigned int getFirstSetBit(int n){
        // Your code here
        
        // int ans=1;
        // while((n&1)!=1){
        //     ans++;
        //     n=n>>1;
        // }
        // return ans;
        
        int mask=(n & -n);
        return 1+log2(mask);
        
    }
    
    Link - https://practice.geeksforgeeks.org/problems/find-first-set-bit-1587115620/1
```

---

## 16. Find position of set bit

**Q:** Given a number `N` with exactly one set bit, find its 1-based position, or return -1 otherwise.

**A:** Validate it's a power of 2, then use log2 on the isolated bit.

**Explanation:**
- The condition `N && !(N & N-1)` strictly checks if the number has exactly one set bit (is a power of 2).
- If true, `N & -N` safely extracts that bit, and `log2` yields the 0-based position.
- Adding 1 returns the 1-based index, while the fallback handles invalid inputs.

**Complexity:** Time O(1), Space O(1).

```cpp
    int findPosition(int N) {
        if(N && !(N & N-1)){
            return 1+log2(N & -N);
        }else{
            return -1;
        }
    }
    
    Link - https://practice.geeksforgeeks.org/problems/find-position-of-set-bit3706/1
```

---

## 17. Longest Consecutive 1's

**Q:** Find the length of the longest contiguous block of 1s in the binary representation of `N`.

**A:** Right-shift and count consecutive true bits.

**Explanation:**
- Continuously extracts the least significant bit with `N & 1`.
- Increments a running count if the bit is 1, updating the maximum found so far.
- Resets the count to 0 whenever a 0 bit is encountered, shifting `N` right each iteration until it is depleted.

**Complexity:** Time O(32) max loop, Space O(1).

```cpp
    int maxConsecutiveOnes(int N){
        int ans=0;
        int cnt=0;
        while(N){
            if(N&1){
                cnt++;
                ans=max(ans,cnt);
            }
            else{
                cnt=0;
            }
            N=N>>1;
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/longest-consecutive-1s-1587115620/1
```

---

## 18. Maximum AND Value

**Q:** Find the maximum possible bitwise AND value among any pair of elements in an array.

**A:** Greedy bit-matching from most significant to least significant.

**Explanation:**
- Scans bit positions from 31 down to 0, attempting to include the current bit in the final answer `ans | (1 << i)`.
- It counts how many elements in the array contain all the set bits in this proposed `ans` mask.
- If at least two elements match the proposed mask, that bit is safely locked into the final answer.

**Complexity:** Time O(n * 32) optimal and O(n^2) brute force, Space O(1).

```cpp
// Brute Force...
    int maxAND (int arr[], int N){
        int ans=0;
        for(int i=0;i<N;i++){
            for(int j=i+1;j<N;j++){
                ans=max(ans,arr[i]&arr[j]);
            }
        }
        return ans;
    }
    
// Optimized...
    int countP(int p,int arr[],int n){
        int c=0;
        for(int i=0;i<n;i++){
            if((p&arr[i])==p) c++;
        }
        return c;
    }
    int maxAND (int arr[], int N){
        int ans=0;
        for(int i=31;i>=0;i--){
            int count=countP(ans|(1<<i),arr,N);
            if(count>1){
                ans=ans|(1<<i);
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/maximum-and-value-1587115620/1
```

---

## 19. Missing number in array

**Q:** Find the one integer missing from the sequence 1 to `n` in an array of size `n-1`.

**A:** Full sequence XOR canceled against the actual array.

**Explanation:**
- Calculates the XOR sum of all ideal values from 1 to `n`.
- Calculates the XOR sum of all actual elements present in the array.
- Since pairs of identical values cancel out to 0, combining both XOR sums perfectly leaves the missing number.

**Complexity:** Time O(n), Space O(1).

```cpp
  int MissingNumber(vector<int>& array, int n) {
        // int s=n*(n+1)/2;
        // for(auto e : array){
        //     s-=e;
        // }
        // return s;
        
        int xorsn=0;
        for(int i=1;i<=n;i++) xorsn^=i;
        for(auto it : array) xorsn^=it;
        return xorsn;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/missing-number-in-array1416/1
```

---

## 20. Non Repeating Numbers

**Q:** Find the two elements that appear exactly once in an array where all other elements appear exactly twice.

**A:** Global XOR parity split using the lowest set bit.

**Explanation:**
- XORs all elements to get `xors`, which equals the XOR of the two unique targets.
- Uses `rms = xors & -xors` to isolate the rightmost bit where the two targets differ.
- Scans the array again, XORing elements into `n1` only if they share that specific set bit, separating one target from the other.

**Complexity:** Time O(n) two-pass, Space O(1).

```cpp
    vector<int> singleNumber(vector<int> nums){
        // Code here.
        vector<int> ans;
        int xors=0;
        for(auto i : nums){
            xors=xors^i;
        }
        int rms=xors & -xors;
        int n1=0;
        for(auto it : nums){
            if(it & rms){
                n1=n1^it;
            }
        }
        ans.push_back(n1);
        ans.push_back(xors^n1);
        
        if(ans[0]>ans[1]){
            swap(ans[0],ans[1]);
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/finding-the-numbers0215/1
```

---

## 21. Number is sparse or not

**Q:** Check if a number is "sparse," meaning no two or more consecutive bits are set to 1.

**A:** Right-shift iteration looking for adjacent set bits.

**Explanation:**
- Evaluates the lowest bit `n & 1` and increments a streak counter if it is 1, otherwise resetting to 0.
- If the consecutive counter reaches 2, the number fails the sparse condition and returns false.
- Shifts `n` right repeatedly until depleted.

**Complexity:** Time O(32) max loop, Space O(1).

```cpp
    bool isSparse(int n){
        int cnt=0;
        while(n){
            if(n&1) cnt++;
            else cnt=0;
            if(cnt>=2) return false;
            n=n>>1;
        }
        return true;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/number-is-sparse-or-not-1587115620/1
```

---

## 22. Number of 1 Bits

**Q:** Count the number of set bits (1s) in the binary representation of an integer `N`.

**A:** Strip the lowest set bit iteratively.

**Explanation:**
- Reuses Brian Kernighan's algorithm `N = N & (N - 1)` which removes the lowest 1-bit in each step.
- An accumulator counts how many such stripping operations occur before the number becomes 0.
- This loop runs strictly in proportion to the number of set bits, not the integer width.

**Complexity:** Time O(k) set bits, Space O(1).

```cpp
    int setBits(int N) {
        int c=0;
        while(N){
            c++;
            N=(N & N-1);
        }
        return c;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/set-bits0143/1
```

---

## 23. Party of Couples

**Q:** Find the single person (element) without a partner (duplicate) in an array.

**A:** Full array cumulative XOR.

**Explanation:**
- Iterates over all elements and accumulates their bitwise XOR.
- Because `a ^ a = 0` and `a ^ 0 = a`, every paired couple cancels each other out regardless of array order.
- The lone element is left over in the accumulator.

**Complexity:** Time O(n), Space O(1).

```cpp
    int findSingle(int N, int arr[]){
        // code here
        int ans=arr[0];
        for(int i=1;i<N;i++){
            ans=ans^arr[i];
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/alone-in-couple5507/1
```

---

## 24. Power Of 2 and Subsequences

**Q:** Calculate the number of subsequences where every element is a power of 2.

**A:** Count qualifying elements and compute 2^count - 1 modulo 1e9+7.

**Explanation:**
- Scans the array applying `!(A[i] & (A[i] - 1))` to count elements that are powers of 2 (assuming A[i] > 0).
- If there are `cnt` valid elements, any non-empty combination of them is a valid subsequence.
- The total combinations are `2^cnt - 1`, generated by iterative modular multiplication.

**Complexity:** Time O(n) array scan, Space O(1).

```cpp
    long long numberOfSubsequences(int N, long long A[]){
        int mod=1e9+7;
        int cnt=0;
        for(int i=0;i<N;i++){
            if(!(A[i] & (A[i]-1))) cnt++;
        }
        long long ans=1;
        for(int i=1;i<=cnt;i++){
            ans=(ans*2)%mod;
        }
        return ans-1;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/power-of-2-and-subsequences0759/1
```

---

## 25. Power Set

**Q:** Generate all possible subsequences (power set) of a given string `s` in lexicographical order.

**A:** Integer mask generation iterating from 1 to 2^n - 1.

**Explanation:**
- Loops from 1 to `(1 << n) - 1`, treating each loop index as a bitmask where the `j`-th bit controls the inclusion of `s[j]`.
- Inside the loop, it checks `i & (1 << j)` to selectively extract characters and build the string subset.
- Finally, it sorts the list of subsequences to satisfy the lexicographical order constraint.

**Complexity:** Time O(n * 2^n) subset generation and sorting, Space O(n * 2^n) output list.

```cpp
vector<string> AllPossibleStrings(string s){
    int n=s.size();
    vector<string> ans;
    for(int i=1;i<(1<<n);i++){
	string t="";
	for(int j=0;j<n;j++){
	    if(i & (1<<j)){
		t.push_back(s[j]);
	    }
	}
	ans.push_back(t);
    }
    sort(ans.begin(),ans.end());
    return ans;
}
    
Link - https://practice.geeksforgeeks.org/problems/power-set4302/1
```

---

## 26. Power of 2

**Q:** Determine if a given number `n` is a power of 2.

**A:** Check if the integer has exactly one bit set.

**Explanation:**
- Any perfect power of 2 contains precisely one 1-bit in its binary format.
- The expression `n & (n - 1)` removes the lowest set bit. If `n` was a power of 2, this zeroes the number completely.
- A non-zero check `n &&` ensures 0 itself is correctly rejected.

**Complexity:** Time O(1), Space O(1).

```cpp
    bool isPowerofTwo(long long n){
       return (n && !(n & n-1));
    }
    
    Link  - https://practice.geeksforgeeks.org/problems/power-of-2-1587115620/1
```

---

## 27. Reverse Bits

**Q:** Reverse the 32 bits of an unsigned integer.

**A:** Iterate 32 times, extracting and shifting bits into the inverted position.

**Explanation:**
- Uses a test `mask` (initially 1) and shifts it left to scan each bit of `X`.
- If the current bit is set, it performs `ans | 1` to push a 1 into the bottom of the result.
- Before testing the next bit, the answer is shifted left `ans << 1`, pushing the accumulated bits higher.

**Complexity:** Time O(32) fixed loop, Space O(1).

```cpp
    long long reversedBits(long long X) {
        // code here
        long long ans=0;
        int mask=1;
        for(int i=0;i<32;i++){
            if(X&mask){
                ans=ans|1;
            }
            mask=mask<<1;
            if(i==31) continue;
            ans=ans<<1;
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/reverse-bits3556/1
```

---

## 28. Rightmost different bit

**Q:** Find the 1-based position of the rightmost bit that differs between two integers `m` and `n`.

**A:** XOR to isolate differences, then extract the lowest set bit.

**Explanation:**
- `m ^ n` produces a temporary integer with 1s at every differing bit position.
- If they are identical (XOR yields 0), it correctly returns -1.
- `temp & -temp` isolates the lowest bit, and `1 + log2` converts it to a 1-based index.

**Complexity:** Time O(1), Space O(1).

```cpp
    int posOfRightMostDiffBit(int m, int n){
        // Your code here
        int temp=m^n;
        if(temp==0) return -1;
        return 1+log2(temp & -temp);
    }
    
    Link - https://practice.geeksforgeeks.org/problems/rightmost-different-bit-1587115621/1
```

---

## 29. Set kth bit

**Q:** Set the `K`-th bit of an integer `N` to 1.

**A:** Mask creation with a left shift and bitwise OR.

**Explanation:**
- `1 << K` produces a mask with a 1 positioned precisely at the `K`-th index.
- The `|` (OR) operator forces the `K`-th bit in `N` to become 1 while leaving all other bits unaffected.

**Complexity:** Time O(1), Space O(1).

```cpp
    int setKthBit(int N, int K){
        return (N | (1<<K));
    }
    
    Link - https://practice.geeksforgeeks.org/problems/set-kth-bit3724/1
```

---

## 30. Swap all odd and even bits

**Q:** Swap all adjacent odd and even bits in an unsigned 32-bit integer.

**A:** Apply hex masks for extraction, shift in opposite directions, and combine.

**Explanation:**
- Extracts all even-positioned bits using the alternating mask `0xAAAAAAAA`.
- Extracts all odd-positioned bits using the complementary mask `0x55555555`.
- Shifts the even bits right by 1 and the odd bits left by 1, then recombines them with a bitwise OR.

**Complexity:** Time O(1), Space O(1).

```cpp
    unsigned int swapBits(unsigned int n){
    	unsigned int evenb = n & 0xAAAAAAAA;
    	unsigned int oddb = n & 0x55555555;
    	evenb=evenb>>1;
    	oddb=oddb<<1;
    	return(evenb | oddb);
    }
    
    Link - https://practice.geeksforgeeks.org/problems/swap-all-odd-and-even-bits-1587115621/1
```

---

## 31. Toggle bits given range

**Q:** Toggle (flip) the bits of `N` strictly within the 1-based inclusive range `[L, R]`.

**A:** Iterate over the range and apply XOR at each targeted position.

**Explanation:**
- The loop runs from `L` to `R`.
- Inside, `1 << (i - 1)` builds a mask isolating each bit individually.
- `N = N ^ mask` toggles the target bit because XORing with 1 flips the bit's value.

**Complexity:** Time O(R - L) fixed width loop, Space O(1).

```cpp
    int toggleBits(int N , int L , int R) {
        for(int i=L;i<=R;i++){
            N=(N^(1<<i-1));
        }
        return N;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/toggle-bits-given-range0952/1
```

---
