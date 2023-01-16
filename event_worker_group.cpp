#include "event_worker_group.hpp"

#include "event_channel.hpp"
#include "event_worker.hpp"
#include "serv_types.hpp"

#include <smart_ptr/shared_ptr.hpp>
#include <thread/lock_guard.hpp>
#include <thread/mutex.hpp>

ft::serv::event_worker_group::event_worker_group()
    : lock(), index(), loops() {}

ft::serv::event_worker_group::~event_worker_group() {}

void ft::serv::event_worker_group::put_worker(const ft::shared_ptr<event_worker>& worker)
{
    const ft::lock_guard<ft::mutex> lock(this->lock);
    this->loops.push_back(worker);
}

const ft::shared_ptr<ft::serv::event_worker>& ft::serv::event_worker_group::next()
{
    const ft::lock_guard<ft::mutex> lock(this->lock);
    if (this->index >= this->loops.size())
    {
        this->index = loop_list::size_type();
    }
    return this->loops[this->index++ % this->loops.size()];
}
