#pragma once

#include <string>
#include <uuid.h>
#include <optional>
#include <chrono>

namespace report_portal
{

class attachment final
{
    public:
        attachment(const std::string name, const std::string& data, const std::string mime_type)
          : _name(name),
            _data(data),
            _mime_type(_mime_type)
        {}

        std::string name() const {
            return _name;
        }

        std::string data() const {
            return _data;
        }

        std::string mime_type() const {
            return _mime_type;
        }

    private:
        std::string _name;
        std::string _data;
        std::string _mime_type;
};

enum class log_level : uint32_t {
    unknown = 0,
    trace = 1,
    debug = 2,
    info = 3,
    warn = 4,
    error = 5,
    fatal = 6
};

class log final
{
    public:
        log(
            const uuids::uuid& launch_id,
            const std::chrono::system_clock::time_point& time,
            log_level level,
            const std::string& message);

        log(
            const uuids::uuid& launch_id,
            const uuids::uuid& test_item_id,
            const std::chrono::system_clock::time_point& time,
            log_level level,
            const std::string& message);

        uuids::uuid launch_id() const;

        uuids::uuid test_item_id() const;

        std::chrono::system_clock::time_point time() const;

        std::string message() const;

        log_level level() const;

        void set_attachment(const attachment& attachment);

        const std::optional<report_portal::attachment>& attachment() const;

    private:
        uuids::uuid _launch_id;
        uuids::uuid _test_item_id;
        std::chrono::system_clock::time_point _time;
        std::string _message;
        log_level _level;

        std::optional<report_portal::attachment> _attachment;
};

}
