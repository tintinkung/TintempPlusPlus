#include "pch.h"
#include "utility.h"


#pragma region logger
void log_by_level(logger::level level, std::string& message);
void log_by_level(dpp::loglevel level, std::string& message);

void logger::log(logger::level level, std::string message)
{
	log_by_level(level, message);
}

void logger::log(dpp::loglevel level, std::string message)
{
	log_by_level(level, message);
}

void logger::info(std::string message)
{
	log_by_level(level::ll_info, message);
}

void logger::debug(std::string message)
{
	log_by_level(level::ll_debug, message);
}

void logger::error(std::string message)
{
	log_by_level(level::ll_error, message);
}

void logger::comment(std::string message)
{
	log_by_level(level::ll_comment, message);
}

void logger::warning(std::string message)
{
	log_by_level(level::ll_warning, message);
}

void logger::critical(std::string message)
{
	log_by_level(level::ll_critical, message);
}

void logger::trace(std::string message)
{
	log_by_level(level::ll_trace, message);
}

void log_by_level(logger::level level, std::string &message)
{
	auto date = dpp::utility::current_date_time();
	/* the dpp date utility concat a new line at the end of the date string but we dont want it*/
	date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

	switch (level)
	{
		case logger::level::ll_comment:
		{
			fmt::print("{}{}{}{} {}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::dim_gray) | fmt::emphasis::italic, "["),
				format(fg(fmt::color::dim_gray) | fmt::emphasis::italic, dpp::utility::loglevel(dpp::loglevel::ll_info)),
				format(fg(fmt::color::dim_gray) | fmt::emphasis::italic, "] :"),
				format(fg(fmt::color::dim_gray) | fmt::emphasis::italic, message));
			break;
		}
		case logger::level::ll_info:
		{
			fmt::print("{}{}{}{} {}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::orange) | fmt::emphasis::bold, "["),
				format(fg(fmt::color::gold)   | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_info)),
				format(fg(fmt::color::orange) | fmt::emphasis::bold, "] :"),
				format(fg(fmt::color::honey_dew), message));
			break;
		}
		case logger::level::ll_trace:
		{
			fmt::print("{}{}{}{} {}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, "["),
				format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, dpp::utility::loglevel(dpp::loglevel::ll_trace)),
				format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, "]:"),
				format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, message));
			break;
		}
		case logger::level::ll_debug:
		{
			fmt::print("{}{}{}{} {}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::cornflower_blue) | fmt::emphasis::bold, "["),
				format(fg(fmt::color::aquamarine)      | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_debug)),
				format(fg(fmt::color::cornflower_blue) | fmt::emphasis::bold, "]:"),
				format(fg(fmt::color::light_pink), message));
			break;
		}
		case logger::level::ll_error:
		{
			fmt::print("{}{}{}{} {}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::fire_brick)| fmt::emphasis::bold, "["),
				format(fg(fmt::color::crimson)   | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_error)),
				format(fg(fmt::color::fire_brick)| fmt::emphasis::bold, "]:"),
				format(fg(fmt::color::red)       | fmt::emphasis::italic, message));
			break;
		}
		case logger::level::ll_warning:
		{
			fmt::print("{}{}{}{} {}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::deep_pink)| fmt::emphasis::bold, "["),
				format(fg(fmt::color::hot_pink) | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_warning)),
				format(fg(fmt::color::deep_pink)| fmt::emphasis::bold, "] :"),
				format(fg(fmt::color::salmon)   | fmt::emphasis::italic, message));
			break;
		}
		case logger::level::ll_critical:
		{
			fmt::print("{}{}{}{}{}\n",
				logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
				format(fg(fmt::color::ghost_white) | fmt::emphasis::strikethrough | bg(fmt::color::dark_red), "["),
				format(fg(fmt::color::ghost_white) | fmt::emphasis::strikethrough | bg(fmt::color::dark_red), dpp::utility::loglevel(dpp::loglevel::ll_critical)),
				format(fg(fmt::color::ghost_white) | fmt::emphasis::strikethrough | bg(fmt::color::dark_red), "] :"),
				format(fg(fmt::color::light_golden_rod_yellow) | fmt::emphasis::strikethrough | bg(fmt::color::crimson), message));
			break;
		}

		default:
			break;
	}

}

void log_by_level(dpp::loglevel level, std::string& message)
{
	auto date = dpp::utility::current_date_time();
	/* the dpp date utility concat a new line at the end of the date string but we dont want it*/
	date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

	switch (level)
	{
	case dpp::loglevel::ll_info:
	{
		fmt::print("{}{}{}{} {}\n",
			logger::config.show_date? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
			format(fg(fmt::color::orange) | fmt::emphasis::bold, "["),
			format(fg(fmt::color::gold)   | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_info)),
			format(fg(fmt::color::orange) | fmt::emphasis::bold, "] :"),
			format(fg(fmt::color::honey_dew), message));
		break;
	}
	case dpp::loglevel::ll_trace:
	{
		fmt::print("{}{}{}{} {}\n",
			logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
			format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, "["),
			format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, dpp::utility::loglevel(dpp::loglevel::ll_trace)),
			format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, "]:"),
			format(fg(fmt::color::dark_gray) | fmt::emphasis::italic, message));
		break;
	}
	case dpp::loglevel::ll_debug:
	{
		fmt::print("{}{}{}{} {}\n",
			logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
			format(fg(fmt::color::cornflower_blue) | fmt::emphasis::bold, "["),
			format(fg(fmt::color::aquamarine)      | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_debug)),
			format(fg(fmt::color::cornflower_blue) | fmt::emphasis::bold, "]:"),
			format(fg(fmt::color::light_pink), message));
		break;
	}
	case dpp::loglevel::ll_error:
	{
		fmt::print("{}{}{} {}\n",
			logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
			format(fg(fmt::color::fire_brick)| fmt::emphasis::bold, "["),
			format(fg(fmt::color::crimson)   | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_error)),
			format(fg(fmt::color::fire_brick)| fmt::emphasis::bold, "]:"),
			format(fg(fmt::color::red)       | fmt::emphasis::italic, message));
		break;
	}
	case dpp::loglevel::ll_warning:
	{
		fmt::print("{}{}{}{} {}\n",
			logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
			format(fg(fmt::color::deep_pink) | fmt::emphasis::bold, "["),
			format(fg(fmt::color::hot_pink)  | fmt::emphasis::bold, dpp::utility::loglevel(dpp::loglevel::ll_warning)),
			format(fg(fmt::color::deep_pink) | fmt::emphasis::bold, "] :"),
			format(fg(fmt::color::salmon)    | fmt::emphasis::italic, message));
		break;
	}
	case dpp::loglevel::ll_critical:
	{
		fmt::print("{}{}{}{}{}\n",
			logger::config.show_date ? format(fg(fmt::color::dim_gray) | fmt::emphasis::bold, fmt::format("[{}]", date)) : "",
			format(fg(fmt::color::ghost_white) | fmt::emphasis::strikethrough | bg(fmt::color::dark_red), "["),
			format(fg(fmt::color::ghost_white) | fmt::emphasis::strikethrough | bg(fmt::color::dark_red), dpp::utility::loglevel(dpp::loglevel::ll_critical)),
			format(fg(fmt::color::ghost_white) | fmt::emphasis::strikethrough | bg(fmt::color::dark_red), "] :"),
			format(fg(fmt::color::light_golden_rod_yellow) | fmt::emphasis::strikethrough | bg(fmt::color::crimson), message));
		break;
	}

	default:
		break;
	}
}
#pragma endregion