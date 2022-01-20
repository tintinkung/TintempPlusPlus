#pragma once

namespace util
{
	enum color {
		AQUA = 0x00FFFF,
		BLACK = 0x000000,
		WHITE = 0xFFFFFF,
		YELLOW = 0xFFFF00,
		RED = 0xFF0000,
		GREEN = 0x00FF00,
		BLUE = 0x0000FF,
	};
}

/* @brief logger class for logging in console
 * using by logger::<type>(<message>)
 */
namespace logger
{
	/* log level enum, specify level of logging */
	enum level { ll_comment, ll_trace, ll_info, ll_debug, ll_error, ll_warning, ll_critical };

	/* default log level for global console logging */
	struct config_t {
		int log_level = ll_comment; 
		bool show_date = true;
	}; 
	static config_t config;


	/* @brief log by level,
	 * [LEVEL]: message
	 * @param level : level by enum, support built in dpp::loglevel and util::logger::level
	 */
	void log(logger::level level, std::string message);
	void log(dpp::loglevel level, std::string message);

	/* @brief log with info label, 
	 * [INFO]: message
	 */
	void info(std::string message);

	/* @brief log with debug label, 
	 * [DEBUG}: message
	 */
	void debug(std::string message);

	/* @brief log with gray-like-comment info label, 
	 * [ERROR]: message
	 */
	void comment(std::string message);

	/* @brief log with error label, 
	 * [INFO]: message
	 */
	void error(std::string message);

	/* @brief log with warning label, 
	 * [WARN}: message
	 */
	void warning(std::string message);

	/* @brief log with critical label, 
	 * [CRIT]: message
	 */
	void critical(std::string message);

	/* @brief log with trace label, 
	 * [TRACE]: message
	 */
	void trace(std::string message);
}






