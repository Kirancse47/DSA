# Heaps & Priority Queue — Q&A
> Source: `Kirancse47/DSA` -> `Heaps/` — 11 problems

`priority_queue<int>` is a MAX-heap by default and `priority_queue<int, vector<int>, greater<int>>` is a MIN-heap; `push`/`pop` are `O(log n)` and `top` is `O(1)`; `make_heap` is `O(n)`. The three recurring uses:
- (a) top-K with a size-K heap of the OPPOSITE polarity kept at size K -> `O(n log k)` (e.g., *215. Kth Largest Element in an Array*, *347. Top K Frequent Elements*)
- (b) k-way merge (e.g., *Merge k Sorted Arrays*)
- (c) running median with two heaps (e.g., *295. Find Median from Data Stream*)
Custom comparators for pairs and structs require a functor class with `operator()`.

## 1. 1354. Construct Target Array With Multiple Sums

**Q:** Determine if it is possible to construct a given target array starting from an array of 1s by repeatedly replacing an element with the sum of all elements.

**A:** Work backwards from the target array using a max-heap.

**Explanation:**
- The largest element in the current array must have been formed by adding the sum of the rest of the elements.
- We can find the previous value of the largest element by subtracting the sum of the other elements from it.
- To avoid TLE when the largest element is much larger than the rest, use modulo arithmetic (`mx % sum`).

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
  bool isPossible(vector<int>& target) {
        priority_queue<long long int> pq;
        long long int sum=0;
        for(auto it : target){
            pq.push(it);
            sum+=it;
        }
        while(pq.top()!=1){
            long long int mx=pq.top();pq.pop();
            sum-=mx;
            if(sum<=0 || sum>=mx) return false;
            mx=mx%sum;
            sum+=mx;
            pq.push(mx>0?mx:sum);
            
        }
        return true;
    }
```

---

## 2. 1642. Furthest Building You Can Reach

**Q:** Find the furthest building you can reach given a number of bricks and ladders, where bricks cover height differences exactly and a ladder covers any height difference.

**A:** Use a max-heap to track the largest brick allocations and swap them for ladders when bricks run out.

**Explanation:**
- Always try to use bricks for positive height differences.
- Store the brick quantities used in a max-heap.
- When bricks fall below zero, if a ladder is available, replace the largest previous brick usage with a ladder to reclaim bricks.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
  int furthestBuilding(vector<int>& heights, int bricks, int ladders) {
        int n=heights.size();
        priority_queue<int> maxh;
        int i=0;
        for(i;i<n-1;i++){
            if(heights[i]>=heights[i+1]) continue;
            int diff=heights[i+1]-heights[i];
            if(diff<=bricks){
                bricks-=diff;
                maxh.push(diff);
            }else if(ladders>0){
                if(maxh.size()){
                    int x=maxh.top();
                    if(x>diff){
                        bricks+=x;
                        maxh.pop();
                        maxh.push(diff);
                        bricks-=diff;
                    }
                }
                ladders--;
            }else break;
        }
        return i;
    }
```

---

## 3. 215. Kth Largest Element in an Array

**Q:** Find the kth largest element in an unsorted array.

**A:** Maintain a min-heap of size k.

**Explanation:**
- Push each element into a min-heap.
- When the heap size exceeds k, pop the smallest element.
- The root of the heap will hold the kth largest element at the end.

**Complexity:** Time `O(n log k)`, Space `O(k)`.

```cpp
int findKthLargest(vector<int>& nums, int k) {
        priority_queue<int,vector<int>,greater<int>> minh;
        for(auto it : nums){
            minh.push(it);
            if(minh.size()>k) minh.pop();
        }
        return minh.top();
    }
```

---

## 4. 295. Find Median from Data Stream

**Q:** Design a data structure that supports adding numbers from a data stream and finding their median.

**A:** Maintain two heaps: a max-heap for the lower half and a min-heap for the upper half.

**Explanation:**
- Insert incoming numbers into the min-heap, then move its smallest element to the max-heap.
- Balance the heaps so the max-heap has at most one more element than the min-heap.
- The median is either the root of the max-heap or the average of the two roots.

**Complexity:** Time `O(log n)` per add, `O(1)` per find; Space `O(n)`.

```cpp
class MedianFinder {
    priority_queue<int,vector<int>,greater<int>> mnh;
    priority_queue<int> mxh;
public:
    MedianFinder() {}
    void addNum(int num) {
        mnh.push(num);
        mxh.push(mnh.top());
        mnh.pop();
        if(mxh.size()<mnh.size()){
            mxh.push(mnh.top());
            mnh.pop();
        }
    }
    double findMedian() {
        if(mxh.size()==mnh.size()) return (mxh.top()+mnh.top())/2.0;
        else return mxh.top(); 
    }
};
```

---

## 5. 3066. Minimum Operations to Exceed Threshold Value II

**Q:** Find the minimum number of operations required to make all elements greater than or equal to k.

**A:** Simulate the operations using a min-heap.

**Explanation:**
- Push all elements into a min-heap.
- Extract the two smallest elements if the smallest is below k, compute the new value, and push it back.
- Keep track of the number of operations performed until the top element is at least k.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
    int minOperations(vector<int>& nums, int k) {
        priority_queue<long long, vector<long long> , greater<long long>> minh;
        for(auto it : nums) minh.push(it);
        int ans=0;
        while(!minh.empty() && minh.top()<k){
            long long n1=minh.top();minh.pop();
            long long  n2=minh.top();minh.pop();
            long long  n=((min(n1,n2)*2) + max(n1,n2));
            minh.push(n);
            ans++;
        }
        return ans;
    }
```

---

## 6. 347. Top K Frequent Elements

**Q:** Given an integer array and an integer k, return the k most frequent elements.

**A:** Count frequencies with a hash map and find the top k using a min-heap.

**Explanation:**
- Calculate the frequency of each element using an unordered map.
- Push pairs of `(frequency, element)` into a min-heap, popping when size exceeds k to keep only the top k.
- Extract the elements from the heap and reverse them for the final answer.

**Complexity:** Time `O(n log k)`, Space `O(n)`.

```cpp
    vector<int> topKFrequent(vector<int>& nums, int k) {
        unordered_map<int,int> mp;
        for(auto it : nums){
            mp[it]++;
        }
        priority_queue<pair<int,int>, vector<pair<int,int>> ,greater<pair<int,int>> > minh;
        for(auto it : mp){
            minh.push({it.second,it.first});
            if(minh.size()>k) minh.pop();
        }
        vector<int> ans;
        while(minh.size()>0){
            ans.push_back(minh.top().second);
            minh.pop();
        }
        reverse(ans.begin(),ans.end());
        return ans;
    }
```

---

## 7. 502. IPO

**Q:** Find the maximum capital you can achieve by selecting at most k projects from a list with given profits and capital requirements.

**A:** Use a max-heap to greedily select the most profitable project we can afford.

**Explanation:**
- Sort the projects based on their capital requirements in ascending order.
- Iterate k times: push profits of all affordable projects into a max-heap.
- Pop the highest profit from the heap and add it to our available capital.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
    int findMaximizedCapital(int k, int w, vector<int>& profits, vector<int>& capital) {
        int n = profits.size();
        vector<pair<int,int>>projects;
        for (int i=0;i<n;i++){
            projects.emplace_back(capital[i], profits[i]);
        }
        sort(projects.begin(), projects.end());
        priority_queue<int> maxHeap;
        int i = 0;
        for (int j = 0; j < k; ++j) {
            while (i < n && projects[i].first <= w) {
                maxHeap.push(projects[i].second);
                i++;
            }
            if (maxHeap.empty()) break;
            w += maxHeap.top();
            maxHeap.pop();
        }
        return w;
    }
```

---

## 8. 630. Course Schedule III

**Q:** Find the maximum number of courses you can take given their durations and deadlines.

**A:** Sort by deadline and use a max-heap to drop the longest courses when deadlines are missed.

**Explanation:**
- Sort courses by their deadlines to greedily try finishing them as early as possible.
- Maintain a running sum of time spent and add course durations to a max-heap.
- If the current sum exceeds a course's deadline, remove the longest course taken so far (heap top) to save time.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
int scheduleCourse(vector<vector<int>>& c){
        sort(c.begin(),c.end(),[&](vector<int> &a,vector<int> &b){
            return a[1]<b[1];
        });
        priority_queue<int> pq;
        int  sum=0;
        for(auto it : c){
            sum+=it[0];
            pq.push(it[0]);
            if(sum>it[1]){
                sum-=pq.top();
                pq.pop();
            }
        }
        return pq.size();
    }
```

---

## 9. 703. Kth Largest Element in a Stream

**Q:** Design a class to find the kth largest element in a stream of numbers.

**A:** Maintain a min-heap of size k to keep track of the k largest elements seen so far.

**Explanation:**
- Initialize a min-heap and insert all initial elements, discarding the smallest when size exceeds k.
- On each `add` operation, push the new value and pop if the size exceeds k.
- The root of the min-heap always contains the kth largest element.

**Complexity:** Time `O(log k)` per add, Space `O(k)`.

```cpp
    priority_queue<int,vector<int>,greater<int>> minh;
    int K;
    KthLargest(int k, vector<int>& nums) {
        K=k;
        for(auto it : nums){
            minh.push(it);
            if(minh.size()>k) minh.pop();
        }
    }
    
    int add(int val) {
        minh.push(val);
        if(minh.size()>K) minh.pop();
        return minh.top();
    }
```

---

## 10. Merge k Sorted Arrays

**Q:** Merge k sorted arrays into a single sorted array.

**A:** Use a min-heap to perform a k-way merge.

**Explanation:**
- Push the first element of each array into a min-heap along with its array index and element index.
- Repeatedly extract the minimum element and push the next element from the same array into the heap.
- Continue until the heap is empty.

**Complexity:** Time `O(N log k)` where N is total elements, Space `O(k)`.

```cpp
    // Function to merge k sorted arrays.
    vector<int> mergeKArrays(vector<vector<int>> arr, int k) {
        priority_queue<pair<int,pair<int,int>>,vector<pair<int,pair<int,int>>>,greater<pair<int,pair<int,int>>> > minh;
        vector<int> idx(k,0);
        for(int i=0;i<k;i++){
            minh.push({arr[i][0],{i,0}});
        }
        vector<int> ans;
        while(!minh.empty()){
            auto it = minh.top();minh.pop();
            ans.push_back(it.first);
            int i=it.second.first; //array 
            int j=it.second.second; //idx in array
            if(j+1 < k){
                minh.push({arr[i][j+1],{i,j+1}});
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/merge-k-sorted-arrays/1
```

---

## 11. Minimum Cost of ropes

**Q:** Find the minimum total cost to connect n ropes into one, where the cost of connecting two ropes is the sum of their lengths.

**A:** Greedily connect the two shortest ropes repeatedly using a min-heap.

**Explanation:**
- Push all rope lengths into a min-heap.
- While more than one rope remains, extract the two shortest ropes, add their sum to the total cost, and push the combined rope back.
- The total cost accumulated is the minimum possible cost.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
    long long minCost(vector<long long>& arr) {
        priority_queue<long long,vector<long long>,greater<long long>> minh;
        for(auto it : arr) minh.push(it);
        long long ans=0;
        while(minh.size()>1){
            long long a=minh.top();minh.pop();
            long long b=minh.top();minh.pop();
            ans+=(a+b);
            minh.push(a+b);
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/minimum-cost-of-ropes-1587115620/1
```
