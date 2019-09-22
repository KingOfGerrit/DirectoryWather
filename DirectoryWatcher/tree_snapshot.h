#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>
#include <functional>

namespace fs = std::filesystem;

enum class Status { created, modified, erased };

class TreeSnapshot {
public:
	TreeSnapshot(std::string path) :
		m_path{ path }
	{
		for (auto &file : fs::recursive_directory_iterator(m_path))
		{
			m_content[_getPath(file)] = fs::last_write_time(file);
		}
	}

	void checkUpdates(const std::function<void(std::string, Status)> &action)
	{
		// Check if a file was erased
		auto it = m_content.begin();
		while (it != m_content.end()) {
			if (!fs::exists(it->first))
			{
				action(it->first, Status::erased);
				it = m_content.erase(it);
			}
			else
			{
				it++;
			}
		}

		// Check if a file was created or modified
		for (auto &file : fs::recursive_directory_iterator(m_path))
		{
			auto currentFileLastWriteTime = fs::last_write_time(file);

			// File creation
			if (!_contains( _getPath(file) ))
			{
				m_content[_getPath(file)] = currentFileLastWriteTime;
				action(_getPath(file), Status::created);
				// File modification
			}
			else
			{
				if (m_content[_getPath(file)] != currentFileLastWriteTime)
				{
					m_content[_getPath(file)] = currentFileLastWriteTime;
					action(_getPath(file), Status::modified);
				}
			}
		}
	}

private:
	std::string m_path;

	std::unordered_map<std::string, fs::file_time_type> m_content;

private:
	inline std::string _getPath(const fs::directory_entry &file)
	{
		return file.path().string();
	}

	inline bool _contains(const std::string &key)
	{
		auto el = m_content.find(key);
		return el != m_content.end();
	}
};
