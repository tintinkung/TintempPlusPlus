// TintempPlusPlus.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <dpp/dpp.h>
#include <iostream>
#include "./auth.hpp"

#include <regex> 
#include <string> 
#include <vector>
#include <sstream>

// #include "./uniqueid.hpp"
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

static enum Colors {
    AQUA = 0x00FFFF,
    BLACK = 0x000000,
    WHITE = 0xFFFFFF,
    YELLOW = 0xFFFF00,
    RED = 0xFF0000,
    GREEN = 0x00FF00,
    BLUE = 0x0000FF,
};

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
    // initButtons(bot);


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



std::queue<ttt::confirm_button> global_queue;
std::unique_ptr<ttt::confirm_button> push_onto_queue(const dpp::snowflake player_1, const dpp::snowflake player_2) {
    auto my_var = std::make_unique<ttt::confirm_button>(player_1, player_2);

    global_queue.push(*my_var);

    return std::move(my_var);
}
ttt::confirm_button get_one_from_queue() {
    return global_queue.front();
}
void pop_one_from_queue() {
    global_queue.pop();
}


// tictactoe global queue cataching
std::list<std::pair<ttt,ttt::confirm_button>> ttt_list_g;
std::pair<ttt, ttt::confirm_button> push_onto_list(const dpp::snowflake player_1, const dpp::snowflake player_2)
{
    auto data = std::make_pair(ttt(player_1, player_2), ttt::confirm_button(player_1, player_2));
    //auto data = std::make_pair(std::make_unique<ttt>(player_1, player_2), std::make_unique<ttt::confirm_button>(player_1, player_2));

    ttt_list_g.push_back(data);

    return data;
}
void erase_one_from_list(std::list<std::pair<ttt, ttt::confirm_button>>::iterator i) {
    ttt_list_g.erase(i);
}


void set_origin(dpp::message deployed_msg, int id)
{
    try
    {
        if (!ttt_list_g.empty())
        {
            for (auto itr = ttt_list_g.begin(); itr != ttt_list_g.end(); itr++) // iterate thru our saved list
            {
                if (id == itr->first.get_uid())
                {
                    // we found the itrerator to put message in!
                    if (!itr->first.have_origin)
                    {
                        itr->first.set_origin(deployed_msg);
                        std::cout << "registered ttt message origin: " << itr->first.get_origin().id << " to ttt id: " << itr->first.get_uid() << "\n";
                    }

                }
            }
        }
    }
    catch (const std::out_of_range& e) {
        std::cout << "Error setting message origin: " << e.what() << "\n";
    }
}

void tictactoe(dpp::cluster& bot, dpp::message_create_t& message)
{   
    

    // std::list<std::pair<ttt, ttt::confirm_button>>::iterator i = ttt_list_g.begin();
    // std::advance(i, 3);



    std::vector mention = message.msg.mentions; 
    /**pick first mentioned guy if there's mention in the message
     * Notes: the mention.size() already filter bot mention out :D
     */ 
    if (mention.size() > 0) 
    {   


         /*  init a new ttt operation
         │
         ├─ 📂ttt_opt: std::pair
         │  ├─ 1st: ttt class
         │  │  *manage tic-tac-toe main event.
         │  │
         │  └─ 2nd: ttt::confirm button struct
         │     *confirm player is playing the game or not.
        */
        auto ttt_opt = push_onto_list(message.msg.author.id, mention[0].first.id);  
        /* keep alive the id using uniqur_ptr and waits for callback */
        auto id = std::make_unique<int>(ttt_opt.first.get_uid());
        std::cout << "init new ttt id: "  << *id.get() << "\n";
        std::cout << "ttt: player 1: " << ttt_opt.first.get_player().first  << "\n";
        std::cout << "ttt: player 2: " << ttt_opt.first.get_player().second << "\n";

        /* create an embed */
        dpp::embed confirm_embed;    dpp::embed_author author; 
        author.icon_url = message.msg.author.get_avatar_url();
        author.name = message.msg.author.username;
        std::stringstream msg; msg << "hey <@" << ttt_opt.first.get_player().second << ">, <@" << ttt_opt.first.get_player().first << "> want to play tic-tac-toe with you";


        confirm_embed.set_title("Hey :right_facing_fist:").set_description(msg.str()).set_author(author).set_color(YELLOW); // rgb(255, 255, 102)


        // UniqueID uid_y, uid_n;


        // ttt::confirm_button c_button(mention[0].first.id, uid_y.id, uid_n.id);


        // auto z = push_onto_queue(message.msg.author.id, mention[0].first.id);
        
        
        
        // auto y = std::make_shared<ttt::confirm_button>();

        
        
        // std::shared_ptr<ttt::confirm_button> y2{ new ttt::confirm_button(mention[0].first.id, uid_y.id, uid_n.id) };
        // global_queue.push(c_button);
        
#pragma region components_row
        ttt_opt.second.component
            .add_component(
                dpp::component()
                .set_label("YES")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🗿")
                .set_style(dpp::cos_success)
                .set_id(ttt_opt.second.get_button_id().first)
                )
            .add_component(
                dpp::component()
                .set_label("Nah")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🧻")
                .set_style(dpp::cos_danger)
                .set_id(ttt_opt.second.get_button_id().second)
            );
#pragma endregion


        bot.message_create(dpp::message(message.msg.channel_id, confirm_embed)
            .add_component(ttt_opt.second.component)
            , ([&, id = *id.get()/* pull out id from uniqueptr */](const dpp::confirmation_callback_t& callback) // catch error from callback
                {
                    if (!callback.is_error())
                    {       
                        bot.log(dpp::ll_info, "command exit: succeed");
                        auto deployed_msg = std::get<dpp::message>(callback.value);

                        set_origin(deployed_msg, id);
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

#pragma region unused



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
#pragma endregion
}




void on_confirm_click(dpp::cluster& bot, dpp::message &message)
{
    std::cout << "entered on_confirm_click" << "\n";

    bot.on_button_click([&, message](const dpp::button_click_t& event)
        {

            // fetch out global ttt storage
            if (!ttt_list_g.empty())
            {
                for (auto itr = ttt_list_g.begin(); itr != ttt_list_g.end(); itr++) // iterate thru our saved list
                {
            #pragma region ANSWERED_YES
                    /*[TODO]: filter event if event is ended
                     * 
                     */
                    if (event.custom_id == itr->second.get_button_id().first)
                    {
                        dpp::embed start_embed;
                        std::stringstream msg; msg << "<@" << itr->first.get_player().first << "> ***vs*** <@" << itr->first.get_player().second << ">";
                        start_embed.set_title("Game Started!").set_description(msg.str()).set_color(GREEN);
                        itr->first.set_origin_embed(start_embed);

                        if (event.command.usr.id == itr->second.get_user().first)
                        {
                            std::cout << itr->second.get_user().first << "[1] clicked the button id: " << event.custom_id << "\n";

                            if (itr->second.st.is_player_1_confm) return;
                            itr->second.st.is_player_1_confm = true;

                            /*  this player is first to confirm -> send replie message  */
                            if (!itr->second.st.is_player_2_confm)
                            {
                                std::stringstream msg; msg << "<@" << itr->second.get_user().first << "> confirmed! " << "*(1/2)*";

                                event.reply(dpp::ir_channel_message_with_source, msg.str(), [event, itr](dpp::confirmation_callback_t callback)
                                {
                                    event.get_original_response([itr](dpp::confirmation_callback_t callback)
                                        {
                                            if (!callback.is_error())
                                            {   // store replied message in or global list
                                                itr->second.set_replied_msg(std::get<dpp::message>(callback.value));
                                                std::cout << "get response success!" << "\n";
                                                itr->second.st.replied = true;
                                            }
                                            else
                                            {
                                                std::cout << "get response message error: " << callback.get_error().message << "\n";
                                                for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                                            }


                                        });
                                });
                            }

                            /*  both player is confirmed -> send tictactoe game!  */
                            if (itr->second.st.is_player_2_confm)
                            {
                                bot.message_edit(itr->first.get_origin().add_component(itr->second.on_button_confirm)
                                , [&bot, &event](const dpp::confirmation_callback_t& callback) // catch error from callback
                                    {
                                        if (callback.is_error())
                                        {
                                            std::cout << "edit message error: " << callback.get_error().message << "\n";
                                            for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                                        }
                                        else
                                        {
                                            bot.log(dpp::ll_info, "origin message edit: succeed");
                                        }
                                    }
                                );

                                event.reply(dpp::ir_channel_message_with_source, "...", [event, itr](dpp::confirmation_callback_t callback)
                                    {
                                        if (!callback.is_error())
                                        { 

                                            std::cout << "get response success!" << "\n";
                                            itr->second.st.replied = true;
                                        }
                                        else
                                        {
                                            std::cout << "get response message error: " << callback.get_error().message << "\n";
                                            for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                                        }
                                    }
                                );



                            }
#pragma region unused


                            







                            //if (!itr->second.st.replied)
                            //{
                            //    event.reply(dpp::ir_channel_message_with_source, msg.str(), [event](dpp::confirmation_callback_t callback)
                            //        {
                            //            event.get_original_response([](dpp::confirmation_callback_t callback)
                            //            {
                            //                if (!callback.is_error())
                            //                {
                            //                    auto deployed_msg = std::get<dpp::message>(callback.value);
                            //                    std::cout << "get response success!" << "\n";
                            //                }
                            //                else
                            //                {
                            //                    std::cout << "get response message error: " << callback.get_error().message << "\n";
                            //                    auto errors = callback.get_error().errors;
                            //                    for (auto i : errors)
                            //                    {
                            //                        std::cout << i.reason << "\n";
                            //                    }
                            //                }


                            //            });
                            //        });



                            //}
                            //else
                            //{
                            //    event.get_original_response([](dpp::confirmation_callback_t callback)
                            //        {
                            //            if (!callback.is_error())
                            //            {
                            //                auto deployed_msg = std::get<dpp::message>(callback.value);
                            //                std::cout << "get response success!" << "\n";
                            //            }
                            //            else
                            //            {
                            //                std::cout << "get response message error: " << callback.get_error().message << "\n";
                            //                auto errors = callback.get_error().errors;
                            //                for (auto i : errors)
                            //                {
                            //                    std::cout << i.reason << "\n";
                            //                }
                            //            }
                            //        
                            //        
                            //        });



                            //    
                            //    event.edit_response(msg.str(), [](dpp::confirmation_callback_t callback)
                            //        {
                            //            if (!callback.is_error())
                            //            {
                            //                std::cout << "edit success!" << "\n";
                            //            }
                            //            else
                            //            {
                            //                std::cout << "edit message error: " << callback.get_error().message << "\n";
                            //                auto errors = callback.get_error().errors;
                            //                for (auto i : errors)
                            //                {
                            //                    std::cout << i.reason << "\n";
                            //                }
                            //            }

                            //        });
                            //        
                            //}
                            //itr->second.st.replied = true;


#pragma endregion
                        }

                        if (event.command.usr.id == itr->second.get_user().second)
                        {
                            std::cout << itr->second.get_user().first << "[2] clicked the button id: " << event.custom_id << "\n";

                            if (itr->second.st.is_player_2_confm) return;
                            itr->second.st.is_player_2_confm = true;

                            /*  this player is first to confirm -> send replie message  */
                            if (!itr->second.st.is_player_1_confm)
                            {
                                std::stringstream msg; msg << "<@" << itr->second.get_user().second << "> confirmed! " << "*(1/2)*";

                                event.reply(dpp::ir_channel_message_with_source, msg.str(), [event, itr](dpp::confirmation_callback_t callback)
                                {
                                    event.get_original_response([itr](dpp::confirmation_callback_t callback)
                                        {
                                            if (!callback.is_error())
                                            {   // store replied message in or global list
                                                itr->second.set_replied_msg(std::get<dpp::message>(callback.value));
                                                std::cout << "get response success!" << "\n";
                                                itr->second.st.replied = true;
                                            }
                                            else
                                            {
                                                std::cout << "get response message error: " << callback.get_error().message << "\n";
                                                for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                                            }


                                        });
                                });
                            }

                            /*  both player is confirmed -> send tictactoe game!  */
                            if (itr->second.st.is_player_1_confm)
                            {

                                bot.message_edit(itr->first.get_origin().add_component(itr->second.on_button_confirm)
                                    , [&bot, &event](const dpp::confirmation_callback_t& callback) // catch error from callback
                                {
                                    
                                    if (callback.is_error())
                                    {
                                        std::cout << "edit message error: " << callback.get_error().message << "\n";
                                        for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                                    }
                                    else
                                    {
                                        bot.log(dpp::ll_info, "origin message edit: succeed");
                                    }
                                        
                                }
                                );

                                event.reply(dpp::ir_channel_message_with_source, "...", [event, itr](dpp::confirmation_callback_t callback)
                                {
                                    if (!callback.is_error())
                                    {
                                        std::cout << "get response success!" << "\n";
                                        itr->second.st.replied = true;
                                    }
                                    else
                                    {
                                        std::cout << "get response message error: " << callback.get_error().message << "\n";
                                        for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                                    }
                                }
                                );


                            }
                        }
                    }
            #pragma endregion
            #pragma region ANSWERED_NO
                    if (event.custom_id == itr->second.get_button_id().second)
                    {
                        if (event.command.usr.id == itr->second.get_user().first)
                        {
                            std::cout << itr->second.get_user().first << "[3] clicked the button id: " << event.custom_id << "\n";

                            if (itr->second.st.is_player_1_confm) return;
                            itr->second.st.is_player_1_confm = false;

                            std::stringstream msg; msg << "<@" << itr->second.get_user().first << "> ***Declined!!!***";

                            if (!itr->second.st.replied) event.reply(dpp::ir_update_message, msg.str());
                            else  event.edit_response(msg.str()); 
                            
                            itr->second.st.replied = true;
                        }

                        if (event.command.usr.id == itr->second.get_user().second)
                        {
                            std::cout << itr->second.get_user().second << "[4] clicked the button id: " << event.custom_id << "\n";

                            if (itr->second.st.is_player_2_confm) return;
                            itr->second.st.is_player_2_confm = false;

                            std::stringstream msg; msg << "<@" << itr->second.get_user().second << "> ***Declined!!!***";

                            if (!itr->second.st.replied) event.reply(dpp::ir_update_message, msg.str());
                            else  event.edit_response(msg.str()); 
                            
                            itr->second.st.replied = true;
                        }
                    }
            #pragma endregion
                }
            }
        //


        //        if (!event.command.usr.id == itr->second.get_user().second)
        //        {
        //            event.reply(dpp::ir_channel_message_with_source, "Hey you cant click that!");
        //        }
        //        for (auto itr = ttt_list_g.begin(); itr != ttt_list_g.end(); itr++)
        //        {

        //        
        //            if (!event.command.usr.id == itr->second.get_user().second)
        //            {
        //                event.reply(dpp::ir_channel_message_with_source, "Hey you cant click that!");
        //            }

        //        }
        //    }


        //    ttt::confirm_button button = get_one_from_queue(); // get 
        //    /*
        //    event.thinking([&event](auto v) 
        //    {
        //        std::cout << "thinking done!"<< "\n";
        //    });
        //    */

        //    if (!event.command.usr.id == button.get_user().second)
        //    {
        //        event.reply(dpp::ir_channel_message_with_source, "Hey you cant click that!");
        //    }

        //    while (!global_queue.empty() && event.command.usr.id == button.get_user().second)
        //    {
        //        

        //        std::cout << "event id: " << event.custom_id << "\n";
        //        std::cout << "button id: " << button.get_button_id().first << "\n";
        //        std::cout << "user id: " << event.command.usr.id << "\n";
        //        std::cout << "clicker id: " << button.get_user().second << "\n";

        //        if (event.custom_id == button.get_button_id().first && event.command.usr.id == button.get_user().second) // clicked yes
        //        {
        //            dpp::embed new_embed;
        //            new_embed.set_description("+1");
        //            message.add_embed(new_embed);

        //            event.reply(dpp::ir_update_message, "You clicked: Yes", [&event](auto v)
        //            {
        //                std::cout << "thinking done!" << "\n";
        //            });
        //            
        //            
        //        }

        //        if (event.custom_id == button.get_button_id().second && event.command.usr.id == button.get_user().second) // clicked no
        //        {
        //            event.reply(dpp::ir_update_message,  "You clicked: no", [&event](auto v)
        //            {
        //                std::cout << "thinking done!" << "\n";
        //            });
        //        }    
        //        pop_one_from_queue(); // flush
            

        //    

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
