/// @file   CDockingStateReader.cpp
/// @author Uwe Kindler
/// @date   29.11.2019
/// @brief  Implementation of CDockingStateReader

#include "CDockingStateReader.h"

namespace ads
{

void CDockingStateReader::setFileVersion(int FileVersion)
{
	m_FileVersion = FileVersion;
}

int CDockingStateReader::fileVersion() const
{
	return m_FileVersion;
}
}// namespace ads

// EOF CDockingStateReader.cpp
