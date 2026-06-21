#ifndef MPM_OSAL_H
#define MPM_OSAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uintptr_t osal_thread_t;
typedef uintptr_t osal_mutex_t;
typedef uintptr_t osal_queue_t;
typedef int osal_socket_t;

#define OSAL_SOCKET_INVALID (-1)
#define OSAL_THREAD_INVALID ((osal_thread_t)0)
#define OSAL_MUTEX_INVALID ((osal_mutex_t)0)
#define OSAL_QUEUE_INVALID ((osal_queue_t)0)

typedef void (*osal_thread_fn)(void* arg);

osal_thread_t osal_thread_create(osal_thread_fn fn, void* arg, int priority, size_t stack);
void osal_thread_sleep_ms(uint32_t ms);
uint32_t osal_get_tick_ms(void);

osal_mutex_t osal_mutex_create(void);
void osal_mutex_destroy(osal_mutex_t mutex);
void osal_mutex_lock(osal_mutex_t mutex);
void osal_mutex_unlock(osal_mutex_t mutex);

osal_queue_t osal_queue_create(size_t item_size, size_t depth);
void osal_queue_destroy(osal_queue_t queue);
bool osal_queue_send(osal_queue_t queue, const void* item, uint32_t timeout_ms);
bool osal_queue_recv(osal_queue_t queue, void* item, uint32_t timeout_ms);

bool osal_tcp_init(void);
void osal_tcp_shutdown(void);
osal_socket_t osal_tcp_listen(const char* host, uint16_t port);
osal_socket_t osal_tcp_accept(osal_socket_t listener);
osal_socket_t osal_tcp_connect(const char* host, uint16_t port);
void osal_tcp_close(osal_socket_t socket);
int osal_tcp_send(osal_socket_t socket, const void* buf, size_t len);
int osal_tcp_recv(osal_socket_t socket, void* buf, size_t len);

bool osal_file_read(const char* path, void* buf, size_t* inout_len);
bool osal_file_write(const char* path, const void* buf, size_t len);
bool osal_file_exists(const char* path);

#ifdef __cplusplus
}
#endif

#endif