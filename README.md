# ARC Cache

Implementation of ARC(adaptive replacement cache) in C++20

# Usage

Input format: _cache size_, _number of requests_, _requests_. All divided by spaces.

Output format: _number of cache hits_.

With `-v` or `--verbose` flag program will also print number of hits for **LRU** cache and ideal cache.

### Example

```bash
./arc -v
4 12 1 2 3 4 1 2 5 1 2 4 3 4
ARC hits: 6 out of 12 requests
LRU hits: 6 out of 12 requests
Ideal hits: 7 out of 12 requests
```

# Build

```bash
cmake -S . -B build
cmake --build build
```

# Description

Algorithm source: [FAST Conference, page 9](https://www2.cs.uh.edu/~paris/7360/PAPERS03/arcfast.pdf)

In short, ARC has 4 lists under the hood:

| List | Purpose |
| --- | ---|
| T1 | Cached pages, requested for first time  |
| T2 | Cached pages, requested 2 or more times |
| B1 | Keys of pages that were removed from T1 |
| B2 | Keys of pages that were removed from T2 |

**T1** works similar to **LRU**, **T2** similar to **LFU**.

**ARC** uses *runtime parameter* to tune ratio of **T1** and **T2** sizes. It it used to adjust caching policy based on request pattern.

**ARC** is used in production in DBS like  **Oracle**, **PostgreSQL** and in file systems (**ZFS**).

# Testing

End-to-end tests are included in `tests` folder.

Use `cmake --build build --target test` to run them.
