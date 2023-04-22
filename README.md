## functional
 A heavily vectorized C++17 two header lightweight library without any dependencies like CRT, Windows API, STL and others. 

# TL;DR description
malloc, sprintf, rand and much other crt/stl/... overhead implementation in plain C++ without any dependencies (not even OS dependable)

# Limits 
Currently sprintf supports such format specifiers: %u, %i, %d, %f (no custom precision), %c, %x, %X, %p, %s.

# Quick example (rand)
```cpp
int main() {
    CUniquePointer<CTrustedRandom> random = make_unique<CTrustedRandom>();

	random->srand(RandomSeed());

	for (int idx = 0; idx < 1024; idx++) {
		printf("%d%s", random->autorand(0, 70000), idx != 1023 ? ", " : "\n");
		random->srand(random->autorand(0, 1023));
	}
	
	random.reset();
	
	return 0;
}
```
Refer to examples.cpp for more example codes

# Noteworthy things
* I didn't test this code on other compilers than MSVC too much. Any issues related to compiling with another compiler than MSVC may be ignored by me, but you still may open it.
* Known bugs of sprintf: two format specifiers can't stand near each other: %d%s, %f%X &c
* CTrustedRandom is a pseudo-RNG

# Contributing
Please refer to include/functional.hpp, I made an instruction for you which you need to follow in case of opening a pull request.