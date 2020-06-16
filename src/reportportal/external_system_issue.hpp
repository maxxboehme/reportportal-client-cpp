#pragma once

#include <ctime>
#include <string>

namespace report_portal
{

class external_system_issue
{
    public:
        enum class type : uint32_t {
            product = 1,
            automation = 2,
            system = 3
        };

    external_system_issue() = default;

    external_system_issue(const std::string& ticket_id)
      : _ticket_id(ticket_id),
        _submit_date(),
        _external_system_home_url(),
        _project(),
        _url()
    {}

    bool operator==(const external_system_issue& other) const {
        return _ticket_id == other._ticket_id &&
            _submit_date == other._submit_date &&
            _external_system_home_url == other._external_system_home_url &&
            _project == other._project &&
            _url == other._url;
    }

    private:
        std::string _ticket_id;
        std::time_t _submit_date;
        std::string _external_system_home_url;
        std::string _project;
        std::string _url;
};

}
