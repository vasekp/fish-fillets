package cz.ger.ffng;

import android.util.Log;

import com.badlogic.gdx.Files.FileType;
import com.badlogic.gdx.files.FileHandle;

import java.io.IOException;

public class FFNGFiles {
	static public FileHandle getFileHandle(String file) throws IOException {
		FileHandle fh = null;
		try {
			fh = FFNG.files.getFileHandle(file, FileType.Internal);
		} catch(Exception e) {
			throw new IOException("File doesn't exist: " + file);
		} catch(NoSuchMethodError e) {
			Log.d("FFNG", "FFNGFiles" + e.getMessage());
		}
		
		return fh;
	}

}
