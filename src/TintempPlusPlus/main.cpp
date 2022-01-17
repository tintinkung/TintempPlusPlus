// TintempPlusPlus.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <dpp/dpp.h>
#include <iostream>
#include "./auth.hpp"

#include <regex> 
#include <string> 
#include <vector>
#include <sstream>

#include "./uniqueid.hpp"
#include "./tictactoe.hpp"

#include <queue>
#include <memory>
#include <functional>

#include <list>
#include <algorithm>

void newCommand(std::string command_name, dpp::cluster& bot, dpp::message_create_t& message, void(*command_event)(dpp::cluster&, dpp::message_create_t&));
void initCommands(dpp::cluster& bot);

void tictactoe(dpp::cluster& bot, dpp::message_create_t& message);
void ping(dpp::cluster &bot, dpp::message_create_t &message);

void on_deploy_button(dpp::cluster& bot, dpp::message &message, dpp::embed embed);
void on_confirm_click(dpp::cluster& bot, dpp::message &message);


int main()
{
    dpp::cluster bot(TOKEN);
    try {
        // Be sure to place your token in the line below, and uncomment the line
        // Follow steps here to get a token: https://dpp.dev/creating-a-bot-application.html
        

        bot.on_ready([&bot](const dpp::ready_t& event) {
            bot.log(dpp::ll_info, "Logged in as " + bot.me.username);
            });

        
        bot.on_log([](const dpp::log_t& event) {
            if (event.severity > dpp::ll_trace) {
                std::cout << dpp::utility::loglevel(event.severity) << ": " << event.message << "\n";
            }
            });
         
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }


    initCommands(bot);

    bot.start(false);
    return 0;
}

void initCommands(dpp::cluster &bot)
{
    bot.on_message_create([&bot](dpp::message_create_t message) 
    {
        newCommand("ping", bot, message, &ping);

        newCommand("ttt", bot, message, &tictactoe);
    });

}

void newCommand(std::string command_name, dpp::cluster& bot, dpp::message_create_t& message, void(*command_event)(dpp::cluster&, dpp::message_create_t&))
{
    std::stringstream ss; ss << "^" << "\\" << PREFIX << command_name; // create regex string 
    std::regex regexp(ss.str());
    

    if (std::regex_search(message.msg.content, regexp) == 1)
    {
        bot.log(dpp::ll_info, "command occured: " + command_name);
        command_event(bot, message);
    }
    
}


// commands
void ping(dpp::cluster& bot, dpp::message_create_t& message)
{
    dpp::embed ping_embed;
    ping_embed.set_title("pong!");

    bot.message_create(dpp::message(message.msg.channel_id, ping_embed));
}


// tictactoe global queue cataching
std::queue<ttt::confirm_button> global_queue;
std::unique_ptr<ttt::confirm_button> push_onto_queue(const dpp::snowflake user_id, int uid_y, int uid_n) {
    auto my_var = std::make_unique<ttt::confirm_button>(user_id, uid_y, uid_n);

    global_queue.push(*my_var);

    return std::move(my_var);
}
ttt::confirm_button get_one_from_queue() {
    return global_queue.front();
}
void pop_one_from_queue() {
    global_queue.pop();
}


void tictactoe(dpp::cluster& bot, dpp::message_create_t& message)
{   
    std::vector mention = message.msg.mentions;
        
    //std::list <ttt> confm_list;
    //std::list<ttt>::iterator i = confm_list.begin();
    //std::advance(i, 3);

    //confm_list.push_back(*new ttt());
    //confm_list.back().init();

    /**pick first mentioned guy if there's mention in the message
     * Notes: the mention.size() already filter bot mention out :D
     */ 
    if (mention.size() > 0) 
    {   
        std::cout << "mentions: " << mention[0].first.id << "\n";

        dpp::embed confirm_embed;
        dpp::embed_author author;
        author.icon_url = message.msg.author.get_avatar_url();
        author.name = message.msg.author.username;

        std::stringstream msg; msg << "hey <@" << mention[0].first.id << ">" << mention[0].first.discriminator << " wanna play tic-tac-toe?";

        confirm_embed.set_description(msg.str()).set_author(author);


        ttt ttt_cmd;
        // UniqueID uid_y, uid_n;
        ttt::confirm_button c_button(mention[0].first.id, uid_y.id, uid_n.id);
        auto z = push_onto_queue(mention[0].first.id, uid_y.id, uid_n.id);
        // auto y = std::make_shared<ttt::confirm_button>();

        
        
        std::shared_ptr<ttt::confirm_button> y2{ new ttt::confirm_button(mention[0].first.id, uid_y.id, uid_n.id) };
        // global_queue.push(c_button);
        
#pragma region components_row
        z.get()->component
            .add_component(
                dpp::component()
                .set_label("YES")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🗿")
                .set_style(dpp::cos_success)
                .set_id(z.get()->get_button_id().first)
                )
            .add_component(
                dpp::component()
                .set_label("Nah")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🧻")
                .set_style(dpp::cos_danger)
                .set_id(z.get()->get_button_id().second)
            );
#pragma endregion

        std::cout << "1: " << z.get()->get_button_id().first << "\n";


        bot.message_create(dpp::message(message.msg.channel_id, confirm_embed)
            .add_component(z.get()->component)
            , ([&](const dpp::confirmation_callback_t& callback) // catch error from callback
                {
                    if (!callback.is_error())
                    {       
                        bot.log(dpp::ll_info, "command exit: succeed");
                        auto deployed_msg = std::get<dpp::message>(callback.value);
                        on_confirm_click(bot, deployed_msg);

                    }
                    else
                    {
                        bot.log(dpp::ll_warning, "command exit: " + callback.get_error().message);
                    }
                }
            )
        );
    }


//    // ======================
//
//    enum status {
//        IDLE,
//        IS_X,
//        IS_O,
//        EXPIRED
//    };
//    struct button {
//        status status;
//        std::stringstream id;
//        
//    };
//
//    UniqueID ttt_id;
//    ttt this_ttt(ttt_id.id);
//
//    dpp::snowflake this_message;
//    UniqueID uid_1, uid_2, uid_3, uid_4, uid_5, uid_6, uid_7, uid_8, uid_9;
//    button ttt_000, ttt_001, ttt_002, ttt_100, ttt_101, ttt_102, ttt_200, ttt_201, ttt_202;
//    ttt_000.id << "ttt_" << uid_1.id << "_<000>"; 
//    ttt_001.id << "ttt_" << uid_1.id << "_<001>"; 
//    ttt_002.id << "ttt_" << uid_1.id << "_<002>";
//    ttt_100.id << "ttt_" << uid_1.id << "_<100>";
//    ttt_101.id << "ttt_" << uid_1.id << "_<101>";
//    ttt_102.id << "ttt_" << uid_1.id << "_<102>";
//    ttt_200.id << "ttt_" << uid_1.id << "_<200>";
//    ttt_201.id << "ttt_" << uid_1.id << "_<201>";
//    ttt_202.id << "ttt_" << uid_1.id << "_<202>";
//                
//    dpp::component row_1, row_2, row_3;
//#pragma region components_row
//    row_1.add_component(
//        dpp::component()
//            .set_label("0 0")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"🗿")
//            .set_style(dpp::cos_secondary)
//            .set_id(ttt_000.id.str())
//        )
//        .add_component(
//            dpp::component()
//            .set_label("0 1")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"🧻")
//            .set_style(dpp::cos_success)
//            .set_id(ttt_001.id.str())
//        )
//        .add_component(
//            dpp::component()
//            .set_label("0 2")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"⚔")
//            .set_style(dpp::cos_primary)
//            .set_id(ttt_002.id.str())
//        );
//    row_2.add_component(
//        dpp::component()
//            .set_label("1 0")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"🗿")
//            .set_style(dpp::cos_secondary)
//            .set_id(ttt_100.id.str())
//        )
//        .add_component(
//            dpp::component()
//            .set_label("1 1")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"🧻")
//            .set_style(dpp::cos_success)
//            .set_id(ttt_101.id.str())
//        )
//        .add_component(
//            dpp::component()
//            .set_label("1 2")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"⚔")
//            .set_style(dpp::cos_primary)
//            .set_id(ttt_102.id.str())
//        );
//    row_3.add_component(
//        dpp::component()
//            .set_label("2 0")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"🗿")
//            .set_style(dpp::cos_secondary)
//            .set_id(ttt_200.id.str())
//        )
//        .add_component(
//            dpp::component()
//            .set_label("2 1")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"🧻")
//            .set_style(dpp::cos_success)
//            .set_id(ttt_201.id.str())
//        )
//        .add_component(
//            dpp::component()
//            .set_label("2 2")
//            .set_type(dpp::cot_button)
//            //.set_emoji(u8"⚔")
//            .set_style(dpp::cos_primary)
//            .set_id(ttt_202.id.str())
//        );
//#pragma endregion components_row
//
//    
//    bot.message_create(dpp::message(message.msg.channel_id, ping_embed)
//        .add_component(row_1).add_component(row_2).add_component(row_3)
//        , ([&](const dpp::confirmation_callback_t& callback) // catch error from callback
//            {
//
//                if (!callback.is_error())
//                {
//                    bot.log(dpp::ll_info, "command exit: succeed");
//                    auto deployed_msg = std::get<dpp::message>(callback.value); //get message object from a callback.value
//
//                    std::cout << "grabbing msg : " << deployed_msg.id << "\n";
//                    on_deploy_button(bot, deployed_msg, ping_embed);
//                    
//                }      
//                else
//                {
//                    bot.log(dpp::ll_warning, "command exit: " + callback.get_error().message);
//                }
//                    
//                // std::cout << "before message: " << message.msg.id << "\n";
//                // this_ttt.set_origin(message.msg.id);
//                // std::cout << "captured message: " << this_ttt.get_origin() << "\n";
//        
//               
//
//            }
//        )
//    );
        

        ///
        ///* When a user clicks your button, the on_button_click event will fire,
        // * containing the custom_id you defined in your button.
        // */
        //bot.on_button_click([&](const dpp::button_click_t& event) {
        //    /* Button clicks are still interactions, and must be replied to in some form to
        //     * prevent the "this interaction has failed" message from Discord to the user.
        //     */
        //

        //    event.reply(dpp::ir_channel_message_with_source, "You clicked: `" + event.custom_id + "`");
        //    std::cout << "message: " << this_ttt.get_origin() << "\n";
        //    
        //    bot.message_edit(dpp::message(this_ttt.get_origin(), ping_embed)
        //        .add_component(row_1).add_component(row_2).add_component(row_3)
        //        , [&bot, &message](const dpp::confirmation_callback_t& callback) // catch error from callback
        //        {

        //            if (callback.is_error())
        //                bot.log(dpp::ll_warning, "command exit: " + callback.get_error().message);
        //            else
        //                bot.log(dpp::ll_info, "command exit: succeed");
        //        }
        //    );
        //
        //    // bot.message_edit()

        //    std::cout << event.raw_event << "\n";

        //    }
        //);


    /*
    int my_int
   
    my_function(parameter, [&](const callback_object object)
        {
            my_int = 69;
        }
    )

   std::cout << my_int << "how do i make my_int 69\n";
        */
}

void on_confirm_click(dpp::cluster& bot, dpp::message& message)
{
    std::cout << "entered on_confirm_click" << "\n";
    bot.on_button_click([&](const dpp::button_click_t& event) 
        {
            ttt::confirm_button button = get_one_from_queue(); // get 
            /*
            event.thinking([&event](auto v) 
            {
                std::cout << "thinking done!"<< "\n";
            });
            */

            if (!event.command.usr.id == button.get_user())
            {
                event.reply(dpp::ir_channel_message_with_source, "Hey you cant click that!");
            }

            while (!global_queue.empty() && event.command.usr.id == button.get_user())
            {
                

                std::cout << "event id: " << event.custom_id << "\n";
                std::cout << "button id: " << button.get_button_id().first << "\n";
                std::cout << "user id: " << event.command.usr.id << "\n";
                std::cout << "clicker id: " << button.get_user() << "\n";

                if (event.custom_id == button.get_button_id().first && event.command.usr.id == button.get_user()) // clicked yes
                {
                    dpp::embed new_embed;
                    new_embed.set_description("+1");
                    message.add_embed(new_embed);

                    event.reply(dpp::ir_update_message, "You clicked: Yes", [&event](auto v)
                    {
                        std::cout << "thinking done!" << "\n";
                    });
                    
                    
                }

                if (event.custom_id == button.get_button_id().second && event.command.usr.id == button.get_user()) // clicked no
                {
                    event.reply(dpp::ir_update_message,  "You clicked: no", [&event](auto v)
                    {
                        std::cout << "thinking done!" << "\n";
                    });
                }    
                pop_one_from_queue(); // flush
            }

            

        }
    );
}


void on_deploy_button(dpp::cluster &bot, dpp::message &message, dpp::embed embed)
{
    std::cout << "entered deployment with message: " << message.id << "\n";

    bot.on_button_click([&](const dpp::button_click_t& event) {
        /* Button clicks are still interactions, and must be replied to in some form to
         * prevent the "this interaction has failed" message from Discord to the user.
         */
        std::cout << "clicked button from message id: " << message.id << "\n";
        // event.reply(dpp::ir_channel_message_with_source, "You clicked: `" + event.custom_id + "`");

        dpp::component row_1, row_2, row_3;
#pragma region components_row
        row_1.add_component(
            dpp::component()
            .set_label("0 01")
            .set_type(dpp::cot_button)
            //.set_emoji(u8"🗿")
            .set_style(dpp::cos_secondary)
            .set_id("1")
        )
            .add_component(
                dpp::component()
                .set_label("0 11")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🧻")
                .set_style(dpp::cos_success)
                .set_id("2")
            )
            .add_component(
                dpp::component()
                .set_label("0 21")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"⚔")
                .set_style(dpp::cos_primary)
                .set_id("3")
            );
        row_2.add_component(
            dpp::component()
            .set_label("1 0")
            .set_type(dpp::cot_button)
            //.set_emoji(u8"🗿")
            .set_style(dpp::cos_secondary)
            .set_id("4")
        )
            .add_component(
                dpp::component()
                .set_label("1 1")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🧻")
                .set_style(dpp::cos_success)
                .set_id("5")
            )
            .add_component(
                dpp::component()
                .set_label("1 2")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"⚔")
                .set_style(dpp::cos_primary)
                .set_id("6")
            );
        row_3.add_component(
            dpp::component()
            .set_label("2 0")
            .set_type(dpp::cot_button)
            //.set_emoji(u8"🗿")
            .set_style(dpp::cos_secondary)
            .set_id("7")
        )
            .add_component(
                dpp::component()
                .set_label("2 1")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🧻")
                .set_style(dpp::cos_success)
                .set_id("8")
            )
            .add_component(
                dpp::component()
                .set_label("2 2")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"⚔")
                .set_style(dpp::cos_primary)
                .set_id("9")
            );
#pragma endregion components_row


        dpp::embed ping_embed;
        ping_embed.set_title("yay!");
        message.add_embed(ping_embed);

        event.thinking([&bot, &event](const dpp::confirmation_callback_t& callback) 
        {
            if (!callback.is_error())
            {
                auto confirmation = std::get<dpp::confirmation>(callback.value); //get message object from a callback.value

                
            }
            else
            {
                bot.log(dpp::ll_warning, "command exit: " + callback.get_error().message);
            }
        });


  

        bot.message_edit(message.add_component(row_1).add_component(row_2).add_component(row_3)
            , [&bot, &event](const dpp::confirmation_callback_t& callback) // catch error from callback
            {

                if (callback.is_error())
                    bot.log(dpp::ll_warning, "command exit: " + callback.get_error().message); 
                else
                    bot.log(dpp::ll_info, "command exit: succeed"); 
            }
        );

        // bot.message_edit()

        std::cout << event.raw_event << "\n";

        }
    );
}

/*
std::queue<std::pair<std::function<void(int)>, int>> global_queue_or_smth;


std::unique_ptr<int> push_onto_queue() {
    auto my_var = std::make_unique<int>(4);

    global_queue_or_smth.push({ [my_var = my_var.get()] (int x) {
      *my_var = x;
    }, 0 });

    return std::move(my_var);
}



void pop_one_from_queue() {
    auto [func, val] = global_queue_or_smth.front();

    func(val);

    global_queue_or_smth.pop();
}
*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
