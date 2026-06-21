#ifndef MPM_PROTO_ENCODER_H
#define MPM_PROTO_ENCODER_H

#include "monitor.pb.h"
#include "osal/osal.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace mpm::proto {

constexpr uint32_t kMaxFramePayload = 64 * 1024;

bool encode_envelope(const monitor::Envelope& envelope, std::vector<uint8_t>& out);
bool decode_envelope(const uint8_t* data, size_t len, monitor::Envelope& out);

bool write_frame(osal_socket_t socket, const monitor::Envelope& envelope);
bool read_frame(osal_socket_t socket, monitor::Envelope& envelope);

}  // namespace mpm::proto

#endif