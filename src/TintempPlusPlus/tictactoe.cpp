#include "pch.h"
#include "tictactoe.hpp"

#include "./uniqueid.hpp"
#include "./utility.h"


#pragma region ttt class

ttt::ttt(const dpp::snowflake player_1, const dpp::snowflake player_2)
{
    this->player_1 = player_1;
    this->player_2 = player_2;
	init();
}

ttt::~ttt()
{
    // on destruction
}

void ttt::init() 
{
    init_uid((*new UniqueID).id);
}

#pragma region ttt::confirm_button
// a confiem button to challenge player 2
void ttt::confirm_button::init(const dpp::snowflake player_1, const dpp::snowflake player_2, int uid_y, int uid_n)
{
	this->id.first = uid_y;
	this->id.second = uid_n;
    this->player_1 = player_1;
    this->player_2 = player_2;
    set_btn_id(this->id);

    this->on_button_confirm
        .add_component(
            dpp::component()
            .set_label("Confirmed")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_disabled(true)
            .set_id("no_interaction_" + uid_y)
        ).add_component(
            dpp::component()
            .set_label("Confirmed")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_disabled(true)
            .set_id("no_interaction_" + uid_n)
        );
} 

std::pair<std::string, std::string> ttt::confirm_button::get_button_id()
{
    return this->btn_id;
}

const std::pair<dpp::snowflake, dpp::snowflake> ttt::confirm_button::get_user(){
    return std::make_pair(this->player_1, this->player_2);
}

ttt::confirm_button::confirm_button(const dpp::snowflake player_1, const dpp::snowflake player_2) {
    this->init(player_1, player_2, (*new UniqueID).id, (*new UniqueID).id);
}

const std::pair<int, int> ttt::confirm_button::get_id()
{
    return this->id;
}

void ttt::confirm_button::set_id(std::pair<int, int> nid)
{
    this->id.first = nid.first;
    this->id.second = nid.second;
}

void ttt::confirm_button::set_btn_id(std::pair<int, int> nid)
{
    std::stringstream ss_1; ss_1 << "confm_" << nid.first << "_y";
    std::stringstream ss_2; ss_2 << "confm_" << nid.second << "_n";

    this->btn_id.first = ss_1.str();
    this->btn_id.second = ss_2.str();
}

#pragma endregion !ttt::confirm_button

#pragma endregion !ttt class


/* * ttt global tracking list
 * use to track ttt command globally.
 * the list stores ttt class and ttt::confirm_button as a pair
 */
std::list<std::pair<ttt, ttt::confirm_button>> ttt_list_g;

std::pair<ttt, ttt::confirm_button> push_onto_list(const dpp::snowflake player_1, const dpp::snowflake player_2) {
    auto data = std::make_pair(ttt(player_1, player_2), ttt::confirm_button(player_1, player_2));

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

void on_confirm_click(dpp::cluster& bot, const dpp::button_click_t& event)
{
    // fetch out global ttt storage
    if (!ttt_list_g.empty())
    {
        std::cout << "entered on_confirm_click" << "\n";
        for (auto itr = ttt_list_g.begin(); itr != ttt_list_g.end(); itr++) // iterate thru our saved list
        {
            /* setup */
            dpp::snowflake player_1 = itr->second.get_user().first;
            dpp::snowflake player_2 = itr->second.get_user().second;
            short player = 0; /* player who pressed the button (1 or 2) */
            if (event.command.usr.id == player_1) player = 1;
            else if (event.command.usr.id == player_2) player = 2;

#pragma region ANSWERED_YES
            /*[TODO]: filter event if event is ended
             *
             */
            /* pressed the "YES" button */
            if (event.custom_id == itr->second.get_button_id().first)
            {
            /* if this player already pressed the button, return */
                if (player == 1? itr->second.st.is_player_1_confm : itr->second.st.is_player_2_confm) return;
                player == 1 ? itr->second.st.is_player_1_confm = true : itr->second.st.is_player_2_confm = true;

            /*  this player is first to confirm -> send replie message  */
                if (!(player == 1? itr->second.st.is_player_2_confm: itr->second.st.is_player_1_confm))
                {
                    std::stringstream msg; msg << "<@" << (player == 1? player_1 : player_2) << "> confirmed! " << "*(1/2)*";

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
                else if (player == 1 ? itr->second.st.is_player_2_confm : itr->second.st.is_player_1_confm)
                {
                    /* create embed displaying that the game has started */
                    dpp::embed start_embed;
                    auto confirmed_msg = fmt::format("<@{}> ***vs*** <@{}>", player_1, player_2);
                    start_embed.set_title("Game Started!").set_description(confirmed_msg).set_color(util::GREEN);
                    itr->first.set_origin_embed(start_embed);

                    /* edit the origin embed */
                    bot.message_edit(itr->first.get_origin().add_component(itr->second.on_button_confirm)
                    , [&bot, &event](const dpp::confirmation_callback_t& callback) // catch error from callback
                        {
                            if (!callback.is_error())
                            {
                                bot.log(dpp::ll_info, "origin message edit: succeed");

                            }
                            else
                            {
                                std::cout << "edit message error: " << callback.get_error().message << "\n";
                                for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
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
#pragma endregion
#pragma region ANSWERED_NO
            /* pressed the "NO" Button */
            if (event.custom_id == itr->second.get_button_id().second)
            {
            /* if this player already pressed the button, return */
                if (player == 1 ? itr->second.st.is_player_1_confm : itr->second.st.is_player_2_confm) return;
                player == 1 ? itr->second.st.is_player_1_confm = true : itr->second.st.is_player_2_confm = true;

            /* create embed displaying that the game has been cancelled */
                dpp::embed cancelled_embed;
                auto cancelled_msg = fmt::format("<@{}> Declined", player == 1 ? player_1 : player_2);
                cancelled_embed.set_title("*Cancelled*").set_description(cancelled_msg).set_color(util::RED);
                itr->first.set_origin_embed(cancelled_embed);

                /* edit the origin embed */
                bot.message_edit(itr->first.get_origin()
                , [&bot, &event](const dpp::confirmation_callback_t& callback) // catch error from callback
                    {
                        if (!callback.is_error()) {
                            bot.log(dpp::ll_info, "origin message edit: succeed");
                        }
                        else {
                            std::cout << "edit message error: " << callback.get_error().message << "\n";
                            for (auto i : callback.get_error().errors) { std::cout << i.reason << "\n"; }
                        }
                    }
                );

            /* if there's confirm repli sent before, delet it */
                if (itr->second.st.replied) {
                    bot.message_delete(itr->second.get_replied_msg().id, itr->second.get_replied_msg().channel_id);
                }
            }
#pragma endregion
        }
    }
        
    
}