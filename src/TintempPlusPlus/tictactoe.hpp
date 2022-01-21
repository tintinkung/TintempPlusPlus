#pragma once

/* * tic-tac-toe game manager class
 * manages ttt game command and everything.
 */
class ttt
{
private:
    /* unique id of this class */
    int uid;

	/* origin message id of this ttt */ 
	dpp::message origin;  
    /* tic-tac-toe game message id the event deployed */
    dpp::message game;
    /* pair of this turn's player id and player's character(X, O) */
    std::pair< dpp::snowflake, int > turn;

    /* challenger playing this game */
    dpp::snowflake player_1; dpp::snowflake player_2;

    /* class initializer */
    void init();
    void init_uid(int &uid) { this->uid = uid; };

public:
// --- Variables ---
    /* * is this ttt class have origin message assigned? */
    bool have_origin = false;
    /* * result of this tic-tac-toe game [ENUM]*/
    enum RESULT { IN_PROGRESS, EXPIRED, DRAW, WON };
    RESULT result;
    /* * turns count of tic-tac-toe game
     * increment each time swap_turn() is called
     * used to escape out of game if turn is maxed out to 9
     */
    int turns = 0;

    /* * 2 button component sent when user first asked for ttt command
     * the component send confirm message with confirm_button attaches
     */
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
    // --- constructor ---
        confirm_button(const dpp::snowflake player_1, const dpp::snowflake player_2);
    };
    /* * sub memmber of board array
     * stores stat of each pair in tic-tac-toe board
     */
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

    // --- constructor ---
        pair(std::string pos);
        pair() = default; // make it default
    };
    /* * base 3X3 tic-tac-toe board
     * made with array of pair sub member above
     * this will be initilize on init_game()
     */
    std::array<std::array<pair, 3>, 3> board;


// --- Constructors ---
    /* @brief tic-tac-toe class constructors
     * @param player_1 : the first player that called this command
     * @param player_2 : the player first-player mentioned to play with
     */
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
    void set_game_origin(dpp::message origin_message) { this->game = origin_message; }


// --- Functions ---

    /* @brief tic-tac-toe game initializer 
     * this initialize game's board and pair inside it
     * as well as button components for the board
     */
    void init_game();

    /* @brief call swap turn 
     * call when the tic-tac-toe turn is finished by the event
     */
    void swap_turn();

    /* @brief fetch winner of this tic-tac-toe game
     * @return void, edited this class's status and pair 
     */
    void fetch_winner();

    /* @brief get this turn of tic-tac-toe game's player data
     * @return pair of this turn's player id and player's character ENUM(X or O)
     */
    std::pair< dpp::snowflake, int >& this_turn() { return this->turn; };

    /* @brief fetch message to match this turn's data
     * called after swap_turn() is called and components is edited
     * @return dpp::message : message callback that can be assign as new fetch-ed message
     */
    dpp::message& fetch_ttt_turn(dpp::message& msg_to_fetch);

    /* @brief fetch new component of tic-tac-toe-game
     * called after swap_turn() is called 
     * @return tuple of 3 component : first row, second row and third row of board's buttons
     */
    std::tuple<dpp::component, dpp::component, dpp::component> fetch_ttt_components(std::array<std::array<ttt::pair, 3>, 3> board);
};

#pragma region tic-tac-toe game's event
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

/* * event when tic-tac-toe game's board is deployed
 * happens on_button_click() event of board's buttons
 * this event changes the ttt board's with game's content and finish the game deploy message
 */
void on_ttt_interaction(dpp::cluster& bot, const dpp::button_click_t& event);
#pragma endregion