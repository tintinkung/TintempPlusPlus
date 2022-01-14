// TintempPlusPlus.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <dpp/dpp.h>
#include <iostream>
#include "./auth.hpp"

int main()
{
    try {
        // Be sure to place your token in the line below, and uncomment the line
        // Follow steps here to get a token: https://dpp.dev/creating-a-bot-application.html
        dpp::cluster bot(TOKEN);

        bot.on_ready([&bot](const dpp::ready_t& event) {
            bot.log(dpp::ll_info, "Logged in as " + bot.me.username);
            });

        bot.on_message_create([&bot](const dpp::message_create_t& event) {
            if (event.msg.content == "!ping") {
                bot.message_create(dpp::message(event.msg.channel_id, "Pong!"));
            }
            });

        bot.on_log([](const dpp::log_t& event) {
            if (event.severity > dpp::ll_trace) {
                std::cout << dpp::utility::loglevel(event.severity) << ": " << event.message << "\n";
            }
            });

        bot.start(false);
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
