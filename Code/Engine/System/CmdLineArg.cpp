#include "CmdLineArg.hpp"

CCmdLineArg::CCmdLineArg(const char* name, const char* value, ECmdLineArgType type)
{
	m_name  = name;
	m_value = value;
	m_type  = type;
}

CCmdLineArg::~CCmdLineArg()
{
}

const char* CCmdLineArg::GetName() const
{
	return m_name.c_str();
}
const char* CCmdLineArg::GetValue() const
{
	return m_value.c_str();
}
const ECmdLineArgType CCmdLineArg::GetType() const
{
	return m_type;
}
const float CCmdLineArg::GetFValue() const
{
	return (float)atof(m_value.c_str());
}
const int CCmdLineArg::GetIValue() const
{
	return atoi(m_value.c_str());
}