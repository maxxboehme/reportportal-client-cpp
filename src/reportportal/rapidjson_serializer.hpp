#pragma once

#include <reportportal/iservice.hpp>
#include <reportportal/launch.hpp>
#include <reportportal/log.hpp>
#include <reportportal/attribute_map.hpp>
#include <reportportal/issue.hpp>
#include <reportportal/ijson_serializer.hpp>
#include <uuid.h>

namespace report_portal
{

class rapidjson_serializer : public ijson_serializer
{
    public:
    std::string serialize_begin_launch(
        const std::string& name,
        const std::chrono::system_clock::time_point& start_time,
        std::optional<std::string> description,
        std::optional<uuids::uuid> uuid,
        std::optional<attribute_map> attributes,
        std::optional<launch_mode> mode,
        std::optional<bool> is_rerun,
        std::optional<uuids::uuid> rerunof) override;

    begin_launch_responce deserialize_begin_launch_responce(const std::string& responce) override;

    std::string serialize_end_launch(
        const std::chrono::system_clock::time_point& end_time) override;

    end_launch_responce deserialize_end_launch_responce(const std::string& responce) override;

    std::string serialize_begin_test_item(
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
        std::optional<bool> has_stats) override;

    begin_test_item_responce deserialize_begin_test_item_responce(const std::string& responce) override;

    std::string serialize_end_test_item(
        const std::chrono::system_clock::time_point& end_time,
        const uuids::uuid& launch_uuid,
        std::optional<bool> retry,
        std::optional<test_item_status> status,
        std::optional<issue> issue) override;

    end_test_item_responce deserialize_end_test_item_responce(const std::string& responce) override;

    std::string serialize_batched_logs(const std::vector<log> logs) override;

    ui_token_responce deserialize_ui_token_responce(const std::string responce);

    api_token_responce deserialize_api_token_responce(const std::string responce);
};

}
