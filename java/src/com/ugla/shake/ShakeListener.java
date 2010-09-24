package com.ugla.shake;

/**	This is the interface classes should implement to receive events from the SHAKE. The parameter
 * will be one of the constants defined in ShakeEvents */
public interface ShakeListener {
	public void ShakeEvent(int type);
}
