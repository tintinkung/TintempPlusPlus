// #include "./uniqueid.hpp"

#include "tictactoe.hpp"


ttt::ttt()
{
	init();
}

ttt::ttt(int uid)
{

}

ttt::~ttt()
{
    // on destruction
}

void ttt::init() {

}





std::pair<std::string, std::string> ttt::confirm_button::get_button_id()
{
    return this->btn_id;
}

void ttt::confirm_button::init(const dpp::snowflake user_id, int uid_y, int uid_n)
{
	this->id.first = uid_y;
	this->id.second = uid_n;
	this->user_id = user_id;
    set_btn_id(this->id);
} 

const dpp::snowflake ttt::confirm_button::get_user(){
    return this->user_id;
}

ttt::confirm_button::confirm_button(const dpp::snowflake user_id, int uid_y, int uid_n) {
    this->init(user_id, uid_y, uid_n);
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
    std::stringstream ss_2; ss_2 << "confm_" << nid.second << "_y";

    this->btn_id.first = ss_1.str();
    this->btn_id.second = ss_2.str();
}