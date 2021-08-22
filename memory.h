#ifndef _MEMORY_H_
#define _MEMORY_H_

struct kv;

int memory_append(struct kv **list_head, const char *variable_name, int value);
struct kv *memory_destroy_all(struct kv *memory);
int memory_extract(const struct kv *kv, const char *variable_name);
void memory_print(const struct kv *kv);

#endif // _MEMORY_H_
