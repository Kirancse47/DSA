# Stack & Queue — Q&A

> Source: `Kirancse47/DSA` -> `StackQueue/` — 19 problems

Ordering: Alphabetical by filename

## Cheat sheet
**Monotonic Stacks & Deques:** 
Use a stack to maintain elements in increasing or decreasing order. This is perfect for "next greater/smaller element", "largest rectangle in histogram", and "stock span". Use a monotonic deque for sliding-window maximums.
- **Time Complexity:** Amortized `O(n)` — although there is a nested `while` loop, every element is pushed and popped at most once.
- **Problems:** `739. Daily Temperatures`, `84. Largest Rectangle in Histogram`, `K Sized Subarray Maximum`.

**Bracket Matching & Parsing:**
Use a stack to match pairs of symbols or evaluate postfix expressions. Push opening brackets/operands and pop when encountering closing brackets/operators.
- **Problems:** `20. Valid Parentheses`, `Evaluation of Postfix Expression`.

**Structural Simulations & State:**
Implement queues with stacks, min-stacks encoding minimums in pairs, or state tracking for string reduction.
- **Problems:** `232. Implement Queue using Stacks`, `Get Min from Stack`, `Restrictive Candy Crush`.

**Canonical Monotonic Stack Skeleton:**
```cpp
stack<int> st;
for (int i = 0; i < n; i++) {
    // For "Next Greater", maintain decreasing order in stack
    while (!st.empty() && arr[st.top()] < arr[i]) {
        // arr[i] is the next greater element for arr[st.top()]
        st.pop();
    }
    st.push(i); // Push index to calculate widths/spans
}
```

---

## 1. 1249. Minimum Remove to Make Valid Parentheses

**Q:** Remove the minimum number of parentheses from a string to make it valid and return it.

**A:** Bracket matching stack with index tracking.

**Explanation:**
- Store indices of unmatched '(' in the stack.
- When finding ')', pop if stack is non-empty; otherwise, mark the invalid ')' with '#' for removal.
- After the loop, mark any remaining '(' indices in the stack with '#' and build the final string excluding '#'.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    string minRemoveToMakeValid(string s) {
        stack<int> st;
        for(int i=0;i<s.size();i++){
            if(s[i]=='('){
                st.push(i);
            }else if(s[i]==')'){
                if(!st.empty()){
                    st.pop();
                }else{
                    s[i]='#';
                }
            }
        }
        
        while(!st.empty()){
            s[st.top()]='#';
            st.pop();
        }
        
        string ans;
        for(auto it : s){
            if(it!='#')
                ans.push_back(it);
        }
        
        return ans;
    }
```

---

## 2. 20. Valid Parentheses

**Q:** Determine if a string containing only parentheses is valid by closing brackets in the correct order.

**A:** Bracket matching stack.

**Explanation:**
- Push opening brackets `(`, `[`, `{` onto the stack.
- On encountering a closing bracket, check if it matches the top of the stack and pop.
- If it doesn't match or the stack is empty, it's invalid. Return true if the stack is empty at the end.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
  bool isValid(string s) {
        stack<char> st;
        for(auto e : s){
            if(e=='(' || e=='[' || e=='{'){
                st.push(e);
            }else if(!st.empty()){
                if(e==')' && st.top()!='('){
                    return false;
                }else if(e==']' && st.top()!='['){
                    return false;
                }else if(e=='}' && st.top()!='{'){
                    return false;
                }else{
                    st.pop();
                }
            }else{
                return false;
            }
        }
        if(st.empty())
            return true;
        else
            return false;
    }
```

---

## 3. 232. Implement Queue using Stacks

**Q:** Implement a FIFO queue using only two LIFO stacks.

**A:** Two stacks for enqueue and dequeue amortized operations.

**Explanation:**
- Push elements onto the `input` stack.
- For pop/peek, if the `output` stack is empty, transfer all elements from `input` to `output` reversing their order.
- Pop or peek from the `output` stack.

**Complexity:** Time `O(1)` amortized for pop/peek, Space `O(n)`.

```cpp
stack<int> input, output;
    MyQueue() {
        
    }
    
    void push(int x) {
       input.push(x);
    }
    
    int pop() {
        if(output.empty()){
            while(!input.empty()){
                output.push(input.top());
                input.pop();
            }
        }
        int x=output.top();output.pop();
        return x;
    }
    
    int peek() {
        if(output.empty()){
            while(!input.empty()){
                output.push(input.top());
                input.pop();
            }
        }
        return output.top();
    }
    
    bool empty() {
        return (input.empty() && output.empty());
    }
```

---

## 4. 2696. Minimum String Length After Removing Substrings

**Q:** Find the minimum string length after repeatedly removing "AB" or "CD" substrings.

**A:** Character stack for pattern matching.

**Explanation:**
- Push characters onto the stack one by one.
- If the current character forms "AB" or "CD" with the stack's top, pop the top instead of pushing.
- The size of the stack at the end is the minimum string length.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    int minLength(string s) {
        stack<char> stack;
        for (int i = 0; i < s.length(); i++) {
            char cur_char = s[i];
            if (stack.empty()) {
                stack.push(cur_char);
                continue;
            }
            if (cur_char == 'B' && stack.top() == 'A') {
                stack.pop();
            }
            else if (cur_char == 'D' && stack.top() == 'C') {
                stack.pop();
            }
            else {
                stack.push(cur_char);
            }
        }
        return stack.size();
    }
```

---

## 5. 32. Longest Valid Parentheses

**Q:** Find the length of the longest valid (well-formed) parentheses substring.

**A:** Bracket matching stack with index tracking.

**Explanation:**
- Initialize the stack with `-1` to represent the base index before the first valid substring.
- Push the index of `(` onto the stack.
- For `)`, pop the stack. If it becomes empty, push the current index as a new base. Otherwise, update the max length with `i - st.top()`.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    int longestValidParentheses(string s) {
        int n=s.size();
        stack<int> st;
        st.push(-1);
        int ans=0;
        for(int i=0;i<n;i++){
            if(s[i]=='('){
                st.push(i);
            }else{
                st.pop();
                if(st.empty()){
                    st.push(i);
                }else{
                    ans=max(ans,i-st.top());
                }
            }
        }
        return ans;
    }
```

---

## 6. 402. Remove K Digits

**Q:** Remove exactly k digits from a string representation of a number to form the smallest possible integer.

**A:** Monotonic increasing stack (using a string).

**Explanation:**
- Maintain an increasing order of digits. If the current digit is smaller than the last added digit, pop the last digit and decrement `k`.
- Avoid adding leading zeros by checking if the stack is empty.
- Truncate any remaining `k` characters from the end and return "0" if empty.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once, Space `O(n)` for the output string.

```cpp
    string removeKdigits(string num, int k) {
        string st="";
        for(auto it : num){
            while(!st.empty() && st.back()>it && k){
                st.pop_back();
                k--;
            }
            if(!st.empty() || it!='0') st.push_back(it);
        }
        while(!st.empty() && k--) st.pop_back();
        return st=="" ? "0" : st;
    }
```

---

## 7. 503. Next Greater Element II

**Q:** Find the next greater element for every element in a circular array.

**A:** Monotonic decreasing stack traversed twice.

**Explanation:**
- Loop from `2*n - 1` down to `0` to simulate a circular traversal.
- Pop elements from the stack that are less than or equal to the current element `nums[i % n]`.
- If `i < n`, record the stack's top as the answer before pushing the current element.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once, Space `O(n)`.

```cpp
    vector<int> nextGreaterElements(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans(n,-1);
        stack<int> st;
        for(int i=2*n-1;i>=0;i--){
            while(!st.empty() && st.top()<=nums[i%n]){
                st.pop();
            }
            if(i<n){
                if(!st.empty()) ans[i]=st.top();
            }
            st.push(nums[i%n]);
        }
        return ans;
    }
```

---

## 8. 739. Daily Temperatures

**Q:** Find the number of days you have to wait until a warmer temperature for each day in an array.

**A:** Monotonic decreasing stack tracking indices.

**Explanation:**
- Traverse the array backwards and pop elements smaller than or equal to the current temperature.
- The wait time is the difference between the index at the stack's top and the current index.
- Push the current index onto the stack.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once, Space `O(n)`.

```cpp
    vector<int> dailyTemperatures(vector<int>& temp) {
        int n=temp.size();
        vector<int> ans(n,0);
        stack<int> st;
        for(int i=n-1;i>=0;i--){
            while(!st.empty() && temp[i] >= temp[st.top()]) st.pop();
            if(!st.empty()) ans[i]=st.top()-i;
            st.push(i);
        }
        return ans;
    }
```

---

## 9. 84. Largest Rectangle in Histogram

**Q:** Calculate the largest rectangular area possible within a given histogram.

**A:** Monotonic increasing stack to find left and right boundaries.

**Explanation:**
- Two Stack Approach: Find the index of the next smaller element to the left and right by maintaining increasing stacks. Compute area with `(r[i] - l[i] - 1) * a[i]`.
- One Stack Approach: Maintain an increasing stack of indices. When a smaller element is found, pop the top and calculate the area for the popped height using the current index and the new top index as boundaries.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once (one stack approach) or twice (two stack approach); Space `O(n)`.

```cpp
// Using Two Stack and Array...
    int largestRectangleArea(vector<int>& a) {
        int n=a.size();
        vector<int> l(n,-1),r(n,n);
        
        stack<int> st1;
        st1.push(n-1);
        r[n-1]=n;
        for(int i=n-2;i>=0;i--){
            while(st1.size()>0 && a[i]<=a[st1.top()]) st1.pop();
            if(st1.size()==0) r[i]=n;
            else r[i]=st1.top();
            st1.push(i);
        }

        stack<int> st2;
        st2.push(0);
        l[0]=-1;
        for(int i=1;i<n;i++){
            while(st2.size()>0 && a[i]<=a[st2.top()]) st2.pop();
            if(st2.size()==0) l[i]=-1;
            else l[i]=st2.top();
            st2.push(i);
        }

        int mxA=0;
        for(int i=0;i<n;i++){
            mxA=max(mxA,(r[i]-l[i]-1)*a[i]);
        }
        return mxA;
    }
    
// Using one stack...
    int largestRectangleArea(vector<int>& a) {
        int n=a.size();
        int maxA=0;
        stack<int> st;
        for(int i=0;i<=n;i++){
            while(!st.empty() && (i==n || a[st.top()]>=a[i])){
                int h=a[st.top()];
                st.pop();
                int width;
                if(st.empty()) width=i;
                else width=i-st.top()-1;
                maxA=max(maxA,h*width);
            }
            st.push(i);
        }
        return maxA;
    }   
```

---

## 10. 907. Sum of Subarray Minimums

**Q:** Calculate the sum of minimums over all contiguous subarrays.

**A:** Monotonic increasing stacks for next and previous smaller elements.

**Explanation:**
- Use two stacks to find the distance to the next smaller and previous smaller (or equal) elements for each `arr[i]`.
- The number of subarrays where `arr[i]` is the minimum is `(ns[i] + 1) * (ps[i] + 1)`.
- Sum these counts multiplied by `arr[i]` modulo `1e9+7`.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once for both passes, Space `O(n)`.

```cpp
    int sumSubarrayMins(vector<int>& arr) {
        stack<int> s1,s2;
        int n=arr.size();
        vector<int> ns(n),ps(n);
        for(int i=0;i<n;i++){
            ns[i]=n-i-1;
            ps[i]=i;
        }

        for(int i=0;i<n;i++){
            while(!s1.empty() && arr[s1.top()]>arr[i]){
                ns[s1.top()]=i-s1.top()-1;
                s1.pop();
            }
            s1.push(i);
        }

        for(int i=n-1;i>=0;i--){
            while(!s2.empty() && arr[s2.top()]>=arr[i]){
                ps[s2.top()]=s2.top()-i-1;
                s2.pop();
            }
            s2.push(i);
        }

        long long ans=0;
        long long mod=1e9+7;
        for(int i=0;i<n;i++){
            long long t=( (arr[i]*(ns[i]+1))%mod * (ps[i]+1) ) % mod;
            ans=(ans+t)%mod;
        }
        return ans;
    }
```

---

## 11. Evaluation of Postfix Expression

**Q:** Evaluate an arithmetic expression given in Reverse Polish Notation (postfix).

**A:** Stack-based operand evaluation.

**Explanation:**
- Push numerical operands onto the stack.
- When encountering an operator, pop the top two operands, apply the operation, and push the result back.
- The final result is the single element remaining on the stack.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    int evaluate(vector<string>& arr) {
        stack<int> st;
        int ans=0;
        for(auto it : arr){
            int num;
            if(isdigit(it[it.size()-1])){
                num=stoi(it);
            }else{
                int n2=st.top();st.pop();
                int n1=st.top();st.pop();
                switch(it[0]){
                    case '+':
                        num=n1+n2;
                        break;
                    case '-':
                        num=n1-n2;
                        break;
                    case '*':
                        num=n1*n2;
                        break;
                    case '/':
                        num=n1/n2;
                        break;
                    default:
                        return -1;
                        break;
                }
            }
            st.push(num);
        }
        return st.top();
    }

gfg link - https://www.geeksforgeeks.org/problems/evaluation-of-postfix-expression1735/1
```

---

## 12. Get Min from Stack

**Q:** Implement a stack that supports push, pop, peek, and retrieving the minimum element in constant time.

**A:** Auxiliary state stored in a stack of pairs.

**Explanation:**
- Push pairs of `{value, current_minimum}` onto the stack.
- The `current_minimum` is determined by taking the minimum of the new value and the previous `current_minimum` (at the top).
- `getMin` simply returns the second element of the pair at the top.

**Complexity:** Time `O(1)` per operation, Space `O(n)`.

```cpp
class Solution {
  public:
    stack<pair<int,int>> st;
    Solution() {
    }

    // Add an element to the top of Stack
    void push(int x) {
        if(!st.empty()){
            st.push({x,min(x,st.top().second)});
        }else{
            st.push({x,x});
        }
    }

    // Remove the top element from the Stack
    void pop() {
        if(!st.empty()){
            st.pop();
        }
    }

    // Returns top element of the Stack
    int peek() {
        if(!st.empty()){
            return st.top().first;
        }
        return -1;
    }

    // Finds minimum element of Stack
    int getMin() {
        if(!st.empty()){
            return st.top().second;
        }
        return -1;
    }
};


gfg link - https://www.geeksforgeeks.org/problems/get-minimum-element-from-stack/1
```

---

## 13. K Sized Subarray Maximum

**Q:** Find the maximum element in every contiguous subarray of size k.

**A:** Monotonic decreasing deque for sliding window maximums.

**Explanation:**
- Maintain a decreasing deque of values. Pop elements from the back that are smaller than the incoming element.
- Once the window reaches size `k`, record the maximum at the front of the deque.
- Remove the outgoing element from the front if it matches `arr[i]` as the window slides.

**Complexity:** Time `O(n)` amortised because each element is added and removed from the deque at most once, Space `O(k)`.

```cpp
    vector<int> maxOfSubarrays(vector<int>& arr, int k) {
        deque<int> dq;
        int n=arr.size(),i=0,j=0;
        vector<int> ans;
        while(j<n){
            while(!dq.empty() && dq.back()<arr[j]) dq.pop_back();
            dq.push_back(arr[j]);
            if(j-i+1<k) j++;
            else if(j-i+1==k){
                ans.push_back(dq.front());
                if(dq.front()==arr[i]) dq.pop_front();
                i++;j++;
            }
        }
        return ans;
    }

gff link - https://www.geeksforgeeks.org/problems/maximum-of-all-subarrays-of-size-k3101/1
```

---

## 14. Longest Bounded-Difference Subarray

**Q:** Find the longest subarray where the absolute difference between any two elements is at most x.

**A:** Two monotonic deques for sliding window minimum and maximum.

**Explanation:**
- Maintain an increasing deque `mn` for minimums and a decreasing deque `mx` for maximums as the `j` pointer advances.
- If `mx.front() - mn.front() > x`, shrink the window from the left by advancing `i` and popping the matching fronts.
- Keep track of the longest valid window `[st, en]` and return the subarray slice.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once across the deques, Space `O(n)`.

```cpp
    vector<int> longestSubarray(vector<int>& arr, int x) {
        int n=arr.size();
        int i=0,j=0;
        deque<int> mn,mx;
        int st=0,en=0;
        while(j<n){
            while(!mn.empty() && mn.back()>arr[j]) mn.pop_back();
            while(!mx.empty() && mx.back()<arr[j]) mx.pop_back();
            mx.push_back(arr[j]);
            mn.push_back(arr[j]);
            if(!mx.empty() && !mn.empty() && (mx.front()-mn.front() <= x)){
                if((en-st)<(j-i)) { en=j; st=i; };
                j++;
            }else if(i<j && !mx.empty() && !mn.empty() && (mx.front()-mn.front() > x)){
                if(arr[i]==mn.front()) mn.pop_front();
                if(arr[i]==mx.front()) mx.pop_front();
                i++;j++;
            }
        }
        if(st==en) return {arr[0]};
        vector<int> res;
        for(int i=st;i<=en;i++) res.push_back(arr[i]);
        return res;
    }

gfg link - https://www.geeksforgeeks.org/problems/longest-bounded-difference-subarray/1
```

---

## 15. Maximum Difference

**Q:** Find the maximum absolute difference between the nearest smaller element on the left and right for every element in an array.

**A:** Two monotonic increasing stacks.

**Explanation:**
- Traverse left-to-right to find the nearest smaller element for `ls[i]` using an increasing stack.
- Traverse right-to-left to find the nearest smaller element for `rs[i]` using a new increasing stack.
- Iterate through the arrays to maximize `abs(ls[i] - rs[i])`.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once per pass, Space `O(n)`.

```cpp
    int findMaxDiff(vector<int> &arr) {
        int n=arr.size();
        vector<int> ls(n,0),rs(n,0);
        stack<int> st;
        st.push(arr[0]);
        for(int i=1;i<n;i++){
            while(!st.empty() && st.top()>=arr[i]) st.pop();
            if(!st.empty()) ls[i]=st.top();
            st.push(arr[i]);
        }
        while(!st.empty()) st.pop();
        st.push(arr[n-1]);
        for(int i=n-2;i>=0;i--){
            while(!st.empty() && st.top()>=arr[i]) st.pop();
            if(!st.empty()) rs[i]=st.top();
            st.push(arr[i]);
        }
        int ans=0;
        for(int i=0;i<n;i++) ans=max(ans,abs(ls[i]-rs[i]));
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/maximum-difference-1587115620/1
```

---

## 16. Remove K Digits

**Q:** Remove exactly k digits from a string representation of a number to form the smallest possible integer.

**A:** Monotonic increasing character stack.

**Explanation:**
- Loop through the string, popping characters from the stack when a smaller digit is encountered and `K > 0`.
- Ignore leading zeros by only pushing when the stack is non-empty or the digit is non-zero.
- If `K` remains, pop from the end. Reconstruct the string backwards and reverse (or slice from the end).

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once, Space `O(n)`.

```cpp
    string removeKdigits(string S, int K) {
        int n=S.size();
        stack<char> st;
        for(auto s : S){
            while(!st.empty() && K>0 && st.top()>s){
                st.pop();
                K--;
            }
            
            if(!st.empty() || s!='0'){
                st.push(s);
            }
        }
        
        while(!st.empty() && K--){
            st.pop();
        }
        
        if(st.empty()) return "0";
        
        while(!st.empty()){
            S[n-1]=st.top();
            st.pop();
            n--;
        }
        return S.substr(n);
    }

gfg Link - https://www.geeksforgeeks.org/problems/remove-k-digits/1
```

---

## 17. Restrictive Candy Crush

**Q:** Remove any adjacent k identical characters from a string repeatedly until no such sequence remains.

**A:** Stack of character frequency pairs.

**Explanation:**
- Push pairs `{character, count}` onto the stack.
- If the current character matches the stack's top, increment its count. Otherwise, push with count `1`.
- If the count reaches `k-1` when processing the matching `k`-th character, pop the previous `k-1` characters.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
string Reduced_String(int k,string s){
        if(k==1) return "";
        stack<pair<char,int>> st;
        for(auto it : s){
            if(st.empty()) {
                st.push({it,1});
                continue;
            }
            char ch=st.top().first;
            int cnt=st.top().second;
            if(it!=ch){
                st.push({it,1});
            }else{
                if(cnt==k-1){
                    while(cnt-- && !st.empty()) st.pop();
                }else{
                    st.push({it,cnt+1});
                }
            }
        }
        string ans="";
        while(!st.empty()){
            ans.push_back(st.top().first);
            st.pop();
        }
        reverse(ans.begin(),ans.end());
        return ans;
    }


gfg link - https://www.geeksforgeeks.org/problems/restrictive-candy-crush--141631/1?itm_source=geeksforgeeks&itm_medium=article&itm_campaign=practice_card
```

---

## 18. Sorting a Queue without extra space

**Q:** Sort a queue in ascending order using only queue operations and no additional data structures.

**A:** Repeated minimum extraction and rear insertion.

**Explanation:**
- For each un-sorted portion size `n - i`, traverse the queue to find the minimum element in that window.
- Extract the minimum and push it to the rear while pushing the other elements back in order.
- Repeat until all elements are sorted at the rear.

**Complexity:** Time `O(n^2)`, Space `O(1)`.

```cpp
int minIndex(queue<int> &q, int sortedIndex){
    int min_index = -1;
    int min_val = INT_MAX;
    int n = q.size();
    for (int i=0; i<n; i++){
        int curr = q.front();q.pop();
        if (curr <= min_val && i <= sortedIndex){
            min_index = i;
            min_val = curr;
        }
        q.push(curr);
    }
    return min_index;
}

void insertMinToRear(queue<int> &q, int min_index){
    int min_val;
    int n = q.size();
    for (int i = 0; i < n; i++){
        int curr = q.front();q.pop();
        if (i != min_index) q.push(curr);
        else min_val = curr;
    }
    q.push(min_val);
}
  
void sortQueue(queue<int> &q){
    for (int i = 1; i <= q.size(); i++){
        int min_index = minIndex(q, q.size() - i);
        insertMinToRear(q, min_index);
    }
}

Link - https://www.geeksforgeeks.org/sorting-queue-without-extra-space/
```

---

## 19. Stock span problem

**Q:** Calculate the stock span for each day, defined as the maximum number of consecutive days just before the given day where the price is less than or equal to today's price.

**A:** Monotonic decreasing stack tracking indices.

**Explanation:**
- Traverse the prices, popping days from the stack where the price is less than or equal to the current day.
- The span is `i + 1` if the stack becomes empty (no larger previous price), else `i - st.top()`.
- Push the current day's index onto the stack.

**Complexity:** Time `O(n)` amortised because each element is pushed and popped at most once, Space `O(n)`.

```cpp
    vector<int> calculateSpan(vector<int>& arr) {
        int n=arr.size();
        stack<int> st;
        vector<int> ans(n);
        for(int i=0;i<n;i++){
            while(!st.empty() && arr[st.top()]<=arr[i]) st.pop();
            if(st.empty()) ans[i]=i+1;
            else ans[i]=i-st.top();
            st.push(i);
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/stock-span-problem-1587115621/1
```

---

