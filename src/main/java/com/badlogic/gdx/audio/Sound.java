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
package com.badlogic.gdx.audio;

import com.badlogic.gdx.Audio;
import com.badlogic.gdx.files.FileHandle;

/**
 * <p>A Sound is a short audio clip that can be played numerous times in parallel.
 * It's completely loaded into memory so only load small audio files. Call the 
 * {@link #dispose()} method when you're done using the Sound.</p>
 * 
 * <p>Sound instances are created via a call to {@link Audio#newSound(FileHandle)}.</p>
 * 
 * @author badlogicgames@gmail.com
 * 
 */
public interface Sound {
	/**
	 * Plays the sound, you can call this repeatedly to play the same sound a
	 * couple of times with a bit of lag introduced.
	 */
	public void play();

	/**
	 * Plays the sound, you can call this repeatedly to play the same sound a
	 * couple of times with a bit of lag introduced.
	 * 
	 * @param volume
	 *            the volume in the range [0,1]
	 */
	public void play(float volume);
	
	public void stop();
	
	public void setVolume(float vol);

	/**
	 * Releases all the resources.
	 */
	public void dispose();
}
