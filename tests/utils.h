#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>
#include <optional>
#include <uuid.h>
#include <reportportal/ijson_serializer.hpp>

std::chrono::high_resolution_clock::time_point from_iso_8601(const std::string& time);

namespace std {
std::ostream& operator<<(std::ostream& output, const std::optional<uuids::uuid>& value);
std::ostream& operator<<(std::ostream& output, const std::optional<std::string>& value);
std::ostream& operator<<(std::ostream& output, const std::optional<uint64_t>& value);
std::ostream& operator<<(std::ostream& output, const std::chrono::high_resolution_clock::time_point& value);

std::ostream& operator<<(std::ostream& output, const report_portal::launch_mode& value);
std::ostream& operator<<(std::ostream& output, const std::optional<report_portal::launch_mode>& value);
std::ostream& operator<<(std::ostream& output, const report_portal::test_item_type& value);
std::ostream& operator<<(std::ostream& output, const std::optional<report_portal::test_item_type>& value);
std::ostream& operator<<(std::ostream& output, const report_portal::test_item_status& value);
std::ostream& operator<<(std::ostream& output, const std::optional<report_portal::test_item_status>& value);
}

