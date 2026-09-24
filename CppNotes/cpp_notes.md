# C AND C++ — STRUCTURED NOTES

Rewritten, de-duplicated and expanded from `cpp.txt`.
Every code block compiles as-is unless explicitly marked `// ERROR` or `// UB`.

```
Compile examples with:
    g++ -std=c++20 -Wall -Wextra -pedantic file.cpp -o app
    gcc -std=c17   -Wall -Wextra -pedantic file.c   -o app
```

---

## TABLE OF CONTENTS

**Part I — Foundations**
1. [Compilation Model](#1-compilation-model)
2. [Preprocessor and Include Guards](#2-preprocessor-and-include-guards)
3. [Linking: Static vs Dynamic](#3-linking-static-vs-dynamic)
4. [Memory Layout of a Process](#4-memory-layout-of-a-process)
5. [Storage Classes and Keywords](#5-storage-classes-and-keywords)
6. [const, constexpr, volatile, mutable](#6-const-constexpr-volatile-mutable)
7. [Pointers, References and Arrays](#7-pointers-references-and-arrays)
8. [Dynamic Memory](#8-dynamic-memory)
9. [Type Casting](#9-type-casting)
10. [Struct/Class Padding and sizeof](#10-structclass-padding-and-sizeof)
11. [Bitwise Operators and Bitfields](#11-bitwise-operators-and-bitfields)
12. [Endianness](#12-endianness)

**Part II — Object Oriented C++**
13. [OOP Pillars](#13-oop-pillars)
14. [Constructors and Destructors](#14-constructors-and-destructors)
15. [Copy Semantics: Shallow vs Deep](#15-copy-semantics-shallow-vs-deep)
16. [Inheritance](#16-inheritance)
17. [Polymorphism, vtable and vptr](#17-polymorphism-vtable-and-vptr)
18. [Object Slicing and Function Hiding](#18-object-slicing-and-function-hiding)
19. [Operator Overloading](#19-operator-overloading)
20. [Functors](#20-functors)
21. [friend, static members, this](#21-friend-static-members-this)

**Part III — Modern C++**
22. [lvalue/rvalue and Move Semantics](#22-lvaluervalue-and-move-semantics)
23. [Rule of 0/3/5, Copy Elision](#23-rule-of-035-copy-elision)
24. [Smart Pointers and RAII](#24-smart-pointers-and-raii)
25. [Lambdas and std::function](#25-lambdas-and-stdfunction)
26. [Templates and Generic Programming](#26-templates-and-generic-programming)
27. [Initialization Rules](#27-initialization-rules)
28. [Standard Version Feature Map](#28-standard-version-feature-map)

**Part IV — STL**
29. [Containers and Complexity](#29-containers-and-complexity)
30. [Iterators](#30-iterators)
31. [map / unordered_map Internals](#31-map--unordered_map-internals)
32. [Strings and Character Arrays](#32-strings-and-character-arrays)
33. [Algorithms Cheat Sheet](#33-algorithms-cheat-sheet)

**Part V — Systems and Runtime**
34. [Exception Handling and Stack Unwinding](#34-exception-handling-and-stack-unwinding)
35. [Multithreading](#35-multithreading)
36. [File Handling](#36-file-handling)
37. [Design Patterns](#37-design-patterns)
38. [Complexity Notation and Sorting](#38-complexity-notation-and-sorting)
39. [C vs C++](#39-c-vs-c)
40. [Gotchas, UB and Quick-Fire Answers](#40-gotchas-ub-and-quick-fire-answers)

---
---

# PART I — FOUNDATIONS

## 1. Compilation Model

Four stages from `.cpp` to executable:

| Stage | Tool | Input | Output | What happens |
|---|---|---|---|---|
| 1. Preprocessing | `cpp` | `.cpp` | `.i` | `#include` pasted, macros expanded, `#if/#ifdef` resolved, comments stripped |
| 2. Compilation | `cc1plus` | `.i` | `.s` | Parse, semantic analysis, optimization, emit assembly |
| 3. Assembly | `as` | `.s` | `.o` | Assembly to machine code (relocatable object) |
| 4. Linking | `ld` | `.o` + libs | binary | Resolve symbols, relocate, produce executable / shared object |

### Useful gcc/g++ flags

```
-o <name>     name the output binary
-E            stop after preprocessing (dump .i)
-S            stop after compilation (dump .s)
-c            compile+assemble only, no link (dump .o)
-Wall         enable common warnings
-Wextra       enable additional warnings (-Wall does not imply this)
-Werror       treat warnings as errors
-w            disable ALL warnings (avoid)
-pedantic     warn on non-standard extensions
-g            emit debug symbols (needed by gdb)
-O0/-O1/-O2/-O3/-Os   optimization levels
-std=c++20    language standard
-l<name>      link library (e.g. -lpthread, -lm)
-L<dir>       add library search path
-I<dir>       add header search path
-fsanitize=address,undefined   ASan/UBSan runtime checks
-v            verbose: show each internal pass
-MMD -MP      emit header dependency files for make
```

### Inspecting each stage

```bash
g++ -E main.cpp -o main.i     # preprocessed
g++ -S main.cpp -o main.s     # assembly
g++ -c main.cpp -o main.o     # object
nm -C main.o                  # list symbols, demangled
objdump -d main.o             # disassemble
readelf -d ./app              # dynamic section (NEEDED libs)
ldd ./app                     # resolved shared libraries
```

### Name mangling

The compiler encodes the function's namespace, class and parameter types into the
symbol name so that overloads become distinct symbols at link time.

```cpp
int  fx(int, int);     // -> _Z2fxii
int  fx(float, int);   // -> _Z2fxfi
```

C has no overloading, therefore no mangling. To call C from C++ (or export a C++
function to C), suppress mangling:

```cpp
extern "C" void c_api(int);       // single declaration

extern "C" {                      // block form, typical in headers
    #include <some_c_header.h>
    void init(void);
    void shutdown(void);
}
```

Standard idiom for a dual-use header:

```c
#ifdef __cplusplus
extern "C" {
#endif

void my_c_function(int);

#ifdef __cplusplus
}
#endif
```

Demangle by hand: `c++filt _Z2fxii` -> `fx(int, int)`.

---

## 2. Preprocessor and Include Guards

The same header can be pulled in twice (directly or transitively), causing
redefinition errors. Guard it.

```cpp
// MyHeader.h
#ifndef MYHEADER_H
#define MYHEADER_H

class MyClass {
public:
    void print();
};

#endif // MYHEADER_H
```

`#pragma once` is a non-standard but universally supported one-liner alternative:

```cpp
#pragma once
```

| | `#ifndef` guard | `#pragma once` |
|---|---|---|
| Standard | Yes | No (de-facto everywhere) |
| Risk | Macro name collision | Confused by hardlinks/symlinks on odd filesystems |
| Speed | Marginally slower | Compiler can skip re-open |

### Conditional compilation

```cpp
#ifdef DEBUG
  #define LOG(x) std::cout << x << std::endl
#else
  #define LOG(x) ((void)0)      // no-op that still needs a semicolon
#endif

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif
```

### Macro hygiene

```cpp
#define SQUARE(x)      x * x          // BAD: SQUARE(1+2) -> 1+2*1+2 == 5
#define SQUARE_OK(x)   ((x) * (x))    // BETTER, but still double-evaluates
constexpr int square(int x) { return x * x; }   // BEST in C++
```

Multi-statement macros must use `do { ... } while(0)`:

```cpp
#define SWAP(a, b) do { int _t = (a); (a) = (b); (b) = _t; } while (0)
```

### Predefined macros

```cpp
__FILE__       // "main.cpp"
__LINE__       // 42
__func__       // "main"          (standard, C99/C++11)
__FUNCTION__   // "main"          (compiler extension, widely available)
__PRETTY_FUNCTION__  // full signature, GCC/Clang
__DATE__ __TIME__
__cplusplus    // 202002L for C++20
```

Handy trace macro:

```cpp
#define TRACE() std::cout << __FILE__ << ':' << __LINE__ \
                          << ' ' << __func__ << '\n'
```

---

## 3. Linking: Static vs Dynamic

### Static linking

Library code is copied into the executable at link time.

```bash
g++ main.cpp libmath.a -o app      # or: g++ -static main.cpp -o app
ar rcs libmath.a a.o b.o           # create a static archive
```

Pros: no runtime dependency, faster startup (no symbol resolution), single
deployable file.
Cons: large binary, library duplicated in every process, must relink the whole
program to patch the library.

### Dynamic linking

Executable stores only *references*; the loader (`ld.so`) maps the shared object
at runtime and binds symbols through the PLT/GOT.

```bash
g++ -fPIC -shared a.cpp b.cpp -o libmath.so     # build shared object
g++ main.cpp -L. -lmath -o app                  # link against it
LD_LIBRARY_PATH=. ./app                         # run
```

Pros: small binary, one copy in physical memory shared across processes,
library patchable independently, enables plugins (`dlopen`).
Cons: must be present at runtime, version conflicts ("DLL hell"), slight startup
cost, symbol interposition surprises.

### Lazy vs eager binding

By default symbols resolve on first call (lazy, via PLT stub). Force eager
resolution with `LD_BIND_NOW=1` or link with `-Wl,-z,now`.

### Loading a plugin at runtime

```cpp
#include <dlfcn.h>
void* h = dlopen("./libplugin.so", RTLD_LAZY);
auto  f = reinterpret_cast<void(*)()>(dlsym(h, "plugin_entry"));
if (f) f();
dlclose(h);
```

---

## 4. Memory Layout of a Process

```
  high address
  +--------------------+
  | command line, env  |
  +--------------------+
  |       STACK        |  local vars, params, return addrs, saved regs
  |         |          |  grows DOWN, LIFO, per-thread
  |         v          |
  +--------------------+
  |        ...         |  unmapped guard region
  +--------------------+
  |         ^          |
  |         |          |
  |        HEAP        |  new/malloc, grows UP, manual lifetime
  +--------------------+
  |        BSS         |  uninitialized globals/statics  -> zero-filled
  +--------------------+
  |    DATA (.data)    |  initialized globals/statics
  +--------------------+
  |   RODATA (.rodata) |  string literals, const globals  (read-only)
  +--------------------+
  |    TEXT (.text)    |  machine code (read-only, executable)
  +--------------------+
  low address
```

| Region | Contents | Lifetime | Who frees |
|---|---|---|---|
| `.text` | code | whole program | OS |
| `.rodata` | literals, `const` globals | whole program | OS |
| `.data` | initialized globals/statics | whole program | OS |
| `.bss` | zero-init globals/statics (occupies no file space) | whole program | OS |
| heap | `new` / `malloc` | until freed | you |
| stack | locals, params | until scope exits | compiler |

```cpp
int   g_init   = 5;         // .data
int   g_uninit;             // .bss  (zero-initialized)
const char* s = "hello";    // pointer in .data, "hello" in .rodata
static int  fileLocal = 1;  // .data, internal linkage

void f() {
    int local;              // stack (indeterminate value)
    static int cnt = 0;     // .data — persists across calls
    int* p = new int(7);    // pointer on stack, 7 on heap
    delete p;
}
```

String literals are read-only. Writing to one is undefined behaviour:

```cpp
char* p = (char*)"hello";
p[0] = 'H';                 // UB — segfault in practice
char arr[] = "hello";       // OK — copy lives on the stack
arr[0] = 'H';
```

### Stack frame anatomy

A call pushes: arguments (or passes them in registers), return address, saved
frame pointer, then allocates space for locals. Return pops it all. This is why
returning a pointer to a local is a dangling pointer.

```cpp
int* bad()  { int x = 5; return &x; }        // UB — x dies at return
int* ok()   { return new int(5); }           // caller must delete
```

---

## 5. Storage Classes and Keywords

| Specifier | Scope | Lifetime | Purpose |
|---|---|---|---|
| `auto` (C) | block | until block exits | default for locals; in C++11 means type deduction |
| `register` | block | until block exits | hint to keep in CPU register (deprecated C++17) |
| `static` | block or file | whole program | retains value between calls; internal linkage at file scope |
| `extern` | global | whole program | refers to a definition in another TU |
| `mutable` | class member | object lifetime | modifiable even from a `const` member function |
| `thread_local` | block or global | thread lifetime | one instance per thread |

### static — three distinct meanings

```cpp
// 1) static local variable: initialized once, lives in .data/.bss
int counter() {
    static int n = 0;       // thread-safe init since C++11
    return ++n;
}

// 2) static member variable: one shared copy for all objects
class A {
public:
    int a;
    static int b;           // declaration only
};
int A::b = 10;              // definition, required, outside the class
// sizeof(A) == 4 — static members do NOT live inside the object

// 3) static member function: no `this`, callable without an object
class B {
public:
    static void hi() { std::cout << "hi\n"; }  // can touch only statics
};
B::hi();
```

A static member function has no `this`, so it cannot read non-static members
directly — unless you hand it an object.

### extern

```cpp
// a.cpp
int shared = 42;            // definition

// b.cpp
extern int shared;          // declaration — no storage allocated
void use() { std::cout << shared; }
```

If nothing defines it, the *linker* (not the compiler) errors:
`undefined reference to 'shared'`.

You may declare `extern` many times; you may define it once (ODR).
Initializing an `extern` inside a function is illegal:

```cpp
extern int c = 10;          // OK at file scope (this is a definition)
void f() { extern int a = 10; }   // ERROR
```

### thread_local

```cpp
thread_local int tls_counter = 0;   // each thread gets its own copy
```

### mutable

```cpp
class Cache {
    mutable int hits = 0;           // bookkeeping, not logical state
    int value = 7;
public:
    int get() const { ++hits; return value; }   // legal because of mutable
};
```

### register

Hint only; compilers ignore it. In C you cannot take the address of a register
variable; C++ allows it. Deprecated in C++11, removed as a storage class in C++17.

---

## 6. const, constexpr, volatile, mutable

### Every meaning of `const`

```cpp
const int  v = 10;              // 1) constant object — must be initialized

int* const p  = &x;             // 2) const POINTER: p cannot be re-pointed
const int* q  = &x;             // 3) pointer to const: *q cannot be written
int const* q2 = &x;             //    identical to (3)
const int* const r = &x;        // 4) const pointer to const

void f(const int a);            // 5) parameter is read-only inside f
void g(const std::string& s);   //    the common, useful form: no copy, no write

class C {
    int  n = 0;
    mutable int hits = 0;
public:
    int get() const { ++hits; return n; }   // 6) const member function
    const int limit = 5;                    // 7) const data member
};

const C obj;                    // 8) const object — only const members callable
```

Read pointer declarations right-to-left: `int * const p` = "p is a const pointer
to int"; `const int * p` = "p is a pointer to const int".

### Initializing const data members

```cpp
class A {
    static const int a = 10;    // 1) integral static const: init in-class
};

class B {
    static const int a;         // 2) declare here...
};
const int B::a = 10;            //    ...define outside

class C {
    const int b;
    int& ref;
public:
    C(int c, int& r) : b(c), ref(r) {}   // 3) member initializer list
};

class D {
    const int b = 7;            // 4) C++11 default member initializer
};
```

**The initializer list is mandatory for:**
1. `const` data members
2. reference data members
3. members without a default constructor
4. base-class subobjects when the base has no default constructor

Also note: members are initialized in *declaration order*, not in the order
written in the initializer list.

```cpp
class E {
    int a, b;
public:
    E(int x) : b(x), a(b) {}    // BUG: a initialized FIRST, b is still garbage
};
```

### constexpr

`const` = read-only. `constexpr` = known at compile time.

```cpp
constexpr int x = 10;           // compile-time constant
const     int y = getRuntime(); // legal: read-only, computed at runtime
// constexpr int z = getRuntime();  // ERROR

constexpr int square(int n) { return n * n; }

int main() {
    constexpr int s = square(5);    // evaluated at compile time
    int arr[s];                     // OK: s is a constant expression
    int n; std::cin >> n;
    int r = square(n);              // same function, runtime evaluation
}
```

C++11 restricted `constexpr` functions to a single `return`. C++14 allows loops,
locals and branches. C++17 adds `if constexpr`. C++20 adds `consteval`
(must be compile-time) and `constinit` (compile-time init, mutable after).

```cpp
consteval int must(int n) { return n * 2; }  // C++20: compile-time only
constinit int g = square(4);                 // C++20: no static-init-order fiasco
```

### volatile

Tells the compiler the value may change outside the program's control
(memory-mapped hardware register, signal handler). Suppresses caching the value
in a register and suppresses reordering of the access.

```cpp
volatile uint32_t* status = reinterpret_cast<uint32_t*>(0xDEADBEEF);
while (*status == 0) { }        // must actually re-read memory each iteration
```

`volatile` is **not** a threading primitive. It gives no atomicity and no
memory-ordering guarantee between threads. Use `std::atomic` for that.

---

## 7. Pointers, References and Arrays

### Pointer vs reference

| | Pointer | Reference |
|---|---|---|
| Can be null | Yes | No |
| Re-seatable | Yes | No, bound once at init |
| Must be initialized | No | Yes |
| Arithmetic | Yes | No |
| Levels of indirection | Many (`int**`) | One |
| `sizeof` | pointer size | size of referent |

**Use a reference or pointer instead of a value when:**
1. Passing a large object (avoid the copy)
2. Avoiding object slicing
3. Modifying the caller's variable
4. Achieving runtime polymorphism

**Reference only (not pointer):**
- Copy constructor parameter (`T(const T&)`) — a by-value parameter would recurse
  infinitely.
- Operator overloads that must support chaining.

**Pointer only (not reference):**
- Node-based data structures (linked list, tree, graph) that need a null sentinel.
- Anything that must be re-pointed after construction.
- Optional / owning semantics, C APIs.

### Pointer arithmetic

```cpp
int arr[5] = {1,2,3,4,5};

arr        // decays to &arr[0]        type int*
arr + 1    // &arr[1]                  address + 1*sizeof(int) = +4
&arr       // address of WHOLE array   type int(*)[5]
&arr + 1   // address past the array   address + 5*sizeof(int) = +20

int*  p  = arr;         // pointer to first element
int (*pa)[5] = &arr;    // pointer to the entire array
std::cout << (*pa)[2];  // 3
```

Legal operations: `++`, `--`, `+ n`, `- n`, `p1 - p2` (same array),
comparison, compare against `nullptr`. Multiplying pointers is not a thing.

### Array-to-pointer decay

`sizeof` does **not** decay an array; passing it to a function does.

```cpp
void fun(int arr[]) {                       // silently means int* arr
    std::cout << sizeof(arr) / sizeof(arr[0]);   // 8/4 == 2  — WRONG
}

int main() {
    int arr[] = {1,2,3,4,5};
    std::cout << sizeof(arr)/sizeof(arr[0]);     // 20/4 == 5 — correct
    fun(arr);
}
```

Safe C++ alternatives that keep the size:

```cpp
template <size_t N> void f(int (&arr)[N]) { /* N is the real length */ }
void g(std::span<int> s)     { s.size(); }      // C++20
void h(std::array<int,5>& a) { a.size(); }
```

### Multidimensional arrays

Passing one requires every dimension except the first:

```cpp
void fun(int (*arr)[5]) {                   // same as int arr[][5]
    std::cout << sizeof(arr)     << '\n';   // 8  — pointer
    std::cout << sizeof(arr[0])  << '\n';   // 20 — arr[0] is int[5]
}

int main() {
    int arr[5][5] = {};
    std::cout << sizeof(arr)    << '\n';    // 100
    std::cout << sizeof(arr[0]) << '\n';    // 20
    fun(arr);
}
```

### Returning a 2-D array from a function

Returning a pointer to a local array is UB (the frame dies). Three valid ways:

```cpp
constexpr int N = 3;

// 1) Heap allocation — caller owns it
int** heapArray() {
    int** a = new int*[N];
    for (int i = 0; i < N; ++i) {
        a[i] = new int[N];
        for (int j = 0; j < N; ++j) a[i][j] = i + j;
    }
    return a;                       // caller must delete[] each row, then a
}

// 2) static storage — lives in .data, but shared and not reentrant
int (*staticArray())[N] {
    static int a[N][N] = {{0,1,2},{3,4,5},{6,7,8}};
    return a;
}

// 3) Wrap in a struct/std::array — returned by value, safest
struct Grid { int a[N][N]; };
Grid byValue() {
    Grid g{};
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) g.a[i][j] = i + j;
    return g;                       // NRVO: usually zero copies
}

// 4) Modern: std::vector<std::vector<int>> or a flat vector + index math
std::vector<int> flat(N*N);
auto at = [&](int r, int c) -> int& { return flat[r*N + c]; };
```

### Function pointers

```cpp
int add(int a, int b) { return a + b; }

int (*fp)(int, int) = add;          // declare + assign
std::cout << fp(2, 3);              // 5
using BinOp = int(*)(int, int);     // readable alias
BinOp op = add;

// callback style
void apply(const int* a, int n, int (*f)(int)) {
    for (int i = 0; i < n; ++i) std::cout << f(a[i]) << ' ';
}
```

Member function pointers use a different syntax:

```cpp
struct S { int f(int x) { return x * 2; } };
int (S::*mp)(int) = &S::f;
S s;
std::cout << (s.*mp)(21);           // 42
S* ps = &s;
std::cout << (ps->*mp)(21);
```

### Blocking address-of

```cpp
class NoAddr {
public:
    NoAddr* operator&() = delete;   // taking &obj is now a compile error
};
// alternative: declare operator& private and leave it undefined (pre-C++11)
```

---

## 8. Dynamic Memory

### malloc vs new

| | `malloc` | `new` |
|---|---|---|
| Kind | library function | operator (overloadable) |
| Constructor | not called | called |
| Return type | `void*` (cast needed in C++) | correct typed pointer |
| Size | you compute it | compiler computes it |
| On failure | returns `NULL` | throws `std::bad_alloc` |
| Resize | `realloc` | not possible |
| Paired with | `free` | `delete` / `delete[]` |
| Memory region | heap | free store (conceptually distinct) |

```cpp
int*  a = static_cast<int*>(malloc(10 * sizeof(int)));   free(a);
int*  b = new int[10];                                   delete[] b;

// nothrow variant
int* c = new (std::nothrow) int[10];
if (!c) { /* allocation failed, no exception */ }
```

Rules: `new` pairs with `delete`, `new[]` pairs with `delete[]`. Mixing them is
UB. Deleting a null pointer is a safe no-op. Double-delete is UB — null the
pointer after deleting, or better, use a smart pointer.

### malloc vs calloc vs realloc

```c
void* malloc(size_t n);                 // n bytes, uninitialized
void* calloc(size_t cnt, size_t sz);    // cnt*sz bytes, zero-filled, overflow-checked
void* realloc(void* p, size_t n);       // grow/shrink; may move the block
void  free(void* p);
```

`calloc` is slower than bare `malloc` because it zeroes, but faster than
`malloc` + `memset` for large blocks: the kernel can hand back pages that are
already zero (copy-on-write from the zero page), skipping the memset entirely.

`realloc` caveat: if it returns `NULL` the original block is still valid — never
write `p = realloc(p, n);` directly or you leak on failure.

### new for arrays and multi-dimensional data

```cpp
int*  x  = new int;             delete x;
int*  a  = new int[10];         delete[] a;
int*  z  = new int[10]();       // value-initialized to 0
int*  v  = new int[10]{1,2,3};  // rest zero-initialized

// 2-D, contiguous (best for cache)
int rows = 3, cols = 5;
int* grid = new int[rows * cols]();
auto at = [&](int r, int c) -> int& { return grid[r * cols + c]; };
at(1,2) = 42;
delete[] grid;

// 2-D, array of pointers (rows can be ragged, worse locality)
int** m = new int*[rows];
for (int i = 0; i < rows; ++i) m[i] = new int[cols]();
for (int i = 0; i < rows; ++i) delete[] m[i];   // rows first
delete[] m;                                     // then the spine
```

### Placement new

Constructs an object in memory you already own — no allocation, no kernel round
trip. The core of memory pools, arenas and `std::vector`'s internals.

```cpp
#include <new>

alignas(Base) char pool[10 * sizeof(Base)];     // pre-allocated storage

Base* o1 = new (pool + 0 * sizeof(Base)) Base;
Base* o2 = new (pool + 1 * sizeof(Base)) Base;
Base* o3 = new (pool + 2 * sizeof(Base)) Base;

o1->~Base();        // destructors MUST be called explicitly
o2->~Base();
o3->~Base();
// do NOT delete o1/o2/o3 — the storage was not allocated by ::operator new
```

Use `alignas` (or `std::aligned_storage`) so the buffer meets the type's
alignment requirement; a raw `char[]` is only 1-byte aligned.

### Overloading new/delete

```cpp
class Tracked {
public:
    static void* operator new(size_t sz) {
        std::cout << "alloc " << sz << '\n';
        return ::operator new(sz);
    }
    static void operator delete(void* p) noexcept {
        std::cout << "free\n";
        ::operator delete(p);
    }
};
```

### Common memory bugs

| Bug | Symptom | Fix |
|---|---|---|
| Leak | RSS grows | RAII / smart pointers |
| Dangling pointer | use-after-free, random corruption | null after delete, use `weak_ptr` |
| Double free | heap corruption / abort | ownership discipline |
| Buffer overrun | corrupts neighbours | bounds-checked containers, `.at()` |
| Mismatched `delete`/`delete[]` | UB | match the form |
| Uninitialized read | non-deterministic | always initialize |

Tooling: `-fsanitize=address,undefined`, `valgrind --leak-check=full`.

---

## 9. Type Casting

### Implicit conversion (promotion)

```
char -> short -> int -> unsigned int -> long -> long long -> float -> double -> long double
```

Widening is safe; narrowing loses data and should be explicit.

```cpp
int   i = 3;
float f = i;        // 3.0f, exact
int   j = 3.99;     // 3 — fractional part discarded
```

Integer promotion traps:

```cpp
unsigned u = 1;
int      s = -1;
if (s < u) std::cout << "less";     // NOT printed: s converts to a huge unsigned
```

### static_cast

Compile-time checked conversion between related types. No runtime check.

```cpp
double d = 3.9;
int    i = static_cast<int>(d);         // 3

class Animal { public: virtual ~Animal() = default; };
class Dog : public Animal { public: bool loyal = true; };

Dog d2;
Animal  a  = static_cast<Animal>(d2);   // upcast by value — SLICES d2
Animal* ap = &d2;
Dog*    dp = static_cast<Dog*>(ap);     // downcast, UNCHECKED — you must be right
```

### dynamic_cast

Runtime-checked downcast/crosscast for polymorphic types.
Requires at least one virtual function in the base (needs the vtable/RTTI).

- pointer form: returns `nullptr` on failure
- reference form: throws `std::bad_cast` on failure

```cpp
class Base    { public: virtual void print() { std::cout << "Base\n"; } };
class Derived1: public Base { void print() override { std::cout << "D1\n"; } };
class Derived2: public Base { void print() override { std::cout << "D2\n"; } };

Derived1 d1;
Base* bp = &d1;

if (auto* p = dynamic_cast<Derived2*>(bp)) p->print();
else std::cout << "Casting Failed\n";       // this branch runs

try {
    Derived2& r = dynamic_cast<Derived2&>(*bp);   // throws std::bad_cast
} catch (const std::bad_cast& e) {
    std::cout << e.what() << '\n';
}
```

### const_cast

Adds or removes `const`/`volatile`. Writing through it is UB if the original
object was truly `const`.

```cpp
int n = 5;
const int* cp = &n;
int* p = const_cast<int*>(cp);
*p = 10;                        // OK — n was never really const

const int k = 5;
*const_cast<int*>(&k) = 10;     // UB — k is genuinely const
```

Legitimate use: calling a legacy C API that takes `char*` but does not modify it.

### reinterpret_cast

Reinterprets the bit pattern. No conversion, no check. Platform dependent.

```cpp
struct MyStruct { int x; int y; char c; bool b; };

MyStruct s{5, 10, 'a', true};

int* p = reinterpret_cast<int*>(&s);
std::cout << *p << '\n';        // 5
++p;
std::cout << *p << '\n';        // 10
++p;
char* ch = reinterpret_cast<char*>(p);
std::cout << *ch << '\n';       // 'a'
++ch;
bool* bp2 = reinterpret_cast<bool*>(ch);
std::cout << *bp2 << '\n';      // 1
```

Strict aliasing: reading an object through a pointer of an unrelated type is UB.
The portable way to reinterpret bytes is `std::memcpy`, or C++20 `std::bit_cast`:

```cpp
float f = 1.0f;
uint32_t bits = std::bit_cast<uint32_t>(f);     // C++20, constexpr-friendly
// pre-C++20:
uint32_t bits2; std::memcpy(&bits2, &f, sizeof f);
```

### C-style cast

`(int)x` tries, in order: `const_cast`, `static_cast`, `static_cast +
const_cast`, `reinterpret_cast`, `reinterpret_cast + const_cast`. It is
unsearchable and can silently do something dangerous. Prefer the named casts.

### Summary

| Cast | Checked | Use for |
|---|---|---|
| `static_cast` | compile time | numeric conversion, upcast, known-safe downcast |
| `dynamic_cast` | runtime | safe polymorphic downcast/crosscast |
| `const_cast` | compile time | add/strip const |
| `reinterpret_cast` | none | bit reinterpretation, pointer <-> integer |

---

## 10. Struct/Class Padding and sizeof

### Why padding exists

The CPU reads memory in word-sized chunks (4 bytes on 32-bit, 8 on 64-bit). An
`int` that straddles a word boundary would need two reads (and on some ISAs it
faults). The compiler inserts padding so each member sits at an address that is a
multiple of its own alignment.

```cpp
struct S { char a; char b; int c; };
```

Without padding: `a b c c c c` — reading `c` costs two cycles.
With padding: `a b _ _ c c c c` — one cycle.
`sizeof(S) == 8`.

### Rules

1. Each member is placed at an offset that is a multiple of its alignment.
2. The struct's alignment is the largest alignment among its members.
3. Trailing padding is added so `sizeof` is a multiple of the struct's alignment
   (so arrays of the struct stay aligned).

```cpp
struct A { char c; int i; char d; };
// c @0, pad 1..3, i @4..7, d @8, pad 9..11  -> sizeof == 12, alignof == 4

struct B { int i; char c; char d; };
// i @0..3, c @4, d @5, pad 6..7             -> sizeof == 8,  alignof == 4
```

**Order members from largest to smallest to minimize padding.**

```cpp
struct Bad  { char a; double b; char c; };   // 24 bytes
struct Good { double b; char a; char c; };   // 16 bytes
```

### Disabling padding

```cpp
#pragma pack(push, 1)
struct Packed { int a; char b; double c; };  // sizeof == 13
#pragma pack(pop)

struct Packed2 { int a; char b; double c; } __attribute__((packed));  // GCC/Clang
```

Packed structs cost performance (unaligned access) and taking the address of a
misaligned member is UB. Use only for wire/disk formats — and prefer explicit
serialization over `memcpy` of a packed struct.

### Querying and forcing alignment

```cpp
alignof(int)            // 4
alignas(64) struct CacheLine { int counter; };   // avoid false sharing
static_assert(sizeof(Packet) == 20, "wire format changed");
offsetof(struct A, i)   // byte offset of member i  <cstddef>
```

### sizeof of user-defined types

```cpp
class Empty {};                 // sizeof == 1  (distinct addresses required)
class OneInt { int a; };        // sizeof == 4
struct EmptyS {};               // sizeof == 1
enum  EmptyE {};                // sizeof == 4  (underlying type int)
enum  FilledE { apple };        // sizeof == 4
typedef int Alias;              // sizeof == 4
class WithVirtual { virtual void f(); };  // sizeof == 8 (vptr only)
```

Sizes change with inheritance:

```cpp
class A1 { public: int a; double c; char b; };   // 24 (align 8)
class B1 { public: int x, y, z, w; };            // 16
class B2 : public A1 { public: int x,y,z,w; };   // 40 = 24 + 16
```

Adding any virtual function injects an 8-byte vptr (64-bit) and raises the
alignment to 8.

Empty base optimization: an empty base contributes 0 bytes.

```cpp
struct E {};
struct D : E { int x; };        // sizeof(D) == 4, not 5
```

`static` members are not part of the object:

```cpp
class C { public: int a; static int b; };
int C::b = 10;
// sizeof(C) == 4
```

---

## 11. Bitwise Operators and Bitfields

| Operator | Name | Example (`a=5 (0101)`, `b=3 (0011)`) |
|---|---|---|
| `&` | AND | `a & b == 1` (0001) |
| `\|` | OR | `a \| b == 7` (0111) |
| `^` | XOR | `a ^ b == 6` (0110) |
| `~` | NOT | `~a == -6` (two's complement) |
| `<<` | left shift | `a << 1 == 10` (multiply by 2) |
| `>>` | right shift | `a >> 1 == 2` (divide by 2) |

### Common idioms

```cpp
x & (1u << n)            // test bit n
x |=  (1u << n)          // set bit n
x &= ~(1u << n)          // clear bit n
x ^=  (1u << n)          // toggle bit n
x & (x - 1)              // clear lowest set bit; ==0 means power of two
x & -x                   // isolate lowest set bit
(x >> n) & 1             // extract bit n
__builtin_popcount(x)    // count set bits (GCC/Clang)
std::popcount(x)         // C++20 <bit>
std::has_single_bit(x)   // C++20 power-of-two test
```

Shifting by >= the width of the type, or shifting a negative value left, is UB.
Right-shifting a negative signed value is implementation-defined (arithmetic
shift in practice). Use unsigned types for bit manipulation.

### Bitfields

```cpp
struct Flags {
    unsigned isValid   : 1;
    unsigned priority  : 3;     // 0..7
    unsigned reserved  : 4;
    unsigned id        : 24;
};                              // packs into 4 bytes
```

Layout is implementation-defined (bit order, straddling). Never use bitfields as
a portable wire format; shift and mask explicitly instead.

### Practical bit flags

```cpp
enum Perm : unsigned {
    Read = 1u << 0, Write = 1u << 1, Exec = 1u << 2
};
unsigned p = Read | Write;
if (p & Write) { /* ... */ }
```

---

## 12. Endianness

Little endian stores the least significant byte at the lowest address; big endian
stores the most significant byte first.

```
int x = 0x01234567;

big endian    (network order):  | 01 | 23 | 45 | 67 |
little endian (x86, ARM def.):  | 67 | 45 | 23 | 01 |
```

### Detecting it at runtime

```cpp
unsigned int a = 1;
char* c = reinterpret_cast<char*>(&a);
std::cout << (*c ? "Little Endian" : "Big Endian");
```

### Detecting it at compile time (C++20)

```cpp
#include <bit>
if constexpr (std::endian::native == std::endian::little) { /* ... */ }
```

### Byte swapping

```cpp
#include <arpa/inet.h>
uint32_t net  = htonl(host);    // host -> network (big endian)
uint32_t host = ntohl(net);

uint32_t swap32(uint32_t v) {
    return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
           ((v & 0x00FF0000u) >>  8) | ((v & 0xFF000000u) >> 24);
}
// GCC/Clang: __builtin_bswap32(v)      C++23: std::byteswap
```

Endianness matters whenever bytes cross a machine boundary: network protocols,
binary file formats, memory-mapped device registers.

---
---

# PART II — OBJECT ORIENTED C++

## 13. OOP Pillars

### 1) Class and Object

A class is a blueprint (a type); an object is an instance of it (storage +
identity). A `struct` in C++ is a class whose default access is `public`;
a `class` defaults to `private`. That is the only difference.

```cpp
class Point {
    int x, y;                       // private by default
public:
    Point(int a, int b) : x(a), y(b) {}
    int getX() const { return x; }
};

struct Pair { int a, b; };          // public by default
```

### 2) Encapsulation

Bundling data and the functions that operate on it into one unit, and controlling
access to the internals.

- **Access control** — `public` / `protected` / `private` decide who can touch what.
- **Data hiding** — internal representation is not part of the interface, so it
  can change without breaking callers.
- **Getters/setters** — controlled access points where you can validate.

```cpp
class BankAccount {
    double balance = 0;                         // hidden state
public:
    bool deposit(double amt) {                  // enforces an invariant
        if (amt <= 0) return false;
        balance += amt;
        return true;
    }
    double getBalance() const { return balance; }
};
```

### 3) Abstraction

Exposing *what* an object does while hiding *how*. Achieved with access
specifiers, abstract base classes and the PIMPL idiom.

```cpp
class Engine {                      // pure interface
public:
    virtual void start() = 0;
    virtual ~Engine() = default;
};
```

Encapsulation is the mechanism (bundling + hiding); abstraction is the design
goal (exposing only the essential model).

### 4) Inheritance — "IS-A"

One class acquires the members of another. `Maruti IS-A Car`.
(Contrast with composition, "HAS-A": `Car HAS-A Engine`.)

### 5) Polymorphism — one interface, many forms

| Kind | When resolved | Mechanism |
|---|---|---|
| Compile-time (static) | compile | function overloading, operator overloading, templates |
| Run-time (dynamic) | runtime | virtual functions + vtable |

```cpp
// compile-time
int  area(int s)          { return s * s; }
double area(double r)     { return 3.14159 * r * r; }

// run-time
struct Shape { virtual double area() const = 0; virtual ~Shape() = default; };
struct Circle : Shape { double r; double area() const override { return 3.14159*r*r; } };
struct Square : Shape { double s; double area() const override { return s*s; } };

void report(const Shape& sh) { std::cout << sh.area(); }   // works for both
```

### Composition vs Inheritance

Prefer composition. Inheritance couples you to the base class's implementation
and is only correct when the derived type is genuinely substitutable for the base
(Liskov Substitution Principle).

```cpp
class Engine { public: void start() {} };

class Car {                 // HAS-A: Car owns an Engine
    Engine e;
public:
    void start() { e.start(); }
};
```

---

## 14. Constructors and Destructors

### The five special member functions

```cpp
class X {
public:
    X();                        // 1) default constructor
    X(const X&);                // 2) copy constructor
    X& operator=(const X&);     // 3) copy assignment
    X(X&&) noexcept;            // 4) move constructor        (C++11)
    X& operator=(X&&) noexcept; // 5) move assignment         (C++11)
    ~X();                       //    destructor
};
```

### Constructor types

```cpp
class Abc {
public:
    int a = 0, b = 0;

    Abc() = default;                            // default
    Abc(int x, int y) : a(x), b(y) {}           // parameterized
    Abc(const Abc& o) : a(o.a), b(o.b) {}       // copy
    Abc(Abc&& o) noexcept : a(o.a), b(o.b) {}   // move
    explicit Abc(int x) : a(x) {}               // converting, made explicit
};

int main() {
    Abc o1(900, 89990);
    Abc o2(o1);         // copy constructor
    Abc o3 = o1;        // ALSO copy constructor (copy-initialization)
    Abc o4;
    o4 = o1;            // copy ASSIGNMENT — o4 already existed
}
```

Copy constructor vs copy assignment:
- Copy constructor runs when a **new** object is built from an existing one.
- Copy assignment runs when an **existing** object is overwritten.

### Constructor delegation (C++11)

One constructor calls another from its initializer list — removes duplication.

```cpp
class Abc {
    int a, b;
public:
    Abc()             : Abc(0, 0) {}
    Abc(int x)        : Abc(x, 0) {}
    Abc(int x, int y) : a{x}, b{y} {
        // the single place with the complex initialization logic
    }
};
```

### explicit

Blocks implicit one-argument conversions.

```cpp
class Meters {
    int v;
public:
    explicit Meters(int x) : v(x) {}
};

void travel(Meters m);
// travel(5);                 // ERROR with explicit — good
travel(Meters{5});            // clear intent
```

### Order of construction and destruction

1. Base class constructor
2. Member objects, in **declaration order**
3. Derived class constructor body

Destruction is exactly the reverse.

```cpp
class B { public: B() { std::cout << "B ctor\n"; } ~B() { std::cout << "B dtor\n"; } };
class C : public B {
public:
    C() : B() { std::cout << "C ctor\n"; }
    ~C() { std::cout << "C dtor\n"; }
};
// Output: B ctor / C ctor / C dtor / B dtor
```

If the base has **no** default constructor, the derived class must call a base
constructor explicitly:

```cpp
class Base {
    int a;
public:
    Base(int x) : a(x) { std::cout << "base ctor\n"; }   // no default ctor
};

class Child : public Base {
public:
    Child() : Base(5) { std::cout << "child ctor\n"; }   // mandatory
};
```

### What can a constructor / destructor be?

| Property | Constructor | Destructor | Why |
|---|---|---|---|
| Overloaded | Yes | No | destructor takes no parameters |
| `private` | Yes | Yes | singleton / factory / heap-only objects |
| `static` | No | No | they operate on an instance |
| `virtual` | **No** | **Yes (usually should be)** | no vptr exists yet during construction |
| `const` | No | No | their job is to write member state |
| Inherited | Not implicitly (C++11 `using Base::Base;` can) | No | — |
| Can throw | Yes | Should not (`noexcept` by default) | throwing during unwinding calls `terminate` |

### Why the destructor should be virtual in a polymorphic base

```cpp
struct Base          { ~Base() { std::cout << "~Base\n"; } };            // BUG
struct Derived : Base{ ~Derived(){ std::cout << "~Derived\n"; } };

Base* p = new Derived;
delete p;               // UB — only ~Base runs, Derived's resources leak
```

Fix:

```cpp
struct Base { virtual ~Base() = default; };
```

Rule: **if a class has any virtual function, give it a virtual destructor.**
Conversely, do not pay for a vptr in a type never used polymorphically.

### Private constructor use cases

```cpp
class Singleton {
    Singleton() = default;                          // nobody else can construct
public:
    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;
    static Singleton& getInstance() {
        static Singleton instance;                  // thread-safe since C++11
        return instance;
    }
};
```

### Private destructor

Forces heap allocation with a controlled release path — useful for
reference-counted objects.

```cpp
class HeapOnly {
    ~HeapOnly() = default;                  // stack objects now illegal
public:
    void destroy() { delete this; }
};
// HeapOnly h;              // ERROR: destructor is private
HeapOnly* p = new HeapOnly; p->destroy();   // OK
```

### Calling a function before main()

A global object's constructor, or a global initialized from a function call, runs
before `main`.

```cpp
int fun() { std::cout << "Inside fun\n"; return 1; }

class Abc {
public:
    Abc() { fun(); }
} d;                            // global object -> ctor runs before main

int e = fun();                  // dynamic initialization, also before main

int main() { std::cout << "Inside main\n"; }
```

Caveat — the **static initialization order fiasco**: the relative order of
dynamic initialization across translation units is unspecified. Use a function-local
static (the Meyers singleton) instead of a global object when order matters.

```cpp
Logger& log() { static Logger l; return l; }    // initialized on first use
```

### Printing N times with no loop and no recursion

An array of objects invokes the constructor once per element.

```cpp
class Abc { public: Abc() { std::cout << "inside constructor\n"; } };

int main() {
    Abc ob[10];                 // constructor runs 10 times
}
```

---

## 15. Copy Semantics: Shallow vs Deep

### The problem

The compiler-generated copy constructor does a **memberwise** copy. For a raw
pointer member, that copies the *address*, so two objects now own the same
buffer: modifying one is visible in the other, and both destructors free it
(double free).

```
Shallow:   obj1.ptr ──┐
                      ├──> [ 0xABCD heap block ]
           obj2.ptr ──┘

Deep:      obj1.ptr ────> [ 0xABCD ]
           obj2.ptr ────> [ 0xBEEF ]   (independent copy)
```

| | Shallow copy | Deep copy |
|---|---|---|
| Source | compiler-generated | user-written |
| Pointer members | address copied | pointee duplicated |
| Resources | shared | independent |
| Risk | double free, aliasing | none (costs a allocation) |

### Deep copy: full Rule of Three

```cpp
#include <cstring>

class MyString {
    char* str;
public:
    MyString(const char* s = "") {                  // constructor
        str = new char[std::strlen(s) + 1];
        std::strcpy(str, s);
    }

    MyString(const MyString& other) {               // copy constructor
        str = new char[std::strlen(other.str) + 1];
        std::strcpy(str, other.str);
    }

    MyString& operator=(const MyString& other) {    // copy assignment
        if (this != &other) {                       // self-assignment guard
            delete[] str;
            str = new char[std::strlen(other.str) + 1];
            std::strcpy(str, other.str);
        }
        return *this;                               // enables chaining
    }

    ~MyString() { delete[] str; }                   // destructor

    void print() const { std::cout << str << '\n'; }
};

int main() {
    MyString s1("Hello");
    MyString s2 = s1;       // copy constructor
    MyString s3("x");
    s3 = s1;                // copy assignment
}
```

### Copy-and-swap idiom — exception-safe assignment in one line

```cpp
class MyString {
    char* str;
public:
    friend void swap(MyString& a, MyString& b) noexcept {
        using std::swap;
        swap(a.str, b.str);
    }
    MyString& operator=(MyString other) {   // by VALUE: copy or move happens here
        swap(*this, other);
        return *this;
    }                                        // other's destructor frees the old buffer
};
```

This single operator handles both copy and move assignment, is self-assignment
safe, and is strongly exception safe.

### Why copy assignment returns `T&`

To support chaining and to make `(a = b) = c` behave correctly.

```cpp
class Abc {
public:
    int data = 0;
    Abc() = default;
    explicit Abc(int v) : data(v) {}
    Abc& operator=(const Abc& o) { data = o.data; return *this; }
};

Abc o1(10), o2, o3, o4;
o4 = o3 = o2 = o1;      // right-to-left chain — needs a reference return
(o3 = o2) = o1;         // assigns into o3 twice — needs a REFERENCE, not a copy
```

If it returned `void`, chaining is a compile error. If it returned `Abc` by
value, `(o3 = o2) = o1` would assign into a temporary and silently do nothing.

### Preventing copies

```cpp
// 1) C++11: delete them  (preferred — clear error at the call site)
class NoCopy {
public:
    NoCopy() = default;
    NoCopy(const NoCopy&)            = delete;
    NoCopy& operator=(const NoCopy&) = delete;
};

// 2) pre-C++11: declare private and never define (link error, poor diagnostics)
class NoCopyOld {
    NoCopyOld(const NoCopyOld&);
    NoCopyOld& operator=(const NoCopyOld&);
};

// 3) inherit from a non-copyable base
class Widget : private NoCopy { };
```

---

## 16. Inheritance

### Access specifier matrix

Base member access as seen in the derived class:

| Base member | `public` inheritance | `protected` inheritance | `private` inheritance |
|---|---|---|---|
| `public` | public | protected | private |
| `protected` | protected | protected | private |
| `private` | inaccessible | inaccessible | inaccessible |

```cpp
class Derived : public Base { };     // IS-A          (the usual choice)
class Derived : protected Base { };  // rare
class Derived : private Base { };    // "implemented in terms of" — prefer composition
```

A `class` defaults to `private` inheritance; a `struct` defaults to `public`.
Always write the specifier explicitly.

### Types of inheritance

```cpp
class Single    : public Base {};                       // single
class Multiple  : public Base1, public Base2 {};        // multiple
class Grandkid  : public Parent {};                     // multilevel (Parent : Grandparent)
class Dog       : public Animal {};                     // hierarchical
class Cat       : public Animal {};                     //   (siblings off one base)
class Hybrid    : public Dog, public SomeOther {};      // hybrid
```

### The Diamond Problem

```
      A
     / \
    B   C
     \ /
      D
```

`D` inherits `A` through both `B` and `C`, so it contains **two** `A`
subobjects: `A`'s constructor runs twice, `obj.x` is ambiguous.

```cpp
class A { public: int x = 5; };
class B : public A {};
class C : public A {};
class D : public B, public C {};

D obj;
// std::cout << obj.x;          // ERROR: ambiguous
std::cout << obj.B::x;          // workaround 1: scope resolution — still 2 copies
```

**Solution: virtual inheritance** — one shared `A` subobject.

```cpp
class A { public: int x = 5; };
class B : virtual public A { public: int i = 6; };
class C : virtual public A { public: int i = 7; };
class D : public B, public C { };

int main() {
    D obj;
    std::cout << obj.x;         // 5 — unambiguous, single A
}
```

With virtual inheritance the **most-derived class** (`D`) is responsible for
constructing the virtual base `A`, and it happens before `B` and `C`:

```cpp
class A { public: A(int) {} };
class B : virtual public A { public: B() : A(1) {} };
class C : virtual public A { public: C() : A(2) {} };
class D : public B, public C {
public:
    D() : A(3), B(), C() {}     // D's A(3) wins; B's and C's A(..) are ignored
};
```

Cost: virtual bases add a vbase pointer/offset per object and make member access
slightly indirect.

### final

```cpp
class Base final { };                   // cannot be inherited
// class Derived : public Base { };     // ERROR

class X { virtual void f() final; };    // f cannot be overridden further
```

### Inheriting constructors (C++11)

```cpp
class Base { public: Base(int); Base(int, int); };
class Derived : public Base {
public:
    using Base::Base;           // pulls in both Base constructors
};
Derived d(1, 2);                // OK
```

---

## 17. Polymorphism, vtable and vptr

### Mechanism

- A class with at least one `virtual` function gets a hidden member: the **vptr**.
- The vptr points to that class's **vtable**, a static per-class array of function
  pointers.
- At a virtual call site the compiler emits: load vptr -> index the vtable ->
  indirect call. The target depends on the object's *dynamic* type, not the
  static type of the pointer.

```
Base* p = new Derived;

  p ──> [ vptr ] ──> Derived vtable:  [0] &Derived::~Derived
        [ data ]                      [1] &Derived::speak
                                      [2] &Base::eat        (not overridden)
```

```cpp
class Base {
public:
    virtual void speak() { std::cout << "Base\n"; }
    void eat()           { std::cout << "Base eat\n"; }   // NOT virtual
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void speak() override { std::cout << "Derived\n"; }
    void eat()            { std::cout << "Derived eat\n"; }
};

Base* p = new Derived;
p->speak();     // "Derived"     — virtual: dynamic dispatch
p->eat();       // "Base eat"    — non-virtual: static dispatch on Base*
delete p;
```

Advantages: a base-class pointer can drive derived behaviour; enables plugin and
strategy designs.
Costs: 8 bytes per object (vptr), one extra indirection per call, blocks inlining
of the virtual call.

### override and final

```cpp
class Base    { public: virtual void fun(int) {} };
class Derived : public Base {
public:
    void fun(int) override {}       // compiler verifies a matching base virtual
    // void fun(char) override {}   // ERROR — signature does not match
    void fun2() final;              // no further overriding
};
```

Always write `override`. Without it a typo or mismatched signature silently
creates a *new* function instead of an override.

### Pure virtual functions and abstract classes

```cpp
class Shape {
public:
    virtual double area() const = 0;        // pure virtual ("do nothing")
    virtual ~Shape() = default;
};
// Shape s;                                 // ERROR: abstract class
```

Rules that fall out of `= 0`:
1. You cannot instantiate the class (no object, so no way to call the stub).
2. A derived class must override it to become concrete.
3. It must be `virtual` so a `Base*` pointing at a derived object dispatches
   correctly.

A pure virtual function **may** still have a definition, used as a shared default:

```cpp
class Shape {
public:
    virtual void draw() = 0;
};
void Shape::draw() { std::cout << "default outline\n"; }

class Circle : public Shape {
public:
    void draw() override { Shape::draw(); std::cout << "circle\n"; }
};
```

### Interface class

```cpp
class Drawable {                // pure interface: all pure virtual, virtual dtor
public:
    virtual void draw() const = 0;
    virtual ~Drawable() = default;
};
```

### Why constructors cannot be virtual

During `Base`'s constructor the vptr still points at `Base`'s vtable (the derived
part does not exist yet). There is no vtable to dispatch "which constructor" from.
Corollary: **calling a virtual function from a constructor or destructor
dispatches to the current class's version, not the derived override.**

```cpp
class Base {
public:
    Base() { init(); }                      // calls Base::init, NOT Derived::init
    virtual void init() { std::cout << "Base init\n"; }
};
class Derived : public Base {
public:
    void init() override { std::cout << "Derived init\n"; }
};
Derived d;      // prints "Base init"
```

Virtual-constructor *idiom* (a clone method) when you need polymorphic copying:

```cpp
class Shape {
public:
    virtual Shape* clone() const = 0;
    virtual ~Shape() = default;
};
class Circle : public Shape {
public:
    Circle* clone() const override { return new Circle(*this); }   // covariant return
};
```

### RTTI: typeid

```cpp
#include <typeinfo>

class A {}; class B {};

A a1, a2; B b1;
if (typeid(a1) == typeid(a2)) std::cout << "same class\n";
if (typeid(a1) == typeid(b1)) std::cout << "same class\n"; else std::cout << "different\n";
std::cout << typeid(a1).name();     // mangled; run through c++filt to read
```

For a polymorphic type `typeid(*ptr)` gives the **dynamic** type; for a
non-polymorphic type it gives the static type.

---

## 18. Object Slicing and Function Hiding

### Object slicing

Assigning or passing a derived object **by value** to a base copies only the base
subobject — the derived part is sliced off, and virtual dispatch is lost.

```cpp
class Base {
public:
    int a, b, c;
    virtual void display() { std::cout << "base display\n"; }
    virtual ~Base() = default;
};
class Derived : public Base {
public:
    void display() override { std::cout << "derived display\n"; }
};

void byValue(Base obj)   { obj.display(); }     // SLICES
void byRef  (Base& obj)  { obj.display(); }     // correct
void byPtr  (Base* obj)  { obj->display(); }    // correct

int main() {
    Base b; Derived d;
    byValue(b);  byValue(d);    // "base display"    "base display"    <-- BUG
    byRef  (b);  byRef  (d);    // "base display"    "derived display"
    byPtr(&b);   byPtr(&d);     // "base display"    "derived display"
}
```

Slicing also happens on assignment and when storing derived objects in a
`std::vector<Base>`.

```cpp
std::vector<Base> bad;                      // slices every push_back
std::vector<std::unique_ptr<Base>> good;    // polymorphic and owning
```

Guard against it by making the base abstract, or by deleting base copy operations
(`protected` copy constructor is the classic trick).

### Function hiding (name hiding)

Declaring **any** function with a given name in a derived class hides **all**
base overloads of that name — regardless of signature, and independent of
`virtual`.

```cpp
class Base {
public:
    void fun(int) { std::cout << "Base function\n"; }
};
class Derived : public Base {
public:
    void fun(char) { std::cout << "Derived function\n"; }
};

Derived ob;
ob.fun(500);    // "Derived function" — int converts to char, Base::fun is HIDDEN
ob.fun('a');    // "Derived function"
```

Two fixes:

```cpp
// 1) explicit qualification at the call site
ob.Base::fun(500);

// 2) re-expose the base overloads in the derived class (preferred)
class Derived : public Base {
public:
    using Base::fun;            // brings Base::fun(int) back into scope
    void fun(char) { std::cout << "Derived function\n"; }
};
ob.fun(500);    // now "Base function"
```

Overloading / overriding / hiding at a glance:

| | Scope | Signature | Dispatch |
|---|---|---|---|
| Overloading | same class | must differ | compile time |
| Overriding | base + derived | must match exactly | runtime (needs `virtual`) |
| Hiding | base + derived | differs | compile time, base name shadowed |

---

## 19. Operator Overloading

Any operator that exists for built-in types can be overloaded for user-defined
types, except:

```
::      .       .*      ?:      sizeof      typeid      alignof
```

You cannot invent new operators or change arity/precedence/associativity.

### Binary operator as a member

```cpp
class Complex {
public:
    int re, im;
    Complex(int r = 0, int i = 0) : re(r), im(i) {}

    Complex operator+(const Complex& o) const {     // const: does not modify *this
        return Complex(re + o.re, im + o.im);
    }
    void show() const { std::cout << re << " + " << im << "i\n"; }
};

Complex a(5,1), b(5,2);
Complex c = a + b;          // same as a.operator+(b)
c.show();                   // 10 + 3i
```

### Binary operator as a free function

Necessary when the left operand is not your type (e.g. `2 + complex`) and for
stream operators.

```cpp
class Complex {
    int re, im;
public:
    Complex(int r = 0, int i = 0) : re(r), im(i) {}
    friend Complex operator+(const Complex& a, const Complex& b);
    friend std::ostream& operator<<(std::ostream& os, const Complex& c);
};

Complex operator+(const Complex& a, const Complex& b) {
    return Complex(a.re + b.re, a.im + b.im);
}

std::ostream& operator<<(std::ostream& os, const Complex& c) {
    return os << c.re << " + " << c.im << "i";
}

std::cout << (Complex(1,2) + Complex(3,4));     // 4 + 6i
```

Rule of thumb: operators that modify the left operand (`+=`, `=`, `[]`, `()`,
`->`) must be members. Symmetric binary operators (`+`, `==`, `<<`) are better as
free functions.

### Pre- vs post-increment

The dummy `int` parameter distinguishes the postfix form.

```cpp
class Counter {
    int x = 0;
public:
    Counter& operator++()      { ++x; return *this; }          // ++c  (prefix)
    Counter  operator++(int)   { Counter t = *this; ++x; return t; } // c++ (postfix)
    int get() const { return x; }
};
```

Prefix returns a reference and is cheaper. Postfix must copy the old value —
that is why `++i` is preferred over `i++` in loops over non-trivial types
(for `int` the compiler makes them identical; for iterators it does not).

### The full set worth knowing

```cpp
class V {
public:
    V& operator=(const V&);                     // assignment
    V& operator+=(const V&);                    // compound
    bool operator==(const V&) const;            // C++20 gives != for free
    auto operator<=>(const V&) const = default; // C++20 three-way, gives < <= > >=
    int& operator[](size_t i);                  // subscript (two overloads: const too)
    const int& operator[](size_t i) const;
    int  operator()(int a, int b);              // call operator -> functor
    V*   operator->();                          // smart-pointer-like
    V&   operator*();                           // dereference
    explicit operator bool() const;             // contextual conversion
};
```

### Conversion operators

Class -> primitive:

```cpp
class Abc {
    int data;
public:
    explicit Abc(int v) : data(v) {}
    operator int() const { return data; }       // implicit conversion to int
};

Abc d(10);
int val = d;                    // 10 — implicit
```

Mark it `explicit` to force an intentional cast:

```cpp
class MyClass {
    int value;
public:
    explicit MyClass(int v) : value(v) {}
    explicit operator int() const { return value; }
};

MyClass obj(20);
// int i = obj;                         // ERROR with explicit
int i = static_cast<int>(obj);          // 20
```

Primitive -> class is done by a converting constructor; mark it `explicit` when
implicit conversion would be surprising.

`explicit operator bool()` is the exception — it is still usable in `if (obj)`
because that context performs an explicit conversion.

---

## 20. Functors

A functor (function object) is a class that overloads `operator()`, so instances
can be called like functions. Unlike a plain function pointer, a functor can
carry state and is trivially inlinable by the compiler.

```cpp
class Printer {
public:
    void operator()(int a, int b) const { std::cout << a << ' ' << b << '\n'; }
};

Printer p;
p(4, 5);                        // 4 5
```

### Stateful functor with an algorithm

```cpp
#include <algorithm>

class Increment {
    int num;
public:
    explicit Increment(int n) : num(n) {}
    int operator()(int x) const { return num + x; }
};

int arr[] = {1,2,3,4,5};
std::transform(arr, arr + 5, arr, Increment(5));    // 6 7 8 9 10
```

### Comparator functor

```cpp
struct ByLength {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.size() < b.size();
    }
};

std::vector<std::string> v{"ccc","a","bb"};
std::sort(v.begin(), v.end(), ByLength{});          // a, bb, ccc

std::set<std::string, ByLength> s;                  // comparator baked into the type
```

### Standard functors (`<functional>`)

```cpp
std::plus<int>{}(2,3);              // 5
std::greater<int>{}(3,2);           // true
std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
```

### Functor vs function pointer vs lambda

| | State | Inlinable | Syntax |
|---|---|---|---|
| Function pointer | no | rarely | `int(*)(int)` |
| Functor | yes (members) | yes | class with `operator()` |
| Lambda | yes (captures) | yes | inline, compiler generates a functor |

A lambda *is* a compiler-generated functor — a closure type with `operator()`.

---

## 21. friend, static members, this

### friend class

A friend can access `private` and `protected` members of the granting class.

```cpp
class Foo {
    int a = 10;
public:
    friend class Boo;           // Boo may reach into Foo's internals
};

class Boo {
public:
    void peek(const Foo& f) { std::cout << f.a; }   // legal because of the friend
};
```

### friend function

```cpp
// 1) free function as friend
class Abc {
    int secret = 42;
    friend void reveal(const Abc&);
};
void reveal(const Abc& o) { std::cout << o.secret; }

// 2) a member of another class as friend
class A;
class B { public: void peek(const A&); };
class A { int x = 1; friend void B::peek(const A&); };
void B::peek(const A& a) { std::cout << a.x; }

// 3) the common real use: stream operator
class Vec {
    int x, y;
    friend std::ostream& operator<<(std::ostream&, const Vec&);
};
std::ostream& operator<<(std::ostream& os, const Vec& v) { return os << v.x << ',' << v.y; }
```

Friendship properties:
- Not symmetric (A friend of B does not make B friend of A).
- Not transitive.
- Not inherited.
- Granted by the class, so it does not violate encapsulation — the class chooses.

### this pointer

1. Implicitly passed to every **non-static** member function. Static member
   functions have no `this`.
2. It is a `T* const` holding the address of the calling object.
3. In a `const` member function the type becomes `const T* const`.

```cpp
class Abc {
    int data = 0;
public:
    Abc& setData(int data) {
        this->data = data;      // disambiguate member from parameter
        return *this;           // enables method chaining
    }
    void print() const { std::cout << this->data; }   // this is const Abc* const
};

Abc o;
o.setData(5).print();           // chaining
// ob.fun(10) is conceptually  Abc::fun(&ob, 10)
```

`delete this` is legal but only inside a member of a heap-allocated object, and
you must not touch any member afterwards.

### Static members recap

```cpp
class Counter {
    static int count;                       // shared by all objects
public:
    Counter() { ++count; }
    static int getCount() { return count; } // no object needed
};
int Counter::count = 0;                     // definition in exactly one TU

Counter a, b;
std::cout << Counter::getCount();           // 2
```

C++17 allows `inline static int count = 0;` inside the class, removing the
out-of-line definition.

---
---

# PART III — MODERN C++

## 22. lvalue/rvalue and Move Semantics

### Value categories

- **lvalue** — has an identity (a name / an address you can take). Persists
  beyond the expression.
- **rvalue** — a temporary / literal. No name; about to expire.
- **xvalue** — an "eXpiring" lvalue, e.g. the result of `std::move(x)`. Has an
  address but is safe to cannibalize.

```
        expression
        /        \
    glvalue     rvalue
    /     \     /    \
lvalue    xvalue    prvalue
```

```cpp
int   x = 10;
int&  lr = x;           // lvalue reference binds to an lvalue
// int& bad = 5;        // ERROR: cannot bind non-const lvalue ref to rvalue

int&& rr = 5;           // rvalue reference binds to a temporary
// int&& bad2 = x;      // ERROR: x is an lvalue
int&& rr2 = std::move(x);   // OK: move casts x to xvalue

const int& cr = 5;      // SPECIAL: const lvalue ref binds to an rvalue,
                        // and extends the temporary's lifetime to cr's scope
```

Note: a variable of type `int&&` is itself an **lvalue** (it has a name). That
is why `std::move` is needed again to forward it onward.

### What std::move actually does

Nothing at runtime. It is an unconditional cast to an rvalue reference:

```cpp
template <typename T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}
```

It merely makes the object *eligible* for a move overload. The actual stealing is
done by your move constructor / move assignment operator.

### Move constructor and move assignment

```cpp
class Simple {
public:
    int* p;

    explicit Simple(int data) : p(new int(data)) {
        std::cout << "constructor, data = " << *p << '\n';
    }

    Simple(const Simple& other) : p(new int(*other.p)) {        // COPY: allocate
        std::cout << "copy constructor, data = " << *p << '\n';
    }

    Simple(Simple&& other) noexcept : p(other.p) {              // MOVE: steal
        other.p = nullptr;                                      // leave valid+empty
        std::cout << "move constructor\n";
    }

    Simple& operator=(const Simple& other) {                    // copy assign
        if (this != &other) {
            delete p;
            p = new int(*other.p);
        }
        return *this;
    }

    Simple& operator=(Simple&& other) noexcept {                // move assign
        if (this != &other) {
            delete p;                                           // release own resource first
            p = other.p;
            other.p = nullptr;
        }
        return *this;
    }

    ~Simple() { delete p; }         // delete on nullptr is safe
};
```

Two non-negotiable rules:
1. **Mark move operations `noexcept`.** `std::vector` reallocation only uses your
   move constructor if it is `noexcept`; otherwise it falls back to copying to
   preserve the strong exception guarantee.
2. **Leave the moved-from object in a valid but unspecified state.** Nulling the
   pointer is what makes the destructor safe.

### Worked trace

```cpp
int main() {
    std::vector<Simple> arr;
    arr.reserve(2);

    Simple ob1(10);                 // constructor
    Simple ob2 = std::move(ob1);    // move constructor (ob1.p is now nullptr)
    arr.push_back(ob2);             // ob2 is an LVALUE -> copy constructor

    Simple ob3(Simple(20));         // constructor only (guaranteed copy elision, C++17)
    Simple ob4(Simple(30));         // constructor only
    ob4 = std::move(ob3);           // move assignment

    arr.push_back(ob4);             // lvalue -> copy constructor
    arr.push_back(Simple(40));      // rvalue -> constructor, then MOVE constructor
                                    // reserve(2) exceeded -> reallocation moves
                                    // existing elements (noexcept move ctor used)
}
```

Key readings from that trace:
- `push_back(lvalue)` copies; `push_back(rvalue)` moves; `emplace_back(args...)`
  constructs in place and avoids both.
- Vector growth uses the move constructor only because it is `noexcept`.
- `Simple ob3(Simple(20))` prints one constructor, not two — C++17 mandates
  elision of the temporary.

### Second worked example

```cpp
class String {
    char* data;
public:
    String(const char* s = "") {
        data = new char[std::strlen(s)+1];  std::strcpy(data, s);
        std::cout << "Constructed: " << data << '\n';
    }
    String(const String& o) {
        data = new char[std::strlen(o.data)+1]; std::strcpy(data, o.data);
        std::cout << "Copied: " << data << '\n';
    }
    String(String&& o) noexcept : data(o.data) { o.data = nullptr; std::cout << "Moved!\n"; }
    String& operator=(const String& o) {
        if (this != &o) { delete[] data;
            data = new char[std::strlen(o.data)+1]; std::strcpy(data, o.data); }
        std::cout << "Copy Assigned\n"; return *this;
    }
    String& operator=(String&& o) noexcept {
        if (this != &o) { delete[] data; data = o.data; o.data = nullptr; }
        std::cout << "Move Assigned!\n"; return *this;
    }
    ~String() { delete[] data; }
};

int main() {
    String s1("Hello");             // Constructor
    String s2 = s1;                 // Copy constructor   (s1 is lvalue)
    String s3 = String("World");    // Constructor only   (C++17 elision)
    s2 = s3;                        // Copy assignment
    s3 = String("C++");             // Move assignment    (RHS is a temporary)
}
```

### Perfect forwarding

A `T&&` parameter in a **template** is a *forwarding reference*, not an rvalue
reference. Reference collapsing makes it bind to both lvalues and rvalues.
`std::forward` restores the original value category.

```cpp
template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
```

Reference collapsing rules: `& &` -> `&`, `& &&` -> `&`, `&& &` -> `&`,
`&& &&` -> `&&`. Only `&& &&` stays an rvalue reference.

```cpp
void sink(const std::string&) { std::cout << "lvalue\n"; }
void sink(std::string&&)      { std::cout << "rvalue\n"; }

template <typename T> void relay(T&& t) { sink(std::forward<T>(t)); }

std::string s = "x";
relay(s);               // "lvalue"
relay(std::string("y"));// "rvalue"
relay(std::move(s));    // "rvalue"
```

Use `std::move` on an rvalue reference parameter; use `std::forward` on a
forwarding reference parameter. Never `std::move` a value you still need, and
never `std::move` a return value of a local (it defeats NRVO).

---

## 23. Rule of 0/3/5, Copy Elision

### Rule of Three (C++98)

If you write **any** of: destructor, copy constructor, copy assignment — you
almost certainly need all three. They exist to manage a resource, and the
compiler's defaults will get it wrong.

### Rule of Five (C++11)

Add move constructor and move assignment.

```cpp
class R {
public:
    ~R();
    R(const R&);
    R& operator=(const R&);
    R(R&&) noexcept;
    R& operator=(R&&) noexcept;
};
```

### Rule of Zero (preferred)

Design classes that own **no** raw resources. Hold `std::vector`,
`std::string`, `std::unique_ptr` members and declare none of the five — the
compiler-generated versions are correct and optimal.

```cpp
class Good {
    std::vector<int>            data;   // manages its own memory
    std::unique_ptr<Widget>     w;      // manages its own lifetime
    std::string                 name;
    // no destructor, no copy/move — all correct by default
};
```

### Implicit generation rules (why you should be explicit)

| You declare | Compiler still generates |
|---|---|
| nothing | all five |
| a destructor | copy ops (deprecated), **no move ops** |
| a copy constructor | **no move ops**, copy assign (deprecated) |
| a move constructor | copy ops are **deleted** |

The surprise: writing a destructor silently disables move semantics, so your
class quietly starts copying. Either follow the Rule of Zero, or declare all five
(`= default` where the default is right).

### Copy elision, RVO and NRVO

The compiler may (and since C++17 for prvalues, **must**) construct the return
value directly in the caller's storage, skipping copy/move entirely.

```cpp
Demo make()  { return Demo(); }     // RVO  — guaranteed, zero constructions extra
Demo make2() { Demo d; return d; }  // NRVO — allowed, not guaranteed
```

```cpp
class Demo {
public:
    std::vector<int> v;
    Demo()                        { std::cout << "Default\n"; }
    Demo(const Demo& d) : v(d.v)  { std::cout << "Copy Ctor\n"; }
    Demo(Demo&& d) noexcept : v(std::move(d.v)) { std::cout << "Move Ctor\n"; }
    Demo& operator=(const Demo& d){ v = d.v; std::cout << "Copy Assign\n"; return *this; }
    Demo& operator=(Demo&& d) noexcept { v = std::move(d.v); std::cout << "Move Assign\n"; return *this; }
};

Demo getDemo() { Demo d; return d; }

int main() {
    Demo a;             // Default
    Demo b = a;         // Copy Ctor
    Demo c = getDemo(); // Default only (NRVO); without NRVO: Default + Move Ctor
    b = a;              // Copy Assign
    c = getDemo();      // Default + Move Assign
}
```

Do **not** write `return std::move(local);` — it blocks NRVO and makes things
slower.

---

## 24. Smart Pointers and RAII

### RAII

Resource Acquisition Is Initialization: acquire in the constructor, release in
the destructor. Because destructors run deterministically on scope exit — including
during exception unwinding — resources cannot leak.

```cpp
class FileRAII {
    FILE* f;
public:
    explicit FileRAII(const char* path) : f(std::fopen(path, "r")) {
        if (!f) throw std::runtime_error("open failed");
    }
    ~FileRAII() { if (f) std::fclose(f); }
    FileRAII(const FileRAII&)            = delete;
    FileRAII& operator=(const FileRAII&) = delete;
    FILE* get() const { return f; }
};
```

Everything in the standard library that owns something follows this:
`std::vector`, `std::string`, `std::lock_guard`, `std::fstream`, smart pointers.

### Why smart pointers

Manual `new`/`delete` produces memory leaks, dangling pointers, double frees and
leaks on the exception path. Smart pointers automate the `delete`.

```cpp
void unsafe() {
    Widget* w = new Widget;
    mayThrow();                 // leaks w if this throws
    delete w;
}
void safe() {
    auto w = std::make_unique<Widget>();
    mayThrow();                 // w is destroyed during unwinding
}
```

### unique_ptr — exclusive ownership

Zero overhead versus a raw pointer. Movable, not copyable.

```cpp
#include <memory>

auto p = std::make_unique<int>(42);         // preferred over unique_ptr<int>(new int(42))
// auto q = p;                              // ERROR: copy deleted
auto q = std::move(p);                      // ownership transferred; p is now null

std::cout << *q;
q.reset();                                  // delete now
int* raw = q.get();                         // observe without owning
int* rel = q.release();                     // give up ownership; you must delete

auto arr = std::make_unique<int[]>(10);     // array form, uses delete[]
arr[0] = 1;
```

### shared_ptr — shared ownership

Maintains an atomic reference count in a separate control block. The object is
destroyed when the count hits zero.

```cpp
auto a = std::make_shared<int>(10);         // one allocation for object + control block
{
    auto b = a;                             // use_count() == 2
    std::cout << a.use_count();             // 2
}                                           // b dies, count back to 1
std::cout << a.use_count();                 // 1
```

Costs: a control block allocation, atomic increments/decrements on every copy.
Prefer `unique_ptr` unless ownership is genuinely shared.

`make_shared` is better than `shared_ptr<T>(new T)` — one allocation instead of
two, and exception safe.

### weak_ptr — non-owning observer

Does not affect the reference count. Used to break cycles and to safely test
whether the object still exists.

```cpp
auto sp = std::make_shared<int>(5);
std::weak_ptr<int> wp = sp;                 // use_count() still 1

if (auto locked = wp.lock()) {              // promote to shared_ptr, or null
    std::cout << *locked;
}
std::cout << wp.expired();                  // false
sp.reset();
std::cout << wp.expired();                  // true
```

### Circular reference — the shared_ptr leak

```cpp
struct B;
struct A { std::shared_ptr<B> bptr; ~A() { std::cout << "A destroyed\n"; } };
struct B { std::shared_ptr<A> aptr; ~B() { std::cout << "B destroyed\n"; } };

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();
    a->bptr = b;
    b->aptr = a;
}   // NOTHING is destroyed — each count stuck at 1. Leak.
```

Fix: make the back-edge weak.

```cpp
struct B { std::weak_ptr<A> aptr; ~B() { std::cout << "B destroyed\n"; } };
// Output now: A destroyed / B destroyed
```

Typical shape: parent holds `shared_ptr<Child>`, child holds `weak_ptr<Parent>`.

### Custom deleters

For resources not released by `delete`: `FILE*`, `malloc`, sockets, GPU handles.

```cpp
void customDelete(int* p) {
    std::cout << "Deleting pointer with value " << *p << '\n';
    delete p;
}

std::unique_ptr<int, decltype(&customDelete)> p(new int(42), customDelete);

// lambda deleter
auto fileDeleter = [](FILE* f) { if (f) std::fclose(f); };
std::unique_ptr<FILE, decltype(fileDeleter)> fp(std::fopen("a.txt","r"), fileDeleter);

// shared_ptr takes the deleter as a constructor argument only (not a template param)
std::shared_ptr<FILE> sf(std::fopen("a.txt","r"), [](FILE* f){ if (f) std::fclose(f); });
```

The deleter is part of `unique_ptr`'s **type** (`unique_ptr<T, D>`), which is why
`shared_ptr` is more flexible for type-erased cleanup.

### Comparison

| | `unique_ptr` | `shared_ptr` | `weak_ptr` |
|---|---|---|---|
| Ownership | exclusive | shared | none |
| Copyable | no (movable) | yes | yes |
| Overhead | none | control block + atomics | control block ref only |
| Affects count | n/a | yes | no |
| Access | `*`, `->` | `*`, `->` | must `.lock()` first |
| Use when | single owner (default) | true shared lifetime | break cycles, caches, observers |

### enable_shared_from_this

Needed when a member function must hand out a `shared_ptr` to itself.

```cpp
class Node : public std::enable_shared_from_this<Node> {
public:
    std::shared_ptr<Node> self() { return shared_from_this(); }
    // returning std::shared_ptr<Node>(this) would create a SECOND control block
    // -> double free
};
```

---

## 25. Lambdas and std::function

A lambda is an unnamed function object. The compiler generates a closure class
with an `operator()`.

```
[capture](parameters) mutable noexcept -> return_type { body }
```

Only the capture list and body are mandatory; the return type is deduced when
omitted.

### Capture list

| Form | Meaning |
|---|---|
| `[]` | capture nothing |
| `[=]` | capture everything used, by value (copy) |
| `[&]` | capture everything used, by reference |
| `[x]` | capture `x` by value |
| `[&x]` | capture `x` by reference |
| `[=, &x]` | everything by value, except `x` by reference |
| `[&, x]` | everything by reference, except `x` by value |
| `[this]` | capture the enclosing object's `this` pointer |
| `[*this]` | capture a copy of the enclosing object (C++17) |
| `[x = expr]` | init-capture / generalized capture (C++14) |

```cpp
int main() {
    int a = 4, b = 5;

    [](int x, int y) { std::cout << x << ' ' << y << '\n'; }(a, b);   // params

    [=]() { std::cout << a << ' ' << b << '\n'; }();                  // by value

    [&]() { a = 100; }();                                             // by reference

    // []() { std::cout << a; }();      // ERROR: a not captured

    auto lam = [=]() mutable { a = 99; std::cout << a; };  // mutable: modify the COPY
    lam();                              // prints 99
    std::cout << a;                     // outer a unchanged

    auto owned = [p = std::make_unique<int>(7)]() { return *p; };     // C++14 init-capture
}
```

By default `operator()` is `const`, so value-captured variables are read-only.
`mutable` removes the `const` — it still modifies only the closure's copy.

### Dangling capture — the classic bug

```cpp
std::function<int()> makeBad() {
    int local = 42;
    return [&]() { return local; };     // UB: local dies when makeBad returns
}
std::function<int()> makeGood() {
    int local = 42;
    return [=]() { return local; };     // copy — safe
}
```

### Generic lambdas (C++14) and templated lambdas (C++20)

```cpp
auto add  = [](auto a, auto b) { return a + b; };           // C++14
auto add2 = []<typename T>(T a, T b) { return a + b; };     // C++20, enforces same T
std::cout << add(1, 2) << ' ' << add(1.5, 2.5) << ' ' << add(std::string("a"), std::string("b"));
```

### Return type

```cpp
auto f = [](int x) -> double { return x / 2.0; };   // explicit when deduction
                                                    // is ambiguous or narrowing
auto g = [](bool c) -> double { return c ? 1 : 2.5; };  // without -> it would not compile
```

### Lambdas with algorithms

```cpp
std::vector<int> v{5,3,8,1};

std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });      // descending
auto it = std::find_if(v.begin(), v.end(), [](int x) { return x > 4; });
int  n  = std::count_if(v.begin(), v.end(), [](int x) { return x % 2; });
std::for_each(v.begin(), v.end(), [](int x) { std::cout << x << ' '; });

int sum = 0;
std::for_each(v.begin(), v.end(), [&sum](int x) { sum += x; });
```

### for_each and range-for

```cpp
std::for_each(v.begin(), v.end(), [](int& x) { x *= 2; });

for (int x : v)            {}   // copy
for (int& x : v)           { x *= 2; }   // modify
for (const auto& x : v)    {}   // read-only, no copy — the default you want
for (auto&& x : v)         {}   // generic/forwarding form
```

### std::function

A type-erased wrapper that can hold any callable with a given signature: function
pointer, lambda, functor, bound member function.

```cpp
#include <functional>

std::function<int(int,int)> op;

op = [](int a, int b) { return a + b; };            // lambda
op = std::plus<int>{};                              // functor
int sub(int a, int b) { return a - b; }
op = sub;                                           // function pointer

struct S { int mul(int a, int b) { return a*b; } };
S s;
op = std::bind(&S::mul, &s, std::placeholders::_1, std::placeholders::_2);
op = [&s](int a, int b) { return s.mul(a,b); };     // clearer than bind
```

`std::function` costs an indirect call and possibly a heap allocation. Use
`auto` for the concrete lambda type when you do not need type erasure.

---

## 26. Templates and Generic Programming

A mechanism for writing code parameterized by type, so one definition serves many
types.

**Advantages:** code reuse; type safety checked at compile time; zero runtime cost
(no virtual dispatch).
**Disadvantages:** longer compile times and code bloat (one instantiation per type);
historically poor error messages (fixed by concepts).

### Function templates

```cpp
template <typename T>
void mySwap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

template <typename T>
T add(T a, T b) { return a + b; }

int main() {
    std::string s = "bhaskar", t = "jaiswal";
    mySwap(s, t);                       // T deduced as std::string
    int x = 10, y = 1;
    mySwap(x, y);                       // T deduced as int

    std::cout << add(3, 5);             // int
    std::cout << add(3.5, 2.1);         // double
    std::cout << add<double>(3, 2.1);   // explicit: forces double
}
```

Multiple parameters and a deduced return type:

```cpp
template <typename T1, typename T2>
auto add(T1 a, T2 b) -> decltype(a + b) { return a + b; }   // C++11
template <typename T1, typename T2>
auto add2(T1 a, T2 b) { return a + b; }                     // C++14
```

Non-type template parameters:

```cpp
template <typename T, std::size_t N>
class FixedArray {
    T data[N];
public:
    constexpr std::size_t size() const { return N; }
};
FixedArray<int, 10> fa;
```

### Class templates

```cpp
template <typename T>
class MyContainer {
    T data;
public:
    explicit MyContainer(T d) : data(d) {}
    void show() const { std::cout << "Data: " << data << '\n'; }
};

MyContainer<int>         a(42);
MyContainer<std::string> b("Kiran");
MyContainer              c(3.14);       // C++17 class template argument deduction
```

### Worked example: a hand-rolled vector

```cpp
template <typename T>
class MyVector {
    T*          buf   = nullptr;
    std::size_t len   = 0;
    std::size_t cap   = 0;

    void grow(std::size_t want) {
        if (want <= cap) return;
        std::size_t newCap = cap ? cap * 2 : 4;
        while (newCap < want) newCap *= 2;
        T* nb = static_cast<T*>(::operator new(newCap * sizeof(T)));
        for (std::size_t i = 0; i < len; ++i) {
            new (nb + i) T(std::move(buf[i]));      // placement new + move
            buf[i].~T();
        }
        ::operator delete(buf);
        buf = nb; cap = newCap;
    }

public:
    MyVector() = default;
    MyVector(std::size_t n, const T& v) { for (std::size_t i = 0; i < n; ++i) push_back(v); }

    ~MyVector() {
        for (std::size_t i = 0; i < len; ++i) buf[i].~T();
        ::operator delete(buf);
    }

    MyVector(const MyVector&)            = delete;   // keep the example short
    MyVector& operator=(const MyVector&) = delete;

    void push_back(const T& v) { grow(len + 1); new (buf + len) T(v); ++len; }
    void pop_back()            { if (len) buf[--len].~T(); }

    T&       operator[](std::size_t i)       { return buf[i]; }
    const T& operator[](std::size_t i) const { return buf[i]; }

    std::size_t size()     const { return len; }
    std::size_t capacity() const { return cap; }
    bool        empty()    const { return len == 0; }

    T*       begin()       { return buf; }          // enables range-for
    T*       end()         { return buf + len; }
    const T* begin() const { return buf; }
    const T* end()   const { return buf + len; }
};

int main() {
    MyVector<int> v(3, 7);
    for (int i = 0; i < 5; ++i) v.push_back(i);
    for (int x : v) std::cout << x << ' ';          // 7 7 7 0 1 2 3 4
}
```

This is the real shape of `std::vector`: contiguous buffer, geometric growth,
placement new for construction, explicit destructor calls.

### Templates and static members

The compiler generates a **separate class** per type argument, so each
instantiation has its own copy of every static member.

```cpp
template <typename T>
class Test {
public:
    static int count;
    Test() { ++count; }
};

template <typename T> int Test<T>::count = 0;       // out-of-class definition

int main() {
    Test<int>    a, b;
    Test<double> c;
    std::cout << Test<int>::count    << '\n';       // 2
    std::cout << Test<double>::count << '\n';       // 1
}
```

### Full specialization

A completely custom implementation for one exact type.

```cpp
template <typename T>
class Printer {
public:
    void print(T data) { std::cout << "Generic: " << data << '\n'; }
};

template <>                                 // note: empty <>
class Printer<char*> {
public:
    void print(char* data) { std::cout << "String: " << data << '\n'; }
};

template <>
class Printer<int*> {
public:
    void print(int* data) { std::cout << "int: " << *data << '\n'; }
};

int main() {
    Printer<int>  p1; p1.print(10);         // Generic: 10
    int k = 7;
    Printer<int*> p2; p2.print(&k);         // int: 7
}
```

Functions can also be fully specialized:

```cpp
template <typename T> void print(T data)     { std::cout << data << '\n'; }
template <>           void print(int* data)  { std::cout << "int: " << *data << '\n'; }
```

### Partial specialization

A custom implementation for a *pattern* of types. **Class templates only** —
function templates cannot be partially specialized (use overloading instead).

```cpp
template <typename T>
class MyContainer { public: void show() { std::cout << "General type\n"; } };

template <typename T>                       // any pointer type
class MyContainer<T*> { public: void show() { std::cout << "Pointer type\n"; } };

MyContainer<int>  o1; o1.show();            // General type
MyContainer<int*> o2; o2.show();            // Pointer type
```

```cpp
template <typename T1, typename T2>
class Pair { public: void show() { std::cout << "General Pair\n"; } };

template <typename T>                       // both types identical
class Pair<T, T> { public: void show() { std::cout << "Both types same\n"; } };

Pair<int,double> p1; p1.show();             // General Pair
Pair<int,int>    p2; p2.show();             // Both types same
```

Workaround for functions — SFINAE with `enable_if`:

```cpp
#include <type_traits>

template <typename T>
std::enable_if_t<!std::is_pointer_v<T>> print(T v) { std::cout << "Non-pointer: " << v << '\n'; }

template <typename T>
std::enable_if_t<std::is_pointer_v<T>>  print(T v) { std::cout << "Pointer: " << *v << '\n'; }
```

Or, far cleaner, C++17 `if constexpr`:

```cpp
template <typename T>
void print(T v) {
    if constexpr (std::is_pointer_v<T>) std::cout << "Pointer: " << *v << '\n';
    else                                std::cout << "Value: "   << v  << '\n';
}
```

| | Works for | Matches |
|---|---|---|
| Full specialization | class + function templates | one exact type |
| Partial specialization | class templates only | a pattern of types |

### Variadic templates

Accept zero or more template arguments.

```cpp
template <typename... Args>     // Args...  = type parameter pack
void print(Args... args);       // args...  = function parameter pack
```

Recursive expansion (pre-C++17):

```cpp
void print() { std::cout << '\n'; }                 // base case

template <typename T, typename... Rest>
void print(T first, Rest... rest) {
    std::cout << first << ' ';
    print(rest...);                                 // peel one, recurse
}

print(10, 20.5, "C++", 'A');                        // 10 20.5 C++ A
```

Fold expressions (C++17) — no recursion, no base case:

```cpp
template <typename... Args>
void print(Args... args) { (std::cout << ... << args) << '\n'; }
//                          ^ left fold: ((cout << a1) << a2) << a3

template <typename... Args>
auto sum(Args... args) { return (args + ... + 0); }     // binary right fold

template <typename... Args>
bool allTrue(Args... args) { return (args && ...); }

sizeof...(Args)                 // number of arguments in the pack
```

Perfect-forwarding factory:

```cpp
template <typename T, typename... Args>
T create(Args&&... args) { return T(std::forward<Args>(args)...); }
```

### Type traits (`<type_traits>`)

```cpp
std::is_integral_v<T>
std::is_pointer_v<T>
std::is_same_v<T, U>
std::is_base_of_v<Base, Derived>
std::remove_reference_t<T>
std::remove_const_t<T>
std::decay_t<T>
std::conditional_t<cond, A, B>

static_assert(std::is_integral_v<T>, "T must be an integer type");
```

### Concepts (C++20)

A named compile-time predicate constraining template arguments. Replaces SFINAE
and produces readable errors.

The problem before C++20:

```cpp
template <typename T> T add(T a, T b) { return a + b; }
add(2, 3);          // fine
add("a", "b");      // pages of unreadable template errors
```

The fix:

```cpp
#include <concepts>

template <std::integral T>
T add(T a, T b) { return a + b; }

add(2, 3);          // OK
// add(2.5, 3.5);   // error: constraint not satisfied — 'double' is not integral
```

Equivalent spellings:

```cpp
template <typename T> requires std::integral<T>
T add(T a, T b) { return a + b; }

auto add(std::integral auto a, std::integral auto b) { return a + b; }  // abbreviated
```

Custom concept:

```cpp
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a.size() } -> std::same_as<std::size_t>;      // also require a size() member
};

template <Addable T> T twice(T x) { return x + x; }
```

Standard concepts worth knowing:

| Concept | Meaning |
|---|---|
| `std::integral<T>` | integer type |
| `std::floating_point<T>` | float/double/long double |
| `std::same_as<T,U>` | identical types |
| `std::convertible_to<From,To>` | implicit conversion exists |
| `std::derived_from<D,B>` | D inherits from B |
| `std::invocable<F,Args...>` | callable with those arguments |
| `std::default_initializable<T>` | default-constructible |
| `std::copyable<T>` / `std::movable<T>` | supports copy / move |
| `std::totally_ordered<T>` | supports `< <= > >= == !=` |

### Why templates live in headers

The compiler instantiates a template only where it sees both the definition and
the concrete type. If the definition sits in a `.cpp` and another TU uses
`add<int>`, the linker reports `undefined reference`.

Options:
1. Define templates in the header (normal practice).
2. Put definitions in a `.tpp`/`.ipp` included at the end of the header.
3. Explicit instantiation in one `.cpp` when the type set is known and closed:

```cpp
// mytemplate.cpp
template class MyContainer<int>;        // force these instantiations here
template class MyContainer<double>;
```

### CRTP — static polymorphism

Curiously Recurring Template Pattern: compile-time polymorphism with no vtable.

```cpp
template <typename Derived>
class Shape {
public:
    void draw() { static_cast<Derived*>(this)->drawImpl(); }
};

class Circle : public Shape<Circle> {
public:
    void drawImpl() { std::cout << "circle\n"; }
};

Circle c; c.draw();     // resolved at compile time, fully inlinable
```

---

## 27. Initialization Rules

### Uniform initialization `{}` (C++11)

One syntax for everything, and it forbids narrowing.

```cpp
int a = 5;      // copy initialization
int b(10);      // direct initialization
int c{15};      // list (uniform) initialization — preferred
```

| Context | Example |
|---|---|
| built-in | `int x{10};` |
| class | `Point p{1, 2};` |
| array | `int arr[]{1,2,3};` |
| container | `std::vector<int> v{1,2,3};` |
| member init | `A obj{10, 20};` |
| `new` | `auto p = new A{5, 6};` |
| zero-init | `int x{};` (0), `T obj{};` |

Narrowing is rejected:

```cpp
int a = 3.14;       // OK, silently truncates to 3
int b{3.14};        // ERROR: narrowing conversion
char c{300};        // ERROR
```

### The initializer_list trap

If a class has a `std::initializer_list` constructor, `{}` **strongly** prefers
it over every other constructor.

```cpp
#include <initializer_list>

struct A {
    A(int, int)                        { std::cout << "Normal constructor\n"; }
    A(std::initializer_list<int>)      { std::cout << "Initializer list constructor\n"; }
};

A a(1, 2);      // Normal constructor
A b{1, 2};      // Initializer list constructor
```

The classic bite:

```cpp
std::vector<int> v1(5, 10);     // 5 elements, each 10   -> {10,10,10,10,10}
std::vector<int> v2{5, 10};     // 2 elements            -> {5,10}
```

### Most Vexing Parse

Anything that *can* be parsed as a function declaration *is* parsed as one.

```cpp
class MyClass { public: MyClass() { std::cout << "Constructed\n"; } };

int main() {
    MyClass a();    // declares a FUNCTION `a` returning MyClass — no object!
    MyClass b;      // constructs
    MyClass c{};    // constructs
}
// Output: Constructed / Constructed   (only b and c)
```

A nastier form:

```cpp
std::string s("x");
std::ifstream f("in.txt");
// std::vector<int> v(std::istream_iterator<int>(f), std::istream_iterator<int>());
//   ^ parsed as a function declaration
std::vector<int> v{std::istream_iterator<int>(f), std::istream_iterator<int>()};   // fix
```

Rule: use `{}` for object construction and the ambiguity disappears.

### Initialization kinds

```cpp
int a;          // default    — indeterminate for built-ins at block scope
int b{};        // value      — zero
int c = 5;      // copy
int d(5);       // direct
int e{5};       // list/direct-list

static int f;   // zero-initialized (static storage duration)

struct P { int x, y; };
P p1;           // members indeterminate
P p2{};         // both zero
P p3{1};        // x=1, y=0  (remaining members value-initialized)
```

Partial aggregate initialization zero-fills the rest:

```cpp
struct Temp { int a, b, c; };
Temp t = {10};      // a=10, b=0, c=0
int  arr[5] = {1};  // 1, 0, 0, 0, 0
int  z[5]   = {};   // all zero
```

### Designated initializers (C++20)

```cpp
struct Config { int width = 0; int height = 0; bool fullscreen = false; };
Config c{ .width = 800, .height = 600, .fullscreen = true };
```

### Structured bindings (C++17)

```cpp
std::map<std::string,int> m{{"a",1},{"b",2}};
for (const auto& [key, value] : m) std::cout << key << '=' << value << '\n';

auto [q, r] = std::div(17, 5);
struct P { int x, y; };
P p{1,2};
auto [px, py] = p;
```

---

## 28. Standard Version Feature Map

### C++11 — the big one

- `auto`, `decltype`
- range-based `for`
- lambdas
- rvalue references, move semantics, `std::move`, `std::forward`
- smart pointers: `unique_ptr`, `shared_ptr`, `weak_ptr`
- `nullptr` (replaces `NULL`/`0`)
- uniform initialization `{}`, `std::initializer_list`
- `constexpr` (single return)
- `override`, `final`, `= default`, `= delete`
- variadic templates
- `enum class` (scoped, strongly typed)
- `static_assert`
- `thread_local`; `<thread>`, `<mutex>`, `<atomic>`, `<future>`
- `std::array`, `unordered_map`, `unordered_set`, `std::tuple`
- `using` type aliases (template-friendly `typedef`)
- delegating and inheriting constructors
- thread-safe function-local static initialization

```cpp
enum class Color : uint8_t { Red, Green };  // no implicit int conversion, scoped
Color c = Color::Red;
using IntVec = std::vector<int>;            // beats typedef
template <typename T> using Vec = std::vector<T>;   // typedef cannot do this
```

### C++14

- **Generic lambdas** — `[](auto a, auto b) { return a + b; }`
- **Return type deduction for normal functions** — `auto f() { return 42; }`
- Relaxed `constexpr` (loops, locals, multiple returns)
- Variable templates — `template<typename T> constexpr T pi = T(3.1415926535);`
- `std::make_unique`
- Lambda init-capture — `[p = std::move(ptr)]() {}`
- `std::shared_timed_mutex`
- Binary literals `0b1010`, digit separators `1'000'000`
- `[[deprecated]]`

```cpp
// C++11 lambda
[](int a, int b) -> int { return a + b; };
// C++14 generic lambda — one closure serves every type
[](auto a, auto b) { return a + b; };
```

`std::shared_mutex` (C++17) / `std::shared_timed_mutex` (C++14) allow many
concurrent readers or one exclusive writer.

```cpp
std::shared_mutex m;
void read()  { std::shared_lock lock(m); /* many readers in parallel */ }
void write() { std::unique_lock lock(m); /* exclusive */ }
```

### C++17

- Structured bindings
- `if constexpr`
- Fold expressions
- Class template argument deduction — `std::pair p{1, 2.0};`
- `std::optional`, `std::variant`, `std::any`, `std::string_view`
- `std::filesystem`
- Parallel algorithms — `std::sort(std::execution::par, ...)`
- Guaranteed copy elision for prvalues
- `inline` variables
- `[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]`
- `if (init; cond)` / `switch (init; cond)`

```cpp
std::optional<int> parse(const std::string& s);
if (auto v = parse("42"); v.has_value()) std::cout << *v;

std::string_view sv = "no allocation, no copy";
```

### C++20

- **Concepts** and `requires`
- **Ranges** — `v | std::views::filter(pred) | std::views::transform(f)`
- **Coroutines** — `co_await`, `co_yield`, `co_return`
- **Modules** — `import std;` (replaces `#include`)
- Three-way comparison `<=>` (the spaceship operator)
- `std::span`, `std::format`, `std::jthread`, `std::bit_cast`, `<bit>`
- Designated initializers
- `consteval`, `constinit`
- `[[likely]]`, `[[unlikely]]`

```cpp
struct P {
    int x, y;
    auto operator<=>(const P&) const = default;     // generates < <= > >= == !=
};

#include <ranges>
auto evens = v | std::views::filter([](int n){ return n%2==0; })
               | std::views::transform([](int n){ return n*n; });
```

### C++23 (selected)

- `std::expected<T, E>`
- `std::print` / `std::println`
- `std::mdspan`, `std::flat_map`
- Deducing `this`
- `if consteval`
- `std::byteswap`

---
---

# PART IV — STL

The STL has four pillars: **containers** (store data), **iterators** (traverse
it), **algorithms** (operate on ranges via iterators), **functors/lambdas**
(customize algorithm behaviour). Allocators are the fifth, rarely touched.

## 29. Containers and Complexity

### Sequence containers

| Container | Storage | Backing structure | Iterator | Insert/erase | Random access | Notes |
|---|---|---|---|---|---|---|
| `array<T,N>` | contiguous | fixed C array | random | n/a | O(1) | stack, size fixed at compile time |
| `vector` | contiguous | dynamic array | random | O(n) middle, amortized O(1) back | O(1) | the default choice |
| `deque` | segmented | array of fixed blocks | random | O(1) front/back, O(n) middle | O(1) | fast push_front, not one contiguous block |
| `list` | non-contiguous | doubly linked list | bidirectional | O(1) *at an iterator*, O(n) to find | no, O(n) | stable iterators, splice |
| `forward_list` | non-contiguous | singly linked list | forward | O(1) after an iterator | no | minimal overhead |

### Associative (ordered) — red-black tree

| Container | Structure | Iterator | Search/insert/erase | Duplicates | Ordered |
|---|---|---|---|---|---|
| `set` | red-black tree | bidirectional | O(log n) | no | yes |
| `multiset` | red-black tree | bidirectional | O(log n) | yes | yes |
| `map` | red-black tree | bidirectional | O(log n) | no (keys) | yes |
| `multimap` | red-black tree | bidirectional | O(log n) | yes | yes |

### Unordered associative — hash table

| Container | Structure | Iterator | Average | Worst | Ordered |
|---|---|---|---|---|---|
| `unordered_set` | hash + buckets | forward | O(1) | O(n) | no |
| `unordered_multiset` | hash + buckets | forward | O(1) | O(n) | no |
| `unordered_map` | hash + buckets | forward | O(1) | O(n) | no |
| `unordered_multimap` | hash + buckets | forward | O(1) | O(n) | no |

Worst case O(n) happens when every key hashes into one bucket. Since C++11 the
buckets are chained lists (not open addressing).

### Container adaptors

| Adaptor | Default backing | Interface |
|---|---|---|
| `stack` | `deque` | LIFO: `push`, `pop`, `top` |
| `queue` | `deque` | FIFO: `push`, `pop`, `front`, `back` |
| `priority_queue` | `vector` + heap | max-heap by default: `push` O(log n), `top` O(1) |

```cpp
std::priority_queue<int> maxHeap;
std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
```

### vector: size vs capacity

```cpp
std::vector<int> v;
v.reserve(100);     // capacity >= 100, size still 0 — avoids reallocation
v.resize(10);       // size == 10, elements value-initialized
v.shrink_to_fit();  // non-binding request to release spare capacity
v.size(); v.capacity(); v.empty(); v.clear();
```

Growth is geometric (typically x2 or x1.5), which makes `push_back` amortized
O(1). **Any reallocation invalidates all iterators, pointers and references.**

```cpp
std::vector<int> v{1,2,3};
int* p = &v[0];
v.push_back(4);         // may reallocate
// *p                   // UB — p may dangle
```

### emplace vs insert/push

`emplace_back` constructs in place from the arguments; `push_back` constructs a
temporary and then moves/copies it.

```cpp
std::vector<std::pair<int,std::string>> v;
v.push_back(std::make_pair(1, "a"));    // build pair, then move
v.emplace_back(1, "a");                 // construct pair directly in the buffer
```

### Iterator invalidation rules

| Container | Insert | Erase |
|---|---|---|
| `vector` | all invalidated if reallocated; else from insertion point on | from the erased point onward |
| `deque` | all iterators (refs to elements survive at the ends) | all, unless erasing at an end |
| `list` / `forward_list` | none | only the erased element |
| `set`/`map` (tree) | none | only the erased element |
| `unordered_*` | all iterators invalidated on rehash; refs survive | only the erased element |

The erase-remove idiom (pre-C++20):

```cpp
v.erase(std::remove(v.begin(), v.end(), 42), v.end());
v.erase(std::remove_if(v.begin(), v.end(), [](int x){ return x < 0; }), v.end());

std::erase(v, 42);                                      // C++20
std::erase_if(v, [](int x){ return x < 0; });           // C++20
```

Erasing inside a loop:

```cpp
for (auto it = m.begin(); it != m.end(); ) {
    if (shouldDrop(*it)) it = m.erase(it);      // erase returns the next iterator
    else                 ++it;
}
```

### Which container do I pick?

```
Need key -> value?
  order matters, or need range queries?   -> map / set
  pure lookup speed?                      -> unordered_map / unordered_set
Need a sequence?
  random access + mostly append?          -> vector   (default)
  frequent push_front and push_back?      -> deque
  many splices / stable references?       -> list
  fixed size known at compile time?       -> array
```

Default to `vector`. Linked lists lose to vectors on modern CPUs for most sizes
because of cache locality, even for "insert in the middle" workloads.

---

## 30. Iterators

An iterator is an object that behaves like a pointer into a container,
decoupling algorithms from container internals.

### Categories (weakest to strongest)

| Category | Operations | Containers |
|---|---|---|
| Input | read once, `++` | `istream_iterator` |
| Output | write once, `++` | `ostream_iterator`, `back_inserter` |
| Forward | read/write, `++`, multi-pass | `forward_list`, `unordered_*` |
| Bidirectional | `++`, `--` | `list`, `set`, `map`, `multiset`, `multimap` |
| Random access | `+n`, `-n`, `[]`, `<`, iterator arithmetic | `vector`, `deque`, `array`, raw pointers |
| Contiguous (C++17) | random access + guaranteed contiguous storage | `vector`, `array`, `string` |

Random access iterators are the most powerful and behave exactly like pointers.
An algorithm requiring random access (`std::sort`) cannot run on a `list` — which
is why `list` ships its own `sort()` member.

### Usage

```cpp
std::vector<int> v{1,2,3};

for (auto it = v.begin();  it != v.end();  ++it)  std::cout << *it;
for (auto it = v.rbegin(); it != v.rend(); ++it)  std::cout << *it;   // reverse
for (auto it = v.cbegin(); it != v.cend(); ++it)  /* *it is const */ ;

std::advance(it, 3);                    // works for any category
auto nx = std::next(it, 2);
auto pv = std::prev(it);
auto d  = std::distance(v.begin(), it); // O(1) random access, O(n) otherwise
```

### Stream iterators

```cpp
#include <iterator>

std::vector<int> v{1,2,3};
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));

std::vector<int> in;
std::copy(std::istream_iterator<int>(std::cin), std::istream_iterator<int>(),
          std::back_inserter(in));
```

### Insert iterators

```cpp
std::back_inserter(v)       // calls push_back
std::front_inserter(lst)    // calls push_front
std::inserter(s, s.begin()) // calls insert
```

### Writing a custom iterator

```cpp
template <typename T>
class RingBuffer {
    T* buf; std::size_t cap;
public:
    class iterator {
        T* p;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        explicit iterator(T* q) : p(q) {}
        reference operator*()  const { return *p; }
        pointer   operator->() const { return  p; }
        iterator& operator++()       { ++p; return *this; }
        iterator  operator++(int)    { auto t = *this; ++p; return t; }
        bool operator==(const iterator& o) const { return p == o.p; }
        bool operator!=(const iterator& o) const { return p != o.p; }
    };
    iterator begin() { return iterator(buf); }
    iterator end()   { return iterator(buf + cap); }
};
```

Supplying `begin()`/`end()` is all a type needs to work with range-based `for`
and the whole algorithm library.

---

## 31. map / unordered_map Internals

### std::map — red-black tree

- Self-balancing binary search tree.
- Search / insert / erase: O(log n) guaranteed (no bad case).
- Iteration yields keys in sorted order.
- Stable references: inserting or erasing never invalidates other elements.
- Better than a hash map for small n, for ordered traversal, and for range
  queries (`lower_bound` / `upper_bound`).

### Red-black tree properties

1. Every node is red or black.
2. The root is black.
3. Every leaf (the null sentinel) is black.
4. A red node has only black children (no two reds in a row).
5. Every simple path from a node to its descendant leaves contains the same
   number of black nodes (equal "black height").

These invariants bound the longest path at twice the shortest, guaranteeing
O(log n) height. Insert/erase restore them with recolours plus at most a constant
number of rotations.

### Red-black tree vs AVL tree

| | Red-black | AVL |
|---|---|---|
| Balance | loose (height <= 2 log n) | strict (height difference <= 1) |
| Search | O(log n), slightly slower | O(log n), slightly faster |
| Insert/erase | fewer rotations, cheaper | more rotations |
| Best for | mixed insert/search/delete workloads | search-dominant workloads |
| Used by | `std::map`, `std::set`, Linux CFS | in-memory databases, indexes |

### std::unordered_map — hash table

- Array of buckets; each bucket is a chained list of nodes.
- Index = `hash(key) % bucket_count`.
- Average O(1), worst O(n) when all keys collide.
- **Rehashing** occurs when `load_factor() > max_load_factor()` (default 1.0);
  the bucket array grows and all keys are redistributed — this invalidates all
  iterators (but not references to elements).

```cpp
std::unordered_map<std::string,int> m;
m.reserve(1000);                        // pre-size to avoid rehashing
m.max_load_factor(0.7f);
std::cout << m.bucket_count() << ' ' << m.load_factor();
```

Custom hash:

```cpp
struct Point { int x, y; bool operator==(const Point& o) const { return x==o.x && y==o.y; } };

struct PointHash {
    std::size_t operator()(const Point& p) const noexcept {
        return std::hash<int>{}(p.x) ^ (std::hash<int>{}(p.y) << 1);
    }
};

std::unordered_map<Point, int, PointHash> grid;
```

### map vs unordered_map

| | `map` | `unordered_map` |
|---|---|---|
| Structure | red-black tree | hash table |
| Lookup | O(log n) always | O(1) average, O(n) worst |
| Ordering | sorted by key | none |
| Key needs | `operator<` (or comparator) | `std::hash` + `operator==` |
| Memory | lower per element | higher (bucket array) |
| Iterator stability | very stable | invalidated by rehash |
| Choose when | need order / range queries / predictable latency | need raw lookup speed |

### Access patterns

```cpp
std::map<std::string,int> m;

m["a"] = 1;                     // operator[] INSERTS a default if missing
int x = m["missing"];           // silently creates {"missing", 0}
int y = m.at("a");              // throws std::out_of_range if absent

if (m.count("a")) { }
if (m.find("a") != m.end()) { } // avoids the accidental insert
if (m.contains("a")) { }        // C++20

auto [it, inserted] = m.insert({"b", 2});       // does NOT overwrite
m.insert_or_assign("b", 3);                     // C++17, does overwrite
m.emplace("c", 4);
m.try_emplace("d", 5);                          // C++17, no temporary if present

auto lo = m.lower_bound("b");   // first key >= "b"   (ordered map only)
auto hi = m.upper_bound("b");   // first key >  "b"
```

`operator[]` requires a default-constructible mapped type and is non-const.
Use `find`/`at`/`contains` for pure lookup.

---

## 32. Strings and Character Arrays

### std::string

```cpp
#include <string>
std::string s = "hello world";
```

**Capacity and access**

```cpp
s.size();  s.length();      // same thing
s.capacity();               // allocated space
s.empty();
s.resize(20);
s.reserve(100);
s[0];                       // no bounds check
s.at(0);                    // bounds checked, throws std::out_of_range
s.front(); s.back();
s.c_str();                  // const char*, null-terminated
s.data();                   // char* since C++17
```

**Search**

```cpp
s.find("world");            // index of first occurrence, or std::string::npos
s.rfind("o");               // last occurrence
s.find_first_of("aeiou");   // first index of ANY char in the set
s.find_last_of("aeiou");
s.find_first_not_of(" \t"); // first non-whitespace — handy for trimming
s.starts_with("he");        // C++20
s.ends_with("ld");          // C++20
s.contains("wor");          // C++23
```

**Modification**

```cpp
s.append(" again");   s += "!";
s.insert(5, "XYZ");
s.erase(5, 3);                  // erase 3 chars from index 5
s.replace(0, 5, "HELLO");
s.substr(6, 5);                 // "world"
s.push_back('!');   s.pop_back();
s.clear();
```

**Comparison**

```cpp
s1 == s2;  s1 < s2;             // lexicographic
s1.compare(s2);                 // <0, 0, >0
```

**Conversion**

```cpp
std::stoi("42");   std::stol("42");   std::stoll("42");
std::stod("3.14"); std::stof("3.14");
std::to_string(42);
std::stoi("0x1F", nullptr, 16);         // base 16
// std::from_chars / std::to_chars (C++17) — fastest, no allocation, no locale
```

**Splitting and joining**

```cpp
std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) out.push_back(item);
    return out;
}
```

### std::string_view (C++17)

A non-owning `{pointer, length}` view. No allocation, no copy. Perfect for
read-only function parameters.

```cpp
void log(std::string_view msg) { std::cout << msg << '\n'; }

log("literal");             // no std::string temporary is created
std::string s = "abc";
log(s);                     // no copy
std::string_view sv = s;
sv.remove_prefix(1);        // O(1) — just moves the pointer
```

Danger: a `string_view` never owns. Never let it outlive its backing buffer, and
never assume it is null-terminated (do not hand `.data()` to a C API).

```cpp
std::string_view bad() { std::string tmp = "x"; return tmp; }   // dangling
```

### C-style strings (`<cstring>`)

```cpp
char buf[64] = "hello";

std::strlen(buf);                   // length, NOT counting '\0'
std::strcpy(dst, src);              // UNSAFE: no bounds check
std::strncpy(dst, src, n);          // may not null-terminate — check yourself
std::strcat(dst, src);              // append
std::strncat(dst, src, n);
std::strcmp(a, b);                  // <0 / 0 / >0
std::strncmp(a, b, n);
std::strchr(buf, 'l');              // pointer to first 'l'
std::strstr(buf, "ell");            // substring search
std::memcpy(d, s, n);               // raw bytes, regions must not overlap
std::memmove(d, s, n);              // handles overlap
std::memset(buf, 0, sizeof buf);
std::snprintf(buf, sizeof buf, "%d", 42);   // safe formatting
```

`sizeof(buf)` is the array size (64); `strlen(buf)` is the logical length (5).
Always allocate `strlen(s) + 1` bytes to hold the terminator.

### std::string vs char array

| | `std::string` | `char[]` |
|---|---|---|
| Length | tracked, O(1) | `strlen`, O(n) |
| Memory | automatic, grows | fixed or manual |
| Bounds safety | `.at()` throws | none |
| Concatenation | `+`, `+=` | `strcat`, overflow-prone |
| Terminator | managed internally | you must maintain `'\0'` |
| Small strings | SSO: short strings live inline, no heap | n/a |

Small String Optimization: most implementations store strings up to ~15
characters inside the object itself, avoiding any allocation.

---

## 33. Algorithms Cheat Sheet

All take an iterator range `[first, last)` — `last` is one past the end.

### Non-modifying

```cpp
std::find(b, e, val);
std::find_if(b, e, pred);
std::count(b, e, val);           std::count_if(b, e, pred);
std::all_of(b, e, pred);         std::any_of(b, e, pred);   std::none_of(b, e, pred);
std::equal(b1, e1, b2);
std::mismatch(b1, e1, b2);
std::search(b1, e1, b2, e2);     // find a subsequence
std::min_element(b, e);          std::max_element(b, e);    std::minmax_element(b, e);
std::accumulate(b, e, 0);                    // <numeric>
std::inner_product(b1, e1, b2, 0);           // <numeric>
std::reduce(std::execution::par, b, e);      // C++17 parallel
```

### Modifying

```cpp
std::copy(b, e, out);            std::copy_if(b, e, out, pred);
std::move(b, e, out);
std::transform(b, e, out, f);    std::transform(b1,e1,b2,out,binaryF);
std::fill(b, e, val);            std::generate(b, e, gen);
std::replace(b, e, old, nw);     std::replace_if(b, e, pred, nw);
std::remove(b, e, val);          // does NOT shrink — pair with erase
std::unique(b, e);               // collapses ADJACENT duplicates — sort first
std::reverse(b, e);              std::rotate(b, mid, e);
std::shuffle(b, e, rng);
std::iota(b, e, 0);              // fill 0,1,2,...   <numeric>
```

### Sorting and ordered-range operations

```cpp
std::sort(b, e);                         // introsort, O(n log n), NOT stable
std::sort(b, e, cmp);
std::stable_sort(b, e);                  // O(n log^2 n), preserves equal order
std::partial_sort(b, mid, e);            // top-k
std::nth_element(b, nth, e);             // O(n) average, partitions around nth
std::is_sorted(b, e);

// require a sorted range:
std::binary_search(b, e, val);           // bool
std::lower_bound(b, e, val);             // first >= val
std::upper_bound(b, e, val);             // first >  val
std::equal_range(b, e, val);             // {lower, upper}
std::merge(b1, e1, b2, e2, out);
std::set_union(...); std::set_intersection(...); std::set_difference(...);
```

### Heap

```cpp
std::make_heap(b, e);
std::push_heap(b, e);       // after push_back
std::pop_heap(b, e);        // moves max to e-1, then pop_back
std::sort_heap(b, e);
```

### Partition

```cpp
std::partition(b, e, pred);             // pred-true elements first
std::stable_partition(b, e, pred);
std::partition_point(b, e, pred);
```

### Comparator contract

A comparator must be a **strict weak ordering**: irreflexive
(`cmp(a,a) == false`), antisymmetric, transitive. Using `<=` instead of `<`
violates it and `std::sort` can read out of bounds (UB, real crashes).

```cpp
std::sort(v.begin(), v.end(), [](int a, int b) { return a <= b; });  // UB
std::sort(v.begin(), v.end(), [](int a, int b) { return a <  b; });  // correct
```

### Ranges (C++20)

```cpp
#include <ranges>
namespace rv = std::views;

std::vector<int> v{1,2,3,4,5,6};

std::ranges::sort(v);                           // no .begin()/.end() needed
auto result = v | rv::filter([](int n){ return n % 2 == 0; })
                | rv::transform([](int n){ return n * n; })
                | rv::take(2);                  // lazy, composable
for (int x : result) std::cout << x << ' ';     // 4 16
```

---
---

# PART V — SYSTEMS AND RUNTIME

## 34. Exception Handling and Stack Unwinding

An exception is an abnormal condition encountered during execution.

- **Synchronous** — caused by the program's own logic or input: divide by zero,
  bad parse, out-of-range index.
- **Asynchronous** — external: disk failure, keyboard interrupt, signal.
  (C++ exceptions cannot portably catch these; signals are a separate mechanism.)

### try / throw / catch

```cpp
try {
    if (bad) throw std::runtime_error("boom");
}
catch (const std::invalid_argument& e) { /* most derived first */ }
catch (const std::runtime_error& e)    { std::cout << e.what(); }
catch (const std::exception& e)        { /* base — put it later */ }
catch (...)                            { /* catch-all — must be last */ }
```

`throw` terminates the current function immediately and starts searching outward
for a matching handler.

### Matching rules

1. Handlers are tried **in source order** — put the most derived type first. An
   earlier `catch (const std::exception&)` would swallow everything after it.
2. **No implicit conversions** are applied when matching (unlike overload
   resolution). Only derived-to-base, and cv/reference adjustments.

```cpp
try { throw 10; }                       // throws an int
catch (char* e)  { std::cout << "Caught " << e; }   // NOT matched — no int->char*
catch (...)      { std::cout << "Default Exception\n"; }   // this one runs
```

3. If nothing catches it, `std::terminate()` is called and the program aborts.
4. `try`/`catch` blocks can nest; rethrow with a bare `throw;` to preserve the
   original exception (including its dynamic type).

```cpp
try { inner(); }
catch (const std::exception&) {
    log("failed");
    throw;                  // rethrow the SAME object; `throw e;` would slice it
}
```

5. Objects constructed inside the `try` block are destroyed before the handler
   runs.

### Standard exception hierarchy

```
std::exception
├── std::logic_error
│   ├── std::invalid_argument
│   ├── std::domain_error
│   ├── std::length_error
│   └── std::out_of_range
├── std::runtime_error
│   ├── std::range_error
│   ├── std::overflow_error
│   ├── std::underflow_error
│   └── std::system_error
├── std::bad_alloc
├── std::bad_cast
└── std::bad_typeid
```

Always catch by `const` reference. Catching by value slices the exception object.

### Stack unwinding

When an exception propagates, the runtime pops stack frames one at a time,
calling the destructor of every fully-constructed automatic object in each frame,
until it finds a matching handler.

```cpp
void func3() {
    std::cout << "In func3\n";
    throw std::runtime_error("Error occurred!");
    std::cout << "End of func3\n";          // never executed
}
void func2() { std::cout << "In func2\n"; func3(); std::cout << "End of func2\n"; }
void func1() {
    std::cout << "In func1\n";
    try { func2(); }
    catch (const std::exception& e) { std::cout << "Caught exception: " << e.what() << '\n'; }
}
int main() { func1(); }
```

```
In func1
In func2
In func3
Caught exception: Error occurred!
```

`func3` threw -> frames for `func3` and `func2` were unwound (their locals
destroyed) -> a handler was found in `func1` -> execution resumed after the catch.

### Destructors during unwinding — in reverse order

```cpp
struct Demo {
    std::string name;
    explicit Demo(std::string n) : name(std::move(n)) { std::cout << "Construct " << name << '\n'; }
    ~Demo() { std::cout << "Destruct " << name << '\n'; }
};

void foo() {
    Demo a("A");
    Demo b("B");
    throw std::runtime_error("Exception!");
}
int main() {
    try { foo(); } catch (...) { std::cout << "Exception caught in main\n"; }
}
```

```
Construct A
Construct B
Destruct B          <- reverse order of construction
Destruct A
Exception caught in main
```

This is precisely why RAII works: unwinding guarantees cleanup on the error path
without a single explicit `delete`.

### No unwinding without an exception

`std::exit`, `std::abort`, `std::quick_exit` and a segfault do **not** unwind —
destructors of locals never run. Only a thrown exception (or a normal return)
unwinds.

### Destructors must not throw

If a second exception escapes a destructor while unwinding is already in
progress, `std::terminate()` is called.

```cpp
struct A { ~A() noexcept(false) { throw 1; } };     // dangerous

int main() {
    try { A a; throw 2; }           // throw 2 starts unwinding; ~A throws 1
    catch (...) { std::cout << "Caught\n"; }
}
// terminate called after throwing an instance of 'int'
```

Since C++11 destructors are implicitly `noexcept`. Catch and swallow (or log)
inside the destructor instead.

### noexcept

```cpp
void f() noexcept;                  // promises not to throw; violating -> terminate
void g() noexcept(sizeof(int)==4);  // conditional
static_assert(noexcept(f()));       // operator form: query
```

Mark move constructors, move assignment, destructors and swap `noexcept` — the
standard library takes faster paths when it can rely on it.

### Exception safety guarantees

| Level | Promise |
|---|---|
| No-throw | never throws (`noexcept`) |
| Strong | operation either fully succeeds or leaves state unchanged (commit-or-rollback) |
| Basic | no leaks, invariants hold, but state may have changed |
| None | anything goes — avoid |

Copy-and-swap gives the strong guarantee for assignment.

### Custom exceptions

```cpp
class MyError : public std::runtime_error {
    int code_;
public:
    MyError(const std::string& msg, int code)
        : std::runtime_error(msg), code_(code) {}
    int code() const noexcept { return code_; }
};

throw MyError("disk offline", 42);
```

### When not to use exceptions

Hot paths, embedded/kernel code, and codebases compiled with `-fno-exceptions`
use error codes, `std::optional`, or `std::expected<T,E>` (C++23) instead.

---

## 35. Multithreading

### Creating a thread

```cpp
#include <thread>
std::thread t(callable, args...);
```

The callable can be any of five things:

```cpp
// 1) function pointer
void foo(int z) { for (int i = 0; i < z; ++i) std::cout << "fn ptr\n"; }
std::thread t1(foo, 3);

// 2) lambda
auto f = [](int x) { for (int i = 0; i < x; ++i) std::cout << "lambda\n"; };
std::thread t2(f, 3);

// 3) function object (functor)
class Worker { public: void operator()(int x) const { std::cout << "functor\n"; } };
std::thread t3(Worker(), 3);

// 4) non-static member function — pass the object as the second argument
class Base { public: void foo() { std::cout << "member\n"; } };
Base b;
std::thread t4(&Base::foo, &b);

// 5) static member function — no object needed
class Base2 { public: static void foo1() { std::cout << "static member\n"; } };
std::thread t5(&Base2::foo1);

t1.join(); t2.join(); t3.join(); t4.join(); t5.join();
```

### join vs detach

```cpp
t.join();       // block until t finishes
t.detach();     // let it run independently; you can never join it again
t.joinable();   // true if neither joined nor detached
```

**Every thread must be joined or detached before its `std::thread` object is
destroyed, or `std::terminate` is called.** C++20's `std::jthread` joins in its
destructor and supports cooperative cancellation:

```cpp
#include <thread>
std::jthread jt([](std::stop_token st) {
    while (!st.stop_requested()) { /* work */ }
});
// destructor requests stop and joins automatically
```

Arguments are **copied** into the thread by default. Use `std::ref` to pass a
reference:

```cpp
void worker(int& counter);
int c = 0;
std::thread t(worker, std::ref(c));
```

### Data races and mutexes

A data race — two threads accessing the same memory, at least one writing,
without synchronization — is undefined behaviour.

```cpp
#include <mutex>

std::mutex mtx;
int shared = 0;

void increment() {
    std::lock_guard<std::mutex> lock(mtx);  // RAII: unlocks on scope exit
    ++shared;
}                                            // even if an exception is thrown
```

### Lock types

| Type | Use |
|---|---|
| `std::lock_guard` | simplest RAII lock, no manual unlock |
| `std::unique_lock` | movable, deferred/timed locking, needed by `condition_variable` |
| `std::scoped_lock` (C++17) | locks multiple mutexes at once, deadlock-free |
| `std::shared_lock` (C++14) | shared (reader) lock for `shared_mutex` |

```cpp
std::unique_lock<std::mutex> lk(mtx, std::defer_lock);
lk.lock(); lk.unlock(); lk.lock();

std::scoped_lock lock(m1, m2);      // acquires both without deadlock
```

### Mutex flavours

| Mutex | Behaviour |
|---|---|
| `std::mutex` | basic exclusive |
| `std::recursive_mutex` | same thread may lock it repeatedly |
| `std::timed_mutex` | `try_lock_for` / `try_lock_until` |
| `std::shared_mutex` (C++17) | many readers **or** one writer |
| `std::shared_timed_mutex` (C++14) | shared + timed |

Reader/writer pattern:

```cpp
std::shared_mutex rw;
std::map<int,int> cache;

int  read(int k)          { std::shared_lock lk(rw); return cache.at(k); }   // concurrent
void write(int k, int v)  { std::unique_lock lk(rw); cache[k] = v; }         // exclusive
```

### Deadlock

Four necessary conditions: mutual exclusion, hold-and-wait, no preemption,
circular wait. Break any one.

```cpp
// DEADLOCK: thread 1 locks a then b; thread 2 locks b then a
void t1() { std::lock_guard la(a); std::lock_guard lb(b); }
void t2() { std::lock_guard lb(b); std::lock_guard la(a); }

// FIX 1: always acquire in the same global order
// FIX 2: acquire together
void safe() { std::scoped_lock lock(a, b); }
```

### condition_variable

For waiting on a state change instead of spinning.

```cpp
#include <condition_variable>
#include <queue>

std::mutex m;
std::condition_variable cv;
std::queue<int> q;
bool done = false;

void producer() {
    for (int i = 0; i < 10; ++i) {
        { std::lock_guard lk(m); q.push(i); }
        cv.notify_one();
    }
    { std::lock_guard lk(m); done = true; }
    cv.notify_all();
}

void consumer() {
    for (;;) {
        std::unique_lock lk(m);
        cv.wait(lk, [] { return !q.empty() || done; });   // predicate guards spurious wakeups
        if (q.empty() && done) return;
        int v = q.front(); q.pop();
        lk.unlock();
        std::cout << v << '\n';
    }
}
```

`cv.wait(lock, pred)` atomically releases the lock and sleeps; on wake it
re-acquires the lock and re-checks `pred`. **Always use the predicate overload** —
spurious wakeups are permitted by the standard.

### Atomics

Lock-free operations on single values.

```cpp
#include <atomic>

std::atomic<int>  counter{0};
counter++;                                  // atomic read-modify-write
counter.fetch_add(5, std::memory_order_relaxed);
int expected = 5;
counter.compare_exchange_strong(expected, 10);   // CAS

std::atomic<bool> flag{false};
flag.store(true, std::memory_order_release);
while (!flag.load(std::memory_order_acquire)) { }
```

Memory orders, weakest to strongest: `relaxed` (atomicity only, no ordering),
`acquire`/`release` (pairwise synchronization), `acq_rel`, `seq_cst` (default,
a single total order across all threads). Use `seq_cst` unless you have measured
a reason not to.

`volatile` is **not** an alternative — it provides neither atomicity nor ordering.

### future, promise, async

```cpp
#include <future>

std::future<int> f = std::async(std::launch::async, [] { return 42; });
std::cout << f.get();                       // blocks until ready; rethrows exceptions

std::promise<int> p;
std::future<int>  fut = p.get_future();
std::thread t([&p] { p.set_value(7); });
std::cout << fut.get();
t.join();

std::packaged_task<int(int)> task([](int x) { return x * 2; });
auto tf = task.get_future();
std::thread(std::move(task), 21).detach();
std::cout << tf.get();                      // 42
```

`std::async` without an explicit policy may run lazily on `get()`. Always pass
`std::launch::async` when you want real concurrency.

### call_once and thread-safe statics

```cpp
std::once_flag once;
void init() { std::call_once(once, [] { std::cout << "runs exactly once\n"; }); }

Logger& logger() { static Logger l; return l; }  // C++11 guarantees thread-safe init
```

### Complete example

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex io;

void safePrint(const std::string& s) {
    std::lock_guard<std::mutex> lk(io);
    std::cout << s << '\n';
}

int main() {
    std::vector<std::thread> pool;
    for (int i = 0; i < 4; ++i)
        pool.emplace_back([i] { safePrint("worker " + std::to_string(i)); });
    for (auto& t : pool) t.join();
}
```

Compile with `-pthread`.

### Practical rules

1. Prefer immutability and message passing over shared mutable state.
2. Hold locks for the shortest possible time; never call unknown code under a lock.
3. Never return a reference or pointer to data protected by a lock.
4. One mutex per logical invariant, not per variable.
5. Beware false sharing — pad hot per-thread counters to a cache line
   (`alignas(64)`).
6. Test with `-fsanitize=thread`.

---

## 36. File Handling

`<fstream>` provides three stream classes:

| Class | Direction |
|---|---|
| `std::ofstream` | write (create/truncate) |
| `std::ifstream` | read |
| `std::fstream` | read and write |

### Writing

```cpp
#include <fstream>

std::ofstream out("testout.txt");
if (out.is_open()) {
    out << "Welcome.\n";
    out << 42 << ' ' << 3.14 << '\n';
}                                   // destructor closes the file (RAII)
```

### Reading

```cpp
std::ifstream in("testout.txt");
if (!in) { std::cerr << "cannot open\n"; return 1; }

std::string line;
while (std::getline(in, line))      // line-by-line
    std::cout << line << '\n';

int x; double y;
in.clear(); in.seekg(0);
while (in >> x >> y) { }            // token-by-token
```

### Read an entire file

```cpp
std::ifstream f("in.txt");
std::string contents((std::istreambuf_iterator<char>(f)),
                      std::istreambuf_iterator<char>());
```

### Open modes

```cpp
std::ios::in        // read
std::ios::out       // write
std::ios::app       // append at end on every write
std::ios::ate       // seek to end after opening
std::ios::trunc     // truncate existing content
std::ios::binary    // no newline translation

std::fstream fio("sample.txt", std::ios::in | std::ios::out | std::ios::trunc);
std::ofstream logf("app.log", std::ios::app);
```

### Binary I/O

```cpp
struct Rec { int id; double val; };

Rec r{1, 2.5};
std::ofstream bo("data.bin", std::ios::binary);
bo.write(reinterpret_cast<const char*>(&r), sizeof r);
bo.close();

Rec back{};
std::ifstream bi("data.bin", std::ios::binary);
bi.read(reinterpret_cast<char*>(&back), sizeof back);
```

Only valid for trivially copyable types, and the layout is not portable across
compilers/architectures (padding, endianness).

### Positioning and state

```cpp
in.seekg(0, std::ios::end);   auto size = in.tellg();   in.seekg(0, std::ios::beg);
out.seekp(10);                                          // write position

in.good(); in.eof(); in.fail(); in.bad();
in.clear();                 // reset flags after a failed extraction
```

Check `eof()` *after* the read, never as a loop condition:

```cpp
while (!in.eof()) { in >> x; use(x); }      // BUG: processes the last value twice
while (in >> x)   { use(x); }               // correct
```

### std::filesystem (C++17)

```cpp
#include <filesystem>
namespace fs = std::filesystem;

fs::exists("a.txt");
fs::file_size("a.txt");
fs::create_directories("a/b/c");
fs::remove("a.txt");
fs::rename("a.txt", "b.txt");
for (const auto& e : fs::directory_iterator("."))
    std::cout << e.path() << '\n';
```

### C stdio, for reference

```cpp
FILE* f = std::fopen("a.txt", "r");
if (f) {
    char buf[256];
    while (std::fgets(buf, sizeof buf, f)) std::fputs(buf, stdout);
    std::fclose(f);
}
```

---

## 37. Design Patterns

A design pattern is a reusable solution to a recurring design problem. Three
families: **creational** (object construction), **structural** (composition),
**behavioural** (interaction).

### Singleton (creational)

Guarantees exactly one instance with a global access point. Used for loggers,
configuration and connection pools.

Recipe:
1. Private constructor — nobody else can construct it.
2. Deleted copy constructor and copy assignment.
3. A static accessor returning the single instance.

```cpp
class Singleton {
    Singleton() { std::cout << "Singleton instance created.\n"; }
public:
    Singleton(const Singleton&)            = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
        static Singleton instance;      // created on first use, destroyed at exit
        return instance;                // C++11: initialization is thread-safe
    }

    void showMessage() { std::cout << "Hello from Singleton!\n"; }
};

int main() {
    Singleton::getInstance().showMessage();
}
```

This is the **Meyers Singleton**. The function-local static also avoids the
static initialization order fiasco because construction is deferred to first use.

Caveats: a singleton is global mutable state — it hides dependencies, complicates
unit testing and can become a contention point. Prefer dependency injection when
you can.

### Factory Method (creational)

Decouples construction from use.

```cpp
class Shape { public: virtual void draw() = 0; virtual ~Shape() = default; };
class Circle : public Shape { public: void draw() override { std::cout << "circle\n"; } };
class Square : public Shape { public: void draw() override { std::cout << "square\n"; } };

std::unique_ptr<Shape> makeShape(const std::string& kind) {
    if (kind == "circle") return std::make_unique<Circle>();
    if (kind == "square") return std::make_unique<Square>();
    return nullptr;
}
```

### Builder (creational)

Constructs a complex object step by step — the readable alternative to a
constructor with ten parameters.

```cpp
class Pizza {
    std::string dough, sauce; std::vector<std::string> toppings;
public:
    class Builder;
};

class Pizza::Builder {
    Pizza p;
public:
    Builder& dough(std::string d) { p.dough = std::move(d); return *this; }
    Builder& sauce(std::string s) { p.sauce = std::move(s); return *this; }
    Builder& topping(std::string t) { p.toppings.push_back(std::move(t)); return *this; }
    Pizza build() { return std::move(p); }
};

auto pizza = Pizza::Builder{}.dough("thin").sauce("tomato").topping("basil").build();
```

### Observer (behavioural)

One-to-many notification.

```cpp
class Observer {
public:
    virtual void onEvent(int v) = 0;
    virtual ~Observer() = default;
};

class Subject {
    std::vector<std::weak_ptr<Observer>> obs;       // weak: no ownership cycle
public:
    void attach(std::shared_ptr<Observer> o) { obs.push_back(o); }
    void notify(int v) {
        for (auto it = obs.begin(); it != obs.end(); ) {
            if (auto sp = it->lock()) { sp->onEvent(v); ++it; }
            else it = obs.erase(it);                // drop dead observers
        }
    }
};
```

### Strategy (behavioural)

Swap an algorithm at runtime.

```cpp
class Sorter {
    std::function<void(std::vector<int>&)> strategy;
public:
    explicit Sorter(std::function<void(std::vector<int>&)> s) : strategy(std::move(s)) {}
    void run(std::vector<int>& v) { strategy(v); }
};

Sorter asc([](auto& v) { std::sort(v.begin(), v.end()); });
Sorter desc([](auto& v){ std::sort(v.begin(), v.end(), std::greater<>{}); });
```

### RAII (C++ specific, the most important one)

Already covered in section 24. Every resource — memory, file, lock, socket,
handle — should be owned by an object whose destructor releases it.

### PIMPL — Pointer to IMPLementation (structural)

Hides implementation details from the header, cutting compile-time coupling and
keeping the ABI stable.

```cpp
// widget.h
class Widget {
    class Impl;                             // forward declaration only
    std::unique_ptr<Impl> pImpl;
public:
    Widget();
    ~Widget();                              // MUST be defined in the .cpp
    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;
    void doWork();
};

// widget.cpp
class Widget::Impl {
public:
    int heavyState = 0;
    void doWork() { /* ... */ }
};

Widget::Widget() : pImpl(std::make_unique<Impl>()) {}
Widget::~Widget() = default;                // here Impl is complete
void Widget::doWork() { pImpl->doWork(); }
```

The destructor must be out-of-line: `unique_ptr`'s deleter needs the complete
type, which only exists in the `.cpp`.

### Others worth knowing

| Pattern | One-liner |
|---|---|
| Adapter | wrap an incompatible interface |
| Decorator | add behaviour by wrapping, not inheriting |
| Facade | a simple front for a complex subsystem |
| Command | encapsulate a request as an object (undo/redo) |
| State | behaviour changes with internal state |
| Visitor | add operations to a class hierarchy without editing it (`std::visit` + `std::variant`) |
| CRTP | compile-time polymorphism (section 26) |

---

## 38. Complexity Notation and Sorting

### Asymptotic notation

| Notation | Bound | Meaning |
|---|---|---|
| Big O — `O(f)` | upper | worst case; grows no faster than `f` |
| Big Omega — `Omega(f)` | lower | best case; grows at least as fast as `f` |
| Big Theta — `Theta(f)` | tight | both bounds; the exact growth rate |
| little o — `o(f)` | strict upper | grows strictly slower |

Growth order: `O(1) < O(log n) < O(n) < O(n log n) < O(n^2) < O(n^3) < O(2^n) < O(n!)`

Amortized complexity averages over a sequence of operations:
`vector::push_back` is O(n) on the reallocating call but O(1) amortized because
reallocation is geometric.

### Sorting algorithms

| Algorithm | Best | Average | Worst | Space | Stable | Notes |
|---|---|---|---|---|---|---|
| Bubble | O(n) | O(n^2) | O(n^2) | O(1) | yes | teaching only |
| Selection | O(n^2) | O(n^2) | O(n^2) | O(1) | no | fewest swaps: O(n) |
| Insertion | O(n) | O(n^2) | O(n^2) | O(1) | yes | excellent for small/nearly sorted |
| Merge | O(n log n) | O(n log n) | O(n log n) | O(n) | yes | predictable, good for linked lists |
| Quick | O(n log n) | O(n log n) | O(n^2) | O(log n) | no | fastest in practice, cache friendly |
| Heap | O(n log n) | O(n log n) | O(n log n) | O(1) | no | in-place, worst-case guaranteed |
| Counting | O(n+k) | O(n+k) | O(n+k) | O(k) | yes | small integer key range |
| Radix | O(nk) | O(nk) | O(nk) | O(n+k) | yes | fixed-width keys |
| Bucket | O(n+k) | O(n+k) | O(n^2) | O(n) | yes | uniformly distributed input |

**Stable** = equal elements keep their relative order.

`std::sort` is introsort: quicksort, switching to heapsort when recursion gets
too deep (kills the O(n^2) case) and to insertion sort for small partitions.
Not stable. `std::stable_sort` is a merge sort variant.

### Searching

| Algorithm | Time | Requires |
|---|---|---|
| Linear search | O(n) | nothing |
| Binary search | O(log n) | sorted, random access |
| Hash lookup | O(1) avg, O(n) worst | a hash function |
| BST search | O(log n) balanced, O(n) degenerate | ordering |

### Common data structure complexities

| Structure | Access | Search | Insert | Delete |
|---|---|---|---|---|
| Array | O(1) | O(n) | O(n) | O(n) |
| Sorted array | O(1) | O(log n) | O(n) | O(n) |
| Linked list | O(n) | O(n) | O(1)* | O(1)* |
| Stack / Queue | O(n) | O(n) | O(1) | O(1) |
| Hash table | n/a | O(1) avg | O(1) avg | O(1) avg |
| BST (balanced) | O(log n) | O(log n) | O(log n) | O(log n) |
| Binary heap | O(1) top | O(n) | O(log n) | O(log n) |

`*` given an iterator to the position; finding it is O(n).

---

## 39. C vs C++

| Aspect | C | C++ |
|---|---|---|
| Paradigm | procedural | multi-paradigm (procedural, OO, generic, functional) |
| Classes / objects | no (`struct` only, data) | yes |
| Overloading | no | functions and operators |
| Templates | no (macros) | yes |
| Exceptions | no (`errno`, return codes, `setjmp`) | yes |
| References | no | yes |
| Namespaces | no | yes |
| Memory | `malloc`/`free` | `new`/`delete`, smart pointers, RAII |
| Default args | no | yes |
| `struct` | data only, needs `struct` tag in C89 | full class, public by default |
| Type checking | weaker (implicit `void*` conversion) | stricter |
| I/O | `printf`/`scanf` | streams + `printf` |
| `const` | mostly a hint, not a constant expression | true constant, usable as array size |
| Inline | `inline` (C99) | `inline`, `constexpr`, templates |
| Standard library | libc | libc + STL |

### Things valid in C but not C++

```c
void* p = malloc(10);
int*  q = p;                // C: implicit void* conversion. C++: ERROR

char  c = 'abc';            // C: multi-char literal (impl-defined). C++: warning/error
int   f();                  // C: unspecified parameters. C++: means f(void)
struct S { int a; };
S s;                        // C: ERROR, needs `struct S s;`. C++: fine
const int n = 10;
int arr[n];                 // C99: VLA. C++: ERROR unless n is constexpr
```

### Interop

```cpp
// C++ calling C
extern "C" {
    #include "c_header.h"
}

// C calling C++: export with C linkage and no name mangling
extern "C" void cpp_entry(int x) { /* can use C++ internally */ }
```

C++ additions that cost nothing at runtime: `const`, references, templates,
namespaces, RAII, `constexpr`. Things that do cost: exceptions (unwind tables),
RTTI, virtual dispatch.

---

## 40. Gotchas, UB and Quick-Fire Answers

### printf and scanf return values

```c
printf("%d", printf("%d\n", 123));
// output:
// 123
// 4
```

The inner call writes `"123\n"` — four characters — so it returns 4, and the
outer call prints that. Drop the `\n` from the inner format string and it
returns 3. (The commonly repeated "answer 3" is only correct for
`printf("%d", 123)`.)

`printf` returns the number of characters written; `scanf` returns the number of
items successfully assigned.

```c
int a, b;
printf("%d", scanf("%d%d", &a, &b));    // prints 2 when both reads succeed
```

### Multi-character literal

```c
char ch = 'abc';        // implementation-defined; in practice keeps 'c'
```

### Partial aggregate initialization

```c
struct Temp { int a, b, c; };
struct Temp t = {10};   // a=10, b=0, c=0 — the rest are zero-initialized
```

### Modulus on floats

```cpp
// float f = 5.5 % 2;           // ERROR: % requires integral operands
double r = std::fmod(5.5, 2);   // 1.5     <cmath>
```

### Comma operator

Evaluates left to right and yields the **last** operand.

```cpp
int b = (1, 2, 3);      // b == 3
for (int i = 0, j = 9; i < j; ++i, --j) { }   // the useful application
```

### Pre- vs post-increment

`++i` increments then yields the value; `i++` must keep a copy of the old value
to return. For `int` optimizers make them identical; for iterators and
user-defined types `++i` avoids a copy. Default to `++i`.

### Alternative operator tokens

`and` `or` `not` `bitand` `bitor` `xor` `compl` `not_eq` are standard aliases for
`&&` `||` `!` `&` `|` `^` `~` `!=`. Legal, rarely used in C++.

### Code bloat

Producing unnecessary code or objects.

```cpp
std::string s1("www"), s2("google"), s3("com");
std::string addr = s1 + "." + s2 + "." + s3;    // 3 objects + temporaries
std::cout << "www.google.com\n";                // when the value is constant
```

It also refers to template instantiation blow-up: `MyVec<int>`, `MyVec<long>`,
`MyVec<short>` each generate a full copy of the code.

### Why `using namespace std;` is bad

It dumps thousands of names into the global scope. Any of them can collide with
your own identifiers (`count`, `distance`, `swap`, `data`, `size`, `begin`,
`left`) and the resulting ambiguity errors point at the wrong place. Worse, it
can silently change overload resolution. Never put it in a header. In a `.cpp`,
prefer targeted using-declarations:

```cpp
using std::cout;
using std::string;
// or just write std::
```

### Static member does not enlarge the object

```cpp
class A { public: int a; static int b; };
int A::b = 10;
// sizeof(A) == 4
```

A static member is a namespace-scope variable with a class-scoped name.

### Templates and headers

See section 26. Template definitions must be visible where they are instantiated,
so they live in headers (or use explicit instantiation).

### Print N times without loop or recursion

```cpp
class Abc { public: Abc() { std::cout << "ctor\n"; } };
int main() { Abc ob[10]; }      // 10 constructor calls
```

### Preventing operations

| Goal | Technique |
|---|---|
| Prevent copy | `= delete` copy ctor + copy assign |
| Prevent inheritance | `class X final` |
| Prevent override | `void f() final` |
| Prevent instantiation | pure virtual member, or private constructor |
| Prevent `&obj` | `X* operator&() = delete;` |
| Prevent stack allocation | private destructor |
| Prevent heap allocation | `static void* operator new(size_t) = delete;` |
| Prevent implicit conversion | `explicit` |

### Undefined behaviour checklist

```cpp
int  a[5]; a[5] = 1;                    // out of bounds
int* p = nullptr; *p;                   // null dereference
int  x; std::cout << x;                 // uninitialized read
delete p; delete p;                     // double free
int* q = new int; delete[] q;           // mismatched delete
int  i = INT_MAX; ++i;                  // signed overflow
int  z = 0; int y = 1 / z;              // division by zero
1 << 32;                                // shift >= width
(char*)"lit"[0] = 'x';                  // write to a string literal
i = i++ + ++i;                          // unsequenced modification (pre-C++17)
return;  /* from a non-void function */ // missing return value
std::vector<int> v; v[0];               // index into an empty vector
// dereferencing an invalidated iterator
// reading through a type-punned pointer (strict aliasing)
// a data race
```

UB means the compiler may assume it never happens — it can delete your null
checks. Build debug with `-fsanitize=address,undefined`.

### Sequencing

```cpp
f(i++, i++);            // unspecified argument evaluation order (still UB pre-C++17 for the same object)
a[i] = i++;             // UB before C++17
std::cout << i++ << i;  // unspecified before C++17; left-to-right since C++17
```

Write one side effect per statement.

### Integer pitfalls

```cpp
unsigned u = 0;
--u;                            // wraps to UINT_MAX (well-defined for unsigned)

for (size_t i = v.size() - 1; i >= 0; --i) { }   // INFINITE: size_t is never < 0
for (size_t i = v.size(); i-- > 0; )      { }    // correct

if (-1 < 1u) { }                // FALSE: -1 converts to a huge unsigned
int n = v.size();               // narrowing warning; prefer size_t / ssize
```

### Floating point

```cpp
if (0.1 + 0.2 == 0.3) { }               // false
if (std::fabs((0.1+0.2) - 0.3) < 1e-9) { }      // correct
std::cout << std::setprecision(17) << 0.1;      // 0.10000000000000001
```

Never use `==` on floats; never accumulate money in `double`.

### Quick-fire answers

- **Why is the destructor virtual?** So `delete base_ptr` destroys the derived
  part too.
- **Can a constructor be virtual?** No — no vtable exists yet. Use a `clone()`
  idiom.
- **Can a destructor be virtual?** Yes, and it should be in any polymorphic base.
- **Can a constructor throw?** Yes. If it does, the destructor is **not** called
  (the object never existed), but fully constructed members are destroyed.
- **What happens if `new` fails?** It throws `std::bad_alloc`, unless you use
  `new (std::nothrow)`.
- **Difference between `struct` and `class`?** Default access (`public` vs
  `private`) and default inheritance. Nothing else.
- **Can a class have a member of its own type?** Only a pointer or reference — a
  by-value member would be infinitely recursive.
- **What is the size of an empty class?** 1 byte, so distinct objects get
  distinct addresses.
- **Why does `std::vector` double?** Geometric growth makes `push_back`
  amortized O(1).
- **Why must move operations be `noexcept`?** Otherwise `vector` reallocation
  falls back to copying to preserve the strong exception guarantee.
- **What is the diamond problem?** Duplicate base subobject under multiple
  inheritance; fix with `virtual` inheritance.
- **`delete` vs `delete[]`?** They must match the `new` form; mismatching is UB.
- **`malloc` in C++?** It works but skips constructors — use `new` or, better, a
  container.
- **What is a dangling pointer?** A pointer to memory that has been freed or to
  an object whose lifetime ended.
- **What is a memory leak?** Allocated memory that is no longer reachable and
  never freed.

---

## Appendix A — Compiler and Tooling

```bash
# build
g++ -std=c++20 -Wall -Wextra -Wpedantic -g -O0 main.cpp -o app

# sanitizers (do not combine ASan and TSan)
g++ -fsanitize=address,undefined -g main.cpp -o app
g++ -fsanitize=thread            -g main.cpp -o app

# leaks and profiling
valgrind --leak-check=full --show-leak-kinds=all ./app
perf stat ./app
perf record ./app && perf report

# static analysis / formatting
clang-tidy main.cpp -- -std=c++20
cppcheck --enable=all main.cpp
clang-format -i main.cpp

# inspection
nm -C app | grep symbol
objdump -d -C app | less
readelf -d app
c++filt _Z3fooii
```

gdb essentials:

```
break main            run             next / step / finish
print expr            backtrace       info locals
watch var             continue        x/16xb &var
thread apply all bt   frame N         ptype var
```

## Appendix B — Header Quick Reference

| Header | Provides |
|---|---|
| `<iostream>` | `cin`, `cout`, `cerr` |
| `<fstream>` | file streams |
| `<sstream>` | `stringstream` |
| `<iomanip>` | `setw`, `setprecision`, `hex` |
| `<string>` / `<string_view>` | strings |
| `<vector> <list> <deque> <array> <forward_list>` | sequence containers |
| `<map> <set> <unordered_map> <unordered_set>` | associative containers |
| `<stack> <queue>` | adaptors |
| `<algorithm>` | sort, find, transform, ... |
| `<numeric>` | accumulate, iota, reduce, gcd |
| `<functional>` | `function`, `bind`, `plus`, `greater` |
| `<memory>` | smart pointers, `allocator` |
| `<utility>` | `pair`, `move`, `forward`, `swap` |
| `<tuple>` | `tuple`, `tie`, `apply` |
| `<type_traits>` | compile-time type queries |
| `<concepts>` | C++20 concepts |
| `<ranges>` | C++20 ranges and views |
| `<optional> <variant> <any>` | C++17 vocabulary types |
| `<thread> <mutex> <condition_variable> <atomic> <future>` | concurrency |
| `<chrono>` | clocks, durations |
| `<filesystem>` | paths, directory iteration |
| `<exception> <stdexcept>` | exception types |
| `<cstdint>` | `int32_t`, `uint64_t`, ... |
| `<climits> <cfloat> <limits>` | numeric limits |
| `<cmath>` | math functions |
| `<cstring>` | C string/memory functions |
| `<cstdlib>` | `malloc`, `atoi`, `rand`, `exit` |
| `<bit>` | C++20 `bit_cast`, `popcount`, `endian` |
| `<format>` | C++20 `std::format` |

## Appendix C — Style Rules Worth Following

1. Prefer `std::` containers and smart pointers over raw `new`/`delete`.
2. Follow the Rule of Zero; if you cannot, follow the Rule of Five.
3. Pass by `const&` for large read-only parameters, by value for cheap ones,
   by value + `std::move` when you intend to store the argument.
4. Mark `override` on every override, `const` on every non-mutating member,
   `noexcept` on moves/swaps/destructors, `explicit` on single-argument
   constructors.
5. Initialize every variable at the point of declaration.
6. Declare variables in the smallest scope that works.
7. Prefer `enum class` over plain `enum`.
8. Prefer `constexpr` over `#define`.
9. Prefer algorithms and range-for over hand-written index loops.
10. Never use `using namespace std;` in a header.
11. Compile with `-Wall -Wextra` and treat warnings as errors in CI.
12. One class per header; guard every header.
13. Make interfaces hard to use incorrectly: strong types, `explicit`, deleted
    functions.

---

*End of notes.*
