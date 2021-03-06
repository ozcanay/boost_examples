//
// blocking_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

const int max_length = 1024;

void session(tcp::socket sock)
{
    std::cout << "session thread created." << std::endl;
    try
    {
        for (;;)
        {
            char data[max_length];

            boost::system::error_code error;
            std::cout << "before read." << std::endl;
            size_t length = sock.read_some(boost::asio::buffer(data), error);
            std::cout << "after read. data: " << data << std::endl;
            if (error == boost::asio::error::eof) {
                std::cout << "eof read, connection closed cleanly by peer." << std::endl;
                break; // Connection closed cleanly by peer.
            } else if (error) {
                throw boost::system::system_error(error); // Some other error.
            }

            std::cout << "before write." << std::endl;
            boost::asio::write(sock, boost::asio::buffer(data, length));
            std::cout << "after write." << std::endl;

            std::memset(data, 0, max_length); /// I added this to the official example.
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}

void server(boost::asio::io_context& io_context, unsigned short port)
{
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        std::thread(session, a.accept()).detach();
    }
}

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server(io_context, std::atoi(argv[1]));
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}