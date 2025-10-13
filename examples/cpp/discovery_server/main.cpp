// Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file main.cpp
 *
 */

#include <csignal>
#include <memory>
#include <thread>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/log/Log.hpp>

#include "CLIParser.hpp"

#if defined(ENTITY_PUBLISHER)
    #include "ClientPublisherApp.hpp"
    using AppType = eprosima::fastdds::examples::discovery_server::ClientPublisherApp;
    #define APP_NAME "Publisher"
#elif defined(ENTITY_SUBSCRIBER)
    #include "ClientSubscriberApp.hpp"
    using AppType = eprosima::fastdds::examples::discovery_server::ClientSubscriberApp;
    #define APP_NAME "Subscriber"
#elif defined(ENTITY_SERVER)
    #include "ServerApp.hpp"
    using AppType = eprosima::fastdds::examples::discovery_server::ServerApp;
    #define APP_NAME "Discovery Server"
#else
    #error "Must define ENTITY_PUBLISHER, ENTITY_SUBSCRIBER, or ENTITY_SERVER"
#endif

using eprosima::fastdds::dds::Log;
using namespace eprosima::fastdds::examples::discovery_server;

std::shared_ptr<AppType> g_app;

void signal_handler(int signum)
{
    std::cout << "\nSignal " << signum << " received, stopping " << APP_NAME << "." << std::endl;
    if (g_app)
    {
#if defined(ENTITY_PUBLISHER)
        // For Publisher: SIGTERM triggers graceful shutdown (for hot update)
        if (signum == SIGTERM)
        {
            g_app->graceful_shutdown();
        }
        else
        {
            g_app->stop();
        }
#else
        g_app->stop();
#endif
    }
}

int main(int argc, char** argv)
{
    auto ret = EXIT_SUCCESS;
    
#if defined(ENTITY_PUBLISHER)
    auto config = CLIParser::parse_publisher_options(argc, argv);
    uint16_t samples = config.samples;
#elif defined(ENTITY_SUBSCRIBER)
    auto config = CLIParser::parse_subscriber_options(argc, argv);
    uint16_t samples = config.samples;
#elif defined(ENTITY_SERVER)
    auto config = CLIParser::parse_server_options(argc, argv);
    uint16_t samples = 0;
#endif

    try
    {
        g_app = std::make_shared<AppType>(config);
    }
    catch (const std::runtime_error& e)
    {
        EPROSIMA_LOG_ERROR(APP_NAME, e.what());
        ret = EXIT_FAILURE;
    }

    if (EXIT_FAILURE != ret)
    {
        std::thread thread(&AppType::run, g_app);

        if (samples == 0)
        {
            std::cout << APP_NAME << " running. Press Ctrl+C to stop." << std::endl;
        }
        else
        {
            std::cout << APP_NAME << " running for " << samples 
                      << " samples. Press Ctrl+C to stop." << std::endl;
        }

        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);
#ifndef _WIN32
        signal(SIGQUIT, signal_handler);
        signal(SIGHUP, signal_handler);
#endif

        thread.join();
    }

    Log::Reset();
    return ret;
}
