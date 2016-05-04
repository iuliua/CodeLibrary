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
        time_t trial_time = mktime(&time_struct);

        return time(0) > trial_time;
    }
};