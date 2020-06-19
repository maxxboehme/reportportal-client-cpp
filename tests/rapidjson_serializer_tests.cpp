#include <catch2/catch.hpp>
#include <fakeit.hpp>
#include <reportportal/launch.hpp>
#include <reportportal/rapidjson_serializer.hpp>
#include <uuid.h>

static std::chrono::high_resolution_clock::time_point from_iso_8601(const std::string& time)
{
    std::stringstream stream(time);

    // get_time parses as if it is local time. We need to fill in "is daylight savings" ourselves
    std::time_t t = std::time(nullptr);
    std::tm local_tm = *std::localtime(&t);
    stream >> std::get_time(&local_tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::high_resolution_clock::from_time_t(std::mktime(&local_tm));
}

TEST_CASE("Rapidjson deserialize ui token responce", "[rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::string ui_token_responce =
        "{"
        "\"access_token\":\"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE1NzU5MDA0NDgsInVzZXJfbmFtZSI6ImRlZmF1bHQiLCJhdXRob3JpdGllcyI6WyJST0xFX1VTRVIiXSwianRpIjoiOGQxZmUxOGUtNWY4NC00YTcwLWEwMTctNDBmZTU4ZmY3MjU3IiwiY2xpZW50X2lkIjoidWkiLCJzY29wZSI6WyJ1aSJdfQ.-5INLZnYJhNLwU5BTBuEDd0SBPoRGLBX6uX03kaEwLs\","
        "\"token_type\":\"bearer\","
        "\"refresh_token\":\"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX25hbWUiOiJkZWZhdWx0Iiwic2NvcGUiOlsidWkiXSwiYXRpIjoiOGQxZmUxOGUtNWY4NC00YTcwLWEwMTctNDBmZTU4ZmY3MjU3IiwiZXhwIjoxNTc4NDg4ODQ4LCJhdXRob3JpdGllcyI6WyJST0xFX1VTRVIiXSwianRpIjoiMGQyZDdiNTEtNGE3Mi00NjEwLTgxYmUtY2JmYjZhODhjNTgxIiwiY2xpZW50X2lkIjoidWkifQ.YOcpWlQSgF3LuskIqXgasjKvawbM_XP_I2oNJcgt9mM\","
        "\"expires_in\":3600,"
        "\"scope\":\"ui\","
        "\"jti\":\"8d1fe18e-5f84-4a70-a017-40fe58ff7257\""
        "}";

    const report_portal::ui_token_responce deserialized_responce = serializer.deserialize_ui_token_responce(ui_token_responce);

    REQUIRE(deserialized_responce.access_token() == "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE1NzU5MDA0NDgsInVzZXJfbmFtZSI6ImRlZmF1bHQiLCJhdXRob3JpdGllcyI6WyJST0xFX1VTRVIiXSwianRpIjoiOGQxZmUxOGUtNWY4NC00YTcwLWEwMTctNDBmZTU4ZmY3MjU3IiwiY2xpZW50X2lkIjoidWkiLCJzY29wZSI6WyJ1aSJdfQ.-5INLZnYJhNLwU5BTBuEDd0SBPoRGLBX6uX03kaEwLs");
}

TEST_CASE("Rapidjson deserialize api token responce", "[rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::string api_token_string = "039eda00-b397-4a6b-bab1-b1a9a90376d1";
    const uuids::uuid api_token = uuids::uuid::from_string(api_token_string);
    const std::string api_token_responce =
        "{"
        "\"access_token\":\"" + api_token_string + "\","
        "\"token_type\":\"bearer\","
        "\"scope\":\"api\""
        "}";

    const report_portal::api_token_responce deserialized_responce = serializer.deserialize_api_token_responce(api_token_responce);

    REQUIRE(deserialized_responce.access_token() == api_token);
}

TEST_CASE("Rapidjson serializer begin launch with optionals", "[!hide][rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::chrono::high_resolution_clock::time_point start_time = from_iso_8601("2020-05-09T22:30:58-0500");
    uuids::uuid nullId;
    const std::string result = serializer.serialize_begin_launch(
        "Test Launch",
        start_time,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt);

    const std::string expected_result =
        "{"
        "\"name\":\"Test Launch\","
        "\"startTime\":\"2020-05-09T22:30:58.000000-0500\""
        "}";

    REQUIRE(result == expected_result);
}

TEST_CASE("Rapidjson deserialize begin launch responce", "[rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::string id_string = "039eda00-b397-4a6b-bab1-b1a9a90376d1";
    const uuids::uuid launch_id = uuids::uuid::from_string(id_string);

    SECTION("With only \"id\" in responce") {
        const std::string begin_launch_responce =
            "{"
            "\"id\":\"" + id_string + "\""
            "}";

        const report_portal::begin_launch_responce deserialized_responce = serializer.deserialize_begin_launch_responce(begin_launch_responce);

        REQUIRE(deserialized_responce.id() == launch_id);
        REQUIRE(deserialized_responce.number().has_value() == false);
    }

    SECTION("with optional number in repsonce") {
        const uint64_t number = 12345;
        const std::string begin_launch_responce =
            "{"
            "\"id\":\"" + id_string + "\","
            "\"number\":" + std::to_string(number) + ""
            "}";

        const report_portal::begin_launch_responce deserialized_responce = serializer.deserialize_begin_launch_responce(begin_launch_responce);

        REQUIRE(deserialized_responce.id() == launch_id);

        const std::optional<uint64_t> responce_number = deserialized_responce.number();
        REQUIRE(responce_number.has_value());
        REQUIRE(*responce_number == number);
    }
}

TEST_CASE("Rapidjson serializer end launch with optionals", "[!hide][rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::chrono::high_resolution_clock::time_point end_time = from_iso_8601("2020-05-09T22:30:58-0500");
    const std::string result = serializer.serialize_end_launch(
        end_time);

    const std::string expected_result =
        "{"
        "\"endTime\":\"2020-05-09T22:30:58.000000-0500\""
        "}";

    REQUIRE(result == expected_result);
}

TEST_CASE("Rapidjson deserialize end launch responce", "[rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::string id_string = "039eda00-b397-4a6b-bab1-b1a9a90376d1";
    const uuids::uuid launch_id = uuids::uuid::from_string(id_string);

    SECTION("With only \"id\" in responce") {
        const std::string end_launch_responce =
            "{"
            "\"id\":\"" + id_string + "\""
            "}";

        const report_portal::end_launch_responce deserialized_responce = serializer.deserialize_end_launch_responce(end_launch_responce);

        REQUIRE(deserialized_responce.id() == launch_id);
        REQUIRE(deserialized_responce.number().has_value() == false);
        REQUIRE(deserialized_responce.link().has_value() == false);
    }

    SECTION("with optional attributes in responce") {
        const uint64_t number = 12345;
        const std::string link = "https://www.rp.com/etc";
        const std::string end_launch_responce =
            "{"
            "\"id\":\"" + id_string + "\","
            "\"number\":" + std::to_string(number) + ","
            "\"link\":\"" + link + "\""
            "}";

        const report_portal::end_launch_responce deserialized_responce = serializer.deserialize_end_launch_responce(end_launch_responce);

        REQUIRE(deserialized_responce.id() == launch_id);

        const std::optional<uint64_t> responce_number = deserialized_responce.number();
        REQUIRE(responce_number.has_value());
        REQUIRE(*responce_number == number);

        const std::optional<std::string> responce_link = deserialized_responce.link();
        REQUIRE(responce_link.has_value());
        REQUIRE(*responce_link == link);
    }
}

TEST_CASE("Rapidjson serializer begin test item with optionals", "[!hide][rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::chrono::high_resolution_clock::time_point start_time = from_iso_8601("2020-05-09T22:30:58-0500");
    uuids::uuid launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");

    const std::string result = serializer.serialize_begin_test_item(
        "Test Test Item",
        start_time,
        report_portal::test_item_type::suite,
        launch_id,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt);

    const std::string expected_result =
        "{"
        "\"name\":\"Test Test Item\","
        "\"startTime\":\"2020-05-09T22:30:58.000000-0500\","
        "\"type\":\"suite\","
        "\"launchUuid\":\"47183823-2574-4bfd-b411-99ed177d3e43\""
        "}";

    REQUIRE(result == expected_result);
}

TEST_CASE("Rapidjson deserialize begin test item responce", "[rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::string id_string = "039eda00-b397-4a6b-bab1-b1a9a90376d1";
    const uuids::uuid test_item_id = uuids::uuid::from_string(id_string);

    SECTION("With only \"id\" in responce") {
        const std::string begin_test_item_responce =
            "{"
            "\"id\":\"" + id_string + "\""
            "}";

        const report_portal::begin_test_item_responce deserialized_responce = serializer.deserialize_begin_test_item_responce(begin_test_item_responce);

        REQUIRE(deserialized_responce.id() == test_item_id);
    }
}

TEST_CASE("Rapidjson serializer end test item with optionals", "[!hide][rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::chrono::high_resolution_clock::time_point end_time = from_iso_8601("2020-05-09T22:30:58-0500");
    const uuids::uuid launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");

    const std::string result = serializer.serialize_end_test_item(
        end_time,
        launch_id,
        std::nullopt,
        std::nullopt,
        std::nullopt);

    const std::string expected_result =
        "{"
        "\"endTime\":\"2020-05-09T22:30:58.000000-0500\","
        "\"launchUuid\":\"47183823-2574-4bfd-b411-99ed177d3e43\""
        "}";

    REQUIRE(result == expected_result);
}

TEST_CASE("Rapidjson deserialize end test item responce", "[rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const std::string message = "TestItem with ID = '47183823-2574-4bfd-b411-99ed177d3e43' successfully finished.";
    SECTION("With only \"message\" in responce") {
        const std::string end_test_item_responce =
            "{"
            "\"message\":\"" + message + "\""
            "}";

        const report_portal::end_test_item_responce deserialized_responce = serializer.deserialize_end_test_item_responce(end_test_item_responce);

        REQUIRE(deserialized_responce.message() == message);
    }
}

TEST_CASE("Rapidjson serializer batch of logs", "[!hide][rapidjson][serializer]")
{
    report_portal::rapidjson_serializer serializer;

    const uuids::uuid launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");
    const uuids::uuid test_item_id = uuids::uuid::from_string("57183823-2574-4bfd-b411-99ed177d3e43");
    const std::chrono::high_resolution_clock::time_point time = from_iso_8601("2020-05-09T22:30:58-0500");

    const std::vector<report_portal::log> logs = {
        report_portal::log(launch_id, test_item_id, time, report_portal::log_level::trace, "Trace message"),
        report_portal::log(launch_id, test_item_id, time, report_portal::log_level::warn, "Warning message"),
        report_portal::log(launch_id, test_item_id, time, report_portal::log_level::error, "Error message"),
        report_portal::log(launch_id, test_item_id, time, report_portal::log_level::fatal, "Fatal message"),
    };

    const std::string result = serializer.serialize_batched_logs(logs);
    const std::string expected_result =
        "["
        "{"
        "\"launchUuid\":\"47183823-2574-4bfd-b411-99ed177d3e43\","
        "\"time\":\"2020-05-09T22:30:58.000000-0500\","
        "\"itemUuid\":\"57183823-2574-4bfd-b411-99ed177d3e43\","
        "\"message\":\"Trace message\","
        "\"level\":\"trace\""
        "},"
        "{"
        "\"launchUuid\":\"47183823-2574-4bfd-b411-99ed177d3e43\","
        "\"time\":\"2020-05-09T22:30:58.000000-0500\","
        "\"itemUuid\":\"57183823-2574-4bfd-b411-99ed177d3e43\","
        "\"message\":\"Warning message\","
        "\"level\":\"warn\""
        "},"
        "{"
        "\"launchUuid\":\"47183823-2574-4bfd-b411-99ed177d3e43\","
        "\"time\":\"2020-05-09T22:30:58.000000-0500\","
        "\"itemUuid\":\"57183823-2574-4bfd-b411-99ed177d3e43\","
        "\"message\":\"Error message\","
        "\"level\":\"error\""
        "},"
        "{"
        "\"launchUuid\":\"47183823-2574-4bfd-b411-99ed177d3e43\","
        "\"time\":\"2020-05-09T22:30:58.000000-0500\","
        "\"itemUuid\":\"57183823-2574-4bfd-b411-99ed177d3e43\","
        "\"message\":\"Fatal message\","
        "\"level\":\"fatal\""
        "}"
        "]";

    REQUIRE(result == expected_result);
}
