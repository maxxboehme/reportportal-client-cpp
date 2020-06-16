#pragma once

#include <reportportal/iservice.hpp>
#include <reportportal/launch.hpp>
#include <reportportal/log.hpp>
#include <reportportal/issue.hpp>
#include <reportportal/itest_item_parent.hpp>
#include <ctime>
#include <string>
#include <map>
#include <uuid.h>
#include <chrono>
#include <vector>

namespace report_portal
{

class test_item final : public itest_item_parent
{
    public:
        test_item(
           launch& parent,
           const std::string name);

        test_item(
           launch& parent,
           const uuids::uuid& id,
           const std::string name);

        test_item(
           test_item& parent,
           const std::string name,
           test_item_type type);

        test_item(
           test_item& parent,
           const uuids::uuid& id,
           const std::string name,
           test_item_type type);

        uuids::uuid id() const override;
        uuids::uuid launch_id() const override;
        const iservice& service() const override;
        iservice& service() override;
        std::string name() const;
        test_item_type type() const;
        std::string description() const;
        void set_description(const std::string& description);
        attribute_map attributes() const;
        void set_attributes(const attribute_map& attributes);
        void add_attribute(const std::string& key, const std::string& value);
        void remove_attribute(const std::string& key);
        void add_tag(const std::string& tag);
        void remove_tag(const std::string& tag);
        std::string coderef() const;
        void set_coderef(const std::string& coderef);
        const std::map<std::string, std::string>& parameters() const;
        void set_parameters(const std::map<std::string, std::string>& parameters);
        void add_parameter(const std::string& parameter, const std::string value);
        void remove_parameter(const std::string& parameter);
        bool is_retry() const;
        void set_retry(bool is_retry);
        bool has_stats() const;
        void set_has_stats(bool has_stats);
        bool has_started() const override;
        bool has_ended() const override;
        void start(const std::chrono::high_resolution_clock::time_point& start_time);
        void set_status(test_item_status status);
        void end(const std::chrono::high_resolution_clock::time_point& end_time);
        void end(const std::chrono::high_resolution_clock::time_point& end_time, test_item_status status);
        void log(const std::chrono::high_resolution_clock::time_point& time, log_level level, const std::string& message);
        void log(const std::chrono::high_resolution_clock::time_point& time, log_level level, const std::string& message, const attachment& attachment);
        void flush_logs();

    private:
        itest_item_parent& _parent;
        std::string _name;
        test_item_type _type;
        std::string _description;
        attribute_map _attributes;
        std::string _coderef;
        std::map<std::string, std::string> _parameters;
        uuids::uuid _id;
        bool _is_retry;
        bool _has_stats;

        test_item_status _status;
//        issue _issue;

        std::vector<report_portal::log> _batched_logs;

        bool _has_started;
        bool _has_ended;
};

}
