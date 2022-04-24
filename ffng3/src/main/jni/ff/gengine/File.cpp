/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "File.h"

#include "Log.h"
#include "OptionAgent.h"
#include "Dialog.h"

#include "FFNGFiles.h"
#include <android/log.h>

#include <utility>

/*
 * Tries to use path to a localized resource if it exists.
 */
File
File::internal(const std::string &path)
{
    std::string localizedPath = localizePath(path);
    File localized = File(localizedPath, FileType::internal);

    if (localizedPath == path) {
        return localized;
    } else if (localized.exists()) {
        LOG_INFO(ExInfo("localizedPath")
                         .addInfo("path", localized.getPath()));
        return localized;
    } else {
        return File(path, FileType::internal);
    }
}

File
File::external(const std::string &path)
{
    return File(path, FileType::external);
}

//-----------------------------------------------------------------
/**
 * Return path to a localized resource or the original path.
 * The localized path has format: <name>__<lang>.<extension>
 *
 * @param original original path
 * @return localized path if it is meaningful or the original path
 */
std::string
File::localizePath(const std::string &original)
{
    std::string::size_type dotPos = original.rfind('.');
    if (dotPos == std::string::npos) {
        return original;
    }

    std::string lang = OptionAgent::agent()->getParam("lang");
    if (Dialog::DEFAULT_LANG == lang || lang.empty()) {
        return original;
    }

    std::string appendix = "__" + lang;

    std::string::size_type dirPos = original.rfind('/');
    if (dirPos != std::string::npos && dotPos < dirPos) {
        return original;
    }

    std::string path = original;
    path.insert(dotPos, appendix);
    return path;
}

//-----------------------------------------------------------------
bool
File::exists() const
{
	return FFNGFiles::exists(m_path, m_type == FileType::internal ? 1 : 2);
}

//-----------------------------------------------------------------
std::string
File::read() const
{
	return FFNGFiles::read(m_path,  m_type == FileType::internal ? 1 : 2);
}

//-----------------------------------------------------------------
bool
File::write(const std::string &data) const
{
    FFNGFiles::createPath(m_path);
	return FFNGFiles::write(m_path, data);
}