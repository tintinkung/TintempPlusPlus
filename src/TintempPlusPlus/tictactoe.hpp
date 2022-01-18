#ifndef TICTACTOE_HPP
#define TICTACTOE_HPP

#include <dpp/dpp.h>
#include <sstream>



class ttt
{
private:
	// origin message id of this ttt
	dpp::message origin;  

    int uid;
    dpp::snowflake player_1;
    dpp::snowflake player_2;

    void init_uid(int &uid) { this->uid = uid; };
    void init();
public:
    bool have_origin = false;
    struct confirm_button
    {
    private:
        std::pair<int, int> id;
        std::pair<std::string, std::string> btn_id;
        dpp::message replied_msg;

        dpp::snowflake player_1;
        dpp::snowflake player_2;

        void init(const dpp::snowflake player_1, const dpp::snowflake player_2, int uid_y, int uid_n);
    public:
        dpp::component component;
        dpp::component on_button_confirm;
        struct stat {
            bool is_player_1_confm = false;
            bool is_player_2_confm = false;
            bool replied = false;
            bool cancelled = false;
        }; stat st;
       
        const std::pair<dpp::snowflake, dpp::snowflake> const get_user();
        const std::pair<int, int> const get_id();
        std::pair<std::string, std::string> get_button_id();
        dpp::message get_replied_msg() { return this->replied_msg; }

        
        void set_id(std::pair<int, int> nid);
        void set_btn_id(std::pair<int, int> nid);
        void set_replied_msg(dpp::message replied_msg) { this->replied_msg = replied_msg; }

        confirm_button(const dpp::snowflake player_1, const dpp::snowflake player_2);
    };

	// --- Constructors ---
    ttt(const dpp::snowflake player_1, const dpp::snowflake player_2);

	// --- Destructors ---
    ~ttt();

	// --- Accessors ---
	dpp::message& get_origin() { return this->origin;  }
    const std::pair<dpp::snowflake, dpp::snowflake> get_player() const { return std::make_pair(this->player_1, this->player_2); }
    const int& get_uid() const { return this->uid; }

	// --- Modifiers ---
    void set_origin(dpp::message origin_message) { this->origin = origin_message; this->have_origin = true; }
    void set_origin_embed(dpp::embed &new_embed) { this->origin.embeds.front() = new_embed; }

	// --- Functions ---
	
    
};


#endif