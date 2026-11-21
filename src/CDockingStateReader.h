#pragma once

/// @file   CDockingStateReader.h
/// @author Uwe Kindler
/// @date   29.11.2019
/// @brief  Declaration of CDockingStateReader.

#include <QXmlStreamReader>

namespace ads
{

/**
 * @brief Extends QXmlStreamReader with file version information.
 */
class CDockingStateReader : public QXmlStreamReader
{
	public:
		using QXmlStreamReader::QXmlStreamReader;

		/**
		 * @brief Set the file version for this state reader.
		 */
		void setFileVersion(int FileVersion);

		/**
		 * @brief Returns the file version set via setFileVersion.
		 */
		int fileVersion() const;

	private:
		/// @brief The file version.
		int m_FileVersion;
};

}// namespace ads
