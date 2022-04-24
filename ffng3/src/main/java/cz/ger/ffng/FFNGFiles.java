package cz.ger.ffng;

import android.util.Log;

import com.badlogic.gdx.Files.FileType;
import com.badlogic.gdx.files.FileHandle;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.HashSet;

public class FFNGFiles {
	public final static int INTERNAL = 1;

	protected static final HashSet<String> internalFileList = new HashSet<String>();

    public static void createCache() {
		readFileList(internalFileList, "filelist.txt");
	}
	
	protected static void readFileList(HashSet<String> fileList, String sourceFile) {
		String list;
		try {
			list = readStream(FFNG.files.getFileHandle(sourceFile, FileType.Internal).read());
		} catch (IOException e) {
			Log.e("FFNG", "Unable to load " + sourceFile + " to cache the internal file list");
			list = "";
		}
		String[] splittedList = list.split("\n");
		for (String s : splittedList) {
			if (!s.trim().equals("")) {
				fileList.add(s.trim());
			}
		}
	}
	
	static public FileHandle getFileHandle(String file, int type) throws IOException {
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
	
	static public boolean exists(String path, int type) {
		return internalFileList.contains(path);
	}
	
	static public String read(String path, int type) {
		Log.d("FFNG", "FFNGFiles::read " + path);
		try {
			return readStream(getFileHandle(path, type).read());
		} catch(Exception ex) {
			return "";
		}
	}
	
	static public String readStream(InputStream is) throws IOException {
	    /*
	     * To convert the InputStream to String we use the
	     * Reader.read(char[] buffer) method. We iterate until the
	     * Reader return -1 which means there's no more data to
	     * read. We use the StringWriter class to produce the string.
	     */
	    if (is != null) {
	        Writer writer = new StringWriter();
	
	        char[] buffer = new char[1024];
	        try {
	            Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"), 1024);
	            int n;
	            while ((n = reader.read(buffer)) != -1) {
	                writer.write(buffer, 0, n);
	            }
	        } catch(Exception ex) {
	        	return "";
	        } finally {
	        	try {
	        		is.close();
	        	} catch(Exception ex) {
	        	}
	        }
	        return writer.toString();
	    } else {        
	        return "";
	    }
	}
}
