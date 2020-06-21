#pragma once

#include <reportportal/attribute_map.hpp>
#include <reportportal/iservice.hpp>
#include <reportportal/itest_item_parent.hpp>
#include <ctime>
#include <string>
#include <map>
#include <uuid.h>
#include <stdexcept>
#include <chrono>

namespace report_portal
{

class launch final : public itest_item_parent
{
    public:
        /**
         *
         * @param[in] service
         * @param[in] name
         * @param[in] mode
         */
        launch(iservice& service, const std::string& name, launch_mode mode = launch_mode::def);

        /**
         *
         * @param[in] service
         * @param[in] name
         * @param[in] uuid
         * @param[in] launch_mode
         */
        launch(iservice& service, const std::string& name, const uuids::uuid& uuid, launch_mode mode = launch_mode::def);

        std::string name() const;

        std::string description() const;

        void set_description(const std::string& description);

        uuids::uuid id() const override;

        uuids::uuid launch_id() const override;

        const iservice& service() const override;

        iservice& service() override;

        attribute_map attributes() const;

        void set_attributes(const attribute_map& attributes);

        void add_attribute(const std::string& key, const std::string& value);

        void remove_attribute(const std::string& key);

        /**
         * @param tag
         */
        void add_tag(const std::string& tag);

        /**
         * @param tag
         */
        void remove_tag(const std::string& tag);

        /**
         * @return
         */
        launch_mode mode() const;

        /**
         * @return
         */
        bool is_rerun() const;

        /**
         * @return
         */
        uuids::uuid rerunof() const;

        /**
         * @param[in] rerunof The ID of the launch this is a rerun of.
         *                    Set to nil id of wanting to specify not a rerun.
         */
        void set_rerunof(const uuids::uuid& rerunof);

        bool has_started() const;

        bool has_ended() const;

        void start(const std::chrono::system_clock::time_point& start_time);

        void end(const std::chrono::system_clock::time_point& end_time);

    private:
        iservice& _service;

        std::string _name;
        std::string _description;
        uuids::uuid _uuid;
        attribute_map _attributes;
        launch_mode _mode;
        bool _is_rerun;
        uuids::uuid _rerunof;

        uint64_t _number;

        bool _has_started;
        bool _has_ended;
};

}
