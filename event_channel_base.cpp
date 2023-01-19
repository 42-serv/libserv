/* Any copyright is dedicated to the Public Domain.
 * https://creativecommons.org/publicdomain/zero/1.0/ */

#include "event_channel_base.hpp"

#include "event_worker.hpp"
#include "serv_types.hpp"

ft::serv::event_channel_base::event_channel_base(ident_t ident)
    : ident(ident), recv_buf(), send_buf(), readability_interested(), writability_interested(), readability_enabled(), writability_enabled(), input_closed(), output_closed()
{
}

void ft::serv::event_channel_base::trigger_read() throw()
{
}

void ft::serv::event_channel_base::trigger_write() throw()
{
}
