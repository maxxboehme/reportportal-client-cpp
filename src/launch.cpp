#include <reportportal/launch.hpp>
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

launch::launch(iservice& service, const std::string& name, launch_mode mode)
  : _service(service),
    _name(name),
    _description(),
    _uuid(),
    _attributes(),
    _mode(mode),
    _is_rerun(false),
    _rerunof(),
    _number(0),
    _has_started(false),
    _has_ended(false)
{}

launch::launch(iservice& service, const std::string& name, const uuids::uuid& uuid, launch_mode mode)
  : _service(service),
    _name(name),
    _description(),
    _uuid(uuid),
    _attributes(),
    _mode(launch_mode::def),
    _is_rerun(false),
    _rerunof(),
    _number(0),
    _has_started(false),
    _has_ended(false)
{}

std::string launch::name() const {
    return _name;
}

std::string launch::description() const {
    return _description;
}

void launch::set_description(const std::string& description) {
    _description = description;
}

uuids::uuid launch::id() const {
    return _uuid;
}

uuids::uuid launch::launch_id() const {
    return _uuid;
}

const iservice& launch::service() const {
    return _service;
}

iservice& launch::service() {
    return _service;
}

attribute_map launch::attributes() const {
    return _attributes;
}

void launch::set_attributes(const attribute_map& attributes) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    }

    _attributes = attributes;
}

void launch::add_attribute(const std::string& key, const std::string& value) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    }

    _attributes.add_attribute(key, value);
}

void launch::remove_attribute(const std::string& key) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    }

    _attributes.remove_attribute(key);
}

void launch::add_tag(const std::string& tag) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    }

    _attributes.add_tag(tag);
}

void launch::remove_tag(const std::string& tag) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    }

    _attributes.remove_tag(tag);
}

launch_mode launch::mode() const {
    return _mode;
}

bool launch::is_rerun() const {
    return _is_rerun;
}

uuids::uuid launch::rerunof() const {
    return _rerunof;
}

void launch::set_rerunof(const uuids::uuid& rerunof) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    }

    if (rerunof.is_nil()) {
        _is_rerun = false;
    } else {
        _is_rerun = true;
    }

    _rerunof = rerunof;
}

bool launch::has_started() const {
    return _has_started;
}

bool launch::has_ended() const {
    return _has_ended;
}

void launch::start(const std::chrono::system_clock::time_point& start_time) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    } else if (_has_started) {
        throw std::runtime_error("Launch has already started");
    }

    const begin_launch_responce responce = _service.begin_launch(
        _name,
        start_time,
        _description,
        _uuid,
        _attributes,
        _mode,
        _is_rerun,
        _rerunof);

    // begin_launch didn't throw an exception so successful
    // begin was sent.
    _has_started = true;

    // [TODO](maxxboehme)
    _uuid = responce.id();
    // _number = responce.number();
}

void launch::end(const std::chrono::system_clock::time_point& end_time) {
    if (_has_ended) {
        throw std::runtime_error("Launch has already ended");
    } else if (!_has_started) {
        throw std::runtime_error("Launch has not been started yet before end");
    }

    const end_launch_responce responce = _service.end_launch(
        _uuid,
        end_time);

    _has_ended = true;
}

}
