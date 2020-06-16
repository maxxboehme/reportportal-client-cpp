#pragma once

#include <reportportal/iservice.hpp>
#include <string>
#include <uuid.h>

namespace report_portal
{

class itest_item_parent
{
    public:
        /**
         *
         * @returns
         */
        virtual uuids::uuid id() const = 0;

        /**
         *
         * @returns
         */
        virtual uuids::uuid launch_id() const = 0;

        /**
         *
         * @returns
         */
        virtual const iservice& service() const = 0;

        /**
         *
         * @returns
         */
        virtual iservice& service() = 0;

        virtual bool has_started() const = 0;
        virtual bool has_ended() const = 0;

        virtual ~itest_item_parent() {}
};

}
