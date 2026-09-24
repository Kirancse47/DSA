# Binary Search — Q&A

> Source: `Kirancse47/DSA` -> `BinarySearch/` — 31 problems

Ordering: Alphabetical

## Cheat sheet

**Plain binary search:**
Search space `[0, n-1]`.
```cpp
int low = 0, high = n - 1;
while(low <= high) {
    int mid = low + (high - low) / 2;
    if(nums[mid] == target) return mid;
    else if(nums[mid] > target) high = mid - 1;
    else low = mid + 1;
}
```
*Examples:* `704. Binary Search`, `34. Find First and Last Position of Element in Sorted Array`

**Binary search on the answer space (min max / max min):**
Find a bound using a monotone predicate.
```cpp
int low = min_possible, high = max_possible, ans = -1;
while(low <= high) {
    int mid = low + (high - low) / 2;
    if(isValid(mid)) { // greedy check
        ans = mid;
        high = mid - 1; // to minimize. (use low = mid + 1 to maximize)
    } else {
        low = mid + 1;
    }
}
```
*Examples:* `1011. Capacity To Ship Packages Within D Days`, `875. Koko Eating Bananas`, `Split Array Largest Sum`

**Search in rotated sorted array:**
Identify which half is sorted to restrict bounds.
*Examples:* `33. Search in Rotated Sorted Array`, `153. Find Minimum in Rotated Sorted Array`

**Binary search on a 2-D matrix:**
Matrix value range `[1, 1e9]`. Predicate sums row-wise `upper_bound` counts.
*Examples:* `Median in a row-wise sorted Matrix`

**Median of two sorted arrays via partition:**
Partition smaller array, derive second partition, check cross boundary elements.
*Examples:* `4. Median of Two Sorted Arrays`

---


## 1. 1011. Capacity To Ship Packages Within D Days

**Q:** Find the minimum ship capacity required to transport all packages within `days` days in the given order.

**A:** Binary search on the answer space of capacities with a greedy feasibility check.

**Explanation:**
- Search space is the possible capacity range `[max(weights), sum(weights)]`.
- The monotone predicate is whether the packages can be shipped within `days` days for a given capacity `mid`.
- `low` and `high` move: if feasible, `high = mid - 1` to find a smaller capacity; else `low = mid + 1`. Return the minimum feasible capacity `ans`.

**Complexity:** Time `O(N log(sum(weights) - max(weights)))` where `N` is weights length, Space `O(1)`.

```cpp
bool isValid(vector<int>& weights, int days,int mid){
        int wt=0;
        int d=1;
        for(int i=0;i<weights.size();i++){
            wt+=weights[i];
            if(wt>mid){
                d++;
                wt=weights[i];
            }
            if(d>days){
                return false;
            }
        }
        return true;
    }
    
    int shipWithinDays(vector<int>& weights, int days) {
        int ans=0;
        int low=INT_MIN,high=0;
        for(auto it : weights){
            low=max(low,it);
            high+=it;
        }
        while(low<=high){
            int mid=low+(high-low)/2;
            if(isValid(weights,days,mid)){
                ans=mid;
                high=mid-1;
            }else{
                low=mid+1;
            }
        }
        return ans;
    }
```

---

## 2. 1283. Find the Smallest Divisor Given a Threshold

**Q:** Find the smallest divisor such that the sum of the array elements divided by the divisor (rounded up) is less than or equal to `threshold`.

**A:** Binary search on the answer space of divisors with a threshold check.

**Explanation:**
- Search space is the possible divisor range `[1, max(nums)]`.
- The monotone predicate checks if the sum of ceiling divisions is `<= threshold`.
- If valid, `high = mid - 1` to find a smaller divisor; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(max(nums)))` where `N` is array size, Space `O(1)`.

```cpp
    bool check(int divisor,vector<int>& nums, int threshold){
        int sum=0;
        for(auto it : nums){
            sum+=(it/divisor)+(it%divisor==0 ? 0 : 1);
        }
        return sum<=threshold;
    }

    int smallestDivisor(vector<int>& nums, int threshold) {
        int low=1;
        int high=*max_element(nums.begin(),nums.end());
        int ans=-1;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(check(mid,nums,threshold)){
                ans=mid;
                high=mid-1;
            }else low=mid+1;
        }
        return ans;
    }
```

---

## 3. 1482. Minimum Number of Days to Make m Bouquets

**Q:** Find the minimum number of days you need to wait to be able to make `m` bouquets of `k` adjacent flowers each.

**A:** Binary search on the answer space of days with a greedy adjacent-flower count check.

**Explanation:**
- Search space is `[1, max(bloomDay)]`. If `m * k > n`, return `-1`.
- The monotone predicate counts adjacent blooming flowers and forms bouquets when `k` contiguous ones are found.
- If we can form `>= m` bouquets, `high = mid - 1`; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(max(bloomDay)))` where `N` is array size, Space `O(1)`.

```cpp
    bool canHappen(int days, vector<int>& bloomDay, int m, int k) {
        int K = 0, bouquets = 0;
        for(int i=0;i<bloomDay.size();i++) {
            if(bloomDay[i] <= days) {
                K++;
                if(K == k) {
                    bouquets++;
                    K = 0;
                }
            }
            else K = 0;
        }
        return bouquets >= m;
    }
    int minDays(vector<int>& bloomDay, int m, int k) {
        int low = 1, high = 1, n = bloomDay.size(), ans = -1;
        if(m > (n / k)) return -1;
        for(int i = 0;i<bloomDay.size();i++) high = max(high, bloomDay[i]);

        while(low <= high) {
            int mid = low + (high - low)/2;
            if(canHappen(mid, bloomDay, m, k)) {
                ans = mid;
                high = mid - 1;
            } else low = mid + 1;
        }

        return ans;
    }
```

---

## 4. 153. Find Minimum in Rotated Sorted Array

**Q:** Find the minimum element in a sorted array that has been rotated between 1 and `n` times.

**A:** Binary search identifying the sorted half to update the minimum candidate.

**Explanation:**
- Search space is the array indices `[0, n-1]`.
- Compares `nums[i]` with `nums[mid]` to determine which half is sorted.
- Updates the running minimum with the smallest element of the sorted half, then searches the unsorted half. Returns `ans`.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    int findMin(vector<int>& nums) {
        int n=nums.size();
        int i=0,j=n-1;
        int ans=INT_MAX;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[i]<=nums[j]){
                ans=min(ans,nums[i]);
                return ans;
            }
            if(nums[i]<=nums[m]){
                ans=min(ans,nums[i]);
                i=m+1;
            }else{
                ans=min(ans,nums[m]);
                j=m-1;
            }
        }
        return ans;
    }
```

---

## 5. 162. Find Peak Element

**Q:** Find the index of any peak element (strictly greater than its neighbors) in an array.

**A:** Binary search on indices leveraging the local slope.

**Explanation:**
- Search space is indices `[1, n-2]` after explicitly checking the array endpoints.
- The predicate checks if `nums[mid]` is a peak, or follows the increasing slope.
- If `nums[mid] > nums[mid+1]`, a peak exists to the left (including `mid`), so `j = m - 1`; else `i = m + 1`.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    int findPeakElement(vector<int>& nums) {
        int n=nums.size();
        if(n==1) return 0;
        if(nums[0]>nums[1]) return 0;
        if(nums[n-1]>nums[n-2]) return n-1;
        int i=1,j=n-2;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]>nums[m+1] && nums[m]>nums[m-1]) return m;
            if(nums[m] > nums[m+1]) j=m-1;
            else i=m+1;
        }
        return -1;
    }
```

---

## 6. 1870. Minimum Speed to Arrive on Time

**Q:** Find the minimum positive integer speed required to travel all distances within `hour` hours.

**A:** Binary search on the answer space of speeds with a time-accumulation check.

**Explanation:**
- Search space is the integer speed range `[1, 1e7]`.
- The monotone predicate sums the ceil of `dist[i]/speed` (except the last distance) and checks if total time `<= hour`.
- If valid, `high = mid - 1` for a smaller speed; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(10^7))` where `N` is distances size, Space `O(1)`.

```cpp
    bool isP(vector<int>& dist, double hour, int mid, int n){
        double total_time=0;
        for(int i=0;i<n;i++){
            double d=dist[i]*1.0/mid;
            if(i!=n-1) total_time+=ceil(d);
            else total_time+=d;
            if(total_time>hour) return 0;
        }
        if(total_time>hour) return 0;
        return 1;
    }

    int minSpeedOnTime(vector<int>& dist, double hour) {
        int n=dist.size();
        int low=1,high=1e7,ans=-1;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(isP(dist,hour,mid,n)){
                ans=mid;
                high=mid-1;
            }else{
                low=mid+1;
            }
        }
        return ans;
    }
```

---

## 7. 2226. Maximum Candies Allocated to K Children

**Q:** Find the maximum number of candies each child can receive such that `k` children get the same amount of candies from sub-piles.

**A:** Binary search on the answer space of candies per child.

**Explanation:**
- Search space is `[1, max(candies)]`.
- The predicate checks if we can form at least `k` piles of size `mid` across all initial piles via integer division.
- If valid, `low = mid + 1` to maximize candies; else `high = mid - 1`. Returns `ans`.

**Complexity:** Time `O(N log(max(candies)))`, Space `O(1)`.

```cpp
    bool check(int cand,vector<int>& candies, long long k){
        for(auto it : candies){
            k-=(it/cand);
            if(k<=0) return true;
        }
        return false;
    }
    int maximumCandies(vector<int>& candies, long long k) {
        int low=1,high=*max_element(candies.begin(),candies.end());
        int ans=0;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(check(mid,candies,k)){
                ans=mid;
                low=mid+1;
            }else high=mid-1;
        }
        return ans;
    }
```

---

## 8. 2529. Maximum Count of Positive Integer and Negative Integer

**Q:** Find the maximum between the count of strictly positive and strictly negative integers in a sorted array.

**A:** Two separate binary searches for the last negative index and the first positive index.

**Explanation:**
- The first search finds `last_neg` by tracking the rightmost index where `nums[mid] < 0`.
- The second search finds `first_pos` by tracking the leftmost index where `nums[mid] > 0`.
- Returns the maximum of `last_neg + 1` (negative count) and `n - first_pos` (positive count).

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    int maximumCount(vector<int>& nums) {
        int n=nums.size();
        int last_neg=-1;
        int i=0,j=n-1;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]<0){
                last_neg=m;
                i=m+1;
            }
            else j=m-1;
        }

        i=0;j=n-1;
        int first_pos=n;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]>0) {
                first_pos=m;
                j=m-1;
            }
            else i=m+1;
        }
        return max(last_neg+1,n-first_pos);
    }
```

---

## 9. 2560. House Robber IV

**Q:** Find the minimum capability (maximum money in a single robbed house) required to rob at least `k` non-adjacent houses.

**A:** Binary search on the answer space of house capacities with a greedy non-adjacent choice check.

**Explanation:**
- Search space is house money values `[1, max(nums)]`.
- The predicate greedily picks valid non-adjacent houses with money `<= mid` and checks if we can pick `k`.
- If valid, `high = mid - 1` to minimize capability; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(max(nums)))`, Space `O(1)`.

```cpp
    bool check(int cap,vector<int> &nums,int k){
        for(int i=0;i<nums.size();i++){
            if(cap>=nums[i]){
                k--;
                if(k==0) return true;
                i++;
            }
        }
        return false;
    }
    int minCapability(vector<int>& nums, int k) {
        int low=1,high=*max_element(nums.begin(),nums.end());
        int ans=0;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(check(mid,nums,k)){
                ans=mid;
                high=mid-1;
            }else low=mid+1;
        }
        return ans;
    }
```

---

## 10. 2594. Minimum Time to Repair Cars

**Q:** Find the minimum time needed for a group of mechanics with given ranks to repair `cars` cars.

**A:** Binary search on the answer space of total time with a capacity check.

**Explanation:**
- Search space is `[1, max(ranks) * cars * cars]`.
- The predicate calculates total cars repairable in `mid` time as `sum(sqrt(mid / rank))` and checks if it's `>= cars`.
- If valid, `high = mid - 1` to minimize time; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(max(ranks) * cars^2))`, Space `O(1)`.

```cpp
    bool check(long long givenTime,vector<int> &ranks,int cars){
        int carDone=0;
        for(auto it : ranks){
            carDone+=sqrt(givenTime/it);
            if(carDone>=cars) return true;
        }
        return false;
    }

    long long repairCars(vector<int>& ranks, int cars) {
        long long low=1;
        long long high=(long long)*max_element(ranks.begin(),ranks.end())*cars*cars;
        long long ans=-1;
        while(low<=high){
            long long mid=low+(high-low)/2;
            if(check(mid,ranks,cars)){
                ans=mid;
                high=mid-1;
            }else low=mid+1;
        }
        return ans;
    }
```

---

## 11. 2616. Minimize the Maximum Difference of Pairs

**Q:** Find the minimum possible maximum difference of `p` pairs of elements chosen from the array.

**A:** Sort the array and binary search on the answer space of maximum differences.

**Explanation:**
- Search space is `[0, max(nums) - min(nums)]` on the sorted array.
- The predicate iterates through the sorted array and greedily forms pairs if `nums[i] - nums[i-1] <= mid`.
- If we can form `p` pairs, `high = mid - 1` to minimize the difference; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log N + N log(max(nums) - min(nums)))`, Space `O(1)`.

```cpp
int isP(vector<int>& nums, int p,int diff){
    for(int i=1;i<nums.size();i++){
        if(abs(nums[i]-nums[i-1])<=diff){
            p--;
            i++;
        }
        if(p==0) return true;
    }
    return p<=0;
}
    
int minimizeMax(vector<int>& nums, int p) {
    int n=nums.size();
    sort(nums.begin(),nums.end());
    int low=0,high=abs(nums[0]-nums[n-1]),ans=0;
    while(low<=high){
        int mid=low+(high-low)/2;
        if(isP(nums,p,mid)){
            ans=mid;
            high=mid-1;
        }else{
            low=mid+1;
        }
    }
    return ans;
}
```

---

## 12. 33. Search in Rotated Sorted Array

**Q:** Find the index of a `target` element in a distinct rotated sorted array, or `-1` if not found.

**A:** Binary search tracking the sorted half of the array, or two binary searches locating the pivot first.

**Explanation:**
- Search space is the array indices `[0, n-1]`.
- Determines which half (`i` to `m` or `m` to `j`) is sorted.
- Checks if the target falls within the sorted half bounds to prune the search space.

**Complexity:** Time `O(log N)`, Space `O(1)`. One-pass and two-pass via pivot approaches are shown.

```cpp
//Sol - 1
int search(vector<int>& nums, int target) {
        int n=nums.size();
        int i=0,j=n-1;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]==target) return m;
            if(nums[i]<=nums[m]){
                if(nums[i]<=target && target<=nums[m]){
                    j=m-1;
                }else i=m+1;
            }else{
                if(nums[m]<=target && target<=nums[j]){
                    i=m+1;
                }else j=m-1;
            }
        }
        return -1;
    }



//Sol - 2
int binarySearch(vector<int> &arr, int i,int j, int x){
        while(i<=j){
            int m=(i+j)/2;
            if(arr[m]==x){
                return m;
            }else if(x<arr[m]){
                j=m-1;
            }else{
                i=m+1;
            }
        }
        return -1;
    }
    
    int search(vector<int>& nums, int target) {
        int n=nums.size();
        int i=0,j=n-1;
        while(i<j){
            int m=(i+j)/2;
            if(nums[m]<nums[j]){
                j=m;
            }else{
                i=m+1;
            }
        }
        int l=binarySearch(nums,0,j-1,target);
        int r=binarySearch(nums,j,n-1,target);
        return max(l,r);
    }
```

---

## 13. 34. Find First and Last Position of Element in Sorted Array

**Q:** Find the starting and ending indices of a `target` value in a sorted array.

**A:** Two separate binary searches to find the first and last occurrences.

**Explanation:**
- The first search records `first = m` and moves `j = m - 1` when `nums[m] == target` to find the earliest index.
- The second search records `last = m` and moves `i = m + 1` when `nums[m] == target` to find the latest index.
- Returns both indices in a vector.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    vector<int> searchRange(vector<int>& nums, int target) {
        vector<int> ans;
        int n=nums.size();
        //for first occurance...
        int i=0,j=n-1;
        int first=-1;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]==target){
                first=m;
                j=m-1;
            }else if(nums[m]>target){
                j=m-1;
            }else{
                i=m+1;
            }
        }
        //for last occurance....
        i=0,j=n-1;
        int last=-1;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]==target){
                last=m;
                i=m+1;
            }else if(nums[m]>target){
                j=m-1;
            }else{
                i=m+1;
            }
        }

        ans.push_back(first);
        ans.push_back(last);
        return ans;
    }
```

---

## 14. 4. Median of Two Sorted Arrays

**Q:** Find the median of two sorted arrays of sizes `n1` and `n2`.

**A:** Binary search on the smaller array to find a valid partition cut.

**Explanation:**
- Search space is indices of the smaller array `[0, n1]`.
- Cuts both arrays such that the left halves and right halves have equal total elements, ensuring `left1 <= right2` and `left2 <= right1`.
- If valid, the median is calculated from the boundary max/min elements.

**Complexity:** Binary search: Time `O(log(min(N1, N2)))`, Space `O(1)`. Merge variants: Time `O(N1+N2)`, Space `O(N1+N2)` or `O(1)`.

```cpp
// Using Two Pointer.....
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int n1=nums1.size(),n2=nums2.size();
        vector<int> a;
        int i=0,j=0;
        while(i<n1 && j<n2){
            if(nums1[i]<nums2[j]){
                a.push_back(nums1[i]);
                i++;
            }else{
                a.push_back(nums2[j]);
                j++;
            }
        }
        while(i<n1){
            a.push_back(nums1[i]);
            i++;
        }
        while(j<n2){
            a.push_back(nums2[j]);
            j++;
        }

        int n=a.size();
        int m=n/2;
        if(n%2!=0){
            return a[m];
        }else{
            return (a[m]+a[m-1])/2.0;
        }
    }
    
// Using counter.....
    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int n=nums1.size(),m=nums2.size();
        int i = 0,j = 0;
        int count; 
        int m1 = -1, m2 = -1;
        for (count = 0; count <= (m + n)/2; count++){
            m2=m1;
            if(i != n && j != m) m1 = (nums1[i] > nums2[j]) ? nums2[j++] : nums1[i++];
            else if(i < n) m1 = nums1[i++];
            else m1 = nums2[j++];
        }
        if((m + n) % 2 == 1) return m1;
        else return (m1+m2)/2.0;
    }
    
// Using Binary Search...
    double findMedianSortedArrays(vector<int>& a1, vector<int>& a2) {
        if(a1.size()>a2.size()){
        return findMedianSortedArrays(a2,a1);
        }
        int n1=a1.size(),n2=a2.size();
        int low=0,high=n1;
        while(low<=high){
            int cut1=(low+high)/2;
            int cut2=(n1+n2+1)/2-cut1;

            int left1= cut1==0 ? INT_MIN : a1[cut1-1]; 
            int left2= cut2==0 ? INT_MIN : a2[cut2-1]; 

            int right1= cut1==n1 ? INT_MAX : a1[cut1]; 
            int right2= cut2==n2 ? INT_MAX : a2[cut2]; 

            if(left1<=right2 && left2<=right1){
                if((n1+n2)%2!=0){
                    return max(left1,left2);
                }else{
                    return ((max(left1,left2)+min(right1,right2))/2.0);
                }
            }else if(left1>right2){
                high=cut1-1;
            }else{
                low=cut1+1;
            }
        }
        return 0.0;
    }
```

---

## 15. 540. Single Element in a Sorted Array

**Q:** Find the single non-duplicate element in a sorted array where every other element appears exactly twice.

**A:** Binary search on array indices exploiting the even-odd index pairing property.

**Explanation:**
- Search space is indices `[1, n-2]` after edge-case checks for the first and last elements.
- If `m` is even, its pair should be at `m + 1`; if `m` is odd, its pair should be at `m - 1`.
- If the pairing matches, the single element is to the right (`i = m + 1`); otherwise to the left (`j = m - 1`).

**Complexity:** Binary Search: Time `O(log N)`, Space `O(1)`. XOR approach: Time `O(N)`, Space `O(1)`.

```cpp
// Using XOR sum...
     int singleNonDuplicate(vector<int>& nums) {
        int ans=0;
        for(auto it : nums) ans^=it;
        return ans;
    }

// Using Binary Search...
        int singleNonDuplicate(vector<int>& nums) {
        int n=nums.size();
        if(n==1) return nums[0];
        if(nums[0]!=nums[1]) return nums[0];
        if(nums[n-1]!=nums[n-2]) return nums[n-1];
        int i=1,j=n-2;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]!=nums[m-1] && nums[m]!=nums[m+1]) return nums[m];
            if((m%2==1 && nums[m]==nums[m-1]) || (m%2==0 && nums[m]==nums[m+1])) i=m+1;
            else j=m-1;
        }
        return -1;
    }
```

---

## 16. 69. Sqrt(x)

**Q:** Compute and return the integer square root of `x`.

**A:** Binary search on the answer space of integers whose square is `<= x`.

**Explanation:**
- Search space is integers `[1, x]`.
- The predicate checks if `mid * mid <= x`.
- If valid, records `mid` and searches higher (`low = mid + 1`); else searches lower (`high = mid - 1`).

**Complexity:** Time `O(log x)`, Space `O(1)`.

```cpp
int mySqrt(int x) {
        long long int low=1,high=x,ans=0;
        while(low<=high){
            long long int mid=(low+high)>>1;
            long long int st=mid*mid;
            if(st==x){
                return mid;
            }
            else if(st<x){
                ans=mid;
                low=mid+1;
            }else{
                high=mid-1;
            }
        }
        return ans;
    }
```

---

## 17. 704. Binary Search

**Q:** Find the index of a `target` element in a standard sorted array.

**A:** Standard binary search.

**Explanation:**
- Search space is array indices `[0, n-1]`.
- Checks `nums[mid] == target`, and moves `low` or `high` accordingly based on magnitude.
- Returns `mid` if found, else `-1`.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    int search(vector<int>& nums, int target) {
        int low=0,high=nums.size()-1;
        while(low<=high){
            int mid=(low+high)>>1;
            if(nums[mid]==target) return mid;
            else if(nums[mid]>target) high=mid-1;
            else low=mid+1;
        }
        return -1;
    }
```

---

## 18. 729. My Calendar I

**Q:** Implement a calendar class that books non-overlapping events.

**A:** Ordered map leveraging `lower_bound` for boundary intersection checks.

**Explanation:**
- Maintains intervals in a `std::map<int, int>` mapping `endTime -> startTime`.
- `lower_bound(startTime + 1)` finds the first event ending strictly after `startTime`.
- If this event starts before `endTime`, there's an overlap. Otherwise inserts the event.

**Complexity:** Time `O(log N)` per booking, Space `O(N)` where `N` is number of bookings.

```cpp
class MyCalendar {
    map<int,int> intervals;
public:
    MyCalendar() {
        
    }
    bool book(int startTime, int endTime) {
        auto it = intervals.lower_bound(startTime+1);
        if(it!=intervals.end() && it->second<endTime) return false;
        intervals[endTime]=startTime;
        return true;
    }
};
```

---

## 19. 81. Search in Rotated Sorted Array II

**Q:** Check if a `target` element exists in a rotated sorted array that may contain duplicates.

**A:** Binary search tracking sorted halves, with edge case shrinking for duplicate endpoints.

**Explanation:**
- Search space is array indices `[0, n-1]`.
- Shrinks the search space `i++` and `j--` if `nums[i] == nums[m] == nums[j]` to bypass duplicates.
- Otherwise, proceeds identical to standard search in a rotated array by identifying the sorted half.

**Complexity:** Time `O(log N)` average, `O(N)` worst-case if all elements are identical, Space `O(1)`.

```cpp
    bool search(vector<int>& nums, int target) {
        int n=nums.size();
        int i=0,j=n-1;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[m]==target) return true;
            if(nums[i]==nums[m] && nums[j]==nums[m]){
                i++;j--;
                continue;
            }
            if(nums[i]<=nums[m]){
                if(nums[i]<=target && target<=nums[m]){
                    j=m-1;
                }else i=m+1;
            }else{
                if(nums[m]<=target && target<=nums[j]){
                    i=m+1;
                }else j=m-1;
            }
        }
        return false;
    }
```

---

## 20. 875. Koko Eating Bananas

**Q:** Find the minimum banana-eating speed `k` to eat all piles within `h` hours.

**A:** Binary search on the answer space of eating speeds with a time check.

**Explanation:**
- Search space is speeds `[1, max(piles)]`.
- The predicate sums `ceil(pile / mid)` and checks if total time `<= h`.
- If valid, `high = mid - 1` to find a smaller speed; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(max(piles)))`, Space `O(1)`.

```cpp
    bool check(int k, vector<int>& piles, int h){
        int speed=0;
        for(int i=0;i<piles.size();i++){
            speed+=piles[i]/k+(piles[i]%k==0 ? 0 : 1);
            if(speed>h) return false;
        }
        return true;
    }
    
    int minEatingSpeed(vector<int>& piles, int h) {
        int low=1, high=*max_element(piles.begin(),piles.end());
        int ans=-1;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(check(mid,piles,h)){
                ans=mid;
                high=mid-1;
            }else{
                low=mid+1;
            }
        }
        return ans;
    }
```

---

## 21. Aggressive cows

**Q:** Find the largest minimum distance to place `k` cows in `N` stalls.

**A:** Sort the stalls and binary search on the answer space of distances.

**Explanation:**
- Search space is distances `[1, stalls.back() - stalls.front()]`.
- The predicate iterates sorted stalls and greedily places a cow if the distance from the last placed cow is `>= mid`.
- If we can place `k` cows, `low = mid + 1` to maximize distance; else `high = mid - 1`. Returns `ans`.

**Complexity:** Time `O(N log N + N log(max_dist))`, Space `O(1)`.

```cpp
    bool check(int dis,vector<int> &sta,int k){
        int pos=sta[0];
        int cow=1;
        for(int i=1;i<sta.size();i++){
            if(sta[i]-pos >= dis){
                cow++;
                pos=sta[i];
            }
            if(cow==k) return true;
        }
        return false;
    }
    
    int aggressiveCows(vector<int> &stalls, int k) {
        sort(stalls.begin(),stalls.end());
        int low=1,high=stalls[stalls.size()-1]-stalls[0];
        int ans=-1;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(check(mid,stalls,k)){
                ans=mid;
                low=mid+1;
            }else high=mid-1;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/aggressive-cows/1
```

---

## 22. Allocate minimum number of pages

**Q:** Allocate books to `m` students such that the maximum number of pages assigned to any student is minimized.

**A:** Binary search on the answer space of maximum page allocations.

**Explanation:**
- Search space is pages `[max(A), sum(A)]`. If `M > N`, return `-1`.
- The predicate greedily allocates books to a student until their sum exceeds `mid`, then assigns to a new student. Checks if students `<= M`.
- If valid, `end = mid - 1` to minimize pages; else `start = mid + 1`. Returns `ans`.

**Complexity:** Time `O(N log(sum(A) - max(A)))`, Space `O(1)`.

```cpp
  bool isValid(int a[],int n,int m,int mid){
        int stu=1;
        int sum=0;
        for(int i=0;i<n;i++){
            sum+=a[i];
            if(sum>mid){
                stu++;
                sum=a[i];
            }
            if(stu>m){
                return false;
            }
        }
        return true;
    }

    int findPages(int A[], int N, int M){
        if(M>N) return -1;
        int start=INT_MIN,end=0;
        for(int i=0;i<N;i++){
            end+=A[i];
            start=max(start,A[i]);
        }
        int ans=-1;
        while(start<=end){
            int mid=start+(end-start)/2;
            if(isValid(A,N,M,mid)){
                ans=mid;
                end=mid-1;
            }else{
                start=mid+1;
            }
        }
        return ans;
    }
gfg link - https://www.geeksforgeeks.org/problems/allocate-minimum-number-of-pages0937/1?itm_source=geeksforgeeks&itm_medium=article&itm_campaign=bottom_sticky_on_article
```

---

## 23. Find Kth Rotation

**Q:** Find the number of times a distinct sorted array has been rotated, which corresponds to the index of the minimum element.

**A:** Binary search identifying the sorted half to find the minimum element's index.

**Explanation:**
- Search space is array indices `[0, n-1]`.
- Compares `nums[lo]` and `nums[mid]` to determine sorted halves.
- Updates a running minimum `ans` and its index `idx`, then searches the unsorted half.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    int findKRotation(vector<int> &nums) {
        int n=nums.size();
        int i=0,j=n-1;
        int ans=INT_MAX;
        int idx=-1;
        while(i<=j){
            int m=i+(j-i)/2;
            if(nums[i]<=nums[j]){
                if(nums[i]<ans){
                    ans=nums[i];
                    idx=i;
                    break;
                }
            }
            if(nums[i]<=nums[m]){
                if(nums[i]<ans){
                    ans=nums[i];
                    idx=i;
                }
                i=m+1;
            }else{
                if(nums[m]<ans){
                    ans=nums[m];
                    idx=m;
                }
                j=m-1;
            }
        }
        return idx;
    }

gfg link - https://www.geeksforgeeks.org/problems/rotation4723/1
```

---

## 24. Find Nth root of M

**Q:** Find the exact integer `n`-th root of `m`, or `-1` if it doesn't exist.

**A:** Binary search on the answer space of integer roots.

**Explanation:**
- Search space is integers `[1, m]`.
- Checks if `pow(mid, n) == m`. (Note: `pow` floating-point precision can be a trap in some bounds).
- If greater, searches lower; if lesser, searches higher.

**Complexity:** Time `O(log M)`, Space `O(1)`.

```cpp
int NthRoot(int n, int m){
	    int low=1,high=m;
        while(low<=high){
            int mid=(low+high)/2;
            if(pow(mid,n)==m) return mid;
            else if(pow(mid,n)>m) high=mid-1;
            else low=mid+1;
        }
        return -1;
	}  
```

---

## 25. Find Pivote Element

**Q:** Find the index of the minimum element (pivot) in a rotated sorted array.

**A:** Binary search to pinpoint the drop-off point.

**Explanation:**
- Search space is array indices `[0, n-1]`.
- The predicate checks if `arr[m] < arr[j]`. If true, the pivot is at `m` or to its left (`j = m`).
- If false, the pivot is to the right (`i = m + 1`). Returns `j`.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
	int findKRotation(int arr[], int n) {
	    // code here
	    int i=0,j=n-1;
	    if(arr[i]<=arr[j]) return 0;
	    while(i<j){
	        int m=i+(j-i)/2;
	        if(arr[m]<arr[j]){
	            j=m;
	        }else{
	            i=m+1;
	        }
	    }
	    return j;
	}
  
  Link - https://practice.geeksforgeeks.org/problems/rotation4723/1
```

---

## 26. Find the closest number

**Q:** Find the closest number to `k` in a sorted array, prioritizing the maximum value on ties.

**A:** Binary search updating the closest candidate on each step.

**Explanation:**
- Search space is array indices `[0, n-1]`.
- Tracks the minimal absolute difference `abs(arr[mid] - k)` and updates `ans`.
- Moves `lo` or `high` based on whether `arr[mid]` is less than or greater than `k`.

**Complexity:** Time `O(log N)`, Space `O(1)`.

```cpp
    int findClosest( int n, int k,int arr[]) { 
        int i=0,j=n-1;
        int diff=INT_MAX;
        int ans=0;
        while(i<=j){
            int mid=i+(j-i)/2;
            int curDiff=arr[mid]-k;
            if(abs(curDiff)==diff){
                ans=max(ans,arr[mid]);
            }else if(abs(curDiff)<diff){
                ans=arr[mid];
                diff=abs(curDiff);
            }
            if(curDiff==0) return arr[mid];
            else if(curDiff>0) j=mid-1;
            else i=mid+1;
        }
        return ans;
    }

gfg Link - https://www.geeksforgeeks.org/problems/find-the-closest-number5513/1
```

---

## 27. Find the minimum time

**Q:** Find the minimum time required to complete `N` tasks using two workers with speeds `s1` and `s2`.

**A:** Binary search on the answer space of time with a task completion check.

**Explanation:**
- Search space is `[0, min(S1, S2) * N]`.
- The predicate checks if `mid / S1 + mid / S2 >= N`.
- If valid, `high = mid - 1` to minimize time; else `low = mid + 1`. Returns `ans`.

**Complexity:** Time `O(log(min(S1, S2) * N))`, Space `O(1)`.

```cpp
    bool check(int m,int s1,int s2,int n){
        if((m/s1+m/s2)<n) return false;
        return true;
    }
    int minTime(int S1, int S2, int N){
        int s1=min(S1,S2);
        int s2=max(S1,S2);
        int low=0, high=s1*N;
        int ans;
        while(low<=high){
            int mid=low+(high-low)/2;
            if(check(mid,s1,s2,N)){
                ans=mid;
                high=mid-1;
            }else low=mid+1;
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/find-the-minimum-time0253/1/#
```

---

## 28. K-th element of two sorted Arrays

**Q:** Find the `k`-th element after merging two sorted arrays.

**A:** Two-pointer linear merge stopping at `k`.

**Explanation:**
- Uses two pointers `i` and `j` to traverse `arr1` and `arr2`.
- Picks the smaller element and decrements `k`.
- Returns the element picked when `k` reaches `0`. (Standard linear merge approach).

**Complexity:** Time `O(K)`, Space `O(1)`.

```cpp
  int kthElement(int arr1[], int arr2[], int n, int m, int k){   
        int i=0,j=0;
        int ans=-1;
        while(k>0 && i<n && j<m){
            if(arr1[i]<arr2[j]){
                ans=arr1[i++];
            }else{
                ans=arr2[j++];
            }
            k--;
        }
        while(k>0 && i<n){
            ans=arr1[i++];
            k--;
        }
        while(k>0 && j<m){
            ans=arr2[j++];
            k--;
        }
        return ans;
    }
```

---

## 29. Median in a row-wise sorted Matrix

**Q:** Find the median in a row-wise sorted matrix.

**A:** Binary search on the answer space of matrix values using binary search per row to count smaller elements.

**Explanation:**
- Search space is possible values `[1, 1e9]`.
- The predicate counts elements `<= mid` across all rows by doing a binary search (`countLessEquqlMid`) per row.
- If the count is `<= (r * c) / 2`, `low = mid + 1` to find a higher median; else `high = mid - 1`. Returns `low`.

**Complexity:** Time `O(R log(C) log(10^9))` where `R` is rows and `C` is columns, Space `O(1)`.

```cpp
    int countLessEquqlMid(vector<int> &row, int mid){
        int l=0,h=row.size()-1;
        while(l<=h){
            int m=(l+h)>>1;
            if(row[m]<=mid){
                l=m+1;
            }else{
                h=m-1;
            }
        }
        return l;
    }
    
    int median(vector<vector<int>> &matrix, int r, int c){
        int low=1,high=1e9;
        while(low<=high){
            int mid=(low+high)>>1;
            int cnt=0;
            for(int i=0;i<r;i++){
                cnt+=countLessEquqlMid(matrix[i],mid);
            }
            if(cnt<=(r*c)/2){
                low=mid+1;
            }else{
                high=mid-1;
            }
        }
        return low;
    }
```

---

## 30. Search in an almost sorted array

**Q:** Find a `target` element in an array where elements can be at most one position away from their sorted index.

**A:** Binary search checking `mid`, `mid-1`, and `mid+1`.

**Explanation:**
- Search space is indices `[l, r]`.
- Checks `arr[mid]`, `arr[mid-1]`, and `arr[mid+1]` directly.
- Recurses on `l` to `mid-2` or `mid+2` to `r` to skip the adjacent checked elements.

**Complexity:** Time `O(log N)`, Space `O(log N)` (recursive stack).

```cpp
int binarySearch(int arr[], int l, int r, int x){
    if (r >= l){
        int mid = l + (r - l) / 2;
        if (arr[mid] == x) return mid;
        if (mid > l && arr[mid - 1] == x) return (mid - 1);
        if (mid < r && arr[mid + 1] == x) return (mid + 1);
        if (arr[mid] > x) return binarySearch(arr, l, mid - 2, x);
        return binarySearch(arr, mid + 2, r, x);
    }
    return -1;
}

Link - https://www.geeksforgeeks.org/search-almost-sorted-array/
```

---

## 31. Split Array Largest Sum

**Q:** Split an array into `K` contiguous subarrays such that the largest sum among these subarrays is minimized.

**A:** Binary search on the answer space of maximum subarray sums.

**Explanation:**
- Search space is `[max(arr), sum(arr)]`.
- The predicate tests if the array can be partitioned into `<= K` subarrays where no subarray sum exceeds `mid`.
- If valid, `high = mid - 1` to minimize the largest sum; else `low = mid + 1`.

**Complexity:** Time `O(N log(sum(arr) - max(arr)))`, Space `O(1)`.

```cpp
   bool isValid(int a[], int n, int k, int mid){
       int sum=0,cnt=1;
       for(int i=0;i<n;i++){
           if(sum+a[i]<=mid) sum+=a[i];
           else{
               sum=a[i];
               cnt++;
           }
           if(cnt>k) return false;
       }
       if(cnt<=k) return true;
       return false;
   }
  
    int splitArray(int arr[] ,int N, int K) {
        int ans=INT_MAX;
        int mx=INT_MIN,sum=0;
        for(int i=0;i<N;i++){
            mx=max(mx,arr[i]);
            sum+=arr[i];
        }
        int low=mx,high=sum,mid;
        while(low<=high){
            mid=low+(high-low)/2;
            if(isValid(arr,N,K,mid)){
                ans=min(ans,mid);
                high=mid-1;
            }else low=mid+1;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/split-array-largest-sum--141634/1
```

---
