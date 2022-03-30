//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session
        : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket)
            : socket_(std::move(socket))
    {
        std::cout << "--------------------\nsession constructor." << std::endl;
    }

    void start()
    {
        std::cout << "session start." << std::endl;
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        std::cout << "before async read." << std::endl;
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length)
                                {
                                    std::cout << "read callback called. size: " << length << ", data_: " << data_ << std::endl;
                                    if (!ec)
                                    {
                                        std::cout << "read is successful. now will write of size " << length << std::endl;
                                        do_write(length);
                                    }
                                });
        std::cout << "after async read." << std::endl;
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());

        std::cout << "before async write." << std::endl;
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                 [this, self](boost::system::error_code ec, std::size_t length)
                                 {
                                     std::cout << "write callback called. size: " << length << ", data_: " << data_ << std::endl;
                                     if (!ec)
                                     {
                                         std::cout << "write is successful. now will read." << std::endl;
                                         do_read();
                                     }
                                 });
        std::cout << "after async write." << std::endl;
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
            : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "server constructor." << std::endl;
        do_accept();
        std::cout << "listening server socket: " << acceptor_.native_handle() << std::endl;
    }

private:
    void do_accept()
    {
        std::cout << "before async accept." << std::endl;
        acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket)
                {
                    std::cout << "accept callback called. client fd: " << socket.native_handle() << std::endl;

                    if (!ec)
                    {
                        std::cout << "accept succeeded, creating a session." << std::endl;
                        std::make_shared<session>(std::move(socket))->start();
                    }

                    do_accept(); /// server should go back to listening again.
                });
        std::cout << "after async accept." << std::endl;
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        std::cout << "before io run" << std::endl;
        io_context.run();
        std::cout << "after io run" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}