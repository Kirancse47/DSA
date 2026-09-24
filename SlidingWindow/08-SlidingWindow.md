# Sliding Window & Two Pointers — Q&A

> Source: `Kirancse47/DSA` -> `SlidingWindow/` — 29 problems

Ordering: Alphabetical by filename

## Cheat sheet

**Fixed-size window:** Used when the window length is exactly $K$. The window slides by one element at a time, adding the new right element and removing the old left element.
```cpp
int i = 0, j = 0;
while (j < n) {
    // Add arr[j] to window state
    if (j - i + 1 < K) {
        j++;
    } else if (j - i + 1 == K) {
        // Record answer for current window
        // Remove arr[i] from window state
        i++; j++;
    }
}
```
*Examples:* Max Sum Subarray of size K, First negative integer in every window of size k

**Variable-size window:** Used when expanding right as long as valid, and shrinking left while invalid.
```cpp
int i = 0, j = 0;
while (j < n) {
    // Add arr[j] to window state
    while (/* window is invalid */) {
        // Remove arr[i] from window state
        i++;
    }
    // Window is valid here, record answer
    j++;
}
```
*Examples:* Minimum Size Subarray Sum, Longest Substring Without Repeating Characters

**At most K minus at most K-1 trick:** Used when counting substrings with EXACTLY $K$ elements satisfying a condition. `exact(K) = atMost(K) - atMost(K-1)`.
```cpp
long long exactK(string s, int k) {
    return atMost(s, k) - atMost(s, k - 1);
}
```
*Examples:* Subarrays with K Different Integers, Count of Substrings Containing Every Vowel and K Consonants II

**Monotonic Deque:** Used to maintain the maximum or minimum in a sliding window efficiently.
```cpp
deque<int> dq;
for(int j = 0; j < n; j++) {
    while(!dq.empty() && dq.back() < arr[j]) dq.pop_back();
    dq.push_back(arr[j]);
    // Process window and slide left...
}
```
*Examples:* Sliding Window Maximum

---

## 1. 1208. Get Equal Substrings Within Budget

**Q:** Given two strings `s` and `t` and a budget `maxCost`, find the maximum length of a substring where the sum of absolute differences between corresponding characters is at most `maxCost`.

**A:** Variable-size window, shrinking from the left when the running cost exceeds `maxCost`.

**Explanation:**
- A window is valid as long as the running cost is less than or equal to `maxCost`.
- When `right` advances, the absolute difference `abs(s[end] - t[end])` is added to the running cost.
- When `left` advances, the difference `abs(s[start] - t[start])` is subtracted from the cost.
- The maximum length is recorded at every step where the condition holds.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int equalSubstring(string s, string t, int maxCost) {
        int n = s.size();
        int start = 0;
        int current_cost = 0;
        int max_length = 0;
        for (int end = 0; end < n; ++end) {
            current_cost += abs(s[end] - t[end]);
            while (current_cost > maxCost) {
                current_cost -= abs(s[start] - t[start]);
                ++start;
            }
            max_length = max(max_length, end - start + 1);
        }
        return max_length;
    }
```

---

## 2. 1358. Number of Substrings Containing All Three Characters

**Q:** Given a string `s` consisting only of 'a', 'b', and 'c', return the number of substrings containing at least one occurrence of all these characters.

**A:** Variable-size window using a frequency map, shrinking from the left while the window contains all three characters.

**Explanation:**
- The window is valid when all three characters ('a', 'b', 'c') are present in the map.
- When `right` advances, the character at `s[right]` is added to the frequency map.
- When the window has exactly 3 distinct characters, every substring ending at `right` or later and starting at `left` is valid. Thus, `n - j` is added to the answer, and `left` advances.
- The character at `s[left]` is removed from the frequency map to search for other valid substrings.

**Complexity:** Time `O(n)`, Space `O(1)` (since the map size is at most 3).

```cpp
    int numberOfSubstrings(string s) {
        int n=s.size(),i=0,j=0;
        int ans=0;
        unordered_map<char,int> mp;
        while(j<n){
            mp[s[j]]++;
            while(mp.size()==3){
                ans+=n-j;
                mp[s[i]]--;
                if(mp[s[i]]==0) mp.erase(s[i]);
                i++;
            }
            j++;
        }
        return ans;
    }
```

---

## 3. 1423. Maximum Points You Can Obtain from Cards

**Q:** Given an array `cardPoints` and an integer `k`, find the maximum score you can obtain by taking exactly `k` cards from either the beginning or the end of the array.

**A:** Fixed-size window conceptually applied to the end of the array to simulate taking cards from both ends.

**Explanation:**
- Instead of tracking from both ends dynamically, initialize the sum with the first `k` elements.
- Iteratively replace one element from the right end of the selected prefix with one element from the right end of the array.
- In each step, subtract `cardPoints[k - 1 - i]` and add `cardPoints[n - 1 - i]`.
- Keep track of the maximum points recorded during these replacements.

**Complexity:** Time `O(k)`, Space `O(1)`.

```cpp
    int maxScore(vector<int>& cardPoints, int k) {
        int n= cardPoints.size(), points=0;
        for(int i=0;i<k;i++) points+=cardPoints[i];
        int ans=points;
        for(int i=0;i<k;i++){
            points=points-cardPoints[k-1-i]+cardPoints[n-1-i];
            ans=max(ans,points);
        }
        return ans;
    }
```

---

## 4. 1695. Maximum Erasure Value

**Q:** Given an array of positive integers `nums`, find the maximum sum of a subarray containing unique elements.

**A:** Variable-size window with a frequency map, shrinking while the window contains duplicate elements.

**Explanation:**
- A window is valid when the number of unique elements (map size) equals the length of the window (`j - i + 1`).
- When `right` advances, the value is added to the running sum and the frequency map.
- If duplicates exist, `left` advances, and elements are subtracted from the sum and map until the window only has unique elements.
- The maximum running sum is updated whenever the window becomes valid.

**Complexity:** Time `O(n)`, Space `O(n)` (for the frequency map).

```cpp
   int maximumUniqueSubarray(vector<int>& nums) {
        int n=nums.size();
        unordered_map<int,int> mp;
        int i=0,j=0,sum=0,ans=0;
        while(j<n){
            sum+=nums[j];
            mp[nums[j]]++;
            if(mp.size()==j-i+1) ans=max(ans,sum);
            else{
                while(mp.size()<j-i+1){
                    sum-=nums[i];
                    mp[nums[i]]--;
                    if(mp[nums[i]]==0) mp.erase(nums[i]);
                    i++;
                }
                if(mp.size()==j-i+1) ans=max(ans,sum);
            }
            j++;
        }
        return ans;
    }
```

---

## 5. 209. Minimum Size Subarray Sum

**Q:** Given an array of positive integers `nums` and a positive integer `target`, return the minimal length of a subarray whose sum is greater than or equal to `target`.

**A:** Variable-size window, shrinking from the left as long as the running sum remains greater than or equal to `target`.

**Explanation:**
- The window is valid as long as `sum >= target`.
- When `right` advances, `nums[right]` is added to the running sum.
- When the condition is met, the minimal length is recorded, and `left` advances while subtracting `nums[left]` to find the shortest valid subarray.
- The minimum window length is returned at the end.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
int minSubArrayLen(int target, vector<int>& nums) {
    int n=nums.size(),ans=INT_MAX;
    int i=0,j=0;
    long long int sum=0;
    while(j<n){
        sum=sum+nums[j];
        if(sum>=target){
            while(sum>=target){
                ans=min(ans,j-i+1);
                sum=sum-nums[i++];
            }
        }
        j++;
    }
    if(ans==INT_MAX) return 0;
    return ans;
}
```

---

## 6. 2379. Minimum Recolors to Get K Consecutive Black Blocks

**Q:** Given a string `blocks` of 'W' and 'B' and an integer `k`, find the minimum number of operations (changing 'W' to 'B') needed to get `k` consecutive 'B' blocks.

**A:** Fixed-size window of length `k`, maintaining the count of 'B' characters.

**Explanation:**
- The window length is strictly `k`.
- When `right` advances, increment the count of 'B's if the current character is 'B'.
- When the window reaches size `k`, record the minimum of `ans` and `k - B` (which represents the 'W's to recolor).
- When `left` advances to slide the window, decrement the 'B' count if the outgoing character was 'B'.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int minimumRecolors(string blocks, int k) {
        int n=blocks.size();
        int i=0,j=0;
        int B=0;
        int ans=k;
        while(j<n){
            if(blocks[j]=='B') B++;
            if(j-i+1<k) j++;
            else{
                ans=min(ans,(k-B));
                if(blocks[i]=='B') B--;
                i++;j++;
            }
        }
        return ans;
    }
```

---

## 7. 239. Sliding Window Maximum

**Q:** Given an integer array `nums` and a sliding window of size `k`, return the maximum element in the window at each step.

**A:** Fixed-size window with a monotonic decreasing deque to track the maximums.

**Explanation:**
- The monotonic deque stores elements in decreasing order, so the front always holds the maximum for the current window.
- When `right` advances, elements smaller than `nums[right]` are popped from the back of the deque, then `nums[right]` is added.
- When the window size reaches `k`, the front of the deque is recorded into the answer.
- When `left` advances, if the element falling out of the window is equal to the front of the deque, it is popped from the front.

**Complexity:** Time `O(n)`, Space `O(k)`.

```cpp
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        int n=nums.size();
        vector<int> ans;
        deque<int> dq;
        int i=0,j=0;
        while(j<n){
            while(dq.size()>0 && dq.back()<nums[j]) dq.pop_back();
            dq.push_back(nums[j]);
            if(j-i+1<k) j++;
            else if(j-i+1==k){
                ans.push_back(dq.front());
                if(nums[i]==dq.front()) dq.pop_front();
                i++;j++;                
            }
        }
        return ans;
    }
```

---

## 8. 2444. Count Subarrays With Fixed Bounds

**Q:** Given an integer array `nums` and two integers `minK` and `maxK`, return the number of fixed-bound subarrays where the minimum is `minK` and maximum is `maxK`.

**A:** Three-pointer tracking: last seen `minK`, last seen `maxK`, and the most recent invalid element.

**Explanation:**
- The window cannot contain any element strictly outside `[minK, maxK]`. The variable `k` tracks the most recent invalid element.
- When valid elements are scanned, `i` tracks the most recent `minK` and `j` tracks the most recent `maxK`.
- Any subarray ending at the current index and starting anywhere between `k + 1` and `min(i, j)` is valid.
- The total valid subarrays are accumulated by adding `max(0, min(i, j) - k)`.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
long long countSubarrays(vector<int>& nums, int minK, int maxK) {
    int n=nums.size(),i=-1,j=-1,k=-1;
    long long ans=0;
    for(int idx=0;idx<n;idx++){
        if(!(minK<=nums[idx] && nums[idx]<=maxK)) k=idx;
        if(nums[idx]==minK) i=idx;
        if(nums[idx]==maxK) j=idx;
        ans+=max(0,min(i,j)-k);
    }
    return ans;
}
```

---

## 9. 2958. Length of Longest Subarray With at Most K Frequency

**Q:** Given an integer array `nums` and an integer `k`, return the length of the longest good subarray where the frequency of each element is at most `k`.

**A:** Variable-size window with a frequency map, shrinking from the left when any element's frequency exceeds `k`.

**Explanation:**
- The window is valid as long as the maximum frequency of any element in the current window is `<= k`.
- When `right` advances, the frequency of `nums[right]` is incremented. If it exceeds `k`, the window becomes invalid.
- When `left` advances, `nums[left]` is removed from the frequency map until the frequency of `nums[right]` drops back to `k`.
- The answer records the maximum window length while the window is valid.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    int maxSubarrayLength(vector<int>& nums, int k) {
        int n=nums.size(),i=0,j=0,ans=0;
        unordered_map<int,int> mp;
        int cnt=0;
        while(j<n){
            mp[nums[j]]++;
            cnt=max(cnt,mp[nums[j]]);
            if(cnt<=k){
                ans=max(ans,j-i+1);
            }else{
                while(cnt>k){
                    if(cnt==mp[nums[i]]) cnt--;
                    mp[nums[i]]--;
                    i++;
                }
            }
            j++;
        }
        return ans;
    }
```

---

## 10. 2962. Count Subarrays Where Max Element Appears at Least K Times

**Q:** Given an integer array `nums` and an integer `k`, return the number of subarrays where the maximum element of `nums` appears at least `k` times.

**A:** Variable-size window, growing from right and shrinking from left to count subarrays with enough occurrences of the overall maximum element.

**Explanation:**
- First, the global maximum `mx` of the array is found.
- When `right` advances, if `nums[right] == mx`, the count is incremented.
- The window is valid as long as `cnt >= k`. While valid, all subarrays starting at or before `left` and ending at `right` or later are valid (`n - j` added to the answer).
- When `left` advances, the count is decremented if `nums[left] == mx`.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    long long countSubarrays(vector<int>& nums, int k) {
        int n=nums.size(),cnt=0,i=0,j=0;
        int mx=*max_element(nums.begin(),nums.end());
        long long ans=0;
        while(j<n){
            if(nums[j]==mx) cnt++;
            while(cnt>=k){
                if(nums[i]==mx) cnt--;
                i++;
                ans+=n-j;
            }
            j++;
        }
        return ans;
    }
```

---

## 11. 3208. Alternating Groups II

**Q:** Given a circular array `colors` and an integer `k`, count the number of alternating contiguous groups of length `k`.

**A:** Variable-size window scanning up to `n + k - 1` elements to simulate circularity, maintaining an alternating sequence.

**Explanation:**
- Since the array is circular, the limit is expanded to `n + k - 1`. Modulo indexing is used: `colors[index % n]`.
- The `right` pointer advances as long as the current and previous elements have different colors.
- If the valid alternating segment length `r - l` is at least `k`, it contains `(r - l) - k + 1` valid groups of size `k`.
- The `left` pointer jumps directly to `right` when the alternating pattern breaks, preventing redundant checks.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int numberOfAlternatingGroups(vector<int>& colors, int k) {
        int n=colors.size();
        int l=0,limit=n+k-1;
        int count=0;
        while(l<n){
            int r=l+1;
            while(r<limit && colors[(r-1)%n]!=colors[r%n]) r++;
            if(r-l>=k) count+=(r-l)-k+1;
            l=r;
        }
        return count;
    }
```

---

## 12. 3306. Count of Substrings Containing Every Vowel and K Consonants II

**Q:** Given a string `word` and an integer `k`, return the number of substrings containing all 5 vowels at least once and exactly `k` consonants.

**A:** Variable-size window leveraging the "at most K minus at most K-1" counting trick for exact substring combinations.

**Explanation:**
- The function `atLeastK` computes substrings with all 5 vowels and **at least** `k` consonants.
- When `right` advances, the vowel is added to a frequency map, or the consonant count is incremented.
- While the window contains all 5 vowels and `>= k` consonants, `n - j` valid substrings are added to the answer, and `left` is advanced to shrink.
- The exact answer is extracted by `atLeastK(word, k) - atLeastK(word, k + 1)`.

**Complexity:** Time `O(n)`, Space `O(1)` (vowel frequency map max size is 5).

```cpp
    bool isVowel(char c) {
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
    }

    long long atLeastK(string word,int k){
        unordered_map<char,int> mp;
        int cons=0;
        int i=0,j=0,n=word.size();
        long long ans=0;
        while(j<n){
            if(isVowel(word[j])) mp[word[j]]++;
            else cons++;
            while(mp.size()==5 && cons>=k){
                ans += n - j;
                if(isVowel(word[i])){
                    mp[word[i]]--;
                    if(mp[word[i]]==0) mp.erase(word[i]);
                }else cons--;
                i++;
            }
            j++;
        }
        return ans;
    }

    long long countOfSubstrings(string word, int k) {
        return atLeastK(word, k) - atLeastK(word, k + 1);
    }
```

---

## 13. 3. Longest Substring Without Repeating Characters

**Q:** Given a string `s`, find the length of the longest substring without repeating characters.

**A:** Variable-size window with a frequency map, shrinking from the left when the window contains duplicates.

**Explanation:**
- The window is valid when all characters in it are unique, which means the map size equals the window size `j - i + 1`.
- When `right` advances, the character frequency is incremented.
- If the map size is smaller than the window size, a duplicate exists. `left` advances and decrements frequencies until the map size matches the window size again.
- The maximum length is recorded whenever the window is valid.

**Complexity:** Time `O(n)`, Space `O(n)` (or `O(256)`).

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

## 14. 438. Find All Anagrams in a String

**Q:** Given two strings `s` and `p`, return an array of all the start indices of `p`'s anagrams in `s`.

**A:** Fixed-size window of size `p.size()`, matching target character frequencies using a map and a distinct character count.

**Explanation:**
- The target map records frequencies of `p`, and `count` tracks the number of distinct characters that are yet to be fully matched.
- When `right` advances, if `s[right]` is in the map, its frequency is decremented. If it hits 0, `count` decreases.
- The window length is fixed to `k = p.size()`. If `count == 0` when the window is of size `k`, an anagram is found, and `left` (which is `i`) is recorded.
- When `left` advances, if the exiting character is in the map, its frequency is restored.

**Complexity:** Time `O(n)`, Space `O(1)` (for the lowercase English letters map).

```cpp
    vector<int> findAnagrams(string s, string p) {
        unordered_map<char,int> mp;
        vector<int> ans;
        for(auto it : p) mp[it]++;
        int count=mp.size();
        int j=0,i=0,k=p.size(),n=s.size();
        while(j<n){
            if(mp.find(s[j])!=mp.end()){
                mp[s[j]]--;
                if(mp[s[j]]==0) count--;
            }        

            if((j-i+1)<k) j++;
            else if((j-i+1) == k){
                if(count==0) ans.push_back(i);
                if(mp.find(s[i])!=mp.end()){
                    mp[s[i]]++;
                    if( mp[s[i]]==1) count++;
                }
                i++,j++;
            }
        }
        return ans;
    }
```

---

## 15. 567. Permutation in String

**Q:** Given two strings `s1` and `s2`, return true if `s2` contains a permutation of `s1`, or false otherwise.

**A:** Fixed-size window of size `s1.size()`, comparing frequencies with a tracking count exactly like finding anagrams.

**Explanation:**
- The target map counts character frequencies of `s1`. The `count` variable holds the number of distinct characters required.
- When `right` advances, if `s2[right]` is part of `s1`, its frequency is decremented. If it hits 0, `count` decreases.
- If the window reaches size `k = s1.size()` and `count == 0`, a valid permutation is found and `true` is returned immediately.
- When `left` advances, exiting characters from `s1` have their frequencies restored in the map.

**Complexity:** Time `O(n)`, Space `O(1)` (alphabet map).

```cpp
    bool checkInclusion(string s1, string s2) {
        unordered_map<char,int> mp;
        for(auto it : s1) mp[it]++;
        int i=0,j=0,k=s1.size(),n=s2.size(),count=mp.size();
        while(j<n){
            if(mp.find(s2[j])!=mp.end()){
                mp[s2[j]]--;
                if(mp[s2[j]]==0) count--;
            }
            if(j-i+1<k) j++;
            else if(j-i+1==k){
                if(count==0) return true;
                else{
                    if(mp.find(s2[i])!=mp.end()){
                        mp[s2[i]]++;
                        if(mp[s2[i]]==1) count++;
                    }
                    i++;j++;
                }
            }
        }
        return false;
    }
```

---

## 16. 713. Subarray Product Less Than K

**Q:** Given an array of integers `nums` and an integer `k`, return the number of contiguous subarrays where the product of elements is strictly less than `k`.

**A:** Variable-size window, shrinking from the left when the running product exceeds or equals `k`.

**Explanation:**
- The window is valid as long as the running product is `< k`.
- When `right` advances, the running product is multiplied by `nums[right]`.
- If the product exceeds or equals `k`, `left` advances, and the product is divided by `nums[left]` until it is `< k`.
- For every valid window, there are `j - i + 1` valid subarrays ending at `right`, which are added to the answer.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int numSubarrayProductLessThanK(vector<int>& nums, int k) {
        int i=0,j=0,n=nums.size();
        int ans=0;
        int prod=1;
        if(k<=1) return 0;
        while(j<n){
            prod*=nums[j];
            while(prod>=k){
                prod/=nums[i];
                i++;
            }
            ans+=(j-i+1);
            j++;
        }
        return ans;
    }
```

---

## 17. 76. Minimum Window Substring

**Q:** Given strings `s` and `t`, return the minimum window substring of `s` such that every character in `t` is included in the window.

**A:** Variable-size window mapping frequencies and shrinking to find the minimal valid span containing all required characters.

**Explanation:**
- A required map stores character frequencies for `t`. The variable `count` keeps track of distinct required characters.
- When `right` advances, matching characters decrement their frequency in the map. If a character hits 0, `count` decreases.
- The window is valid when `count == 0`. At this point, the minimal substring length is recorded.
- While the window is valid, `left` advances to shrink the window, putting required characters back into the map and incrementing `count` when necessary.

**Complexity:** Time `O(n)`, Space `O(1)` (alphabet map).

```cpp
    string minWindow(string s, string t) {
        int i=0,j=0,ans=INT_MAX,ind=0;
        unordered_map<char,int> mp;
        for(auto it : t) mp[it]++;
        int count=mp.size();
        while(j<s.size()){
            if(mp.find(s[j])!=mp.end()){
                mp[s[j]]--;
                if(mp[s[j]]==0) count--;
            }
            while(count==0){
                if(ans>j-i+1){
                    ans=j-i+1;
                    ind=i;
                }
                if(mp.find(s[i])!=mp.end()){
                    mp[s[i]]++;
                    if(mp[s[i]]==1) count++;
                }
                i++;
            }
            j++;
        }
        return ans==INT_MAX ? "" : s.substr(ind,ans);        
    }
```

---

## 18. 992. Subarrays with K Different Integers

**Q:** Given an integer array `nums` and an integer `k`, return the number of good subarrays (containing exactly `k` different integers).

**A:** Variable-size window using the "at most K minus at most K-1" counting trick.

**Explanation:**
- The helper `solve(nums, k)` calculates the number of subarrays with at most `k` different integers.
- When `right` advances, elements are added to the map.
- When the map size exceeds `k`, `left` advances and elements are removed until the distinct count drops back to `k` or less.
- The answer adds `j - i + 1` subarrays ending at `right` for each valid window.
- The exact answer is given by `solve(nums, k) - solve(nums, k - 1)`.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    int solve(vector<int>& nums, int k){
        int n=nums.size();
        int ans=0;
        unordered_map<int,int> mp;
        int i=0,j=0;
        while(j<n){
            mp[nums[j]]++;
            if(mp.size()<=k){
                ans+=j-i+1;
            }
            else{
                while(mp.size()>k){
                    mp[nums[i]]--;
                    if(mp[nums[i]]==0) mp.erase(nums[i]);
                    i++;
                }
                ans+=j-i+1;
            }
            j++;
        }
        return ans;
    }
    
    int subarraysWithKDistinct(vector<int>& nums, int k) {
        return (solve(nums,k)-solve(nums,k-1));
    }
```

---

## 19. Count distinct elements in every window

**Q:** Given an array of integers and a number `k`, return the count of distinct elements in every window of size `k`.

**A:** Fixed-size window of size `k` mapped with an ongoing frequency map tracking distinct counts.

**Explanation:**
- The first window of size `k` is manually processed, mapping counts and pushing the map size to the answer.
- For each subsequent element, `right` enters the map and `left = right - k` leaves the map.
- If an element's frequency drops to 0, it is erased to keep the map size representative of distinct elements.
- The size of the map is pushed to the answer for every shifted window.

**Complexity:** Time `O(n)`, Space `O(k)`.

```cpp
    vector<int> countDistinct(vector<int> &arr, int k) {
        unordered_map<int,int> mp;
        int n=arr.size();
        if(k>n) return {};
        int i=0;
        while(i<k) mp[arr[i++]]++;
        vector<int> ans;
        ans.push_back(mp.size());
        while(i<n){
            mp[arr[i]]++;
            mp[arr[i-k]]--;
            if(mp[arr[i-k]]==0) mp.erase(arr[i-k]);
            ans.push_back(mp.size());
            i++;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/count-distinct-elements-in-every-window/1
```

---

## 20. Count the subarrays having product less than k

**Q:** Given an array of positive numbers, count the contiguous subarrays whose product is strictly less than `k`.

**A:** Variable-size window, shrinking from the left when the current window's product equals or exceeds `k`.

**Explanation:**
- Identical logic to LeetCode 713 (Subarray Product Less Than K).
- When `right` advances, the running product is multiplied.
- If it exceeds or equals `k`, `left` advances, and the product is divided until it's less than `k`.
- The number of subarrays ending at `right` is `j - i + 1`, which is accumulated into `ans`.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int countSubArrayProductLessThanK(const vector<int>& a, int n, long long k) {
        long long ans=0;
        int i=0,j=0;
        long long pd=1;
        while(j<n){
            pd*=a[j];
            if(pd<k) ans+=j-i+1;
            else{
                while(pd>=k){
                    pd/=a[i];
                    i++;
                }
                if(pd<k) ans+=j-i+1;
            }
            j++;
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/count-the-subarrays-having-product-less-than-k1708/1/?page=1&status[]=unsolved&curated[]=1&curated[]=7&sortBy=submissions#
```

---

## 21. First negative integer in every window of size k

**Q:** Given an array `A[]` and a positive integer `K`, find the first negative integer for each and every window of size `K`.

**A:** Fixed-size window using a queue to store negative integers sequentially.

**Explanation:**
- The queue only stores the negative values encountered.
- When `right` advances, if the element is negative, it's pushed into the queue.
- When the window reaches size `K`, the front of the queue is recorded as the answer (0 if empty).
- When `left` advances, if `A[left]` matches the queue's front, it is popped from the queue.

**Complexity:** Time `O(n)`, Space `O(k)`.

```cpp
vector<long long> printFirstNegativeInteger(long long int A[],long long int N, long long int K) {
    vector<long long> ans;
    queue<long long int> q;
    int i=0,j=0;
    while(j<N){
        if(A[j]<0){
            q.push(A[j]);
        }
        if(j-i+1 < K){
            j++;   
        }else if(j-i+1==K){
            if(q.size()==0){
                ans.push_back(0);
            }else{
                ans.push_back(q.front());
                if(q.front()==A[i]){
                    q.pop();
                }
            }
            i++;j++;
        }
    }
    return ans;
 }
 
 Link - https://practice.geeksforgeeks.org/problems/first-negative-integer-in-every-window-of-size-k3345/1
```

---

## 22. Largest subarray of 0's and 1's

**Q:** Given an array of 0s and 1s, find the length of the largest subarray with equal number of 0s and 1s.

**A:** Prefix sum hashing to identify sections that have equal growth for both 0s and 1s.

**Explanation:**
- Maintain `c0` (count of 0s) and `c1` (count of 1s). The difference `c1 - c0` indicates the balance.
- If the difference is 0, the subarray from index 0 to `i` is perfectly balanced.
- A hash map stores the first occurrence of each difference. If the same difference is encountered again, it means the subarray between these two points is balanced.
- The maximum length is calculated using the distance `i - mp[diff]`.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    int maxLen(vector<int> &arr) {
        int c0=0,c1=0;
        int n=arr.size(),ans=0;
        unordered_map<int,int> mp;
        for(int i=0;i<n;i++){
            if(arr[i]) c1++;
            else c0++;
            int diff=c1-c0;
            if(diff==0) ans=max(ans,i+1);
            if(mp.find(diff)!=mp.end()) ans=max(ans,i-mp[diff]);
            else mp[diff]=i;
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/largest-subarray-of-0s-and-1s/1
```

---

## 23. Largest Sum Subarray of Size at least K

**Q:** Given an array and an integer `k`, find the maximum sum of a subarray with at least `k` elements.

**A:** Kadane's algorithm combined with a fixed window structure to dynamically shed negative prefixes.

**Explanation:**
- Initially compute the sum of the first `k` elements. This is our baseline window.
- When `right` advances beyond `k`, add the new element to the `sum` and simultaneously add `left`'s element to a running `fsum` (prefix sum prior to the window).
- If `fsum` drops below zero, it's dragging down the overall sum. Thus, remove it from `sum` and reset `fsum` to 0.
- Update `ans` with the maximum observed valid `sum`.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    long long int maxSumWithK(long long int a[], long long int n, long long int k) {
        long long int ans=INT_MIN,sum=0,fsum=0;
        int i=0,j=0;
        for(i=0;j<k;j++) sum+=a[j];
        ans=sum;
        for(j=k;j<n;j++){
            sum+=a[j];
            fsum+=a[i++];
            if(fsum<0){
                sum-=fsum;
                fsum=0;
            }
            ans=max(ans,sum);
        }
        return ans;
    }


gfg Link - https://www.geeksforgeeks.org/problems/largest-sum-subarray-of-size-at-least-k3121/1
```

---

## 24. Longest K unique characters substring

**Q:** Given a string you need to print the size of the longest possible substring that has exactly `K` unique characters.

**A:** Variable-size window with a frequency map, tracking distinct character count and shrinking when it exceeds `K`.

**Explanation:**
- The frequency map holds characters in the current window.
- If the distinct count is less than `K`, advance `right`.
- If the distinct count is exactly `K`, record the maximum window length and advance `right`.
- If it exceeds `K`, advance `left` and decrement frequencies until the distinct count drops back to `K`.

**Complexity:** Time `O(n)`, Space `O(1)` (character map).

```cpp
    int longestKSubstr(string s, int k) {
        int i=0,j=0;
        int ans=-1;
        unordered_map<char,int> mp;
        while(j<s.size()){
            mp[s[j]]++;
            if(mp.size()<k) j++;   
            else if(mp.size()==k){
                ans=max(ans,j-i+1);
                j++;
            }
            else{
                while(mp.size()>k){
                    mp[s[i]]--;
                    if(mp[s[i]]==0){
                        mp.erase(s[i]);
                    }
                    i++;
                    if(mp.size()==k){
                        ans=max(ans,j-i+1);
                    }
                }
                j++;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/longest-k-unique-characters-substring0853/1
```

---

## 25. Longest Sub-Array with Sum K

**Q:** Given an array containing integers and an integer `K`, find the length of the longest subarray with the sum of the elements equal to `K`.

**A:** Prefix sum hashing for arrays with negative and positive elements; variable window for strictly positive arrays.

**Explanation:**
- The first approach uses a variable window: valid only when the array contains purely positive elements, shrinking when `sum > K`.
- The second approach uses a prefix sum map to solve for all types of elements (including negatives).
- It stores the earliest index where a specific prefix sum was encountered. If `sum - K` exists in the map, a subarray summing to `K` is found.

**Complexity:** Approach 1 (positives): Time `O(n)`, Space `O(1)`. Approach 2 (all elements): Time `O(n)`, Space `O(n)`.

```cpp
// For positive element only...
    int lenOfLongSubarr(int A[],  int N, int K) { 
        // Complete the function
        int ans=0;
        int i=0,j=0;
        int sum=0;
        while(j<N){
            sum+=A[j];
            if(sum<K) j++;
            else if(sum==K) {
                ans=max(ans,j-i+1);
                j++;
            }else{
                while(sum>K){
                    sum-=A[i];
                    i++;
                    if(sum==K) ans=max(ans,j-i+1);
                }
                j++;
            }
        }
        return ans;
    }
    
// For all elments....
    int lenOfLongSubarr(int A[],  int N, int K) { 
        // Complete the function
        unordered_map<int,int> mp;
        int sum=0,ans=0;
        for(int i=0;i<N;i++){
            sum+=A[i];
            if(sum==K) ans=i+1;
            if(mp.find(sum-K)!=mp.end()){
                ans=max(ans,i-mp[sum-K]);
            }
            if(mp.find(sum)==mp.end()){
                mp[sum]=i;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/longest-sub-array-with-sum-k0809/1
```

---

## 26. Max Sum Subarray of size K

**Q:** Given an array of integers and a number `K`, calculate the maximum sum of a subarray of size `K`.

**A:** Fixed-size window, maintaining a running sum over exactly `K` elements.

**Explanation:**
- When `right` advances, add the current element to the sum.
- If the window size is less than `K`, just advance `right`.
- Once the window size is exactly `K`, update the maximum sum, then subtract the element at `left` and advance both pointers to slide the window.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    long maximumSumSubarray(int K, vector<int> &Arr , int N){
        long ans=INT_MIN,sum=0;
        int i=0,j=0;
        while(j<N){
            sum+=Arr[j];
            if(j-i+1<K) j++;
            else if(j-i+1==K){
                ans=max(ans,sum);
                sum-=Arr[i];
                i++;j++;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/max-sum-subarray-of-size-k5313/1
```

---

## 27. Smallest window containing 0, 1 and 2

**Q:** Given a string `S` consisting of the characters 0, 1, and 2, find the length of the smallest substring containing all three characters.

**A:** Variable-size window with a fixed-size frequency array, shrinking when all three characters are present.

**Explanation:**
- A static array `a[3]` tracks the frequencies of '0', '1', and '2'. `cnt` tracks how many unique characters exist in the window.
- When `right` advances, frequency and distinct count are updated.
- Once `cnt == 3`, the minimum length is recorded, and `left` advances to shrink the window while maintaining `cnt == 3` to find the minimal valid window.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int smallestSubstring(string S) {
        // Code here
        int ans=INT_MAX;
        int i=0,j=0,n=S.size();
        int a[3]={0};
        int cnt=0;
        while(j<n){
            a[S[j]-'0']++;
            if(a[(S[j]-'0')]==1) cnt++;
            if(cnt==3){
                ans=min(ans,j-i+1);
                while(cnt==3){
                    a[(S[i]-'0')]--;
                    if(a[(S[i]-'0')]==0) cnt--;
                    i++;
                    if(cnt==3) ans=min(ans,j-i+1);
                }
            }
            j++;
        }
        if(ans==INT_MAX) return -1;
        return ans;
    }


gfg link - https://www.geeksforgeeks.org/problems/smallest-window-containing-0-1-and-2--170637/1
```

---

## 28. Smallest window in a string containing all the characters of another string

**Q:** Given two strings `s` and `p`, find the smallest substring in `s` containing all characters of `p` (including duplicates).

**A:** Variable-size window tracking exact character frequencies similar to Minimum Window Substring.

**Explanation:**
- The target map initializes frequencies from `p` and `count` records distinct characters to match.
- When `right` advances, if the character is in the target map, it decrements the target frequency. A drop to 0 means this character requirement is fulfilled, decrementing `count`.
- Once `count == 0`, all requirements are met. The window shrinks from the `left`, recording the minimal valid length, until `count` increases again.

**Complexity:** Time `O(n)`, Space `O(1)` (character map).

```cpp
    string smallestWindow (string s, string p){
        unordered_map<char,int> mp;
        for(auto it : p){
            mp[it]++;
        }
        int count=mp.size();
        
        int start=0,length=INT_MAX;
        int i=0,j=0;
        while(j<s.size()){
            if(mp.find(s[j])!=mp.end()){
                mp[s[j]]--;
                if(mp[s[j]]==0){
                    count--;
                }
            }
            if(count==0){
                while(count==0){
                    if(length > j-i+1){
                        length=j-i+1;
                        start=i;
                    }
                    if(mp.find(s[i])!=mp.end()){
                        mp[s[i]]++;
                        if(mp[s[i]]==1){
                            count++;
                        }
                    }
                    i++;
                }
            }
            j++;
            
        }
        if(length!=INT_MAX)
            return s.substr(start,length);
        else
            return "-1";   
    }
    
    Link - https://practice.geeksforgeeks.org/problems/smallest-window-in-a-string-containing-all-the-characters-of-another-string-1587115621/1/
```

---

## 29. Substrings of length k with k-1 distinct elements

**Q:** Given a string `S` and an integer `K`, find the count of substrings of length `K` which have exactly `K-1` distinct characters.

**A:** Fixed-size window, maintaining a frequency map and evaluating exactly when the window hits size `K`.

**Explanation:**
- The map keeps track of the distinct characters and their frequencies inside the window.
- When `right` advances, characters are added to the map.
- When the window length is exactly `K`, if `mp.size() == K - 1`, the window contains exactly `K - 1` distinct elements, so the answer is incremented.
- Then the element at `left` is removed from the map and `left` advances to slide the window.

**Complexity:** Time `O(n)`, Space `O(k)`.

```cpp
    int countOfSubstrings(string S, int K) {
        // code here
        int n=S.size();
        unordered_map<char,int> mp;
        int i=0,j=0,ans=0;
        while(j<n){
            mp[S[j]]++;
            if(j-i+1<K) j++;
            else if(j-i+1==K){
                if(mp.size()==K-1) ans++;
                mp[S[i]]--;
                if(mp[S[i]]==0) mp.erase(S[i]);
                i++;j++;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/substrings-of-length-k-with-k-1-distinct-elements/1
```
