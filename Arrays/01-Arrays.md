# Arrays — Q&A

> Source: `Kirancse47/DSA` -> `Arrays/` — 71 problems (plus 7 in the 2DArray subsection)

Ordering: Kept numerical/alphabetical based on filenames.

## Cheat sheet
- **Two Pointers:** Move from both ends to center (e.g., `15. 3Sum`, `11. Container With Most Water`).
- **Sliding Window:** Expand/shrink a window for contiguous subarray problems (e.g., `3. Longest Substring Without Repeating Characters`).
- **Prefix Sum:** Precompute cumulative sums for quick range sum queries (e.g., `525. Contiguous Array`).
- **Kadane's Algorithm:** Keep a running maximum for subarray sum (e.g., `53. Maximum Subarray`).
- **Boyer-Moore Majority Vote:** Find majority element in O(1) space (e.g., `169. Majority Element`, `229. Majority Element II`).
- **Dutch National Flag:** Three-way partitioning (e.g., `75. Sort Colors`).
- **Cyclic Sort / Index-as-Hash:** Place elements at `nums[i] - 1` (e.g., `41. First Missing Positive`, `442. Find All Duplicates in an Array`).
- **Merge-Sort-Based Counting:** Count inversions during merge (e.g., `493. Reverse Pairs`, `Count Inversions`).
- **Interval Merging:** Sort and merge overlapping ranges (e.g., `56. Merge Intervals`).
- **In-place Matrix Marking:** Use first row/col as markers (e.g., `73. Set Matrix Zeroes`).

---

## 1. 1. Two Sum

**Q:** Find two numbers in an array that add up to a specific target and return their indices.

**A:** Hashing / Map.

**Explanation:**
- Iterate through the array while maintaining a hash map of values seen so far to their indices.
- For each element `nums[i]`, check if the complement `target - nums[i]` exists in the map.
- If it does, we found the pair. If not, insert the current element and its index into the map.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> ans;
        int n=nums.size();
        unordered_map<int,int> mp;
        for(int i=0;i<n;i++){
            if(mp.find(target-nums[i])!=mp.end()){
                ans.push_back(mp[target-nums[i]]);
                ans.push_back(i);
                break;
            }
            mp[nums[i]]=i;
        }
        return ans;
    }
```

---

## 2. 3. Longest Substring Without Repeating Characters

**Q:** Find the length of the longest substring without repeating characters.

**A:** Sliding Window with Frequency Map.

**Explanation:**
- Use two pointers `i` and `j` to represent the current window and expand it by moving `j`.
- Keep track of character frequencies in a hash map; the window size should equal the map size if all characters are unique.
- If the window size exceeds the map size (meaning duplicates exist), shrink the window from the left by incrementing `i` until all characters are unique again.

**Complexity:** Time `O(N)`, Space `O(min(N, M))` where M is the charset size.

```cpp
int lengthOfLongestSubstring(string s) {
           int i=0,j=0;
            int ans=0;
            unordered_map<char,int> mp;
            while(j<s.size()){
                mp[s[j]]++;   
                if(mp.size()==j-i+1){
                    ans=max(ans,j-i+1);
                }
                else{
                    while(mp.size()<j-i+1){
                        mp[s[i]]--;
                        if(mp[s[i]]==0){
                            mp.erase(s[i]);
                        }
                        i++;
                    }
                }
                j++;
            }
            return ans;
    }
```

---

## 3. 15. 3Sum

**Q:** Find all unique triplets in an array which give the sum of zero.

**A:** Sorting + Two Pointers.

**Explanation:**
- Sort the array first. Iterate `i` from `0` to `n-1`, treating `nums[i]` as the first element of the triplet.
- Use two pointers `l` (left) and `h` (right) to find the remaining two elements that sum to `-nums[i]`.
- Skip duplicate elements for `i`, `l`, and `h` to ensure only unique triplets are added to the result.

**Complexity:** Time `O(N^2)`, Space `O(1)` (excluding output).

```cpp
vector<vector<int>> threeSum(vector<int>& nums) {
        sort(nums.begin(),nums.end());
        int n=nums.size();
        vector<vector<int>>ans;
        for(int i=0;i<n;i++){
            if (nums[i] > 0) break;
                    if (i > 0 && nums[i] == nums[i - 1]) continue;
            int l=i+1,h=n-1;
            while(l<h){
                int cur=nums[i]+nums[l]+nums[h];
                if(cur==0){
                    ans.push_back({nums[i],nums[l],nums[h]});
                    while(l < h && nums[h] == nums[h - 1])  h--;
                                    while(l < h && nums[l] == nums[l + 1])  l++;
                                    l++, h--;
                }else if(cur>0){
                    h--;
                }else{
                    l++;
                }
            }
        }
        return ans;
    }
```

---

## 4. 16. 3Sum Closest

**Q:** Find three integers in an array such that the sum is closest to a given target.

**A:** Sorting + Two Pointers.

**Explanation:**
- Sort the array and iterate `i` through the elements, maintaining a minimum difference (`temp`) between the triplet sum and the target.
- Use two pointers `j` and `k` to find the remaining two elements.
- Move the pointers based on whether the current sum is less than or greater than the target to close the gap.

**Complexity:** Time `O(N^2)`, Space `O(1)`.

```cpp
int threeSumClosest(vector<int>& nums, int target) {
        sort(nums.begin(),nums.end());
        int temp=INT_MAX,ans;
        for(int i=0;i<nums.size();i++){
            int j=i+1,k=nums.size()-1;
            while(j<k){
                int sum=nums[i]+nums[j]+nums[k];
                if(sum==target) return sum;
                if(abs(sum-target)<temp){
                    temp=abs(sum-target);
                    ans=sum;
                }
                if(sum<target) j++;
                else k--;
            }
        }
        return ans;
    }
```

---

## 5. 18. 4Sum

**Q:** Find all unique quadruplets in an array which give the sum of a target `k`.

**A:** Sorting + Two Pointers (Nested Loops).

**Explanation:**
- Sort the array to easily skip duplicates and use the two-pointer technique.
- Use two nested loops for the first two elements (`i` and `j`), skipping duplicates for each.
- Use two pointers `lo` and `hi` for the remaining elements, moving them to match the target and skipping duplicate values when a valid quad is found.

**Complexity:** Time `O(N^3)`, Space `O(1)` (excluding output).

```cpp
vector<vector<int>> fourSum(vector<int>& arr, int k) {
        vector<vector<int>> ans;
        int n=arr.size();
        if(n<4) return ans;
        sort(arr.begin(),arr.end());
        vector<int> res(4);
        for(int i=0;i<n;i++){
            for(int j=i+1;j<n;j++){
                int lo=j+1,hi=n-1;
                while(lo<hi){
                    int t1=k-arr[i]-arr[j];
                    int t2=arr[lo]+hi];
                    if(t1==t2){
                        res[0]=arr[i];res[1]=arr[j];res[2]=arr[lo];res[3]=arr[hi];
                        ans.push_back(res);
                        while(lo<hi && arr[lo]==res[2]) lo++;
                        while(lo<hi && arr[hi]==res[3]) hi--;
                    }else if(t2<t1){
                        lo++;
                    }else{
                        hi--;
                    }
                }
                while(j+1<n && arr[j+1]==arr[j]) j++;
            }
            while(i+1<n && arr[i+1]==arr[i]) i++;
        }
        return ans;
    }
```

---

## 6. 26. Remove Duplicates from Sorted Array

**Q:** Remove duplicates from a sorted array in-place such that each element appears only once and return the new length.

**A:** Two Pointers.

**Explanation:**
- Use pointer `i` to scan through the array starting from the second element.
- Use pointer `j` to keep track of the position where the next unique element should be placed.
- If `nums[i]` is different from `nums[i-1]`, it is a new unique element. Place it at `nums[j]` and increment `j`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
 int removeDuplicates(vector<int>& nums) {
        if(nums.size()==0){
            return 0;
        }
        int j=1;
        for(int i=1;i<nums.size();i++){
            if(nums[i]!=nums[i-1]){
                nums[j]=nums[i];
                j++;
            }
        }
        return j;
    }
```

---

## 7. 31. Next Permutation

**Q:** Find the lexicographically next greater permutation of numbers.

**A:** Right-to-Left Scan and Swap.

**Explanation:**
- Find the first element `nums[i]` from the right that is smaller than its adjacent next element `nums[i+1]`.
- If no such element exists, the array is in descending order; reverse it to get the smallest permutation.
- Otherwise, find the smallest element `nums[j]` to the right of `i` that is greater than `nums[i]`.
- Swap `nums[i]` and `nums[j]`, then reverse the subarray to the right of `i` to get the smallest lexicographical order for the suffix.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
void nextPermutation(vector<int>& nums) {
        int n=nums.size(),i,j;
        for(i=n-2;i>=0;i--){
            if(nums[i]<nums[i+1]){
                break;
            }
        }
        if(i<0){
            reverse(nums.begin(),nums.end());
            return;    
        }
        for(j=n-1;j>=0;j--){
            if(nums[j]>nums[i]){ 
                break;
            }
        }
        swap(nums[i],nums[j]);
        reverse(nums.begin()+i+1,nums.end());
    }
```

---

## 8. 41. First Missing Positive

**Q:** Find the smallest missing positive integer in an unsorted array.

**A:** In-place Hashing (Index as Hash).

**Explanation:**
- First, replace all elements less than `1` or greater than `N` with `1`. Also check if `1` is present; if not, return `1`.
- Use the array elements as indices: for each element `nums[i]`, mark the value at index `abs(nums[i]) - 1` as negative.
- Scan the array again; the first index `i` with a positive value corresponds to the missing integer `i + 1`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int firstMissingPositive(vector<int>& nums) {
        int n=nums.size();
        bool one=false;
        for(int i=0;i<n;i++){
            if(nums[i]==1)
                one=true;
            else if(nums[i]<1 || nums[i]>n)
                nums[i]=1;
        }
        if(!one){
            return 1;
        }
        for(int i=0;i<n;i++){
            int idx=abs(nums[i])-1;
            nums[idx]=-abs(nums[idx]);
        }
        for(int i=0;i<n;i++){
            if(nums[i]>0)
                return i+1;
        }
        return n+1;
    }
```

---

## 9. 42. Trapping Rain Water

**Q:** Calculate how much water can be trapped between elevation bars.

**A:** Prefix/Suffix Max arrays (Space) and Two Pointers (In-place).

**Explanation:**
- The first approach uses two arrays `l` and `r` to store the maximum height to the left and right of each bar, respectively.
- The water trapped above each bar is `min(l[i], r[i]) - a[i]`.
- The second approach uses two pointers `l` and `r` and maintains `lmax` and `rmax`.
- Depending on which is smaller, it updates the trapped water at the respective pointer and moves it inward.

**Complexity:** First approach: Time `O(N)`, Space `O(N)`. Second approach: Time `O(N)`, Space `O(1)`.

```cpp
// With space...
    int trap(vector<int>& a) {
        int n=a.size();
        vector<int> l(n),r(n);

        l[0]=a[0];
        for(int i=1;i<n;i++){
            l[i]=max(a[i],l[i-1]);
        }

        r[n-1]=a[n-1];
        for(int i=n-2;i>=0;i--){
            r[i]=max(a[i],r[i+1]);
        }

        int w=0;
        for(int j=1;j<n-1;j++){
            w+=(min(l[j],r[j])-a[j]);
        }
        return w;
    }
    
// Without Space...
    int trap(vector<int>& a) {
        int n=a.size();
        int ans=0;
        int l=0,r=n-1;
        int lmax=a[l],rmax=a[r];
        while(l<=r){
            if(a[l]<=a[r]){
                if(a[l]>=lmax) lmax=a[l];
                else ans+=lmax-a[l];
                l++;
            }
            else{
                if(a[r]>=rmax) rmax=a[r];
                else ans+=rmax-a[r]; 
                r--;
            }
        }
        return ans;
    }
```

---

## 10. 48. Rotate Image

**Q:** Rotate an N x N 2D matrix by 90 degrees clockwise in-place.

**A:** Transpose and Reverse.

**Explanation:**
- First, compute the transpose of the matrix by swapping `m[i][j]` with `m[j][i]`.
- Then, reverse each row of the matrix.
- This two-step process effectively rotates the entire image by 90 degrees.

**Complexity:** Time `O(N^2)`, Space `O(1)`.

```cpp
void rotate(vector<vector<int>>& m) {
        int n=m.size();
        for(int i=0;i<n;i++){
            for(int j=0;j<i;j++){
                swap(m[i][j],m[j][i]);
            }
        }
        for(auto &i : m){
            reverse(i.begin(),i.end());
        }
```

---

## 11. 53. Maximum Subarray

**Q:** Find the contiguous subarray with the largest sum and return its sum.

**A:** Kadane's Algorithm / DP.

**Explanation:**
- The DP approach computes the maximum subarray ending at each position by either starting fresh or appending to the previous subarray sum.
- Kadane's algorithm keeps a running sum (`currmax`) of the current subarray.
- If the running sum becomes negative, it resets to zero, because a negative prefix will only decrease the sum of any future subarray.

**Complexity:** DP: Time `O(N)`, Space `O(N)`. Kadane's: Time `O(N)`, Space `O(1)`.

```cpp
// Using DP....
    int maxSubArray(vector<int>& nums) {
        int n=nums.size();
        vector<int> dp(n+1,0);
        int ans=INT_MIN;
        for(int i=1;i<n+1;i++){
            dp[i]=max(nums[i-1],nums[i-1]+dp[i-1]);
            ans=max(ans,dp[i]);
        }
        return ans;
    }
    
// Kadarn's algo...
    int maxSubArray(vector<int>& nums) {
        int currmax = 0, max = INT_MIN;
        
        for(int i = 0; i < nums.size(); i++){
            currmax += nums[i];
            if(max < currmax) max = currmax;
            if(currmax < 0) currmax = 0;
        }
        return max;
    }
```

---

## 12. 54. Spiral Matrix

**Q:** Return all elements of an m x n matrix in spiral order.

**A:** Layer-by-Layer Traversal / Boundary tracking.

**Explanation:**
- Define four boundaries: row start (`rs`), row end (`re`), column start (`cs`), and column end (`ce`).
- Iterate through the top, right, bottom, and left boundaries in a while loop until all elements are visited.
- After traversing each boundary, adjust the corresponding boundary variable inwards.

**Complexity:** Time `O(N * M)`, Space `O(1)` (excluding output).

```cpp
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        int r=matrix.size();
        int c=matrix[0].size();
        vector<int> ans;
        int rs=0,re=r-1,cs=0,ce=c-1;
        int cnt=0,tcnt=r*c;
        while(cnt<tcnt){
            //top
            for(int i=rs,j=cs;j<=ce && cnt<tcnt;j++){
                ans.push_back(matrix[i][j]);
                cnt++;
            }
            rs++;
            //right
            for(int i=rs,j=ce;i<=re && cnt<tcnt;i++){
                ans.push_back(matrix[i][j]);
                cnt++;
            }
            ce--;
            //down
            for(int i=re,j=ce;j>=cs && cnt<tcnt;j--){
                ans.push_back(matrix[i][j]);
                cnt++;
            }
            re--;
            //left
            for(int i=re,j=cs;i>=rs && cnt<tcnt;i--){
                ans.push_back(matrix[i][j]);
                cnt++;
            }
            cs++;
        }
        return ans;
    }
```

---

## 13. 56. Merge Intervals

**Q:** Merge all overlapping intervals in an array.

**A:** Sorting and Single Pass Merging.

**Explanation:**
- First, sort the intervals by their start times to ensure overlaps are contiguous.
- Iterate through the intervals, maintaining a current interval `t`.
- If the next interval's start is less than or equal to `t`'s end, they overlap, so update `t`'s end to the maximum of both.
- Otherwise, push `t` to the answer and set `t` to the new interval.

**Complexity:** Time `O(N log N)`, Space `O(1)` (or `O(N)` for sorting).

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
        vector<vector<int>> ans;
        if(intervals.size()==0){
            return ans;
        }

        sort(intervals.begin(),intervals.end());
        vector<int> t=intervals[0];
        for(auto it : intervals){
            if(it[0]<=t[1]){
                t[1]=max(t[1],it[1]);
            }else{
                ans.push_back(t);
                t=it;
            }
        }
        ans.push_back(t);
        return ans;
    }
```

---

## 14. 57. Insert Interval

**Q:** Insert a new interval into a sorted array of non-overlapping intervals and merge if necessary.

**A:** Linear Scan and Merge.

**Explanation:**
- Skip all intervals that end before the new interval starts, adding them to the result.
- While intervals overlap with the new interval, merge them by continuously updating the new interval's start and end times.
- Finally, add the merged new interval, followed by all remaining intervals.

**Complexity:** Time `O(N)`, Space `O(1)` (excluding output).

```cpp
    vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newInterval) {
        vector<vector<int>> ans;
        int i=0,n=intervals.size();
        while(i<n && intervals[i][1]<newInterval[0]){
            ans.push_back(intervals[i]);
            i++;
        }
        while(i<n && newInterval[1]>=intervals[i][0]){
            newInterval[0]=min(newInterval[0],intervals[i][0]);
            newInterval[1]=max(newInterval[1],intervals[i][1]);
            i++;
        }
        ans.push_back(newInterval);
        while(i<n){
            ans.push_back(intervals[i]);
            i++;
        }
        return ans;
    }
```

---

## 15. 62. Unique Paths

**Q:** Find the number of possible unique paths from the top-left to the bottom-right of a grid.

**A:** Dynamic Programming (Space Optimized).

**Explanation:**
- The problem can be solved using recursion, memoization, or tabulation, where `dp[i][j] = dp[i-1][j] + dp[i][j-1]`.
- The most optimal solution uses space optimization: only the previous row is needed to compute the current row.
- Initialize a `prev` array with 1s, and iteratively update a `cur` array using the values from `prev` and the current row.

**Complexity:** Space Optimized: Time `O(M * N)`, Space `O(N)`.

```cpp
// //Recursive....
    // int solve(int i,int j){
    //     if(i==0 && j==0) return 1;
    //     if(i<0 || j<0) return 0;
    //     return solve(i-1,j)+solve(i,j-1);
    // }
    // int uniquePaths(int m, int n) {
    //     return solve(m-1,n-1);
    // }

// //Memorization....
    // int solve(int i,int j,vector<vector<int>> &dp){
    //     if(i==0 && j==0) return 1;
    //     if(i<0 || j<0) return 0;
    //     if(dp[i][j]!=-1) return dp[i][j];
    //     return dp[i][j]=solve(i-1,j,dp)+solve(i,j-1,dp);
    // }
    // int uniquePaths(int m, int n) {
    //     vector<vector<int>> dp(m,vector<int>(n,-1));
    //     return solve(m-1,n-1,dp);
    // }
    
// //Tabulation...
    // int uniquePaths(int m, int n) {
    //     vector<vector<int>> dp(m,vector<int>(n,0));
    //     for(int i=0;i<m;i++){
    //         for(int j=0;j<n;j++){
    //             if(i==0 && j==0) dp[i][j]=1;
    //             else dp[i][j]= (i>0 ? dp[i-1][j] : 0) + (j>0 ? dp[i][j-1] : 0);
    //         }
    //     }
    //     return dp[m-1][n-1];        
    // }
    
//Space optimization...
    int uniquePaths(int m, int n) {
        vector<int> prev(n,1);
        for(int i=1;i<m;i++){
            vector<int> cur(n,1);
            for(int j=1;j<n;j++){
                cur[j]=cur[j-1]+prev[j];
            }
            prev=cur;
        }
        return prev[n-1];        
    }
```

---

## 16. 73. Set Matrix Zeroes

**Q:** Given an m x n integer matrix, if an element is 0, set its entire row and column to 0's.

**A:** Set/Hash based tracking.

**Explanation:**
- Iterate through the matrix to find all 0s, storing their row indices in a `row` set and column indices in a `col` set.
- Iterate through the `row` set and set every element in those rows to 0.
- Iterate through the `col` set and set every element in those columns to 0.

**Complexity:** Time `O(N * M)`, Space `O(N + M)`.

```cpp
void setZeroes(vector<vector<int>>& matrix) {
        int n=matrix.size();
        int m=matrix[0].size();
        set<int> row,col;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(matrix[i][j]==0){
                    row.insert(i);
                    col.insert(j);
                }
            }
        }
        for(auto it : row){
            for(int i=0;i<m;i++){
                matrix[it][i]=0;
            }
        }
        for(auto it : col){
            for(int i=0;i<n;i++){
                matrix[i][it]=0;
            }
        }
    }
```

---

## 17. 74. Search a 2D Matrix

**Q:** Search for a value in an m x n matrix where each row is sorted and the first integer of each row is greater than the last integer of the previous row.

**A:** Top-Right Pointer Traversal (or 2D Binary Search).

**Explanation:**
- Start at the top-right corner of the matrix (`r=0, c=m-1`).
- If the target matches the current element, return true.
- If the current element is greater than the target, move left (`c--`); if it's smaller, move down (`r++`).

**Complexity:** Time `O(N + M)`, Space `O(1)`.

```cpp
bool searchMatrix(vector<vector<int>>& matrix, int target) {
        int n=matrix.size();
        int m=matrix[0].size();
        int ans=false;
        int r=0,c=m-1;
        while(r<n && c>=0){
            if(matrix[r][c]==target){
                ans=true;
                return ans;
            }else if(matrix[r][c]>target){
                c--;
            }else{
                r++;
            }
        }
        return ans;
    }
```

---

## 18. 75. Sort Colors

**Q:** Sort an array of 0s, 1s, and 2s in-place.

**A:** Dutch National Flag Algorithm (Three Pointers).

**Explanation:**
- Use three pointers: `l` for 0s, `m` for 1s, and `h` for 2s.
- Iterate `m` until it passes `h`. If `nums[m]` is 0, swap with `l` and advance both.
- If `nums[m]` is 2, swap with `h` and decrement `h` (do not advance `m` because the swapped value needs checking).
- If `nums[m]` is 1, just advance `m`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
void sortColors(vector<int>& nums) {
        int l=0,m=0,h=nums.size()-1;
        while(m<=h){
            if(nums[m]==0){
                swap(nums[m],nums[l]);
                m++;l++;
            }else if(nums[m]==2){
                swap(nums[m],nums[h]);
                h--;
            }else{
                m++;
            }
        }
    }
```

---

## 19. 88. Merge Sorted Array

**Q:** Merge two sorted arrays `nums1` and `nums2` into `nums1` as one sorted array.

**A:** Backwards Three-Pointer Merge.

**Explanation:**
- Set pointers `i` at the end of valid elements in `nums1`, `j` at the end of `nums2`, and `k` at the end of the total capacity of `nums1`.
- Compare elements from the back; place the larger of `nums1[i]` and `nums2[j]` at `nums1[k]`.
- Decrement the respective pointers until `nums2` is completely merged.

**Complexity:** Time `O(M + N)`, Space `O(1)`.

```cpp
void merge(vector<int>& a1, int m, vector<int>& a2, int n) {
        int i=m-1,j=n-1,k=m+n-1;
        while(j>=0){
            if(i>=0 && a1[i]>a2[j]){
                a1[k]=a1[i];
                i--;
            }else{
                a1[k]=a2[j];
                j--;
            }
            k--;
        }
    }
```

---

## 20. 118. Pascal's Triangle

**Q:** Generate the first `numRows` of Pascal's triangle.

**A:** Dynamic Programming / Iterative Generation.

**Explanation:**
- Create a 2D vector `r` of size `numRows`.
- For each row `i`, resize it to `i+1` and set the first and last elements to 1.
- For inner elements, the value is the sum of the two elements directly above it: `r[i-1][j-1] + r[i-1][j]`.

**Complexity:** Time `O(N^2)`, Space `O(N^2)`.

```cpp
vector<vector<int>> generate(int numRows) {
        vector<vector<int>> r(numRows);
        for(int i=0;i<numRows;i++){
            r[i].resize(i+1);
            r[i][0]=r[i][i]=1;
            for(int j=1;j<i;j++)
                r[i][j]=r[i-1][j-1]+r[i-1][j];
        }
        return r;
    }
```

---

## 21. 121. Best Time to Buy and Sell Stock

**Q:** Find the maximum profit achievable by buying and selling a stock once.

**A:** Single Pass (Running Minimum).

**Explanation:**
- Iterate through the prices array.
- Keep track of the minimum price seen so far (`minPrice`).
- At each step, update the maximum profit by calculating the difference between the current price and `minPrice`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
int maxProfit(vector<int>& prices) {
        int n=prices.size();
        int profit=0,minPrice=prices[0];
        for(int i=1;i<n;i++){
            profit=max(profit,prices[i]-minPrice);
            minPrice=min(minPrice,prices[i]);
        }
        return profit;
    }
```

---

## 22. 128. Longest Consecutive Sequence

**Q:** Find the length of the longest consecutive elements sequence in an unsorted array.

**A:** Hashing (Hash Map/Set).

**Explanation:**
- Store all numbers in a hash map.
- Iterate through the map, and if a number `it-1` exists, mark the current number as `false` since it cannot be the start of a sequence.
- For every number marked `true` (which is the start of a sequence), count consecutive numbers upwards (`n++`) using the map and update the maximum length.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
int longestConsecutive(vector<int>& nums) {
        int ans=0;
        unordered_map<int,bool> mp;
        for(auto it : nums){
            mp[it]=true;
        }
        for(auto it : nums){
            if(mp.find(it-1)!=mp.end()){
                mp[it]=false;
            }
        }
        for(auto it : mp){
            if(it.second){
                int curl=1;
                int n=it.first+1;
                while(mp.find(n)!=mp.end()){
                    curl++;
                    n++;
                }
                ans=max(ans,curl);
            }
        }
        return ans;
    }
```

---

## 23. 169. Majority Element

**Q:** Find the majority element that appears more than `n/2` times.

**A:** Boyer-Moore Voting Algorithm.

**Explanation:**
- Maintain a candidate (`ans`) and a `count`.
- If the next element matches the candidate, increment `count`; otherwise, decrement it.
- If `count` drops to 0, pick the current element as the new candidate and reset `count` to 1.
- Finally, do a second pass to verify if the candidate truly appears more than `n/2` times (though guaranteed by problem constraints, it's safer).

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
int majorityElement(vector<int>& nums) {
        int n=nums.size();
        int count=1;
        int ans=0;
        for(int i=1;i<n;i++){
            if(nums[i]==nums[ans])
                count++;
            else
                count--;
            if(count==0){
                count=1;
                ans=i;
            }
        }

        count=0;
        for(int i=0;i<n;i++){
            if(nums[ans]==nums[i])
                count++;
        }
        if(count>n/2)
            return nums[ans];
        else
            return -1;
    }
```

---

## 24. 189. Rotate Array

**Q:** Rotate an array to the right by `k` steps in-place.

**A:** Array Reversal (Three Reversals).

**Explanation:**
- First, calculate the effective rotations `k = k % n` to avoid unnecessary full rotations.
- Reverse the first `n-k` elements of the array.
- Reverse the last `k` elements of the array.
- Finally, reverse the entire array to achieve the rotated result.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    void reverse(vector<int>&a,int i,int j){
        while(i<j){
            swap(a[i],a[j]);
            i++;j--;
        }
    }
    void rotate(vector<int>& nums, int k) {
        int n=nums.size();
        if(n==0 || n==1){
            return;
        }
        k=k%n;
        if(k==n || k==0){
            return;
        }
        reverse(nums,0,n-1-k);
        reverse(nums,n-k,n-1);
        reverse(nums,0,n-1);
    }
```

---

## 25. 229. Majority Element II

**Q:** Find all elements that appear more than `n/3` times.

**A:** Boyer-Moore Voting Algorithm (Extended).

**Explanation:**
- Maintain two candidates (`y` and `z`) and two counters (`cy` and `cz`).
- Update candidates and counts similarly to the standard Boyer-Moore algorithm, but managing two possible majority elements.
- If a new element doesn't match either and both counts are non-zero, decrement both counts.
- Do a second pass to verify if the final candidates actually appear more than `n/3` times.

**Complexity:** Map approach: Time `O(N log N)` or `O(N)`, Space `O(N)`. Voting approach: Time `O(N)`, Space `O(1)`.

```cpp
// vector<int> majorityElement(vector<int>& nums) {
    //     int n=nums.size();
    //     vector<int> ans;
    //     map<int,int> mp;
    //     for(auto it : nums) mp[it]++;
    //     for(auto it : mp){
    //         if(it.second>n/3) ans.push_back(it.first);
    //     }
    //     return ans;
    // }
    
    vector<int> majorityElement(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans;
        int y=-1,z=-1,cy=0,cz=0;
        for(auto x : nums){
            if(x==y) cy++;
            else if(x==z) cz++;
            else if(cy==0) y=x,cy=1;
            else if(cz==0) z=x,cz=1;
            else cy--,cz--;
        }
        cy=0;cz=0;
        for(auto x : nums){
            if(x==y) cy++;
            else if(x==z) cz++;
        }
        if(cy>n/3) ans.push_back(y);
        if(cz>n/3) ans.push_back(z);
        return ans;
    }
```

---

## 26. 238. Product of Array Except Self

**Q:** Return an array where each element is the product of all other elements, without using division.

**A:** Prefix and Suffix Products (Space Optimized).

**Explanation:**
- First, calculate the prefix products and store them directly in the answer array.
- Initialize a variable `rightP` to store the running suffix product, starting from the rightmost element.
- Traverse the array backwards, multiplying the prefix product stored in `ans[i]` by `rightP`, and update `rightP`.

**Complexity:** Time `O(N)`, Space `O(1)` (excluding output).

```cpp
vector<int> productExceptSelf(vector<int>& nums) {
        int n=nums.size();
        vector<int>ans(n);
        ans[0]=1;
        for(int i=1;i<n;i++){
            ans[i]=ans[i-1]*nums[i-1];
        }
        
        int rightP=nums[n-1];
        for(int i=n-2;i>=0;i--){
            ans[i]=ans[i]*rightP;
            rightP=rightP*nums[i];
        }
        return ans;        
    }
```

---

## 27. 283. Move Zeroes

**Q:** Move all 0s to the end of the array while maintaining the relative order of the non-zero elements.

**A:** Two Pointers.

**Explanation:**
- Use pointer `j` to keep track of the position where the next non-zero element should be placed.
- Iterate through the array with pointer `i`.
- When a non-zero element is found at `i`, swap it with the element at `j` and increment `j`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    void moveZeroes(vector<int>& nums) {
        int n = nums.size();
        int j = 0;
        for(int i=0; i<n; i++){
            if(nums[i]!=0){
                swap(nums[i],nums[j]);
                j++;
            }
        }    
    }
```

---

## 28. 287. Find the Duplicate Number

**Q:** Find the duplicate number in an array containing `n + 1` integers where each integer is in the range `[1, n]`.

**A:** Floyd's Cycle Detection / Index Negation.

**Explanation:**
- The first approach uses Floyd's Tortoise and Hare algorithm to detect a cycle caused by the duplicate pointer, then finds the cycle's entry point.
- The second approach modifies the array by treating values as indices and negating the value at `abs(nums[i]) - 1`.
- If the value at that index is already negative, we have found the duplicate.

**Complexity:** Pointer approach: Time `O(N)`, Space `O(1)`. Negation approach: Time `O(N)`, Space `O(1)` (but modifies array).

```cpp
// Using slow and first pointer...
    int findDuplicate(vector<int>& nums) {
        int s=nums[0],f=nums[0];
        do{
            s=nums[s];
            f=nums[nums[f]];
        }while(s!=f);

        f=nums[0];
        while(f!=s){
            f=nums[f];
            s=nums[s];
        }
        return s;
    }
    
// Using negation method....
    int findDuplicate(vector<int>& nums) {
        for(int i=0;i<nums.size();i++){
            if(nums[abs(nums[i])-1]<0) return abs(nums[i]);
            else nums[abs(nums[i])-1]*=(-1);
        }
        return 0;
    }
```

---

## 29. 435. Non-overlapping Intervals

**Q:** Find the minimum number of intervals to remove to make the rest non-overlapping.

**A:** Greedy / Sorting.

**Explanation:**
- Sort the intervals primarily by start time.
- Use pointers `l` and `r` to compare adjacent intervals.
- If they overlap (`intervals[l][1] > intervals[r][0]`), we must remove one. Increment `count` and keep the interval that ends earlier (i.e., smaller end time) to minimize future overlaps.
- If they don't overlap, simply advance the `l` pointer to `r`.

**Complexity:** Time `O(N log N)`, Space `O(1)` (or `O(N)` for sorting).

```cpp
int eraseOverlapIntervals(vector<vector<int>>& intervals) {
        sort(intervals.begin(),intervals.end());
        int l=0,r=1;
        int count=0,n=intervals.size();
        while(r<n){
            if(intervals[l][1] <= intervals[r][0]){
                l=r;
                r++;          
            }else if(intervals[l][1] > intervals[r][1]){
                l=r;
                count++;
                r++;
            }else if(intervals[l][1] <= intervals[r][1]){
                count++;
                r++;
            }
        }
        return count;
    }
```

---

## 30. 442. Find All Duplicates in an Array

**Q:** Find all elements that appear twice in an array of integers in the range `[1, n]`.

**A:** In-place Hashing (Index Negation).

**Explanation:**
- Iterate through the array and use the absolute value of each element minus 1 as an index.
- If the number at that index is already negative, the current absolute value is a duplicate.
- Otherwise, negate the number at that index to mark it as seen.

**Complexity:** Time `O(N)`, Space `O(1)` (excluding output).

```cpp
    vector<int> findDuplicates(vector<int>& nums) {
        vector<int> ans;
        for(int i=0;i<nums.size();i++){
            if(nums[abs(nums[i])-1]<0) ans.push_back(abs(nums[i]));
            else nums[abs(nums[i])-1]*=(-1);
        }
        return ans;
    }
```

---

## 31. 485. Max Consecutive Ones

**Q:** Find the maximum number of consecutive 1s in a binary array.

**A:** Single Pass (State Reset).

**Explanation:**
- Iterate over the array, maintaining a counter `count` of consecutive 1s.
- If the current element is 1, increment the counter and update the maximum `ans`.
- If the current element is 0, reset the counter to 0.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
int findMaxConsecutiveOnes(vector<int>& nums) {
        int ans=0, count=0;
        for(auto it : nums){
            if(it){
                count++;
                ans=max(ans,count);
            }else{
                count=0;
            }
        }
        return ans;
    }
```

---

## 32. 493. Reverse Pairs

**Q:** Find the number of reverse pairs where `i < j` and `nums[i] > 2 * nums[j]`.

**A:** Merge Sort Modification.

**Explanation:**
- Use the divide-and-conquer merge sort algorithm.
- Before merging two sorted halves, count the reverse pairs using a two-pointer approach.
- For each element in the left half, advance a pointer `y` in the right half as long as the condition `a[x] > 2 * a[y]` holds.
- Add the count of valid elements in the right half to the total inversions, then merge the two halves as usual.

**Complexity:** Time `O(N log N)`, Space `O(N)`.

```cpp
int merge(vector<int> &a,int p,int q,int r){
        int  n1=q-p+1;
        int n2=r-q;
        int  a1[n1];
        int  a2[n2];
        int inv=0;
        
        int y=q+1;
        for(int x=p;x<=q;++x) {
            while(y<=r && a[x] > 2LL*a[y]) ++y;
            inv += (y - (q+1));
        }
        
        for(int i=0;i<n1;i++){
            a1[i]=a[p+i];
        }
    
        for(int j=0;j<n2;j++){
            a2[j]=a[q+1+j];
        }
    
        int i=0,j=0,k=p;
        while(i<n1 && j<n2){
            if(a1[i]<=a2[j]){
                a[k]=a1[i];
                k++; i++;
            }else{
                a[k]=a2[j];
                k++; j++;
            }
        }
    
        while(i<n1){
            a[k]=a1[i];
            k++; i++;
        }
    
        while(j<n2){
            a[k]=a2[j];
            k++; j++;
        }
        return inv;
    }
    
    int mergeSort(vector<int> &a,int f,int l){
        int inv=0;
        if(f<l){
            int m=(f+l)/2;
            inv+=mergeSort(a,f,m);
            inv+=mergeSort(a,m+1,l);
            inv+=merge(a,f,m,l);
        }
        return inv;
    }
    
    int reversePairs(vector<int>& nums) {
        int n=nums.size();
        return mergeSort(nums,0,n-1);
    }
```

---

## 33. 525. Contiguous Array

**Q:** Find the maximum length of a contiguous subarray with an equal number of 0s and 1s.

**A:** Prefix Sum with Hash Map.

**Explanation:**
- Treat 0s as -1s and 1s as 1s. This transforms the problem into finding the longest subarray with a sum of 0.
- Maintain a running `sum` and use a hash map to store the first occurrence index of each sum.
- If the same sum is encountered again, it means the subarray between the previous index and the current index has a sum of 0 (equal 0s and 1s).
- Update the maximum length `ans` if a longer valid subarray is found.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
int findMaxLength(vector<int>& nums) {
    int n=nums.size(),i=0,sum=0,ans=0;
    unordered_map<int,int> mp;
    mp[sum]=-1;
    while(i<n){
        if(nums[i]==1) sum+=1;
        else sum-=1;
        if(mp.find(sum)!=mp.end()){
            ans=max(ans,i-mp[sum]);
        }else mp[sum]=i;
        i++;
    }
    return ans;
}
```

---

## 34. 605. Can Place Flowers

**Q:** Check if `n` new flowers can be planted in a flowerbed without violating the no-adjacent-flowers rule.

**A:** Greedy Approach.

**Explanation:**
- Iterate through the flowerbed array.
- For every empty plot (`0`), check its immediate left and right neighbors (treating out-of-bounds as empty `0`).
- If both neighbors are empty, plant a flower (set to `1`) and decrement `n`.
- If `n` reaches 0 or less, return true immediately.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
   bool canPlaceFlowers(vector<int>& flowerbed, int n) {
        if(n==0) return 1;
        int len=flowerbed.size();
        for(int i=0;i<len;i++){
            if(flowerbed[i]==0){
                int next = (i==len-1) ? 0 : flowerbed[i+1];
                int prev = (i==0) ? 0 : flowerbed[i-1];
                if(next==0 && prev==0){
                    flowerbed[i]=1;
                    n--;
                }
            }
            if(n<=0) return 1;
        }
        return 0;
    }
```

---

## 35. 665. Non-decreasing Array

**Q:** Check if an array can become non-decreasing by modifying at most one element.

**A:** Greedy (In-place Modification).

**Explanation:**
- Iterate through the array and find instances where the non-decreasing rule is broken (`nums[i-1] > nums[i]`).
- If this happens more than once (`count > 1`), return false.
- To fix the violation greedily: if `nums[i-2] <= nums[i]`, lower `nums[i-1]` to `nums[i]`; otherwise, raise `nums[i]` to `nums[i-1]`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
bool checkPossibility(vector<int>& nums) {
        int count=0,n=nums.size();
        for(int i=1;i<n;i++){
            if(nums[i-1]>nums[i]){
                if(count==1) return false;
                count++;
                if(i<2 || nums[i-2]<=nums[i]) nums[i-1]=nums[i];
                else nums[i]=nums[i-1];
            }
            
        }
        return true;
    }
```

---

## 36. 1287. Element Appearing More Than 25% In Sorted Array

**Q:** Given an integer array sorted in non-decreasing order, find the integer that occurs more than 25% of the time.

**A:** Single Pass Counting.

**Explanation:**
- Calculate the 25% threshold `t = n / 4`.
- Iterate through the array counting consecutive occurrences of each element.
- As soon as the count exceeds the threshold `t`, return that element.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
int findSpecialInteger(vector<int>& arr) {
    int n=arr.size();
    int t=n/4;
    int p=arr[0],cnt=1;
    for(int i=1;i<n;i++){
        if(arr[i]==p){
            cnt++;
            if(cnt>t) return p;
        }
        else{
            cnt=1;
            p=arr[i];
        }
    }
    return p;
}
```

---

## 37. 1464. Maximum Product of Two Elements in an Array

**Q:** Find the maximum possible value of `(nums[i]-1)*(nums[j]-1)` in an array.

**A:** Single Pass (Find Top Two).

**Explanation:**
- Iterate through the array while maintaining the two largest elements, `h1` (largest) and `h2` (second largest).
- If a number is greater than `h1`, demote `h1` to `h2` and update `h1`.
- Otherwise, if it's greater than `h2`, just update `h2`.
- Return `(h1-1)*(h2-1)`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
int maxProduct(vector<int>& nums) {
    int n=nums.size();
    int h1=nums[0],h2=0;
    for(int i=1;i<n;i++){
        if(nums[i]>h1){
            h2=h1;
            h1=nums[i];
        }else{
            h2=max(h2,nums[i]);
        }
    }
    return (h1-1)*(h2-1);
}
```

---

## 38. 1749. Maximum Absolute Sum of Any Subarray

**Q:** Find the maximum absolute sum of any contiguous subarray.

**A:** Kadane's Algorithm (Twice).

**Explanation:**
- The maximum absolute sum will either be the maximum positive subarray sum or the absolute value of the minimum negative subarray sum.
- First, run Kadane's algorithm to find the maximum positive sum.
- Then, invert the signs of all elements in the array and run Kadane's again (which effectively finds the magnitude of the minimum negative sum).
- Return the maximum of the two results.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int kadane(vector<int>a){
        int sum=0;
        int maxSum=0;
        for(auto it : a){
            sum+=it;
            maxSum=max(maxSum,sum);
            if(sum<0) sum=0;
        }
        return maxSum;
    }

    int maxAbsoluteSum(vector<int>& nums) {
        int ans=kadane(nums);
        for(auto &it : nums){
            it*=(-1);
        }
        ans=max(ans,kadane(nums));
        return ans;
    }
```

---

## 39. 1913. Maximum Product Difference Between Two Pairs

**Q:** Find the maximum product difference between two pairs of elements.

**A:** Single Pass (Find Top Two and Bottom Two).

**Explanation:**
- The maximum product difference is obtained by multiplying the two largest elements and subtracting the product of the two smallest elements.
- Iterate through the array to find the two largest elements (`h1`, `h2`) and the two smallest elements (`l1`, `l2`).
- Return `(h1*h2) - (l1*l2)`.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
int maxProductDifference(vector<int>& nums) {
    int n=nums.size();
    int h1=nums[0],h2=INT_MIN;
    for(int i=1;i<n;i++){
        if(nums[i]>h1){
            h2=h1;
            h1=nums[i];
        }else{
            h2=max(h2,nums[i]);
        }
    }

    int l1=nums[0],l2=INT_MAX;
    for(int i=1;i<n;i++){
        if(nums[i]<l1){
            l2=l1;
            l1=nums[i];
        }else{
            l2=min(l2,nums[i]);
        }
    }

    return ((h1*h2)-(l1*l2));
}
```

---

## 40. 2017. Grid Game

**Q:** Minimize the maximum score the second robot can get on a 2xN grid after the first robot optimizes its path to minimize the second's score.

**A:** Prefix/Suffix Sums on a Grid.

**Explanation:**
- The first robot will drop down at some column `i`. The second robot can then only collect the remainder of the top row (after `i`) OR the beginning of the bottom row (before `i`).
- Precompute the sum of the top row (`tsum`). Maintain a running sum for the bottom row (`bsum`).
- Iterate through each possible drop-down column `i`, updating `tsum` and calculating the second robot's optimal choice `max(tsum, bsum)`. Minimize this across all columns.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    long long gridGame(vector<vector<int>>& grid) {
        long long ans=LONG_MAX;
        long long tsum=accumulate(grid[0].begin(),grid[0].end(),0LL);
        long long bsum=0;
        int n=grid[0].size();
        for(int i=0;i<n;i++){
            tsum-=grid[0][i];
            ans=min(ans,max(tsum,bsum));
            bsum+=grid[1][i];
        }
        return ans;
    }
```

---

## 41. 2149. Rearrange Array Elements by Sign

**Q:** Rearrange an array of equal positive and negative integers such that every consecutive pair has opposite signs.

**A:** Two Pointers (Even/Odd Indexing).

**Explanation:**
- Initialize two pointers, `i = 0` for positive numbers (even indices) and `j = 1` for negative numbers (odd indices).
- Iterate through the array and place elements in their respective positions in a new `ans` array.
- Increment the corresponding pointer by 2 to prepare for the next number of the same sign.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
    vector<int> rearrangeArray(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans(n);
        int i=0,j=1;
        for(auto it : nums){
            if(it<0){
                ans[j]=it;
                j=j+2;
            }else{
                ans[i]=it;
                i=i+2;
            }
        }
        return ans;
    }
```

---

## 42. 2270. Number of Ways to Split Array

**Q:** Count the number of valid splits in an array where the sum of the left part is greater than or equal to the sum of the right part.

**A:** Prefix Sum.

**Explanation:**
- First, calculate the total sum of the array.
- Iterate through the array while maintaining a running prefix sum (`curs`).
- At each index before the last, check if the prefix sum is greater than or equal to the remaining sum (`sum - curs`). If so, increment the valid split count.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int waysToSplitArray(vector<int>& nums) {
        long long sum=0;
        int n=nums.size();
        for(int i=0;i<n;i++) sum+=nums[i];
        long long curs=0;
        int ans=0;
        for(int i=0;i<n-1;i++){
            curs+=nums[i];
            if(curs>=(sum-curs)) ans++;
        }
        return ans;
    }
```

---

## 43. 2381. Shifting Letters II

**Q:** Apply multiple range shift operations to a string and return the final string.

**A:** Difference Array / Prefix Sum.

**Explanation:**
- Use a difference array (`prefixSum`) to record the start and end of each shift in `O(1)` time (+1 or -1).
- Compute the prefix sum of the difference array to get the net shift applied to each character index.
- Apply the net shift to each character, using modulo arithmetic to correctly handle negative shifts and wrapping around the alphabet.

**Complexity:** Time `O(N + Q)`, Space `O(N)`.

```cpp
    string shiftingLetters(string s, vector<vector<int>>& shifts) {
        int n = s.length();
        vector<long> prefixSum(n + 1, 0);
        for (const auto& shift : shifts) {
            int start = shift[0];
            int end = shift[1];
            int direction = shift[2];
            int value = (direction == 1) ? 1 : -1;
            prefixSum[start] += value;
            prefixSum[end + 1] -= value;
        }
        for (int i = 1; i < n; i++) {
            prefixSum[i] += prefixSum[i-1];
        }
        for (int i = 0; i < n; i++) {
            long totalShifts = prefixSum[i];
            totalShifts = ((totalShifts % 26) + 26) % 26;   
            int newChar = (s[i] - 'a' + totalShifts) % 26;
            s[i] = 'a' + newChar;
        }
        return s;
    }
```

---

## 44. 2460. Apply Operations to an Array

**Q:** Double adjacent equal elements and move all zeros to the end of the array.

**A:** Single Pass Simulation and Two Pointers.

**Explanation:**
- Iterate through the array; if adjacent elements are equal, double the first and zero the second.
- Simultaneously use a pointer `nonZeroIdx` to track where the next non-zero element should go.
- When a non-zero element is encountered, swap it to the `nonZeroIdx` position.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    vector<int> applyOperations(vector<int>& nums) {
        int n=nums.size();
        int nonZeroIdx=0;
        for(int i=0;i<n;i++){
            if(i<n-1 && nums[i]==nums[i+1]){
                nums[i]*=2;
                nums[i+1]=0;
            }
            if(nums[i]!=0){
                swap(nums[i],nums[nonZeroIdx]);
                nonZeroIdx++;
            }
        }
        return nums;
    }
```

---

## 45. 2563. Count the Number of Fair Pairs

**Q:** Count the number of pairs `(i, j)` where the sum `nums[i] + nums[j]` falls within a specified range.

**A:** Sorting + Two Pointers (Inclusion-Exclusion).

**Explanation:**
- A pair sum falling in `[lower, upper]` is mathematically equivalent to the number of pairs with sum `> lower-1` minus the number of pairs with sum `> upper`.
- Sort the array first.
- Use a two-pointer helper function to count pairs strictly greater than a given value `cmp`. If `nums[i] + nums[j] > cmp`, all elements between `i` and `j` also form valid pairs with `j`.

**Complexity:** Time `O(N log N)`, Space `O(1)` (or `O(N)` for sorting).

```cpp
    long long countPair(vector<int> &nums,int cmp){
        int n=nums.size();
        int i=0,j=n-1;
        long long ans=0;
        while(i<j){
            int sum=nums[i]+nums[j];
            if(sum>cmp){
                ans+=(j-i);
                j--;
            }else i++;
        }
        return ans;
    }

    long long countFairPairs(vector<int>& nums, int lower, int upper) {
        sort(nums.begin(),nums.end());
        return countPair(nums,lower-1)-countPair(nums,upper);
    }
```

---

## 46. 2815. Max Pair Sum in an Array

**Q:** Find the maximum sum of a pair of numbers that share the same maximum digit.

**A:** Brute Force / Pairwise Comparison.

**Explanation:**
- Use a helper function `maxD` to compute the largest digit of any given number by repeatedly taking modulo 10.
- Iterate through all possible pairs using nested loops.
- If the maximum digits of a pair match, update the global maximum sum `ans`.

**Complexity:** Time `O(N^2)`, Space `O(1)`.

```cpp
    int maxD(int n){
        int ans=-1;
        while(n){
            ans=max(ans,n%10);
            n/=10;
        }
        return ans;
    }

    int maxSum(vector<int>& nums) {
        int n=nums.size(),ans=-1;
        for(int i=0;i<n-1;i++){
            for(int j=i+1;j<n;j++){
                int sum=nums[i]+nums[j];
                if(maxD(nums[i])==maxD(nums[j])){
                    ans=max(ans,sum);
                }
            }
        }
        return ans;
    }
```

---

## 47. 2948. Make Lexicographically Smallest Array by Swapping Elements

**Q:** Make the array lexicographically smallest by swapping elements whose absolute difference is at most a given limit.

**A:** Sorting and Connected Components.

**Explanation:**
- Store elements with their original indices and sort them by value.
- Group contiguous elements into connected components where adjacent sorted elements have a difference `<= limit`.
- For each connected component, collect and sort their original indices.
- Place the sorted values from the component back into the array at the sorted original indices.

**Complexity:** Time `O(N log N)`, Space `O(N)`.

```cpp
vector<int> lexicographicallySmallestArray(vector<int>& nums, int limit) {
        int n=nums.size();
        vector<pair<int,int>> copy(n);
        for(int i=0;i<n;i++){
            copy[i]={nums[i],i};
        }
        sort(copy.begin(),copy.end());

        int i=0,j=1;
        while(j<n){
            vector<int> pos={copy[i].second};
            while(j<n && (copy[j].first - copy[j-1].first)<=limit){
                pos.push_back(copy[j].second);
                j++;
            }
            sort(pos.begin(),pos.end());
            for(int k=0;k<j-i;k++){
                nums[pos[k]]=copy[i+k].first;
            }
            i=j;
            j++;
        }
        return nums;
    }
```

---

## 48. Code   Testcase Testcase Test Result 2161. Partition Array According to Given Pivot

**Q:** Rearrange the array such that elements smaller than the pivot appear before it, and greater elements appear after, preserving relative order.

**A:** Two Pointers with Auxiliary Array.

**Explanation:**
- Use an auxiliary array `ans` of the same size.
- Use two pointers `l` reading from the left (for elements `< pivot`) and `r` reading from the right (for elements `> pivot`).
- Simultaneously write to `ans` from the left (`al`) and from the right (`ar`).
- Fill the remaining middle gap in the `ans` array with the pivot value itself.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
    vector<int> pivotArray(vector<int>& nums, int pivot) {
        int n=nums.size();
        vector<int> ans(n);
        int l=0,r=n-1;
        int al=0,ar=n-1;
        while(l<n){
            if(nums[l]<pivot) ans[al++]=nums[l];
            if(nums[r]>pivot) ans[ar--]=nums[r];
            l++;r--;
        }
        for(int i=al;i<=ar;i++) ans[i]=pivot;
        return ans;
    }
```

---

## 49. Container With Most Water

**Q:** Find two lines that together with the x-axis form a container that holds the most water.

**A:** Two Pointers.

**Explanation:**
- Start with two pointers, `left` at the beginning and `right` at the end of the array.
- Calculate the area constrained by the shorter line and update the maximum area.
- Move the pointer pointing to the shorter line inward, as this is the only way to potentially find a taller boundary and increase the area.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int maxWater(vector<int> &arr) {
        int n=arr.size();
        int left=0,right=n-1,ans=0;
        while(left<right){
            ans=max(ans,min(arr[left],arr[right])*(right-left));
            if(arr[left]<arr[right]) left++;
            else right--;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/container-with-most-water0535/1
```

---

## 50. Count Inversions

**Q:** Count the number of inversions in an array where `i < j` and `arr[i] > arr[j]`.

**A:** Merge Sort Modification.

**Explanation:**
- Use the divide-and-conquer merge sort algorithm.
- During the merge step of two sorted halves, if an element in the left half `a1[i]` is strictly greater than an element in the right half `a2[j]`, then it is also greater than all subsequent elements in the left half.
- Add `n1 - i` to the inversion count and proceed with standard merging.

**Complexity:** Time `O(N log N)`, Space `O(N)`.

```cpp
long long int merge(long long a[],long long p,long long q,long long r){
        long long  n1=q-p+1;
        long long n2=r-q;
        long long  a1[n1];
        long long  a2[n2];
        long long int inv=0;
    
        for(long long i=0;i<n1;i++){
            a1[i]=a[p+i];
        }
    
        for(long long j=0;j<n2;j++){
            a2[j]=a[q+1+j];
        }
    
        long long i=0,j=0,k=p;
        while(i<n1 && j<n2){
            if(a1[i]<=a2[j]){
                a[k]=a1[i];
                k++; i++;
            }else{
                a[k]=a2[j];
                inv+=n1-i;   // a1[i] > a2[j] and i < j 
                k++; j++;
            }
        }
    
        while(i<n1){
            a[k]=a1[i];
            k++; i++;
        }
    
        while(j<n2){
            a[k]=a2[j];
            k++; j++;
        }
        return inv;
    }
    
    long long int mergeSort(long long a[],long long f,long long l){
        long long int inv=0;
        if(f<l){
            long long m=(f+l)/2;
            inv+=mergeSort(a,f,m);
            inv+=mergeSort(a,m+1,l);
            inv+=merge(a,f,m,l);
        }
        return inv;
    }
    
    long long int inversionCount(long long arr[], long long N)
    {
        // Your Code Here
        return mergeSort(arr,0,N-1);
       
    }
```

---

## 51. Count Pairs whose sum is less than target

**Q:** Count the number of pairs in an array whose sum is strictly less than a given target.

**A:** Sorting + Two Pointers.

**Explanation:**
- Sort the array to allow the two-pointer technique.
- Initialize pointers `i` at the start and `j` at the end.
- If the sum of elements at `i` and `j` is less than the target, then all pairs `(i, i+1)` through `(i, j)` are valid, so add `j - i` to the count and increment `i`.
- Otherwise, decrement `j`.

**Complexity:** Time `O(N log N)`, Space `O(1)`.

```cpp
    int countPairs(vector<int> &arr, int target) {
        sort(arr.begin(),arr.end());
        int n=arr.size(),i=0,j=n-1,ans=0;
        while(i<j){
            int sum=arr[i]+arr[j];
            if(sum<target){
                ans+=(j-i);
                i++;
            }else j--;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/count-pairs-whose-sum-is-less-than-target/1
```

---

## 52. Count all triplets with given sum in sorted array

**Q:** Count all unique triplets in a sorted array that sum up to a given target.

**A:** Two Pointers with Duplicate Handling.

**Explanation:**
- Iterate through the array with pointer `i`. Use two pointers `j` and `k` for the remaining elements.
- When a valid sum is found, we need to count duplicates correctly. Let `cj` and `ck` be the frequency of the values at pointers `j` and `k`.
- If the elements at `j` and `k` are identical, the number of combinations is `cj * (cj - 1) / 2`.
- Otherwise, it is `cj * ck`.

**Complexity:** Time `O(N^2)`, Space `O(1)`.

```cpp
    int countTriplets(vector<int> &arr, int target) {
        int n=arr.size(),ans=0;
        for(int i=0;i<n-2;i++){
            int j=i+1,k=n-1;
            while(j<k){
                int sum=arr[i]+arr[j]+arr[k];
                if(sum<target) j++;
                else if(sum>target) k--;
                else{
                    int ej=arr[j],ek=arr[k],cj=0,ck=0;
                    while(j<=k && arr[j]==ej){
                        cj++;
                        j++;
                    }
                    while(j<=k && arr[k]==ek){
                        ck++;
                        k--;
                    }
                    if(ej==ek) ans+=(cj*(cj-1))/2;
                    else ans+=(cj*ck);
                }
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/count-all-triplets-with-given-sum-in-sorted-array/1
```

---

## 53. Count the number of possible triangles

**Q:** Count the number of possible triangles that can be formed by elements of an array.

**A:** Sorting + Two Pointers.

**Explanation:**
- Sort the array. For three sides to form a triangle, the sum of any two must be greater than the third.
- Since the array is sorted, we only need to check if `arr[low] + arr[high] > arr[i]` where `arr[i]` is the largest side.
- Fix the largest side `i` starting from the end. Use two pointers `low = 0` and `high = i - 1`.
- If the condition is met, all sides between `low` and `high` can form a triangle with `high` and `i`, so add `high - low` and decrement `high`.

**Complexity:** Time `O(N^2)`, Space `O(1)`.

```cpp
    int countTriangles(vector<int>& arr) {
        int n=arr.size(),ans=0;
        sort(arr.begin(),arr.end());
        for(int i=n-1;i>=2;i--){
            int high=i-1,low=0;
            while(high>low){
                int sum=arr[high]+arr[low];
                if(sum<=arr[i]) low++;
                else{
                    ans+=(high-low);
                    high--;
                }
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/count-possible-triangles-1587115620/1
```

---

## 54. K-th missing element

**Q:** Find the k-th missing element in an array between its minimum and maximum elements.

**A:** Hash Set Traversal.

**Explanation:**
- Insert all array elements into an unordered set for `O(1)` lookups.
- Iterate from the first element `a[0]` up to the maximum element `a[n-1]`.
- Increment a counter each time a number is not found in the set.
- Return the number when the counter reaches `k`.

**Complexity:** Time `O(max - a[0])` or `O(N)`, Space `O(N)`.

```cpp
int KthMissingElement(int a[], int n, int k){
   int max= a[n-1];
   unordered_set<int>s;
   for(int i=0; i<n;i++) s.insert(a[i]);
   int count=0;
   for(int i=a[0]; i<=max;i++){
       if(s.find(i)==s.end() and count!=k) count++;
       if(s.find(i)==s.end() and count==k) return i;
   }
   return -1;
}

Link - https://practice.geeksforgeeks.org/problems/k-th-missing-element3635/1
```

---

## 55. Largest subarray with 0 sum

**Q:** Find the length of the longest subarray with a sum of 0.

**A:** Prefix Sum with Hash Map.

**Explanation:**
- Compute the prefix sum for the array.
- Store the first occurrence of each prefix sum in a hash map.
- If the prefix sum at index `i` is 0, the subarray from index 0 to `i` has sum 0 (length `i + 1`).
- If the prefix sum has been seen before at index `j`, the subarray between `j` and `i` has sum 0 (length `i - j`). Update the maximum length.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
int maxLen(vector<int>&A, int n){
      vector<int> psum(n);
      psum[0]=A[0];
      for(int i=1;i<n;i++) psum[i]=psum[i-1]+A[i];
      unordered_map<int,int> mp;
      int ans=0;
      for(int i=0;i<n;i++){
          if(psum[i]==0){
              ans=max(ans,i+1);
          }
          else if(mp.find(psum[i])!=mp.end()){
              ans=max(ans,i-mp[psum[i]]);
          }else{
              mp[psum[i]]=i;
          }
      }
      return ans;
  }
```

---

## 56. Maximum Index

**Q:** Find the maximum difference `j - i` such that `a[i] <= a[j]`.

**A:** Suffix Maximum Array + Two Pointers.

**Explanation:**
- Create a suffix maximum array `mx` where `mx[i]` stores the maximum element from `a[i]` to `a[n-1]`.
- Use two pointers `i` and `j` starting at 0.
- If `a[i] <= mx[j]`, update the answer with `j - i` and increment `j` to look for a larger difference.
- Otherwise, increment `i`.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
int maxIndexDiff(int a[], int n) { 
    int mx[n];
    mx[n-1]=a[n-1];
    for(int i=n-2;i>=0;i--){
        mx[i]=max(mx[i+1],a[i]);
    }
    int i=0,j=0,ans=0;
    while(i<n && j<n){
        if(a[i]<=mx[j]){
            ans=max(ans,j-i);
            j++;
        }else i++;
    }
    return ans;
}

gfg link - https://www.geeksforgeeks.org/problems/maximum-index-1587115620/1
```

---

## 57. Maximum Product Subarray

**Q:** Find the contiguous subarray that has the largest product.

**A:** Prefix and Suffix Product Traversal.

**Explanation:**
- A negative number can flip a large negative product to a large positive one, so we must track both ends.
- Traverse the array from left to right, maintaining a running product and updating the maximum. Reset the product to 1 if it hits 0.
- Traverse the array from right to left, doing the same.
- The overall maximum from both traversals gives the answer.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int maxProduct(vector<int> &nums) {
        int n=nums.size();
        long long int pro=1,maxp=INT_MIN;
        for(int i=0;i<n;i++){
            pro*=nums[i];
            maxp=max(maxp,pro);
            if(pro==0) pro=1;
        }
        
        pro=1;
        for(int i=n-1;i>=0;i--){
            pro*=nums[i];
            maxp=max(maxp,pro);
            if(pro==0) pro=1;
        }
        return maxp;
    }

gfg link - https://www.geeksforgeeks.org/problems/maximum-product-subarray3604/1
```

---

## 58. Maximum sum Rectangle

**Q:** Find the maximum sum of a rectangular submatrix in a 2D matrix.

**A:** Kadane's Algorithm on Rows (or Columns).

**Explanation:**
- Iterate through all possible top rows `i`.
- Maintain an array `sub` that accumulates the column sums from row `i` down to row `j`.
- For each bottom row `j`, apply Kadane's 1D Maximum Subarray algorithm on the `sub` array to find the maximum sum rectangle bounded by rows `i` and `j`.

**Complexity:** Time `O(N^2 * M)`, Space `O(M)`.

```cpp
    int maximumSumRectangle(int n, int m, vector<vector<int>> mat) {
        int best=INT_MIN;
        for(int i=0;i<n;i++){
           int sub[m]={0};
           for(int j=i;j<n;j++){
               int sum=0;
               for(int k=0;k<m;k++) sub[k]+=mat[j][k];
               for(int k=0;k<m;k++){
                   sum+=sub[k];
                   best=max(best,sum);
                   if(sum<0)
                   sum=0;
               }
           }
       }
       return best;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/maximum-sum-rectangle2948/1
```

---

## 59. Modify the Array

**Q:** Double the first element and zero out the second if they are equal and non-zero, then move all zeros to the end.

**A:** Linear Scan and Two Pointers.

**Explanation:**
- First, iterate through the array. If `arr[i]` equals `arr[i+1]` (and is not 0), double `arr[i]` and set `arr[i+1]` to 0.
- Next, use a two-pointer approach (like in 'Move Zeroes') to push all non-zero elements to the front.
- Swap non-zero elements with the position marked by `lastNonZero` pointer.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    vector<int> modifyAndRearrangeArray(vector<int> &arr) {
        int n=arr.size();
        for(int i=0;i<n-1;i++){
            if(arr[i]!=0 && arr[i]==arr[i+1]){
                arr[i]*=2;
                arr[i+1]=0;
            }
        }
        int lastNonZero=0;
        for(int i=0;i<n;i++){
            if(arr[i]!=0){
                swap(arr[i],arr[lastNonZero]);
                lastNonZero++;
            }
        }
        return arr;
    }

gfg link - https://www.geeksforgeeks.org/problems/ease-the-array0633/1
```

---

## 60. Pairs with specific difference

**Q:** Find the maximum sum of disjoint pairs whose absolute difference is less than `K`.

**A:** Sorting and Greedy Pair Selection.

**Explanation:**
- Sort the array in descending order to pair larger numbers first, maximizing the sum.
- Iterate through the array. If the difference between adjacent elements `arr[i]` and `arr[i+1]` is less than `K`, pair them up.
- Add their sum to the total and jump by 2 indices. If not, just move 1 index.

**Complexity:** Time `O(N log N)`, Space `O(1)`.

```cpp
    int maxSumPairWithDifferenceLessThanK(int arr[], int N, int K){
         int ans=0;
         sort(arr,arr+N,greater<int>());
         for(int i=0,j=1;i<N-1,j<N;){
            if((arr[i]-arr[j])<K){
                ans+=arr[i]+arr[j];
                i=j+1;
            }else i++;
            j=i+1;
         }
         return ans;
    }
```

---

## 61. Pair with given sum in a sorted array

**Q:** Count the number of pairs in a sorted array that sum to a given target.

**A:** Two Pointers with Duplicate Handling.

**Explanation:**
- Use two pointers `i` at the start and `j` at the end.
- If `arr[i] + arr[j] == target`, we must count duplicates. If `arr[i] == arr[j]`, all elements between `i` and `j` are identical, contributing `C * (C - 1) / 2` pairs.
- Otherwise, count the number of identical elements at both pointers (`ic` and `jc`) and add their product `ic * jc` to the answer.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int countPairs(vector<int> &arr, int target) {
        int n=arr.size();
        int i=0,j=n-1;
        int ans=0;
        while(i<j){
            int sum=arr[i]+arr[j];
            if(sum<target) i++;
            else if(sum>target) j--;
            else{
                int ie=arr[i],je=arr[j],ic=0,jc=0;
                while(i<=j && ie==arr[i]) {
                    ic++;
                    i++;
                }
                while(i<=j && je==arr[j]) {
                    jc++;
                    j--;
                }
                if(ie!=je) ans+=(ic*jc);
                else ans+=(ic*(ic-1))/2;
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/pair-with-given-sum-in-a-sorted-array4940/1
```

---

## 62. Product array puzzle

**Q:** Return an array where each element is the product of all other elements, without using division.

**A:** Prefix and Suffix Products (Space Optimized).

**Explanation:**
- First, calculate the suffix products and store them directly in the result array `res`.
- Maintain a running prefix product `prod` starting from the left.
- Iterate through the array, multiplying the prefix product by the precomputed suffix product to get the final answer for each position.

**Complexity:** Time `O(N)`, Space `O(1)` (excluding output).

```cpp
    vector<int> productExceptSelf(vector<int>& arr) {
        int n=arr.size();
        vector<int> res(n);
        res[n-1]=arr[n-1];
        for(int i=n-2;i>0;i--){
            res[i]=res[i+1]*arr[i];
        }
        int prod=arr[0];
        res[0]=res[1];
        for(int i=1;i<=n-2;i++){
            res[i]=prod*res[i+1];
            prod*=arr[i];
        }
        res[n-1]=prod;
        return res;
    }

gfgf - https://www.geeksforgeeks.org/problems/product-array-puzzle4525/1
```

---

## 63. Repeat and Missing Number Array

**Q:** Find the repeating and the missing number in a read-only array of `N` integers from `1` to `N`.

**A:** Cyclic Sort (Index as Hash).

**Explanation:**
- Since the numbers are in the range `[1, N]`, place each number at its correct index (`B[i]` at index `B[i] - 1`) by swapping.
- Iterate through the array until every element is at its correct position or a duplicate is found.
- Finally, scan the array; the index that doesn't have the correct element indicates the missing number (`i + 1`), and the element sitting there is the repeating number (`B[i]`).

**Complexity:** Time `O(N)`, Space `O(N)` (because it copies to `B`).

```cpp
vector<int> Solution::repeatedNumber(const vector<int> &A) {
    vector<int> B=A;
    int i=0,n=B.size();
    while(i<n){
        int ind=B[i]-1;
        if(B[i]!=B[ind]) swap(B[i],B[ind]);
        else i++;
    }
    for(int i=0;i<n;i++){
        if(B[i]!=i+1){
            return {B[i],i+1};
        }
    }
    return {B[i],i+1};
}
```

---

## 64. Sorted subsequence of size 3

**Q:** Find three elements `arr[i] < arr[j] < arr[k]` such that `i < j < k`.

**A:** Prefix Min and Suffix Max Arrays.

**Explanation:**
- Precompute a `lmin` array storing the minimum element seen so far from the left.
- Precompute a `rmax` array storing the maximum element seen so far from the right.
- Iterate through the array; if the current element is greater than `lmin[i]` and strictly less than `rmax[i]`, we have found a valid triplet.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
    vector<int> find3Numbers(vector<int> &arr) {
        int n=arr.size();
        vector<int> lmin(n,1e9),rmax(n,0);
        
        for(int i=1;i<n;i++){
            lmin[i]=min(arr[i-1],lmin[i-1]);
        }
        for(int i=n-2;i>=0;i--){
            rmax[i]=max(arr[i+1],rmax[i+1]);
        }
        
        for(int i=1;i<n-1;i++){
            if(arr[i]>lmin[i] && arr[i]<rmax[i]) return {lmin[i],arr[i],rmax[i]};
        }
        return {};
    }

gfg link - https://www.geeksforgeeks.org/problems/sorted-subsequence-of-size-3/1
```

---

## 65. Split an array into two equal Sum subarrays

**Q:** Determine if an array can be split into two contiguous subarrays with equal sums.

**A:** Prefix Sum.

**Explanation:**
- Calculate the total sum of the array. If the sum is odd, it's impossible to split equally.
- Iterate through the array accumulating a running sum `cur`.
- If `cur` matches exactly half of the total sum, the array can be split.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    bool canSplit(vector<int>& arr) {
        int sum=accumulate(arr.begin(),arr.end(),0);
        if(sum%2==1) return false;
        sum/=2;
        int cur=0,i=0;
        while(i<arr.size() && cur<sum) cur+=arr[i++];
        return cur==sum;
    }

gfg link - https://www.geeksforgeeks.org/problems/split-an-array-into-two-equal-sum-subarrays/1
```

---

## 66. Subarray range with given sum

**Q:** Count the number of subarrays with a given sum.

**A:** Prefix Sum with Hash Map.

**Explanation:**
- Maintain a running `curr_sum` and a hash map of prefix sums encountered.
- For each element, calculate `prefix_sum = curr_sum - tar`.
- If this `prefix_sum` exists in the map, it means there is a subarray ending at the current element that sums to the target.
- Add the frequency of this prefix sum to the total answer.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
    int subArraySum(vector<int>& arr, int tar) {
        map<int,int>mp;
        int prefix_sum=0,curr_sum=0,ans=0;
        mp[0]=1;
        for(int i=0;i<arr.size();i++){
            curr_sum+=arr[i];
            prefix_sum=curr_sum-tar;
            if(mp[prefix_sum]>0) ans+=mp[prefix_sum];
            mp[curr_sum]++;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/subarray-range-with-given-sum0128/1
```

---

## 67. Subarray with given XOR

**Q:** Count the number of subarrays with a given XOR sum.

**A:** Prefix XOR with Hash Map.

**Explanation:**
- Maintain a running `xorsum` and a hash map tracking the frequencies of prefix XOR sums.
- For each element, check if the current `xorsum` equals the target `B`. If so, increment the answer.
- Use the property `A ^ B = C => A ^ C = B`. Look up `xorsum ^ B` in the hash map to find subarrays matching the required XOR sum.

**Complexity:** Time `O(N)`, Space `O(N)`.

```cpp
int Solution::solve(vector<int> &A, int B) {
    unordered_map<int,int> mp;
    int xorsum=0;
    int ans=0;
    for(auto it : A){
        xorsum^=it;
        if(xorsum==B) ans++;
        if(mp.find(xorsum^B)!=mp.end()){
            ans+=mp[xorsum^B];
        }
        mp[xorsum]++;
    }
    return ans;
}
```

---

## 68. Sum of Middle elements of two sorted arrays

**Q:** Find the sum of the two middle elements when merging two sorted arrays into one.

**A:** Two Pointers (Linear Merge Simulation).

**Explanation:**
- Simulate the merging process of the two sorted arrays without actually building the merged array.
- Use pointers `i`, `j`, and a counter `k` to track the current position in the merged array.
- When `k` reaches `len - 1` and `len` (the two middle indices), add the corresponding values to the answer.

**Complexity:** Time `O(N + M)`, Space `O(1)`.

```cpp
    int SumofMiddleElements(vector<int> &arr1, vector<int> &arr2) {
        int l1=arr1.size(),l2=arr2.size();
        int len=l1+l2;
        len/=2;
        int i=0,j=0,k=0;
        int ans=0;
        while(i<l1 && j<l2){
            if(arr1[i]<=arr2[j]){
                if(k==len || k==len-1) ans+=arr1[i];
                i++;
            }
            else {
                if(k==len || k==len-1) ans+=arr2[j];
                j++;
            }
            k++;
        }
        while(i<l1){
            if(k==len || k==len-1) ans+=arr1[i];
            i++;
            k++;
        }
        while(j<l2){
            if(k==len || k==len-1) ans+=arr2[j];
            j++;
            k++;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/sum-of-middle-elements-of-two-sorted-arrays2305/1
```

---

## 69. Sum of Subarrays

**Q:** Find the sum of all possible contiguous subarrays of an array.

**A:** Combinatorics (Contribution Technique).

**Explanation:**
- Instead of generating all subarrays, calculate the contribution of each element to the total sum.
- An element at index `i` appears in exactly `(i + 1) * (n - i)` subarrays.
- Multiply the element's value by this count and add it to the running total.

**Complexity:** Time `O(N)`, Space `O(1)`.

```cpp
    int subarraySum(vector<int>& arr) {
        int ans=0;
        int n=arr.size();
        for(int i=0;i<n;i++){
            ans+=arr[i]*(i+1)*(n-i);
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/sum-of-subarrays2229/1
```

---

## 70. Triplet Family

**Q:** Find if there exists any triplet in the array such that the sum of two elements equals the third.

**A:** Sorting + Two Pointers.

**Explanation:**
- Sort the array to allow the two-pointer technique.
- Iterate from `i = 2` to `n-1`, treating `arr[i]` as the target sum.
- Use two pointers `j = 0` and `k = i - 1` to find a pair that sums to `arr[i]`.

**Complexity:** Time `O(N^2)`, Space `O(1)` (or `O(N)` for sorting).

```cpp
    bool findTriplet(vector<int>& arr) {
        sort(arr.begin(),arr.end());
        int n=arr.size();
        for(int i=2;i<n;i++){
            int j=0,k=i-1;
            while(j<k){
                int sum=arr[j]+arr[k];
                if(sum==arr[i]) return true;
                else if(sum<arr[i]) j++;
                else k--;
            }
        }
        return false;
    }

gfg - https://www.geeksforgeeks.org/problems/triplet-family/1
```

---

## 71. kb.txt

*Note: This is a stray text file containing just the word "array", not a valid C++ solution.*

---

# 2D Arrays / Matrix

## 72. 463. Island Perimeter

**Q:** Find the perimeter of a single island (1s) in a grid of water (0s).

**A:** Grid Traversal.

**Explanation:**
- Iterate through every cell in the grid.
- For every land cell (`1`), start by adding 4 to the perimeter.
- Check all four neighbors (up, down, left, right). For every neighbor that is also land, subtract 1 from the perimeter because that side is shared and not on the boundary.

**Complexity:** Time `O(N * M)`, Space `O(1)`.

```cpp
int islandPerimeter(vector<vector<int>>& grid) {
    int n=grid.size(),m=grid[0].size(),ans=0;
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            if(grid[i][j]){    
                ans+=4;
                if(i>0 && grid[i-1][j]) ans--;
                if(i<n-1 && grid[i+1][j]) ans--;
                if(j<m-1 && grid[i][j+1]) ans--;
                if(j>0 && grid[i][j-1]) ans--;
            }
        }
    }
    return ans;
}
```

---

## 73. 944. Delete Columns to Make Sorted

**Q:** Determine the minimum number of columns to delete so that each remaining column is sorted in non-decreasing order.

**A:** Column-by-Column Linear Scan.

**Explanation:**
- Iterate column by column.
- For each column, scan downwards through the strings.
- If any character is smaller than the character directly above it, the column is not sorted; increment the deletion count and break to move to the next column.

**Complexity:** Time `O(N * M)`, Space `O(1)`.

```cpp
  int minDeletionSize(vector<string>& strs) {
        int ans=0;
        int n=strs.size();
        int m=strs[0].size();
        for(int j=0;j<m;j++){
            for(int i=1;i<n;i++){
                if(strs[i][j] < strs[i-1][j]){
                    ans++;
                    break;
                }
            }
        }
        return ans;
  }
```

---

## 74. 1074. Number of Submatrices That Sum to Target

**Q:** Find the number of submatrices that sum to a given target.

**A:** 2D Prefix Sums with Hash Map.

**Explanation:**
- First, calculate the prefix sum for each row to allow `O(1)` sum retrieval for any row segment.
- Iterate through all possible pairs of starting column `c1` and ending column `c2`.
- For a fixed column pair, iterate down the rows accumulating a 1D prefix sum, and use a hash map (like in 'Subarray Sum Equals K') to count submatrices that hit the target.

**Complexity:** Time `O(N^2 * M)`, Space `O(M)`.

```cpp
    int numSubmatrixSumTarget(vector<vector<int>>& matrix, int target) {
        int m=matrix.size(),n=matrix[0].size();
        for(int i=0;i<m;i++){
            for(int j=1;j<n;j++){
                matrix[i][j]+=matrix[i][j-1];
            }
        }

        int cnt = 0;

        for(int c1=0;c1<n;c1++){
            for(int c2=c1;c2<n;c2++){
                unordered_map<int,int> mp;
                mp[0]=1;
                int sum=0;
                for(int r=0;r<m;r++){
                    sum+=matrix[r][c2] - (c1>0 ? matrix[r][c1-1] : 0);
                    cnt+=mp[sum-target];
                    mp[sum]++;
                }
            }
        }
        return cnt;      
    }
```

---

## 75. 1582. Special Positions in a Binary Matrix

**Q:** Find the number of 'special' positions where a `1` exists and all other elements in its row and column are `0`.

**A:** Row and Column Sums.

**Explanation:**
- Precompute the sum of all elements in each row into array `r` and each column into array `c`.
- Since the matrix is binary, a row or column having a sum of `1` means it contains exactly one `1`.
- Iterate through the matrix again; a cell is special if `mat[i][j] == 1`, `r[i] == 1`, and `c[j] == 1`.

**Complexity:** Time `O(N * M)`, Space `O(N + M)`.

```cpp
int numSpecial(vector<vector<int>>& mat) {
    int m=mat.size(), n=mat[0].size(),ans=0;
    vector<int> r(m,0),c(n,0);
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            if(mat[i][j]){
                r[i]++;
                c[j]++;
            }
        }
    }
    for(int i=0;i<m;i++){
        for(int j=0;j<n;j++){
            if(mat[i][j] && r[i]==1 && c[j]==1) ans++;
        }
    }
    return ans;
}
```

---

## 76. 2482. Difference Between Ones and Zeros in Row and Column

**Q:** Construct a difference matrix where `diff[i][j] = onesRow[i] + onesCol[j] - zerosRow[i] - zerosCol[j]`.

**A:** Row and Column Counts.

**Explanation:**
- Traverse the grid to count the number of 1s and 0s in each row and column (`r1`, `r0`, `c1`, `c0`).
- Traverse the grid a second time to compute the formula for each cell.
- Update the matrix in-place and return it.

**Complexity:** Time `O(N * M)`, Space `O(N + M)`.

```cpp
vector<vector<int>> onesMinusZeros(vector<vector<int>>& grid) {
int m=grid.size(), n=grid[0].size();
vector<int> r0(m,0),r1(m,0),c0(n,0),c1(n,0);
for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
        if(grid[i][j]){
            r1[i]++;
            c1[j]++;
        }else{
            r0[i]++;
            c0[j]++;
        }
    }
}
for(int i=0;i<m;i++){
    for(int j=0;j<n;j++){
        grid[i][j]=r1[i]+c1[j]-r0[i]-c0[j];
    }
}
return grid;
}
```

---

## 77. 3212. Count Submatrices With Equal Frequency of X and Y

**Q:** Count submatrices starting at `(0,0)` that contain an equal number of 'X' and 'Y' characters, with at least one 'X'.

**A:** 2D Prefix Sums.

**Explanation:**
- Maintain two 2D prefix sum matrices: `csx` for 'X' counts and `csy` for 'Y' counts.
- For each cell `(i, j)`, calculate the cumulative count of 'X' and 'Y' from `(0,0)` to `(i,j)` using the standard 2D prefix sum formula.
- If the counts are equal and greater than 0, increment the answer.

**Complexity:** Time `O(N * M)`, Space `O(N * M)`.

```cpp
    int numberOfSubmatrices(vector<vector<char>>& grid) {
        int n=grid.size(), m=grid[0].size();
        vector<vector<int>> csx(n,vector<int>(m,0)),csy(n,vector<int>(m,0));
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                csx[i][j]=(grid[i][j]=='X');
                csy[i][j]=(grid[i][j]=='Y');
                if(i>0){
                    csx[i][j]+=csx[i-1][j];
                    csy[i][j]+=csy[i-1][j];
                }
                if(j>0){
                    csx[i][j]+=csx[i][j-1];
                    csy[i][j]+=csy[i][j-1];
                }
                if(i>0 && j>0){
                    csx[i][j]-=csx[i-1][j-1];
                    csy[i][j]-=csy[i-1][j-1];
                }
                if(csx[i][j]==csy[i][j] && csx[i][j]>0) ans++;
            }
        }
        return ans;
    }
```

---

## 78. Exit Point in a Matrix

**Q:** Find the coordinates where you will exit a binary matrix, moving straight on `0` and turning right on `1` (which then becomes `0`).

**A:** Simulation with Direction Vectors.

**Explanation:**
- Use direction arrays `di` and `dj` to represent movement (Right, Down, Left, Up).
- Maintain the current position `(i, j)` and a direction index `k`. Keep track of the last valid position in `ans`.
- When encountering a `1`, set it to `0` and turn right `k = (k + 1) % 4`.
- Move one step in the current direction. The loop terminates when the boundaries are crossed, and `ans` holds the exit point.

**Complexity:** Time `O(N * M)`, Space `O(1)`.

```cpp
    vector<int> FindExitPoint(int n, int m, vector<vector<int>>& matrix) {
        vector<int> ans(2,0);
        int di[]={0,1,0,-1};
        int dj[]={1,0,-1,0};
        int i=0,j=0,k=0;
        while(i>=0 && j>=0 && i<n && j<m){
            if(matrix[i][j]){
                matrix[i][j]=0;
                k=(k+1)%4;
            }
            ans[0]=i;ans[1]=j;
            i=i+di[k];
            j=j+dj[k];
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/exit-point-in-a-matrix0905/1
```

---

