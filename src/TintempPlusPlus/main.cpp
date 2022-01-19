// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"

#include "./auth.hpp"
#include "./tictactoe.hpp"
#include "./utility.h"

#pragma region pre-declared functions

void initCommands(dpp::cluster& bot);
void initButtons(dpp::cluster& bot);
void newCommand(std::string command_name, dpp::cluster& bot, dpp::message_create_t& message, void(*command_event)(dpp::cluster&, dpp::message_create_t&));

void tictactoe(dpp::cluster& bot, dpp::message_create_t& message);
void ping(dpp::cluster &bot, dpp::message_create_t &message);

#pragma endregion

int main()
{
/* login */
    dpp::cluster bot(TOKEN);
    try 
    {
        bot.on_ready([&bot](const dpp::ready_t& event) {
            bot.log(dpp::ll_info, "Logged in as " + bot.me.username);
        });
        
        bot.on_log([](const dpp::log_t& event) {
           if (event.severity >= logger::config.log_level) 
           {
               try
               {
                    logger::log(event.severity, event.message);
               }
               catch (const std::exception& e)
               {    /* just in case, use standard std::cout logging if the logger somehow fail */
                   std::cout << dpp::utility::current_date_time() << " [" << dpp::utility::loglevel(event.severity) << "]: " << event.message << "\n";
               }

           }
        });
         
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }

/* runtime */
    initCommands(bot);
    initButtons(bot);


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
void initButtons(dpp::cluster& bot)
{
    bot.on_button_click([&bot](const dpp::button_click_t& event)
        {
            bot.log(dpp::ll_info, fmt::format("user <@{}> pressed the button [{}]", event.command.usr.id, event.custom_id));

            /* list all button interaction here */
            on_confirm_click(bot, event);

            on_ttt_interaction(bot, event);

        }
    );
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

#pragma region commands

void ping(dpp::cluster& bot, dpp::message_create_t& message)
{
    dpp::embed ping_embed;
    ping_embed.set_title("pong!");

    bot.message_create(dpp::message(message.msg.channel_id, ping_embed));
}

void tictactoe(dpp::cluster& bot, dpp::message_create_t& message)
{   
    /**pick first mentioned guy if there's mention in the message
     * Notes: the mention.size() already filter bot mention out :D
     */ 
    std::vector mention = message.msg.mentions; 
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

        bot.log(dpp::ll_trace, fmt::format("init new ttt id: {}", *id.get()));
        bot.log(dpp::ll_trace, fmt::format("ttt: player 1:  {}", ttt_opt.first.get_player().first));
        bot.log(dpp::ll_trace, fmt::format("ttt: player 2: {}", ttt_opt.first.get_player().second));


        /* create an embed */
        dpp::embed confirm_embed;    dpp::embed_author author; 
        author.icon_url = message.msg.author.get_avatar_url();
        author.name = message.msg.author.username;
        auto description = fmt::format("<@{}>, <@{}> want to play tic-tac-toe with you", ttt_opt.first.get_player().second, ttt_opt.first.get_player().first);
        confirm_embed.set_title("Hey :right_facing_fist:").set_description(description).set_author(author).set_color(util::YELLOW); // rgb(255, 255, 102)
        
    #pragma region components_row
        ttt_opt.second.component
            .add_component(dpp::component()
                .set_label("YES")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🗿")
                .set_style(dpp::cos_success)
                .set_id(ttt_opt.second.get_button_id().first)
            ).add_component(dpp::component()
                .set_label("Nah")
                .set_type(dpp::cot_button)
                //.set_emoji(u8"🧻")
                .set_style(dpp::cos_danger)
                .set_id(ttt_opt.second.get_button_id().second)
            );
    #pragma endregion

        /* init a first message of this command */
        bot.message_create(dpp::message(message.msg.channel_id, confirm_embed).add_component(ttt_opt.second.component)
            , ([&, id = *id.get()/* pull out id from uniqueptr */](const dpp::confirmation_callback_t& callback) // catch error from callback
                {
                    if (!callback.is_error())
                    {       
                        bot.log(dpp::ll_trace, "command exit: succeed");
                        auto deployed_msg = std::get<dpp::message>(callback.value);

                        set_origin(deployed_msg, id);
                    }
                    else
                    {
                        bot.log(dpp::ll_error, "command exit: " + callback.get_error().message);
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
#pragma endregion
}

#pragma endregion

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
