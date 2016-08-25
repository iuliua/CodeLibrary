class CTrialPeriod
{
public:
    inline static bool Expired(int year, int month, int day)
    {
        if (year == 0)
            return false;
        struct tm time_struct = { 0 };
        time_struct.tm_year = year - 1900;
        time_struct.tm_mon = month - 1;
        time_struct.tm_mday = day;

        return time(0) > mktime(&time_struct);
    }
    //format expected is YYYY-MM-DD
    inline static bool Expired(const std::string &date)
    {
        if (date.empty())
            return FALSE;
        std::istringstream iss(date);
        std::string year, month, day;
        std::getline(iss, year, '-');
        std::getline(iss, month, '-');
        std::getline(iss, day, '-');
        struct tm time_struct = { 0 };
        time_struct.tm_year = atoi(year.c_str()) - 1900;
        time_struct.tm_mon = atoi(month.c_str()) - 1;
        time_struct.tm_mday = atoi(day.c_str());
        time_struct.tm_hour = 23;
        time_struct.tm_min = 59;
        time_struct.tm_sec = 59;

        return time(0) > mktime(&time_struct);
    }
};