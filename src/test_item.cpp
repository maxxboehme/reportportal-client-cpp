#include <reportportal/test_item.hpp>
#include <reportportal/iservice.hpp>
#include <reportportal/launch.hpp>
#include <reportportal/log.hpp>
#include <reportportal/issue.hpp>
#include <reportportal/itest_item_parent.hpp>
#include <ctime>
#include <string>
#include <map>
#include <uuid.h>
#include <chrono>
#include <vector>

namespace report_portal
{

test_item::test_item(
   launch& parent,
   const std::string name)
  : _parent(parent),
    _name(name),
    _type(test_item_type::suite),
    _description(),
    _attributes(),
    _coderef(),
    _parameters(),
    _id(),
    _is_retry(),
    _has_stats(true),
    _status(test_item_status::inherit),
    _batched_logs(),
    _has_started(false),
    _has_ended(false)
{}

test_item::test_item(
   launch& parent,
   const uuids::uuid& id,
   const std::string name)
  : _parent(parent),
    _name(name),
    _type(test_item_type::suite),
    _description(),
    _attributes(),
    _coderef(),
    _parameters(),
    _id(id),
    _is_retry(),
    _has_stats(true),
    _status(test_item_status::inherit),
    _batched_logs(),
    _has_started(false),
    _has_ended(false)
{}


test_item::test_item(
   test_item& parent,
   const std::string name,
   test_item_type type)
  : _parent(parent),
    _name(name),
    _type(type),
    _description(),
    _attributes(),
    _coderef(),
    _parameters(),
    _id(),
    _is_retry(),
    _has_stats(true),
    _status(test_item_status::inherit),
    _batched_logs(),
    _has_started(false),
    _has_ended(false)
{}

test_item::test_item(
   test_item& parent,
   const uuids::uuid& id,
   const std::string name,
   test_item_type type)
  : _parent(parent),
    _name(name),
    _type(type),
    _description(),
    _attributes(),
    _coderef(),
    _parameters(),
    _id(id),
    _is_retry(),
    _has_stats(true),
    _status(test_item_status::inherit),
    _batched_logs(),
    _has_started(false),
    _has_ended(false)
{}

uuids::uuid test_item::id() const {
    return _id;
}

uuids::uuid test_item::launch_id() const {
    return _parent.launch_id();
}

const iservice& test_item::service() const {
    return _parent.service();
}

iservice& test_item::service() {
    return _parent.service();
}

std::string test_item::name() const {
    return _name;
}

test_item_type test_item::type() const {
    return _type;
}

std::string test_item::description() const {
    return _description;
}

void test_item::set_description(const std::string& description) {
    _description = description;
}

attribute_map test_item::attributes() const {
    return _attributes;
}

void test_item::set_attributes(const attribute_map& attributes) {
    _attributes = attributes;
}

void test_item::add_attribute(const std::string& key, const std::string& value) {
    _attributes.add_attribute(key, value);
}

void test_item::remove_attribute(const std::string& key) {
    _attributes.remove_attribute(key);
}

void test_item::add_tag(const std::string& tag) {
    _attributes.add_tag(tag);
}

void test_item::remove_tag(const std::string& tag) {
    _attributes.remove_tag(tag);
}

std::string test_item::coderef() const {
    return _coderef;
}

void test_item::set_coderef(const std::string& coderef) {
    _coderef = coderef;
}

const std::map<std::string, std::string>& test_item::parameters() const {
    return _parameters;
}

void test_item::set_parameters(const std::map<std::string, std::string>& parameters) {
    _parameters = parameters;
}

void test_item::add_parameter(const std::string& parameter, const std::string value) {
    _parameters.insert(std::map<std::string, std::string>::value_type(parameter, value));
}

void test_item::remove_parameter(const std::string& parameter) {
    _parameters.erase(parameter);
}

bool test_item::is_retry() const {
    return _is_retry;
}

void test_item::set_retry(bool is_retry) {
    _is_retry = is_retry;
}

bool test_item::has_stats() const {
    return _has_stats;
}

void test_item::set_has_stats(bool has_stats) {
    _has_stats = has_stats;
}

bool test_item::has_started() const {
    return _has_started;
}

bool test_item::has_ended() const {
    return _has_ended;
}

void test_item::start(const std::chrono::high_resolution_clock::time_point& start_time) {
    if (!_parent.has_started()) {
        throw std::runtime_error("parent has not started yet");
    } else if (_parent.has_ended()) {
        throw std::runtime_error("parent has already ended");
    } if (_has_ended) {
        throw std::runtime_error("test_item has already ended");
    } else if (_has_started) {
        throw std::runtime_error("test_item has already been started");
    }

    uuids::uuid parent_id;
    if (_parent.id() != _parent.launch_id()) {
        parent_id = _parent.id();
    }

    iservice& service = _parent.service();
    const begin_test_item_responce responce = service.begin_test_item(
        _name,
        start_time,
        _type,
        _parent.launch_id(),
        parent_id,
        _description,
        _attributes,
        _id,
        _coderef,
        _parameters,
        _is_retry,
        _has_stats);

    // begin_test_item did not throw an exception so successful
    // begin was sent.
    _has_started = true;
    _id = responce.id();
}

void test_item::set_status(test_item_status status) {
    _status = status;
}

void test_item::end(const std::chrono::high_resolution_clock::time_point& end_time) {
    if (_has_ended) {
        throw std::runtime_error("test_item has already ended");
    } else if (!_has_started) {
        throw std::runtime_error("test_item has not been started yet before end");
    }

    iservice& service = _parent.service();
    const end_test_item_responce responce = service.end_test_item(
        _id,
        _parent.id(),
        end_time,
        _status,
        std::nullopt);
}

void test_item::end(const std::chrono::high_resolution_clock::time_point& end_time, test_item_status status) {
    if (_has_ended) {
        throw std::runtime_error("test_item has already ended");
    } else if (!_has_started) {
        throw std::runtime_error("test_item has not been started yet before end");
    }

    // Flush the logs before sending ending message.
    flush_logs();

    _status = status;
    iservice& service = _parent.service();
    const end_test_item_responce responce = service.end_test_item(
        _parent.id(),
        _id,
        end_time,
        _status,
        std::nullopt);
}

void test_item::log(const std::chrono::high_resolution_clock::time_point& time, log_level level, const std::string& message) {
    if (!_has_started) {
        throw std::runtime_error("test_item has not been started yet before end");
    }

    _batched_logs.emplace_back(_parent.launch_id(), _id, time, level, message);
}

void test_item::log(const std::chrono::high_resolution_clock::time_point& time, log_level level, const std::string& message, const attachment& attachment) {
    if (!_has_started) {
        throw std::runtime_error("test_item has not been started yet before end");
    }

    report_portal::log tmp(_parent.launch_id(), _id, time, level, message);
    tmp.set_attachment(attachment);
    _batched_logs.push_back(tmp);
}

void test_item::flush_logs() {
    if (!_batched_logs.empty()) {
        iservice& service = _parent.service();
        service.batch_logs(_batched_logs);
    }
}
}
