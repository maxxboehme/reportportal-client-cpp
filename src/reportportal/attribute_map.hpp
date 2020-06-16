#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace report_portal
{

class attribute_map final
{
    public:
        attribute_map() = default;

        attribute_map(const std::multimap<std::string, std::string> attributes, const std::vector<std::string>& tags)
          : _attributes(attributes),
            _tags(tags)
        {}

        bool operator==(const attribute_map& other) const {
            return _attributes == other._attributes && _tags == other._tags;
        }

        // TODO: return value previously owned by that key?
        void add_attribute(const std::string& key, const std::string& value) {
            _attributes.insert(std::map<std::string, std::string>::value_type(key, value));
        }

        void remove_attribute(const std::string& key) {
            _attributes.erase(key);
        }

        void add_tag(const std::string& tag) {
            _tags.push_back(tag);
        }

        void remove_tag(const std::string& tag) {
            std::remove(std::begin(_tags), std::end(_tags), tag);
        }

        bool empty() {
            return _attributes.empty() && _tags.empty();
        }

    private:
        std::multimap<std::string, std::string> _attributes;
        std::vector<std::string> _tags;
};

}
