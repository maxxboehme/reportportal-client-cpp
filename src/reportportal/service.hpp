#pragma once

#include <reportportal/rapidjson_serializer.hpp>
#include <reportportal/iservice.hpp>
#include <string>
#include <uuid.h>
#include <chrono>

namespace report_portal
{

class service final : public iservice
{
    public:
        /**
         * @param endpoint
         * @param project
         * @param username
         * @param password
         */
        service(const std::string& endpoint, const std::string& project, const std::string& username, const std::string& password);

        begin_launch_responce begin_launch(
            const std::string& name,
            const std::chrono::system_clock::time_point& start_time,
            std::optional<std::string> description,
            std::optional<uuids::uuid> uuid,
            std::optional<attribute_map> attributes,
            std::optional<launch_mode> mode,
            std::optional<bool> is_rerun,
            std::optional<uuids::uuid> rerunof) override;

        end_launch_responce end_launch(
            const uuids::uuid& uuid,
            const std::chrono::system_clock::time_point& end_time) override;

        begin_test_item_responce begin_test_item(
            const std::string& name,
            const std::chrono::system_clock::time_point& start_time,
            test_item_type type,
            const uuids::uuid& launch_uuid,
            std::optional<uuids::uuid> parent_uuid,
            std::optional<std::string> description,
            std::optional<attribute_map> attributes,
            std::optional<uuids::uuid> uuid,
            std::optional<std::string> coderef,
            std::optional<std::map<std::string, std::string> > parameters,
            std::optional<bool> retry,
            std::optional<bool> has_stats) override;

        end_test_item_responce end_test_item(
            const uuids::uuid& uuid,
            const uuids::uuid& launch_uuid,
            const std::chrono::system_clock::time_point end_time,
            std::optional<test_item_status> status,
            std::optional<issue> issue) override;

        void batch_logs(const std::vector<log> logs) override;

    private:
        // [TODO] static const char* const api_version = "/api/v1/";

        std::string _endpoint;
        std::string _project;
        std::string _api_url;
        uuids::uuid _api_token;

        rapidjson_serializer _serializer;

        std::string post(const std::string& append_url, const std::string& data);
        std::string mime(const std::string& append_url, const std::string& data);
        std::string put(const std::string& append_url, const std::string& data);

        std::string get_ui_token(const std::string& endpoint, const std::string& username, const std::string password);
        std::string get_api_token(const std::string& endpoint, const std::string& ui_token);
};

}
