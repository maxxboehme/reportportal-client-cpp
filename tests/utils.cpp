#include <utils.h>


static std::string to_iso_8601(const std::chrono::high_resolution_clock::time_point& time)
{
    std::time_t epoch_seconds = std::chrono::high_resolution_clock::to_time_t(time);

    // Format this as date time to seconds resolution.
    // e.g. 2016-08-30T08:18:51
    std::stringstream stream;
    std::tm utc_time = *std::localtime(&epoch_seconds);
    stream << std::put_time(&utc_time, "%FT%T");

    // If new now convert back to a time_point we will get the time truncated
    // to while seconds
    auto truncated = std::chrono::high_resolution_clock::from_time_t(epoch_seconds);

    // Now we wubtract this seconds count from the original time to
    // get the number of extra microseconds...
    uint64_t delta_us = std::chrono::duration_cast<std::chrono::microseconds>(time - truncated).count();

    // And append this to the output stream as fractional seconds
    // e.g. 2016-08-30T08:18:51.867479
    stream << "." <<std::fixed << std::setw(6) << std::setfill('0') << delta_us << std::put_time(&utc_time,"%z");

    return stream.str();
}

std::chrono::high_resolution_clock::time_point from_iso_8601(const std::string& time)
{
    std::stringstream stream(time);

    // get_time parses as if it is local time. We need to fill in "is daylight savings" ourselves
    std::time_t t = std::time(nullptr);
    std::tm local_tm = *std::localtime(&t);
    stream >> std::get_time(&local_tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::high_resolution_clock::from_time_t(std::mktime(&local_tm));
}

namespace std {
std::ostream& operator<<(std::ostream& output, const std::optional<uuids::uuid>& value) {
    if (value) {
        output << uuids::to_string(*value);
    } else {
        output << "std::nullopt";
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const std::optional<std::string>& value) {
    if (value) {
        output << "\"" << *value << "\"";
    } else {
        output << "std::nullopt";
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const std::optional<uint64_t>& value) {
    if (value) {
        output << *value;
    } else {
        output << "std::nullopt";
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const report_portal::launch_mode& value) {
    static const std::map<report_portal::launch_mode, std::string> valid_values = {
        {report_portal::launch_mode::def, "default"},
        {report_portal::launch_mode::debug, "debug"}
    };

    output << valid_values.at(value);
    return output;
}

std::ostream& operator<<(std::ostream& output, const std::optional<report_portal::launch_mode>& value) {
    if (value) {
        output << *value;
    } else {
        output << "std::nullopt";
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const report_portal::test_item_type& value) {
    static const std::map<report_portal::test_item_type, std::string> valid_values = {
        {report_portal::test_item_type::suite, "suite"},
        {report_portal::test_item_type::story, "story"},
        {report_portal::test_item_type::test, "test"},
        {report_portal::test_item_type::scenario, "scenario"},
        {report_portal::test_item_type::step, "step"},
        {report_portal::test_item_type::before_class, "before_class"},
        {report_portal::test_item_type::before_groups, "before_groups"},
        {report_portal::test_item_type::before_method, "before_method"},
        {report_portal::test_item_type::before_suite, "before_suite"},
        {report_portal::test_item_type::before_test, "before_test"},
        {report_portal::test_item_type::after_class, "after_class"},
        {report_portal::test_item_type::after_groups, "after_groups"},
        {report_portal::test_item_type::after_method, "after_method"},
        {report_portal::test_item_type::after_suite, "after_suite"},
        {report_portal::test_item_type::after_test, "after_test"}
    };

    output << valid_values.at(value);
    return output;
}

std::ostream& operator<<(std::ostream& output, const std::optional<report_portal::test_item_type>& value) {
    if (value) {
        output << *value;
    } else {
        output << "std::nullopt";
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const report_portal::test_item_status& value) {
    static const std::map<report_portal::test_item_status, std::string> valid_values = {
        {report_portal::test_item_status::inherit, "inherit"},
        {report_portal::test_item_status::passed, "passed"},
        {report_portal::test_item_status::failed, "failed"},
        {report_portal::test_item_status::stopped, "stopped"},
        {report_portal::test_item_status::skipped, "skipped"},
        {report_portal::test_item_status::interrupted, "interrupted"},
        {report_portal::test_item_status::cancelled, "cancelled"}
    };

    output << valid_values.at(value);
    return output;
}

std::ostream& operator<<(std::ostream& output, const std::optional<report_portal::test_item_status>& value) {
    if (value) {
        output << *value;
    } else {
        output << "std::nullopt";
    }

    return output;
}

std::ostream& operator<<(std::ostream& output, const std::chrono::high_resolution_clock::time_point& value) {
    output << to_iso_8601(value);
    return output;
}


}
