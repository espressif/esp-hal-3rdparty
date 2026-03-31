/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file heap_caps.c
 * @brief NuttX reimplementation of ESP-IDF heap_caps functions.
 *
 * This file provides NuttX-compatible implementations of the ESP-IDF
 * heap capabilities API. Since NuttX does not have capability-based
 * heap regions like ESP-IDF, the capability flags are ignored and
 * standard NuttX kernel memory allocation functions are used instead.
 */

#include <nuttx/config.h>
#include <string.h>
#include <malloc.h>

#include <nuttx/compiler.h>
#include <nuttx/kmalloc.h>

#include "esp_heap_caps.h"
#ifdef CONFIG_ESPRESSIF_RETENTION_HEAP
#  include "esp_retentionheap.h"
#endif

static esp_alloc_failed_hook_t alloc_failed_callback;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_ESPRESSIF_RETENTION_HEAP

static bool esp_heap_caps_retention_caps_exact(uint32_t caps)
{
  return (caps & MALLOC_CAP_RETENTION) != 0 &&
         (caps & ~MALLOC_CAP_RETENTION) == 0;
}

#else
#  define esp_heap_caps_retention_caps_exact(caps)    false
#  define esp_retentionheap_malloc(size)              NULL
#  define esp_retentionheap_calloc(n, elem_size)      NULL
#  define esp_retentionheap_memalign(alignment, size) NULL
#  define esp_retentionheap_realloc(ptr, size)        NULL
#  define esp_retentionheap_free(mem)
#  define esp_retentionheap_heapmember(mem)           false
#  define esp_retentionheap_malloc_size(mem)          0
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: heap_caps_register_failed_alloc_callback
 *
 * Description:
 *   Register a callback to be invoked when a memory allocation fails.
 *
 ****************************************************************************/

esp_err_t heap_caps_register_failed_alloc_callback(
    esp_alloc_failed_hook_t callback)
{
  alloc_failed_callback = callback;
  return ESP_OK;
}

/****************************************************************************
 * Name: heap_caps_malloc
 *
 * Description:
 *   Allocate memory with the given capabilities.  Most requests use
 *   kmm_malloc; with CONFIG_ESPRESSIF_RETENTION_HEAP, MALLOC_CAP_RETENTION
 *   only uses the retention mm heap.
 *
 ****************************************************************************/

void *heap_caps_malloc(size_t size, uint32_t caps)
{
  FAR void *ptr;

  if (esp_heap_caps_retention_caps_exact(caps))
    {
      ptr = esp_retentionheap_malloc(size);
    }
  else
    {
      ptr = kmm_malloc(size);
    }

  if (ptr == NULL && alloc_failed_callback)
    {
      alloc_failed_callback(size, caps, __func__);
    }

  return ptr;
}

/****************************************************************************
 * Name: heap_caps_free
 *
 * Description:
 *   Free memory previously allocated via heap_caps_malloc() or
 *   heap_caps_realloc().
 *
 ****************************************************************************/

void heap_caps_free(void *ptr)
{
  if (esp_retentionheap_heapmember(ptr))
    {
      esp_retentionheap_free(ptr);
    }
  else
    {
      kmm_free(ptr);
    }
}

/****************************************************************************
 * Name: heap_caps_realloc
 *
 * Description:
 *   Reallocate memory previously allocated via heap_caps_malloc() or
 *   heap_caps_realloc().
 *
 ****************************************************************************/

void *heap_caps_realloc(void *ptr, size_t size, uint32_t caps)
{
  FAR void *new_ptr;

  if (ptr == NULL)
    {
      return heap_caps_malloc(size, caps);
    }

  if (esp_retentionheap_heapmember(ptr))
    {
      if (size == 0)
        {
          esp_retentionheap_free(ptr);
          return NULL;
        }

      new_ptr = esp_retentionheap_realloc(ptr, size);
    }
  else
    {
      new_ptr = kmm_realloc(ptr, size);
    }

  if (new_ptr == NULL && size > 0 && alloc_failed_callback)
    {
      alloc_failed_callback(size, caps, __func__);
    }

  return new_ptr;
}

/****************************************************************************
 * Name: heap_caps_calloc
 *
 * Description:
 *   Allocate zero-initialized memory with the given capabilities.
 *
 ****************************************************************************/

void *heap_caps_calloc(size_t n, size_t size, uint32_t caps)
{
  size_t total = n * size;
  void *ptr;

  if (esp_heap_caps_retention_caps_exact(caps))
    {
      ptr = esp_retentionheap_calloc(n, size);
    }
  else
    {
      ptr = kmm_calloc(n, size);
    }

  if (ptr == NULL && alloc_failed_callback)
    {
      alloc_failed_callback(total, caps, __func__);
    }

  return ptr;
}

/****************************************************************************
 * Name: heap_caps_aligned_alloc
 *
 * Description:
 *   Allocate aligned memory with the given capabilities.
 *
 ****************************************************************************/

void *heap_caps_aligned_alloc(size_t alignment, size_t size, uint32_t caps)
{
  FAR void *ptr;

  if (esp_heap_caps_retention_caps_exact(caps))
    {
      ptr = esp_retentionheap_memalign(alignment, size);
    }
  else
    {
      ptr = kmm_memalign(alignment, size);
    }

  if (ptr == NULL && alloc_failed_callback)
    {
      alloc_failed_callback(size, caps, __func__);
    }

  return ptr;
}

/****************************************************************************
 * Name: heap_caps_aligned_free
 *
 * Description:
 *   Free memory previously allocated via heap_caps_aligned_alloc().
 *
 ****************************************************************************/

void heap_caps_aligned_free(void *ptr)
{
  heap_caps_free(ptr);
}

/****************************************************************************
 * Name: heap_caps_aligned_calloc
 *
 * Description:
 *   Allocate aligned, zero-initialized memory with the given capabilities.
 *
 ****************************************************************************/

void *heap_caps_aligned_calloc(size_t alignment, size_t n, size_t size,
                               uint32_t caps)
{
  size_t total = n * size;
  FAR void *ptr;

  if (esp_heap_caps_retention_caps_exact(caps))
    {
      ptr = esp_retentionheap_memalign(alignment, total);
    }
  else
    {
      ptr = kmm_memalign(alignment, total);
    }

  if (ptr != NULL)
    {
      memset(ptr, 0, total);
    }
  else if (alloc_failed_callback)
    {
      alloc_failed_callback(total, caps, __func__);
    }

  return ptr;
}

/****************************************************************************
 * Name: heap_caps_get_total_size
 *
 * Description:
 *   Get the total size of all the regions that have the given capabilities.
 *
 ****************************************************************************/

size_t heap_caps_get_total_size(uint32_t caps)
{
  struct mallinfo info = kmm_mallinfo();

  return info.arena;
}

/****************************************************************************
 * Name: heap_caps_get_free_size
 *
 * Description:
 *   Get the total free size of all the regions that have the given
 *   capabilities.
 *
 ****************************************************************************/

size_t heap_caps_get_free_size(uint32_t caps)
{
  struct mallinfo info = kmm_mallinfo();

  return info.fordblks;
}

/****************************************************************************
 * Name: heap_caps_get_minimum_free_size(uint32_t caps)
 *
 * Description:
 *   Get the total minimum free memory of all regions with the given
 *   capabilities.
 *
 ****************************************************************************/

size_t heap_caps_get_minimum_free_size(uint32_t caps)
{
  /* NuttX does not track minimum free size, return current free size */

  return heap_caps_get_free_size(caps);
}

/****************************************************************************
 * Name: heap_caps_get_largest_free_block
 *
 * Description:
 *   Get the largest free block of memory able to be allocated with the
 *   given capabilities.
 *
 ****************************************************************************/

size_t heap_caps_get_largest_free_block(uint32_t caps)
{
  struct mallinfo info = kmm_mallinfo();

  return info.mxordblk;
}

/****************************************************************************
 * Name: heap_caps_get_info
 *
 * Description:
 *   Get heap info for all regions with the given capabilities.
 *
 ****************************************************************************/

void heap_caps_get_info(multi_heap_info_t *info, uint32_t caps)
{
  struct mallinfo minfo = kmm_mallinfo();

  memset(info, 0, sizeof(multi_heap_info_t));
  info->total_free_bytes = minfo.fordblks;
  info->total_allocated_bytes = minfo.uordblks;
  info->largest_free_block = minfo.mxordblk;
}

/****************************************************************************
 * Name: heap_caps_malloc_prefer
 *
 * Description:
 *   Allocate a chunk of memory as preference in decreasing order.
 *
 ****************************************************************************/

void *heap_caps_malloc_prefer(size_t size, size_t num, ...)
{
  return kmm_malloc(size);
}

/****************************************************************************
 * Name: heap_caps_realloc_prefer
 *
 * Description:
 *   Reallocate a chunk of memory as preference in decreasing order.
 *
 ****************************************************************************/

void *heap_caps_realloc_prefer(void *ptr, size_t size, size_t num, ...)
{
  return kmm_realloc(ptr, size);
}

/****************************************************************************
 * Name: heap_caps_calloc_prefer
 *
 * Description:
 *   Allocate a chunk of memory as preference in decreasing order.
 *
 ****************************************************************************/

void *heap_caps_calloc_prefer(size_t n, size_t size, size_t num, ...)
{
  return kmm_calloc(n, size);
}

/****************************************************************************
 * Stubs for functions that are not applicable on NuttX
 ****************************************************************************/

esp_err_t heap_caps_monitor_local_minimum_free_size_start(void)
{
  return ESP_OK;
}

esp_err_t heap_caps_monitor_local_minimum_free_size_stop(void)
{
  return ESP_OK;
}

void heap_caps_print_heap_info(uint32_t caps)
{
}

bool heap_caps_check_integrity_all(bool print_errors)
{
  return true;
}

bool heap_caps_check_integrity(uint32_t caps, bool print_errors)
{
  return true;
}

bool heap_caps_check_integrity_addr(intptr_t addr, bool print_errors)
{
  return true;
}

void heap_caps_malloc_extmem_enable(size_t limit)
{
}

void heap_caps_dump(uint32_t caps)
{
}

void heap_caps_dump_all(void)
{
}

size_t heap_caps_get_allocated_size(void *ptr)
{
  if (esp_retentionheap_heapmember(ptr))
    {
      return esp_retentionheap_malloc_size(ptr);
    }

  return kmm_malloc_size(ptr);
}

size_t heap_caps_get_containing_block_size(void *ptr)
{
  if (esp_retentionheap_heapmember(ptr))
    {
      return esp_retentionheap_malloc_size(ptr);
    }

  return kmm_malloc_size(ptr);
}

void heap_caps_walk(uint32_t caps, heap_caps_walker_cb_t walker_func,
                    void *user_data)
{
}

void heap_caps_walk_all(heap_caps_walker_cb_t walker_func, void *user_data)
{
}

