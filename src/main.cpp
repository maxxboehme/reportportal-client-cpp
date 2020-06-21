#include <service.h>
#include <launch.h>
#include <test_item.h>
#include <iostream>
#include <chrono>

int main(int argc, const char *argv[])
{
    report_portal::service service("http://web.demo.reportportal.io", "DEFAULT_PERSONAL", "default", "1q2w3e");

    report_portal::launch launch(service, "Test Launch");
    launch.set_description("This is a test launch");

//    report_portal::attribute_map attributes(
//        {
//            "build": "1.2.3",
//            "os": "ubuntu",
//            "os version": "18.04"
//        },
//        [
//            "C++"
//        ]);
//    launch.set_attributes(attributs);

    launch.start(std::chrono::system_clock::now());
    {
        report_portal::test_item suite(launch, "Test Suite");
        suite.set_description("This is a test suite.");

        suite.start(std::chrono::system_clock::now());
        {
            report_portal::test_item test(suite, "Test 1", report_portal::test_item_type::test);
            test.set_description("This is a test test.");

            test.start(std::chrono::system_clock::now());
            {
                report_portal::test_item step(test, "Step 1", report_portal::test_item_type::step);

                step.start(std::chrono::system_clock::now());

                step.log(std::chrono::system_clock::now(), report_portal::log_level::info, "REQUIRE(result == actual)");
                step.log(std::chrono::system_clock::now(), report_portal::log_level::info, "this is another log");
                step.log(std::chrono::system_clock::now(), report_portal::log_level::info, "and another");

                step.end(std::chrono::system_clock::now(), report_portal::test_item_status::passed);
            }

            test.end(std::chrono::system_clock::now());
        }
//        {
//            report_portal::test_item test(suite, "Test 2", report_portal::test_item_type::test);
//            test.set_description("This is a test test.");
//
//            test.start(std::chrono::system_clock::now());
//            {
//                report_portal::test_item scenario(test, "Scenario 1", report_portal::test_item_type::scenario);
//
//                scenario.start(std::chrono::system_clock::now());
//                scenario.end(std::chrono::system_clock::now(), report_portal::test_item_status::passed);
//            }
//
//            test.end(std::chrono::system_clock::now());
//        }
//        {
//            report_portal::test_item test(suite, "Test 3", report_portal::test_item_type::test);
//            test.set_description("This is a test test.");
//
//            test.start(std::chrono::system_clock::now());
//            test.end(std::chrono::system_clock::now(), report_portal::test_item_status::stopped);
//        }
//        {
//            report_portal::test_item test(suite, "Test 4", report_portal::test_item_type::test);
//            test.set_description("This is a test test.");
//
//            test.start(std::chrono::system_clock::now());
//            test.end(std::chrono::system_clock::now(), report_portal::test_item_status::skipped);
//        }
//        {
//            report_portal::test_item test(suite, "Test 5", report_portal::test_item_type::test);
//            test.set_description("This is a test test.");
//
//            test.start(std::chrono::system_clock::now());
//            test.end(std::chrono::system_clock::now(), report_portal::test_item_status::interrupted);
//        }
//        {
//            report_portal::test_item test(suite, "Test 6", report_portal::test_item_type::test);
//            test.set_description("This is a test test.");
//
//            test.start(std::chrono::system_clock::now());
//            test.end(std::chrono::system_clock::now(), report_portal::test_item_status::cancelled);
//        }
        suite.end(std::chrono::system_clock::now());

    }
    launch.end(std::chrono::system_clock::now());

    return 0;
}
