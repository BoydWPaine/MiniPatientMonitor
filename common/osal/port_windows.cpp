#include "osal/osal.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

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

struct MutexContext {
    CRITICAL_SECTION section;
};

unsigned __stdcall thread_trampoline(void* raw)
{
    auto* ctx = static_cast<ThreadContext*>(raw);
    ctx->fn(ctx->arg);
    delete ctx;
    return 0;
}

bool tcp_send_all(SOCKET socket, const void* buf, size_t len)
{
    const auto* bytes = static_cast<const uint8_t*>(buf);
    size_t sent = 0;

    while (sent < len) {
        const int chunk = ::send(socket, reinterpret_cast<const char*>(bytes + sent),
                                 static_cast<int>(len - sent), 0);
        if (chunk <= 0) {
            return false;
        }
        sent += static_cast<size_t>(chunk);
    }

    return true;
}

bool tcp_recv_all(SOCKET socket, void* buf, size_t len)
{
    auto* bytes = static_cast<uint8_t*>(buf);
    size_t received = 0;

    while (received < len) {
        const int chunk = ::recv(socket, reinterpret_cast<char*>(bytes + received),
                                 static_cast<int>(len - received), 0);
        if (chunk <= 0) {
            return false;
        }
        received += static_cast<size_t>(chunk);
    }

    return true;
}

}  // namespace

osal_thread_t osal_thread_create(osal_thread_fn fn, void* arg, int /*priority*/, size_t stack)
{
    auto* ctx = new ThreadContext{fn, arg};
    const uintptr_t handle = _beginthreadex(
        nullptr, static_cast<unsigned>(stack), thread_trampoline, ctx, 0, nullptr);
    if (handle == 0) {
        delete ctx;
        return OSAL_THREAD_INVALID;
    }

    CloseHandle(reinterpret_cast<HANDLE>(handle));
    return handle;
}

osal_mutex_t osal_mutex_create(void)
{
    auto* mutex = new MutexContext;
    InitializeCriticalSection(&mutex->section);
    return reinterpret_cast<osal_mutex_t>(mutex);
}

void osal_mutex_destroy(osal_mutex_t mutex)
{
    if (mutex == OSAL_MUTEX_INVALID) {
        return;
    }

    auto* handle = reinterpret_cast<MutexContext*>(mutex);
    DeleteCriticalSection(&handle->section);
    delete handle;
}

void osal_mutex_lock(osal_mutex_t mutex)
{
    EnterCriticalSection(&reinterpret_cast<MutexContext*>(mutex)->section);
}

void osal_mutex_unlock(osal_mutex_t mutex)
{
    LeaveCriticalSection(&reinterpret_cast<MutexContext*>(mutex)->section);
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
    WSADATA data {};
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void osal_tcp_shutdown(void)
{
    WSACleanup();
}

osal_socket_t osal_tcp_listen(const char* host, uint16_t port)
{
    const SOCKET listener = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET) {
        return OSAL_SOCKET_INVALID;
    }

    BOOL yes = TRUE;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&yes), sizeof(yes));

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (InetPtonA(AF_INET, host, &addr.sin_addr) != 1) {
        closesocket(listener);
        return OSAL_SOCKET_INVALID;
    }

    if (bind(listener, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR ||
        listen(listener, 1) == SOCKET_ERROR) {
        closesocket(listener);
        return OSAL_SOCKET_INVALID;
    }

    return static_cast<osal_socket_t>(listener);
}

osal_socket_t osal_tcp_accept(osal_socket_t listener)
{
    const SOCKET client = accept(static_cast<SOCKET>(listener), nullptr, nullptr);
    return client == INVALID_SOCKET ? OSAL_SOCKET_INVALID : static_cast<osal_socket_t>(client);
}

osal_socket_t osal_tcp_connect(const char* host, uint16_t port)
{
    const SOCKET socket_fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd == INVALID_SOCKET) {
        return OSAL_SOCKET_INVALID;
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (InetPtonA(AF_INET, host, &addr.sin_addr) != 1) {
        closesocket(socket_fd);
        return OSAL_SOCKET_INVALID;
    }

    if (connect(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        closesocket(socket_fd);
        return OSAL_SOCKET_INVALID;
    }

    return static_cast<osal_socket_t>(socket_fd);
}

void osal_tcp_close(osal_socket_t socket)
{
    if (socket != OSAL_SOCKET_INVALID) {
        closesocket(static_cast<SOCKET>(socket));
    }
}

int osal_tcp_send(osal_socket_t socket, const void* buf, size_t len)
{
    return tcp_send_all(static_cast<SOCKET>(socket), buf, len) ? static_cast<int>(len) : -1;
}

int osal_tcp_recv(osal_socket_t socket, void* buf, size_t len)
{
    return tcp_recv_all(static_cast<SOCKET>(socket), buf, len) ? static_cast<int>(len) : -1;
}

bool osal_tcp_wait_readable(osal_socket_t socket, uint32_t timeout_ms)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(static_cast<SOCKET>(socket), &readfds);

    timeval tv {};
    timeval* tv_ptr = nullptr;
    if (timeout_ms != UINT32_MAX) {
        tv.tv_sec = static_cast<long>(timeout_ms / 1000U);
        tv.tv_usec = static_cast<long>((timeout_ms % 1000U) * 1000U);
        tv_ptr = &tv;
    }

    const int ready = ::select(0, &readfds, nullptr, nullptr, tv_ptr);
    return ready > 0 && FD_ISSET(static_cast<SOCKET>(socket), &readfds);
}

bool osal_file_read(const char* path, void* buf, size_t* inout_len)
{
    if (path == nullptr || buf == nullptr || inout_len == nullptr) {
        return false;
    }

    FILE* file = nullptr;
    if (fopen_s(&file, path, "rb") != 0 || file == nullptr) {
        return false;
    }

    const size_t read_bytes = fread(buf, 1, *inout_len, file);
    const bool ok = !ferror(file);
    fclose(file);

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

    FILE* file = nullptr;
    if (fopen_s(&file, path, "wb") != 0 || file == nullptr) {
        return false;
    }

    const size_t written = fwrite(buf, 1, len, file);
    const bool ok = written == len && !ferror(file);
    fclose(file);
    return ok;
}

bool osal_file_exists(const char* path)
{
    if (path == nullptr) {
        return false;
    }

    FILE* file = nullptr;
    if (fopen_s(&file, path, "rb") != 0 || file == nullptr) {
        return false;
    }

    fclose(file);
    return true;
}