#include <reportportal/log.hpp>
#include <string>
#include <uuid.h>
#include <optional>
#include <chrono>

namespace report_portal
{

log::log(
    const uuids::uuid& launch_id,
    const std::chrono::high_resolution_clock::time_point& time,
    log_level level,
    const std::string& message)
  : _launch_id(launch_id),
    _test_item_id(),
    _time(time),
    _message(message),
    _level(level),
    _attachment()
{}

log::log(
    const uuids::uuid& launch_id,
    const uuids::uuid& test_item_id,
    const std::chrono::high_resolution_clock::time_point& time,
    log_level level,
    const std::string& message)
  : _launch_id(launch_id),
    _test_item_id(test_item_id),
    _time(time),
    _message(message),
    _level(level),
    _attachment()
{}

uuids::uuid log::launch_id() const {
    return _launch_id;
}

uuids::uuid log::test_item_id() const {
    return _test_item_id;
}

std::chrono::high_resolution_clock::time_point log::time() const {
    return _time;
}

std::string log::message() const {
    return _message;
}

log_level log::level() const {
    return _level;
}

void log::set_attachment(const report_portal::attachment& attachment) {
    _attachment = attachment;
}

const std::optional<report_portal::attachment>& log::attachment() const {
    return _attachment;
}

}
