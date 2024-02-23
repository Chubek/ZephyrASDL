#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asdl.h"

GCNode *new_gc_node(void *data) {
  GCNode *node = (GCNode *)malloc(sizeof(GCNode));
  if (node != NULL) {
    node->data = data;
    node->next = NULL;
    node->marked = 0;
  }
  return node;
}

Heap *create_heap() {
  Heap *heap = (Heap *)malloc(sizeof(Heap));
  if (heap != NULL) {
    heap->head = NULL;
  }
  return heap;
}

void *heap_alloc(Heap *heap, size_t size) {
  void *data = malloc(size);
  if (data != NULL) {
    GCNode *node = new_gc_node(data);
    if (node != NULL) {
      node->next = heap->head;
      heap->head = node;
    } else {
      free(data);
    }
  }
  return data;
}

void *heap_realloc(Heap *heap, void *data, size_t new_size) {
  Heap *node = NULL;
  for (node = heap; node != NULL; node = node->next)
    if (node->data = data)
      break;
  if (node == NULL)
    return data;
  node->data = realloc(node->data, new_size);
  return node->data;
}

void mark(GCNode *node) {
  if (node != NULL && !node->marked) {
    node->marked = 1;
    mark(node->next);
  }
}

void sweep(Heap *heap) {
  GCNode *current = heap->head;
  GCNode *prev = NULL;
  while (current != NULL) {
    if (!current->marked) {
      GCNode *temp = current;
      current = current->next;
      if (prev != NULL) {
        prev->next = current;
      } else {
        heap->head = current;
      }
      free(temp->data);
      free(temp);
    } else {
      current->marked = 0;
      prev = current;
      current = current->next;
    }
  }
}

void garbage_collect(Heap *heap) {
  mark(heap->head);
  sweep(heap);
}

void dump_heap(Heap *heap) {
  GCNode *current = heap->head;
  while (current != NULL) {
    GCNode *temp = current;
    current = current->next;
    free(temp->data);
    free(temp);
  }
  free(heap);
}
