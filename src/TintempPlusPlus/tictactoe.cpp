#include "pch.h"
#include "tictactoe.hpp"

#include "./uniqueid.hpp"
#include "./utility.h"


#pragma region ttt class

ttt::ttt(const dpp::snowflake player_1, const dpp::snowflake player_2) {
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

void ttt::init_game()
{

    dpp::component row_1, row_2, row_3;
#pragma region components_row
        row_1.add_component(
            dpp::component()
            .set_label("0 0")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[0][0].get_id())
        ).add_component(
            dpp::component()
            .set_label("0 1")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[0][1].get_id())
        ).add_component(
            dpp::component()
            .set_label("0 2")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[0][2].get_id())
        );
        row_2.add_component(
            dpp::component()
            .set_label("1 0")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[1][0].get_id())
        ).add_component(
            dpp::component()
            .set_label("1 1")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[1][1].get_id())
        ).add_component(
            dpp::component()
            .set_label("1 2")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[1][2].get_id())
        );
        row_3.add_component(
            dpp::component()
            .set_label("2 0")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[2][0].get_id())
        ).add_component(
            dpp::component()
            .set_label("2 1")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[2][1].get_id())
        ).add_component(
            dpp::component()
            .set_label("2 2")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_secondary)
            .set_id(board[2][2].get_id())
        );
#pragma endregion components_row

    if (this->have_game)
    {
        this->game.set_content("uwu")
            .add_component(row_1)
            .add_component(row_2)
            .add_component(row_3);
    }

}

void ttt::pair::init(std::string& pos)
{
    this->id = fmt::format("p_{}_{}", pos, (*new UniqueID).id);

}

ttt::pair::pair(std::string pos)
{
    this->init(pos);
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
                        logger::comment(fmt::format("registered ttt message origin: {} to ttt id: {}", itr->first.get_origin().id, itr->first.get_uid()));
                    }

                }
            }
        }
    }
    catch (const std::out_of_range& e) {
        logger::error(fmt::format("Error setting message origin: {}", e.what()));
    }
}

void on_confirm_click(dpp::cluster& bot, const dpp::button_click_t& event)
{
    // fetch out global ttt storage
    if (!ttt_list_g.empty())
    {
        logger::comment("entered on_confirm_click()");
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
                                    logger::comment("successfull get_original_response()!");
                                    itr->second.st.replied = true;
                                }
                                else
                                {
                                    logger::error("error on get_original_response(): ");
                                    for (auto i : callback.get_error().errors) { logger::trace(i.reason); }
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
                                bot.log(dpp::ll_trace, "successfull bot.message_edit()");
                            }
                            else
                            {
                                bot.log(dpp::ll_error, "error on bot.message_edit(): " + callback.get_error().message);
                                for (auto i : callback.get_error().errors) { logger::trace(i.reason); }
                            }
                        }
                    );
                    /* delete earlier waiting message */
                    bot.message_delete(itr->second.get_replied_msg().id, itr->second.get_replied_msg().channel_id);
                    /* reply thinking and finally deploy the game */
                    event.reply(dpp::ir_channel_message_with_source, "lessgo"
                        , [&bot, event, itr](dpp::confirmation_callback_t callback)
                        {
                            if (!callback.is_error())
                            {
                                logger::comment("successfull event.reply()!");
                                itr->second.st.replied = true;

                                event.get_original_response([&bot, itr](dpp::confirmation_callback_t callback)
                                    {
                                        if (!callback.is_error())
                                        {   // init actual tic-tac-toe game
                                            itr->first.set_game_origin(std::get<dpp::message>(callback.value));
                                            itr->first.init_game();

                                            logger::comment("successfull get_original_response()!");
                                            itr->second.st.replied = true;

                                        /* get game and edit it then deploy */
                                            bot.message_edit(itr->first.get_game()
                                                , [&bot](const dpp::confirmation_callback_t& callback) // catch error from callback
                                                {
                                                    if (!callback.is_error())
                                                    {
                                                        bot.log(dpp::ll_info, "ttt game initialize: succeed");
                                                    }
                                                    else
                                                    {
                                                        bot.log(dpp::ll_error, "error on get_original_response(): " + callback.get_error().message);
                                                        for (auto i : callback.get_error().errors) { logger::trace(i.reason); }
                                                    }
                                                }
                                            );

                                        }
                                        else
                                        {
                                            bot.log(dpp::ll_error, "error on bot.message_edit(): " + callback.get_error().message);
                                            for (auto i : callback.get_error().errors) { logger::trace(i.reason); }
                                        }
                                    }
                                );
                            }
                            else
                            {
                                bot.log(dpp::ll_error, "error on event.reply(): " + callback.get_error().message);
                                for (auto i : callback.get_error().errors) { logger::trace(i.reason); }
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
                            bot.log(dpp::ll_trace, "succesfully bot.message_edit()!");
                        }
                        else {
                            bot.log(dpp::ll_error, "error on bot.message_edit(): " + callback.get_error().message);
                            for (auto i : callback.get_error().errors) { logger::trace(i.reason); }
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

void on_ttt_interaction(dpp::cluster& bot, const dpp::button_click_t& event)
{

}
