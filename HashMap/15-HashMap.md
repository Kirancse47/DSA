# Hashing (map / set) — Q&A

> Source: `Kirancse47/DSA` -> `HashMap/` — 21 problems

Ordering: Conceptual complexity, progressing from frequency counts to custom comparators.

## Cheat sheet

- **`std::map` vs `std::unordered_map`**:
  - `std::unordered_map` (Hash Table): `O(1)` average lookup/insertion, `O(n)` worst case. Does not maintain order. Ideal for pure frequencies or lookups.
  - `std::map` (Red-Black Tree): `O(log n)` lookup/insertion. Maintains keys in sorted order. Use when key ordering is required or when hashing complex keys (like `std::pair`) is not supported natively.

- **Frequency Counting**: Tally occurrences to detect duplicates, isolate elements, or apply combinatorics.
  - *Examples*: 1347. Minimum Number of Steps to Make Two Strings Anagram, 2870. Minimum Number of Operations to Make Array Empty, 645. Set Mismatch.
- **Complement Lookup**: Check historical state (the map) for a calculated target (e.g., a multiple or difference) in a single pass.
  - *Examples*: 1346. Check If N and Its Double Exist.
- **Grouping by Canonical Key (Signature)**: Map a computed property (like a sorted pair or digit sum) to aggregate or compare colliding values.
  - *Examples*: 1128. Number of Equivalent Domino Pairs, 2342. Max Sum of a Pair With Equal Sum of Digits.
- **Dual Map / Bijection Tracking**: Ensure strict 1:1 mapping between two distinct sets of entities by tracking relations in both directions.
  - *Examples*: 205. Isomorphic Strings, 3160. Find the Number of Distinct Colors Among the Balls.
- **Index and Coordinate Mapping**: Store the exact structural location of a value for `O(1)` positional retrieval.
  - *Examples*: 2661. First Completely Painted Row or Column, 1930. Unique Length-3 Palindromic Subsequences.

---

## 1. 1128. Number of Equivalent Domino Pairs

**Q:** Given a list of dominoes represented as pairs of integers, return the number of pairs `(i, j)` where `i < j` and the dominoes are equivalent (matching either normally or flipped).

**A:** Canonical key frequency counting.

**Explanation:**
- The map's KEY is a canonicalized `std::pair<int, int>` representing a domino (sorted so the smaller value is first).
- The VALUE is the frequency of that specific domino configuration seen so far.
- When a domino is processed, the current value in the map is added to the total pairs before incrementing its frequency.
- The answer accumulates the pairing counts dynamically as occurrences increase.

**Complexity:** Time: `O(n log n)` because `std::map` operations take logarithmic time. Space: `O(n)` maximum distinct domino configurations.

```cpp
    int numEquivDominoPairs(vector<vector<int>>& dom) {
        int ans=0;
        map<pair<int,int>,int> mp;
        for(auto it : dom){
            int a=it[0],b=it[1];
            if(a>b) swap(a,b);
            ans+=mp[{a,b}];
            mp[{a,b}]++;
        }
        return ans;
    }
```

---

## 2. 1346. Check If N and Its Double Exist

**Q:** Given an array of integers, return true if there exist two indices `i` and `j` such that `arr[i] == 2 * arr[j]`.

**A:** Complement lookup using a hash map.

**Explanation:**
- The map's KEY is the array element.
- The VALUE is the array index where the element was encountered.
- During iteration, the map is populated while simultaneously checking if half the current element (if even) or twice the current element already exists.
- The answer evaluates to true immediately upon finding a valid complement pair.

**Complexity:** Time: `O(n)` average due to `std::unordered_map` lookups. Space: `O(n)` distinct keys stored in the map.

```cpp
    bool checkIfExist(vector<int>& arr) {
        int n=arr.size();
        unordered_map<int,int> mp;
        for(int i=0;i<n;i++){
            if(arr[i]%2==0 && mp.find(arr[i]/2)!=mp.end() && i!=mp[arr[i]/2]) return true;
            if(mp.find(arr[i]*2)!=mp.end() && i!=mp[arr[i]*2]) return true;
            mp[arr[i]]=i;
        }
        return false;
    }
```

---

## 3. 1347. Minimum Number of Steps to Make Two Strings Anagram

**Q:** Given two equal-length strings `s` and `t`, return the minimum number of character replacements needed to make `t` an anagram of `s`.

**A:** Character frequency differential counting.

**Explanation:**
- The map's KEY is a character.
- The VALUE is the net frequency difference between string `t` and string `s`.
- The map increments for characters in `t` and decrements for characters in `s`.
- The answer is extracted by summing the absolute values of all net differences and dividing by 2 (since each replacement resolves one surplus and one deficit).

**Complexity:** Time: `O(n)` average where `n` is the string length. Space: `O(1)` bounded by 26 lowercase English letters.

```cpp
    int minSteps(string s, string t) {
        unordered_map<char,int> mp;
        for(auto it : t) mp[it]++;
        for(auto it : s) mp[it]--;
        int ans=0;
        for(auto it :mp) ans+=abs(it.second);
        return ans/2;
    }
```

---

## 4. 1930. Unique Length-3 Palindromic Subsequences

**Q:** Given a string, return the number of unique palindromic subsequences of length 3 that can be formed.

**A:** Index boundary tracking and interior distinct character counting.

**Explanation:**
- The primary map's KEY is a character.
- The primary map's VALUE is a vector of all indices where that character appears.
- For each character, its first and last occurrence indices define the outer bounds of potential palindromes.
- A secondary map tracks distinct characters between these bounds to count unique middle characters.

**Complexity:** Time: `O(n log c)` per character where `c` is the unique character count due to `std::map` overhead. Space: `O(n)` total elements across index vectors.

```cpp
    int countPalindromicSubsequence(string s) {
        map<char, vector<int>> mp;
        for(int i=0; i<s.size(); i++){
            mp[s[i]].push_back(i);
        }
        int ans=0;
        for(auto it: mp){
            vector<int> ind = it.second;
            int st = ind[0];
            int e = ind[ind.size()-1];
            if(e-st<=1){
                continue;
            }
            else{
                map<char, int> mpp;
                for(int i=st+1; i<e; i++){
                    mpp[s[i]]++;
                }
                ans+=mpp.size();
            }
        }
        return ans;
    }
```

---

## 5. 205. Isomorphic Strings

**Q:** Given two strings `s` and `t`, return true if characters in `s` can be consistently replaced to get `t` while maintaining a one-to-one mapping.

**A:** Dual hash map bijection tracking.

**Explanation:**
- The first map's KEY is a character from `s`, and its VALUE is the assigned mapped character in `t`.
- The second map enforces the reverse mapping (KEY from `t`, VALUE from `s`).
- During iteration, if a mapping already exists and conflicts with the current character pairing, the strings are not isomorphic.
- The maps are updated sequentially as new valid character pairs are encountered.

**Complexity:** Time: `O(n)` average via `unordered_map` lookups. Space: `O(1)` maximum 256 distinct ASCII keys per map.

```cpp
    bool isIsomorphic(string s, string t) {
        int n=s.size();
        unordered_map<char,char> mps,mpt;
        for(int i=0;i<n;i++){
            if(mps.find(s[i])!=mps.end()){
                if(mps[s[i]]!=t[i]) return false;
            }else mps[s[i]]=t[i];
            if(mpt.find(t[i])!=mpt.end()){
                if(mpt[t[i]]!=s[i]) return false;
            }else mpt[t[i]]=s[i];
        }
        return true;
    }
```

---

## 6. 2225. Find Players With Zero or One Losses

**Q:** Given a list of match results (winner, loser), return a sorted list of players who have not lost any matches and a sorted list of players who have lost exactly one match.

**A:** Loss frequency mapping and set-based uniqueness tracking.

**Explanation:**
- The map's KEY is the player ID, and the VALUE is the total number of losses they suffered.
- An ordered `std::set` tracks all unique player IDs (both winners and losers) as they are parsed.
- The answer is extracted by iterating through the naturally sorted set and checking the map to bucket players with zero or one loss.

**Complexity:** Time: `O(n log p)` where `p` is the number of unique players due to `std::set` insertion. Space: `O(p)` distinct players tracked across the set and map.

```cpp
    vector<vector<int>> findWinners(vector<vector<int>>& matches) {
        int n=matches.size();
        vector<vector<int>> ans(2);
        unordered_map<int,int> mp;
        set<int> st;
        for(int i=0;i<n;i++){
            st.insert(matches[i][0]);
            st.insert(matches[i][1]);
            mp[matches[i][1]]++;
        }
        for(auto it : st){
            if(mp.find(it)==mp.end()) ans[0].push_back(it);
            else if(mp[it]==1) ans[1].push_back(it);
        }
        return ans;
    }
```

---

## 7. 2342. Max Sum of a Pair With Equal Sum of Digits

**Q:** Given an array of numbers, return the maximum sum of two numbers that share the same sum of digits.

**A:** Grouping by computed signature with maximum tracking.

**Explanation:**
- The map's KEY is the computed sum of a number's digits.
- The VALUE is the maximum array element seen so far that yields that specific digit sum.
- When an element is processed with an existing digit sum, the global maximum pair sum is updated, and the map value is replaced by the maximum of the colliding element and the stored value.

**Complexity:** Time: `O(n)` average for `unordered_map` paired with bounded digit calculation. Space: `O(k)` distinct digit sums (typically small, max ~81 for standard integers).

```cpp
    int sumDigit(int no){
        int sum=0;
        while(no){
            sum+=no%10;
            no/=10;
        }
        return sum;
    }
    int maximumSum(vector<int>& nums) {
        unordered_map<int,int> mp;
        int ans=-1;
        for(auto it : nums){
            int sdig=sumDigit(it);
            if(mp.find(sdig)!=mp.end()){
                int pval=mp[sdig];
                ans=max(ans,it+pval);
                mp[sdig]=max(pval,it);
            }else mp[sdig]=it;
        }
        return ans;
    }
```

---

## 8. 2353. Design a Food Rating System

**Q:** Design a system that tracks the ratings of food items and can efficiently return the highest-rated food for a given cuisine, breaking ties lexicographically.

**A:** Multi-map cross-referencing with custom-sorted ordered sets.

**Explanation:**
- The `cnsFood` map's KEY is a cuisine string, and the VALUE is a custom-ordered `std::set` of `(rating, food)` pairs.
- The `foodCns` and `foodRating` maps provide rapid `O(1)` lookups to determine a food's cuisine and current rating for updates.
- When a rating changes, the old pair is erased from the ordered set and the new one is inserted, guaranteeing the highest-rated item remains at `begin()`.

**Complexity:** Time: `O(log k)` for `changeRating` and `O(1)` for `highestRated`, where `k` is the number of items per cuisine. Space: `O(n)` total items stored across strings and sets.

```cpp
class FoodRatings {
    struct cmp {
        bool operator()(const pair<int,string>& a, const pair<int,string>& b) const {
            if (a.first != b.first) return a.first > b.first; // higher rating first
            return a.second < b.second; // lexicographically smaller food first
        }
    };
    unordered_map<string, set<pair<int,string>, cmp>> cnsFood;
    unordered_map<string, string> foodCns;
    unordered_map<string, int> foodRating;
public:
    FoodRatings(vector<string>& foods, vector<string>& cuisines, vector<int>& ratings) {
        int n = foods.size();
        for (int i = 0; i < n; i++) {
            string food = foods[i];
            string cns = cuisines[i];
            int rating = ratings[i];
            cnsFood[cns].insert({rating, food});
            foodCns[food] = cns;
            foodRating[food] = rating;
        }
    }
    void changeRating(string food, int newRating) {
        string cns = foodCns[food];
        int oldRating = foodRating[food];
        // erase old entry
        cnsFood[cns].erase({oldRating, food});
        // insert new entry
        cnsFood[cns].insert({newRating, food});
        foodRating[food] = newRating;
    }
    string highestRated(string cuisine) {
        if(cnsFood.find(cuisine)==cnsFood.end()) return "";
        return cnsFood[cuisine].begin()->second; // best food is always at begin    
    }
};
```

---

## 9. 2610. Convert an Array Into a 2D Array With Conditions

**Q:** Given an array, partition it into the minimum number of rows such that no row contains duplicate integers.

**A:** Frequency counting to determine row placement.

**Explanation:**
- The map's KEY is the array element.
- The VALUE is its frequency count across the array.
- The maximum frequency dictates the total number of rows required in the 2D array.
- The answer is built by placing each element into consecutive rows up to its frequency count.

**Complexity:** Time: `O(n)` average using `unordered_map`. Space: `O(n)` distinct elements.

```cpp
    vector<vector<int>> findMatrix(vector<int>& nums) {
        
        unordered_map<int,int> mp;
        int mx=0;
        for(auto it : nums){
            mp[it]++;
            mx=max(mx,mp[it]);
        }
        vector<vector<int>> ans(mx);
        for(auto it : mp){
            for(int i=0;i<it.second;i++){
                ans[i].push_back(it.first);
            }
        }
        return ans;
    }
```

---

## 10. 2661. First Completely Painted Row or Column

**Q:** Given a matrix and an array dictating the order in which matrix cells are painted, return the array index at which a row or column becomes completely painted.

**A:** Value-to-coordinate mapping with row/column completion tallies.

**Explanation:**
- A direct mapping array `mp` acts as the map: KEY is the matrix value, VALUE is its `(row, col)` coordinate pair.
- Two separate tally arrays track how many cells have been painted in each row and each column.
- Elements are updated based on the drawing array; the first operation to increment a row tally to `n` or a column tally to `m` triggers the return.

**Complexity:** Time: `O(m * n)` to index the matrix and process the order sequence. Space: `O(m * n)` for the direct coordinate lookup array and dimension tallies.

```cpp
    int firstCompleteIndex(vector<int>& arr, vector<vector<int>>& mat) {
        int m=mat.size(),n=mat[0].size();
        vector<pair<int,int>> mp(m*n+1);
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                mp[mat[i][j]]={i,j};
            }
        }
        vector<int> row(m,0),col(n,0);
        for(int k=0;k<arr.size();k++){
            auto it=mp[arr[k]];
            row[it.first]++;
            col[it.second]++;
            if(row[it.first]==n || col[it.second]==m) return k;
        }
        return -1;
    }
```

---

## 11. 2870. Minimum Number of Operations to Make Array Empty

**Q:** Given an array, find the minimum number of operations to empty it by deleting pairs or triplets of equal elements, or return -1 if impossible.

**A:** Frequency counting and greedy divisibility math.

**Explanation:**
- The map's KEY is the array element.
- The VALUE is its total occurrence count.
- If any element maps to a frequency of exactly 1, the array cannot be emptied (returns -1).
- The minimal operations are calculated greedily via integer division by 3, adding 1 for any remainder since `t % 3 == 1` or `t % 3 == 2` both resolve optimally with one additional pair.

**Complexity:** Time: `O(n)` average via `unordered_map`. Space: `O(n)` distinct elements.

```cpp
    int minOperations(vector<int>& nums) {
        int ans=0;
        unordered_map<int,int> mp;
        for(auto it :  nums) mp[it]++;

        for(auto it : mp){
            int t=it.second;
            if(t==1) return -1;
            if(t%3==0) ans+=t/3;
            else ans+=(t/3+1);
        }
        return ans;
    }
```

---

## 12. 3160. Find the Number of Distinct Colors Among the Balls

**Q:** Given a sequence of queries painting balls with specific colors, return the number of distinct colors present after each query.

**A:** Dual-map tracking for ball-to-color and color-frequency.

**Explanation:**
- The `ball` map's KEY is the ball ID, and the VALUE is its currently assigned color.
- The `col` map's KEY is a color, and the VALUE is the count of balls painted that color.
- When a ball is painted, its old color count is decremented (and erased if zero), the new color is tallied, and the total distinct colors are extracted via `col.size()`.

**Complexity:** Time: `O(q)` average for `q` queries via `unordered_map`. Space: `O(q)` distinct balls and colors introduced across queries.

```cpp
    vector<int> queryResults(int limit, vector<vector<int>>& que) {
        unordered_map<int,int> ball,col;
        vector<int> ans;
        for(auto it : que){
            col[it[1]]++;
            if(ball.find(it[0])!=ball.end()){
                int preCol=ball[it[0]];
                col[preCol]--;
                if(col[preCol]==0) col.erase(preCol);
            }
            ball[it[0]]=it[1];
            ans.push_back(col.size());
        }
        return ans;
    }
```

---

## 13. 506. Relative Ranks

**Q:** Given an array of athlete scores, return their ranks formatted as specific medals for the top three and numeric strings for the rest.

**A:** Priority queue (max heap) sorting by score.

**Explanation:**
- A max heap serves as an ordered map where the KEY is the score and the VALUE is the original array index.
- Elements are popped continuously in descending order to assign rankings.
- The cached original index maps the formatted medal/string value directly into the corresponding position of the result vector.

**Complexity:** Time: `O(n log n)` for priority queue insertions. Space: `O(n)` for the heap and answer array.

```cpp
    vector<string> findRelativeRanks(vector<int>& score) {
        int n=score.size();
        vector<string> ans(n);
        priority_queue<pair<int,int>>pq;
        for(int i=0;i<n;i++) pq.push({score[i],i});
        int rank=1;
        while(!pq.empty()){
            auto it = pq.top();pq.pop();
            if(rank==1) ans[it.second]="Gold Medal";
            else if(rank==2) ans[it.second]="Silver Medal";
            else if(rank==3) ans[it.second]="Bronze Medal";
            else ans[it.second]=to_string(rank);
            rank++;
        }
        return ans;
    }
```

---

## 14. 645. Set Mismatch

**Q:** Given an array of size `n` containing numbers from `1` to `n` where exactly one number is duplicated and one is missing, return both numbers.

**A:** Frequency counting to identify the double and the zero.

**Explanation:**
- The map's KEY is the number from the array.
- The VALUE is the frequency count of that number.
- After populating the map, a secondary loop checks numbers `1` through `n`.
- The answer is found by locating the key missing from the map and the key with a frequency of 2.

**Complexity:** Time: `O(n)` average using `unordered_map`. Space: `O(n)` distinct valid keys.

```cpp
    vector<int> findErrorNums(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans(2);
        unordered_map<int,int> mp;
        for(auto it : nums) mp[it]++;
        for(int i=1;i<=n;i++){
            if(mp.find(i)==mp.end()) ans[1]=i;
            else if(mp[i]==2) ans[0]=i;
        }
        return ans;
    }
```

---

## 15. 706. Design HashMap

**Q:** Implement a custom Hash Map class without using built-in hash table libraries, supporting `put`, `get`, and `remove` functionality.

**A:** Separate chaining with linked lists.

**Explanation:**
- The underlying bucket array is a vector of linked list `node*` pointers.
- The bucket index is computed manually using a modulo hash function `key % size`.
- Key-value pairs are updated, retrieved, or deleted by traversing the targeted linked list, checking for key equivalence along the nodes.

**Complexity:** Time: `O(1)` average, `O(n/k)` worst-case per operation based on load factor. Space: `O(m)` nodes where `m` is inserted elements, plus the fixed bucket array.

```cpp
struct node{
    int key;
    int val;
    node* next;
    node(int k,int v){
        key=k;
        val=v;
        next=NULL;
    }
};

class MyHashMap {
    vector<node*> mp;
    static const int size=1009;
public:
    MyHashMap() {
        mp.resize(size);
        for(int i=0;i<size;i++){
            mp[i]=new node(-1,-1);
        }
    }
    
    void put(int key, int value) {
        if(key==-1) return;
        int hash=key%size;
        node *ptr=mp[hash];
        while(ptr->next){
            if(ptr->next->key==key) {
                ptr->next->val=value;
                return;
            }
            ptr=ptr->next;
        }
        ptr->next=new node(key,value);
    }
    
    int get(int key) {
        int hash=key%size;
        node *ptr=mp[hash]->next;
        while(ptr){
            if(ptr->key==key) return ptr->val;
            ptr=ptr->next;
        }
        return -1;
    }
    
    void remove(int key) {
        int hash=key%size;
        node *ptr=mp[hash];
        while(ptr && ptr->next && ptr->next->key!=key){
            ptr=ptr->next;
        }
        if(ptr->next){
            node *tmp=ptr->next;
            ptr->next=ptr->next->next;
            delete tmp;
        }
    }
};
```

---

## 16. 786. K-th Smallest Prime Fraction

**Q:** Given a sorted array of prime numbers, find the `k`-th smallest fraction that can be formed by picking two numbers.

**A:** Min heap over all possible pairwise combinations.

**Explanation:**
- The min heap stores pairs where the KEY is the computed fraction `(double)arr[i]/arr[j]`.
- The VALUE is the nested tuple pair of prime numbers used to build that fraction.
- The entire Cartesian product of fractions is pushed into the heap (`greater<>`), ordering smallest to largest.
- The answer is found by popping the heap `k-1` times and extracting the components of the top element.

**Complexity:** Time: `O(n^2 log(n^2))` since all valid fraction pairs are pushed into the heap. Space: `O(n^2)` total combinations held simultaneously in memory.

```cpp
    vector<int> kthSmallestPrimeFraction(vector<int>& arr, int k) {
        priority_queue<pair<double, pair<int, int>>, vector<pair<double, pair<int, int>>>, greater<>> minHeap;
        for(int i=0;i<arr.size();i++){
            for(int j=i+1;j<arr.size();j++){
                minHeap.push({(double)arr[i]/arr[j],{arr[i],arr[j]}});
            }
        }
        k--;
        while(k--) minHeap.pop();
        auto ans=minHeap.top();
        return {ans.second.first,ans.second.second};
    }
```

---

## 17. 791. Custom Sort String

**Q:** Given an `order` string specifying a custom character sequence and a string `s`, sort `s` so its characters match the custom order.

**A:** Character frequency mapping and sequential reconstruction.

**Explanation:**
- The map's KEY is a character present in `s`.
- The VALUE is its raw frequency count.
- The result is built by scanning the `order` string and appending each letter as many times as dictated by the frequency map.
- Unsorted leftover characters map values are consumed in a final sweep to complete the string reconstruction.

**Complexity:** Time: `O(n + m)` average where `n` is `s.length` and `m` is `order.length`. Space: `O(1)` restricted strictly to lowercase English letters.

```cpp
string customSortString(string order, string s) {
    unordered_map<char,int> mp;
    for(auto it : s) mp[it]++;
    string ans="";
    for(auto it : order){
        while(mp[it]--) ans.push_back(it);
    }
    for(auto it : mp){
        for(int i=0;i<it.second;i++) ans.push_back(it.first);
    }
    return ans;
}
```

---

## 18. 997. Find the Town Judge

**Q:** Given a number of people `n` and a list of `trust` relationships, identify the town judge who trusts nobody and is trusted by everyone else.

**A:** In-degree and out-degree graph vertex counting.

**Explanation:**
- The `mp1` map's KEY is the trusted person, and its VALUE is the total trust votes received (in-degree).
- The `mp2` map's KEY is the trusting person, tracking total votes cast (out-degree).
- The town judge is confirmed by verifying an in-degree equivalent to `n - 1` with a zero out-degree check.

**Complexity:** Time: `O(t + n)` average utilizing `unordered_map` across `t` trust edges. Space: `O(n)` to record relationships per individual.

```cpp
    int findJudge(int n, vector<vector<int>>& trust) {
        unordered_map<int,int> mp1,mp2;
        for(auto it : trust){
            mp1[it[1]]++;
            mp2[it[0]]++;
        }
        for(auto it : mp1){
            if(it.second==n-1 && mp2[it.first]==0) return it.first;
        }
        if(n==1) return 1;
        return -1;
    }
```

---

## 19. Find duplicate rows in a binary matrix

**Q:** Given a binary matrix, return the zero-based indices of all rows that are exact duplicates of a previously seen row.

**A:** `std::set` lookup for full vector equality.

**Explanation:**
- The set's KEY is an entire `std::vector<int>` row configuration extracted from the matrix.
- `std::set` automatically utilizes vector comparison semantics to determine exact structural equality.
- A row index is pushed directly to the answer vector if its configuration has already been successfully inserted into the set.

**Complexity:** Time: `O(M * N * log M)` worst-case because structural vector comparisons take `O(N)` scaling with an `O(log M)` tree insertion. Space: `O(M * N)` max keys buffered.

```cpp
    vector<int> repeatedRows(vector<vector<int>> &matrix, int M, int N) {
        vector<int> ans;
        set<vector<int>> st;
        for(int i=0;i<M;i++){
            if(st.find(matrix[i])==st.end()) st.insert(matrix[i]);
            else ans.push_back(i);
        }
        return ans;
    }


gfg link - https://www.geeksforgeeks.org/problems/find-duplicate-rows-in-a-binary-matrix/1
```

---

## 20. K closest elements

**Q:** Given an array, find the `k` elements closest to a target value `x`, breaking distance ties by favoring the larger element.

**A:** Max heap with a custom comparator.

**Explanation:**
- The max heap stores pairs where the KEY is the distance `abs(it - x)`, and the VALUE is the array element itself.
- A custom comparator dictates that larger distances (or larger elements on a distance tie) sit at the top of the heap.
- The heap strictly enforces size `k` by popping the worst candidates, enabling extraction of the closest `k` elements at the conclusion.

**Complexity:** Time: `O(n log k)` to continuously push and regulate a heap of size `k`. Space: `O(k)` queue storage limit.

```cpp
    struct cmp{
        bool operator()(pair<int,int> &p1,pair<int,int> &p2){
            if(p1.first<p2.first) return true;
            if(p1.first==p2.first){
                return p1.second>p2.second;
            }
            return false;
        }
    };
  
    vector<int> printKClosest(vector<int> arr, int n, int k, int x) {
        priority_queue< pair<int,int>, vector<pair<int,int>>, cmp> maxh;
        for(auto it : arr) {
            if(it==x) continue;
            maxh.push({abs(it-x),it});
            if(maxh.size()>k) maxh.pop();
        }
        vector<int> ans;
        while(maxh.size()>0){
            ans.push_back(maxh.top().second);
            maxh.pop();
        }
        reverse(ans.begin(),ans.end());
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/k-closest-elements3619/1
```

---

## 21. Substrings with same first and last characters

**Q:** Given a string, count the total number of substrings that begin and end with the exact same character.

**A:** Combinatorics on character frequency counts.

**Explanation:**
- A fixed-size array tracks the frequency count of each lowercase character.
- The map logic exploits that if a character appears `k` times globally, it inherently builds `k * (k + 1) / 2` matching substrings.
- The final answer strictly accumulates combinatorial products across the bounded alphabet limits.

**Complexity:** Time: `O(n)` to build the frequency map across the string. Space: `O(1)` fixed structure scaling maximally to 26 limits.

```cpp
    int countSubstring(string &s) {
        int n=s.size();
        vector<int> freq(26,0);
        for(auto it : s) freq[it-'a']++;
        int ans=0;
        for(int i=0;i<26;i++) ans+=(freq[i]*(freq[i]+1)/2);
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/substrings-with-similar-first-and-last-characters3644/1
```
