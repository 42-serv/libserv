#include "event_worker.hpp"

#include <thread/condition_variable.hpp>
#include <thread/lock_guard.hpp>

#include <exception>

void ft::serv::event_worker::offer_task(const ft::shared_ptr<task_base>& task)
{
    const ft::lock_guard<ft::mutex> lock(this->lock);
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

bool ft::serv::event_worker::is_in_event_loop() throw()
{
    assert(this->loop_thread);

    return ft::thread::self() == this->loop_thread;
}

void ft::serv::event_worker::wait_for_loop()
{
    const ft::lock_guard<ft::mutex> lock(this->lock);
    this->cond.wait(this->lock);
}

void ft::serv::event_worker::execute_tasks() throw()
{
    task_list snapshot;
    {
        const ft::lock_guard<ft::mutex> lock(this->lock);
        this->tasks.swap(snapshot);
    }

    for (task_list::iterator it = snapshot.begin(); it != snapshot.end(); ++it)
    {
        ft::shared_ptr<task_base> task = *it;
        task->run();
    }
}
