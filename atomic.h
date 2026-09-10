#ifndef ATOMIC_H
#define ATOMIC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Atomic wrapper around size_t.
 *
 * Encapsulating volatile size_t within a struct prevents accidental direct assignment
 * or bypassing memory barrier fences at compile-time.
 */
typedef struct AtomicSize {
    volatile size_t value;
} AtomicSize;

/**
 * @brief Type alias for C99 compatibility.
 */
typedef AtomicSize atomic_size_t;

/**
 * @brief Stores a value into an atomic_size_t object with release semantics.
 *
 * Ensures all prior memory operations (reads and writes) are committed and visible
 * before the new value is stored into the target object. Uses direct compiler
 * intrinsics for optimal target-instruction selection.
 *
 * @param obj Pointer to the atomic_size_t object.
 * @param desired The value to store.
 */
static inline void atomic_store_release(atomic_size_t* obj, size_t desired)
{
    if (obj == NULL) {
        return;
    }

#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_RELEASE)
    __atomic_store_n(&obj->value, desired, __ATOMIC_RELEASE);
#elif defined(__GNUC__)
    __sync_synchronize();
    obj->value = desired;
#else
    obj->value = desired;
#endif
}

/**
 * @brief Loads a value from an atomic_size_t object with acquire semantics.
 *
 * Reads the object's current value and enforces an acquire fence, ensuring subsequent
 * memory operations cannot be reordered before this read.
 *
 * @param obj Pointer to the atomic_size_t object.
 * @return The value loaded from the object, or 0 if obj is NULL.
 */
static inline size_t atomic_load_acquire(const atomic_size_t* obj)
{
    if (obj == NULL) {
        return 0U;
    }

#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_ACQUIRE)
    return __atomic_load_n(&obj->value, __ATOMIC_ACQUIRE);
#elif defined(__GNUC__)
    size_t val = obj->value;
    __sync_synchronize();
    return val;
#else
    return obj->value;
#endif
}

/**
 * @brief Loads a value from an atomic_size_t object with relaxed semantics (no memory fence).
 *
 * Suitable when reading a variable owned exclusively by the current execution context.
 *
 * @param obj Pointer to the atomic_size_t object.
 * @return The value loaded from the object, or 0 if obj is NULL.
 */
static inline size_t atomic_load_relaxed(const atomic_size_t* obj)
{
    if (obj == NULL) {
        return 0U;
    }

#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_RELAXED)
    return __atomic_load_n(&obj->value, __ATOMIC_RELAXED);
#else
    return obj->value;
#endif
}

/**
 * @brief Stores a value into an atomic_size_t object with relaxed semantics (no memory fence).
 *
 * @param obj Pointer to the atomic_size_t object.
 * @param desired The value to store.
 */
static inline void atomic_store_relaxed(atomic_size_t* obj, size_t desired)
{
    if (obj == NULL) {
        return;
    }

#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_RELAXED)
    __atomic_store_n(&obj->value, desired, __ATOMIC_RELAXED);
#else
    obj->value = desired;
#endif
}

/**
 * @brief Initializes an atomic_size_t object without memory fences.
 *
 * Safe for single-threaded or pre-thread setup.
 *
 * @param obj Pointer to the atomic_size_t object.
 * @param desired Initial value.
 */
static inline void atomic_init_size_t(atomic_size_t* obj, size_t desired)
{
    if (obj == NULL) {
        return;
    }

    obj->value = desired;
}

/**
 * @brief Issues a standalone acquire memory barrier.
 */
static inline void atomic_thread_fence_acquire(void)
{
#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_ACQUIRE)
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
#elif defined(__GNUC__)
    __sync_synchronize();
#endif
}

/**
 * @brief Issues a standalone release memory barrier.
 */
static inline void atomic_thread_fence_release(void)
{
#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_RELEASE)
    __atomic_thread_fence(__ATOMIC_RELEASE);
#elif defined(__GNUC__)
    __sync_synchronize();
#endif
}

/**
 * @brief Issues a standalone sequentially consistent (full) memory barrier.
 */
static inline void atomic_thread_fence_seq_cst(void)
{
#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__ATOMIC_SEQ_CST)
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
#elif defined(__GNUC__)
    __sync_synchronize();
#endif
}

#endif /* ATOMIC_H */
