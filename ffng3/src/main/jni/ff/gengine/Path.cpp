/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "Path.h"

#include "Log.h"
#include "OptionAgent.h"
#include "Dialog.h"

#include <stdio.h>
#include "FFNGFiles.h"
#include <android/log.h>

//-----------------------------------------------------------------
    Path::Path(const std::string &file, const FileType &type)
: m_path(file),
  m_type(type)
{
    /* empty */
}
/**
 * Return path to system file.
 * Path does not need to exist.
 */
    Path
Path::dataSystemPath(const std::string &file)
{
    return constructPath(file, INTERNAL);
}
//-----------------------------------------------------------------
/**
 * Return path to user file.
 * Path does not need to exist.
 */
    Path
Path::dataUserPath(const std::string &file)
{
    return constructPath(file, EXTERNAL);
}
//-----------------------------------------------------------------
/**
 * Create path to the given file in the given directory.
 * Tries to use path to a localized resource if it exists.
 */
    Path
Path::constructPath(const std::string &file, const FileType &type)
{
    std::string localized = localizePath(file);
    Path localizedPath = Path(localized, type);

    if (localized == file) {
        return localizedPath;
    } else if (localizedPath.exists()) {
        LOG_INFO(ExInfo("localized")
                .addInfo("path", localizedPath.getNative()));
        return localizedPath;
    } else {
        return Path(file, type);
    }
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
Path::localizePath(const std::string &original)
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
std::string
Path::getNative() const
{
    return m_path;
}
//-----------------------------------------------------------------
bool
Path::exists() const
{
	return FFNGFiles::exists(m_path, m_type);
}
//-----------------------------------------------------------------
std::string
Path::read() const
{
	return FFNGFiles::read(m_path, m_type);
}
//-----------------------------------------------------------------
bool
Path::write(const std::string &data) const
{
    FFNGFiles::createPath(dataSystemPath(m_path).getPosixName());
	return FFNGFiles::write(m_path, data);
}