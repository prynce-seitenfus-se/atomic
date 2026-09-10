# Atomic

A lightweight, strictly C99-compliant, header-only atomic abstraction layer designed for lock-free data structures, bare-metal firmware, and RTOS applications.

---

## Key Features

* **Header-Only**: Self-contained in [`atomic.h`](atomic.h). Zero linking dependencies.
* **Type-Safe Encapsulation**: Encloses `volatile size_t` within a struct (`AtomicSize` / `atomic_size_t`), preventing accidental direct assignments or reads that bypass hardware memory barriers.
* **Hardware-Agnostic Synchronization**: Employs release/acquire memory fences mapped directly to GCC/Clang built-ins (`__atomic_*`), optimizing target-instruction selection on ARM Cortex-M and RISC-V with graceful legacy fallback to `__sync_synchronize()`.
* **Zero Overhead**: Fully defined with `static inline` functions for direct inlining and zero function-call overhead.
* **Relaxed Context Optimizations**: Provides relaxed load/store operations (`atomic_load_relaxed`, `atomic_store_relaxed`) for variables owned exclusively by a single context, eliminating redundant pipeline barriers.
* **MISRA C:2012 Compliant**: Adheres to MISRA C:2012 standards, including defensive pointer validation, standard explicit types (`<stddef.h>`, `<stdint.h>`, `<stdbool.h>`), and unsigned literal typing.

---

## API Reference

The full implementation resides in [`atomic.h`](atomic.h):

```c
/**
 * @brief Type-safe atomic size_t wrapper.
 */
typedef struct AtomicSize {
    volatile size_t value;
} AtomicSize;

typedef AtomicSize atomic_size_t;

/**
 * @brief Stores value with release semantics (preceding writes are committed first).
 * @param obj Pointer to atomic object.
 * @param desired Value to store.
 */
static inline void atomic_store_release(atomic_size_t* obj, size_t desired);

/**
 * @brief Loads value with acquire semantics (subsequent reads cannot be reordered prior).
 * @param obj Pointer to atomic object.
 * @return Value loaded, or 0 if pointer is NULL.
 */
static inline size_t atomic_load_acquire(const atomic_size_t* obj);

/**
 * @brief Loads value with relaxed semantics (no memory fence).
 * @param obj Pointer to atomic object.
 * @return Value loaded, or 0 if pointer is NULL.
 */
static inline size_t atomic_load_relaxed(const atomic_size_t* obj);

/**
 * @brief Stores value with relaxed semantics (no memory fence).
 * @param obj Pointer to atomic object.
 * @param desired Value to store.
 */
static inline void atomic_store_relaxed(atomic_size_t* obj, size_t desired);

/**
 * @brief Initializes atomic object without memory fences (pre-concurrency setup).
 */
static inline void atomic_init_size_t(atomic_size_t* obj, size_t desired);

/**
 * @brief Standalone acquire memory barrier.
 */
static inline void atomic_thread_fence_acquire(void);

/**
 * @brief Standalone release memory barrier.
 */
static inline void atomic_thread_fence_release(void);

/**
 * @brief Standalone full (sequentially consistent) memory barrier.
 */
static inline void atomic_thread_fence_seq_cst(void);
```

---

## Usage Example

```c
#include "atomic.h"
#include <stdio.h>

typedef struct {
    atomic_size_t write_index;
    atomic_size_t read_index;
} SharedChannel;

int main(void)
{
    SharedChannel channel;

    /* Initialize without fences */
    atomic_init_size_t(&channel.write_index, 0U);
    atomic_init_size_t(&channel.read_index, 0U);

    /* Producer: Store with release semantics */
    atomic_store_release(&channel.write_index, 10U);

    /* Consumer: Load with acquire semantics */
    size_t idx = atomic_load_acquire(&channel.write_index);

    return 0;
}
```

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.