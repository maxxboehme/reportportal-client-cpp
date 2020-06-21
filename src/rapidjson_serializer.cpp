#include <reportportal/rapidjson_serializer.hpp>
#include <reportportal/iservice.hpp>
#include <reportportal/launch.hpp>
#include <reportportal/log.hpp>
#include <reportportal/attribute_map.hpp>
#include <reportportal/issue.hpp>
#include <reportportal/ijson_serializer.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <uuid.h>

namespace report_portal
{

static void add_required(rapidjson::Document& doc, const std::string& member, const std::string& value)
{
    if (value.empty()) {
        // [TODO] throw std::invalid_argument("value was empty");
    }

    doc.AddMember(
        rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
        rapidjson::Value(rapidjson::StringRef(value.c_str(), value.length()), doc.GetAllocator()).Move(),
        doc.GetAllocator());
}

static void add_required(rapidjson::Document& doc, const std::string& member, test_item_type value)
{
    static const std::map<test_item_type, std::string> valid_values = {
        {test_item_type::suite, "suite"},
        {test_item_type::story, "story"},
        {test_item_type::test, "test"},
        {test_item_type::scenario, "scenario"},
        {test_item_type::step, "step"},
        {test_item_type::before_class, "before_class"},
        {test_item_type::before_groups, "before_groups"},
        {test_item_type::before_method, "before_method"},
        {test_item_type::before_suite, "before_suite"},
        {test_item_type::before_test, "before_test"},
        {test_item_type::after_class, "after_class"},
        {test_item_type::after_groups, "after_groups"},
        {test_item_type::after_method, "after_method"},
        {test_item_type::after_suite, "after_suite"},
        {test_item_type::after_test, "after_test"}
    };

    const std::string& value_string = valid_values.at(value);
    doc.AddMember(
        rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
        rapidjson::Value(rapidjson::StringRef(value_string.c_str(), value_string.length()), doc.GetAllocator()).Move(),
        doc.GetAllocator());
}

static void add_required(rapidjson::Document& doc, const std::string& member, const uuids::uuid& value)
{
    if (value.is_nil()) {
        throw std::invalid_argument("UUID cannot be null");
    }

    const std::string uuid_string = uuids::to_string(value);
    doc.AddMember(
        rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
        rapidjson::Value(rapidjson::StringRef(uuid_string.c_str(), uuid_string.length()), doc.GetAllocator()).Move(),
        doc.GetAllocator());
}

std::string to_string(const std::tm& tm)
{
    std::stringstream stream;
    stream << "tm:\n";
    stream << "- sec: " << tm.tm_sec << "\n";
    stream << "- min: " << tm.tm_min << "\n";
    stream << "- hour: " << tm.tm_hour << "\n";
    stream << "- day: " << tm.tm_mday << "\n";
    stream << "- month: " << tm.tm_mon << "\n";
    stream << "- year: " << tm.tm_year << "\n";
    stream << "- week day: " << tm.tm_wday << "\n";
    stream << "- year day: " << tm.tm_yday << "\n";
    stream << "- isdst: " << tm.tm_isdst << "\n";

    return stream.str();
}

static std::string to_iso_8601(const std::chrono::system_clock::time_point& time)
{
    std::time_t epoch_seconds = std::chrono::system_clock::to_time_t(time);

    // Format this as date time to seconds resolution.
    // e.g. 2016-08-30T08:18:51
    std::stringstream stream;
    std::tm utc_time = *std::localtime(&epoch_seconds);
    stream << std::put_time(&utc_time, "%FT%T");

    // If new now convert back to a time_point we will get the time truncated
    // to while seconds
    auto truncated = std::chrono::system_clock::from_time_t(epoch_seconds);

    // Now we wubtract this seconds count from the original time to
    // get the number of extra microseconds...
    uint64_t delta_us = std::chrono::duration_cast<std::chrono::microseconds>(time - truncated).count();

    // And append this to the output stream as fractional seconds
    // e.g. 2016-08-30T08:18:51.867479
    stream << "." <<std::fixed << std::setw(6) << std::setfill('0') << delta_us << std::put_time(&utc_time,"%z");

    return stream.str();
}

static void add_required(rapidjson::Document& doc, const std::string& member, const std::chrono::system_clock::time_point& value)
{
    const std::string iso_8601_string = to_iso_8601(value);
    doc.AddMember(
        rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
        rapidjson::Value(rapidjson::StringRef(iso_8601_string.c_str(), iso_8601_string.length()), doc.GetAllocator()).Move(),
        doc.GetAllocator());
}

static void add_optional(rapidjson::Document& doc, const std::string& member, const std::optional<std::string>& value)
{
    if (value) {
        doc.AddMember(
            rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(value->c_str(), value->length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());
    }
}

static void add_optional(rapidjson::Document& doc, const std::string& member, const std::optional<uuids::uuid>& value)
{
    if (value && !value->is_nil()) {
        const std::string& uuid_string = uuids::to_string(*value);
        doc.AddMember(
            rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(uuid_string.c_str(), uuid_string.length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());
    }
}

static void add_optional(rapidjson::Document& doc, const std::string& member, const std::optional<bool>& value, bool default_value = false)
{
    if (value && *value != default_value) {
        doc.AddMember(
            rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
            *value,
            doc.GetAllocator());
    }
}

static void add_optional(rapidjson::Document& doc, const std::string& member, const std::optional<launch_mode>& value)
{
    static const std::map<launch_mode, std::string> valid_values = {
        {launch_mode::def, "DEFAULT"},
        {launch_mode::debug, "DEBUG"}
    };

    if (value && *value != launch_mode::def) {
        const std::string& value_string = valid_values.at(*value);
        doc.AddMember(
            rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(value_string.c_str(), value_string.length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());
    }
}

static void add_optional(rapidjson::Document& doc, const std::string& member, std::optional<test_item_status>& value)
{
    static const std::map<test_item_status, std::string> valid_values = {
        {test_item_status::passed, "passed"},
        {test_item_status::failed, "failed"},
        {test_item_status::stopped, "stopped"},
        {test_item_status::skipped, "skipped"},
        {test_item_status::interrupted, "interrupted"},
        {test_item_status::cancelled, "cancelled"}
    };

    if (value && *value != test_item_status::inherit) {
        const std::string& value_string = valid_values.at(*value);
        doc.AddMember(
            rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(value_string.c_str(), value_string.length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());
    }
}

static void add_optional(rapidjson::Document& doc, const std::string& member, const std::optional<issue>& value)
{
    static const std::map<issue::type, std::string> valid_issue_type_values = {
        {issue::type::no_defect, "nd"},
        {issue::type::to_investigate, "ti"},
        {issue::type::product, "pd"},
        {issue::type::automation, "ab"},
        {issue::type::system, "si"}
    };

    if (!value) {
        return;
    }

    rapidjson::Value issue_value;
    issue_value.SetObject();

    const std::string& issue_type_string = valid_issue_type_values.at(value->get_type());
    std::stringstream type_locator_id_stream;
    type_locator_id_stream << std::setw(3) << std::setfill('0') << value->get_type_locator_id();
    const std::string full_issue_type_identifer = issue_type_string + type_locator_id_stream.str();
    issue_value.AddMember(
        rapidjson::Value("issueType", doc.GetAllocator()).Move(),
        rapidjson::Value(rapidjson::StringRef(full_issue_type_identifer.c_str(), full_issue_type_identifer.length()), doc.GetAllocator()).Move(),
        doc.GetAllocator());

    issue_value.AddMember(
        rapidjson::Value("comment", doc.GetAllocator()).Move(),
        rapidjson::Value(rapidjson::StringRef(value->get_comment().c_str(), value->get_comment().length()), doc.GetAllocator()).Move(),
        doc.GetAllocator());

    issue_value.AddMember(
        rapidjson::Value("autoAnalyzed", doc.GetAllocator()).Move(),
        value->is_auto_analyzed(),
        doc.GetAllocator());

    issue_value.AddMember(
        rapidjson::Value("ignoreAnalyzer", doc.GetAllocator()).Move(),
        value->ignore_analyzer(),
        doc.GetAllocator());

    doc.AddMember(
        rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
        issue_value.Move(),
        doc.GetAllocator());
}


// static void add_optional(rapidjson::Document& doc, const std::string& member, const std::map<std::string, std::string>& value)
// {
//     if (value != launch_mode::def) {
//         doc.AddMember(
//             rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
//             "DEBUG",
//             doc.GetAllocator());
//     }
// }

// [TODO]
// static void add_optional(rapidjson::Document& doc, const std::string& member, const attribute_map& attributes)
// {
//
//     if (value != default_value) {
//         doc.AddMember(
//             rapidjson::Value(rapidjson::StringRef(member.c_str(), member.length()), doc.GetAllocator()).Move(),
//             value,
//             doc.GetAllocator());
//     }
// }

static void throw_if_error(const rapidjson::Document& doc) {
     if (doc.HasMember("errorCode")) {
        const int32_t error_code = doc["errorCode"].GetInt();
        const std::string message = doc["message"].GetString();
        const std::string error_message = "Error Code: " + std::to_string(error_code) + "\n" + message;
        throw std::runtime_error(error_message);
    }
}

std::string rapidjson_serializer::serialize_begin_launch(
    const std::string& name,
    const std::chrono::system_clock::time_point& start_time,
    std::optional<std::string> description,
    std::optional<uuids::uuid> uuid,
    std::optional<attribute_map> attributes,
    std::optional<launch_mode> mode,
    std::optional<bool> is_rerun,
    std::optional<uuids::uuid> rerunof)
{
    rapidjson::Document doc;
    doc.SetObject();
    add_required(doc, "name", name);
    add_required(doc, "startTime", start_time);

    add_optional(doc, "description", description);
    add_optional(doc, "uuid", uuid);
    // [TODO] add_optional(doc, "attributes", attributes);
    add_optional(doc, "mode", rerunof);
    add_optional(doc, "rerunOf", rerunof);
    add_optional(doc, "rerun", is_rerun);
    add_optional(doc, "rerunOf", rerunof);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

begin_launch_responce rapidjson_serializer::deserialize_begin_launch_responce(const std::string& responce)
{
    rapidjson::Document doc;
    doc.Parse(responce.c_str());
    throw_if_error(doc);

    const uuids::uuid id = uuids::uuid::from_string(doc["id"].GetString());

    std::optional<uint64_t> number;
    if (doc.HasMember("number")) {
        number = doc["number"].GetUint64();
    }

    return begin_launch_responce(id, number);
}

std::string rapidjson_serializer::serialize_end_launch(
    const std::chrono::system_clock::time_point& end_time)
{
    rapidjson::Document doc;
    doc.SetObject();
    add_required(doc, "endTime", end_time);

    // add_optional(doc, "status", status);
    // add_optional(doc, "description", description);
    // add_optional(doc, "attributes", attributes);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

end_launch_responce rapidjson_serializer::deserialize_end_launch_responce(const std::string& responce)
{
    rapidjson::Document doc;
    doc.Parse(responce.c_str());
    throw_if_error(doc);

    const uuids::uuid id = uuids::uuid::from_string(doc["id"].GetString());
    std::optional<uint64_t> number;
    if (doc.HasMember("number")) {
        number = doc["number"].GetUint64();
    }

    std::optional<std::string> link;
    if (doc.HasMember("link")) {
        link = doc["link"].GetString();
    }

    return end_launch_responce(id, number, link);
}

std::string rapidjson_serializer::serialize_begin_test_item(
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
    std::optional<bool> has_stats)
{
    rapidjson::Document doc;
    doc.SetObject();
    add_required(doc, "name", name);
    add_required(doc, "startTime", start_time);
    add_required(doc, "type", type);
    add_required(doc, "launchUuid", launch_uuid);

    add_optional(doc, "description", description);
    // [TODO] add_optional(doc, "attributes", attributes);
    add_optional(doc, "uuid", uuid);
    add_optional(doc, "coderef", coderef);
    // [TODO] add_optional(doc, "parameters", parameters);
    add_optional(doc, "retry", retry);
    add_optional(doc, "hasStats", has_stats, true /* default value*/);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

begin_test_item_responce rapidjson_serializer::deserialize_begin_test_item_responce(const std::string& responce)  {
    rapidjson::Document doc;
    doc.Parse(responce.c_str());
    throw_if_error(doc);

    const uuids::uuid id = uuids::uuid::from_string(doc["id"].GetString());
    return begin_test_item_responce(id);
}

std::string rapidjson_serializer::serialize_end_test_item(
    const std::chrono::system_clock::time_point& end_time,
    const uuids::uuid& launch_uuid,
    std::optional<bool> retry,
    std::optional<test_item_status> status,
    std::optional<issue> issue)
{
    rapidjson::Document doc;
    doc.SetObject();
    add_required(doc, "endTime", end_time);
    add_required(doc, "launchUuid", launch_uuid);

    add_optional(doc, "status", status);
    add_optional(doc, "retry", retry);
    add_optional(doc, "issue", issue);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

end_test_item_responce rapidjson_serializer::deserialize_end_test_item_responce(const std::string& responce)  {
    rapidjson::Document doc;
    doc.Parse(responce.c_str());
    throw_if_error(doc);

    const std::string message = doc["message"].GetString();
    return end_test_item_responce(message);

}

std::string rapidjson_serializer::serialize_batched_logs(const std::vector<log> logs)
{
    rapidjson::Document doc;
    doc.SetArray();
    for(const log& log : logs) {
        rapidjson::Value log_value;
        log_value.SetObject();

        const std::string launch_id_string = uuids::to_string(log.launch_id());
        log_value.AddMember(
            rapidjson::Value("launchUuid", doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(launch_id_string.c_str(), launch_id_string.length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());

        const std::string iso_8601_string = to_iso_8601(log.time());
        log_value.AddMember(
            rapidjson::Value("time", doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(iso_8601_string.c_str(), iso_8601_string.length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());

        if (!log.test_item_id().is_nil()) {
            const std::string test_item_id_string = uuids::to_string(log.test_item_id());
            log_value.AddMember(
                rapidjson::Value("itemUuid", doc.GetAllocator()).Move(),
                rapidjson::Value(rapidjson::StringRef(test_item_id_string.c_str(), test_item_id_string.length()), doc.GetAllocator()).Move(),
                doc.GetAllocator());
        }

        log_value.AddMember(
            rapidjson::Value("message", doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(log.message().c_str(), log.message().length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());

        static const std::map<log_level, std::string> valid_level_values = {
            {log_level::unknown, "unknown"},
            {log_level::trace, "trace"},
            {log_level::debug, "debug"},
            {log_level::info, "info"},
            {log_level::warn, "warn"},
            {log_level::error, "error"},
            {log_level::fatal, "fatal"}
        };

        const std::string& level_string = valid_level_values.at(log.level());
        log_value.AddMember(
            rapidjson::Value("level", doc.GetAllocator()).Move(),
            rapidjson::Value(rapidjson::StringRef(level_string.c_str(), level_string.length()), doc.GetAllocator()).Move(),
            doc.GetAllocator());

        if (log.attachment().has_value()) {
            rapidjson::Value attachment_value;
            attachment_value.SetObject();

            const std::optional<attachment>& attachment = log.attachment();
            attachment_value.AddMember(
                rapidjson::Value("name", doc.GetAllocator()).Move(),
                rapidjson::Value(rapidjson::StringRef(attachment->name().c_str(), attachment->name().length()), doc.GetAllocator()).Move(),
                doc.GetAllocator());

            attachment_value.AddMember(
                rapidjson::Value("data", doc.GetAllocator()).Move(),
                rapidjson::Value(rapidjson::StringRef(attachment->data().c_str(), attachment->data().length()), doc.GetAllocator()).Move(),
                doc.GetAllocator());

            attachment_value.AddMember(
                rapidjson::Value("type", doc.GetAllocator()).Move(),
                rapidjson::Value(rapidjson::StringRef(attachment->mime_type().c_str(), attachment->mime_type().length()), doc.GetAllocator()).Move(),
                doc.GetAllocator());

            log_value.AddMember(
                rapidjson::Value("file", doc.GetAllocator()).Move(),
                attachment_value.Move(),
                doc.GetAllocator());
        }

        doc.PushBack(log_value, doc.GetAllocator());
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

ui_token_responce rapidjson_serializer::deserialize_ui_token_responce(const std::string responce) {
    rapidjson::Document doc;
    doc.Parse(responce.c_str());
    throw_if_error(doc);

    const std::string access_token = doc["access_token"].GetString();
    const std::string token_type = doc["token_type"].GetString();
    const std::string refresh_token = doc["refresh_token"].GetString();
    const uint64_t expires_in = doc["expires_in"].GetUint64();
    const std::string scope = doc["scope"].GetString();
    const uuids::uuid jti = uuids::uuid::from_string(doc["jti"].GetString());

    return ui_token_responce(
        access_token,
        token_type,
        refresh_token,
        expires_in,
        scope,
        jti);
}

api_token_responce rapidjson_serializer::deserialize_api_token_responce(const std::string responce) {
    rapidjson::Document doc;
    doc.Parse(responce.c_str());
    throw_if_error(doc);

    const uuids::uuid access_token = uuids::uuid::from_string(doc["access_token"].GetString());
    const std::string token_type = doc["token_type"].GetString();
    const std::string scope = doc["scope"].GetString();

    return api_token_responce(
        access_token,
        token_type,
        scope);
}

}
