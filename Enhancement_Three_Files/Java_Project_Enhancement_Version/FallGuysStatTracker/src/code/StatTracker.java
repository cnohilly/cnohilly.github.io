package code;

import mysql.first.MySQLAccess;

public class StatTracker {

	private GUI _gui;
	private MySQLAccess _mySQLAccess;
	
	public StatTracker() {
		try {
			_mySQLAccess = new MySQLAccess();
		} catch (Exception e) {
		}
		_gui = new GUI(_mySQLAccess);
	}
}
