template<class T>
void CSystem::PrintMemoryUsageForName(const char* name, IFFont* pFont, typename Sizer<T>::Func fn, T* This, float px, float py)
{
	static char  stats[256];
	CrySizerImpl sizer;
	SIZER_COMPONENT_NAME(&sizer, name);
	(This->*fn)(&sizer);
	auto len   = sprintf(stats, "%-15.15s %-14d", name, sizer.GetTotalSize());
	stats[len] = 0;

	PrintRightAlignedText(py, stats, pFont);
}

