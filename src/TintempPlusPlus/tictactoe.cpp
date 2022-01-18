#include "./uniqueid.hpp"
#include "tictactoe.hpp"


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

#pragma region Confirm button 
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

const std::pair<dpp::snowflake, dpp::snowflake> const ttt::confirm_button::get_user(){
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

#pragma endregion