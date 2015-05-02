class Tools
{
public:
    // set current directory to gateway's executable folder location
	static void SetCurrDirToModuleLocation(LPCWSTR folder=NULL)
	{
		WCHAR buffer[MAX_PATH];
		GetModuleFileName(GetModuleHandle(NULL), buffer, _countof(buffer));
		std::wstring module_name(buffer);
		module_name = module_name.substr(0, module_name.rfind(L'\\'));
        if (folder)
            module_name = module_name + L"\\" + std::wstring(folder);
		SetCurrentDirectory(module_name.c_str());
	}
	static UINT GetNumberOfDigits(double number)
	{
		char buf[32];
		char* pointer = buf;
		UINT digits = 0;
		bool start_counting = false;
		sprintf_s(buf, 32, "%f", number);
		while (*pointer)
		{
			if (*pointer == '.')
			{
				start_counting = true;
				pointer++;
				digits++;
				continue;
			}
			if (*pointer == '0')
			{
				if (start_counting)
					digits++;
				pointer++;
			}
			else
				return digits;
		}
		return 0;
	}
    static const std::wstring StringToWString(const std::string& s)
    {
        wchar_t dst[128];
        if (!::MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, dst, _countof(dst)))
            dst[0] = 0;
        return dst;
    }
    static std::string WStringToString(const std::wstring& s)
    {
        char dst[128];
        if (!::WideCharToMultiByte(CP_ACP, 0, s.c_str(), -1, dst, _countof(dst), NULL, NULL))
            dst[0] = 0;
        return dst;
    }
    inline static bool DoubleEqual(double d1, double d2)
    {
        return (abs(d1 - d2) < 0.0001);
    }
    inline static bool DoubleNotZero(double d1)
    {
        return !DoubleEqual(d1, 0);
    }
    inline static bool DoubleEqualZero(double d1)
    {
        return DoubleEqual(d1, 0);
    }
};