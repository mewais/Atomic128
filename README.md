# Atomic128
This is a 128-bit generic atomic library for C++20 and above. It is based on inline assembly (CMPXCHG16B) and/or compiler intrinsics, and so is definitely only for x86_64. This choice was mainly made because GCC's atomic int128 requires linking with libatomic, something I was not keen on doing. So I wrote this up with support for GCC, Clang, as well MSVC.

## Status
This was only tested on GCC and Clang, I have not yet tested it with MSVC.

## Installation
The library is header-only. If you want, you can simply copy it and stick it in your project. Otherwise, you can install it with CMake through the use of `ExternalProject`.

## Usage
```cpp
// Include this in your project
#include <Atomic128.hpp>

// Define your 128 bit data structure
// VIP: Must be 16 bytes in size, 16 bytes in alignment
struct alignas(16) My128BitData
{
    int8_t a;
    int8_t b;
    int16_t c;
    int32_t d;
    int64_t e;
};

A128::Atomic128<My128BitData> atomic_data;

// The following functions are available
My128BitData atomic_data.Load();
void atomic_data.Store(My128BitData data);
My128BitData atomic_data.Exchange(My128BitData data);
bool atomic_data.CompareExchange(My128BitData& expected, My128BitData desired);

// The following operators are available
operator My128BitData();                        // For reading
My128BitData atomic_data.operator=(My128BitData data);          // For writing
bool operator==(My128BitData expected);         // For comparing
bool operator!=(My128BitData expected);         // For comparing
```

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.