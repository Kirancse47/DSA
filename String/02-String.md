# Strings — Q&A

> Source: `Kirancse47/DSA` -> `String/` — 34 problems

Alphabetical ordering.

## Cheat sheet
- **Two pointers:** Scanning strings from opposite ends or tracking positions in two strings. (*1750. Minimum Length of String After Deleting Similar Ends*, *2486. Append Characters to String to Make Subsequence*)
- **Frequency/count arrays:** Character counts to determine structural properties like anagrams or palindrome potential. (*242. Valid Anagram*, *1400. Construct K Palindrome Strings*)
- **Hashing:** Mapping characters or strings to group items or verify isomorphism. (*49. Group Anagrams*, *890. Find and Replace Pattern*)
- **KMP / prefix function:** Fast pattern matching and calculating the longest prefix that is also a suffix. (*Search Pattern (KMP-Algorithm)*, *Minimum characters to be added at front to make string palindrome*)
- **Palindrome expansion:** Expanding outward from possible centers to find all palindromes or the longest palindrome. (*5. Longest Palindromic Substring*, *647. Palindromic Substrings*)
- **Sliding window over characters:** Expanding and shrinking a range to find a substring satisfying specific conditions. (*3. Longest Substring Without Repeating Characters*)
- **String builder/reversal tricks:** Building strings incrementally or rotating/reversing parts of the string. (*67. Add Binary*, *459. Repeated Substring Pattern*)
- **Stack-based parsing:** Processing adjacent characters that cancel out or modify each other. (*1544. Make The String Great*)

---

## 1. 13. Roman to Integer

**Q:** Given a Roman numeral string, return its integer equivalent.

**A:** String parsing / Simulation.

**Explanation:**
- Iterate through the string character by character.
- Compare the current character's value with the next character's value.
- If the current value is less than the next, subtract it from the total; otherwise, add it.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the length of the string.

```cpp
    int romanToInt(string s) {
        int n=0;
        for(int i=0;i<s.size();i++){
            if(s[i]=='I'){
                if(s[i+1]=='V' || s[i+1]=='X'){
                    n-=1;
                }else{
                    n+=1;
                }
            }
            else if(s[i]=='X'){
                if(s[i+1]=='L' || s[i+1]=='C'){
                    n-=10;
                }else{
                    n+=10;
                }
            }
            else if(s[i]=='C'){
                if(s[i+1]=='D' || s[i+1]=='M'){
                    n-=100;
                }else{
                    n+=100;
                }
            }
            else if(s[i]=='V'){
                n+=5;
            }
            else if(s[i]=='L'){
                n+=50;
            }
            else if(s[i]=='D'){
                n+=500;
            }
            else if(s[i]=='M'){
                n+=1000;
            }
        }
        return n;
    }
```

---

## 2. 14. Longest Common Prefix

**Q:** Given an array of strings, return the longest common prefix string amongst them.

**A:** Two pointers / Simulation.

**Explanation:**
- Find the minimum length among all strings to prevent out-of-bounds errors.
- Iterate through each character position up to the minimum length.
- Compare the character across all strings; if a mismatch is found, return the accumulated prefix.

**Complexity:** Time `O(n * m)`, Space `O(1)` where `n` is the number of strings and `m` is the minimum string length.

```cpp
    string longestCommonPrefix(vector<string>& strs) {
        int n=strs.size();
        if(n==1) return strs[0];
        string ans="";
        int sz=INT_MAX;
        for(int i=0;i<n;i++){
            int t=strs[i].size();
            sz=min(sz,t);
        }
        for(int i=0;i<sz;i++){
            char c=strs[0][i];
            for(int j=0;j<n-1;j++){
                if(strs[j][i]!=strs[j+1][i]) return ans;
            }
            ans.push_back(c);
        }
        return ans;
    }
```

---

## 3. 1400. Construct K Palindrome Strings

**Q:** Given a string `s` and an integer `k`, return true if you can use all the characters in `s` to construct `k` palindrome strings or false otherwise.

**A:** Frequency/count arrays.

**Explanation:**
- First, if `k` is greater than the string length, it's impossible.
- Count the frequencies of all 26 characters.
- Palindromes can have at most one character with an odd frequency. Thus, the number of characters with odd frequencies must be less than or equal to `k`.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the string length.

```cpp
    bool canConstruct(string s, int k) {
        if(k>s.size()) return false;
        vector<int> fq(26,0);
        for(auto it : s) fq[it-'a']++;
        int nof=0;
        for(int i=0;i<26;i++){
            if(fq[i] & 1) nof++;
        }
        if(nof<=k) return true;
        return false;
    }
```

---

## 4. 151. Reverse Words in a String

**Q:** Given an input string `s`, reverse the order of the words and return it without leading, trailing, or multiple spaces between words.

**A:** Two pointers / String traversal.

**Explanation:**
- Iterate through the string from the end to the beginning.
- Skip trailing and multiple spaces.
- When a word is found, extract it and append it to the answer string, followed by a space if more words exist.

**Complexity:** Time `O(n)`, Space `O(n)` where `n` is the string length.

```cpp
  string reverseWords(string s) {
        int n=s.size();
        string ans="";
        int i=n-1,j=n-1;
        while(s[i]==' '){
            i--;j--;
        }
        while(i>=0){
            while(i>=0 && s[i]!=' ') i--;
            ans+=s.substr(i+1,j-i);
            j=i;
            while(i>=0 && s[i]==' '){
                i--;j--;
            }
            if(i>=0 && s[i]!=' ') ans.push_back(' ');
        }
        return ans;
    }
```

---

## 5. 1544. Make The String Great

**Q:** Given a string `s` of lower and upper case English letters, return the string after removing all adjacent characters that are the same letter but different cases.

**A:** Stack-based parsing.

**Explanation:**
- Iterate through the string, pushing characters onto a stack.
- If the current character and the top of the stack differ only in case (ASCII difference of 32), pop the stack (canceling them out).
- Finally, reconstruct the string from the stack and reverse it.

**Complexity:** Time `O(n)`, Space `O(n)` where `n` is the string length.

```cpp
string makeGood(string s) {
    stack<char> st;
    for(auto c : s){
        if(!st.empty() && abs(c-st.top())==32) st.pop();
        else st.push(c);
    }
    string ans="";
    while(!st.empty()){
        ans.push_back(st.top());
        st.pop();
    }
    reverse(ans.begin(),ans.end());
    return ans;
}
```

---

## 6. 1750. Minimum Length of String After Deleting Similar Ends

**Q:** Given a string `s`, repeatedly delete prefixes and suffixes consisting of the same single character, and return the minimum length of the remaining string.

**A:** Two pointers.

**Explanation:**
- Use two pointers, one at the start and one at the end of the string.
- If the characters at both pointers match, shrink both ends inward past all contiguous occurrences of that character.
- Stop when the pointers cross or point to different characters.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the string length.

```cpp
int minimumLength(string s) {
    int n=s.size();
    int i=0,j=n-1;
    while(i<j && s[i]==s[j]){
        char temp=s[i];
        while(i<=j && s[i]==temp) i++;
        while(i<=j && s[j]==temp) j--;
    }
    return j-i+1;
}
```

---

## 7. 1910. Remove All Occurrences of a Substring

**Q:** Given two strings `s` and `part`, return `s` after repeatedly removing the leftmost occurrence of `part`.

**A:** String builder / Simulation.

**Explanation:**
- Iterate through `s` and build the answer string incrementally.
- Check if the end of the answer string matches the last character of `part`.
- If it does and the length is sufficient, check if the suffix matches `part`; if so, erase it.

**Complexity:** Time `O(n * m)`, Space `O(n)` where `n` is the length of `s` and `m` is the length of `part`.

```cpp
    string removeOccurrences(string s, string part) {
        string ans;
        int len=part.size();
        char endCh=part[len-1];
        for(auto it : s){
            ans.push_back(it);
            if(it==endCh && ans.size()>=len){
                if(ans.substr(ans.size()-len)==part){
                    ans.erase(ans.size()-len);
                }
            }
        }
        return ans;
    }
```

---

## 8. 242. Valid Anagram

**Q:** Given two strings `s` and `t`, return true if `t` is an anagram of `s`, and false otherwise.

**A:** Frequency/count arrays.

**Explanation:**
- Early exit if the lengths differ.
- Use a hash map (or array) to count character frequencies of `s`.
- Decrement frequencies for characters in `t`. Return false if any count drops below zero.

**Complexity:** Time `O(n)`, Space `O(1)` (for 26 letters) where `n` is the string length.

```cpp
    bool isAnagram(string s, string t) {
        if(s==t) return true;
        if(s.size()!=t.size()) return false;
        unordered_map<char,int> mp;
        for(auto it : s) mp[it]++;
        for(auto it : t){
            mp[it]--;
            if(mp[it]<0) return false;
        }
        return true;;
    }
```

---

## 9. 2486. Append Characters to String to Make Subsequence

**Q:** Given strings `s` and `t`, return the minimum number of characters that need to be appended to the end of `s` so that `t` becomes a subsequence of `s`.

**A:** Two pointers.

**Explanation:**
- Iterate through `s` looking for the characters of `t` in order.
- Maintain a pointer in `t` that advances only when a match is found in `s`.
- The number of characters left unmatched in `t` is the answer.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the length of `s`.

```cpp
    int appendCharacters(string s, string t) {
        int s_index = 0, t_index = 0;
        int s_length = s.length(), t_length = t.length();
        while (s_index < s_length && t_index < t_length) {
            if (s[s_index] == t[t_index]) {
                t_index++;
            }
            s_index++;
        }
    
        return t_length - t_index;
    }
```

---

## 10. 28. Find the Index of the First Occurrence in a String

**Q:** Given two strings `needle` and `haystack`, return the index of the first occurrence of `needle` in `haystack`, or -1 if it is not part of `haystack`.

**A:** String matching (KMP / Rabin-Karp / Brute Force).

**Explanation:**
- **Brute Force:** Check every possible starting index in `haystack` for a match.
- **KMP:** Compute the prefix function array for the `needle`, then traverse `haystack` efficiently without backtracking the `haystack` pointer.
- **Rabin-Karp:** Compute rolling hashes to quickly check if a substring of `haystack` matches the hash of `needle`.

**Complexity:** Time `O(n * m)` for brute force, `O(n + m)` for KMP, `O(n + m)` expected for Rabin-Karp. Space `O(1)` for brute force, `O(m)` for KMP, `O(n)` for Rabin-Karp prefix hashes.

```cpp
// BruteForce...
     int strStr(string haystack, string needle) {
       int n=haystack.size(),m=needle.size();
       for(int i=0;i<=n-m;i++){
           if(haystack[i]==needle[0]){
               if(haystack.substr(i,m)==needle) return i;
           }
       }
       return -1;
    }
    
// Using KMP...
    vector<int> lps(string &s){
        int n=s.size();
        vector<int> ps(n,0);
        int i=0,j=1;
        while(j<n){
            if(s[i]==s[j]){
                ps[j]=i+1;
                i++;j++;
            }else{
                if(i!=0) i=ps[i-1];
                else{
                    ps[j]=0;
                    j++;
                }
            }
        }
        return ps;
    }
    
    int strStr(string txt, string pat) {
        int tl=txt.size(),pl=pat.size();
        vector<int> ps=lps(pat);
        int i=0,j=0;
        while(i<tl && j<pl){
            if(txt[i]==pat[j]){
                i++;j++;
            }else{
                if(j!=0) j=ps[j-1];
                else i++;
            }
            if(j==pl) return i-pl;
        }
        return -1;
    }
    
 // Using Rabin-Karp Algo...
    int strStr(string txt, string pat) {
        int T=txt.size(),S=pat.size();
        int p=31;
        int N=1e5+7,m=1e9+7;
        vector<long long>power(N);
        power[0]=1;
        for(int i=1;i<N;i++){
            power[i]=(power[i-1]*p)%m;
        }
    
        vector<long long> h(T+1,0);
        for(int i=0;i<T;i++){
            h[i+1]=(h[i]+(txt[i]-'a'+1)*power[i])%m;
        }
    
        long long h_s=0;
        for(int i=0;i<S;i++){
            h_s=(h_s+(pat[i]-'a'+1)*power[i])%m;
        }
        for(int i=0;i+S-1<T;i++){
            long long cur_h=(h[i+S]-h[i]+m)%m;
            if(cur_h==(h_s*power[i])%m){
                return i;
            }
        }
        return -1;
    }
```

---

## 11. 3. Longest Substring Without Repeating Characters

**Q:** Given a string `s`, return the length of the longest substring without repeating characters.

**A:** Sliding window over characters.

**Explanation:**
- Expand a sliding window defined by two pointers `i` and `j`.
- Track the characters in the window using a frequency map.
- If a duplicate is detected (map size < window size), shrink the window from the left (`i`) until all characters are unique.

**Complexity:** Time `O(n)`, Space `O(min(n, m))` where `n` is string length and `m` is alphabet size.

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

## 12. 3042. Count Prefix and Suffix Pairs I

**Q:** Given an array of strings, return the number of pairs `(i, j)` where `i < j` and `words[i]` is both a prefix and a suffix of `words[j]`.

**A:** Two pointers / Simulation.

**Explanation:**
- Compare each possible pair of strings.
- A string `s1` is a prefix and suffix of `s2` if their lengths allow it, and the characters match at both the beginning and the end of `s2`.

**Complexity:** Time `O(n^2 * m)`, Space `O(1)` where `n` is the number of strings and `m` is the max string length.

```cpp
    bool isPrefixAndSuffix(string s1,string s2){
        int n=s2.size(),m=s1.size();
        if(m>n) return false;
        for(int i=0;i<m;i++){
            if(s1[i]!=s2[i] || s1[i]!=s2[n-m+i]) return false;
        }
        return true;
    }

    int countPrefixSuffixPairs(vector<string>& words) {
        int n=words.size();
        int ans=0;
        for(int i=0;i<n-1;i++){
            for(int j=i+1;j<n;j++){
                if(isPrefixAndSuffix(words[i],words[j])) ans++;
            }
        }
        return ans;
    }
```

---

## 13. 345. Reverse Vowels of a String

**Q:** Given a string `s`, reverse only all the vowels in the string and return it.

**A:** Two pointers.

**Explanation:**
- Start with pointers at the beginning and end of the string.
- Move inward until both point to a vowel.
- Swap the vowels and continue moving inward.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the string length.

```cpp
    bool check(char c) {
        if(c=='A' || c=='a') return true;
        if(c=='E' || c=='e') return true;
        if(c=='I' || c=='i') return true;
        if(c=='O' || c=='o') return true;
        if(c=='U' || c=='u') return true;
        return false;
    }
    string reverseVowels(string s) {
        int i=0,j=s.size()-1;
        while(i<j){
            while(i<j && !check(s[i])) i++;
            while(i<j && !check(s[j])) j--;
            swap(s[i],s[j]);
            i++;j--;
        }
        return s;
    }
```

---

## 14. 38. Count and Say

**Q:** Given an integer `n`, return the `n`th term of the count-and-say sequence.

**A:** Recursion / String simulation.

**Explanation:**
- Recursively obtain the string for `n - 1`.
- Iterate over the previous string, counting consecutive occurrences of the same character.
- Append the count and the character to build the answer for `n`.

**Complexity:** Time `O(2^n)` (approximate length growth), Space `O(2^n)` where `n` is the term number.

```cpp
    string countAndSay(int n) {
        if(n==1) return "1";
        string s=countAndSay(n-1);
        int c=1;
        string ans="";
        for(int i=0;i<s.size();i++){
            if(i==s.size()-1 || s[i]!=s[i+1]){
                ans+=to_string(c)+s[i];
                c=1;
            }
            else c++;
        }
        return ans;
    }
```

---

## 15. 459. Repeated Substring Pattern

**Q:** Given a string `s`, check if it can be constructed by taking a substring of it and appending multiple copies of the substring together.

**A:** String rotation / matching.

**Explanation:**
- Concatenate the string with itself to create `t = s + s`.
- Remove the first and last characters of `t`.
- If `s` can still be found in `t`, then `s` is a repeated substring pattern.

**Complexity:** Time `O(n)`, Space `O(n)` where `n` is the string length.

```cpp
    bool repeatedSubstringPattern(string s) {
        string t=s+s;
        t.pop_back();
        t.erase(t.begin());
        if(t.find(s)!=-1) return 1;
        else return 0;
    }
```

---

## 16. 49. Group Anagrams

**Q:** Given an array of strings, group the anagrams together and return the grouped array.

**A:** Hashing and sorting.

**Explanation:**
- For each string, generate a canonical key by sorting its characters.
- Use a hash map to group the original strings using this sorted key.
- Extract all grouped lists from the map.

**Complexity:** Time `O(n * m \log m)`, Space `O(n * m)` where `n` is the number of strings and `m` is the maximum string length.

```cpp
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        unordered_map<string,vector<string>> mp;
        for(auto str : strs){
            string key=str;
            sort(key.begin(),key.end());
            mp[key].push_back(str);
        }
        vector<vector<string>> ans;
        for(auto it : mp){
            ans.push_back(it.second);
        }
        return ans;
    }
```

---

## 17. 5. Longest Palindromic Substring

**Q:** Given a string `s`, return the longest palindromic substring in `s`.

**A:** Palindrome expansion.

**Explanation:**
- Iterate through each character in the string, treating it as a possible center of a palindrome.
- Expand outward for both odd-length (centered on the character) and even-length (centered between characters) palindromes.
- Keep track of the maximum length and the starting index found so far.

**Complexity:** Time `O(n^2)`, Space `O(1)` where `n` is the string length.

```cpp
    string longestPalindrome(string s) {
        int n=s.size();
        
        int start=0,len=1;
        int l,h;
        for(int i=1;i<n;i++){
            
            // For even length palindrome...
            l=i-1;
            h=i;
            while(l>=0 && h<n && s[l]==s[h]){
                if(h-l+1>len){
                    start=l;
                    len=h-l+1;
                }
                l--;h++;
            }
            
            // For odd length palindrom...
            l=i-1;
            h=i+1;
            while(l>=0 && h<n && s[l]==s[h]){
                if(h-l+1>len){
                    start=l;
                    len=h-l+1;
                }
                l--;h++;
            }
        }
        return s.substr(start,len);
    }
```

---

## 18. 520. Detect Capital

**Q:** Given a string `word`, return true if the usage of capitals in it is valid (all caps, all lowercase, or only first letter capitalized).

**A:** String parsing / simulation.

**Explanation:**
- Count the total number of uppercase letters.
- The string is valid if there are no uppercase letters, if all letters are uppercase, or if exactly one letter is uppercase and it is the first letter.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the string length.

```cpp
    bool detectCapitalUse(string word) {
        int count=0;
        if(word.size()==1) return true;
        for(int i=0; i<word.size(); i++)
            if(isupper(word[i])) count++;
        if(count==1 && isupper(word[0])) return true;
        if(count==0 || count==word.size()) return true;
        else return false;
    }
```

---

## 19. 647. Palindromic Substrings

**Q:** Given a string `s`, return the total number of palindromic substrings in it.

**A:** Palindrome expansion / Dynamic Programming.

**Explanation:**
- **Two Pointer:** Expand outward from each possible center (both characters and gaps between characters), incrementing the count for each palindrome found.
- **DP:** Maintain a boolean table to memoize whether `s[i...j]` is a palindrome by checking if ends match and the inner substring is a palindrome.

**Complexity:** Time `O(n^2)` for two pointer, `O(n^2)` for DP. Space `O(1)` for two pointer, `O(n^2)` for DP.

```cpp
// Two Pointer...
    int countSubstrings(string s) {
        int n=s.size();
        int ans=0;
        
        for(int i=0;i<n;i++){
            int x=i-1,y=i+1;
            ans++;
            while(x>=0 and y<n and s[x]==s[y]){
                ans++;
                x--;
                y++;
            }
             x=i,y=i+1;
            while(x>=0 and y<n and s[y]== s[x]){
                ans++;
                x--;
                y++;
            }
        }
        return ans;
    }
   
// DP...
    int isP(int i,int j,string &s,vector<vector<int>> &dp){
        if(i>=j) return 1;
        if(dp[i][j]!=-1) return dp[i][j];
        if(s[i]!=s[j]) return dp[i][j]=0;
        return dp[i][j]=isP(i+1,j-1,s,dp);
    }
    
    int countSubstrings(string s) {
        int n=s.size();
        vector<vector<int>> dp(n,vector<int>(n,-1));
        int ans=0;
        for(int i=0;i<n;i++) {
            for(int j=i+1;j<n;j++) {
                if(isP(i,j,s,dp)) ans++;
            }
        }
        return ans+n;
    }
```

---

## 20. 67. Add Binary

**Q:** Given two binary strings `a` and `b`, return their sum as a binary string.

**A:** Two pointers / String simulation.

**Explanation:**
- Use pointers iterating from the end of each string to process from right to left.
- Compute the sum of the digits and the current carry.
- Append the remainder to the answer string and update the carry, then reverse the result at the end.

**Complexity:** Time `O(\max(n, m))`, Space `O(\max(n, m))` where `n` and `m` are the lengths of `a` and `b`.

```cpp
    string addBinary(string a, string b) {
        int i=a.size()-1,j=b.size()-1;
        int carry=0;
        string ans="";
        while(i>=0 || j>=0){
            int sum=carry;
            sum+= i<0 ? 0 : a[i--]-'0';
            sum+= j<0 ? 0 : b[j--]-'0';
            carry= sum>1 ? 1 : 0;
            ans+=to_string(sum%2);
        }
        if(carry) ans+=to_string(carry);
        reverse(ans.begin(),ans.end());
        return ans;
    }
```

---

## 21. 792. Number of Matching Subsequences

**Q:** Given a string `s` and an array of strings `words`, return the number of strings in `words` that are a subsequence of `s`.

**A:** Hashing / Two Pointers.

**Explanation:**
- **Brute Force:** Traverse `s` and `t` simultaneously for each word. (Causes TLE).
- **Hashing:** Avoid duplicate work by caching previously checked words in a hash map.

**Complexity:** Time `O(w * (\max(|s|, |t|)))` where `w` is the number of unique words, Space `O(w)` for the hash map.

```cpp
// Brute Force...(TLE)
    bool isS(string &s,string &t){
        int ss=s.size(),ts=t.size();
        int i=0,j=0;
        while(i<ss && j<ts){
            if(s[i]==t[j]) j++;
            i++;
        }
        if(j==ts) return true;
        return false;
    }
    int numMatchingSubseq(string s, vector<string>& words) {
        int ans=0;
        for(auto it : words){
            if(isS(s,it)) ans++;
        }
        return ans;
    }
    
// Using Hashing...
    bool isS(string &s,string &t,unordered_map<string,bool> &mp){
        int ss=s.size(),ts=t.size();
        int i=0,j=0;
        while(i<ss && j<ts){
            if(s[i]==t[j]) j++;
            i++;
        }
        mp[t]=(j==ts);
        return j==ts;
    }
    int numMatchingSubseq(string s, vector<string>& words) {
        int ans=0;
        unordered_map<string,bool> mp;
        for(auto it : words){
            if(mp.find(it)==mp.end()){
                if(isS(s,it,mp)) ans++;
            }else{
                if(mp[it]) ans++;
            }
        }
        return ans;
    }
```

---

## 22. 8. String to Integer (atoi)

**Q:** Given a string `s`, convert it to a 32-bit signed integer.

**A:** String parsing.

**Explanation:**
- Skip leading whitespace.
- Check for and record an optional sign.
- Parse digits into an integer, checking boundaries to clamp values at `INT_MAX` or `INT_MIN` before overflow occurs.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the string length.

```cpp
    int myAtoi(string s) {
        int n = s.length();
        int idx = 0;
        if(n == 0){
            return 0;
        } 
        while(idx < n && s[idx] == ' '){
            idx++;
        }
        if(idx == n){
            return 0;
        } 
        bool isNegative = false;
        if(s[idx] == '-'){
            isNegative = true;
            idx++;
        }else if(s[idx] == '+'){
            idx++;
        }
        int result = 0;
        while(idx < n && isdigit(s[idx])){
            int digit = s[idx] - '0';
            if(result > INT_MAX / 10 || (result == INT_MAX /10 && digit > 7) ){
                return isNegative ? INT_MIN : INT_MAX;
            }
            result = result*10 + digit;
            idx++;
        }

        return isNegative ? -result : result;
    }
```

---

## 23. 890. Find and Replace Pattern

**Q:** Given a list of strings `words` and a string `pattern`, return all strings in `words` that match the given `pattern`.

**A:** Hashing (Two-way mapping).

**Explanation:**
- To ensure a valid bijection, map characters from the word to the pattern and from the pattern to the word.
- Iterate through each word, checking against the pattern. If both mappings are consistent, the word matches.

**Complexity:** Time `O(n * m)`, Space `O(1)` (for 26 letters) where `n` is the number of words and `m` is the pattern length.

```cpp
    bool match(string &w,string &p){
        int n=p.size();
        unordered_map<char,char> mw,mp;
        for(int i=0;i<n;i++){
            if(mw.find(w[i])==mw.end()) mw[w[i]]=p[i];
            if(mp.find(p[i])==mp.end()) mp[p[i]]=w[i];
            if(mw[w[i]]!=p[i] || mp[p[i]]!=w[i]) return 0;
        }
        return 1;
    }
    vector<string> findAndReplacePattern(vector<string>& words, string pattern) {
        vector<string> ans;
        for(auto word : words){
            if(match(word,pattern)){
                ans.push_back(word);
            }
        }
        return ans;
    }
```

---

## 24. 916. Word Subsets

**Q:** Given two string arrays `words1` and `words2`, return all universal strings in `words1` that contain every string in `words2` as a subset.

**A:** Frequency/count array.

**Explanation:**
- Compute the maximum required frequency of each character across all words in `words2`.
- For each word in `words1`, calculate its character frequencies.
- Include the word in the result only if it meets or exceeds the required frequency for all characters.

**Complexity:** Time `O(n + m)`, Space `O(1)` where `n` and `m` are total characters across `words1` and `words2`.

```cpp
    vector<int> getFrq(string &s){
        vector<int> fq(26,0);
        for(auto it : s) fq[it-'a']++;
        return fq;
    }
    vector<string> wordSubsets(vector<string>& words1, vector<string>& words2) {
        vector<int> mfqw2(26,0);
        for(auto it : words2){
            vector<int> temp=getFrq(it);
            for(int i=0;i<26;i++){
                mfqw2[i]=max(mfqw2[i],temp[i]);
            }
        }
        
        vector<string> ans;
        for(auto it : words1){
            vector<int> temp=getFrq(it);
            bool flag=true;
            for(int i=0;i<26;i++){
                if(temp[i]<mfqw2[i]){
                    flag=false;
                    break;
                }
            }
            if(flag) ans.push_back(it);
        }
        return ans;
    }
```

---

## 25. 97. Interleaving String

**Q:** Given strings `s1`, `s2`, and `s3`, find whether `s3` is formed by an interleaving of `s1` and `s2`.

**A:** Dynamic Programming.

**Explanation:**
- Early exit if the combined lengths of `s1` and `s2` do not equal `s3`.
- Recursively attempt to match `s3[k]` with either `s1[i]` or `s2[j]`.
- Memoize the result based on the indices of the three strings to avoid redundant checks.

**Complexity:** Time `O(l * m * n)`, Space `O(l * m * n)` where `l, m, n` are the lengths of the strings (optimizable down to `O(l * m)`).

```cpp
    bool solve(int i,int j,int k,string &s1,string &s2,string &s3,vector<vector<vector<int>>> &dp){
        if(i==0 && j==0 && k==0) return 1;
        if(dp[i][j][k]!=-1) return dp[i][j][k];
        if(i>0 && j>0 && k>0 && s1[i-1]==s3[k-1] && s2[j-1]==s3[k-1]) return dp[i][j][k]=solve(i-1,j,k-1,s1,s2,s3,dp)||solve(i,j-1,k-1,s1,s2,s3,dp);
        if(i>0 && k>0 && s1[i-1]==s3[k-1]) return dp[i][j][k]=solve(i-1,j,k-1,s1,s2,s3,dp);
        if(j>0 && k>0 && s2[j-1]==s3[k-1]) return dp[i][j][k]=solve(i,j-1,k-1,s1,s2,s3,dp);
        return dp[i][j][k]=0;        
    }
    
    bool isInterleave(string s1, string s2, string s3) {
        int l=s1.size(),m=s2.size(),n=s3.size();
        if(l+m!=n) return 0;
        vector<vector<vector<int>>> dp(l+1,vector<vector<int>>(m+1,vector<int>(n+1,-1)));
        return solve(l,m,n,s1,s2,s3,dp);
    }
```

---

## 26. Common Subsequence

**Q:** Given two strings, return true if they share a common subsequence of length at least 1.

**A:** Hashing (Set/Map).

**Explanation:**
- Store the frequency of all characters in the first string using a hash map.
- Check each character of the second string to see if it exists in the map.

**Complexity:** Time `O(n + m)`, Space `O(1)` (at most 26 lowercase letters) where `n` and `m` are string lengths.

```cpp
        bool commonSubseq (string a, string b){
            unordered_map<char,int> mp;
            for(auto it : a) mp[it]++;
            for(auto it : b) if(mp.find(it)!=mp.end()) return 1;
            return 0;
        }
  
  Link - https://practice.geeksforgeeks.org/problems/common-subsequence4049/1
```

---

## 27. Longest String Chain

**Q:** Given an array of words, return the length of the longest possible word chain where each step removes exactly one character.

**A:** Dynamic Programming / Hashing.

**Explanation:**
- Store words in a hash map for `O(1)` lookup.
- For each word, recursively delete one character at a time and search for the resulting string in the map.
- Memoize the longest chain length extending from each string.

**Complexity:** Time `O(n * l^2)`, Space `O(n)` where `n` is the number of words and `l` is maximum word length.

```cpp
    int solve(string s,unordered_map<string,int> &mp){
        if(s.size()<=0) return 0;
        if(mp[s]>1) return mp[s];
        int ans=0;
        for(int i=0;i<s.size();i++){
            string tmp=s;
            tmp.erase(i,1);
            if(mp.find(tmp)!=mp.end()){
                ans=max(ans,1+solve(tmp,mp));
            }
        }
        return mp[s]=ans;
    }
  
    int longestStringChain(vector<string>& words) {
        unordered_map<string,int> mp;
        for(auto it : words) mp[it]=1;
        int res=0;
        for(auto it : words) res=max(res,solve(it,mp));
        return res+1;
    }

gfg link - https://www.geeksforgeeks.org/problems/longest-string-chain/1
```

---

## 28. Minimum characters to be added at front to make string palindrome

**Q:** Given a string, return the minimum number of characters that need to be added to its front to make it a palindrome.

**A:** KMP / prefix function.

**Explanation:**
- **Brute Force:** Try erasing characters from the end of the string until the remaining string is a palindrome.
- **KMP:** Append a separator and the reversed string to the original string. Compute the longest prefix-suffix (LPS) array on this new string. The result is the original length minus the last LPS value.

**Complexity:** Time `O(n^2)` for brute force, `O(n)` for KMP. Space `O(1)` for brute force, `O(n)` for KMP.

```cpp
// BrutForce...
    bool isP(string &s){
        int i=0,j=s.size()-1;
        while(i<j){
            if(s[i]!=s[j]) return false;
            i++;j--;
        }
        return true;
    }
    int minChar(string str){
        int n=str.size();
        int ans=0;
        while(str.size()>1){
            if(isP(str)){
                return ans;
            }else{
                str.erase(str.end()-1);
                ans++;
            }
        }
        return ans;
    }
    
// Using KMP...
    vector<int> lps(string &s){
        int n=s.size();
        vector<int> ps(n,0);
        int i=0,j=1;
        while(j<n){
            if(s[i]==s[j]){
                ps[j]=i+1;
                i++;j++;
            }else{
                if(i!=0) i=ps[i-1];
                else{
                    ps[j]=0;
                    j++;
                }
            }
        }
        return ps;
    }
        
    int minChar(string str){
        string s="";
        s.append(str);
        s.push_back('*');
        reverse(str.begin(),str.end());
        s.append(str);
        vector<int> ps=lps(s);
        return str.size()-ps[s.size()-1];
    }
    
    Link - https://practice.geeksforgeeks.org/problems/minimum-characters-to-be-added-at-front-to-make-string-palindrome/1/#
```

---

## 29. Nearest multiple of 10

**Q:** Given a string representing a number, round it to the nearest multiple of 10 and return the updated string.

**A:** String simulation.

**Explanation:**
- Check the last digit. If it is 5 or less, round down by setting it to 0.
- Otherwise, round up: set it to 0 and propagate a carry leftward over all 9s, setting them to 0.
- If a carry propagates past the first digit, prepend '1'.

**Complexity:** Time `O(n)`, Space `O(1)` where `n` is the length of the string.

```cpp
    string roundToNearest(string str) {
        int n=str.length();
        if(str[n-1]<='5'){
            str[n-1]='0';
            return str;
        }
        str[n-1]='0';
        int i=n-2;
        while(i>=0 && str[i]=='9'){
            str[i]='0';
            i--;
        }
        if(i<0) return '1'+str;
        else str[i]++;
        return str;
    }

gfg link - https://www.geeksforgeeks.org/problems/nearest-multiple-of-102437/1
```

---

## 30. Possible Words From Phone Digits

**Q:** Given an array representing keypad digits, return all possible letter combinations that those digits could represent.

**A:** Breadth-First Search / Iterative combinations.

**Explanation:**
- Map each digit to its keypad characters.
- Start with the letters of the first digit.
- Iteratively append every character of the next digit to all combinations generated so far. Finally, sort the strings.

**Complexity:** Time `O(4^n \log(4^n))` due to sorting, Space `O(4^n)` where `n` is the number of digits.

```cpp
    vector<string> possibleWords(int a[], int N){
        vector<string> prev;
        string key[]={"","","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
        for(auto it : key[a[0]]){
            string t="";
            t.push_back(it);
            prev.push_back(t);
        }
        if(N==1) return prev;
        for(int i=1;i<N;i++){
            vector<string> cur;
            for(auto it : key[a[i]]){
                for(auto it2 : prev){
                    string t=it2;
                    t.push_back(it);
                    cur.push_back(t);
                }
            }
            prev=cur;
            
        }
        sort(prev.begin(),prev.end());
        return prev;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/possible-words-from-phone-digits-1587115620/1/
```

---

## 31. Rearrange characters

**Q:** Given a string, rearrange the characters so that no two adjacent characters are identical, returning the string or "-1" if impossible.

**A:** Priority Queue / Greedy.

**Explanation:**
- Count character frequencies. If any frequency exceeds `ceil(n/2)`, returning a valid arrangement is impossible.
- Store frequencies and characters in a max heap (priority queue).
- Greedily pop the most frequent character, append it to the answer, and hold it back for one iteration before pushing it back into the heap.

**Complexity:** Time `O(n \log k)`, Space `O(k)` where `n` is string length and `k` is distinct characters (at most 26).

```cpp
    string rearrangeString(string str){
        //code here
        int n=str.size();
        unordered_map<char,int> mp;
        int mx=0;
        for(auto it : str){
            mp[it]++;
            mx=max(mx,mp[it]);
        }
        
        if(mx>ceil((float)n/2)) return "-1";
        
        priority_queue<pair<int,char>> pq;
        for(auto it : mp){
            pq.push({it.second,it.first});
        }
        string ans="";
        pair<int,char> prev={-1,'$'};
        while(!pq.empty()){
            auto cur=pq.top();pq.pop();
            ans.push_back(cur.second);
            if(prev.first>0) pq.push({prev.first,prev.second});
            cur.first--;
            prev=cur;
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/rearrange-characters4649/1
```

---

## 32. Search Pattern (KMP-Algorithm)

**Q:** Given a pattern and a text string, return all starting indices of the pattern's occurrences in the text.

**A:** KMP / prefix function.

**Explanation:**
- Compute the longest prefix-suffix (LPS) array for the pattern to track matching prefixes.
- Compare pattern against text. On mismatch, use the LPS array to skip unnecessary comparisons without moving the text pointer backward.
- When a match is fully found, record the index and reset the pattern pointer using the LPS array.

**Complexity:** Time `O(n + m)`, Space `O(m)` where `n` is text length and `m` is pattern length.

```cpp
    vector<int> lps(string &s){
        int n=s.size();
        vector<int> ps(n,0);
        int i=0,j=1;
        while(j<n){
            if(s[i]==s[j]){
                ps[j]=i+1;
                i++;j++;
            }else{
                if(i!=0) i=ps[i-1];
                else{
                    ps[j]=0;
                    j++;
                }
            }
        }
        return ps;
    }

    vector <int> search(string pat, string txt){
        vector<int> ans,ps=lps(pat);
        int pl=pat.size(),tl=txt.size();
        int i=0,j=0;
        while(i<pl && j<tl){
            if(pat[i]==txt[j]){
                i++;j++;
            }else{
                if(i!=0) i=ps[i-1];
                else j++;
            }
            if(i==pl){
                ans.push_back(j-pl+1);
                i=ps[i-1];
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/search-pattern0205/1#
```

---

## 33. Search Pattern (Rabin-Karp Algorithm)

**Q:** Given a pattern and a text string, return all 1-based starting indices of occurrences using a hashing strategy.

**A:** String hashing (Rabin-Karp).

**Explanation:**
- Precompute powers of the base (31) modulo `1e9+7`.
- Calculate the prefix hashes for the text and the full hash of the pattern.
- For each substring of the length of the pattern in the text, extract its hash from the prefix hashes and compare it against the pattern's hash multiplied by the power offset.

**Complexity:** Time `O(n + m)`, Space `O(n)` where `n` is text length and `m` is pattern length.

```cpp
    int p=31;
    int N=1e5+7,m=1e9+7;
    
    vector <int> search(string pat, string txt){
        vector<int> ans;
        int T=txt.size(),S=pat.size();
        vector<long long>power(N);
        power[0]=1;
        for(int i=1;i<N;i++){
            power[i]=(power[i-1]*p)%m;
        }
    
        vector<long long> h(T+1,0);
        for(int i=0;i<T;i++){
            h[i+1]=(h[i]+(txt[i]-'a'+1)*power[i])%m;
        }
    
        long long h_s=0;
        for(int i=0;i<S;i++){
            h_s=(h_s+(pat[i]-'a'+1)*power[i])%m;
        }
        for(int i=0;i+S-1<T;i++){
            long long cur_h=(h[i+S]-h[i]+m)%m;
            if(cur_h==(h_s*power[i])%m){
                ans.push_back(i+1);
            }
        }
        if(ans.size()==0) return {-1};
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/31272eef104840f7430ad9fd1d43b434a4b9596b/1/
```

---

## 34. Sum of two large numbers

**Q:** Given two large numbers represented as strings, calculate and return their sum as a string without leading zeros.

**A:** Two pointers / String simulation.

**Explanation:**
- Skip any leading zeros for both string representations.
- Traverse the strings from right to left using two pointers.
- Add the corresponding digits along with any carry, appending the resulting unit digit to the answer string. Finish by reversing the final answer string.

**Complexity:** Time `O(\max(n, m))`, Space `O(\max(n, m))` where `n` and `m` are the lengths of the strings.

```cpp
    string findSum(string X, string Y) {
        // Your code goes here
        int n=X.size(),m=Y.size();
        int i=n-1,j=m-1;
        int k=0,l=0;
        while(k<n && X[k]=='0') k++;
        while(l<m && Y[l]=='0') l++;
        if(k==n && l==m) return "0";
        int carry=0;
        string ans="";
        while(i>=k || j>=l){
            int d1= i>=k ? X[i]-'0' : 0;
            int d2= j>=l ? Y[j]-'0' : 0;
            int sum=d1+d2+carry;
            int res=sum%10;
            carry=sum/10;
            i--;j--;
            ans.push_back('0'+res);
        }
        if(carry){
            ans.push_back('0'+carry);
        }
        reverse(ans.begin(),ans.end());
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/sum-of-numbers-or-number1219/1
```
