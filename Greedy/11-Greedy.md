# Greedy — Q&A

> Source: `Kirancse47/DSA` -> `Greedy/` — 30 problems

Ordering: Numerical by LeetCode ID, followed by classical problems alphabetically.

## Cheat sheet
A greedy algorithm makes locally optimal choices at each step to find a global optimum. It works if the problem has the **greedy-choice property** (a local optimum never locks you out of the global optimum) and **optimal substructure** (an optimal solution to the problem contains optimal solutions to subproblems).

**Greedy vs DP - how to tell:** If making the local best choice is always safe without needing to explore overlapping subproblems or alternate paths, it's greedy. If future decisions depend heavily on the branch taken now, it requires DP.

**Common Sort Keys & Patterns:**
*   **Sort by end time:** Used when finding the maximum number of non-overlapping intervals. Correct because finishing earlier leaves the most room for subsequent choices. (e.g., *Activity Selection*, *N meetings in one room*, *452. Minimum Number of Arrows*).
*   **Sort by start time:** Used when merging intervals or allocating resources over time. Correct because you process events chronologically. (e.g., *Minimum Platforms*, *3169. Count Days Without Meetings*).
*   **Sort by ratio (Value/Weight):** Correct for continuous resource allocation where maximizing density yields the highest total. (e.g., *Fractional Knapsack*).
*   **Sort by deadline (descending profit):** Assign highest-profit items to their latest possible valid slot to reserve earlier slots for other tasks. (e.g., *Job Sequencing Problem*).
*   **Sort by frequency / value:** Matching biggest constraints to biggest capacities. (e.g., *455. Assign Cookies*, *881. Boats to Save People*, *2285. Maximum Total Importance*).
*   **Running Deficit / Capacity Scan:** No sorting needed. Scan linearly; if a local segment fails (e.g., gas runs out), the starting point must be reset strictly *after* the failure point. (e.g., *134. Gas Station*, *55. Jump Game*).

---

## 1. 1007. Minimum Domino Rotations For Equal Row

**Q:** Given two arrays representing top and bottom halves of dominoes, return the minimum rotations to make one row all the same number.

**A:** Try making all elements match each possible number (1-6) and take the minimum valid rotation count.

**Explanation:**
- The target number must be either the top or bottom of the first domino, but the code checks all numbers 1-6 for simplicity.
- The greedy choice is to iterate through both rows and count swaps needed to align each candidate number to either the top or the bottom.
- If a candidate cannot be placed in a column (it exists in neither top nor bottom), that candidate is impossible.

**Complexity:** Time `O(n)` because we check 6 numbers across the arrays, Space `O(1)`.

```cpp
int tryMatch(int number,vector<int>& first,vector<int>& second){
    int count = 0;
    for(int i=0;i<first.size();++i){
        if(first[i]==number) continue;
        else if(second[i]==number) count++;
        else return INT_MAX;
    }
    return count;
}

int minDominoRotations(vector<int>& tops, vector<int>& bottoms) {
    int min_rotations = INT_MAX;
    for(int number = 1; number<=6; number++){
        int count_tops = tryMatch(number,tops,bottoms);
        int count_bottoms = INT_MAX;
        if(count_tops!=INT_MAX) count_bottoms = tryMatch(number,bottoms,tops);
        min_rotations = min(min_rotations,min(count_tops,count_bottoms));
    }
    return min_rotations==INT_MAX? -1: min_rotations;
}
```

---

## 2. 134. Gas Station

**Q:** Find the starting gas station index from which you can travel a circular route without running out of gas.

**A:** Scan linearly while tracking current gas; if current gas drops below zero, reset the start index to the next station.

**Explanation:**
- **Running Deficit Scan:** If you can't reach station `B` from `A`, no station between `A` and `B` can reach `B` either.
- The greedy choice is to immediately reset the candidate start index to `i + 1` whenever the tank becomes negative.
- If the total gas across all stations is at least the total cost, a valid starting station is guaranteed to exist.

**Complexity:** Time `O(n)` for optimized scan (or `O(n^2)` for the TLE brute force), Space `O(1)`.

```cpp
// Broute Force ....(TLE)
int solve(int s,vector<int>& gas, vector<int>& cost){
    int n=gas.size();
    int curP=0;
    for(int i=0;i<n;i++){
        int idx=(s+i)%n;
        curP+=gas[idx];
        if(curP>=cost[idx]) curP-=cost[idx];
        else return false;
    }
    return true;
}

int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
    int n=gas.size();
    for(int i=0;i<n;i++){
        if(gas[i]>=cost[i] && solve(i,gas,cost)) return i;
    }
    return -1;
}

// Optimized...
int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
    int n=gas.size();
    int tgas=0,tcost=0;
    for(int i=0;i<n;i++){
        tgas+=gas[i];
        tcost+=cost[i];
    }
    if(tcost>tgas) return -1;
    int curG=0,start=0;
    for(int i=0;i<n;i++){
        if(curG<0){
            start=i;
            curG=0;
        }
        curG+=(gas[i]-cost[i]);
    }
    return start;
}
```

---

## 3. 135. Candy

**Q:** Distribute the minimum number of candies to children such that each gets at least one, and a child with a higher rating than their neighbor gets more candies.

**A:** Do a two-pass scan (left-to-right then right-to-left) to satisfy the rating inequalities locally.

**Explanation:**
- **Left-to-right pass:** Ensures every child with a higher rating than the left neighbor gets more candy.
- **Right-to-left pass:** Ensures every child with a higher rating than the right neighbor gets more candy, taking the maximum of the two passes.
- This greedily satisfies both left and right dependencies independently using the minimal valid increments.

**Complexity:** Time `O(n)`, Space `O(n)` to store the candy distribution array.

```cpp
int candy(vector<int>& rat) {
    int n=rat.size();
    vector<int> can(n,1);
    for(int i=1;i<n;i++){
        if(rat[i]>rat[i-1]) can[i]=can[i-1]+1;
    }
    int candy=can[n-1];
    for(int i=n-2;i>=0;i--){
        if(rat[i]>rat[i+1] && can[i]<=can[i+1]) can[i]=can[i+1]+1;
        candy+=can[i];
    }
    return candy;
}
```

---

## 4. 1465. Maximum Area of a Piece of Cake After Horizontal and Vertical Cuts

**Q:** Find the maximum area of a piece of cake after making horizontal and vertical cuts.

**A:** Sort the cuts and multiply the maximum gap between adjacent horizontal cuts by the maximum gap between adjacent vertical cuts.

**Explanation:**
- **Sort by position:** By sorting the cuts, adjacent array elements represent the actual physical segments of the cake.
- The greedy choice is that the largest horizontal segment and the largest vertical segment will inherently intersect to form the largest area.
- Multiplication is done modulo `1e9+7` to prevent overflow.

**Complexity:** Time `O(n log n + m log m)` where `n` and `m` are cut array sizes, Space `O(1)`.

```cpp
int maxArea(int h, int w, vector<int>& horizontalCuts, vector<int>& verticalCuts) {
    int nh=horizontalCuts.size(),nv=verticalCuts.size();
    sort(horizontalCuts.begin(),horizontalCuts.end());
    sort(verticalCuts.begin(),verticalCuts.end());
    int mh=max(horizontalCuts[0],(h-horizontalCuts[nh-1]));
    int mv=max(verticalCuts[0],(w-verticalCuts[nv-1]));
    for(int i=1;i<nh;i++){
        mh=max(mh,(horizontalCuts[i]-horizontalCuts[i-1]));
    }
    
    for(int i=1;i<nv;i++){
        mv=max(mv,(verticalCuts[i]-verticalCuts[i-1]));
    }
    
    int mod = 1e9+7;
    
    return (((long long)mv%mod)*((long long)mh%mod))%mod;
}
```

---

## 5. 1689. Partitioning Into Minimum Number Of Deci-Binary Numbers

**Q:** Find the minimum number of positive deci-binary numbers needed to sum up to a given string of digits `n`.

**A:** Find and return the maximum single digit present in the string.

**Explanation:**
- Since each deci-binary number contributes at most `1` to any digit position, a digit `d` requires exactly `d` numbers to reach its value.
- The greedy choice is bounded completely by the largest digit in the string.
- No actual partitioning is needed; the maximum digit defines the lower bound.

**Complexity:** Time `O(L)` where `L` is the length of string `n`, Space `O(1)`.

```cpp
int minPartitions(string n) {
    int ans=0;
    for(auto it : n) ans=max(ans,it-'0');
    return ans;
}
```

---

## 6. 1710. Maximum Units on a Truck

**Q:** Given box types (number of boxes and units per box), maximize the total units loaded onto a truck of limited size.

**A:** Sort box types by units per box descending and greedily take as many boxes as possible from the top.

**Explanation:**
- **Sort by unit density:** Picking the box with the most units first guarantees the maximum payload for the consumed capacity.
- This is a classic greedy knapsack variant where we can take whole boxes until the truck fills up, taking a fraction of a box group at the very end.
- The exchange argument proves that swapping a higher-unit box for a lower-unit box strictly decreases total profit.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
int maximumUnits(vector<vector<int>>& boxTypes, int truckSize) {
     sort(boxTypes.begin(),boxTypes.end(),[&](vector<int> &a, vector<int> &b){ return a[1]>b[1]; });
     int ans=0;
     for(auto it : boxTypes){
         if(truckSize>=it[0]){
             ans+=it[0]*it[1];
             truckSize-=it[0];
         }else{
             ans+=truckSize*it[1];
             truckSize=0;
             break;
         }
     }
     return ans;
 }
```

---

## 7. 1833. Maximum Ice Cream Bars

**Q:** Maximize the number of ice cream bars you can buy given an array of costs and a total coin limit.

**A:** Sort the ice cream costs ascending and buy them starting from the cheapest until coins run out.

**Explanation:**
- **Sort by cost:** Buying cheaper items consumes less of the coin budget, leaving maximum capacity for more items.
- The greedy choice guarantees that replacing any purchased cheap bar with a more expensive one would result in equal or fewer total bars.
- Iteration stops the moment the current bar's cost exceeds the remaining coins.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
int maxIceCream(vector<int>& costs, int coins) {
    sort(costs.begin(),costs.end());
    int i=0,n=costs.size();
    for(;i<n;i++){
        coins-=costs[i];
        if(coins<0) break;
    }
    return i;
}
```

---

## 8. 2285. Maximum Total Importance of Roads

**Q:** Assign values `1` to `n` to cities to maximize the total importance of all roads (sum of endpoint values).

**A:** Count the degree (number of roads) of each city, sort by degree, and assign the highest values to the highest-degree cities.

**Explanation:**
- **Sort by frequency/degree:** A city's value is added to the total score once for every road connected to it.
- The greedy choice is to assign the largest multiplier (`n`) to the city that gets added the most times.
- We map each city to its optimal value and then calculate the total sum iterating over the roads.

**Complexity:** Time `O(n log n)` due to sorting degrees, Space `O(n)`.

```cpp
long long maximumImportance(int n, vector<vector<int>>& roads) {
    vector<int> mp(n,0);
    for(auto it : roads){
        mp[it[0]]++;
        mp[it[1]]++;
    }
    vector<pair<int,int>> temp;
    for(int i=0;i<n;i++){
        temp.push_back({mp[i],i});
    }
    sort(temp.begin(),temp.end());
    
    unordered_map<int,int> m;
    int no=1;
    for(auto it : temp){
        m[it.second]=no;
        no++;
    }
    
    long long ans=0;
    for(auto it : roads){
        ans+=m[it[0]];
        ans+=m[it[1]];
    }
    return ans;
}
```

---

## 9. 2294. Partition Array Such That Maximum Difference Is K

**Q:** Partition an array into the minimum number of subsequences such that the max difference in each subsequence is at most `k`.

**A:** Sort the array and start a new subsequence whenever the current element exceeds the subsequence's minimum element plus `k`.

**Explanation:**
- **Sort by value:** Sorting brings elements with close values together, allowing us to form optimal contiguous windows.
- The greedy choice is to pack as many consecutive elements as possible into the current partition until the constraint `max - min <= k` is violated.
- This guarantees the fewest number of boundaries.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
int partitionArray(vector<int>& nums, int k) {
    sort(nums.begin(),nums.end());
    int ans=0,minVal=nums[0];
    for(int i=1;i<nums.size();i++){
        if(nums[i]>minVal+k){
            ans++;
            minVal=nums[i];
        }
    }
    return ans+1;
}
```

---

## 10. 3169. Count Days Without Meetings

**Q:** Given the total number of days and an array of meeting intervals, count the days that have no meetings scheduled.

**A:** Sort and merge overlapping meeting intervals, then count the un-merged gap days.

**Explanation:**
- **Sort by start time:** Sorting lets us process meetings chronologically to easily merge overlapping or adjacent intervals.
- The greedy choice tracks the farthest end-date seen so far (`t[1]`); if a new meeting starts after this, a gap is confirmed.
- Total free days are the gap before the first meeting, gaps between merged meetings, and the gap after the last meeting.

**Complexity:** Time `O(n log n)`, Space `O(n)` to store merged intervals.

```cpp
int countDays(int days, vector<vector<int>>& meets) {
    sort(meets.begin(),meets.end());
    vector<vector<int>> m;
    vector<int> t=meets[0];
    for(auto it : meets){
        if(it[0]<=t[1]){
            t[1]=max(t[1],it[1]);
        }else{
            m.push_back(t);
            t=it;
        }
    }
    m.push_back(t);

    int res=0;
    int n=m.size();
    res+=(m[0][0]-1);
    for(int i=0;i<n-1;i++){
        if(m[i+1][0]>m[i][1]) res+=(m[i+1][0]-m[i][1]-1);
    }
    res+=(days-m[n-1][1]);
    return res;
}
```

---

## 11. 3191. Minimum Operations to Make Binary Array Elements Equal to One I

**Q:** Find the minimum operations to flip all elements to 1, where one operation flips three consecutive elements.

**A:** Scan left-to-right; if a `0` is found, greedily flip it and the next two elements.

**Explanation:**
- **Deterministic state scan:** Once we pass an index, we cannot go back to flip it. Thus, the leftmost `0` *must* be fixed by a flip starting at its exact index.
- The greedy choice flips a 3-element window strictly when the current element is `0`.
- If the array cannot be fully resolved (checked via the last two elements), we return -1.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
int minOperations(vector<int>& nums) {
    int ans=0;
    int n=nums.size();
    for(int i=0;i<=n-3;i++){
        if(nums[i]==0){
            ans++;
            nums[i]^=1;
            nums[i+1]^=1;
            nums[i+2]^=1;
        }
    }
    if(nums[n-2]==0 || nums[n-1]==0) return -1;
    return ans;
}
```

---

## 12. 334. Increasing Triplet Subsequence

**Q:** Determine if there exists an increasing triplet `i < j < k` in the array.

**A:** Track the smallest and second smallest values seen so far; return true if a value greater than both is found.

**Explanation:**
- **Running threshold scan:** By maintaining the lowest possible values for the first two elements of a potential triplet, we maximize the chance of finding a third.
- If we find an element smaller than `i`, it becomes the new `i`. If it's between `i` and `j`, it becomes the new `j`.
- A third element greater than `j` instantly confirms the triplet property.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
bool increasingTriplet(vector<int>& nums) {
    int i=INT_MAX,j=INT_MAX;
    for(auto k : nums){
        if(k<=i) i=k;
        else if(k<=j) j=k;
        else return true;
    }
    return false;
}
```

---

## 13. 3394. Check if Grid can be Cut into Sections

**Q:** Check if rectangles can be separated by at least 2 distinct horizontal or vertical non-overlapping cut lines.

**A:** Use line-sweep by sorting interval boundaries; a valid cut exists when the running overlap drops to 0.

**Explanation:**
- **Sort by boundaries (Line Sweep):** Events are tagged with `1` for start and `0` for end. Sorting allows counting active rectangles.
- The greedy choice is that an overlap of `0` means the grid can physically be severed there.
- We check if either the X or Y projections yield at least 3 disjoint sections (i.e., at least 2 valid cuts).

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
#define pii pair<int,int>
bool countLineIntersections(vector<pii>& coordinates){
    int lines = 0;
    int overlap = 0;
    for(int i=0;i<coordinates.size();++i){
        if(coordinates[i].second==0)    overlap--;
        else                            overlap++;
        if(overlap==0)
            lines++;
    }
    return lines>=3;
}
bool checkValidCuts(int n, vector<vector<int>>& rectangles) {
    vector<pii> y_cordinates,x_cordinates;
    for(auto& rectangle: rectangles){
        y_cordinates.push_back(make_pair(rectangle[1],1));
        y_cordinates.push_back(make_pair(rectangle[3],0));
        x_cordinates.push_back(make_pair(rectangle[0],1));
        x_cordinates.push_back(make_pair(rectangle[2],0));
    }
    sort(y_cordinates.begin(),y_cordinates.end());
    sort(x_cordinates.begin(),x_cordinates.end());

    //Line-Sweep on x and y cordinates
    return (countLineIntersections(y_cordinates) or countLineIntersections(x_cordinates));
}
```

---

## 14. 452. Minimum Number of Arrows to Burst Balloons

**Q:** Find the minimum arrows required to burst all overlapping balloon intervals.

**A:** Sort balloons by end coordinate and shoot an arrow at the end of the current overlapping cluster.

**Explanation:**
- **Sort by end time:** Sorting by the end coordinate ensures we greedily delay shooting the arrow until the absolute last moment, hitting maximum balloons.
- If the next balloon starts *after* our current arrow limit, a new arrow is necessary and the limit updates.
- This is mathematically equivalent to the classic activity selection problem.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
int findMinArrowShots(vector<vector<int>>& points) {
    sort(points.begin(),points.end(),[&](vector<int> &a,vector<int> &b){
        return a[1]<b[1];
    });
    int ans=1;
    int limit=points[0][1];
    for(int i=1;i<points.size();i++){
        if(points[i][0]>limit){
            limit=points[i][1];
            ans++;
        }
    }
    return ans;
}
```

---

## 15. 455. Assign Cookies

**Q:** Maximize the number of content children given their greed factors and the available cookie sizes.

**A:** Sort both arrays and greedily match the smallest adequate cookie to the least greedy child.

**Explanation:**
- **Sort by constraint:** Matching the smallest cookie that works minimizes waste, preserving larger cookies for greedier children.
- Two pointers iterate through children and cookies; if a cookie fits, both advance; otherwise, only the cookie pointer advances to try a larger cookie.
- This guarantees the maximum pairing density.

**Complexity:** Time `O(n log n + m log m)`, Space `O(1)`.

```cpp
int findContentChildren(vector<int>& g, vector<int>& s) {
    sort(g.begin(),g.end());
    sort(s.begin(),s.end());
    int ans=0,n=g.size(),m=s.size();
    int i=0,j=0;
    while(i<n && j<m){
        if(s[j]>=g[i]){
            ans++;
            i++;j++;
        }else{
            j++;
        }
    }
    return ans;
}
```

---

## 16. 45. Jump Game II

**Q:** Find the minimum number of jumps to reach the last index of an array where elements represent maximum jump lengths.

**A:** Maintain the maximum reach for the current jump; when you hit the boundary of the current jump, increment the jump count.

**Explanation:**
- **Running threshold scan:** `curReach` tracks how far we can currently go, and `curMax` tracks how far we can go on the *next* jump.
- The greedy choice defers committing to a new jump until we hit the end of our current reach.
- This implicitly explores all options in `O(1)` space without building a BFS queue.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
int jump(vector<int>& nums) {
    int n=nums.size();
    int jump=0;
    int curMax=0;
    int curReach=0;
    for(int i=0;i<n-1;i++){
        curMax=max(curMax,i+nums[i]);
        if(i>curReach) return -1;
        if(i==curReach){
            jump++;
            curReach=curMax;
        }
    }
    if(curReach<n-1) return -1;
    return jump;
}
```

---

## 17. 55. Jump Game

**Q:** Determine if you can reach the last index starting from index 0.

**A:** Track the furthest reachable index; if you encounter an index beyond your reach, return false.

**Explanation:**
- **Running deficit scan:** We maintain `reachable`. If at any `i`, `i > reachable`, the progression is broken.
- The greedy choice updates `reachable` to the maximum of its current value and `i + nums[i]`.
- If the loop completes or `reachable` exceeds the last index, the end is reachable.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
bool canJump(vector<int>& nums) {
    if(nums.size()<2) return true;
    if(nums[0]==0) return false;
    int reachable=nums[0];
    for(int i=1;i<nums.size();i++){
        if(i>reachable) return false;
        reachable=max(reachable,i+nums[i]);
    }
    return true;
}
```

---

## 18. 624. Maximum Distance in Arrays

**Q:** Find the maximum absolute difference between elements selected from two different sorted arrays.

**A:** Track the global min and max seen so far and compare them against the ends of the current array.

**Explanation:**
- **Running boundaries scan:** Instead of comparing all pairs, we only care about the minimums and maximums since the individual arrays are sorted.
- The greedy choice updates the running maximum distance using the current array's tail against the global min, and the current array's head against the global max.
- The global min and max are updated *after* the distance calculation to ensure elements come from different arrays.

**Complexity:** Time `O(n)` where `n` is the number of arrays, Space `O(1)`.

```cpp
int maxDistance(vector<vector<int>>& arrays) {
    int mn=arrays[0][0];
    int mx=arrays[0].back();
    int ans=0;
    for(int i=1;i<arrays.size();i++){
        ans=max(ans,abs(mn-arrays[i].back()));
        ans=max(ans,abs(mx-arrays[i][0]));
        mn=min(mn,arrays[i][0]);
        mx=max(mx,arrays[i].back());
    }
    return ans;
}
```

---

## 19. 873. Length of Longest Fibonacci Subsequence

**Q:** Find the length of the longest Fibonacci-like subsequence in a strictly increasing array.

**A:** Use a hash set for `O(1)` lookups and try all pairs as the first two elements of a Fibonacci sequence.

**Explanation:**
- **State exploration:** This uses a pseudo-greedy/brute-force approach where we lock in pairs `(arr[i], arr[j])` and greedily chain the sum.
- Because the array values grow exponentially in a Fibonacci sequence, the while loop executes very few times.
- A hash set enables instantly checking if the required next term exists.

**Complexity:** Time `O(n^2 log M)` where `M` is the max element, Space `O(n)` for the hash set.

```cpp
int lenLongestFibSubseq(vector<int>& arr) {
    int n=arr.size();
    unordered_set<int> s(arr.begin(),arr.end());
    int ans=0;
    for(int i=0;i<n-1;i++){
        for(int j=i+1;j<n;j++){
            int a=arr[i];
            int b=arr[j];
            int fib_len=2;
            while(s.find(a+b)!=s.end()){
                int sum=a+b;
                a=b;
                b=sum;
                fib_len++;
            }
            if(fib_len>2) ans=max(ans,fib_len);
        }
    }
    return ans;
}
```

---

## 20. 881. Boats to Save People

**Q:** Find the minimum number of boats to rescue people given a weight limit, where each boat carries at most 2 people.

**A:** Sort by weight and greedily pair the heaviest person with the lightest person if they fit together.

**Explanation:**
- **Sort by weight (Two pointers):** The heaviest person is the hardest to accommodate. If they can pair with the lightest person, we save a boat.
- If they cannot fit together, the heaviest person must go alone, and we decrement the heavy pointer.
- This securely optimizes pairing capacity.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
int numRescueBoats(vector<int>& people, int limit) {
    sort(people.begin(),people.end());
    int n=people.size();
    int i=0,j=n-1,ans=0;
    while(i<=j){
        ans++;
        if((people[i]+people[j])<=limit) i++;
        j--;
    }
    return ans;
}
```

---

## 21. 948. Bag of Tokens

**Q:** Maximize your score by playing tokens face-up (lose power, gain score) or face-down (lose score, gain power).

**A:** Sort tokens and use a two-pointer approach: play lightest tokens face-up to gain score, and heaviest face-down to gain power.

**Explanation:**
- **Sort by value (Two pointers):** Gaining a point should be done as cheaply as possible (left pointer). Buying power should yield as much as possible (right pointer).
- The greedy choice always prioritizes scoring if power allows; otherwise, it trades 1 score for maximum power to fuel future scoring.
- The maximum score achieved is tracked during the process in case trading power is a net loss at the end.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
int bagOfTokensScore(vector<int>& tokens, int power) {
    int n=tokens.size();
    if(n==0) return 0;
    sort(tokens.begin(),tokens.end());
    if(power<tokens[0]) return 0;
    int i=0,j=n-1;
    int ans=0;
    int res=0;
    while(i<=j){
        if(power>=tokens[i]){
            ans++;
            power-=tokens[i];
            i++;
            res=max(res,ans);
        }else if(ans>0){
            ans--;
            power+=tokens[j];
            j--;
        }else break;
    }
    return res;
}
```

---

## 22. Activity Selection

**Q:** Find the maximum number of activities that can be performed by a single person given their start and end times.

**A:** Sort activities by end time and iteratively select the next activity that starts after the current one finishes.

**Explanation:**
- **Sort by end time:** Finishing an activity as early as possible maximizes the remaining time available for subsequent activities.
- The greedy choice ensures that no other activity could leave more room for the future.
- If a subsequent activity's start time conflicts with the ongoing end time, it is skipped.

**Complexity:** Time `O(n log n)`, Space `O(n)` for pairing start and end arrays.

```cpp
int activitySelection(vector<int> start, vector<int> end, int n){
    vector<pair<int,int>> a;
    for(int i=0;i<n;i++){
        a.push_back({start[i],end[i]});
    }
    sort(a.begin(),a.end(),[](pair<int,int> &p1,pair<int,int> &p2){
        return p1.second<p2.second;
    });
    int i, j;
    i = 0;
    int ans=1;
    for (j = 1; j < n; j++){
      if (a[j].first > a[i].second){
          ans++;
          i = j;
      }
    }
    return ans;
}

Link - https://practice.geeksforgeeks.org/problems/activity-selection-1587115620/1
```

---

## 23. Fractional Knapsack

**Q:** Maximize total value in a knapsack of capacity `W` by taking whole or fractional parts of items.

**A:** Sort items by value-to-weight ratio descending and fill the knapsack, taking a fraction of the last item if needed.

**Explanation:**
- **Sort by ratio (Value/Weight):** Taking the item with the highest density guarantees maximum value per unit of weight consumed.
- Unlike the 0/1 knapsack (which requires DP), the ability to take fractions ensures the greedy approach is perfectly optimal.
- The algorithm stops when capacity hits `0`.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
static bool cmp(Item i1,Item i2){
    double v1=(double)i1.value/i1.weight;
    double v2=(double)i2.value/i2.weight;
    return v1 > v2;
}
double fractionalKnapsack(int W, Item arr[], int n){
    sort(arr,arr+n,cmp);
    double profit=0;
    for(int i=0;i<n;i++){
        if(W>=arr[i].weight){
            W-=arr[i].weight;
            profit+=arr[i].value;
        }else{
            double vw=(double)arr[i].value/arr[i].weight;
            profit+=vw*W;
            W=0;
            break;   
        }
    }
    return profit;
}

Link - https://practice.geeksforgeeks.org/problems/fractional-knapsack-1587115620/1#
```

---

## 24. Job Sequencing Problem

**Q:** Maximize profit by scheduling jobs (each taking 1 unit of time) before their respective deadlines.

**A:** Sort jobs by profit descending and place each job in the latest possible available time slot before its deadline.

**Explanation:**
- **Sort by deadline & profit:** Sorting by profit ensures we prioritize high-value jobs.
- The greedy choice of placing a job as close to its deadline as possible leaves earlier slots open for jobs with tighter deadlines.
- We simulate slots with an array initialized to `-1`.

**Complexity:** Time `O(n log n + n * max_deadline)`, Space `O(max_deadline)` for the slots.

```cpp
vector<int> JobScheduling(Job arr[], int n) { 
    sort(arr,arr+n,[&](Job a, Job b){
        return a.profit > b.profit;
    });
    
    int maxd=arr[0].dead;
    for(int i=1;i<n;i++){
        maxd=max(maxd,arr[i].dead);
    }
    
    vector<int> slot(maxd+1,-1);
    int jobs=0,profit=0;
    
    for(int i=0;i<n;i++){
        for(int j=arr[i].dead;j>0;j--){
            if(slot[j]==-1){
                jobs++;
                slot[j]=arr[i].id;
                profit+=arr[i].profit;
                break;
            }
        }
    }
    return {jobs,profit};
}

Link - https://practice.geeksforgeeks.org/problems/job-sequencing-problem-1587115620/1#
```

---

## 25. Minimum number of Coins

**Q:** Find the minimum number of coins to make a given value `N` using Indian currency denominations.

**A:** Iterate through denominations from highest to lowest and greedily subtract the largest possible coin.

**Explanation:**
- **Running capacity scan:** By always taking the largest denomination that fits, we reduce the remaining total `N` in the fewest steps.
- This greedy strategy works specifically because the given coin denominations follow a canonical system where local minimums guarantee global minimums.
- It might fail for arbitrary coin systems (e.g., `{1, 3, 4}` for value `6`), which would require DP.

**Complexity:** Time `O(N)` in worst case (though bounded tightly by denominations), Space `O(N)` for result vector.

```cpp
vector<int> minPartition(int N){
    // code here
    vector<int> ans;
    int coins[] = { 1, 2, 5, 10, 20, 50, 100, 200, 500, 2000 };
    for(int i=9;i>=0;i--){
        while(N>=coins[i]){
            ans.push_back(coins[i]);
            N-=coins[i];
        }
    }
    return ans;
}

Link - https://practice.geeksforgeeks.org/problems/-minimum-number-of-coins4426/1
```

---

## 26. Minimum Platforms

**Q:** Find the minimum number of railway platforms required to accommodate all trains given their arrival and departure times.

**A:** Sort arrivals and departures independently, then use two pointers to track the number of trains currently at the station.

**Explanation:**
- **Sort by start time (Chronological sweep):** By treating arrivals and departures as independent events, we can simulate time passing.
- An arrival increments the platform count, and a departure decrements it.
- The maximum overlapping count at any point dictates the minimum platforms required.

**Complexity:** Time `O(n log n)`, Space `O(1)` avoiding original arrays' space constraint.

```cpp
int findPlatform(int arr[], int dep[], int n){
    sort(arr,arr+n);
    sort(dep,dep+n);

    int i=1,j=0;
    int ans=1, maxp=1;

    while(i<n && j<n){
        if(arr[i]<=dep[j]){
            maxp++;
            i++;
        }else{ 
            maxp--;
            j++;
        }
        ans=max(ans,maxp);
    }
    return ans;
}

Link - https://practice.geeksforgeeks.org/problems/minimum-platforms-1587115620/1#
```

---

## 27. N meetings in one room

**Q:** Find the maximum number of meetings that can be accommodated in one room given start and end times.

**A:** Pair the times, sort by end time, and greedily pick non-overlapping meetings.

**Explanation:**
- **Sort by end time:** Exactly the same logic as the Activity Selection problem.
- By picking the meeting that ends earliest, we leave maximum time for remaining meetings.
- Overlapping meetings are simply skipped.

**Complexity:** Time `O(n log n)`, Space `O(n)` to store pairs.

```cpp
static bool compare(pair<int,int> &p1,pair<int,int> &p2){
    return p1.second<p2.second;
}

int maxMeetings(int start[], int end[], int n){
    vector<pair<int,int>> meet;
    for(int i=0;i<n;i++){
        meet.push_back({start[i],end[i]});
    }
    sort(meet.begin(),meet.end(),compare);
    int ans=1;
    int endTime=meet[0].second;
    for(int i=1;i<n;i++){
        if(meet[i].first>endTime){
            ans++;
            endTime=meet[i].second;
        }
    }
    return ans;
}

Link-https://practice.geeksforgeeks.org/problems/n-meetings-in-one-room-1587115620/1
```

---

## 28. Shop in Candy Store

**Q:** Calculate the minimum and maximum cost to buy all candies, where buying one candy allows you to take `k` candies for free.

**A:** Sort the candy prices; for minimum cost, buy cheapest and take most expensive for free; for maximum cost, buy most expensive and take cheapest.

**Explanation:**
- **Sort by cost:** Sorting aligns items such that the two-pointer extremes represent best/worst case scenarios.
- For minimum cost, we increment the left pointer (buying) and rapidly decrement the right pointer (freebies).
- For maximum cost, we do the inverse.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
vector<int> minMaxCandy(vector<int>& prices, int k) {
    sort(prices.begin(),prices.end());
    int n=prices.size();
    if(k==0){
        int sum=accumulate(prices.begin(),prices.end(),0);
        return {sum,sum};
    }
    int mn=0,mx=0;
    int i=0,j=n-1;
    while(i<=j){
        mn+=prices[i++];
        j-=k;
    }
    i=0,j=n-1;
    while(i<=j){
        mx+=prices[j--];
        i+=k;
    }
    return {mn,mx};
}

gfg link - https://www.geeksforgeeks.org/problems/shop-in-candy-store1145/1
```

---

## 29. The Celebrity Problem

**Q:** Find the celebrity in a party of `n` people (celebrity knows nobody, but everyone knows the celebrity) using an `n x n` matrix.

**A:** Use a two-pointer or elimination approach to find a candidate, then verify if the candidate truly meets the criteria.

**Explanation:**
- **Elimination scan:** If `A` knows `B`, `A` cannot be the celebrity. If `A` doesn't know `B`, `B` cannot be the celebrity. Each check eliminates one person.
- The greedy choice is evaluating the relationship `mat[cand][i]` to constantly funnel down to a single candidate.
- A final linear pass verifies if the candidate is universally known and knows no one.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
int celebrity(vector<vector<int>>& mat) {
    int n=mat.size();
    int cand=0;
    for(int i=1;i<n;i++){
        if(mat[cand][i]) cand=i;
    }
    for(int j=0;j<n;j++){
        if(j==cand) continue;
        if(mat[cand][j] || !mat[j][cand]) return -1;
    }
    return cand;
}

gfg link - https://www.geeksforgeeks.org/problems/the-celebrity-problem/1
```

---

## 30. Water the plants

**Q:** Find the minimum number of sprinklers to turn on to water a gallery of plants, given coverage radii.

**A:** Convert each sprinkler into an `[start, end]` coverage interval, then eagerly jump to the furthest coverage possible.

**Explanation:**
- **Precompute intervals (Jump Game variant):** The `plant` array tracks the maximum rightward reach for any sprinkler covering a specific index `i`.
- The greedy choice is that at index `i`, we jump to the absolute maximum reach covering `i`.
- If an index has no coverage (`plant[i] == -1`), it's impossible.

**Complexity:** Time `O(n)`, Space `O(n)` for the reach array.

```cpp
int min_sprinklers(int gallery[], int n){
    vector<int> plant(n,-1);
    for(int i = 0;i<n;i++){
        if(gallery[i] != -1){
            int a = max(0,i-gallery[i]);
            int b = min(n-1,i+gallery[i]);
            for(int j = a;j<=b;j++){
                plant[j] = max(plant[j],b);
            }
        }
    }
    int ans = 0;
    for(int i=0;i<n;i++){
        if(plant[i] == -1){
            return -1;
        }
        if(i<=plant[i]){
            ans++;
            i = plant[i];
        }
    }
    return ans;
}

gfg links - https://www.geeksforgeeks.org/problems/water-the-plants--170646/1
```
