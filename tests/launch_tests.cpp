#include <utils.h>

#include <catch2/catch.hpp>
#include <fakeit.hpp>
#include <reportportal/launch.hpp>

using namespace fakeit;

TEST_CASE("Launch simple construction", "[launch]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(
        service_mock.get(),
        "Test Launch");

    REQUIRE(launch.id().is_nil());
    REQUIRE(launch.description() == "");
    REQUIRE(&launch.service() == &service_mock.get());
}

TEST_CASE("Launch start", "[launch]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(
        service_mock.get(),
        "Test Launch");

    uuids::uuid generated_launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");
    When(Method(service_mock, begin_launch)
        .Using(
            _,
            _,
            _,
            uuids::uuid(),
            _,
            _,
            _,
            _))
        .Return(report_portal::begin_launch_responce(generated_launch_id));

    const std::chrono::system_clock::time_point start_time = from_iso_8601("2020-05-09T22:30:58-0500");
    launch.start(start_time);

    REQUIRE(launch.id() == generated_launch_id);

    SECTION("try to start again") {
        REQUIRE_THROWS_AS(launch.start(start_time), std::runtime_error);
    }

    Verify(Method(service_mock, begin_launch)
        .Using(
            "Test Launch",
            start_time,
            "",
            uuids::uuid(),
            report_portal::attribute_map(),
            report_portal::launch_mode::def,
            false,
            uuids::uuid()))
        .Exactly(1);
}

TEST_CASE("Launch is rerun", "[launch]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(
        service_mock.get(),
        "Test Launch");

    SECTION("default values after construction") {
        REQUIRE(launch.is_rerun() == false);
        REQUIRE(launch.rerunof().is_nil());
    }

    SECTION("after setting rerunof") {
        uuids::uuid rerun_launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");
        launch.set_rerunof(rerun_launch_id);

        REQUIRE(launch.is_rerun());
        REQUIRE(launch.rerunof() == rerun_launch_id);

        SECTION("then setting rerunof back to nil") {
            launch.set_rerunof(uuids::uuid());

            REQUIRE(launch.is_rerun() == false);
            REQUIRE(launch.rerunof().is_nil());
        }
    }
}

TEST_CASE("Launch end before starting", "[launch]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(
        service_mock.get(),
        "Test Launch");

    const std::chrono::system_clock::time_point end_time = from_iso_8601("2020-05-09T22:30:58-0500");
    REQUIRE_THROWS_AS(launch.end(end_time), std::runtime_error);
}

TEST_CASE("Launch end after starting", "[launch]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(
        service_mock.get(),
        "Test Launch");

    uuids::uuid generated_launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");
    When(Method(service_mock, begin_launch)
        .Using(
            _,
            _,
            _,
            uuids::uuid(),
            _,
            _,
            _,
            _))
        .Return(report_portal::begin_launch_responce(generated_launch_id));
    const std::chrono::system_clock::time_point start_time = from_iso_8601("2020-05-09T22:20:58-0500");
    launch.start(start_time);

    When(Method(service_mock, end_launch)
        .Using(
            _,
            _))
        .Return(report_portal::end_launch_responce(generated_launch_id));
    const std::chrono::system_clock::time_point end_time = from_iso_8601("2020-05-09T22:30:58-0500");
    REQUIRE_NOTHROW(launch.end(end_time));

    SECTION("try to end again") {
        REQUIRE_THROWS_AS(launch.end(end_time), std::runtime_error);
    }

    Verify(Method(service_mock, end_launch)
        .Using(
            generated_launch_id,
            end_time))
        .Exactly(1);
}
