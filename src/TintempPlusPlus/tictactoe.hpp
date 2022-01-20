#pragma once

/* * tic-tac-toe game manager class
 * manages ttt game command and everything.
 */
class ttt
{
private:
	/* origin message id of this ttt */ 
	dpp::message origin;  
    dpp::message game;
    /* pair of this turn's player id and player's character(X, O) */
    std::pair< dpp::snowflake, int > turn;

    int uid;
    dpp::snowflake player_1;
    dpp::snowflake player_2;

    void init_uid(int &uid) { this->uid = uid; };
    void init();
public:
    bool have_origin = false;
    bool have_game = false;
    enum RESULT { IN_PROGRESS, EXPIRED, DRAW, WON };
    RESULT result;
    int turns = 0;


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
        struct status {
            bool is_player_1_confm = false;
            bool is_player_2_confm = false;
            bool replied = false;
            bool cancelled = false;
        }; status st;
       
        const std::pair<dpp::snowflake, dpp::snowflake> get_user();
        const std::pair<int, int> get_id();
        std::pair<std::string, std::string> get_button_id();
        dpp::message get_replied_msg() { return this->replied_msg; }

        
        void set_id(std::pair<int, int> nid);
        void set_btn_id(std::pair<int, int> nid);
        void set_replied_msg(dpp::message replied_msg) { this->replied_msg = replied_msg; }

        confirm_button(const dpp::snowflake player_1, const dpp::snowflake player_2);
    };

    struct pair
    {
    private:
        std::string id;

        void init(std::string& pos);
    public:
        enum status_t { IDLE, X, O };
        int status = IDLE;
        bool won = false;

        const std::string get_id() const { return this->id; }
        void set_status(int stats) { status = stats; }

        // constructor
        pair(std::string pos);
        pair() = default; // make it default
    };

    std::array<std::array<pair, 3>, 3> board;


	// --- Constructors ---
    ttt(const dpp::snowflake player_1, const dpp::snowflake player_2);

	// --- Destructors ---
    ~ttt();

	// --- Accessors ---
	dpp::message& get_origin() { return this->origin;  }
    dpp::message& get_game() { return this->game; }
    const std::pair<dpp::snowflake, dpp::snowflake> get_player() const { return std::make_pair(this->player_1, this->player_2); }
    const int& get_uid() const { return this->uid; }

	// --- Modifiers ---
    void set_origin(dpp::message origin_message) { this->origin = origin_message; this->have_origin = true; }
    void set_origin_embed(dpp::embed &new_embed) { this->origin.embeds.front() = new_embed; }
    void set_game_origin(dpp::message origin_message) { this->game = origin_message; this->have_game = true; }


	// --- Functions ---
    void init_game();
    std::pair< dpp::snowflake, int >& this_turn() { return this->turn; };
    void swap_turn();
    dpp::message& fetch_ttt_turn(dpp::message& msg_to_fetch);
    std::tuple<dpp::component, dpp::component, dpp::component> fetch_ttt_components(std::array<std::array<ttt::pair, 3>, 3> board);
    void fetch_winner();
};


// ===== tictactoe global queue cataching  ======

/* * push new tic-tac-toe game operation to global list and local data for usage.
 *
 * @param player_1 : the first player challenging ttt game (this should be user that call the command)
 * @param player_2 : second player playing with player 1 (this should be mentioned player by player_1's command)
 * 
 * @return results of operation as std::pair<ttt, ttt::confirm_button>, can be used locally 
 */
std::pair<ttt, ttt::confirm_button> push_onto_list(const dpp::snowflake player_1, const dpp::snowflake player_2);

/* * erase 1 data from the ttt global list. (occured when 1 of the ttt command is done)
 * @param i the iterator of `ttt_list_g` list
 */
void erase_one_from_list(std::list<std::pair<ttt, ttt::confirm_button>>::iterator i);

/* * search for ttt class by unique id and set its message origin.
 * (since message origin is deployed by bot.message_create callback, 
 * this command for assigning callback message from local scope out to the global list)
 *
 * @param deployed_msg : deployed message wanted to assign to this ttt id
 * @param id : unique id of ttt class to search for
 */
void set_origin(dpp::message deployed_msg, int id);

/* * event when player clicked the confirm button sent 
 * this event response with confirm message by player's interaction 
 * and reach out to deploying actual tic-tac-toe game
 */
void on_confirm_click(dpp::cluster& bot, const dpp::button_click_t& event);

void on_ttt_interaction(dpp::cluster& bot, const dpp::button_click_t& event);