#include <reportportal/service.hpp>
#include <reportportal/rapidjson_serializer.hpp>
#include <string>
#include <cstring>
#include <uuid.h>
#include <chrono>
#include <curl/curl.h>

namespace report_portal
{

static size_t write_function(char *buffer, size_t size, size_t nmemb, void* userdata) {
    std::string* result = static_cast<std::string*>(userdata);
    const size_t realsize = size * nmemb;

    result->append(buffer, nmemb);
    return realsize;
}

struct read_function_data
{
    read_function_data(const std::string& ref_data)
      : data(ref_data),
        index(0)
    {}

    const std::string& data;
    uint64_t index;
};

static size_t read_function(char *buffer, size_t size, size_t nitems, void* user_data_void) {
    read_function_data* user_data = static_cast<read_function_data*>(user_data_void);

    const size_t amount_copied = std::min(nitems, user_data->data.length());

    const char* string_beggining = user_data->data.c_str();
    std::strncpy(buffer, string_beggining + user_data->index, amount_copied);
    user_data->index += amount_copied;

    return amount_copied;
}

static bool is_valid_time(const std::chrono::high_resolution_clock::time_point& time_point)
{
    return time_point.time_since_epoch().count() > 0;
}

service::service(const std::string& endpoint, const std::string& project, const std::string& username, const std::string& password)
  : _endpoint(endpoint),
    _project(project),
    _api_url(endpoint + "/api/v1/" + project),
    _api_token(),
    _serializer()
{
    const std::string ui_token_responce_serialized = get_ui_token(endpoint, username, password);
    const ui_token_responce ui_responce = _serializer.deserialize_ui_token_responce(ui_token_responce_serialized);

    const std::string api_token_responce_serialized = get_api_token(endpoint, ui_responce.access_token());
    const api_token_responce api_responce = _serializer.deserialize_api_token_responce(api_token_responce_serialized);

    _api_token = api_responce.access_token();
}

begin_launch_responce service::begin_launch(
    const std::string& name,
    const std::chrono::high_resolution_clock::time_point& start_time,
    std::optional<std::string> description,
    std::optional<uuids::uuid> uuid,
    std::optional<attribute_map> attributes,
    std::optional<launch_mode> mode,
    std::optional<bool> is_rerun,
    std::optional<uuids::uuid> rerunof)
{

    if (!is_valid_time(start_time)) {
        throw std::invalid_argument("start_time is not valid");
    }

    const std::string request = _serializer.serialize_begin_launch(
        name,
        start_time,
        description,
        uuid,
        attributes,
        mode,
        is_rerun,
        rerunof);
    const std::string responce = post("launch", request);
    return _serializer.deserialize_begin_launch_responce(responce);
}

end_launch_responce service::end_launch(
    const uuids::uuid& uuid,
    const std::chrono::high_resolution_clock::time_point& end_time)
{
    if (!is_valid_time(end_time)) {
        throw std::invalid_argument("end_time is not valid");
    }

    const std::string request = _serializer.serialize_end_launch(
        end_time);
    const std::string responce = put(
        "launch/" + uuids::to_string(uuid) + "/finish",
         request);
    return _serializer.deserialize_end_launch_responce(responce);
}

begin_test_item_responce service::begin_test_item(
    const std::string& name,
    const std::chrono::high_resolution_clock::time_point& start_time,
    test_item_type type,
    const uuids::uuid& launch_uuid,
    std::optional<uuids::uuid> parent_uuid,
    std::optional<std::string> description,
    std::optional<attribute_map> attributes,
    std::optional<uuids::uuid> uuid,
    std::optional<std::string> coderef,
    std::optional<std::map<std::string, std::string> > parameters,
    std::optional<bool> retry,
    std::optional<bool> has_stats)
{
    if (!is_valid_time(start_time)) {
        throw std::invalid_argument("end_time is not valid");
    }

    const std::string request = _serializer.serialize_begin_test_item(
        name,
        start_time,
        type,
        launch_uuid,
        description,
        attributes,
        uuid,
        coderef,
        parameters,
        retry,
        has_stats);

    std::string append_url = "item";
    if (!parent_uuid->is_nil()) {
       append_url += "/" + uuids::to_string(*parent_uuid);
    }

    const std::string responce = post(append_url, request);
    return _serializer.deserialize_begin_test_item_responce(responce);
}

end_test_item_responce service::end_test_item(
    const uuids::uuid& uuid,
    const uuids::uuid& launch_uuid,
    const std::chrono::high_resolution_clock::time_point end_time,
    std::optional<test_item_status> status,
    std::optional<issue> issue)
{
    if (!is_valid_time(end_time)) {
        throw std::invalid_argument("end_time is not valid");
    }

    const std::string request = _serializer.serialize_end_test_item(
        end_time,
        launch_uuid,
        std::nullopt, // [TODO]
        status,
        issue);

    const std::string responce = put(
        "/item/" + uuids::to_string(uuid),
        request);
    return _serializer.deserialize_end_test_item_responce(responce);
}

void service::batch_logs(const std::vector<log> logs)
{
    const std::string request = _serializer.serialize_batched_logs(logs);
    const std::string responce = mime("/log", request);
}


std::string service::post(const std::string& append_url, const std::string& data)
{
    CURL *curl;
    CURLcode res;
    std::string result;

    /* In windows, this will init winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the data.
        // /api/{version}/{projectName}/launch
        const std::string url = _api_url + "/" + append_url;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Now specify the POST data
        // Does not copy data passed in so make sure it exists until the associated
        // transfer finishes. Use CURLOPT_COPYPOSTFIELDS if needing to copy the data.
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        // header
        curl_slist *header_list = nullptr;
        header_list = curl_slist_append(header_list, "Content-Type: application/json");

        const std::string authorization_header = "Authorization: bearer " + uuids::to_string(_api_token);
        header_list = curl_slist_append(header_list, authorization_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        // The write funciton may be called multiple times when the data is returned in chunks.
        // So we append to the string.
        curl_easy_setopt(
            curl,
            CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // Perform the request, res will get the return code.
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        // ALWAYS CLEANUP
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
    }

    curl_global_cleanup();
    return result;
}

std::string service::mime(const std::string& append_url, const std::string& data)
{
    CURL *curl = nullptr;
    CURLcode res;
    std::string result;

    /* In windows, this will init winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the data.
        // /api/{version}/{projectName}/launch
        const std::string url = _api_url + "/" + append_url;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Now specify the POST data
        // Does not copy data passed in so make sure it exists until the associated
        // transfer finishes. Use CURLOPT_COPYPOSTFIELDS if needing to copy the data.
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_mime* mime = curl_mime_init(curl);
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_data(part, data.c_str(), data.length());
        curl_mime_name(part, "json_request_part");
        curl_mime_type(part, "application/json");
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // header
        curl_slist *header_list = nullptr;
        header_list = curl_slist_append(header_list, "Content-Type: multipart/form-data");

        const std::string authorization_header = "Authorization: bearer " + uuids::to_string(_api_token);
        header_list = curl_slist_append(header_list, authorization_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        // The write funciton may be called multiple times when the data is returned in chunks.
        // So we append to the string.
        curl_easy_setopt(
            curl,
            CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // Perform the request, res will get the return code.
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        // ALWAYS CLEANUP
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
    }

    curl_global_cleanup();
    return result;
}


std::string service::put(const std::string& append_url, const std::string& data)
{
    CURL *curl;
    CURLcode res;
    std::string result;

    /* In windows, this will init winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the data.
        // /api/{version}/{projectName}/launch
        const std::string url = _api_url + "/" + append_url;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        read_function_data read_data(data);
        curl_easy_setopt(curl, CURLOPT_READDATA, &read_data);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_function);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE, data.length());

        // header
        curl_slist *header_list = nullptr;
        header_list = curl_slist_append(header_list, "Content-Type: application/json");

        const std::string authorization_header = "Authorization: bearer " + uuids::to_string(_api_token);
        header_list = curl_slist_append(header_list, authorization_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

        // The write funciton may be called multiple times when the data is returned in chunks.
        // So we append to the string.
        curl_easy_setopt(
            curl,
            CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // Perform the request, res will get the return code.
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        // ALWAYS CLEANUP
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
    }

    curl_global_cleanup();
    return result;
}


std::string service::get_ui_token(const std::string& endpoint, const std::string& username, const std::string password) {
    CURL *curl;
    CURLcode res;
    std::string result;

    /* In windows, this will init winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the data.
        // /api/{version}/{projectName}/launch
        const std::string url = _endpoint + "/uat/sso/oauth/token";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        // Now specify the POST data
        // Does not copy data passed in so make sure it exists until the associated
        // transfer finishes. Use CURLOPT_COPYPOSTFIELDS if needing to copy the data.
        const std::string data = "grant_type=password&username=" + username + "&password=" + password;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        // header
        curl_slist *header_list = nullptr;
        header_list = curl_slist_append(header_list, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "ui");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "uiman");

        // The write funciton may be called multiple times when the data is returned in chunks.
        // So we append to the string.
        curl_easy_setopt(
            curl,
            CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // Perform the request, res will get the return code.
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        // ALWAYS CLEANUP
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
    }

    curl_global_cleanup();
    return result;
}

std::string service::get_api_token(const std::string& endpoint, const std::string& ui_token) {
    CURL *curl;
    CURLcode res;
    std::string result;

    /* In windows, this will init winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if (curl) {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the data.
        // /api/{version}/{projectName}/launch
        const std::string url = _endpoint + "/uat/sso/me/apitoken";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // header
        curl_slist *header_list = nullptr;
        const std::string authorization_header = "Authorization: bearer " + ui_token;
        header_list = curl_slist_append(header_list, authorization_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        // The write funciton may be called multiple times when the data is returned in chunks.
        // So we append to the string.
        curl_easy_setopt(
            curl,
            CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

        // Perform the request, res will get the return code.
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        // ALWAYS CLEANUP
        curl_easy_cleanup(curl);
        curl_slist_free_all(header_list);
    }

    curl_global_cleanup();
    return result;
}

} // namespace report_portal
