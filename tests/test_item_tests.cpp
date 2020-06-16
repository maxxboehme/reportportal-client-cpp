#include <utils.h>

#include <catch2/catch.hpp>
#include <fakeit.hpp>
#include <reportportal/launch.hpp>
#include <reportportal/test_item.hpp>

using namespace fakeit;

TEST_CASE("Test_item simple construction", "[test_item]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(
        service_mock.get(),
        "Test Launch");

    report_portal::test_item suite(launch, "Test Suite");

    REQUIRE(suite.id().is_nil());
    REQUIRE(&suite.service() == &service_mock.get());
    // [TODO] REQUIRE(&suite.parent() == &launch);
    REQUIRE(suite.launch_id() == launch.id());
    REQUIRE(suite.name() == "Test Suite");
    REQUIRE(suite.type() == report_portal::test_item_type::suite);
    REQUIRE(suite.description() == "");
    REQUIRE(suite.coderef() == "");
    REQUIRE(suite.parameters().empty());
    REQUIRE(suite.is_retry() == false);
    REQUIRE(suite.has_stats());
}

// [TODO]
// TEST_CASE("Test_item start", "[test_item]")
// {
//     Mock<report_portal::iservice> service_mock;
//     report_portal::launch launch(service_mock.get(), "Test Launch");
// 
//     report_portal::test_item suite(launch, "Test Suite");
// 
//     SECTION("try to start before parent") {
//         const std::chrono::high_resolution_clock::time_point start_time = from_iso_8601("2020-05-09T22:30:58-0500");
//         REQUIRE_THROWS_AS(suite.start(start_time), std::runtime_error);
//     }
// 
//     SECTION("start after parent") {
//        uuids::uuid generated_launch_id = uuids::uuid::from_string("47183823-2574-4bfd-b411-99ed177d3e43");
//        When(Method(service_mock, begin_launch)
//            .Using(
//                _,
//                _,
//                _,
//                uuids::uuid(),
//                _,
//                _,
//                _,
//                _))
//            .Return(report_portal::begin_launch_responce(generated_launch_id));
// 
//        const std::chrono::high_resolution_clock::time_point launch_start_time = from_iso_8601("2020-05-09T22:30:58-0500");
//        launch.start(launch_start_time);
// 
//        CHECK(launch.id() == generated_launch_id);
// 
//        uuids::uuid generated_test_item_id = uuids::uuid::from_string("47183823-2574-4bfd-c411-99ed177d3e44");
//        When(Method(service_mock, begin_test_item)
//            .Using(
//                _,
//                _,
//                _,
//                _,
//                _,
//                _,
//                _,
//                _,
//                _,
//                _,
//                _,
//                _))
//            .Return(report_portal::begin_test_item_responce(generated_test_item_id));
// 
//        const std::chrono::high_resolution_clock::time_point suite_start_time = from_iso_8601("2020-05-09T22:35:58-0500");
//        suite.start(suite_start_time);
// 
//        REQUIRE(suite.id() == generated_test_item_id);
// 
//        SECTION("try to start again") {
//            REQUIRE_THROWS_AS(suite.start(suite_start_time), std::runtime_error);
//        }
// 
//        Verify(Method(service_mock, begin_launch)
//            .Using(
//                "Test Launch",
//                launch_start_time,
//                "",
//                uuids::uuid(),
//                report_portal::attribute_map(),
//                report_portal::launch_mode::def,
//                false,
//                uuids::uuid()))
//            .Exactly(1);
// 
//           Verify(Method(service_mock, begin_test_item)
//               .Using(
//                   "Test Suite",
//                   suite_start_time,
//                   report_portal::test_item_type::suite,
//                   _, // FakeIt limitation: fakeit is not good at storing arguments which are references.
//                   generated_launch_id,
//                   "",
//                   report_portal::attribute_map(),
//                   uuids::uuid(),
//                   "",
//                   std::map<std::string, std::string>(),
//                   false,
//                   true))
//               .Exactly(1);
//     }
// }

TEST_CASE("Test_item end", "[test_item]")
{
    Mock<report_portal::iservice> service_mock;
    report_portal::launch launch(service_mock.get(), "Test Launch");

    report_portal::test_item suite(launch, "Test Suite");

    SECTION("try to end before start") {
        const std::chrono::high_resolution_clock::time_point end_time = from_iso_8601("2020-05-09T22:30:58-0500");
        REQUIRE_THROWS_AS(suite.end(end_time), std::runtime_error);
    }

    SECTION("end after start parent") {
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

        const std::chrono::high_resolution_clock::time_point launch_start_time = from_iso_8601("2020-05-09T22:30:58-0500");
        launch.start(launch_start_time);

        CHECK(launch.id() == generated_launch_id);

        uuids::uuid generated_test_item_id = uuids::uuid::from_string("47183823-2574-4bfd-c411-99ed177d3e44");
        When(Method(service_mock, begin_test_item)
            .Using(
                _,
                _,
                _,
                _,
                _,
                _,
                _,
                _,
                _,
                _,
                _,
                _))
            .Return(report_portal::begin_test_item_responce(generated_test_item_id));

        const std::chrono::high_resolution_clock::time_point suite_start_time = from_iso_8601("2020-05-09T22:35:58-0500");
        suite.start(suite_start_time);

        CHECK(suite.id() == generated_test_item_id);

        const std::chrono::high_resolution_clock::time_point suite_end_time = from_iso_8601("2020-05-09T23:35:58-0500");
        const std::string responce_message = "TestItem with ID = '47183823-2574-4bfd-b411-99ed177d3e43' successfully finished.";
        When(Method(service_mock, end_test_item)
            .Using(
                _,
                _,
                _,
                _,
                _))
            .Return(report_portal::end_test_item_responce(responce_message));
        suite.end(suite_end_time);

        Verify(Method(service_mock, end_test_item)
            .Using(
                generated_test_item_id,
                _,
                suite_end_time,
                report_portal::test_item_status::inherit,
                std::nullopt))
            .Exactly(1);
    }
}
