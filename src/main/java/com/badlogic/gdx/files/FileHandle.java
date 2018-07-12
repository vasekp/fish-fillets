/*
 * Copyright 2010 Mario Zechner (contact@badlogicgames.com), Nathan Sweet (admin@esotericsoftware.com)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS"
 * BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
 */
package com.badlogic.gdx.files;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import com.badlogic.gdx.Files;
import com.badlogic.gdx.Files.FileType;
import com.badlogic.gdx.utils.GdxRuntimeException;

/**
 * Represents a file or directory on the filesystem, classpath, Android SD card, or Android assets directory. FileHandles are
 * created via a {@link Files} instance.
 * @author mzechner
 * @author Nathan Sweet <misc@n4te.com>
 */
public abstract class FileHandle {
	protected File file;
	protected FileType type;

	protected FileHandle () {
	}

	public String path () {
		return file.getPath();
	}

	public String name () {
		return file.getName();
	}

	public FileType type () {
		return type;
	}

	/**
	 * Returns a stream for reading this file.
	 * @throw GdxRuntimeException if this file handle represents a directory, if it is a {@link FileType#Classpath} or
	 *        {@link FileType#Internal} and the file doesn't exist, or if it could not be read.
	 */
	public InputStream read () {
		if (type == FileType.Internal && !file.exists()) {
			InputStream input = FileHandle.class.getResourceAsStream(file.getPath().replace('\\', '/'));
			if (input == null) throw new GdxRuntimeException("File not found: " + file + " (" + type + ")");
			return input;
		}
		try {
			return new FileInputStream(file);
		} catch (FileNotFoundException ex) {
			throw new GdxRuntimeException("Error reading file: " + file + " (" + type + ")", ex);
		}
	}

	/**
	 * Returns a stream for writing to this file.
	 * @param append If false, this file will be overwritten if it exists, otherwise it will be appended.
	 * @throw GdxRuntimeException if this file handle represents a directory, if it is a {@link FileType#Classpath} or
	 *        {@link FileType#Internal} file, or if it could not be written.
	 */
	public OutputStream write (boolean append) {
		if (type == FileType.Internal) throw new GdxRuntimeException("Cannot write to an internal file: " + file);
		try {
			return new FileOutputStream(file, append);
		} catch (FileNotFoundException ex) {
			throw new GdxRuntimeException("Error writing file: " + file + " (" + type + ")", ex);
		}
	}

	/**
	 * Returns the paths to the children of this directory. Returns an empty list if this file handle represents a file and not a
	 * directory.
	 * @throw GdxRuntimeException if this file is an {@link FileType#Classpath} file.
	 */
	public FileHandle[] list () {
		String[] relativePaths = file.list();
		if (relativePaths == null) return new FileHandle[0];
		FileHandle[] handles = new FileHandle[relativePaths.length];
		for (int i = 0, n = relativePaths.length; i < n; i++)
			handles[i] = child(relativePaths[i]);
		return handles;
	}

	/**
	 * Returns true if this file is a directory. Always returns false for classpath files. On Android, an internal handle to an
	 * empty directory will return false.
	 */
	public boolean isDirectory () {
		return file.isDirectory();
	}

	/**
	 * Returns a handle to the child with the specified name.
	 * @throw GdxRuntimeException if this file handle is a {@link FileType#Classpath} or {@link FileType#Internal} and the child
	 *        doesn't exist.
	 */
	abstract public FileHandle child (String name);

	abstract public FileHandle parent ();

	/**
	 * @throw GdxRuntimeException if this file handle is a {@link FileType#Classpath} or {@link FileType#Internal} file.
	 */
	public void mkdirs () {
		if (type == FileType.Internal) throw new GdxRuntimeException("Cannot mkdirs with an internal file: " + file);
		file.mkdirs();
	}

	public boolean exists () {
		// Classpath and internal FileHandles can't be created unless they exist.
		if (type == FileType.Internal) return true;
		return file.exists();
	}

	public String toString () {
		return file.getPath();
	}

}
