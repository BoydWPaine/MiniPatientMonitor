#include "osal/osal.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace {

struct ThreadContext {
    osal_thread_fn fn;
    void* arg;
};

struct QueueContext {
    size_t item_size;
    size_t depth;
    std::deque<std::vector<uint8_t>> items;
    std::mutex mutex;
    std::condition_variable not_empty;
    std::condition_variable not_full;
};

void* thread_trampoline(void* raw)
{
    auto* ctx = static_cast<ThreadContext*>(raw);
    ctx->fn(ctx->arg);
    delete ctx;
    return nullptr;
}

bool tcp_send_all(int fd, const void* buf, size_t len)
{
    const auto* bytes = static_cast<const uint8_t*>(buf);
    size_t sent = 0;

    while (sent < len) {
        const ssize_t chunk = ::send(fd, bytes + sent, len - sent, MSG_NOSIGNAL);
        if (chunk <= 0) {
            return false;
        }
        sent += static_cast<size_t>(chunk);
    }

    return true;
}

bool tcp_recv_all(int fd, void* buf, size_t len)
{
    auto* bytes = static_cast<uint8_t*>(buf);
    size_t received = 0;

    while (received < len) {
        const ssize_t chunk = ::recv(fd, bytes + received, len - received, 0);
        if (chunk <= 0) {
            return false;
        }
        received += static_cast<size_t>(chunk);
    }

    return true;
}

}  // namespace

osal_thread_t osal_thread_create(osal_thread_fn fn, void* arg, int /*priority*/, size_t /*stack*/)
{
    auto* ctx = new ThreadContext{fn, arg};
    pthread_t handle{};
    if (pthread_create(&handle, nullptr, thread_trampoline, ctx) != 0) {
        delete ctx;
        return OSAL_THREAD_INVALID;
    }

    pthread_detach(handle);
    return static_cast<osal_thread_t>(handle);
}

osal_mutex_t osal_mutex_create(void)
{
    auto* mutex = new pthread_mutex_t;
    if (pthread_mutex_init(mutex, nullptr) != 0) {
        delete mutex;
        return OSAL_MUTEX_INVALID;
    }

    return reinterpret_cast<osal_mutex_t>(mutex);
}

void osal_mutex_destroy(osal_mutex_t mutex)
{
    if (mutex == OSAL_MUTEX_INVALID) {
        return;
    }

    auto* handle = reinterpret_cast<pthread_mutex_t*>(mutex);
    pthread_mutex_destroy(handle);
    delete handle;
}

void osal_mutex_lock(osal_mutex_t mutex)
{
    pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(mutex));
}

void osal_mutex_unlock(osal_mutex_t mutex)
{
    pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex));
}

osal_queue_t osal_queue_create(size_t item_size, size_t depth)
{
    if (item_size == 0 || depth == 0) {
        return OSAL_QUEUE_INVALID;
    }

    auto* queue = new QueueContext{item_size, depth, {}, {}, {}, {}};
    return reinterpret_cast<osal_queue_t>(queue);
}

void osal_queue_destroy(osal_queue_t queue)
{
    if (queue == OSAL_QUEUE_INVALID) {
        return;
    }

    delete reinterpret_cast<QueueContext*>(queue);
}

bool osal_queue_send(osal_queue_t queue, const void* item, uint32_t timeout_ms)
{
    auto* ctx = reinterpret_cast<QueueContext*>(queue);
    std::unique_lock<std::mutex> lock(ctx->mutex);

    const auto ready = [&]() { return ctx->items.size() < ctx->depth; };
    if (timeout_ms == UINT32_MAX) {
        ctx->not_full.wait(lock, ready);
    } else if (!ctx->not_full.wait_for(lock, std::chrono::milliseconds(timeout_ms), ready)) {
        return false;
    }

    std::vector<uint8_t> copy(ctx->item_size);
    std::memcpy(copy.data(), item, ctx->item_size);
    ctx->items.push_back(std::move(copy));
    ctx->not_empty.notify_one();
    return true;
}

bool osal_queue_recv(osal_queue_t queue, void* item, uint32_t timeout_ms)
{
    auto* ctx = reinterpret_cast<QueueContext*>(queue);
    std::unique_lock<std::mutex> lock(ctx->mutex);

    const auto ready = [&]() { return !ctx->items.empty(); };
    if (timeout_ms == UINT32_MAX) {
        ctx->not_empty.wait(lock, ready);
    } else if (!ctx->not_empty.wait_for(lock, std::chrono::milliseconds(timeout_ms), ready)) {
        return false;
    }

    std::vector<uint8_t> front = std::move(ctx->items.front());
    ctx->items.pop_front();
    std::memcpy(item, front.data(), ctx->item_size);
    ctx->not_full.notify_one();
    return true;
}

bool osal_tcp_init(void)
{
    return true;
}

void osal_tcp_shutdown(void)
{
}

osal_socket_t osal_tcp_listen(const char* host, uint16_t port)
{
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return OSAL_SOCKET_INVALID;
    }

    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        ::close(fd);
        return OSAL_SOCKET_INVALID;
    }

    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0 ||
        ::listen(fd, 1) != 0) {
        ::close(fd);
        return OSAL_SOCKET_INVALID;
    }

    return fd;
}

osal_socket_t osal_tcp_accept(osal_socket_t listener)
{
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    const int client_fd = ::accept(listener, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
    return client_fd < 0 ? OSAL_SOCKET_INVALID : client_fd;
}

osal_socket_t osal_tcp_connect(const char* host, uint16_t port)
{
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return OSAL_SOCKET_INVALID;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
        ::close(fd);
        return OSAL_SOCKET_INVALID;
    }

    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        ::close(fd);
        return OSAL_SOCKET_INVALID;
    }

    return fd;
}

void osal_tcp_close(osal_socket_t socket)
{
    if (socket != OSAL_SOCKET_INVALID) {
        ::close(socket);
    }
}

int osal_tcp_send(osal_socket_t socket, const void* buf, size_t len)
{
    return tcp_send_all(socket, buf, len) ? static_cast<int>(len) : -1;
}

int osal_tcp_recv(osal_socket_t socket, void* buf, size_t len)
{
    return tcp_recv_all(socket, buf, len) ? static_cast<int>(len) : -1;
}

bool osal_file_read(const char* path, void* buf, size_t* inout_len)
{
    if (path == nullptr || buf == nullptr || inout_len == nullptr) {
        return false;
    }

    FILE* file = std::fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }

    const size_t read_bytes = std::fread(buf, 1, *inout_len, file);
    const bool ok = !std::ferror(file);
    std::fclose(file);

    if (!ok) {
        return false;
    }

    *inout_len = read_bytes;
    return true;
}

bool osal_file_write(const char* path, const void* buf, size_t len)
{
    if (path == nullptr || buf == nullptr) {
        return false;
    }

    FILE* file = std::fopen(path, "wb");
    if (file == nullptr) {
        return false;
    }

    const size_t written = std::fwrite(buf, 1, len, file);
    const bool ok = written == len && !std::ferror(file);
    std::fclose(file);
    return ok;
}

bool osal_file_exists(const char* path)
{
    if (path == nullptr) {
        return false;
    }

    struct stat info {};
    return stat(path, &info) == 0;
}