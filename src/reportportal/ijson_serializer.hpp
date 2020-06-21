#pragma once

#include <reportportal/issue.hpp>
#include <reportportal/attribute_map.hpp>
#include <reportportal/log.hpp>
#include <string>
#include <uuid.h>
#include <map>
#include <optional>

namespace report_portal
{
enum class launch_mode : uint32_t {
    def = 0, // default
    debug = 1
};

enum class test_item_type : uint32_t {
    suite = 1,
    story = 2,
    test = 3,
    scenario = 4,
    step = 5,
    before_class = 6,
    before_groups = 7,
    before_method = 8,
    before_suite = 9,
    before_test = 10,
    after_class = 11,
    after_groups = 12,
    after_method = 13,
    after_suite = 14,
    after_test = 15
};

enum class test_item_status : uint32_t {
    inherit = 1,
    passed = 2,
    failed = 3,
    stopped = 4,
    skipped = 5,
    interrupted = 6,
    cancelled = 7
};

// class error_responce
// {
//     public:
//         error(int32_t code, const std::string& message)
//           : _code(code),
//             _message(message)
//         {}
// 
//     int32_t code() const {
//         return _code;
//     }
// 
//     std::string message() const {
//         return _message;
//     }
// 
//     private;
//     int32_t code;
//     std::string message;
// 
// };

class ui_token_responce final
{
    public:
        ui_token_responce(
            const std::string& access_token,
            const std::string& token_type,
            const std::string& refresh_token,
            const uint64_t expires_in,
            const std::string& scope,
            const uuids::uuid& jti)
          : _access_token(access_token),
            _token_type(token_type),
            _refresh_token(refresh_token),
            _expires_in(expires_in),
            _scope(scope),
            _jti(jti)
        {}

        std::string access_token() const {
            return _access_token;
        }

    private:
        std::string _access_token;
        std::string _token_type;
        std::string _refresh_token;
        uint64_t _expires_in;
        std::string _scope;
        uuids::uuid _jti;
};

class api_token_responce final
{
    public:
        api_token_responce(
            const uuids::uuid& access_token,
            const std::string& token_type,
            const std::string& scope)
          : _access_token(access_token),
            _token_type(token_type),
            _scope(scope)
        {}

        uuids::uuid access_token() const {
            return _access_token;
        }

    private:
        uuids::uuid _access_token;
        std::string _token_type;
        std::string _scope;
};

class begin_launch_responce final
{
    public:
    begin_launch_responce(const uuids::uuid& id, std::optional<uint64_t> number = std::nullopt)
      : _id(id),
        _number(number)
    {}

    uuids::uuid id() const {
        return _id;
    }

    std::optional<uint64_t> number() const {
        return _number;
    }

    private:
    uuids::uuid _id;
    std::optional<uint64_t> _number;
};

class end_launch_responce final
{
    public:
    end_launch_responce(const uuids::uuid& id, std::optional<uint64_t> number = std::nullopt, std::optional<std::string> link = std::nullopt)
      : _id(id),
        _number(number),
        _link(link)
    {}

    uuids::uuid id() const {
        return _id;
    }

    std::optional<uint64_t> number() const {
        return _number;
    }

    std::optional<std::string> link() const {
        return _link;
    }

    private:
    uuids::uuid _id;
    std::optional<uint64_t> _number;
    std::optional<std::string> _link;
};

class begin_test_item_responce final
{
    public:
    begin_test_item_responce(const uuids::uuid& id)
      : _id(id)
    {}

    uuids::uuid id() const {
        return _id;
    }

    private:
    uuids::uuid _id;
};

class end_test_item_responce final
{
    public:
    end_test_item_responce(const std::string& message)
      : _message(message)
    {}

    const std::string& message() const {
        return _message;
    }

    private:
    std::string _message;
};

class ijson_serializer
{
    public:
    virtual std::string serialize_begin_launch(
        const std::string& name,
        const std::chrono::system_clock::time_point& start_time,
        std::optional<std::string> description,
        std::optional<uuids::uuid> uuid,
        std::optional<attribute_map> attributes,
        std::optional<launch_mode> mode,
        std::optional<bool> is_rerun,
        std::optional<uuids::uuid> rerunof) = 0;

    virtual begin_launch_responce deserialize_begin_launch_responce(const std::string& responce) = 0;

    virtual std::string serialize_end_launch(
        const std::chrono::system_clock::time_point& end_time) = 0;

    virtual end_launch_responce deserialize_end_launch_responce(const std::string& responce) = 0;

    virtual std::string serialize_begin_test_item(
        const std::string& name,
        const std::chrono::system_clock::time_point& start_time,
        test_item_type type,
        const uuids::uuid& launch_uuid,
        std::optional<std::string> description,
        std::optional<attribute_map> attributes,
        std::optional<uuids::uuid> uuid,
        std::optional<std::string> coderef,
        std::optional<std::map<std::string, std::string> > parameters,
        std::optional<bool> retry,
        std::optional<bool> has_stats) = 0;

    virtual begin_test_item_responce deserialize_begin_test_item_responce(const std::string& responce) = 0;

    virtual std::string serialize_end_test_item(
        const std::chrono::system_clock::time_point& end_time,
        const uuids::uuid& launch_uuid,
        std::optional<bool> retry,
        std::optional<test_item_status> status,
        std::optional<issue> issue) = 0;

    virtual end_test_item_responce deserialize_end_test_item_responce(const std::string& responce) = 0;

    virtual std::string serialize_batched_logs(const std::vector<log> logs) = 0;

    virtual ~ijson_serializer() {}
};

}
