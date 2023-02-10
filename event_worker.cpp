#include "event_worker.hpp"

#include <thread/condition_variable.hpp>
#include <thread/lock_guard.hpp>

#include <cassert>
#include <exception>

void ft::serv::event_worker::offer_task(const ft::shared_ptr<task_base>& task)
{
    const ft::lock_guard<ft::mutex> lock(this->lock);
    if (this->task_closed)
    {
        // TODO: reject
        return;
    }

    try
    {
        this->tasks.push_back(task);
        this->wake_up();
    }
    catch (const std::exception& e)
    {
        // TODO: reject
    }
}

void ft::serv::event_worker::shutdown_loop() throw()
{
    {
        const ft::lock_guard<ft::mutex> lock(this->lock);
        this->active = false;
    }
    this->wake_up();
}

void ft::serv::event_worker::wait_for_loop()
{
    const ft::lock_guard<ft::mutex> lock(this->lock);
    if (!this->loop_thread)
    {
        this->cond.wait(this->lock);
    }
}

bool ft::serv::event_worker::is_in_event_loop() throw()
{
    assert(this->loop_thread);

    return ft::thread::self() == this->loop_thread;
}

bool ft::serv::event_worker::execute_tasks() throw()
{
    bool in_progress;
    task_list snapshot;
    {
        const ft::lock_guard<ft::mutex> lock(this->lock);
        this->tasks.swap(snapshot);

        in_progress = this->active;
        if (!in_progress)
        {
            this->task_closed = true;
        }
    }

    for (task_list::iterator it = snapshot.begin(); it != snapshot.end(); ++it)
    {
        const ft::shared_ptr<task_base>& task = *it;
        task->run();
    }

    return in_progress;
}
