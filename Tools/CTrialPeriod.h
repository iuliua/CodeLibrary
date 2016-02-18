class CTrialPeriod
{
    time_t      m_trial_time;
    bool        m_is_full_version;
    std::string m_trial_end_date;
public:
    CTrialPeriod(const std::string &trial_end_date)
        :m_trial_end_date(trial_end_date),
        m_is_full_version(false)
    {
        // trial end date MUST be in format DD-MM-YYYY
        struct tm time_struct = { 0 };
        sscanf_s(trial_end_date.c_str(), "%2d-%2d-%4d", &time_struct.tm_mday, &time_struct.tm_mon, &time_struct.tm_year);
        time_struct.tm_mon -= 1;
        time_struct.tm_year -= 1900;
        m_trial_time = mktime(&time_struct);
    }
    CTrialPeriod()
        :m_is_full_version(true)
    {

    }
    inline bool expired()
    {
        if (m_is_full_version)
            return false;
        return time(0) > m_trial_time;
    }
};