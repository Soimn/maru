unsigned int
CString_Len(char* cstring)
{
	unsigned int result = 0;
	for (char* scan = cstring; *scan; ++scan) ++result;
	return result;
}

bool
CString_Match(char* a, char* b)
{
	bool result = false;
	
	if (a == b) result = true;
	else
	{
		char* scan_a = a;
		char* scan_b = b;

		for (;;)
		{
			if (*scan_a != *scan_b) break;
			else
			{
				if (*scan_a == 0)
				{
					result = true;
					break;
				}
				else
				{
					++scan_a;
					++scan_b;
					continue;
				}
			}
		}
	}

	return result;
}

bool
CString_MatchNChars(char* a, char* b, unsigned n)
{
	bool result = false;
	
	if (a == b) result = true;
	else
	{
		char* scan_a = a;
		char* scan_b = b;

		result = true;
		for (unsigned int i = 0; i < n; ++i)
		{
			if (*scan_a == 0 || *scan_a != *scan_b)
			{
				result = false;
				break;
			}
			else
			{
				++scan_a;
				++scan_b;
			}
		}

		result = (result && *scan_a == *scan_b);
	}

	return result;
}

bool
CString_HasPrefix(char* string, char* prefix)
{
	bool result = false;

	if (prefix == string) result = true;
	else
	{
		char* scan_p = prefix;
		char* scan_s = string;

		result = true;
		while (*scan_p != 0)
		{
			if (*scan_p != *scan_s)
			{
				result = false;
				break;
			}
			else
			{
				++scan_p;
				++scan_s;
				continue;
			}
		}
	}

	return result;
}
