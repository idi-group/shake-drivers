package SHAKE;

import java.util.Vector;
public class Splitter {

	public static Vector split(String s, String delim) {
		Vector v = new Vector(2);
		
		String cur = s;
		while(true) {
			int curindex = cur.indexOf(delim);
			if(curindex == -1) {
				v.addElement(cur.substring(0));
				break;
			}
			v.addElement(cur.substring(0, curindex));
			cur = cur.substring(curindex + delim.length());
		}
		
		return v;
	}
	
}
