

#ifndef TICTACTOE_HPP
#define TICTACTOE_HPP

#include <dpp/dpp.h>
#include <sstream>



class ttt
{
private:
	// origin message id of this ttt
	dpp::snowflake origin;  

    int uid;
    dpp::snowflake player_1;
    dpp::snowflake player_2;

public:
    struct confirm_button
    {
    private:
        std::pair<int, int> id;
        std::pair <std::string, std::string> btn_id;

        dpp::snowflake user_id;

        void init(const dpp::snowflake user_id, int uid_y, int uid_n);
    public:
        dpp::component component;
       
        const dpp::snowflake const get_user();
        const std::pair<int, int> const get_id();
        std::pair<std::string, std::string> get_button_id();

        
        void set_id(std::pair<int, int> nid);
        void set_btn_id(std::pair<int, int> nid);

        confirm_button(const dpp::snowflake user_id, int uid_y, int uid_n);
    };
	// --- Constructors ---
	// --- Constructors ---
    ttt(int uid);
	ttt();

	// --- Destructors ---
    ~ttt();

	// --- Accessors ---
	const dpp::snowflake& get_origin() const { return this->origin;  }

	// --- Modifiers ---
	void set_origin(dpp::snowflake& message_id) { this->origin = message_id; }

	// --- Functions ---
	void init();
};


#endif