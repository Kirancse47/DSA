# Math & Number Theory — Q&A

> Source: `Kirancse47/DSA` -> `Math/` — 29 problems

Ordering: Alphabetical by filename

## Cheat sheet
- **Divisor Enumeration / Primality**: Check up to `sqrt(n)`. See: *204. Count Primes(Sieve of Eratosthenes)*
- **Sieve of Eratosthenes**: Mark multiples starting from `i*i` up to `n` in `O(n log log n)` time. See: *2523. Closest Prime Numbers in Range*
- **Fast Exponentiation**: Compute powers in `O(log n)` time using `num%2` and `num/2` while squaring the base. See: *50. Pow(x, n)*
- **Digit Manipulation**: Extract digits with `% 10` and ` / 10`. See: *7. Reverse Integer*
- **Overflow-safe Patterns**: Use `long long` for accumulations or compare against `INT_MAX/10` before multiplying by 10. See: *7. Reverse Integer*
- **Modular Arithmetic**: Apply `% MOD` at each step to prevent overflow. See: *Code Testcase Testcase Test Result 1922. Count Good Numbers*
- **Prime Factorization**: Extract prime factors by dividing out `i` starting from 2 up to `sqrt(n)`. See: *650. 2 Keys Keyboard*

---

## 1. 1103. Distribute Candies to People

**Q:** Distribute `c` candies to `n` people where the `i`-th distribution gives `candy` (incrementing by 1) candies until they run out, returning an array of candies per person.

**A:** Simulation using a while loop and modulo-like wrap around.

**Explanation:**
- Keep track of the current candy amount to give and iterate through the array.
- Reset the index `i` to 0 when it reaches `n` to simulate the cyclic distribution.
- Subtract the distributed amount from the total and handle the remaining fractional candies at the end.

**Complexity:** Time `O(sqrt(c))`, Space `O(n)`.

```cpp
    vector<int> distributeCandies(int c, int n) {
        vector<int> ans(n,0);
        int candy=1;
        int i=0;
        while(candy<=c){
            if(i==n) i=0;
            ans[i]+=candy;
            c-=candy;
            candy++;
            i++;
        }
        if(c>0){
            if(i==n) i=0;
            ans[i]+=c;
        }
        return ans;
    }
```

---

## 2. 1262. Greatest Sum Divisible by Three

**Q:** Given an array of integers, find the maximum possible sum of its elements that is divisible by 3.

**A:** Dynamic programming tracking the maximum sum for each modulo remainder 0, 1, and 2.

**Explanation:**
- Maintain a DP array `best` of size 3 where `best[idx]` stores the max sum with `sum % 3 == idx`.
- For each number, compute new possible sums by adding it to all states in `best`, then update `best` if the new sums are larger for their respective modulo indices.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int maxSumDivThree(vector<int>& nums) {
        vector<int> best(3,0);
        vector<int> sum(3,0);
        for(auto it : nums){
            for(int i=0;i<3;i++){
                sum[i]=best[i]+it;
            }
            for(int i=0;i<3;i++){
                int idx=sum[i]%3;
                best[idx]=max(best[idx],sum[i]);
            }
        }
        return best[0];
    }
```

---

## 3. 1291. Sequential Digits

**Q:** Find all integers in the range `[low, high]` that have sequential digits.

**A:** Generate all possible sequential digit numbers starting from each digit 1 through 9.

**Explanation:**
- Start with a single digit `i` and progressively append `i+1`, `i+2`, etc., multiplying by 10.
- If the generated number falls within `[low, high]`, append it to the answer.
- Sort the resulting array before returning since generation order doesn't guarantee sorted value order across different starting digits.

**Complexity:** Time `O(1)`, Space `O(1)`.

```cpp
    vector<int> sequentialDigits(int low, int high) {
        vector<int> ans;
        for (int i=1;i<=9;i++) {
            int num=i;
            int nextDigit =i+1;
            while (num <= high && nextDigit <= 9) {
                num = num * 10 + nextDigit;
                if (low <= num && num <= high) ans.push_back(num);
                nextDigit++;
            }
        }
        sort(ans.begin(), ans.end());
        return ans;
    }
```

---

## 4. 1701. Average Waiting Time

**Q:** Given a 2D array of customers with arrival and prep times, calculate the average waiting time for all customers.

**A:** Simulation of a single chef's timeline tracking when each order finishes.

**Explanation:**
- Maintain the current time (`end`) which advances to either the chef's previous finish time or the customer's arrival time, whichever is later.
- Add the prep time to the current time, and add the difference between this finish time and the customer's arrival to the total waiting time.
- Divide the total wait by the number of customers.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    double averageWaitingTime(vector<vector<int>>& customers) {
        int n=customers.size();
        int start = customers[0][0];
        int end=start+customers[0][1];
        double ans=customers[0][1];
        for(int i=1;i<n;i++){
            if(customers[i][0]<end){
                ans+=(end-customers[i][0]);
                start=end;
            }else start=customers[i][0];
            end=start+customers[i][1];
            ans+=customers[i][1];
        }
        ans=ans/(double)n;
        return ans;
    }
```

---

## 5. 1780. Check if Number is a Sum of Powers of Three

**Q:** Check if an integer `n` can be represented as a sum of distinct powers of 3.

**A:** Base 3 conversion checking if any digit is 2.

**Explanation:**
- Repeatedly divide `n` by 3 and check the remainder.
- If the remainder is 2, it means the number requires a coefficient of 2 for some power of 3, which violates the "distinct powers" rule.

**Complexity:** Time `O(log n)`, Space `O(1)`.

```cpp
    bool checkPowersOfThree(int n) {
        while(n){
            if(n%3==2) return false;
            n/=3;
        }
        return true;
    }
```

---

## 6. 204. Count Primes(Sieve of Eratosthenes)

**Q:** Count the number of prime numbers strictly less than an integer `n`.

**A:** Sieve of Eratosthenes using a boolean array.

**Explanation:**
- Initialize a boolean array of size `n` to true.
- Iterate up to `sqrt(n)`, and for each prime `i`, mark its multiples starting from `i*i` as false since lower multiples are already covered by smaller primes.
- Count and return the number of true values from index 2 to `n-1`.

**Complexity:** Time `O(n log log n)`, Space `O(n)`.

```cpp
    int countPrimes(int n) {
        int ans=0;
        vector<bool> prime(n,true);
        for(int i=2;i*i<n;i++){
            if(prime[i]){
                for(int j=i*i;j<n;j+=i) prime[j]=false;
            }
        }
        for(int i=2;i<n;i++){
            if(prime[i]) ans++;
        }
        return ans;
    }
```

---

## 7. 2125. Number of Laser Beams in a Bank

**Q:** Calculate the total number of laser beams between security devices in a bank represented by an array of binary strings, where beams connect devices in adjacent non-empty rows.

**A:** Counting the number of '1's in each row and multiplying adjacent non-zero counts.

**Explanation:**
- Traverse each row and sum the integer values of the characters to count devices.
- Store non-zero counts in a new array, filtering out empty rows which beams pass through without incident.
- Multiply consecutive elements in the filtered array and accumulate the sum.

**Complexity:** Time `O(m * n)`, Space `O(m)`.

```cpp
    int numberOfBeams(vector<string>& bank) {
        int m=bank.size(),n=bank[0].size();
        vector<int> a;
        for(int i=0;i<m;i++){
            int cnt=0;
            for(int j=0;j<n;j++){
                cnt+=(bank[i][j]-'0');
            }
            if(cnt>0) a.push_back(cnt);
        }
        int ans=0;
        for(int i=1;i<a.size();i++){
            ans+=(a[i]*a[i-1]);
        }
        return ans;
    }
```

---

## 8. 2523. Closest Prime Numbers in Range

**Q:** Find a pair of prime numbers in the range `[left, right]` that have the minimum absolute difference.

**A:** Sieve of Eratosthenes to generate primes followed by a linear scan to find the minimum gap.

**Explanation:**
- Use the sieve up to `right` to mark non-primes.
- Scan the range `[left, right]` tracking the previous prime found (`pre`) and the current prime (`cur`).
- Update the answer if `cur - pre` is smaller than the smallest difference found so far.

**Complexity:** Time `O(R log log R)`, Space `O(R)`.

```cpp
    vector<int> closestPrimes(int left, int right) {
        vector<int> prime(right+1,true);
        prime[0]=false;prime[1]=false;
        for(int i=2;i*i<=right;i++){
            if(prime[i]){
                for(int j=i*i;j<=right;j+=i) prime[j]=false;
            }
        }
        vector<int> ans(2,-1);
        int diff=INT_MAX;
        int pre=left;
        while(pre<=right && !prime[pre]) pre++;
        for(int cur=pre+1;cur<=right;cur++){
            if(prime[cur]){
                if(diff>(cur-pre)){
                    ans[0]=pre;
                    ans[1]=cur;
                    diff=cur-pre;
                }
                pre=cur;
            }
        }
        return ans;
    }
```

---

## 9. 2579. Count Total Number of Colored Cells

**Q:** Return the total number of colored cells in a grid after `n` minutes of coloring outward from a center cell.

**A:** Iterative accumulation adding `4 * (i - 1)` at each step.

**Explanation:**
- The pattern starts with 1 cell.
- At each minute `i` from 2 to `n`, 4 cells are added to each of the `i-1` diagonal expanding sides, so we add `4 * (i - 1)`.
- Accumulate the sum using a `long long` to prevent integer overflow for large `n`.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    long long coloredCells(int n) {
        long long ans=1;
        for(int i=2;i<=n;i++){
            ans+=4*(i-1);
        }
        return ans;
    }
```

---

## 10. 263. Ugly Number

**Q:** Determine if an integer `n` is an ugly number (whose prime factors are limited to 2, 3, and 5).

**A:** Iterative division by 2, 3, and 5.

**Explanation:**
- Handle base cases: non-positive numbers return false, 1 returns true.
- Repeatedly divide out the factor 2 while `n` is even, then 3, then 5.
- If the remaining value is exactly 1, the number is ugly.

**Complexity:** Time `O(log n)`, Space `O(1)`.

```cpp
    bool isUgly(int n) {
        if(n<=0) return false;
        if(n==1) return true;
        while(n%2==0) n/=2;
        while(n%3==0) n/=3;
        while(n%5==0) n/=5;
        if(n!=1) return false;
        return true;
    }
```

---

## 11. 264. Ugly Number II

**Q:** Return the `n`-th ugly number.

**A:** Min-heap and a HashSet to generate and track ugly numbers in ascending order.

**Explanation:**
- Seed the priority queue and set with 1.
- Extract the minimum `n` times; for each extracted number, multiply it by 2, 3, and 5 and push the new values into the heap if not already seen.
- Use `long` to prevent intermediate multiplication overflows before the `n`-th value is found.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
    int nthUglyNumber(int n) {
        vector<int> up={2,3,5};
        unordered_set<long> vis;
        priority_queue<long,vector<long>,greater<long>> mnh;
        vis.insert(1);
        mnh.push(1);
        long cur_ugly;
        for(int i=0;i<n;i++){
            cur_ugly=mnh.top();mnh.pop();
            for(auto it : up){
                long new_ugly=cur_ugly*it;
                if(vis.find(new_ugly)==vis.end()){
                    mnh.push(new_ugly);
                    vis.insert(new_ugly);
                }
            } 
        }
        return (int)cur_ugly;
    }
```

---

## 12. 367. Valid Perfect Square

**Q:** Given a positive integer `num`, return true if it is a perfect square, else false, without using a built-in sqrt function.

**A:** Binary search from 1 to `num/2`.

**Explanation:**
- Check the base case for `num == 1`.
- Use `long long` for the midpoint square `m * m` to prevent overflow during comparison against `num`.
- Narrow the binary search range `[l, h]` based on whether `m * m` is less than or greater than `num`.

**Complexity:** Time `O(log n)`, Space `O(1)`.

```cpp
    bool isPerfectSquare(int num) {
        if(num==1) return true;
        int l=1,h=num/2;
        while(l<=h){
            long long m=l+(h-l)/2;
            if(m*m==num) return true;
            else if(m*m<num) l=m+1;
            else h=m-1;
        }
        return false;
    }
```

---

## 13. 50. Pow(x, n)

**Q:** Implement `pow(x, n)`, which calculates `x` raised to the power `n`.

**A:** Fast exponentiation by squaring (Binary Exponentiation).

**Explanation:**
- Cast `n` to a `long long` and take its absolute value to handle the edge case `n == INT_MIN` avoiding overflow.
- If the current power is odd, multiply the result by `x` and decrement the power. If even, square `x` and halve the power.
- Invert the final answer if the original `n` was negative.

**Complexity:** Time `O(log n)`, Space `O(1)`.

```cpp
    double myPow(double x, int n) {
        double ans=1.0;
        long long int num=n;
        num=abs(num);
        while(num){
            if(num%2==1){
                ans=ans*x;
                num--;
            }else{
                x=x*x;
                num=num/2;
            }
        }
        if(n>=0){
            return ans; 
        }
        return 1.0/ans;
    }
```

---

## 14. 60. Permutation Sequence

**Q:** Return the `k`-th permutation sequence of numbers from 1 to `n`.

**A:** Mathematical construction using factorials to select digits one by one.

**Explanation:**
- Precompute `(n-1)!` and populate a vector with available digits from 1 to `n`.
- Treat `k` as 0-indexed by subtracting 1.
- Find the index of the current digit by dividing `k` by the current factorial, append it, remove it from the list, update `k = k % fact`, and adjust the factorial for the remaining size.

**Complexity:** Time `O(n^2)`, Space `O(n)`.

```cpp
    string getPermutation(int n, int k) {
        int fact=1;
        vector<int> numbers;
        for(int i=1;i<n;i++){
            fact=fact*i;
            numbers.push_back(i);
        }
        numbers.push_back(n);
        string ans="";
        k=k-1;
        while(true){
            ans=ans+to_string(numbers[k/fact]);
            numbers.erase(numbers.begin()+k/fact);
            if(numbers.size()==0){
                break;
            }
            k=k%fact;
            fact=fact/numbers.size();
        }
        return ans;
    }
```

---

## 15. 650. 2 Keys Keyboard

**Q:** Return the minimum number of 'Copy All' and 'Paste' operations to get exactly `n` 'A' characters on the screen.

**A:** Prime factorization summing the factors.

**Explanation:**
- To minimize operations, greedily break the problem into the smallest possible uniform multiplicative steps (prime factors).
- Iterate up to `sqrt(n)`. When `n` is divisible by `i`, add `i` to the operations and divide `n` by `i`.
- If `n` is greater than 1 after the loop, add the remaining prime itself.

**Complexity:** Time `O(sqrt(n))`, Space `O(1)`.

```cpp
    int minSteps(int n) {
        if(n==1) return 0;
        int ans=0;
        int i=2;
        while(i*i<=n){
            if(n%i==0){
                ans+=i;
                n/=i;
            }else i++;
        }
        if(n!=1) ans+=n;
        return ans;
    }
```

---

## 16. 7. Reverse Integer

**Q:** Reverse the digits of a 32-bit signed integer `x`, returning 0 if the reversed value overflows.

**A:** Modulo extraction and accumulation with overflow bounds checks.

**Explanation:**
- Repeatedly pop the last digit via `x % 10` and reduce `x` via `x / 10`.
- Before updating `rev = rev * 10 + pop`, check if `rev` exceeds `INT_MAX/10` or drops below `INT_MIN/10`.
- Also perform the exact last-digit check (`> 7` for max, `< -8` for min) if `rev` is exactly at the `10`-divided threshold.

**Complexity:** Time `O(log x)`, Space `O(1)`.

```cpp
    int reverse(int x) {
        int rev = 0;
        while (x != 0) {
            int pop = x % 10;
            x /= 10;
            if (rev > INT_MAX/10 || (rev == INT_MAX / 10 && pop > 7)) return 0;
            if (rev < INT_MIN/10 || (rev == INT_MIN / 10 && pop < -8)) return 0;
            rev = rev * 10 + pop;
        }
        return rev;
    }
```

---

## 17. 869. Reordered Power of 2

**Q:** Check if the digits of `n` can be reordered such that it is a power of 2.

**A:** String sorting and comparison against precomputed powers of 2.

**Explanation:**
- Convert the input `n` to a string and sort its characters to form a canonical signature.
- Generate the first 32 powers of 2 (up to `2^31`), convert each to a string, and sort them.
- Return true if the canonical signature of `n` matches any power of 2's signature.

**Complexity:** Time `O(1)`, Space `O(1)`.

```cpp
    bool reorderedPowerOf2(int n) {
        string n1=to_string(n);
        sort(n1.begin(),n1.end());
        for(int i=0;i<32;i++){
            string n2=to_string(1<<i);
            sort(n2.begin(),n2.end());
            if(n1==n2) return true;
        }
        return false;
    }
```

---

## 18. Check if a number is divisible by 8

**Q:** Given a number represented as a string, determine if it is divisible by 8 (return 1 or -1).

**A:** Divisibility rule for 8 checking only the last three digits.

**Explanation:**
- A number is divisible by 8 if and only if its last three digits form a number divisible by 8.
- If the string length exceeds 3, extract the substring of the last 3 characters; otherwise, use the whole string.
- Convert to integer and check modulo 8.

**Complexity:** Time `O(1)`, Space `O(1)`.

```cpp
int DivisibleByEight(string s){
    int n=s.size();
    int num;
    if(n>3){
        string st=s.substr(n-3);
        num=stoi(st);
    }else num=stoi(s);
    if(num%8==0) return 1;
    return -1;
}

gfg link - https://www.geeksforgeeks.org/problems/check-if-a-number-is-divisible-by-83957/1
```

---

## 19. Code Testcase Testcase Test Result 1018. Binary Prefix Divisible By 5

**Q:** Given a binary array, return a boolean array indicating whether the binary prefix number ending at each index is divisible by 5.

**A:** Running modulo accumulation.

**Explanation:**
- Maintain the current integer value of the prefix.
- Since the prefix can grow extremely large, update the value at each step using `num = ((num << 1) + nums[i]) % 5`.
- Check if the modulo is exactly 0 and store the result.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    vector<bool> prefixesDivBy5(vector<int>& nums) {
        int n=nums.size();
        int num=0;
        vector<bool> ans(n,false);
        int mod=1e9+7;
        for(int i=0;i<n;i++){
            num=((num<<1) + nums[i])%5;
            ans[i]=(num==0);
        }
        return ans;
    }
```

---

## 20. Code Testcase Testcase Test Result 1922. Count Good Numbers

**Q:** Count the number of "good" digit strings of length `n` (even indices are even digits, odd indices are prime digits) modulo 10^9+7.

**A:** Binary exponentiation calculating `5^(even_positions) * 4^(odd_positions) % MOD`.

**Explanation:**
- There are 5 even digits (0,2,4,6,8) and 4 prime digits (2,3,5,7) between 0 and 9.
- The string has `n - n/2` even indices and `n/2` odd indices.
- Use modular fast exponentiation to compute the large powers and multiply them, avoiding overflow.

**Complexity:** Time `O(log n)`, Space `O(1)`.

```cpp
    #define ll long long
    int MOD = 1000000007;

    ll binaryExponentiation(ll a,ll b){
        ll res = 1;
        while(b){
            if(b&1) res = (res * a) % MOD;
            a = (a * a) % MOD;
            b /= 2;
        }
        return res;
    }

    int countGoodNumbers(long long n) {
        return (binaryExponentiation(4,n/2) * binaryExponentiation(5,n-n/2))%MOD;
    }
```

---

## 21. Consecutive 1's not allowed

**Q:** Count the number of binary strings of length `n` that do not contain consecutive 1s, modulo 10^9+7.

**A:** Dynamic programming via Fibonacci-like sequence building.

**Explanation:**
- Keep track of the number of valid strings ending in '0' and ending in '1'.
- For length `i`, strings ending in '1' can only come from strings of length `i-1` ending in '0'. Strings ending in '0' can come from any valid string.
- Update `oneending` and `zeroending` iteratively and apply modulo at each addition.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
ll countStrings(int n) {
    // code here
    ll int  zeroending=1;
      ll int   oneending=1;
      ll int sum=zeroending+oneending;
    
      if(n==1) return sum;
      
      int i=2;
      while(i<=n){
          oneending=zeroending;
          zeroending=sum;
          sum=(oneending+zeroending)%1000000007;
          i++;
      }
      return sum%1000000007;
}


problem - https://www.geeksforgeeks.org/problems/consecutive-1s-not-allowed1912/1
```

---

## 22. Count of sum of consecutives

**Q:** Count the number of ways a positive integer `N` can be expressed as the sum of consecutive positive integers.

**A:** Mathematical constraint solving using the arithmetic progression sum formula.

**Explanation:**
- The sum of `L+1` consecutive integers starting at `a` is `(L+1)*a + (L*(L+1))/2`.
- Rearranging for `a`: `a = (N - L*(L+1)/2) / (L+1)`. We iterate `L` starting from 1 while `L*(L+1) < 2*N`.
- Check if `a` resolves to an exact integer by checking if the floating-point division has no fractional part.

**Complexity:** Time `O(sqrt(N))`, Space `O(1)`.

```cpp
    int getCount(int N) {
        // code here
        int ans=0;
        for(int l=1;l*(l+1)<N*2;l++){
            double a=((double)N-(l*(l+1)/2))/(l+1);
            if(a-(int)a == 0.0) ans++;
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/count-of-sum-of-consecutives3741/1
```

---

## 23. Count Unique Vowel Strings

**Q:** Count unique strings that can be formed by arranging the distinct vowels found in string `s`.

**A:** Factorial permutation scaled by individual vowel frequencies.

**Explanation:**
- Count the frequencies of only vowels ('a', 'e', 'i', 'o', 'u') present in the string.
- The number of unique permutations of distinct items is `factorial(unique_vowels)`.
- Multiply this base permutation count by the occurrences of each unique vowel, satisfying this problem's specific constraints.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    int fact(int n){
        return ((n == 1 || n == 0) ? 1 : n * fact(n - 1));
    }
    int vowelCount(string& s) {
        unordered_map<char,int> mp;
        for(auto it : s){
            if(it =='a' || it =='e' || it =='i' || it =='o' || it =='u') mp[it]++;
        }
        if(mp.size()==0) return 0;
        int ans =0;
        ans=fact(mp.size());
        for(auto it : mp){
            ans*=it.second;
        }
        return ans;
    }

gfg - https://www.geeksforgeeks.org/problems/count-unique-vowel-strings/1
```

---

## 24. Number of Rectangles in a Circle

**Q:** Find the number of distinct rectangles with integral side lengths `l` and `w` that can fit entirely inside a circle of radius `r`.

**A:** Nested loops iterating over all possible integral side lengths.

**Explanation:**
- A rectangle fits in a circle if its diagonal length is less than or equal to the circle's diameter `2*r`.
- Check the condition `l*l + w*w <= 4*r*r` for `l` and `w` up to `2*r`.
- Increment a counter for every pair that satisfies the condition.

**Complexity:** Time `O(r^2)`, Space `O(1)`.

```cpp
    int rectanglesInCircle(int r) {
        int track=0;
        for(int l=1; l<=2*r; l++){
            for(int w=1; w<=2*r; w++){
                if(l*l+w*w<=4*r*r){
                    track++;
                }
            }
        }
        return track;
    }

gfg link - https://www.geeksforgeeks.org/problems/rectangles-in-a-circle0457/1
```

---

## 25. Prime Pair with Target Sum

**Q:** Find a pair of prime numbers `(i, n-i)` that sum up to `n`, returning the lexicographically smallest pair, or `[-1, -1]`.

**A:** Sieve of Eratosthenes to query primality in `O(1)`.

**Explanation:**
- Precompute all primes up to `n` using the Sieve of Eratosthenes.
- Iterate `i` from 2 to `n-1`. For the first `i` where both `i` and `n-i` are prime, return the pair.
- The first hit naturally yields the smallest `i` since we iterate ascendingly.

**Complexity:** Time `O(n log log n)`, Space `O(n)`.

```cpp
    vector<bool> SieveOfEratosthenes(int n){
        vector<bool>prime(n+1,true);
        for(int p=2;p*p<=n;p++){
            if(prime[p]==true) {
                for(int i=p*p;i<=n;i+=p) prime[i]=false;
            }
        }
        return prime;
    }
    vector<int> getPrimes(int n) {
        vector<int>ans(2,-1);
        vector<bool>temp=SieveOfEratosthenes(n);
        for(int i=2;i<n;i++){
            if(temp[i]==true && temp[n-i]==true){
                ans[0]=i;
                ans[1]=n-i;
                return ans;
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/sum-of-prime4751/1
```

---

## 26. Recursive sequence

**Q:** Find the value of the `n`-th term of a sequence where term `i` is the product of `i` consecutive increasing numbers, summing all terms modulo 10^9+7.

**A:** Iterative simulation tracking the current multiplier `st` globally.

**Explanation:**
- The sequence sums terms: 1 + (2*3) + (4*5*6) + ...
- Maintain a running global counter `st`. For the `i`-th term, multiply `i` consecutive numbers from `st` using `% 10^9+7` at each multiplication.
- Add each term's result to the total sum and modulo it again.

**Complexity:** Time `O(n^2)`, Space `O(1)`.

```cpp
    long long sequence(int n){
        long long ans=0;
        int st=1;
        long long mod=1e9+7;
        for(int i=1;i<n+1;i++){
            long long cur=1;
            int j=0;
            while(j<i){
                cur=(cur*st)%mod;
                st++;
                j++;
            }
            ans=(ans+cur)%mod;
        }
        return ans%mod;
    }


gfg link - https://www.geeksforgeeks.org/problems/recursive-sequence1611/1
```

---

## 27. Smallest number

**Q:** Find the smallest `d`-digit number whose sum of digits equals `s`, returning as a string or "-1" if impossible.

**A:** Brute force linear scan from `10^(d-1)` up to `10^d - 1`.

**Explanation:**
- Calculate the numeric bounds for a `d`-digit number: start at `pow(10, d-1)` and end at `pow(10, d) - 1`.
- Write a helper to compute the sum of digits of a given number.
- Return the string representation of the first number that matches the sum `s`.

**Complexity:** Time `O(10^d * d)`, Space `O(1)`.

```cpp
    int sum(int n){
        int t=0;
        while(n){
            t+=n%10;
            n/=10;
        }
        return t;
    }
  
    string smallestNumber(int s, int d) {
        for(int i=pow(10,d-1);i<=pow(10,d)-1;i++){
            if(sum(i)==s) return to_string(i);
        }
        return "-1";
    }

gfg link - https://www.geeksforgeeks.org/problems/smallest-number5829/1
```

---

## 28. Three way partitioning

**Q:** Partition an array into three parts: elements `< a`, elements `in [a, b]`, and elements `> b`.

**A:** Dutch National Flag algorithm (3-pointer approach).

**Explanation:**
- Maintain `i` for the boundary of `< a`, `k` for `> b`, and `j` for the current element.
- If `array[j] < a`, swap it with `array[i]`, and increment both `i` and `j`.
- If `array[j] > b`, swap it with `array[k]`, and decrement `k` (do not increment `j` since the swapped element needs inspection).

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
   void threeWayPartition(vector<int>& array,int a, int b){
        int i=0,j=0,k=array.size()-1;
        while(j<=k){
            if(array[j]<a){
                swap(array[i],array[j]);
                i++;j++;
            }else if(array[j]>b){
                swap(array[k],array[j]);
                k--;
            }else j++;
        }
    }

gfg link - https://www.geeksforgeeks.org/problems/three-way-partitioning/1
```

---

## 29. Two Repeated Elements

**Q:** Find the two elements that are repeated in an array of size `n+2` containing numbers from 1 to `n`.

**A:** In-place marking using the array indices themselves by negating values.

**Explanation:**
- For each element, treat its absolute value minus 1 as an index `idx`.
- If `arr[idx]` is already negative, the element is a duplicate, so push it to the answer.
- Otherwise, negate `arr[idx]` to mark it as seen.

**Complexity:** Time `O(n)`, Space `O(1)`.

```cpp
    vector<int> twoRepeated (int arr[], int n) {
        vector<int> ans;
        for(int i=0;i<n+2;i++){
            if(arr[abs(arr[i])-1]<0) ans.push_back(abs(arr[i]));
            else arr[abs(arr[i])-1]*=(-1);
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/two-repeated-elements-1587115621/1
```
