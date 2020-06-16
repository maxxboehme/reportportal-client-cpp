#pragma once

#include <reportportal/external_system_issue.hpp>
#include <string>
#include <uuid.h>

namespace report_portal
{

class issue
{
    public:
    enum class type : uint32_t {
        no_defect = 0,
        to_investigate = 1,
        product = 2,
        automation = 3,
        system = 4
    };

    issue()
      : _type(type::to_investigate),
        _type_locator_id(0),
        _comment(),
        _is_auto_analyzed(false),
        _ignore_analyzer(false),
        _external_system_issues()
    {}

    /**
     * @param[in] type
     */
    issue(type type)
      : _type(type),
        _type_locator_id(0),
        _comment(),
        _is_auto_analyzed(false),
        _ignore_analyzer(false),
        _external_system_issues()
    {}

    /**
     * @param[in] type
     * @param[in] type_locator_id
     */
    issue(type type, uint32_t type_locator_id)
      : _type(type),
        _type_locator_id(0),
        _comment(),
        _is_auto_analyzed(false),
        _ignore_analyzer(false),
        _external_system_issues()
    {}

    bool operator==(const issue& other) const {
        return _type == other._type &&
            _type_locator_id == other._type_locator_id &&
            _comment == other._comment &&
            _is_auto_analyzed == other._is_auto_analyzed &&
            _ignore_analyzer == other._ignore_analyzer &&
            _external_system_issues == other._external_system_issues;
    }

    type get_type() const {
        return _type;
    }

    uint32_t get_type_locator_id() const {
        return _type_locator_id;
    }

    void set_comment(const std::string& comment) {
        _comment = comment;
    }

    std::string get_comment() const {
        return _comment;
    }

    bool is_auto_analyzed() const {
        return _is_auto_analyzed;
    }

    void set_auto_analyzed(bool is_auto_analzyed) {
        _is_auto_analyzed = is_auto_analzyed;
    }

    bool ignore_analyzer() const {
        return _ignore_analyzer;
    }

    void set_ignore_analyzer(bool ignore_analyzer) {
        _ignore_analyzer = ignore_analyzer;
    }

    std::vector<external_system_issue> external_system_issues() const {
        return _external_system_issues;
    }

    void set_external_system_issues(const std::vector<external_system_issue>& external_system_issues) {
        _external_system_issues = external_system_issues;
    }

    void append(external_system_issue external_system_issue) {
        _external_system_issues.push_back(external_system_issue);
    }

    private:
        type _type;
        uint32_t _type_locator_id;

        std::string _comment;
        bool _is_auto_analyzed;
        bool _ignore_analyzer;
        std::vector<external_system_issue> _external_system_issues;
};

}
