#include "proto/encoder.h"

#include <array>
#include <cstring>

namespace mpm::proto {
namespace {

uint32_t read_be_u32(const uint8_t* bytes)
{
    return (static_cast<uint32_t>(bytes[0]) << 24) |
           (static_cast<uint32_t>(bytes[1]) << 16) |
           (static_cast<uint32_t>(bytes[2]) << 8) |
           static_cast<uint32_t>(bytes[3]);
}

void write_be_u32(uint32_t value, uint8_t* bytes)
{
    bytes[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    bytes[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    bytes[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    bytes[3] = static_cast<uint8_t>(value & 0xFF);
}

}  // namespace

bool encode_envelope(const monitor::Envelope& envelope, std::vector<uint8_t>& out)
{
    std::string payload;
    if (!envelope.SerializeToString(&payload)) {
        return false;
    }

    if (payload.size() > kMaxFramePayload) {
        return false;
    }

    out.resize(4 + payload.size());
    write_be_u32(static_cast<uint32_t>(payload.size()), out.data());
    std::memcpy(out.data() + 4, payload.data(), payload.size());
    return true;
}

bool decode_envelope(const uint8_t* data, size_t len, monitor::Envelope& out)
{
    if (data == nullptr || len == 0) {
        return false;
    }

    return out.ParseFromArray(data, static_cast<int>(len));
}

bool write_frame(osal_socket_t socket, const monitor::Envelope& envelope)
{
    std::vector<uint8_t> frame;
    if (!encode_envelope(envelope, frame)) {
        return false;
    }

    return osal_tcp_send(socket, frame.data(), frame.size()) == static_cast<int>(frame.size());
}

bool read_frame(osal_socket_t socket, monitor::Envelope& envelope)
{
    std::array<uint8_t, 4> header {};
    if (osal_tcp_recv(socket, header.data(), header.size()) != static_cast<int>(header.size())) {
        return false;
    }

    const uint32_t payload_len = read_be_u32(header.data());
    if (payload_len == 0 || payload_len > kMaxFramePayload) {
        return false;
    }

    std::vector<uint8_t> payload(payload_len);
    if (osal_tcp_recv(socket, payload.data(), payload.size()) != static_cast<int>(payload.size())) {
        return false;
    }

    return decode_envelope(payload.data(), payload.size(), envelope);
}

}  // namespace mpm::proto