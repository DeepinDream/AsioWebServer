#pragma once

#if defined(ASIO_STANDALONE)
// MSVC : define environment path 'ASIO_STANDALONE_INCLUDE', e.g.
// 'E:\bdlibs\asio-1.10.6\include'

#    include <asio.hpp>
#    include <asio/ssl.hpp>
#    include <asio/steady_timer.hpp>
namespace boost {
namespace asio {
    using namespace ::asio;
}
namespace system {
    using ::std::error_code;
}
}  // namespace boost
#else
#    include <boost/asio.hpp>
#    include <boost/asio/ssl.hpp>
#    include <boost/asio/steady_timer.hpp>

#endif
